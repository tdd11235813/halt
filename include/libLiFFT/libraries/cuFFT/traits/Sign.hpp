/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cufft.h>

namespace LiFFT {
namespace libraries {
namespace cuFFT {
namespace traits{

        /**
         * Defines the sign for the given FFT type (forward/backward)
         */
        template< bool T_isFwd=true >
        struct Sign: std::integral_constant<int, CUFFT_FORWARD>{};

        template<>
        struct Sign<false>: std::integral_constant<int, CUFFT_INVERSE>{};

}  // namespace traits
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
