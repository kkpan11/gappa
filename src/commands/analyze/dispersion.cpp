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

#include "commands/analyze/dispersion.hpp"

#include "options/global.hpp"
#include "tools/cli_setup.hpp"

#include "CLI/CLI.hpp"

#include "genesis/placement/function/epca.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/masses.hpp"
#include "genesis/placement/function/sample_set.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/math/matrix.hpp"

#include <cassert>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

// =================================================================================================
//      Internal Helper Classes
// =================================================================================================

/**
 * @brief Helper struct that stores one of the variants of the dispersion method and its properties.
 *
 * In the run function, we create a list of these, according to which options the user specified.
 * This list is then iterated to produce the resulting coloured trees for each variant.
 */
struct DispersionVariant
{
    enum EdgeValues
    {
        kMasses,
        kImbalances
    };

    enum DispersionMethod
    {
        kStandardDeviation,
        kVariance,
        kCoeffcientOfVariation,
        kIndexOfDispersion
    };

    DispersionVariant( std::string const& n, EdgeValues m, DispersionMethod d, bool l )
        : name(n)
        , edge_values(m)
        , dispersion_method(d)
        , log_scaling(l)
    {}

    std::string      name;
    EdgeValues       edge_values;
    DispersionMethod dispersion_method;
    bool             log_scaling = false;
};

// =================================================================================================
//      Setup
// =================================================================================================

void setup_dispersion( CLI::App& app )
{
    // Create the options and subcommand objects.
    auto options = std::make_shared<DispersionOptions>();
    auto sub = app.add_subcommand(
        "dispersion",
        "Calculate the Edge Dispersion between samples."
    );

    // Input.
    options->jplace_input.add_jplace_input_opt_to_app( sub );
    options->jplace_input.add_mass_norm_opt_to_app( sub, true );
    options->jplace_input.add_point_mass_opt_to_app( sub );
    options->jplace_input.add_ignore_multiplicities_opt_to_app( sub );

    // Edge value representation
    sub->add_option(
        "--edge-values",
        options->edge_values,
        "Values per edge used to calculate the dispersion. "
        "Using `masses` focuses on per-branch dispersion, while using `imbalances` focuses on "
        "per-clade dispersion; see the paper for details.",
        true
    )->group( "Settings" )
    ->transform(
        CLI::IsMember({ "both", "imbalances", "masses" }, CLI::ignore_case )
    );

    // Dispersion method
    sub->add_option(
        "--method",
        options->method,
        "Method of dispersion. Either `all` (as far as they are applicable), or any of: "
        "coefficient of variation (`cv`, standard deviation divided by mean), "
        "coefficient of variation log-scaled (`cv-log`), "
        "standard deviation (`sd`), standard deviation log-scaled (`sd-log`)"
        "variance (`var`), variance log-scaled (`var-log`), "
        "variance to mean ratio (`vmr`, also called Index of Dispersion), "
        "variance to mean ratio log-scaled (`vmr-log`). "
        "It typically is useful to use `all`, in order to spot all patterns "
        "that can emerge from this method.",
        true
    )->group( "Settings" )
    ->transform(
        CLI::IsMember(
            { "all", "cv", "cv-log", "sd", "sd-log", "var", "var-log", "vmr", "vmr-log" },
            CLI::ignore_case
        )
    );

    // Color.
    options->color_map.add_color_list_opt_to_app( sub, "viridis" );
    options->color_map.add_mask_opt_to_app( sub, "#dfdfdf" );

    // Output files.
    // options->file_output.set_optionname( "tree" );
    options->file_output.add_default_output_opts_to_app( sub );
    // options->file_output.add_default_output_opts_to_app( sub, ".", "dispersion_" );
    options->tree_output.add_tree_output_opts_to_app( sub );

    // Set the run function as callback to be called when this subcommand is issued.
    // Hand over the options by copy, so that their shared ptr stays alive in the lambda.
    sub->callback( gappa_cli_callback(
        sub,
        {
            "Czech2019-analyzing-and-visualizing-samples"
        },
        [ options ]() {
            run_dispersion( *options );
        }
    ));
}

// =================================================================================================
//      Helper Functions
// =================================================================================================

/**
 * @brief Activate variants according to options being set.
 */
