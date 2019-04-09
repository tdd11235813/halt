/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <type_traits>

namespace LiFFT {

    /**
     * Type used to indicate that a given value should be automatically detected
     */
    struct AutoDetect: std::integral_constant<unsigned, 0>{};

}  // namespace LiFFT
