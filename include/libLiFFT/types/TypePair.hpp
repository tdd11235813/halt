/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace types {

    template< typename T1, typename T2 >
    struct TypePair
    {
        using First = T1;
        using Second = T2;
    };

}  // namespace types
}  // namespace LiFFT
