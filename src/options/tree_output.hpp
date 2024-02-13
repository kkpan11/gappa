#ifndef GAPPA_OPTIONS_TREE_OUTPUT_H_
#define GAPPA_OPTIONS_TREE_OUTPUT_H_

/*
    gappa - Genesis Applications for Phylogenetic Placement Analysis
    Copyright (C) 2017-2024 Lucas Czech

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

#include "CLI/CLI.hpp"

#include "options/tree_output_newick.hpp"
#include "options/tree_output_svg.hpp"
#include "options/file_output.hpp"

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/map.hpp"
#include "genesis/utils/color/normalization.hpp"

#include <string>
#include <vector>

// =================================================================================================
//      Tree Output Options
// =================================================================================================

/**
 * @brief This options class encapsulates all options needed to produce trees with colored branches
 * in different formats.
 *
 * Thus, whenenver a color tree is needed, this option can be used to add all the needed options
 * to a command at once. It adds different output formats and offers simple functions
 * to write to the ones specified by the user.
 *
 * It does not add the color options, as this is too dependent on what the command needs.
 * It just accepts a vector of colors for the branches of the tree.
 */
class TreeOutputOptions
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    TreeOutputOptions()  = default;
    ~TreeOutputOptions() = default;

    TreeOutputOptions( TreeOutputOptions const& other ) = default;
    TreeOutputOptions( TreeOutputOptions&& )            = default;

    TreeOutputOptions& operator= ( TreeOutputOptions const& other ) = default;
    TreeOutputOptions& operator= ( TreeOutputOptions&& )            = default;

    // -------------------------------------------------------------------------
    //     Setup Functions
    // -------------------------------------------------------------------------

    void add_tree_output_opts_to_app( CLI::App* sub );

    // -------------------------------------------------------------------------
    //     Run Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Print a warning if no output tree format is specified.
     */
    void check_tree_formats() const;

    /**
     * @brief Return the list of file extensions (without dot) of the tree formats that were
     * specified by the user.
     */
    std::vector<std::string> get_extensions() const;

    /**
     * @brief Write a simple tree with no colors to all formats specified by the user.
     */
    void write_tree_to_files(
        genesis::tree::CommonTree const&          tree,
        FileOutputOptions const&                  file_output_options,
        std::string const&                        infix
    ) const;

    /**
     * @brief Write a tree with colored branches to all formats specified by the user.
     */
    void write_tree_to_files(
        genesis::tree::CommonTree const&          tree,
        std::vector<genesis::utils::Color> const& color_per_branch,
        FileOutputOptions const&                  file_output_options,
        std::string const&                        infix
    ) const;

    /**
     * @brief Write a tree with colored branches to all formats specified by the user,
     * as well as information about the color scale (with a proper scale in the svg file).
     */
    void write_tree_to_files(
        genesis::tree::CommonTree const&          tree,
        std::vector<genesis::utils::Color> const& color_per_branch,
        genesis::utils::ColorMap const&           color_map,
        genesis::utils::ColorNormalization const& color_norm,
        FileOutputOptions const&                  file_output_options,
        std::string const&                        infix
    ) const;

    // -------------------------------------------------------------------------
    //     Option Members
    // -------------------------------------------------------------------------

private:

    bool write_newick_tree_   = false;
    bool write_nexus_tree_    = false;
    bool write_phyloxml_tree_ = false;
    bool write_svg_tree_      = false;

    NewickTreeOutputOptions newick_tree_output;
    SvgTreeOutputOptions svg_tree_output;

};

#endif // include guard
