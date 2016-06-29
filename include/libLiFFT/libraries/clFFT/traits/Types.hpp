#pragma once
#include <clFFT.h>
namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace traits{
    template< int T_NumDims >
    struct Dims: std::integral_constant<clfftDim, CLFFT_1D>{};
    template<>
    struct Dims<1>: std::integral_constant<clfftDim, CLFFT_1D>{};
    template<>
    struct Dims<2>: std::integral_constant<clfftDim, CLFFT_2D>{};
    template<>
    struct Dims<3>: std::integral_constant<clfftDim, CLFFT_3D>{};
}  // namespace traits
}  // namespace clFFT
}  // namespace libraries
}  // namespace LiFFT
