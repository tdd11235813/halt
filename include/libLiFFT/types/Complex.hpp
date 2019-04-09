/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsBinaryCompatible.hpp"
#include "libLiFFT/types/Real.hpp"
#include "libLiFFT/c++14_types.hpp"

namespace LiFFT {
namespace types {

    template< typename T=double, bool T_isConst = false >
    struct ComplexRef;

    /**
     * Type used to store complex values (real and imaginary part)
     * Uses the template parameter to choose the memory type (precision)
     */
    template< typename T=double >
    struct Complex
    {
        using type = T;
        static constexpr bool isComplex = true;
        Real<T> real, imag;

        Complex(){}
        template< typename U, typename = std::enable_if_t< std::is_integral<U>::value || std::is_floating_point<U>::value > >
        Complex(U realIn): real(realIn), imag(0){}
        Complex(const Complex&) = default;
        template< typename U, typename V >
        Complex(U&& realIn, V&& imagIn): real(std::forward<U>(realIn)), imag(std::forward<V>(imagIn)){}
        template< typename U, bool T_isConst >
        Complex(const ComplexRef<U, T_isConst>& ref): real(ref.real), imag(ref.imag){}

        template< typename U >
        std::enable_if_t< traits::IsBinaryCompatible< U, Complex>::value, Complex >
        operator=(const U& other)
        {
            auto m_other = reinterpret_cast<const Complex&>(other);
            return *this = m_other;
        }

        Complex& operator=(const Complex&) = default;
    };

    /**
     * Generic reference to a complex value
     * Can be used with either AoS or SoA
     *
     * \tparam T Base type to use (float, double) [double]
     * \tparam T_isConst True if this is a const reference [false]
     */
    template< typename T, bool T_isConst >
    struct ComplexRef
    {
        using type = T;
        static constexpr bool isConst = T_isConst;
        static constexpr bool isComplex = true;
        using Real_t = std::conditional_t< isConst, const Real<T>, Real<T> >;
        using Complex_t = std::conditional_t< isConst, const Complex<T>, Complex<T> >;

        Real_t &real;
        Real_t &imag;

        explicit ComplexRef(Complex_t& value): real(value.real), imag(value.imag){}
        ComplexRef(Real_t& r, Real_t& i): real(r), imag(i){}

        ComplexRef&
        operator=(const Complex_t& c){
            real = c.real;
            imag = c.imag;
            return *this;
        }
    };

}  // namespace types
}  // namespace LiFFT

template<typename T>
std::ostream& operator<< (std::ostream& stream, LiFFT::types::Complex<T> val){
    stream << val.real << " " << val.imag;
    return stream;
}

template<typename T, bool T_isConst>
std::ostream& operator<< (std::ostream& stream, LiFFT::types::ComplexRef<T, T_isConst> val){
    stream << LiFFT::types::Complex<T>(val);
    return stream;
}

namespace std {

    template< typename T, bool T_isConst >
    struct is_const< LiFFT::types::ComplexRef<T, T_isConst> >: std::integral_constant< bool, T_isConst >{};

    template< typename T, bool T_isConst >
    struct is_lvalue_reference< LiFFT::types::ComplexRef<T, T_isConst> >: std::true_type{};

}  // namespace std
