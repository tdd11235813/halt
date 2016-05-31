#pragma once

#include <CL/cl.h>
#include "libLiFFT/libraries/clFFT/clfft_helper.hpp"

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

    /**
     * Copier
     * @todo put different copiers (mem-mem, mem-ptr)
     * https://github.com/clMathLibraries/clFFT
     */
    struct OpenCLMemCpy
    {
    public:

      /* H2D */
      template<typename T_Src>
      void copy(cl_mem dst, T_Src src, size_t size, cl_command_queue queue) const
      {
        CHECK_CL( clEnqueueWriteBuffer(queue, dst, false, 0, size, src, 0, NULL, NULL) );
      }
      /* D2H */
      template<typename T_Dst>
      void copy(T_Dst dst, cl_mem src, size_t size, cl_command_queue queue) const
      {
        CHECK_CL( clEnqueueReadBuffer(queue, src, false, 0, size, dst, 0, NULL, NULL) );
      }
      /* D2D */
      void copy(cl_mem dst, cl_mem src, size_t size, cl_command_queue queue) const
      {
        CHECK_CL( clEnqueueCopyBuffer(queue, src, dst, 0, 0, size, 0, NULL, NULL) );
      }


    };

}  // namespace policies
}  // namespace clFFT
}  // namespace libraries
}  // namespace LiFFT
