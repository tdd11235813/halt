#pragma once

#include "clfft_helper.hpp"

#include <CL/cl.h>
#include <clFFT.h>
#include <memory>

namespace LiFFT {
namespace libraries {
namespace clFFT {

    template< typename T_Context, class T_Deleter >
    struct Plan
    {
    private:

        T_Context context;

        struct Deleter
        {
            void
            operator()(cl_mem ptr)
            {
                T_Deleter().free(ptr);
            }
        };
        Plan(const Plan&) = delete;
        Plan& operator=(const Plan&) = delete;
    public:
        cl_context ctx = 0;
        cl_command_queue queue = 0;
        clfftPlanHandle handle = 0;
        std::unique_ptr<_cl_mem, Deleter> InDevicePtr;
        std::unique_ptr<_cl_mem, Deleter> OutDevicePtr;

        Plan()
        {
          std::cout << "Plan: create\n";
          cl_int err = 0;
          ctx = context.context();
          queue = clCreateCommandQueue( ctx, context.device(), 0, &err );
          CHECK_CL(err);
        }
        Plan(Plan&& obj)
        : ctx(obj.ctx), queue(obj.queue), handle(obj.handle),
          InDevicePtr(std::move(obj.InDevicePtr)), OutDevicePtr(std::move(obj.OutDevicePtr))
        {
          obj.ctx = 0;
          obj.queue = 0;
          obj.handle = 0;
        }

        Plan& operator=(Plan&& obj)
        {
            if(this!=&obj)
                return *this;
            ctx = obj.ctx; obj.ctx = 0;
            queue = obj.queue; obj.queue = 0;
            handle = obj.handle; obj.handle = 0;
            InDevicePtr = std::move(obj.InDevicePtr);
            OutDevicePtr = std::move(obj.OutDevicePtr);
            return *this;
        }

        void cleanup() {
          if(queue){
            std::cout << "Plan: clean\n";
            CHECK_CL(clReleaseCommandQueue( queue ));
            queue = 0;
            if(handle){
              CHECK_CL(clfftDestroyPlan(&handle));
              handle=0;
            }
          }
        }
        ~Plan(){
          cleanup();
        }
  };

}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
