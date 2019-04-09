/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace tiffWriter{

    enum class ImageFormat
    {
        ARGB, // 32bit unsigned value per pixel -> 8bit per channel
        Float, // Monochrome, 32bit FP
        Double // Monochrome, 64bit FP
    };

}  // namespace tiffWriter
