#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_
#include "../clfft_helper.hpp"
#include <clFFT.h>
#include <iostream>

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace policies {

  class Context {
  public:
    cl_context ctx = 0;
    cl_device_id dev = 0;
  };

  class ContextLocal : private Context {
  public:
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
      CHECK_CL(err);
      /* Setup clFFT. */
      clfftSetupData fftSetup;
      CHECK_CL( clfftInitSetupData(&fftSetup) );
      CHECK_CL( clfftSetup(&fftSetup) );
    }
    ~ContextLocal()
    {
      CHECK_CL(clfftTeardown( ));
      clReleaseContext( ctx );
      ctx = 0;
    }
  };


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

  class ContextWrapper
  {
    static Context instance;
  public:
    static void wrap(cl_context c, cl_device_id d)
    {
      instance.ctx = c;
      instance.dev = d;
    }
    cl_context context()
    {
      return instance.ctx;
    }
    cl_device_id device()
    {
      return instance.dev;
    }
  };

}
}
}
}

#endif /* CONTEXT_HPP_ */
