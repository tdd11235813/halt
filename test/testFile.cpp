/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "testUtils.hpp"

#include "libLiFFT/mem/DataContainer.hpp"
#include "libLiFFT/mem/FileContainer.hpp"
#include "libLiFFT/accessors/ImageAccessor.hpp"
#include "tiffWriter/image.hpp"
#include "tiffWriter/traitsAndPolicies.hpp"
#include "libLiFFT/FFT.hpp"
#include "libLiFFT/generateData.hpp"
#include "libLiFFT/accessors/TransposeAccessor.hpp"
#include <boost/test/unit_test.hpp>


using LiFFT::generateData;
using namespace LiFFT::generators;

namespace LiFFTTest {

    BOOST_AUTO_TEST_SUITE(File)

    BOOST_AUTO_TEST_CASE(File)
    {
        using FileType = LiFFT::mem::FileContainer<
            tiffWriter::Image<>,
            LiFFT::accessors::ImageAccessorGetColorAsFp<TestPrecision>,
            TestPrecision,
            false
            >;
        FileType file("rect.tif");

        using FFTResult_t = LiFFT::mem::ComplexContainer<2, TestPrecision>;
        FFTResult_t fftResult(LiFFT::types::Idx2D(file.getExtents()[1], file.getExtents()[0]/2+1));
        using FFT_Type = LiFFT::FFT_2D_R2C<TestPrecision>;
        auto input = FFT_Type::wrapInput(file);
        auto output = FFT_Type::wrapOutput(fftResult);
        auto fft = LiFFT::makeFFT<TestLibrary>(input, output);
        file.loadData(true);
        fft(input, output);
        LiFFT::policies::copy(file, baseR2CInput);
        execBaseR2C();
        checkResult(baseR2COutput, fftResult, "R2C with file input");
    }

    BOOST_AUTO_TEST_CASE(TiffCopy)
    {
        std::string filePath = "input1.tif";
        std::string filePath2 = filePath+"2.tif";
        tiffWriter::FloatImage<> img(filePath);
        img.saveTo(filePath2);
        img.close();
        tiffWriter::FloatImage<> img1(filePath);
        tiffWriter::FloatImage<> img2(filePath2);
        std::remove(filePath2.c_str());
        checkResult(img1, img2, "TIFF copy");
    }

    BOOST_AUTO_TEST_CASE(TiffModify)
    {
        std::string filePath = "input1.tif";
        std::string filePath2 = filePath+"2.tif";
        tiffWriter::FloatImage<> img(filePath);
        LiFFT::mem::RealContainer<2, float> data(LiFFT::types::Vec<2>(img.getHeight(), img.getWidth()));
        generateData(data, Circle<float>(50, img.getHeight()/2));
        auto acc = LiFFT::accessors::makeTransposeAccessorFor(img);
        LiFFT::policies::copy(data, img, LiFFT::traits::getIdentityAccessor(data), acc);
        img.saveTo(filePath2);
        img.close();
        tiffWriter::FloatImage<> img2(filePath2);
        auto accData = LiFFT::accessors::makeTransposeAccessorFor(data);
        auto res = compare(data, img2, CmpError(1e-8, 1e-8), accData);
        std::remove(filePath2.c_str());
        if(!res.first)
            BOOST_ERROR("Error for TiffModify: " << res.second);
    }

    BOOST_AUTO_TEST_CASE(TiffFile)
    {
        std::string filePath = "input1.tif";
        tiffWriter::FloatImage<> img(filePath, false);
        using FFT_Type = LiFFT::FFT_2D_R2C_F<>;
        auto input = FFT_Type::wrapInput(img);
        auto output = FFT_Type::createNewOutput(input);
        auto fft = LiFFT::makeFFT< TestLibrary, false >(input, output);
        img.load();
        LiFFT::policies::copy(img, baseR2CInput);
        fft(input, output);
        execBaseR2C();
        visualizeOutput(BaseInstance::OutR2C, "outputTiff.pdf");
        checkResult(baseR2COutput, output, "TIFF test");
    }

    BOOST_AUTO_TEST_SUITE_END()

}  // namespace LiFFTTest
