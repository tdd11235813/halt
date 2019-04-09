/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <boost/utility.hpp>
#include <array>
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/c++14_types.hpp"

namespace LiFFT {
namespace policies {

    /**
     * Default implementation when an internal contiguous array has to be allocated
     */
    template< typename T_Data, bool T_copy = true >
    struct GetExtentsRawPtrImpl: private boost::noncopyable
    {
        using Data = T_Data;
        static constexpr unsigned numDims = traits::NumDims<T_Data>::value;

        GetExtentsRawPtrImpl(Data& data){
            GetExtents< Data > extents(data);
            for(unsigned i=0; i<numDims; ++i)
                m_extents[i] = extents[i];
        }

        const unsigned* operator()() const
        {
            return m_extents.data();
        }
    private:
        std::array< unsigned, numDims > m_extents;
    };

    /**
     * Partial specialization when we already have a contiguous array
     */
    template< typename T_Data >
    struct GetExtentsRawPtrImpl< T_Data, false >
    {
        using Data = T_Data;

        GetExtentsRawPtrImpl(Data& data): m_value(data.extents.data()){}

        const unsigned* operator()() const
        {
            return m_value;
        }
    private:
        unsigned* m_value;
    };

    /**
     * Functor that returns a raw ptr to an unsigned int array
     * containing 1 entry per dimension with the extents in that dimensions
     * If a custom numDims value is specified only the last n dimensions are considered
     */
    template< typename T_Data, class T_SFINAE = void >
    struct GetExtentsRawPtr: GetExtentsRawPtrImpl< T_Data, true >{
        using Data = T_Data;
        using Parent = GetExtentsRawPtrImpl< T_Data, true >;

        using Parent::Parent;
    };

    /**
     * Specialization when we have an extents member with a data() function returning a pointer
     */
    template< typename T_Data >
    struct GetExtentsRawPtr<
        T_Data,
        std::enable_if_t<
            std::is_pointer<
                decltype(
                    std::declval<T_Data>().extents.data()
                )
            >::value
        >
    >: GetExtentsRawPtrImpl< T_Data, false >{
        using Data = T_Data;
        using Parent = GetExtentsRawPtrImpl< T_Data, false >;

        using Parent::Parent;
    };

}  // namespace policies
}  // namespace LiFFT
