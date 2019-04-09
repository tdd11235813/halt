/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/types/Vec.hpp"
#include "libLiFFT/policies/SafePtrCast.hpp"
#include "libLiFFT/testLib/IntensityLibrary.hpp"
#include <boost/array.hpp>

namespace LiFFT {
namespace policies {

    template< class T_Input, class T_Accessor = typename T_Input::Accessor >
    class IntensityCalculator
    {
    public:
        using Input = T_Input;
        using Output = typename traits::IntegralType<Input>::type*;
        using Extents = GetExtents<Input>;
        using ExtentsPtr = GetExtentsRawPtr<Input>;
        static constexpr bool isComplex = traits::IsComplex<Input>::value;
        static constexpr unsigned numDims = traits::NumDims<Input>::value;
        static_assert(numDims >= 1, "Need >= 1 dimension");
        static constexpr bool isAoS = traits::IsAoS<Input>::value;

    private:
        template< unsigned T_numDims, bool T_isComplex, bool T_isAoS, class DUMMY = void >
        struct ExecutionPolicy;

        // Real 1D
        template< bool T_isAoS >
        struct ExecutionPolicy< 1, false, T_isAoS >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                LibFoo::calculateR1D(
                        safe_ptr_cast<Output>(&m_acc(LiFFT::types::Vec<1>::all(0), input)),
                        output,
                        Extents(input)[0]);
            }
        };

        // Real ND
        template< unsigned T_numDims, bool T_isAoS >
        struct ExecutionPolicy< T_numDims, false, T_isAoS >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                ExtentsPtr extents(input);
                LibFoo::calculateRND(safe_ptr_cast<Output>(&m_acc(LiFFT::types::Vec<T_numDims>::all(0), input)),
                        output, T_numDims, extents());
            }
        };

        // Complex 1D
        template< class DUMMY >
        struct ExecutionPolicy< 1, true, true, DUMMY >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                LibFoo::calculateC1D(
                        safe_ptr_cast<Output>(&m_acc(LiFFT::types::Vec<1>::all(0), input)),
                        output,
                        Extents(input)[0]);
            }
        };

        // Complex ND
        template< unsigned T_numDims, class DUMMY >
        struct ExecutionPolicy< T_numDims, true, true, DUMMY >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                ExtentsPtr extents(input);
                LibFoo::calculateCND(safe_ptr_cast<Output>(&m_acc(LiFFT::types::Vec<T_numDims>::all(0), input)),
                        output, T_numDims, extents());
            }
        };

        // Complex(SoA) 1D
        template< class DUMMY >
        struct ExecutionPolicy< 1, true, false, DUMMY >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                auto ptr = m_acc(LiFFT::types::Vec<1>::all(0), input);
                LibFoo::calculateC1D(safe_ptr_cast<Output>(&ptr.real),
                        safe_ptr_cast<Output>(&ptr.imag),
                        output,
                        Extents(input)[0]);
            }
        };

        // Complex(SoA) ND
        template< unsigned T_numDims, class DUMMY >
        struct ExecutionPolicy< T_numDims, true, false, DUMMY >
        {
            T_Accessor m_acc;
            void operator()(Input& input, Output output){
                ExtentsPtr extents(input);
                auto ptr = m_acc(LiFFT::types::Vec<T_numDims>::all(0), input);
                LibFoo::calculateCND(safe_ptr_cast<Output>(&ptr.real),
                        safe_ptr_cast<Output>(&ptr.imag),
                        output,
                        T_numDims,
                        extents());
            }
        };

    public:
        using type = ExecutionPolicy< numDims, isComplex, isAoS >;
    };

    template< class T_Input, class Output >
    void
    useIntensityCalculator(T_Input& input, Output output){
        typename IntensityCalculator<T_Input>::type calculator;
        calculator(input, output);
    }

}  // namespace policies
}  // namespace LiFFT
