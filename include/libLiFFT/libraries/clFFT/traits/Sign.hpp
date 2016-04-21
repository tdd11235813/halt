#pragma once

#include <clFFT.h>

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace traits{

        /**
         * Defines the sign for the given FFT type (forward/backward)
         */
        template< bool T_isFwd=true >
        struct Sign: std::integral_constant<clfftDirection, CLFFT_FORWARD>{};

        template<>
        struct Sign<false>: std::integral_constant<clfftDirection, CLFFT_BACKWARD>{};

}  // namespace traits
}  // namespace clFFT
}  // namespace libraries
}  // namespace foobar
