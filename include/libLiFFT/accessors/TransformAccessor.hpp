/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace LiFFT {
namespace accessors {

    /**
     * Accessor that transforms all elements on access using the specified functor
     */
    template< class T_BaseAccessor, class T_Func >
    struct TransformAccessor
    {
    private:
        using BaseAccessor = T_BaseAccessor;
        using Func = T_Func;

        BaseAccessor m_acc;
        Func m_func;
    public:

        TransformAccessor(){}
        template< class T, class U >
        explicit TransformAccessor(T&& baseAccessor, U&& func = T_Func()): m_acc(std::forward<T>(baseAccessor)), m_func(std::forward<U>(func)){}

        template< class T_Index, class T_Data >
        auto
        operator()(const T_Index& idx, const T_Data& data) const
        -> decltype(m_func(m_acc(idx, data)))
        {
            return m_func(m_acc(idx, data));
        }
    };

    /**
     * Creates a transform accessor for the given function and accessor
     * This will transform all values returned by the base accessor with the function
     *
     * @param acc Base accessor
     * @param func Transform functor
     * @return TransformAccessor
     */
    template< class T_BaseAccessor, class T_Func >
    TransformAccessor< T_BaseAccessor, T_Func >
    makeTransformAccessor(T_BaseAccessor&& acc, T_Func&& func)
    {
        return TransformAccessor< T_BaseAccessor, T_Func >(std::forward<T_BaseAccessor>(acc), std::forward<T_Func>(func));
    }

    /**
     * Creates a transform accessor for the given function when applied to the given container using its default accessor
     * @param func Transform functor
     * @param      Container instance
     * @return TransformAccessor
     */
    template< class T, class T_Func >
    TransformAccessor< traits::IdentityAccessor_t<T>, T_Func >
    makeTransformAccessorFor(T_Func&& func, const T& = T())
    {
        return TransformAccessor< traits::IdentityAccessor_t<T>, T_Func >(traits::IdentityAccessor_t<T>(), std::forward<T_Func>(func));
    }

}  // namespace accessors
}  // namespace LiFFT
