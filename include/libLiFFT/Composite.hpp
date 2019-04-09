/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {

    /**
     * Functor that is composed of multiple functors
     * The last functor is applied first, then the next is applied to the result and so on
     */
    template < class... T_Funcs >
    struct Composite;

    template< class T_Func >
    struct Composite< T_Func >: T_Func{};

    template< class T_Func, class... T_Funcs >
    struct Composite< T_Func, T_Funcs... >{

        T_Func f1;
        Composite< T_Funcs... > f2;

        template< typename... T >
        auto
        operator()(T&&... args)
        -> decltype( f1( f2( std::forward<T>(args)... ) ) )
        {
            return f1( f2( std::forward<T>(args)... ) );
        }
    };


}  // namespace LiFFT
