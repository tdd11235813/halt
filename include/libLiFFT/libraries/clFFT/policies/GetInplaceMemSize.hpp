#pragma once

#include "libLiFFT/policies/GetNumElements.hpp"

namespace LiFFT {
namespace libraries {
namespace clFFT {
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

        template< class T_Input >
        static size_t
        get(const T_Input& input)
        {
          //@todo
            // ATTENTION: Complex values take up more space than real values. Make sure we have enough!
            // Get the extents from the complex values (also for C2R/R2C)
            //if(!isComplexIn || !isComplexOut)
            //  extents[numDims - 1] = extents[numDims - 1] / 2 + 1;
            // Get number of complex elements
            //unsigned numElements = LiFFT::policies::getNumElementsFromExtents(extents);
            return input.getMemSize(); //numElements * (isComplexIn ? sizeof(LibInType) : sizeof(LibOutType));
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
