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
     * Enum to define the type/kind of the FFT
     */
    enum class FFT_Kind
    {
        Complex2Complex,
        Real2Complex,
        Complex2Real
    };

    namespace traits {

        /**
         * Trait to specify whether the input is complex
         */
        template< FFT_Kind T_Type >
        struct IsComplexInput: std::true_type{};

        template<>
        struct IsComplexInput< FFT_Kind::Real2Complex >: std::false_type{};

        /**
         * Trait to specify whether the output is complex
         */
        template< FFT_Kind T_Type >
        struct IsComplexOutput: std::true_type{};

        template<>
        struct IsComplexOutput< FFT_Kind::Complex2Real>: std::false_type{};

    }  // namespace traits

}  // namespace LiFFT
