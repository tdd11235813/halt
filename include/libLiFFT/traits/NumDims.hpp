/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/void_t.hpp"

namespace LiFFT {
namespace traits {

    /**
     * Returns the number of dimensions for the given array-like type
     */
    template< typename T, typename T_SFINAE = void >
    struct NumDims;

    template< typename T >
    struct NumDims< T, void_t< decltype(T::numDims) > >: std::integral_constant< unsigned, T::numDims >{};

    template< typename T >
    struct NumDims< T& >: NumDims<T>{};

}  // namespace traits
}  // namespace LiFFT
