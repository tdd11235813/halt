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
     * Evaluates to true type if the given type is Complex, false for Real
     */
    template< typename T, typename T_SFINAE = void >
    struct IsComplex;

    /**
     * Specialization for pointers
     */
    template< typename T >
    struct IsComplex< T* >: IsComplex< T >{};

    template< typename T >
    struct IsComplex< T& >: IsComplex< T >{};

    template<>
    struct IsComplex< double >: std::false_type{};

    template<>
    struct IsComplex< float >: std::false_type{};

    template< typename T >
    struct IsComplex< T, void_t< decltype(T::isComplex) > >: std::integral_constant< bool, T::isComplex >{};

}  // namespace traits
}  // namespace LiFFT
