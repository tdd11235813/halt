/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <boost/utility.hpp>
#include "libLiFFT/traits/IsBinaryCompatible.hpp"
#include "libLiFFT/c++14_types.hpp"
#include "libLiFFT/void_t.hpp"

namespace LiFFT {
namespace policies {

    /**
     * Converts one pointer type into another
     */
    template< typename T_Src, typename T_Dest = T_Src >
    struct Ptr2Ptr;

    template< typename T_Src, typename T_Dest >
    struct Ptr2Ptr< T_Src*, T_Dest* >
    {
        using Src = T_Src;
        using Dest = T_Dest;

        Dest*
        operator()(Src* data) const
        {
            return reinterpret_cast<Dest*>(data);
        }
    };

    /**
     * Implementation of the SafePtrCast, specialize only this!
     * If it errors, then no safe ptr cast from T_Src to T_Dest pointer was possible.
     * If the conversion is safe, define the PtrsAreBinaryCompatible-trait specialize this!"
     */
    template< typename T_Src, typename T_Dest, typename T_SFINAE = void >
    struct SafePtrCast_Impl;

    template< typename T_Src, typename T_Dest >
     struct SafePtrCast_Impl<
         T_Src,
         T_Dest,
         std::enable_if_t<traits::IsBinaryCompatible< std::remove_pointer_t<T_Src>, std::remove_pointer_t<T_Dest> >::value>
     >: Ptr2Ptr< T_Src, T_Dest >{};

    template< typename T_Src, typename T_Dest, typename T_SFINAE = void >
    struct SafePtrCastExist: std::false_type{};

    template< typename T_Src, typename T_Dest >
    struct SafePtrCastExist<
        T_Src,
        T_Dest,
        void_t<
            std::result_of_t<
                SafePtrCast_Impl< T_Src, T_Dest >(T_Src)
            >
        >
    >: std::true_type{};

    template< typename T_Src, typename T_Dest >
    struct SafePtrCast_Impl< std::pair< T_Src*, T_Src* >, std::pair< T_Dest*, T_Dest* > >
    {
        using Base = SafePtrCast_Impl< T_Src*, T_Dest* >;

        std::pair< T_Dest*, T_Dest* >
        operator()(const std::pair< T_Src*, T_Src* >& data) const
        {
            return std::make_pair(Base()(data.first), Base()(data.second));
        }
    };

    /**
     * Converts a pointer to another pointer in a safe way
     * If it errors, then no safe ptr cast from T_Src to T_Dest pointer was possible.
     * If the conversion is safe, define the PtrsAreBinaryCompatible-trait specialize this!"
     */
    template< typename T_Src, typename T_Dest, typename T_SFINAE = void >
    struct SafePtrCast: SafePtrCast_Impl< T_Src, T_Dest >{};

    template< typename T >
    struct SafePtrCast<T, T>
    {
        T&
        operator()(T& data) const
        {
            return data;
        }
    };

    template< typename T_Src, typename T_Dest >
    struct SafePtrCast<
        T_Src,
        T_Dest,
        std::enable_if_t<
            !std::is_same< T_Src, T_Dest >::value &&
            SafePtrCastExist< T_Src, float* >::value &&
            SafePtrCastExist< float*, T_Dest >::value
        >
    >{
        SafePtrCast_Impl< T_Src, float* > conv1;
        SafePtrCast_Impl< float*, T_Dest > conv2;

        T_Dest
        operator()(T_Src data) const
        {
            return conv2(conv1(data));
        }
    };

    template< typename T_Src, typename T_Dest >
    struct SafePtrCast<
        T_Src,
        T_Dest,
        std::enable_if_t<
            !std::is_same<T_Src, T_Dest>::value &&
            SafePtrCastExist< T_Src, double* >::value &&
            SafePtrCastExist< double*, T_Dest >::value
        >
    >{
        SafePtrCast_Impl< T_Src, double* > conv1;
        SafePtrCast_Impl< double*, T_Dest > conv2;

        T_Dest
        operator()(T_Src data) const
        {
            return conv2(conv1(data));
        }
    };

    /**
     * Safely casts a pointer to another one
     *
     * @param data pointer to convert
     * @return converted pointer
     */
    template< typename T_Dest, typename T_Src>
    T_Dest
    safe_ptr_cast(T_Src* data)
    {
        return SafePtrCast< T_Src*, T_Dest >()(data);
    }

    /**
     * Safely casts a pointer to another one
     *
     * @param data pointer to convert
     * @return converted pointer
     */
    template< typename T_Dest, typename T_Src>
    T_Dest
    safe_ptr_cast(std::pair<T_Src*, T_Src*> data)
    {
        return SafePtrCast< std::pair<T_Src*, T_Src*>, T_Dest >()(data);
    }

}  // namespace policies
}  // namespace LiFFT
