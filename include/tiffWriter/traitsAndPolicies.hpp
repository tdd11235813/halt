/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "tiffWriter/image.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/traits/IdentityAccessor.hpp"
#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/IsStrided.hpp"
#include "libLiFFT/accessors/VolumeAccessor.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/types/Vec.hpp"
#include "libLiFFT/util.hpp"

namespace LiFFT {
namespace traits {

    template< tiffWriter::ImageFormat T_imgFormat, class T_Allocator >
    struct NumDims< tiffWriter::Image< T_imgFormat, T_Allocator > >: std::integral_constant<unsigned, 2>{};

    template< tiffWriter::ImageFormat T_imgFormat, class T_Allocator >
    struct IsComplex< tiffWriter::Image< T_imgFormat, T_Allocator > >: BoolConst<false>{};

    template< tiffWriter::ImageFormat T_imgFormat, class T_Allocator >
    struct IsStrided< tiffWriter::Image< T_imgFormat, T_Allocator > >: BoolConst<false>{};

    template< tiffWriter::ImageFormat T_imgFormat, class T_Allocator >
    struct IdentityAccessor< tiffWriter::Image< T_imgFormat, T_Allocator > >
    {
        using type = accessors::VolumeAccessor;
    };

}  // namespace traits

namespace policies {

    template< tiffWriter::ImageFormat T_imgFormat, class T_Allocator >
    struct GetExtentsImpl< tiffWriter::Image< T_imgFormat, T_Allocator > >
    {
        using type = tiffWriter::Image< T_imgFormat, T_Allocator >;

        GetExtentsImpl(const type& data): m_extents(data.getHeight(), data.getWidth()){}

        unsigned
        operator[](unsigned dim) const
        {
            return m_extents[dim];
        }
    private:
        const types::Vec<2> m_extents;
    };

}  // namespace policies
}  // namespace LiFFT
