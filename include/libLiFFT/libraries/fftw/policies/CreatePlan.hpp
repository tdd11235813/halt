/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/fftw/fftw3Include.h"
#include "libLiFFT/libraries/fftw/traits/LibTypes.hpp"
#include "libLiFFT/ignore_unused.hpp"

namespace LiFFT {
namespace libraries {
namespace fftw {
namespace policies{

    /**
     * Creates a plan for the given precision type
     */
    template< typename T_Precision >
    struct CreatePlan;

    template<>
    struct CreatePlan<float>
    {
        using PlanType = typename traits::LibTypes<float>::PlanType;
        using ComplexType = typename traits::LibTypes<float>::ComplexType;
        using RealType = float;

        static_assert(!std::is_same<RealType, ComplexType>::value, "Need different types for Real/Complex");

        PlanType
        Create(int rank, const int* n, ComplexType* in, ComplexType* out, int sign, unsigned flags)
        {
            return fftwf_plan_dft(rank, n, in, out, sign, flags);
        }

        PlanType
        Create(int rank, const int* n, RealType* in, ComplexType* out, int sign, unsigned flags)
        {
            assert(sign == FFTW_FORWARD);
            ignore_unused(sign);
            return fftwf_plan_dft_r2c(rank, n, in, out, flags);
        }

        PlanType
        Create(int rank, const int* n, ComplexType* in, RealType* out, int sign, unsigned flags)
        {
            assert(sign == FFTW_BACKWARD);
            ignore_unused(sign);
            return fftwf_plan_dft_c2r(rank, n, in, out, flags);
        }
    };

    template<>
    struct CreatePlan<double>
    {
        using PlanType = typename traits::LibTypes<double>::PlanType;
        using ComplexType = typename traits::LibTypes<double>::ComplexType;
        using RealType = double;

        static_assert(!std::is_same<RealType, ComplexType>::value, "Need different types for Real/Complex");

        PlanType
        Create(int rank, const int* n, ComplexType* in, ComplexType* out, int sign, unsigned flags)
        {
            return fftw_plan_dft(rank, n, in, out, sign, flags);
        }

        PlanType
        Create(int rank, const int* n, RealType* in, ComplexType* out, int sign, unsigned flags)
        {
            assert(sign == FFTW_FORWARD);
            ignore_unused(sign);
            return fftw_plan_dft_r2c(rank, n, in, out, flags);
        }

        PlanType
        Create(int rank, const int* n, ComplexType* in, RealType* out, int sign, unsigned flags)
        {
            assert(sign == FFTW_BACKWARD);
            ignore_unused(sign);
            return fftw_plan_dft_c2r(rank, n, in, out, flags);
        }
    };

}  // namespace policies
}  // namespace fftw
}  // namespace libraries
}  // namespace LiFFT
