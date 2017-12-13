/*
    gappa - Genesis Applications for Phylogenetic Placement Analysis
    Copyright (C) 2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#include "commands/squash.hpp"

#include "CLI/CLI.hpp"

#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/squash_clustering.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <fstream>

// =================================================================================================
//      Setup
// =================================================================================================

void setup_squash( CLI::App& app, GeneralOptions const& opt_general )
{
    // Create the options and subcommand objects.
    auto opt = std::make_shared<SquashOptions>();
    auto sub = app.add_subcommand(
        "squash",
        "performs squash clustering."
    );

    // Add common options.
    opt->add_jplace_input_options( sub );
    opt->add_output_dir_options( sub );

    // Fill in custom options.
    sub->add_option(
        "--point-mass", opt->point_mass,
        "Treat every pquery as a point mass concentrated on the highest-weight placement"
    );

    // TODO add option for selcting the distance measure: kr/emd or nhd

    // Set the run function as callback to be called when this subcommand is issued.
    // Hand over the options by copy, so that their shared ptr stays alive in the lambda.
    sub->set_callback( [ opt, &opt_general ]() {
        run_squash( *opt, opt_general );
    });
}

// =================================================================================================
//      Run
// =================================================================================================

void run_squash( SquashOptions const& options, GeneralOptions const& opt_general )
{
    using namespace genesis;

    // Check if any of the files we are going to produce already exists. If so, fail early.
    options.check_nonexistent_output_files({ "cluster.newick" });

    // Print some user output.
    options.print_jplace_input_options( opt_general.verbosity() );

    // Get the samples.
    auto sample_set = options.sample_set();

    if( options.point_mass ) {
        for( auto& sample : sample_set ) {
            placement::filter_n_max_weight_placements( sample.sample );
        }
    }

    auto mass_trees = convert_sample_set_to_mass_trees( sample_set );

    // LOG_INFO << "Starting squash clustering";
    auto sc = tree::squash_clustering( std::move( mass_trees.first ));
    // LOG_INFO << "Finished squash clustering";

    // LOG_INFO << "Writing cluster tree";
    std::ofstream file_clust_tree;
    utils::file_output_stream( options.out_dir() + "cluster.newick",  file_clust_tree );
    file_clust_tree << squash_cluster_tree( sc, options.jplace_base_file_names() );

    // LOG_INFO << "Writing fat trees";
    for( size_t i = 0; i < sc.clusters.size(); ++i ) {
        // auto const& cc = sc.clusters[i];

        // auto const cv = tree::mass_tree_mass_per_edge( cc.tree );
        // auto const colors = counts_to_colors(cv, false);

        // write_color_tree_to_nexus( avg_tree, colors, options.out_dir + "/tree_" + std::to_string(i) + ".nexus" );
        // write_color_tree_to_svg( avg_tree, colors, options.out_dir + "/tree_" + std::to_string(i) );
    }
}