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
          std::cout << "malloc cl buffer "<<memSize<<" b\n";
          cl_int err=0;
          cl_mem mem = clCreateBuffer( ctx,
                                       CL_MEM_READ_WRITE,
                                       memSize,
                                       NULL,
                                       &err );
          CHECK_CL(err);
          float initValue = 0.0;
          CHECK_CL( clEnqueueFillBuffer ( queue,
                                          mem,
                                          &initValue, // pattern
                                          sizeof(float), // pattern size
                                          0, // offset
                                          memSize ,
                                          0, // num events
                                          nullptr,
                                          nullptr) );
          return mem;
        }

        void
        free(cl_mem mem) const
        {
          std::cout << "release cl buffer\n";
          CHECK_CL(clReleaseMemObject(mem));
        }
    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
