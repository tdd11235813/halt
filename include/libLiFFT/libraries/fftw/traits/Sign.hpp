/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/fftw/fftw3Include.h"

namespace LiFFT {
namespace libraries {
namespace fftw {
namespace traits{

        /**
         * Defines the sign for the given FFT type (forward/backward)
         */
        template< bool T_isFwd=true >
        struct Sign: std::integral_constant<int, FFTW_FORWARD>{};

        template<>
        struct Sign<false>: std::integral_constant<int, FFTW_BACKWARD>{};

}  // namespace traits
}  // namespace fftw
}  // namespace libraries
}  // namespace LiFFT
