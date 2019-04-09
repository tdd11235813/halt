/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/cuFFT/traits/LibTypes.hpp"
#include "libLiFFT/policies/GetNumElements.hpp"

namespace LiFFT {
namespace libraries {
namespace cuFFT {
namespace policies {

    /**
     * Getting memory size for inplace transforms is a bit tricky. Therefore use this trait
     */
    template< typename T_Precision, bool T_isComplexIn, bool T_isComplexOut, unsigned T_numDims >
    struct GetInplaceMemSize
    {
        using Precision = T_Precision;
        static constexpr bool isComplexIn = T_isComplexIn;
        static constexpr bool isComplexOut = T_isComplexOut;
        static constexpr unsigned numDims = T_numDims;

        using LibTypes = traits::LibTypes< Precision, isComplexIn, isComplexOut >;
        using LibInType = typename LibTypes::InType;
        using LibOutType = typename LibTypes::OutType;

        template< class T_Extents >
        static size_t
        get(const T_Extents& fullExtents)
        {
            // ATTENTION: Complex values take up more space then real values. Make sure we have enough!
            // Get the extents from the complex values (also for C2R/R2C)
            auto extents(fullExtents);
            if(!isComplexIn || !isComplexOut)
                extents[numDims - 1] = extents[numDims - 1] / 2 + 1;
            // Get number of complex elements
            unsigned numElements = LiFFT::policies::getNumElementsFromExtents(extents);
            return numElements * (isComplexIn ? sizeof(LibInType) : sizeof(LibOutType));
        }
    };

}  // namespace policies
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
