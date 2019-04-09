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
     * Evaluates to true type if the given type is an In-Place Indicator type
     * That is: Input data ptr should be uses as Output data ptr
     * and data format matches input data format except the type (complex or real)
     */
    template< class T >
    struct IsInplace: std::false_type{};

}  // namespace traits
}  // namespace LiFFT
