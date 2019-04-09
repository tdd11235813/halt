/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/types/Real.hpp"
#include "libLiFFT/mem/AoSValues.hpp"

namespace LiFFT {
namespace mem {

    template< typename T, bool T_ownsPointer = true >
    class RealValues: public detail::AoSValues< types::Real<T>, T_ownsPointer >
    {
    public:
        using Parent = detail::AoSValues< types::Real<T>, T_ownsPointer >;

        using Parent::Parent;
    };

}  // namespace mem
}  // namespace LiFFT
