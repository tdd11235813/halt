#pragma once

#include <clFFT.h>

namespace LiFFT {
namespace libraries {
namespace clFFT {
namespace traits{

  template< typename T_Precision >
  struct FFTPrecision: std::integral_constant< clfftPrecision, CLFFT_SINGLE >{};
  template<>
  struct FFTPrecision<float>: std::integral_constant< clfftPrecision, CLFFT_SINGLE >{};
  template<>
  struct FFTPrecision<double>: std::integral_constant< clfftPrecision, CLFFT_DOUBLE >{};

  template< bool T_IsComplex, bool T_IsOtherComplex >
  struct FFTLayout: std::integral_constant< clfftLayout, CLFFT_COMPLEX_INTERLEAVED >{};
  template<>
  struct FFTLayout<true,true>: std::integral_constant< clfftLayout, CLFFT_COMPLEX_INTERLEAVED >{};
  /* R2C/C2R only uses Hermitian layout */
  template<>
  struct FFTLayout<true,false>: std::integral_constant< clfftLayout,  CLFFT_HERMITIAN_INTERLEAVED  >{};
  template<>
  struct FFTLayout<false,true>: std::integral_constant< clfftLayout, CLFFT_REAL >{};

  template< bool T_isInplace >
  struct FFTInplace: std::integral_constant< clfftResultLocation, CLFFT_INPLACE >{};
  template<>
  struct FFTInplace<true>: std::integral_constant< clfftResultLocation, CLFFT_INPLACE >{};
  template<>
  struct FFTInplace<false>: std::integral_constant< clfftResultLocation, CLFFT_OUTOFPLACE >{};

}  // namespace traits
}  // namespace clFFT
}  // namespace libraries
}  // namespace LiFFT
