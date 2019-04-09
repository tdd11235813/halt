/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

namespace LiFFT {
namespace libraries {
namespace cuFFT {

    template< typename T_In, typename T_Out, class T_Deleter >
    struct Plan
    {
    private:
        template<typename T>
        struct Deleter
        {
            void
            operator()(T* ptr)
            {
                T_Deleter().free(ptr);
            }
        };
        Plan(const Plan&) = delete;
        Plan& operator=(const Plan&) = delete;
    public:
        cufftHandle handle;
        std::unique_ptr<T_In, Deleter<T_In>> InDevicePtr;
        std::unique_ptr<T_Out, Deleter<T_Out>> OutDevicePtr;

        Plan() = default;
        Plan(Plan&& obj): handle(obj.handle), InDevicePtr(std::move(obj.InDevicePtr)), OutDevicePtr(std::move(obj.OutDevicePtr))
        {
            obj.handle = 0;
        }

        Plan& operator=(Plan&& obj)
        {
            if(this!=&obj)
                return *this;
            handle = obj.handle; obj.handle = 0;
            InDevicePtr = std::move(obj.InDevicePtr);
            OutDevicePtr = std::move(obj.OutDevicePtr);
            return *this;
        }

        ~Plan(){
            if(handle)
                cufftDestroy(handle);
        }
  };

}  // namespace cuFFT
}  // namespace libraries
}  // namespace LiFFT
