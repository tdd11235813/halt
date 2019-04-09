/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <boost/utility.hpp>
#include "libLiFFT/traits/IsStrided.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/types/Vec.hpp"

namespace LiFFT {
namespace policies {

    namespace detail {

        /**
         * Default specialization for unstrided types: Use extents
         */
        template< typename T_Data, bool T_isStrided = traits::IsStrided< T_Data >::value >
        struct GetStrides: private boost::noncopyable
        {
            using Data = T_Data;
            static constexpr unsigned numDims = traits::NumDims< Data >::value;

            GetStrides(const Data& data)
            {
                GetExtents< T_Data > extents(data);
                static_assert(numDims >= 1, "No dimensions");
                m_strides[numDims - 1] = 1;
                for(unsigned i=numDims - 1; i>0; --i)
                    m_strides[i-1] = m_strides[i] * extents[i];
            }

            size_t operator[](unsigned dimIdx) const
            {
                return m_strides[dimIdx];
            }
        protected:
            types::Vec<numDims, size_t> m_strides;
        };

        /**
         * Default specialization for strided types: Use strides member
         */
        template< typename T_Data >
        struct GetStrides< T_Data, true >: private boost::noncopyable
        {
            using Data = T_Data;

            GetStrides(const Data& data): m_data(data){}

            size_t operator[](unsigned dimIdx) const
            {
                return m_data.strides[dimIdx];
            }
        protected:
            const Data& m_data;
        };

    }  // namespace detail

    /**
     * Provides a []-operator to get the strides in the specified dimension of the data object given in the constructor
     */
    template< typename T_Data >
    struct GetStrides: detail::GetStrides< T_Data >
    {
        using detail::GetStrides< T_Data >::GetStrides;
    };

}  // namespace policies
}  // namespace LiFFT
