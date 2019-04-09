/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace traits{

        /**
         * Returns whether the given data container is already on the device or not
         * A true_type implies that the underlying memory is on the device and therefore a reference is a device pointer
         */
        template< typename T >
        struct IsDeviceMemory: std::false_type{};

}  // namespace traits
}  // namespace LiFFT