std::vector<DispersionVariant> get_variants( DispersionOptions const& options )
{
    std::vector<DispersionVariant> variants;

    // Masses can use all methods.
    if(( options.edge_values == "both" ) || ( options.edge_values == "masses" )) {

        // Linear
        if(( options.method == "all" ) || ( options.method == "sd" )) {
            variants.push_back({ "masses_sd", DispersionVariant::kMasses, DispersionVariant::kStandardDeviation, false });
        }
        if(( options.method == "all" ) || ( options.method == "var" )) {
            variants.push_back({ "masses_var", DispersionVariant::kMasses, DispersionVariant::kVariance, false });
        }
        if(( options.method == "all" ) || ( options.method == "cv" )) {
            variants.push_back({ "masses_cv", DispersionVariant::kMasses, DispersionVariant::kCoeffcientOfVariation, false });
        }
        if(( options.method == "all" ) || ( options.method == "vmr" )) {
            variants.push_back({ "masses_vmr", DispersionVariant::kMasses, DispersionVariant::kIndexOfDispersion, false });
        }

        // Log scaled
        if(( options.method == "all" ) || ( options.method == "sd-log" )) {
            variants.push_back({ "masses_sd_log", DispersionVariant::kMasses, DispersionVariant::kStandardDeviation, true });
        }
        if(( options.method == "all" ) || ( options.method == "var-log" )) {
            variants.push_back({ "masses_var_log", DispersionVariant::kMasses, DispersionVariant::kVariance, true });
        }
        if(( options.method == "all" ) || ( options.method == "cv-log" )) {
            variants.push_back({ "masses_cv_log", DispersionVariant::kMasses, DispersionVariant::kCoeffcientOfVariation, true });
        }
        if(( options.method == "all" ) || ( options.method == "vmr-log" )) {
            variants.push_back({ "masses_vmr_log", DispersionVariant::kMasses, DispersionVariant::kIndexOfDispersion, true });
        }
    }

    // For imbalances, only sd and variance make sense.
    if(( options.edge_values == "both" ) || ( options.edge_values == "imbalances" )) {

        // Linear
        if(( options.method == "all" ) || ( options.method == "sd" )) {
            variants.push_back({ "imbalances_sd", DispersionVariant::kImbalances, DispersionVariant::kStandardDeviation, false });
        }
        if(( options.method == "all" ) || ( options.method == "var" )) {
            variants.push_back({ "imbalances_var", DispersionVariant::kImbalances, DispersionVariant::kVariance, false });
        }

        // Log scaled
        if(( options.method == "all" ) || ( options.method == "sd-log" )) {
            variants.push_back({ "imbalances_sd_log", DispersionVariant::kImbalances, DispersionVariant::kStandardDeviation, true });
        }
        if(( options.method == "all" ) || ( options.method == "var-log" )) {
            variants.push_back({ "imbalances_var_log", DispersionVariant::kImbalances, DispersionVariant::kVariance, true });
        }
    }

    return variants;
}

// =================================================================================================
//      Make Color Tree
// =================================================================================================

void make_dispersion_color_tree(
    DispersionOptions const&   options,
    std::vector<double> const& values,
    bool                       log_scaling,
    genesis::tree::Tree const& tree,
    std::string const&         infix
) {
    using namespace genesis::utils;

    // Just in case...
    if( values.size() != tree.edge_count() ) {
        throw std::runtime_error( "Internal error: Trees and matrices do not fit to each other." );
    }

    // Get color norm and map.
    auto color_map  = options.color_map.color_map();
    std::unique_ptr<ColorNormalizationLinear> color_norm;
    if( log_scaling ) {
        color_norm = make_unique<ColorNormalizationLogarithmic>();
    } else {
        color_norm = make_unique<ColorNormalizationLinear>();
    }

    // Scale correctly. This checks for invalid values as well.
    color_norm->autoscale_max( values );

    // Set log scale minimum. Log scaling cannot use 0. Show some orders of magnitude instead.
    if( log_scaling ) {
        if( color_norm->max_value() > 1.0 ) {
            color_norm->min_value( 1.0 );
        } else {
            color_norm->min_value( color_norm->max_value() / 10e4 );
        }
        color_map.clip_under( true );
    }

    // Now, make a color vector and write to files.
    auto const colors = color_map( *color_norm, values );
    options.tree_output.write_tree_to_files(
        tree,
        colors,
        color_map,
        *color_norm,
        options.file_output,
        infix
    );
}

// =================================================================================================
//      Run with Matrix
// =================================================================================================

/**
 * @brief Run with either the masses or the imbalances matrix.
 */
