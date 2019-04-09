/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/fftw/fftw3Include.h"
#include "libLiFFT/libraries/fftw/traits/LibTypes.hpp"

namespace LiFFT {
namespace libraries {
namespace fftw {
namespace policies{

    /**
     * Executes a given plan
     */
    template< typename T_Precision >
    struct ExecutePlan;

    template<>
    struct ExecutePlan<float>
    {
        using PlanType = typename traits::LibTypes<float>::PlanType;

        void
        operator()(PlanType& plan)
        {
            fftwf_execute(plan);
        }
    };

    template<>
    struct ExecutePlan<double>
    {
        using PlanType = typename traits::LibTypes<double>::PlanType;

        void
        operator()(PlanType& plan)
        {
            fftw_execute(plan);
        }
    };

}  // namespace policies
}  // namespace fftw
}  // namespace libraries
}  // namespace LiFFT
