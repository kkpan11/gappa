#ifndef GAPPA_COMMANDS_PREPARE_UNCHUNKIFY_H_
#define GAPPA_COMMANDS_PREPARE_UNCHUNKIFY_H_

/*
    gappa - Genesis Applications for Phylogenetic Placement Analysis
    Copyright (C) 2017-2018 Lucas Czech and HITS gGmbH

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

#include "CLI/CLI.hpp"

#include "options/jplace_input.hpp"
#include "options/file_input.hpp"
#include "options/file_output.hpp"
#include "options/sequence_input.hpp"

#include <string>
#include <vector>

// =================================================================================================
//      Options
// =================================================================================================

class UnchunkifyOptions
{
public:

    std::string chunk_list_file;
    std::string chunk_file_expression;
    size_t jplace_cache_size = 0;
    std::string hash_function = "SHA1";

    JplaceInputOptions jplace_input;
    FileInputOptions   abundance_map_input;
    FileOutputOptions  file_output;
    SequenceInputOptions sequence_input;
};

// =================================================================================================
//      Functions
// =================================================================================================

void setup_unchunkify( CLI::App& app );
void run_unchunkify( UnchunkifyOptions const& options );

#endif // include guard
