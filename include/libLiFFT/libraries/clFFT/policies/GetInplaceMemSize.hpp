#pragma once

#include "libLiFFT/policies/GetNumElements.hpp"

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

    /**
     * Getting memory size for inplace transforms is a bit tricky. Therefore use this trait
     */
    template< typename T_Precision,
              bool T_isComplexIn,
              bool T_isComplexOut,
              unsigned T_numDims >
    struct GetInplaceMemSize
    {
        using Precision = T_Precision;
        static constexpr bool isComplexIn = T_isComplexIn;
        static constexpr bool isComplexOut = T_isComplexOut;
        static constexpr unsigned numDims = T_numDims;

        template< class T_Extents >
        static size_t
        get(const T_Extents& fullExtents)
        {
          auto extents(fullExtents);
          if(!isComplexIn || !isComplexOut)
          {
            extents[numDims - 1] = extents[numDims - 1] / 2 + 1;
          }
          // Get number of complex elements
          unsigned numElements = LiFFT::policies::getNumElementsFromExtents(extents);
          return numElements * 2 * sizeof(T_Precision);
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace LiFFT
