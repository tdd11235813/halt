/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace std {

    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;

    template< bool B, class T, class U >
    using conditional_t = typename conditional< B, T, U >::type;

    template< class T >
    using result_of_t = typename result_of<T>::type;

    template< class T >
    using decay_t = typename decay<T>::type;

    template< class T >
    using remove_pointer_t = typename remove_pointer<T>::type;

    template< class T >
    using remove_reference_t = typename remove_reference<T>::type;

    template< class T >
    using remove_const_t = typename remove_const<T>::type;

    template< class T >
    using remove_cv_t = typename remove_cv<T>::type;

    template< class T >
    using make_unsigned_t = typename make_unsigned<T>::type;

    template< class T >
    using add_cv_t       = typename add_cv<T>::type;

    template< class T >
    using add_const_t    = typename add_const<T>::type;

    template< class T >
    using add_volatile_t = typename add_volatile<T>::type;

    template< typename T >
    struct integral_type
    {
        using type = T;
    };

    template< class T >
    constexpr const T& cmin( const T& a, const T& b )
    {
        return (b < a) ? b : a;
    }

}  // namespace std
