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
         * Defines the (cuFFT)-Type of the FFT
         */
        template< typename T_Precision, bool T_IsComplexIn, bool T_IsComplexOut >
        struct FFTType: std::integral_constant< cufftType, CUFFT_C2C >{};

        template<>
        struct FFTType< float, false, true >: std::integral_constant< cufftType, CUFFT_R2C >{};

        template<>
        struct FFTType< float, true, false >: std::integral_constant< cufftType, CUFFT_C2R >{};

        template<>
        struct FFTType< double, true, true >: std::integral_constant< cufftType, CUFFT_Z2Z >{};

        template<>
        struct FFTType< double, false, true >: std::integral_constant< cufftType, CUFFT_D2Z >{};

        template<>
        struct FFTType< double, true, false >: std::integral_constant< cufftType, CUFFT_Z2D >{};

}  // namespace traits
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
