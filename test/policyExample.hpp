/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IntegralType.hpp"
#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/IsAoS.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/policies/GetStrides.hpp"
#include "libLiFFT/accessors/DataContainerAccessor.hpp"
#include "libLiFFT/policies/CalcIntensity.hpp"

namespace LiFFT {

    template< class T_Input >
    std::enable_if_t< (traits::IsComplex<T_Input>::value && traits::IsAoS<T_Input>::value) >
    calcIntensity(T_Input& input, typename traits::IntegralType<T_Input>::type* output)
    {
        policies::GetExtents<T_Input> extents(input);
        policies::GetStrides<T_Input> strides(input);
        for(unsigned i=0; i<extents[0]; ++i){
            unsigned idx = i*strides[0];
            auto real = input.data[idx].real;
            auto imag = input.data[idx].imag;
            output[i] = real*real+imag*imag;
        }
    }

    template< class T_Input >
    std::enable_if_t< (traits::IsComplex<T_Input>::value && !traits::IsAoS<T_Input>::value) >
    calcIntensity(T_Input& input, typename traits::IntegralType<T_Input>::type* output)
    {
        policies::GetExtents<T_Input> extents(input);
        policies::GetStrides<T_Input> strides(input);
        for(unsigned i=0; i<extents[0]; ++i){
            unsigned idx = i*strides[0];
            auto real = input.data.getRealData()[idx];
            auto imag = input.data.getImagData()[idx];
            output[i] = real*real+imag*imag;
        }
    }

    template< class T_Input >
    std::enable_if_t< !traits::IsComplex<T_Input>::value >
    calcIntensity(T_Input& input, typename traits::IntegralType<T_Input>::type* output)
    {
        policies::GetExtents<T_Input> extents(input);
        policies::GetStrides<T_Input> strides(input);
        for(unsigned i=0; i<extents[0]; ++i){
            unsigned idx = i*strides[0];
            auto real = input.data[idx];
            output[i] = real*real;
        }
    }

    template< class T_Input >
    void
    calcIntensity2(T_Input& input, typename traits::IntegralType<T_Input>::type* output)
    {
        policies::CalcIntensity< accessors::DataContainerAccessor<> > calcIntensity;
        policies::GetExtents<T_Input> extents(input);
        policies::GetStrides<T_Input> strides(input);
        for(unsigned i=0; i<extents[0]; ++i){
            unsigned idx = i*strides[0];
            output[i] = calcIntensity(input, idx);
        }
    }



}  // namespace LiFFT
