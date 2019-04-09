/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/mem/DataContainer.hpp"
#include "libLiFFT/mem/RealValues.hpp"
#include "libLiFFT/mem/ComplexAoSValues.hpp"
#ifdef WITH_CUDA
#include "libLiFFT/libraries/cuFFT/cuFFT.hpp"
#else
#include "libLiFFT/libraries/fftw/FFTW.hpp"
#endif

namespace LiFFTTest{
    // Types used for the test suite
    constexpr unsigned testNumDims = 2;
    constexpr unsigned testSize = 2048;
    using TestExtents      = LiFFT::types::Vec<testNumDims>;
    using TestPrecision    = float;
    using RealType         = LiFFT::mem::RealValues<TestPrecision>;
    using ComplexType      = LiFFT::mem::ComplexAoSValues<TestPrecision>;
    using RealContainer    = LiFFT::mem::DataContainer<testNumDims, RealType>;
    using ComplexContainer = LiFFT::mem::DataContainer<testNumDims, ComplexType>;
    using BaseR2CInput  = RealContainer;
    using BaseR2COutput = ComplexContainer;
    using BaseC2CInput  = ComplexContainer;
    using BaseC2COutput = ComplexContainer;

    #ifdef WITH_CUDA
    using TestLibrary = LiFFT::libraries::cuFFT::CuFFT<>;
    #else
    using TestLibrary = LiFFT::libraries::fftw::FFTW<>;
    #endif

    // Control values used in the test suite
    extern BaseR2CInput  baseR2CInput;
    extern BaseR2COutput baseR2COutput;
    extern BaseC2CInput  baseC2CInput;
    extern BaseC2COutput baseC2COutput;
}
