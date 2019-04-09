/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/types/Vec.hpp"
#include "libLiFFT/traits/IdentityAccessor.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/accessors/ArrayAccessor.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/types/Range.hpp"
#include "libLiFFT/traits/IntegralType.hpp"
#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/IsStrided.hpp"
#include "libLiFFT/traits/IsAoS.hpp"

namespace LiFFT {
namespace types {

    /**
     * Provides a view on a container
     * Outside users will see it like it has a specific extent which is only a part of the real extents
     * The view is specified by an offset (in each dimension) and the extents (in each dimension)
     */
    template<
        class T_Base,
        typename T_HasInstance,
        class T_BaseAccessor = traits::IdentityAccessor_t<T_Base>
    >
    class View
    {
        using Base = T_Base;
        static constexpr bool hasInstance = T_HasInstance::value;
        using BaseAccessor = T_BaseAccessor;

        using RefType = typename std::add_lvalue_reference<Base>::type;
        using InstanceType = std::conditional_t< hasInstance, Base, RefType >;
        using ParamType = typename std::conditional_t< hasInstance, std::add_rvalue_reference<Base>, std::add_lvalue_reference<Base> >::type;

    public:
        static constexpr unsigned numDims = traits::NumDims<Base>::value;
        using Extents = Vec<numDims>;
        using IdentityAccessor = accessors::ArrayAccessor<true>;

    private:
        InstanceType m_base;
        BaseAccessor m_acc;
        Extents m_offsets, m_extents;

    public:

        /**
         * Creates a view with offsets and extents
         * Validation on those is performed at runtime
         *
         * @param base Base container
         * @param offsets Offsets for each dimension
         * @param extents New extents
         * @param acc Accessor to access the base class
         */
        View(ParamType base, const Extents& offsets, const Extents& extents, const BaseAccessor& acc = BaseAccessor()):
            m_base(static_cast<ParamType>(base)), m_acc(acc), m_offsets(offsets), m_extents(extents)
        {
            policies::GetExtents<Base> bExtents(m_base);
            for(unsigned i=0; i<numDims; ++i)
            {
                if(extents[i] > bExtents[i])
                    throw std::runtime_error("Invalid extents");
                if(offsets[i] + extents[i] > bExtents[i])
                    throw std::runtime_error("Invalid offset or extent");
            }
        }

        template<typename T_Idx>
        std::result_of_t< BaseAccessor(const T_Idx&, Base&) >
        operator()(T_Idx idx)
        {
            for(unsigned i=0; i<numDims; ++i)
                idx[i]+=m_offsets[i];
            return m_acc(idx, m_base);
        }

        template<typename T_Idx>
        std::result_of_t< BaseAccessor(const T_Idx&, const Base&) >
        operator()(T_Idx idx) const
        {
            for(unsigned i=0; i<numDims; ++i)
                idx[i]+=m_offsets[i];
            const Base& cBase = const_cast<const Base&>(m_base);
            return m_acc(idx, cBase);
        }

        /**
         * Returns a reference to the base class
         * @return Reference to base data
         */
        RefType
        getBase()
        {
            return m_base;
        }

        size_t
        getMemSize() const
        {
            return traits::getMemSize(m_base);
        }

        const Extents&
        getExtents() const
        {
            return m_extents;
        }
    };

    template<
            class T_Base,
            class T_BaseAccessor = traits::IdentityAccessor_t<std::remove_reference_t<T_Base>>,
            class T_Range
        >
    View< std::remove_reference_t<T_Base>, negate< std::is_lvalue_reference<T_Base> >, T_BaseAccessor >
    makeView(T_Base&& base, const T_Range& range, const T_BaseAccessor& acc = T_BaseAccessor())
    {
        using Base = std::remove_cv_t<std::remove_reference_t<T_Base>>;
        return View< std::remove_reference_t<T_Base>, negate< std::is_lvalue_reference<T_Base> >, T_BaseAccessor >(
                std::forward<T_Base>(base),
                GetRangeOffset<T_Range, Base>::get(range),
                GetRangeExtents<T_Range, Base>::get(range, base),
                acc);
    }
}  // namespace types

namespace traits {

    template< class T_Base, class... T >
    struct IntegralType< types::View< T_Base, T... > >: IntegralType<T_Base>{};

    template< class T_Base, class... T >
    struct IsComplex< types::View< T_Base, T... > >: IsComplex<T_Base>{};

    template< class T_Base, class... T >
    struct IsStrided< types::View< T_Base, T... > >: IsStrided<T_Base>{};

    template< class T_Base, class... T >
    struct IsAoS< types::View< T_Base, T... > >: IsAoS<T_Base>{};

}  // namespace traits

}  // namespace LiFFT
