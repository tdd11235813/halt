/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsBinaryCompatible.hpp"
#include "libLiFFT/types/Complex.hpp"

namespace LiFFT {
namespace libraries {
namespace fftw {
namespace traits{

    /**
     * Defines the FFTW internal types for the given precision type:
     * PlanType, ComplexType
     */
    template< typename T_Precision >
    struct LibTypes;

    template<>
    struct LibTypes<float>
    {
        using PlanType = fftwf_plan;
        using ComplexType = fftwf_complex;
    };

    template<>
    struct LibTypes<double>
    {
        using PlanType = fftw_plan;
        using ComplexType = fftw_complex;
    };

}  // namespace traits
}  // namespace fftw
}  // namespace libraries

namespace policies {

    template< typename T >
    struct SafePtrCast_Impl< typename libraries::fftw::traits::LibTypes<T>::ComplexType*, T* >
        :Ptr2Ptr< typename libraries::fftw::traits::LibTypes<T>::ComplexType*, T* >{};

}  // namespace policies

namespace traits {

    template< typename T >
    struct IsBinaryCompatible< types::Complex<T>, typename libraries::fftw::traits::LibTypes<T>::ComplexType >
        :std::true_type{};

}  // namespace traits
}  // namespace LiFFT
