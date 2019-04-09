/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/policies/IntensityCalculator.hpp"
#include "libLiFFT/policies/GetNumElements.hpp"

void testIntensityCalculator()
{
    using namespace LiFFT::types;
    using namespace LiFFT::mem;
    using LiFFT::policies::getNumElements;
    using LiFFT::policies::useIntensityCalculator;
    DataContainer< 1, RealValues<double>> real;
    DataContainer< 1, ComplexAoSValues<double>> complexAoS;
    DataContainer< 1, ComplexSoAValues<double>> complexSoA;
    DataContainer< 3, RealValues<double>> real3D;
    DataContainer< 3, ComplexAoSValues<double>> complexAoS3D;
    DataContainer< 3, ComplexSoAValues<double>> complexSoA3D;

    std::vector<double> result(NumVals);

    real.extents = {NumVals};
    real.data = new Real<double>[getNumElements(real)];

    complexAoS.extents = {NumVals};
    complexAoS.data = new Complex<double>[getNumElements(complexAoS)];

    complexSoA.extents = {NumVals};
    complexSoA.data.getRealData() = new Real<double>[getNumElements(complexSoA)];
    complexSoA.data.getImagData() = new Real<double>[getNumElements(complexSoA)];

    real3D.extents = {NumVals, NumVals+1, NumVals+2};
    real3D.data = new Real<double>[getNumElements(real3D)];

    complexAoS3D.extents = {NumVals, NumVals+1, NumVals+2};
    complexAoS3D.data = new Complex<double>[getNumElements(complexAoS3D)];

    complexSoA3D.extents = {NumVals, NumVals+1, NumVals+2};
    complexSoA3D.data.getRealData() = new Real<double>[getNumElements(complexSoA3D)];
    complexSoA3D.data.getImagData() = new Real<double>[getNumElements(complexSoA3D)];

    useIntensityCalculator(real, result.data());
    useIntensityCalculator(complexAoS, result.data());
    useIntensityCalculator(complexSoA, result.data());
    useIntensityCalculator(real3D, result.data());
    useIntensityCalculator(complexAoS3D, result.data());
    useIntensityCalculator(complexSoA3D, result.data());
}
