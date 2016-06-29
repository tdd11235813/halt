#pragma once

#include <CL/cl.h>
#include "libLiFFT/libraries/clFFT/clfft_helper.hpp"

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

    /**
     * Copier
     * https://github.com/clMathLibraries/clFFT
     */
    struct OpenCLMemCpy
    {
    public:
      /* H2D */
      void copyPitched(cl_mem dst,
                       const void* src,
                       cl_command_queue queue,
                       size_t w,
                       size_t h,
                       size_t pitch) const
      {
        const size_t offset[3] = {0, 0, 0};
        size_t region[3] = {w, h, 1};
        CHECK_CL(clEnqueueWriteBufferRect( queue,
                                           dst,
                                           CL_TRUE, // blocking_write
                                           offset, // buffer origin
                                           offset, // host origin
                                           region,
                                           pitch, // buffer row pitch
                                           0, // buffer slice pitch
                                           0, // host row pitch
                                           0, // host slice pitch
                                           src,
                                           0, // num_events_in_wait_list
                                           nullptr, // event_wait_list
                                           nullptr )); // event

      }
      /* D2H */
      void copyPitched(void* dst,
                       cl_mem src,
                       cl_command_queue queue,
                       size_t w,
                       size_t h,
                       size_t pitch) const
      {
        const size_t offset[3] = {0, 0, 0};
        size_t region[3] = {w, h, 1};
        CHECK_CL(clEnqueueReadBufferRect( queue,
                                          src,
                                          CL_TRUE, // blocking_write
                                          offset, // buffer origin
                                          offset, // host origin
                                          region,
                                          pitch, // buffer row pitch
                                          0, // buffer slice pitch
                                          0, // host row pitch
                                          0, // host slice pitch
                                          dst,
                                          0, // num_events_in_wait_list
                                          nullptr, // event_wait_list
                                          nullptr )); // event
      }

      /* H2D */
      void copy(cl_mem dst, const void* src, size_t size, cl_command_queue queue) const
      {
        CHECK_CL( clEnqueueWriteBuffer(queue, dst, false, 0, size, src, 0, NULL, NULL) );
      }
      /* D2H */
      void copy(void* dst, cl_mem src, size_t size, cl_command_queue queue) const
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
