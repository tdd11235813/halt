/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace traits {

    /**
     * Removes all modifiers (const, volatile, reference, pointer, array extents) from the given type
     */
    template< typename T >
    struct RemoveModifer{
        using type = typename std::remove_pointer<
                         typename std::remove_reference<
                             typename std::remove_all_extents<
                                 typename std::remove_cv<T>::type
                             >::type
                         >::type
                     >::type;
    };
}  // namespace traits
}  // namespace LiFFT
