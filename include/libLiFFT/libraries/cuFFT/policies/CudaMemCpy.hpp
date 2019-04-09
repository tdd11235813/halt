/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/libraries/cuFFT/cufft_helper.hpp"

#include <cuda.h>
#include <cuda_runtime_api.h>

namespace LiFFT {
namespace libraries {
namespace cuFFT {
namespace policies {

    /**
     * Copier that uses standard cudaMemCpy
     */
    struct CudaMemCpy
    {
    private:
        void cpy1D(void* ptrTo, void* ptrFrom, size_t size, cudaMemcpyKind direction) const
        {
            CHECK_ERROR(cudaMemcpy(ptrTo, ptrFrom, size, direction));
        }

        void cpyPitched(void* ptrTo, void* ptrFrom, size_t w, size_t h, size_t pitchTo, size_t pitchFrom, cudaMemcpyKind direction) const
        {
            if( pitchTo == pitchFrom && (h == 1 || pitchTo == w) )
                cpy1D(ptrTo, ptrFrom, pitchTo*h, direction);
            CHECK_ERROR(cudaMemcpy2D(ptrTo, pitchTo, ptrFrom, pitchFrom, w, h, direction));
        }
    public:
        template< typename T >
        void H2D(T* ptrTo, T* ptrFrom, size_t w, size_t h, size_t pitchTo, size_t pitchFrom) const
        {
            cpyPitched(ptrTo, ptrFrom, w, h, pitchTo, pitchFrom, cudaMemcpyHostToDevice);
        }

        template< typename T >
        void D2H(T* ptrTo, T* ptrFrom, size_t w, size_t h, size_t pitchTo, size_t pitchFrom) const
        {
            cpyPitched(ptrTo, ptrFrom, w, h, pitchTo, pitchFrom, cudaMemcpyDeviceToHost);
        }
    };

}  // namespace policies
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
