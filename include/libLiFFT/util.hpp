/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/c++14_types.hpp"
#include <type_traits>

namespace LiFFT {

    template< unsigned T_value >
    using UnsignedConst = std::integral_constant<unsigned, T_value>;

    template< bool T_value >
    using BoolConst = std::integral_constant<bool, T_value>;

    template <typename Assertion>
    struct AssertValue
    {
        static bool const value = Assertion::value;
        static_assert(value, "Assertion failed <see above for more information>");
    };

    /**
     * Negates a BoolConst-like type
     */
    template< typename T >
    struct negate: BoolConst< !T::value >{};

    template <class T, class M>
    M get_member_type(M T:: *);

    #define GET_TYPE_OF(mem) decltype(get_member_type(&mem))

    /**
     * Adds const to the value of a type
     * int* -> const int*
     * int*& -> const int*&
     */
    template< typename T >
    struct AddConstVal: std::add_const<T>{};

    template< typename T >
    using AddConstVal_t = typename AddConstVal<T>::type;

    template< typename T >
    struct AddConstVal<T&>
    {
        using type = AddConstVal_t<T>&;
    };

    template< typename T >
    struct AddConstVal<T*>
    {
        using type = AddConstVal_t<T>*;
    };

    template< typename T >
    struct AddConstVal<T const>
    {
        using type = AddConstVal_t<T> const;
    };

    template< typename T >
    struct AddConstVal<T volatile>
    {
        using type = AddConstVal_t<T> volatile;
    };

    template< typename T >
    struct AddConstVal<T []>
    {
        using type = AddConstVal_t<T> [];
    };

    template< typename T, std::size_t N >
    struct AddConstVal<T [N]>
    {
        using type = AddConstVal_t<T> [N];
    };

    /**
     * Returns a type with correct const modifier added
     * That is, if T_Base is a type and T_Mem is a member then the value of T_Mem is const if, and only if, T_Base is const
     */
    template< class T_Base, typename T_Mem >
    struct GetConstCorrect
    {
        using Base = T_Base;
        using Mem = T_Mem;

        static constexpr bool isConst = std::is_const<Base>::value;
        using type = std::conditional_t< isConst,
                AddConstVal_t<Mem>,
                Mem
                >;
    };

    template< class T_Base, typename T_Mem >
    using GetConstCorrect_t = typename GetConstCorrect< T_Base, T_Mem >::type;

    template< class T_Base, typename T_Mem >
    GetConstCorrect_t< T_Base, T_Mem& >
    getConstCorrect(T_Mem& mem)
    {
        return const_cast<GetConstCorrect_t< T_Base, T_Mem& >>(mem);
    }

}  // namespace LiFFT
