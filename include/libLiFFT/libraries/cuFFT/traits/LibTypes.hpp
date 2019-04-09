/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/cuFFT/traits/Types.hpp"
#include "libLiFFT/c++14_types.hpp"

namespace LiFFT {
namespace libraries {
namespace cuFFT {
namespace traits{

    /**
     * Defines the LibIn/OutType for a configuration
     */
    template<
            typename T_Precision,
            bool T_isComplexIn,
            bool T_isComplexOut
    >
    struct LibTypes
    {
        using Precision = T_Precision;
        static constexpr bool isComplexIn = T_isComplexIn;
        static constexpr bool isComplexOut = T_isComplexOut;

        using RealType = typename Types< Precision >::RealType;
        using ComplexType = typename Types< Precision >::ComplexType;
        using InType = std::conditional_t<
                              isComplexIn,
                              ComplexType,
                              RealType
                          >;
        using OutType = std::conditional_t<
                              isComplexOut,
                              ComplexType,
                              RealType
                          >;
    };

}  // namespace traits
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