void run_with_matrix(
    DispersionOptions const&              options,
    std::vector<DispersionVariant> const& variants,
    genesis::utils::Matrix<double> const& values,
    DispersionVariant::EdgeValues         edge_values,
    genesis::tree::Tree const&            tree
) {
    using namespace genesis::utils;

    if( values.cols() != tree.edge_count() ) {
        throw std::runtime_error( "Internal Error: Edge values does not have corrent length." );
    }

    auto matrix_col_mean_stddev = []( Matrix<double> const& data ){
        auto ret = std::vector<MeanStddevPair>( data.cols(), { 0.0, 0.0 } );

        // Nothing to do. Better stop here or we risk dividing by zero.
        if( data.rows() == 0 ) {
            return ret;
        }

        // Iterate columns.
        #pragma omp parallel for
        for( size_t c = 0; c < data.cols(); ++c ) {
            ret[c] = mean_stddev( data.col(c).begin(), data.col(c).end() );
        }

        return ret;
    };

    // Calculate things. We calculate everyting, which might be a bit wasteful if the "all" option
    // is not used. But these are really cheap calculations, and in the standard "all" case,
    // we need all of them twice (linear and log scaling).
    auto const mean_stddev = matrix_col_mean_stddev( values );
    auto sd_vec  = std::vector<double>( mean_stddev.size(), 0.0 );
    auto var_vec = std::vector<double>( mean_stddev.size(), 0.0 );
    auto cv_vec  = std::vector<double>( mean_stddev.size(), 0.0 );
    auto vmr_vec = std::vector<double>( mean_stddev.size(), 0.0 );
    for( size_t i = 0; i < mean_stddev.size(); ++i ) {
        sd_vec[ i ]  = mean_stddev[ i ].stddev;
        var_vec[ i ] = mean_stddev[ i ].stddev * mean_stddev[ i ].stddev;
        cv_vec[ i ]  = mean_stddev[ i ].stddev / mean_stddev[ i ].mean;
        vmr_vec[ i ] = mean_stddev[ i ].stddev * mean_stddev[ i ].stddev / mean_stddev[ i ].mean;
    }

    // Loop over all variants that have been set.
    for( auto const& variant : variants ) {

        // Only process the variants that have the current input metrix.
        // This is ugly, I know. But the distinction has to be made somewhere...
        if( variant.edge_values != edge_values ) {
            continue;
        }

        // Get the data vector that we want to use for this variant.
        std::vector<double> const* vec;
        switch( variant.dispersion_method ) {
            case DispersionVariant::kStandardDeviation: {
                vec = &sd_vec;
                break;
            }
            case DispersionVariant::kVariance: {
                vec = &var_vec;
                break;
            }
            case DispersionVariant::kCoeffcientOfVariation: {
                vec = &cv_vec;
                break;
            }
            case DispersionVariant::kIndexOfDispersion: {
                vec = &vmr_vec;
                break;
            }
            default: {
                throw std::runtime_error( "Internal Error: Invalid dispersion variant." );
            }
        }
        assert( vec );

        // Make a tree using the data vector and name of the variant.
        make_dispersion_color_tree( options, *vec, variant.log_scaling, tree, variant.name );
    }
}

// =================================================================================================
//      Run
// =================================================================================================

void run_dispersion( DispersionOptions const& options )
{
    using namespace genesis;
    using namespace genesis::placement;
    using namespace genesis::tree;
    using namespace genesis::utils;

    // -------------------------------------------------------------------------
    //     Checks and Preparation
    // -------------------------------------------------------------------------

    // User output.
    options.tree_output.check_tree_formats();
    options.jplace_input.print();

    // Get which variants of the method to run.
    auto const variants = get_variants( options );

    // Check for existing output files.
    std::vector<std::pair<std::string, std::string>> files_to_check;
    for( auto const& m : variants ) {
        for( auto const& e : options.tree_output.get_extensions() ) {
            files_to_check.emplace_back( m.name, e );
        }
    }
    options.file_output.check_output_files_nonexistence( files_to_check );

    // -------------------------------------------------------------------------
    //     Calculations and Output
    // -------------------------------------------------------------------------

    // Get the data. Read all samples and calcualte the matrices.
    auto const profile = options.jplace_input.placement_profile();
    assert( profile.edge_masses.rows() == profile.edge_imbalances.rows() );
    assert( profile.edge_masses.cols() == profile.edge_imbalances.cols() );
    if( profile.edge_masses.rows() <= 1 ) {
        throw std::runtime_error(
            "Cannot compute edge dispersion of a single sample, as the method is meant to visualize "
            "dispersion (variance) across a set of samples."
        );
    }

    LOG_MSG2 << "Calculating dispersions and writing files.";

    // Calculate things as needed.
    if(( options.edge_values == "both" ) || ( options.edge_values == "masses" )) {
        run_with_matrix(
            options, variants, profile.edge_masses, DispersionVariant::kMasses, profile.tree
        );
    }
    if(( options.edge_values == "both" ) || ( options.edge_values == "imbalances" )) {
        run_with_matrix(
            options, variants, profile.edge_imbalances, DispersionVariant::kImbalances, profile.tree
        );
    }
}
