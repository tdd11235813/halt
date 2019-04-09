/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/types/Vec.hpp"

namespace LiFFT {
namespace types {

    /** Return offset of a range on a given container (translates special values for ranges to actual values) */
    template< class T_Range, class T_Data, bool T_isOrigin = T_Range::isOrigin >
    struct GetRangeOffset
    {
        static constexpr unsigned numDims = traits::NumDims<T_Data>::value;

        static Vec<numDims>
        get(const T_Range&)
        {
            return Vec<numDims>::all(0u);
        }
    };

    template< class T_Range, class T_Data >
    struct GetRangeOffset< T_Range, T_Data, false >
    {
        static constexpr unsigned numDims = traits::NumDims<T_Data>::value;

        static typename T_Range::Offset
        get(const T_Range& range)
        {
            return range.m_offset;
        }
    };

    /** Return extent of a range on a given container (translates special values for ranges to actual values) */
    template< class T_Range, class T_Data, bool T_isFullSize = T_Range::isFullSize >
    struct GetRangeExtents
    {
        static constexpr unsigned numDims = traits::NumDims<T_Data>::value;

        static Vec<numDims>
        get(const T_Range& range, const T_Data& data)
        {
            policies::GetExtents<T_Data> dataExtents(data);
            Vec<numDims> offsets = GetRangeOffset<T_Range, T_Data>::get(range);
            Vec<numDims> extents;
            for(unsigned i=0; i<numDims; i++)
                extents[i] = dataExtents[i] - offsets[i];
            return extents;
        }
    };

    template< class T_Range, class T_Data >
    struct GetRangeExtents< T_Range, T_Data, false >
    {
        static constexpr unsigned numDims = traits::NumDims<T_Data>::value;

        static typename T_Range::Extents
        get(const T_Range& range, const T_Data& /*data*/ )
        {
            return range.m_extents;
        }
    };

    /**
     * Special class for specifying the origin (0, 0, ...)
     */
    class Origin{};
    /**
     * Full size or remaining size if offset different than origin is specified
     */
    class FullSize{};

    /**
     * Specifies a range in a data container
     */
    template< class T_Offset = Origin, class T_Extents = FullSize >
    struct Range
    {
        using Offset = T_Offset;
        using Extents = T_Extents;

        const Offset m_offset;
        const Extents m_extents;

        static constexpr bool isOrigin = std::is_same<Offset, Origin>::value;
        static constexpr bool isFullSize = std::is_same<Extents, FullSize>::value;

        Range(const T_Offset& offset = T_Offset(), const T_Extents& extents = T_Extents()): m_offset(offset), m_extents(extents){}
    };

    template< class T_Offset = Origin, class T_Extents = FullSize >
    Range< T_Offset, T_Extents >
    makeRange(const T_Offset& offset = T_Offset(), const T_Extents& extents = T_Extents())
    {
        return Range< T_Offset, T_Extents >(offset, extents);
    }

}  // namespace types
}  // namespace LiFFT
