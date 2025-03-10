#ifndef GAPPA_COMMANDS_PREPARE_H_
#define GAPPA_COMMANDS_PREPARE_H_

/*
    gappa - Genesis Applications for Phylogenetic Placement Analysis
    Copyright (C) 2017-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

#include "CLI/CLI.hpp"

#include "commands/prepare/chunkify.hpp"
#include "commands/prepare/clean_tree.hpp"
#include "commands/prepare/phat.hpp"
#include "commands/prepare/taxonomy_tree.hpp"
#include "commands/prepare/unchunkify.hpp"

#include "options/global.hpp"
#include "tools/cli_setup.hpp"
#include "tools/misc.hpp"

#include <string>
#include <vector>

// =================================================================================================
//      Functions
// =================================================================================================

inline void setup_prepare( CLI::App& app )
{
    // Create the module subcommand objects.
    auto sub = app.add_subcommand(
        "prepare",
        "Commands for preparing and preprocessing of phylogenetic and placement data."
    );
    sub->require_subcommand( 1 );

    // Add module subcommands.
    setup_chunkify( *sub );
    setup_clean_tree( *sub );
    setup_phat( *sub );
    setup_taxonomy_tree( *sub );
    setup_unchunkify( *sub );

    // Add the global options to each of the above subcommands.
    // This has to be run here, so that these options are added to all above commands,
    // but not to the legacy commands that come next.
    global_options.add_to_module( *sub );
    set_module_help_group( *sub );

    // Add legacy commands.
    add_legacy_command( *sub, "extract", "edit extract" );
    add_legacy_command( *sub, "random-alignment" , "simulate random-alignment" );
    add_legacy_command( *sub, "random-placements", "simulate random-placements" );
    add_legacy_command( *sub, "random-tree",       "simulate random-tree" );
}

#endif // include guard
