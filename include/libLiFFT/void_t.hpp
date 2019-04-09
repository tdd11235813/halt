/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {

    /**
     * Helper for void_t (Workaround, see Paper from Walter Brown)
     */
    template <typename...>
    struct voider { using type = void; };

    /**
     * void_t for evaluating arguments, then returning void
     * Used for SFINAE evaluation of types
     */
    template <typename... Ts>
    using void_t = typename voider<Ts...>::type;

}  // namespace LiFFT
