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
 
#include "testUtils.hpp"
#include "testDefines.hpp"
#include "libLiFFT/types/Vec.hpp"
#include "libLiFFT/FFT.hpp"
#include "libLiFFT/generateData.hpp"
#include "libLiFFT/accessors/TransformAccessor.hpp"
#include "libLiFFT/accessors/TransposeAccessor.hpp"
#include "libLiFFT/types/SymmetricWrapper.hpp"
#include "libLiFFT/accessors/StreamAccessor.hpp"
#include "libLiFFT/types/AddDimsWrapper.hpp"
#include "libLiFFT/policies/CalcIntensityFunctor.hpp"
#include "libLiFFT/types/View.hpp"
#include "libLiFFT/types/SliceView.hpp"
#include <iostream>
#include <fstream>
// see https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
#include <memory>

using LiFFT::generateData;
using namespace LiFFT::generators;

namespace LiFFTTest {

    BaseR2CInput  baseR2CInput;
    BaseR2COutput baseR2COutput;
    BaseC2CInput  baseC2CInput;
    BaseC2COutput baseC2COutput;

    using FFT_R2C_Definition = LiFFT::FFT_Definition< LiFFT::FFT_Kind::Real2Complex,
                                                      testNumDims,
                                                      TestPrecision,
                                                      std::true_type>;
    using FFT_C2C_Definition = LiFFT::FFT_Definition< LiFFT::FFT_Kind::Complex2Complex,
                                                      testNumDims,
                                                      TestPrecision,
                                                      std::true_type>;
    using FFT_R2C = LiFFT::FFT<TestLibrary,
                               decltype(FFT_R2C_Definition::wrapInput(baseR2CInput)),
                               decltype(FFT_R2C_Definition::wrapOutput(baseR2COutput)),
                               false>;
    using FFT_C2C = LiFFT::FFT<TestLibrary,
                               decltype(FFT_C2C_Definition::wrapInput(baseC2CInput)),
                               decltype(FFT_C2C_Definition::wrapOutput(baseC2COutput)),
                               false>;

    FFT_R2C* fftR2C;
    FFT_C2C* fftC2C;

    /**
     * Writes nD data to a file as strings

     * @param name filePath
     * @param data Data to write
     * @param acc  Accessor to use
     */
    template< typename T, class T_Accessor = LiFFT::traits::IdentityAccessor_t<T> >
    void write2File(const std::string& name, T& data, T_Accessor acc = T_Accessor()){
        auto copy = LiFFT::policies::makeCopy(acc, LiFFT::accessors::StringStreamAccessor<>());

        LiFFT::types::AddDimsWrapper< std::ofstream, 2 > file(name.c_str());
        copy(data, file);
        file.close();
    }

    /**
     * Writes the intensity of the data to a file (as string)
     *
     * @param name filePath
     * @param data Data to write
     * @param acc  Accessor to use
     */
    template< typename T, class T_Accessor = LiFFT::traits::IdentityAccessor_t<T> >
    void writeIntensity2File(const std::string& name, T& data, T_Accessor acc = T_Accessor()){
        write2File(name, data, LiFFT::accessors::makeTransformAccessor(acc, LiFFT::policies::CalcIntensityFunc()));
    }

    void init()
    {
        TestExtents size = TestExtents::all(testSize);
        TestExtents sizeHalf = size;
        sizeHalf[testNumDims-1] = size[testNumDims-1] / 2 + 1;
        baseR2CInput.allocData(size);
        baseR2COutput.allocData(sizeHalf);
        baseC2CInput.allocData(size);
        baseC2COutput.allocData(size);

        {
            auto input = FFT_R2C_Definition::wrapInput(baseR2CInput);
            auto output = FFT_R2C_Definition::wrapOutput(baseR2COutput);
            fftR2C = new FFT_R2C(input, output);
        }
        {
            auto input = FFT_C2C_Definition::wrapInput(baseC2CInput);
            auto output = FFT_C2C_Definition::wrapOutput(baseC2COutput);
            fftC2C = new FFT_C2C(input, output);
        }
    }

    void finalize()
    {
        baseR2CInput.freeData();
        baseR2COutput.freeData();
        baseC2CInput.freeData();
        baseC2COutput.freeData();
        delete fftR2C;
        delete fftC2C;
    }

    void visualizeBase()
    {
        generateData(baseR2CInput, Rect<TestPrecision>(20,testSize/2));
        generateData(baseC2CInput, Rect<TestPrecision>(20,testSize/2));
        execBaseR2C();
        execBaseC2C();
        auto fullR2COutput = LiFFT::types::makeSymmetricWrapper(baseR2COutput, baseC2CInput.getExtents()[baseR2CInput.numDims-1]);
        checkResult(baseC2COutput, fullR2COutput, "Self check");

        visualizeOutput(BaseInstance::InC2C, "inputC2C.pdf");
        visualizeOutput(BaseInstance::InR2C, "inputR2C.pdf");
        visualizeOutput(BaseInstance::OutC2C, "outputC2C.pdf");
        visualizeOutput(BaseInstance::OutR2C, "outputR2C.pdf");
    }

    void visualizeOutput(BaseInstance inst, const std::string& filePath){
        std::string txtFile = filePath+".txt";
        switch(inst)
        {
        case BaseInstance::InC2C:
            writeIntensity2File(txtFile, baseC2CInput);
            break;
        case BaseInstance::InR2C:
            writeIntensity2File(txtFile, baseC2CInput);
            break;
        case BaseInstance::OutC2C:
            writeIntensity2File(txtFile, baseC2COutput, LiFFT::accessors::makeTransposeAccessorFor(baseC2COutput));
            break;
        case BaseInstance::OutR2C:
            {
                auto fullR2COutput = LiFFT::types::makeSymmetricWrapper(baseR2COutput, baseC2CInput.getExtents()[baseR2CInput.numDims-1]);
                writeIntensity2File(txtFile, fullR2COutput, LiFFT::accessors::makeTransposeAccessorFor(fullR2COutput));
            }
            break;
        default:
            throw std::runtime_error("Invalid value");
        }
        std::string cmd = "python writeData.py -i \"" + txtFile + "\" -o \"" + filePath + "\"";
        if(std::system(cmd.c_str()))
            std::cout << "Error converting txt to pdf for " << filePath << "\n";
    }

    void execBaseR2C()
    {
        auto input  = FFT_R2C_Definition::wrapInput( baseR2CInput);
        auto output = FFT_R2C_Definition::wrapOutput(baseR2COutput);
        (*fftR2C)(input, output);
    }

    void execBaseC2C()
    {
        auto input  = FFT_C2C_Definition::wrapInput( baseC2CInput);
        auto output = FFT_C2C_Definition::wrapOutput(baseC2COutput);
        (*fftC2C)(input, output);
    }

}  // namespace LiFFTTest
