/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace accessors {

    /**
     * Accessor that access the data directly via the index
     *
     * \tparam T_isFunctor When false, the array is accessed via []-operator, else via ()-operator
     */
    template< bool T_isFunctor = false >
    struct ArrayAccessor
    {
    public:
        template< class T_Index, class T_Data >
        auto
        operator()(const T_Index& idx, T_Data& data) const
        -> decltype(data[idx])
        {
            return data[idx];
        }

        template< class T_Index, class T_Data, typename T_Value >
        void
        operator()(const T_Index& idx, T_Data& data, T_Value&& value) const
        {
            data[idx] = std::forward<T_Value>(value);
        }
    };

    template<>
    struct ArrayAccessor< true >
    {
    public:
        template< class T_Index, class T_Data >
        auto
        operator()(const T_Index& idx, T_Data& data) const
        -> decltype(data(idx))
        {
            return data(idx);
        }

        template< class T_Index, class T_Data, typename T_Value >
        void
        operator()(const T_Index& idx, T_Data& data, T_Value&& value) const
        {
            data(idx) = std::forward<T_Value>(value);
        }
    };

}  // namespace accessors
}  // namespace LiFFT
