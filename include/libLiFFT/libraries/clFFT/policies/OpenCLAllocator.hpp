#pragma once

#include "libLiFFT/libraries/clFFT/clfft_helper.hpp"

#include <clFFT.h>
#include <CL/cl.h>

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

    /**
     * Allocator
     */
    struct OpenCLAllocator
    {
        cl_mem
        malloc(size_t memSize, cl_context ctx) const
        {
          cl_int err=0;
          cl_mem mem = clCreateBuffer( ctx, CL_MEM_READ_WRITE, memSize, NULL, &err );
          clCheckError(err);
          return mem;
        }

        void
        free(cl_mem mem) const
        {
          clSafeCall(clReleaseMemObject(mem));
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
