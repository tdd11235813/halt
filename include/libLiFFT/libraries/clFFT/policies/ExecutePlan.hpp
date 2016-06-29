#pragma once

#include <cassert>
#include <type_traits>
#include <clFFT.h>
#include <string>
#include "libLiFFT/types/TypePair.hpp"
#include "libLiFFT/libraries/clFFT/Plan.hpp"
#include "libLiFFT/libraries/clFFT/traits/FFTType.hpp"
#include "libLiFFT/libraries/clFFT/traits/Sign.hpp"
#include "libLiFFT/policies/SafePtrCast.hpp"
#include "libLiFFT/libraries/clFFT/policies/GetInplaceMemSize.hpp"

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

  namespace detail {

    template<
      typename T_Precision,
      bool T_isComplexIn,
      bool T_isComplexOut,
      bool T_isFwd
      >
    struct Executer
    {
      cl_int operator()(clfftPlanHandle plan,
                        cl_command_queue queue,
                        cl_mem in,
                        cl_mem out) {
        return clfftEnqueueTransform(plan,
                                     traits::Sign< T_isFwd >::value,
                                     1, &queue, // queue
                                     0, NULL, NULL, // events
                                     &in, // input buffer
                                     &out,  // output buffer
                                     NULL   // tmp buffer
          );
      }
    };


  }  // namespace detail

  template<
    typename T_Precision,
    class T_InOutTypes,
    bool T_isFwd,
    bool T_isInplace,
    unsigned T_numDims,
    bool T_isComplexIn,
    bool T_isComplexOut
    >
  struct ExecutePlan
  {
  private:
    using Precision = T_Precision;
    using Input = typename T_InOutTypes::First;
    using Output = typename T_InOutTypes::Second;
    static constexpr bool isFwd = T_isFwd;
    static constexpr unsigned numDims = T_numDims;
    static constexpr bool isInplace = T_isInplace;
    static constexpr bool isComplexIn = T_isComplexIn;
    static constexpr bool isComplexOut = T_isComplexOut;

    using Executer = detail::Executer< Precision, isComplexIn, isComplexOut, isFwd >;

  public:
    template< class T_Plan, class T_Copier >
    void
    operator()(T_Plan& plan,
               Input& input,
               Output& output,
               bool useInplaceForHost,
               const T_Copier& copy) {
      printf("ExecutePlan : in + out: %s\n", isComplexIn&&isComplexOut?"C2C":isComplexIn?"C2R":"R2C");
      using LiFFT::policies::safe_ptr_cast;
      static_assert(!isInplace, "Cannot be used for inplace transforms!");
      size_t memsizeIn = sizeof(Precision)*input.getNumElements() * (isComplexIn?2:1);
      size_t memsizeOut = sizeof(Precision)*output.getNumElements() * (isComplexOut?2:1);
      printf("size %zu %zu, counts %zu %zu\n", memsizeIn, memsizeOut,input.getNumElements(), output.getNumElements());
      cl_mem pIn = 0;
      if( plan.InDevicePtr )
      {
        // host w unpitched reals to pitched reals in device buffer
        if(useInplaceForHost && !isComplexIn) {
          auto fullExtents = input.getFullExtents();
          size_t w = fullExtents[numDims-1] * sizeof(Precision);
          size_t pitch = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);
          size_t h = memsizeIn/sizeof(Precision) / fullExtents[numDims-1];
          copy.copyPitched(plan.InDevicePtr.get(),
                           input.getDataPtr(),
                           plan.queue,
                           w,
                           h,
                           pitch
            );
        }else
          copy.copy(plan.InDevicePtr.get(),
                    input.getDataPtr(),
                    memsizeIn,
                    plan.queue);
        pIn = safe_ptr_cast<cl_mem>(plan.InDevicePtr.get());
      }else if(Input::IsDeviceMemory::value)
        pIn = reinterpret_cast<cl_mem>(input.getDataPtr());
      else
        throw std::runtime_error("No in device pointer");

      cl_mem pOut;
      if(plan.OutDevicePtr)
        pOut = safe_ptr_cast<cl_mem>(plan.OutDevicePtr.get());
      else if(Output::IsDeviceMemory::value)
        pOut = reinterpret_cast<cl_mem>(output.getDataPtr());
      else if(useInplaceForHost)
        pOut = safe_ptr_cast<cl_mem>(plan.InDevicePtr.get());
      else
        throw std::runtime_error("No out device pointer");

      CHECK_CL(clFinish(plan.queue));
      CHECK_CL( Executer()(plan.handle, plan.queue, pIn, pOut) );
      CHECK_CL(clFinish(plan.queue));

      if( plan.OutDevicePtr || !Output::IsDeviceMemory::value) {
        if(useInplaceForHost && !isComplexOut) {
          auto fullExtents = output.getFullExtents();
          size_t w = fullExtents[numDims-1] * sizeof(Precision);
          size_t pitch = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);
          size_t h = memsizeOut/sizeof(Precision) / fullExtents[numDims-1];
          copy.copyPitched(output.getDataPtr(),
                           pOut,
                           plan.queue,
                           w,
                           h,
                           pitch
            );
        }else
          copy.copy(output.getDataPtr(), pOut, memsizeOut, plan.queue);
      }
      for(size_t i=0; i<32; ++i) {
        std::cout << input.getDataPtr()[i] << " | ";
      }
      std::cout << std::endl;
      for(size_t i=0; i<32; ++i) {
        std::cout << output.getDataPtr()[i] << " | ";
      }
      std::cout << std::endl;
    }

    template< class T_Plan, class T_Copier >
    void
    operator()(T_Plan& plan, Input& inOut, const T_Copier& copy) {
      printf("ExecutePlan : inout : %s, %s\n", isComplexIn&&isComplexOut?"C2C":isComplexIn?"C2R":"R2C", isFwd?"Fwd":"Inv");
      using LiFFT::policies::safe_ptr_cast;
      static_assert(isInplace, "Must be used for inplace transforms!");

//      auto fullExtents = inOut.getFullExtents();
//      size_t n = LiFFT::policies::getNumElementsFromExtents(fullExtents);

      cl_mem pIn = 0;
      if( plan.InDevicePtr ) {
/*        if(!isComplexIn) {
          size_t w = fullExtents[numDims-1]*sizeof(Precision);
          size_t pitch = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);
          size_t h = n / fullExtents[numDims-1];
          copy.copyPitched(plan.InDevicePtr.get(),
                           inOut.getDataPtr(),
                           plan.queue,
                           w,
                           h,
                           pitch
            );
        }else if(!isComplexOut){
          size_t w = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);
          size_t pitch = w;
          size_t h = n / fullExtents[numDims-1];
          copy.copyPitched(plan.InDevicePtr.get(),
                           inOut.getDataPtr(),
                           plan.queue,
                           w,
                           h,
                           pitch
            );
        }else{*/
          copy.copy(plan.InDevicePtr.get(),
                    inOut.getDataPtr(),
                    inOut.getMemSize(),
                    plan.queue);
//        }
        pIn = plan.InDevicePtr.get();
      }else if(Input::IsDeviceMemory::value)
        pIn = reinterpret_cast<cl_mem>(inOut.getDataPtr());
      else
        throw std::runtime_error("No in device pointer");

      cl_mem pOut = pIn;

      CHECK_CL( Executer()(plan.handle, plan.queue, pIn, pOut) );

      auto pOutHost = inOut.getDataPtr();
      if( plan.InDevicePtr ) {
        /*      if(!isComplexIn) { // download complex
          size_t w = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);
          size_t pitch = w;
          size_t h = n / fullExtents[numDims-1];
          copy.copyPitched(pOutHost,
                           pOut,
                           plan.queue,
                           w,
                           h,
                           pitch
            );
        }else if(!isComplexOut){ // download reals
          size_t w = fullExtents[numDims-1]*sizeof(Precision);
          size_t pitch = (fullExtents[numDims-1]/2+1)*2*sizeof(Precision);;
          size_t h = n / fullExtents[numDims-1];
          copy.copyPitched(pOutHost,
                           pOut,
                           plan.queue,
                           w,
                           h,
                           pitch
            );
            }else{*/
          copy.copy(pOutHost,
                    pOut,
                    inOut.getMemSize(),
                    plan.queue);
//        }
      }
      CHECK_CL(clFinish(plan.queue));
    }
  };

}  // namespace policies
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
