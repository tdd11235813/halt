/* This file is part of libLiFFT.
 *
 * libLiFFT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libLiFFT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libLiFFT.  If not, see <www.gnu.org/licenses/>.
 */

#pragma once

#include "libLiFFT/FFT_Kind.hpp"
#include "libLiFFT/FFT_LibPtrWrapper.hpp"
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
     * Wrapper for the OpenCL-Library that executes the FFT on an OpenCL device,
     * prefering GPU(s).
     *
     * Note: Allocation and copy will only occur if the IsDeviceMemory trait returns false for the given container
     *
     * \tparam T_InplacePolicy Either InplaceExplicit or InplaceForHost
     * \tparam T_Allocator Policy to alloc/free memory for the input
     * \tparam T_Copier Policy to copy memory to and from the device
     * \tparam T_FFT_Properties Placeholder that will be replaced by a class containing the properties for this FFT
     */
    template<
        class T_InplacePolicy = InplaceForHost,
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
        using PlanType = Plan<Allocator>;
        using ContextFallback = policies::ContextGlobal;

        PlanType plan_;
        ContextFallback* context_ = nullptr;

        ClFFT(ClFFT& ) = delete;
        ClFFT& operator=(const ClFFT&) = delete;

    public:

        explicit ClFFT(Input& input, Output& output)
        {
            try{
                context_ = new ContextFallback();
                Planner()(plan_, input, output, inplaceForHost, Allocator(), context_->context(), context_->queue());
            }catch(const std::runtime_error& e){
                if(context_) {
                    plan_.cleanup();
                    delete context_;
                    context_ = nullptr;
                }
                throw e;
            }
        }

        explicit ClFFT(Input& inOut)
        {
            try{
                context_ = new ContextFallback();
                Planner()(plan_, inOut, Allocator(), context_->context(), context_->queue());
            }catch(const std::runtime_error& e){
                if(context_) {
                    plan_.cleanup();
                    delete context_;
                    context_ = nullptr;
                }
                throw e;
            }
        }

        template<typename T_Context>
        explicit ClFFT(Input& input, Output& output, T_Context& ctx)
        {
            try{
                Planner()(plan_, input, output, inplaceForHost, Allocator(), ctx.context(), ctx.queue());
            }catch(const std::runtime_error& e){
                plan_.cleanup();
                throw e;
            }
        }

        template<typename T_Context>
        explicit ClFFT(Input& inOut, T_Context& ctx)
        {
            try{
                Planner()(plan_, inOut, Allocator(), ctx.context(), ctx.queue());
            }catch(const std::runtime_error& e){
                plan_.cleanup();
                throw e;
            }
        }

        ~ClFFT() {
            if(context_) {
                plan_.cleanup();
                delete context_;
                context_ = nullptr;
            }
        }

        ClFFT(ClFFT&&) = default;
        ClFFT& operator=(ClFFT&&) = default;

        void operator()(Input& input, Output& output)
        {
            try {
                ExecutePlan()(plan_, input, output, inplaceForHost, Copier(), context_->queue());
            }catch(const std::runtime_error& e){
                std::cerr << "Error in ExecutePlan()() occurred (outplace): "<<e.what()<<std::endl;
                plan_.cleanup();
            }
        }

        void operator()(Input& inOut)
        {
            try {
                ExecutePlan()(plan_, inOut, Copier(), context_->queue());
            }catch(const std::runtime_error& e){
                plan_.cleanup();
                throw e;
            }
        }

        template<typename T_Context>
        void operator()(Input& input, Output& output, T_Context& ctx)
        {
            try {
                ExecutePlan()(plan_, input, output, inplaceForHost, Copier(), ctx.queue());
            }catch(const std::runtime_error& e){
                plan_.cleanup();
                throw e;
            }
        }

        template<typename T_Context>
        void operator()(Input& inOut, T_Context& ctx)
        {
            try {
                ExecutePlan()(plan_, inOut, Copier(), ctx.queue());
            }catch(const std::runtime_error& e){
                plan_.cleanup();
                throw e;
            }
        }
    };

}  // namespace clFFT
}  // namespace libraries
}  // namespace LiFFT
