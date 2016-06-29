#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_
#include "../clfft_helper.hpp"
#include <clFFT.h>
#include <iostream>

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

  /**
   * Base class with OpenCL context and device pointer
   */
  struct Context {
    cl_context ctx = 0;
    cl_device_id dev = 0;
  };

  struct ClFFTContext {
    bool initialized = false;
    /**
     * Setting up clfft context
     */
    void init() {
      clfftSetupData fftSetup;
      CHECK_CL( clfftInitSetupData(&fftSetup) );
      CHECK_CL( clfftSetup(&fftSetup) );
      initialized = true;
    }
    /**
     * Destroying clfft context if it was initialized
     */
    ~ClFFTContext() {
      if(initialized)
        CHECK_CL(clfftTeardown());
    }
  };

  /**
   * Provides OpenCL context and device after instantiation, where clfft is
   * initialized as well.
   *
   * If no GPU is found, CPUs are used by OpenCL. For own OpenCL context and
   * device, use the ContextWrapper class.
   */
  class ContextLocal : private Context {
  public:
    ClFFTContext clfft;
    cl_context context()
    {
      return ctx;
    }
    cl_device_id device()
    {
      return dev;
    }
    ContextLocal()
    {
      cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
      cl_platform_id platform = 0;
      cl_int err = 0;
      findClDevice(CL_DEVICE_TYPE_GPU, &platform, &dev);
      props[1] = (cl_context_properties)platform;
      ctx = clCreateContext(props, 1, &dev, NULL, NULL, &err);
      clfft.init();
      CHECK_CL(err);
    }
    ~ContextLocal()
    {
      clReleaseContext( ctx );
      ctx = 0;
    }
  };

/**
 * A singleton by using a single ContextLocal instance.
 */
  class ContextGlobal
  {
  private:
    static ContextLocal* getInstance()
    {
      static ContextLocal instance;
      return &instance;
    }
  public:
    cl_context context()
    {
      return getInstance()->context();
    }
    cl_device_id device()
    {
      return getInstance()->device();
    }
  };

/**
 * Wraps OpenCL context and device given by user.
 */
  class ContextWrapper
  {
  public:
    static void wrap(cl_context c, cl_device_id d, bool create_clfft_context)
    {
      static ClFFTContext clfft;
      if(create_clfft_context)
        clfft.init();
      context_static(c);
      device_static(d);
    }
    static cl_context context_static(cl_context c=0) {
      static cl_context ctx; // init
      if(c!=0) ctx=c; // re-assign
      return ctx;
    }
    static cl_device_id device_static(cl_device_id d=0) {
      static cl_device_id dev; // init
      if(d!=0) dev=d; // re-assign
      return dev;
    }
    cl_context context()
    {
      return context_static();
    }
    cl_device_id device()
    {
      return device_static();
    }
  };
}
}
}
}

#endif /* CONTEXT_HPP_ */
