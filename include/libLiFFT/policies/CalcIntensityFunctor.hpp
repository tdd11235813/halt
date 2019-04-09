/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsComplex.hpp"

namespace LiFFT {
namespace policies {

    struct CalcIntensityFunc
    {
        template< typename T, typename = std::enable_if_t< LiFFT::traits::IsComplex<T>::value > >
        auto
        operator()(const T& val) const
        -> decltype(val.real*val.real + val.imag*val.imag)
        {
            return val.real*val.real + val.imag*val.imag;
        }

        template< typename T, typename = std::enable_if_t< LiFFT::traits::IsComplex<T>::value > >
        auto
        operator()(const T& val) const
        -> decltype(val[0]*val[0] + val[1]*val[1])
        {
            return val[0]*val[0] + val[1]*val[1];
        }

        template< typename T, typename = std::enable_if_t< !LiFFT::traits::IsComplex<T>::value > >
        auto
        operator()(const T& val) const
        -> decltype(val*val)
        {
            return val*val;
        }
    };

}  // namespace policies
}  // namespace LiFFT
