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
     * Allocator that uses cudaMalloc/cudaFree
     */
    struct CudaAllocator
    {
        template< typename T >
        T*
        malloc(size_t memSize) const
        {
            void* ptr;
            CHECK_ERROR(cudaMalloc(&ptr, memSize));
            return reinterpret_cast<T*>(ptr);
        }

        template< typename T >
        void
        free(T* ptr) const
        {
            CHECK_ERROR(cudaFree(ptr));
        }
    };

}  // namespace policies
}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
