/*
 * @todo clFFT features like scaling factor (planScale), batches, callback
 */
#pragma once

#include "libLiFFT/libraries/clFFT/policies/Context.hpp"
#include "libLiFFT/libraries/clFFT/policies/OpenCLAllocator.hpp"
#include "libLiFFT/libraries/clFFT/policies/OpenCLMemCpy.hpp"
#include "libLiFFT/libraries/clFFT/policies/Planner.hpp"
#include "libLiFFT/libraries/clFFT/policies/ExecutePlan.hpp"
#include <boost/mpl/placeholders.hpp>

namespace bmpl = boost::mpl;

namespace LiFFT {
namespace libraries {
namespace clFFT {

    /**
     * Specifies that inplace transforms are only used when explicitly requested from the FFT
     */
    struct InplaceExplicit: std::false_type{};
    /**
     * Specifies that inplace transforms should be used if both memories reside on host
     */
    struct InplaceForHost: std::true_type{};

    /**
     * Wrapper for the OpenCL-Library that executes the FFT on GPU(s)
     *
     * Note: Allocation and copy will only occur if the IsDeviceMemory trait returns false for the given container
     *
     * \tparam T_InplacePolicy Either InplaceExplicit or InplaceForHost
     * \tparam T_Allocator Policy to alloc/free memory for the input
     * \tparam T_Copier Policy to copy memory to and from the device
     * \tparam T_FFT_Properties Placeholder that will be replaced by a class containing the properties for this FFT
     */
    template<
        class T_Context,
        class T_InplacePolicy = InplaceExplicit,//InplaceForHost,
        class T_Allocator = policies::OpenCLAllocator,
        class T_Copier = policies::OpenCLMemCpy,
        class T_FFT_Properties = bmpl::_1
    >
    class ClFFT
    {
    private:
        static constexpr bool inplaceForHost = T_InplacePolicy::value;
        using Allocator = T_Allocator;
        using Copier = T_Copier;
        using FFT = T_FFT_Properties;
        using Input = typename FFT::Input;
        using Output = typename FFT::Output;
        using PrecisionType = typename FFT::PrecisionType;
        using Planner =
                policies::Planner<
                    T_Context,
                    PrecisionType,
                    LiFFT::types::TypePair< Input, Output >,
                    FFT::isInplace,
                    FFT::numDims,
                    FFT::isComplexIn,
                    FFT::isComplexOut,
                    FFT::isAoSIn,
                    FFT::isAoSOut,
                    FFT::isStridedIn,
                    FFT::isStridedOut
                >;
        using ExecutePlan =
                policies::ExecutePlan<
                    PrecisionType,
                    LiFFT::types::TypePair< Input, Output >,
                    FFT::isFwd,
                    FFT::isInplace,
                    FFT::numDims,
                    FFT::isComplexIn,
                    FFT::isComplexOut
                >;
        using PlanType = Plan<T_Context, Allocator>;

        PlanType plan_;

        ClFFT(ClFFT& obj) = delete;
        ClFFT& operator=(const ClFFT&) = delete;
    public:
        explicit ClFFT(Input& input, Output& output)
        {
          try{
            Planner()(plan_, input, output, inplaceForHost, Allocator());
          }catch(...){
            plan_.cleanup();
          }
        }

        explicit ClFFT(Input& inOut)
        {
          try{
            Planner()(plan_, inOut, Allocator());
          }catch(...){
            plan_.cleanup();
          }
        }

        ClFFT(ClFFT&& obj) = default;
        ClFFT& operator=(ClFFT&&) = default;

        void operator()(Input& input, Output& output)
        {
          try{
            ExecutePlan()(plan_, input, output, inplaceForHost, Copier());
          }catch(...){
            plan_.cleanup();
          }
        }

        void operator()(Input& inOut)
        {
          try{
            ExecutePlan()(plan_, inOut, Copier());
          }catch(...){
            plan_.cleanup();
          }
        }
    };

}  // namespace ClFFT
}  // namespace libraries
}  // namespace LiFFT
