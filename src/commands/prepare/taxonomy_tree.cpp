/*
    gappa - Genesis Applications for Phylogenetic Placement Analysis
    Copyright (C) 2017-2022 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

#include "commands/prepare/taxonomy_tree.hpp"

#include "options/global.hpp"
#include "tools/cli_setup.hpp"

#include "CLI/CLI.hpp"

#include "genesis/taxonomy/formats/taxonomy_reader.hpp"
#include "genesis/taxonomy/formats/taxopath_parser.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/tree.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

#include "genesis/tree/tree.hpp"
#include "genesis/tree/common_tree/newick_writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/csv/input_iterator.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <cassert>
#include <cctype>
#include <string>
#include <stdexcept>
#include <vector>

// =================================================================================================
//      Setup
// =================================================================================================

void setup_taxonomy_tree( CLI::App& app )
{
    // Create the options and subcommand objects.
    auto opt = std::make_shared<TaxonomyTreeOptions>();
    auto sub = app.add_subcommand(
        "taxonomy-tree",
        "Turn a taxonomy into a tree that can be used as a constraint for tree inference."
    );

    // -----------------------------------------------------------
    //     Input Data
    // -----------------------------------------------------------

    // Taxon list file
    auto taxon_list_file_opt = sub->add_option(
        "--taxon-list-file",
        opt->taxon_list_file,
        "File that maps taxon names to taxonomic paths."
    );
    taxon_list_file_opt->check( CLI::ExistingFile );
    taxon_list_file_opt->group( "Input" );
    // taxon_list_file_opt->required();

    // Taxonomy file
    auto taxonomy_file_opt = sub->add_option(
        "--taxonomy-file",
        opt->taxonomy_file,
        "File that lists the taxa of the taxonomy as taxonomic paths."
    );
    taxonomy_file_opt->check( CLI::ExistingFile );
    taxonomy_file_opt->group( "Input" );
    // taxonomy_file_opt->required();

    // -----------------------------------------------------------
    //     Settings
    // -----------------------------------------------------------

    // keep singleton inner nodes
    sub->add_flag(
        "--keep-singleton-inner-nodes",
        opt->keep_singleton_inner_nodes,
        "Taxonomic paths can go down several levels without any furcation. "
        "Use this option to keep such paths, instead of collapsing them into a single level."
    )->group( "Settings" );

    // keep inner node names
    sub->add_flag(
        "--keep-inner-node-names",
        opt->keep_inner_node_names,
        "Taxonomies contain names at every level, while trees usually do not. "
        "Use this option to also set taxonomic names for the inner nodes of the tree."
    )->group( "Settings" );

    // Max level
    sub->add_option(
        "--max-level",
        opt->max_level,
        "Maximum taxonomic level to process (0-based). "
        "Taxa below this level are not added to the tree."
    )->group( "Settings" );

    // replace invalid chars
    sub->add_flag(
        "--replace-invalid-chars",
        opt->replace_invalid_chars,
        "Replace invalid characters in node labels (` ,:;\"()[]`) by underscores, which can "
        "occur if the input taxonomic paths contain such characters. "
        "The Newick format requires node labels to be wrapped in double quotation marks "
        "if they contain these characters, but many parsers cannot handle this. "
        "For such cases, replacing the characters can help."
    )->group( "Settings" );

    // -----------------------------------------------------------
    //     Output Options
    // -----------------------------------------------------------

    opt->file_output.add_default_output_opts_to_app( sub );

    // -----------------------------------------------------------
    //     Callback
    // -----------------------------------------------------------

    // Set the run function as callback to be called when this subcommand is issued.
    // Hand over the options by copy, so that their shared ptr stays alive in the lambda.
    sub->callback( gappa_cli_callback(
        sub,
        {},
        [ opt ]() {
            run_taxonomy_tree( *opt );
        }
    ));
}

// =================================================================================================
//      Run
// =================================================================================================

void run_taxonomy_tree( TaxonomyTreeOptions const& options )
{
    using namespace ::genesis;
    using namespace ::genesis::taxonomy;
    using namespace ::genesis::tree;
    using namespace ::genesis::utils;

    // Check that at least one of the options is set.
    if( options.taxonomy_file.empty() && options.taxon_list_file.empty() ) {
        throw CLI::ValidationError(
            "--taxon-list-file, --taxonomy-file",
            "At least one of the input options has to be used."
        );
    }

    // Check if the output file name already exists. If so, fail early.
    options.file_output.check_output_files_nonexistence( "taxonomy_tree", "newick" );

    // If taxonomy is given, read it.
    Taxonomy taxonomy;
    if( ! options.taxonomy_file.empty() ) {
        taxonomy = TaxonomyReader().read( from_file( options.taxonomy_file ) );
        // sort_by_name( taxonomy );
    }

    // If taxon list is given, read it.
    std::unordered_map<std::string, Taxopath> taxa_list;
    if( ! options.taxon_list_file.empty() ) {
        auto reader = CsvReader();
        reader.separator_chars( "\t" );
        auto csv_it = CsvInputIterator( from_file( options.taxon_list_file ), reader );
        while( csv_it ) {
            auto const& line = *csv_it;
            if( line.size() != 2 ) {
                throw CLI::ValidationError(
                    "--taxon-list-file (" + options.taxon_list_file + ")",
                    "Invalid line that does not have two fields."
                );
            }
            if( taxa_list.count( line[0] ) > 0 ) {
                throw CLI::ValidationError(
                    "--taxon-list-file (" + options.taxon_list_file + ")",
                    "Duplicate taxon name (" + line[0] + ")."
                );
            }

            assert( line.size() == 2 );
            auto const path = TaxopathParser().parse( line[1] );
            taxa_list[ line[0] ] = path;

            csv_it.increment();
        }
    }

    // Make the tree!
    auto const tree = taxonomy_to_tree(
        taxonomy,
        taxa_list,
        options.keep_singleton_inner_nodes,
        options.keep_inner_node_names,
        options.max_level
    );

    // Taxonomies often contain symbols that are not valid in newick.
    // We can handle them and they get wrapped in quotes in the newick output,
    // but maybe still better warn the user about this!
    // We do this check here, directly on the tree, so that we only warn about chars
    // that will actually be in the output file.
    auto is_valid_name_char = [&]( char c ){
        return   ::isprint(c)
            && ! ::isspace(c)
            && c != ':'
            && c != ';'
            && c != '('
            && c != ')'
            && c != '['
            && c != ']'
            && c != ','
            && c != '"'
        ;
    };
    if( options.replace_invalid_chars ) {
        size_t inval_cnt = 0;
        size_t total_cnt = 0;
        for( auto& node : tree.nodes() ) {
            auto& name = node.data<CommonNodeData>().name;
            bool valid_name = true;
            for( size_t i = 0; i < name.size(); ++i ) {
                if( ! is_valid_name_char( name[i] ) ) {
                    valid_name = false;
                    name[i] = '_';
                }
            }
            if( !valid_name ) {
                ++inval_cnt;
            }
            ++total_cnt;
        }
        LOG_MSG1 << "Replaced invalid characters in " << inval_cnt
                 << " of " << total_cnt << " node labels.";
    } else {
        bool warned_bad_chars = false;
        for( auto const& node : tree.nodes() ) {
            auto const& name = node.data<CommonNodeData>().name;

            // Check validity of name, and warn if needed.
            bool valid_name = true;
            for( size_t i = 0; i < name.size(); ++i ) {
                if( ! is_valid_name_char( name[i] ) ) {
                    valid_name = false;
                    break;
                }
            }
            if( ! valid_name && ! warned_bad_chars ) {
                warned_bad_chars = true;
                LOG_WARN << "Warning: Taxonomy contains characters that are not valid in Newick "
                << "files: ' ,:;\"()[]'. We can handle this, and they get wrapped in "
                << "quotation marks in the output, according to the Newick standard. "
                << "However, many downstream tools do not correctly interpret such names. "
                << "We hence recommend to remove them from the input taxonomy, or to use the "
                << "--replace-invalid-chars option to automatically replace them by underscores.";
            }
            if( ! valid_name ) {
                LOG_WARN << " - Invalid name: \"" << name << "\"";
            }
        }
    }

    // Create a newick tree from it.
    auto nw = CommonTreeNewickWriter();
    nw.enable_branch_lengths( false );
    nw.replace_name_spaces( false );
    nw.write( tree, options.file_output.get_output_target( "taxonomy_tree", "newick" ));
}
