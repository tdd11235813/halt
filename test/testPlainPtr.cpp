/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "testUtils.hpp"
#include "libLiFFT/types/Complex.hpp"
#include "libLiFFT/types/Real.hpp"
#include "libLiFFT/mem/PlainPtrWrapper.hpp"
#include "libLiFFT/FFT.hpp"
#include "libLiFFT/policies/Copy.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

namespace LiFFTTest {

    BOOST_AUTO_TEST_SUITE(PlainPtr)

    BOOST_AUTO_TEST_CASE(PlainPtr)
    {
        static_assert(testNumDims == 2, "Currently only 2D is tested");
        using Real = LiFFT::types::Real<TestPrecision>;
        using Complex = LiFFT::types::Complex<TestPrecision>;
        std::unique_ptr<Real[]> input(new Real[testSize*testSize]);
        std::unique_ptr<Complex[]> output(new Complex[testSize*(testSize/2+1)]);
        for(unsigned i=0; i<testSize*testSize; ++i)
            input[i] = std::rand() / RAND_MAX;
        using FFT_TYPE = LiFFT::FFT_2D_R2C<TestPrecision>;
        auto inWrapped = FFT_TYPE::wrapInput( LiFFT::mem::wrapPtr<false>(input.get(), TestExtents(testSize, testSize)) );
        auto outWrapped = FFT_TYPE::wrapOutput(LiFFT::mem::wrapPtr<true>(output.get(), TestExtents(testSize, testSize/2+1)));
        auto fft = LiFFT::makeFFT<TestLibrary>(inWrapped, outWrapped);
        fft(inWrapped, outWrapped);
        LiFFT::policies::copy(inWrapped, baseR2CInput);
        execBaseR2C();
        checkResult(baseR2COutput, outWrapped, "R2C with PlainPtrWrapper");
    }

    BOOST_AUTO_TEST_SUITE_END()

}  // namespace LiFFTTest
