#ifndef GAPPA_TOOLS_VERSION_H_
#define GAPPA_TOOLS_VERSION_H_

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

#include <string>

// =================================================================================================
//      Gappa Version
// =================================================================================================

inline std::string gappa_version()
{
    // Do not change manually. Automatically updated by the tools/deploy/release.sh script.
    return "v0.9.0"; // #GAPPA_VERSION#
}

inline std::string gappa_header()
{
    return "\
                                              ....      ....  \n\
                                             '' '||.   .||'   \n\
                                                  ||  ||      \n\
                                                  '|.|'       \n\
     ...'   ....   ... ...  ... ...   ....        .|'|.       \n\
    |  ||  '' .||   ||'  ||  ||'  || '' .||      .|'  ||      \n\
     |''   .|' ||   ||    |  ||    | .|' ||     .|'|.  ||     \n\
    '....  '|..'|'. ||...'   ||...'  '|..'|.    '||'    ||:.  \n\
    '....'          ||       ||                               \n\
                   ''''     ''''   " + gappa_version() + " (c) 2017-2025\n\
                                   by Lucas Czech and Pierre Barbera\n";
}

inline std::string gappa_title()
{
    return "gappa - a toolkit for analyzing and visualizing phylogenetic (placement) data";
    // return "gappa - Genesis Applications for Phylogenetic Placement Analysis";
}

#endif // include guard
