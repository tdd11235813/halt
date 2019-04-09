/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <stdexcept>

namespace tiffWriter {

    struct FormatException : public std::runtime_error
    {
       using std::runtime_error::runtime_error;
    };

    struct InfoMissingException : public std::runtime_error
    {
        InfoMissingException(std::string s): std::runtime_error("Info missing: "+s){}
    };

    struct InfoWriteException : public std::runtime_error
    {
        InfoWriteException(std::string s): std::runtime_error("Could not write "+s){}
    };

}  // namespace tiffWriter
