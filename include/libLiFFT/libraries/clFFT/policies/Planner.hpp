#pragma once

#include <clFFT.h>
#include "libLiFFT/types/TypePair.hpp"
#include "libLiFFT/libraries/clFFT/Plan.hpp"
#include "libLiFFT/libraries/clFFT/traits/FFTType.hpp"
#include "libLiFFT/libraries/clFFT/traits/Sign.hpp"
#include "libLiFFT/libraries/clFFT/traits/Types.hpp"
#include "libLiFFT/libraries/clFFT/policies/GetInplaceMemSize.hpp"
#include <cassert>
#include <limits>

namespace LiFFT {
namespace libraries {
namespace clFFT {

namespace policies {

    /**
     * Functor that creates a plan.
     * Takes references to the plan, input, output and their allocators
     */
    template<
            typename T_Context,
            typename T_Precision,
            class T_InOutTypes,
            bool T_isInplace,
            unsigned T_numDims,
            bool T_isComplexIn,
            bool T_isComplexOut,
            bool T_isAoSIn,
            bool T_isAoSOut,
            bool T_isStridedIn,
            bool T_isStridedOut
    >
    struct Planner;

    /**
     * Both AoS, Both non-strided
     */
    template<
            typename T_Context,
            typename T_Precision,
            class T_InOutTypes,
            bool T_isInplace,
            unsigned T_numDims,
            bool T_isComplexIn,
            bool T_isComplexOut
    >
    struct Planner< T_Context, T_Precision, T_InOutTypes, T_isInplace, T_numDims, T_isComplexIn, T_isComplexOut, true, true, false, false >
    {
    private:
        using Precision = T_Precision;
        static constexpr unsigned numDims = T_numDims;
        using Input  = typename T_InOutTypes::First;
        using Output = typename T_InOutTypes::Second;
        static constexpr bool isInplace    = T_isInplace;
        static constexpr bool isComplexIn  = T_isComplexIn;
        static constexpr bool isComplexOut = T_isComplexOut;

        static_assert(isComplexIn || isComplexOut, "Real2Real conversion not supported");
        //static_assert(T_isComplexIn || T_isFwd, "Real2Complex is always a forward transform");
        //static_assert(T_isComplexOut || !T_isFwd, "Complex2Real is always a backward transform");
        static_assert(numDims > 0 && numDims <= 3, "Only 1D-3D is supported");


    private:
        template< class T_Plan, class T_Extents >
        void
        createPlan(T_Plan& plan, T_Extents& extents)
        {
          assert(plan.handle==0);
          size_t cldims[numDims];
          cldims[0] = extents[0];
          if(numDims>1) cldims[1] = extents[1];
          if(numDims>2) cldims[2] = extents[2];
          clSafeCall( clfftCreateDefaultPlan(&plan.handle, plan.ctx, traits::Dims< T_numDims >::value, cldims) );
          clSafeCall( clfftSetPlanPrecision(plan.handle, traits::FFTPrecision< T_Precision >::value) );
          clSafeCall( clfftSetLayout(plan.handle, traits::FFTLayout<T_isComplexIn,T_isComplexOut>::value, traits::FFTLayout<T_isComplexOut,T_isComplexIn>::value) );
          clSafeCall( clfftSetResultLocation(plan.handle, traits::FFTInplace<T_isInplace>::value) );
        }

        void checkSize(size_t size)
        {
            if(size > std::numeric_limits<unsigned>::max())
                throw std::runtime_error("Size is too big (clFFT limitation) :" + std::to_string(size));
        }
    public:

        template< class T_Plan, class T_Allocator >
        void
        operator()(T_Plan& plan, Input& input, Output& output, bool useInplaceForHost, const T_Allocator& alloc)
        {
            static_assert(!isInplace, "Cannot be used for inplace transforms!");
            auto extents(input.getExtents());
            auto extentsOut(output.getExtents());
            for(unsigned i=0; i<numDims; ++i){
                unsigned eIn = extents[i];
                unsigned eOut = extentsOut[i];
                // Same extents in all dimensions unless we have a C2R or R2C and compare the last dimension
                bool dimOk = (eIn == eOut || (i+1 == numDims && !(isComplexIn && isComplexOut)));
                // Half input size for first dimension of R2C
                dimOk &= (isComplexIn || i+1 != numDims || eIn/2+1 == eOut);
                // Half output size for first dimension of C2R
                dimOk &= (isComplexOut || i+1 != numDims || eIn == eOut/2+1);
                if(!dimOk)
                    throw std::runtime_error("Dimension " + std::to_string(i) + ": Extents mismatch");
            }
            // Need 2 counts as they are different for C2R/R2C (maybe 1 element off)
            size_t numElementsIn = input.getNumElements();
            size_t numElementsOut = output.getNumElements();

            if(useInplaceForHost && !Input::IsDeviceMemory::value && !Output::IsDeviceMemory::value){
                size_t size = std::max(numElementsIn, numElementsOut) * sizeof(Precision);
                checkSize(size);
                plan.InDevicePtr.reset(alloc.malloc(size,plan.ctx));
            }else{
              /* @todo need correct sizes here,
               * R2C can be Real->cl_mem->Complex
               * or Real->cl_mem->cl_mem->Complex (useInplaceForHost==false)
               * Real equals Precision, but Complex==? Is sizeof(Complex) just 2*sizeof(Precision)?
               */
                size_t inSize = numElementsIn * sizeof(Precision) * (isComplexIn?2:1);
                size_t outSize = numElementsOut * sizeof(Precision) * (isComplexOut?2:1);
                checkSize(inSize);
                checkSize(outSize);
                if(!Input::IsDeviceMemory::value)
                    plan.InDevicePtr.reset(alloc.malloc(inSize,plan.ctx));
                if(!Output::IsDeviceMemory::value)
                    plan.OutDevicePtr.reset(alloc.malloc(outSize,plan.ctx));
            }
            // Always use fullExtents, that is the extents of the real container for R2C/C2R
            // For C2C it does not matter
            createPlan(plan, isComplexIn ? extentsOut : extents);
        }

        template< class T_Plan, class T_Allocator >
        void
        operator()(T_Plan& plan, Input& inOut, const T_Allocator& alloc)
        {
          // @todo correct size for inplace GetInplaceMemSize
          size_t size = inOut.getNumElements() * sizeof(Precision);//policies::GetInplaceMemSize<Precision, isComplexIn, isComplexOut, numDims>::get(inOut.getFullExtents());
          checkSize(size);
          if(!Input::IsDeviceMemory::value)
              plan.InDevicePtr.reset(alloc.malloc(size,plan.ctx));
          createPlan(plan, inOut.getFullExtents());
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace libLiFFT
