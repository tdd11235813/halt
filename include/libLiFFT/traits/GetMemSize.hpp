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
     * Returns the size of the allocated memory in bytes
     */
    template<class T>
    struct GetMemSize
    {
        size_t operator()(const T& data) const
        {
            return data.getMemSize();
        }
    };

    template<class T>
    struct GetMemSize<const T>: GetMemSize<T>{};

    template<class T>
    struct GetMemSize<T&>: GetMemSize<T>{};

    template<class T>
    size_t
    getMemSize(const T& data)
    {
        return GetMemSize<T>()(data);
    }

}  // namespace traits
}  // namespace LiFFT
