/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <libLiFFT/policies/flattenIdx.hpp>
#include "libLiFFT/c++14_types.hpp"
#include "libLiFFT/util.hpp"

namespace LiFFT {
namespace accessors {

    /**
     * Accessor that can read and write a DataContainer (type with an array-like member named data)
     *
     * \tparam T_isFlatMemory if true, the idx is flattened on access
     */
    template< bool T_isFlatMemory = true >
    struct DataContainerAccessor
    {
    public:
        template< class T_Index, class T_Data >
        auto
        operator()(T_Index&& idx, T_Data& data) const
        -> decltype(
                std::declval< typename T_Data::BaseAccessor >()(
                        0, getConstCorrect<T_Data>(data.m_data)
                )
           )
        {
            auto flatIdx = policies::flattenIdx(idx, data);
            typename T_Data::BaseAccessor acc;
            return acc(flatIdx, getConstCorrect<T_Data>(data.m_data));
        }

        template< class T_Index, class T_Data, typename T_Value >
        void
        operator()(T_Index&& idx, T_Data& data, T_Value&& value) const
        {
            auto flatIdx = policies::flattenIdx(idx, data);
            typename T_Data::BaseAccessor acc;
            acc(flatIdx, getConstCorrect<T_Data>(data.m_data), std::forward<T_Value>(value));
        }
    };

    template<>
    struct DataContainerAccessor<false>
    {
    public:
        template< class T_Index, class T_Data >
        auto
        operator()(T_Index&& idx, T_Data& data) const
        -> decltype(
                std::declval< typename T_Data::BaseAccessor >()(
                        idx, getConstCorrect<T_Data>(data.m_data)
                )
           )
        {
            typename T_Data::BaseAccessor acc;
            return acc(idx, getConstCorrect<T_Data>(data.m_data));
        }

        template< class T_Index, class T_Data, typename T_Value >
        void
        operator()(T_Index&& idx, T_Data& data, T_Value&& value) const
        {
            typename T_Data::BaseAccessor acc;
            acc(idx, getConstCorrect<T_Data>(data.m_data), std::forward<T_Value>(value));
        }
    };

}  // namespace accessors
}  // namespace LiFFT
