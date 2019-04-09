/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace libraries {
namespace cuFFT {
namespace traits{

    /**
     * Defines the FFTW internal types for the given precision type:
     * PlanType, ComplexType
     */
    template< typename T_Precision >
    struct Types;

    template<>
    struct Types<float>
    {
        using ComplexType = cufftComplex;
        using RealType = cufftReal;
    };

    template<>
    struct Types<double>
    {
        using ComplexType = cufftDoubleComplex;
        using RealType = cufftDoubleReal;
    };

}  // namespace traits
}  // namespace cuFFT
}  // namespace libraries

namespace traits {

    template< typename T >
    struct IsBinaryCompatible< types::Real<T>, typename libraries::cuFFT::traits::Types<T>::RealType >
        :std::true_type{};

    template< typename T >
    struct IsBinaryCompatible< types::Complex<T>, typename libraries::cuFFT::traits::Types<T>::ComplexType >
        :std::true_type{};

}  // namespace traits
}  // namespace LiFFT
