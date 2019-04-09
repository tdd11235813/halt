/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <type_traits>
#include "libLiFFT/traits/RemoveModifiers.hpp"
#include "libLiFFT/void_t.hpp"
#include "libLiFFT/c++14_types.hpp"

namespace LiFFT {
namespace traits {

    // Forward declaration
    template< typename T >
    struct IntegralType;

    /**
     * Specialize this to return the integral type of a given memory representation
     */
    template< typename T, typename T_SFINAE = void >
    struct IntegralTypeImpl;

    template< typename T >
    struct IntegralTypeImpl<
            T,
            std::enable_if_t<
                (std::is_integral<T>::value || std::is_floating_point<T>::value)
            >
        >
    {
        using type = T;
    };

    /**
     * Specialization for structs with a type-member
     * Hint: More Specializations on differently named type aliases are possible with the void_t
     */
    template< typename T >
    struct IntegralTypeImpl< T, void_t< typename T::type > >: IntegralType< typename T::type >{};

    /**
     * Returns the integral type of a given memory representation
     */
    template< typename T >
    struct IntegralType{
        // Removes all modifiers and calls IntegralTypeImpl
        using type = typename IntegralTypeImpl<
                typename RemoveModifer<T>::type
              >::type;
    };

    template< typename T >
    using IntegralType_t = typename IntegralType<T>::type;

}  // namespace traits
}  // namespace LiFFT
