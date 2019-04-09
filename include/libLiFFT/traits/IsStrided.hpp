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
     * Evaluates to true type if the given Array-like type is strided
     */
    template< typename T, typename T_SFINAE = void >
    struct IsStrided;

    template< typename T >
    struct IsStrided< T, void_t< decltype(T::isStrided) > >: std::integral_constant< bool, T::isStrided >{};

}  // namespace traits
}  // namespace LiFFT
