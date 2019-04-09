/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {

    /** Commonly used pattern to silence unused variable warnings */
    template <typename... T>
    void ignore_unused(const T& ...){}

}  // namespace LiFFT
