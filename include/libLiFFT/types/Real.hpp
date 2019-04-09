/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsBinaryCompatible.hpp"

namespace LiFFT {
namespace types {

    template< typename T >
    struct Real
    {
        using type = T;
        static constexpr bool isComplex = false;
        T value;

        Real(){}
        Real(T val):value(val){}
        operator T() const {return value;}
    };

}  // namespace types

namespace traits {

    template< typename T >
    struct IsBinaryCompatibleImpl< types::Real<T>, T >: std::true_type{};

}  // namespace traits
}  // namespace LiFFT
