/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/IntegralType.hpp"
#include <type_traits>

namespace LiFFT {
namespace policies {

    namespace detail
    {
        template< class T, bool T_isComplex = traits::IsComplex<T>::value >
        struct CalcIntensityImpl
        {
            using RetType = typename traits::IntegralType<T>::type;

            RetType
            operator()(const T& value)
            {
                RetType r = value.real;
                RetType i = value.imag;
                return r*r+i*i;
            }
        };

        template< class T >
        struct CalcIntensityImpl< T, false >
        {
            using RetType = typename traits::IntegralType<T>::type;

            RetType
            operator()(const RetType value)
            {
                return value*value;
            }
        };


        template< class T_Accessor >
        struct CalcIntensity
        {
            template< typename T_Data >
            auto
            operator()(const T_Data& data, unsigned idx)
            -> typename traits::IntegralType< std::result_of_t< T_Accessor(unsigned, const T_Data&) > >::type
            {
                T_Accessor accessor;
                CalcIntensityImpl< std::result_of_t< T_Accessor(unsigned, const T_Data&) > > intensity;
                return intensity(accessor(idx, data));
            }
        };

    }//namespace detail

    template< class T_Accessor >
    struct CalcIntensity : public detail::CalcIntensity<T_Accessor>{};

}  // namespace policies
}  // namespace LiFFT
