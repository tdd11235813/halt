#pragma once

#include "libLiFFT/libraries/clFFT/clfft_helper.hpp"

#include <clFFT.h>
#include <CL/cl.h>
#include <iostream>

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
        malloc(size_t memSize, cl_context ctx, cl_command_queue queue) const
        {
          cl_int err=0;
          cl_mem mem = clCreateBuffer( ctx,
                                       CL_MEM_READ_WRITE,
                                       memSize,
                                       NULL,
                                       &err );
          CHECK_CL(err);
          return mem;
        }

        void
        free(cl_mem mem) const
        {
          CHECK_CL(clReleaseMemObject(mem));
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
