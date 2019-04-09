/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/c++14_types.hpp"

namespace LiFFT {
namespace accessors {

    /**
     * Accesses a Volume (1D-3D) by using its ()-operator(x, y, z) where x is the fastest varying index
     */
    struct VolumeAccessor
    {
        template<
            class T_Index,
            class T_Data,
            typename T_SFINAE = std::enable_if_t<
                (traits::NumDims<std::decay_t<T_Data>>::value == 1)
            >*
        >
        auto
        operator()(const T_Index& idx, T_Data& data) const
        -> decltype(data(idx[0]))
        {
            return data(idx[0]);
        }

        template<
            class T_Index,
            class T_Data,
            typename T_SFINAE = std::enable_if_t<
                (traits::NumDims<std::decay_t<T_Data>>::value == 2)
            >*
        >
        auto
        operator()(const T_Index& idx, T_Data& data) const
        -> decltype(data(idx[1], idx[0]))
        {
            return data(idx[1], idx[0]);
        }

        template<
            class T_Index,
            class T_Data,
            typename T_SFINAE = std::enable_if_t<
                (traits::NumDims<std::decay_t<T_Data>>::value == 3)
            >*
        >
        auto
        operator()(const T_Index& idx, T_Data& data) const
        -> decltype(data(idx[2], idx[1], idx[0]))
        {
            return data(idx[2], idx[1], idx[0]);
        }
    };

}  // namespace accessors
}  // namespace LiFFT
