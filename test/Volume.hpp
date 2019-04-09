/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/IntegralType.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/traits/IsComplex.hpp"
#include "libLiFFT/traits/IsStrided.hpp"
#include "libLiFFT/traits/IsAoS.hpp"
#include "libLiFFT/policies/GetExtents.hpp"

template< typename T = double >
class Volume{
    const size_t m_xDim, m_yDim, m_zDim;
    T* m_data;
    bool m_isOwned;
    Volume(const Volume&) = delete;
    Volume& operator=(const Volume&) = delete;
public:
    using value_type = T;
    using Ref = T&;
    using ConstRef = const T&;

    Volume(size_t xDimIn, size_t yDimIn = 1, size_t zDimIn = 1): m_xDim(xDimIn), m_yDim(yDimIn), m_zDim(zDimIn){
        m_data = new T[m_xDim*m_yDim*m_zDim];
        m_isOwned = true;
    }
    Volume(size_t xDimIn, size_t yDimIn, size_t zDimIn, T* dataIn): m_xDim(xDimIn), m_yDim(yDimIn), m_zDim(zDimIn){
        m_data = dataIn;
        m_isOwned = false;
    }

    Volume(Volume&& obj): m_xDim(obj.m_xDim), m_yDim(obj.m_yDim), m_zDim(obj.m_zDim), m_data(obj.m_data), m_isOwned(obj.m_isOwned){
        if(m_isOwned)
            obj.m_data = nullptr;
    }

    Volume& operator=(Volume&& obj){
        if(this == &obj)
            return *this;
        if(m_isOwned)
            delete[] m_data;
        m_data = nullptr;
        m_xDim = obj.m_xDim;
        m_yDim = obj.m_yDim;
        m_zDim = obj.m_zDim;
        m_data = obj.m_data;
        m_isOwned = obj.m_isOwned;
        if(m_isOwned)
            obj.m_data = nullptr;
        return *this;
    }

    ~Volume(){
        if(m_isOwned)
            delete[] m_data;
    }
    T*
    data(){
        return m_data;
    }
    Ref
    operator()(size_t x, size_t y=0, size_t z=0){
        return m_data[(z*m_yDim + y)*m_xDim + x];
    }
    ConstRef
    operator()(size_t x, size_t y=0, size_t z=0) const{
        return m_data[(z*m_yDim + y)*m_xDim + x];
    }

    size_t xDim() const{ return m_xDim; }
    size_t yDim() const{ return m_yDim; }
    size_t zDim() const{ return m_zDim; }
};

namespace LiFFT {
    namespace traits {

        template<typename T>
        struct IntegralTypeImpl< Volume<T> >: IntegralType< T >{}; // or define "type = T" in Volume itself

        template<typename T>
        struct NumDims< Volume<T> >: std::integral_constant< unsigned, 3 >{};

        template<typename T>
        struct IsComplex< Volume<T> >: IsComplex<T>{};

        template<typename T>
        struct IsStrided< Volume<T> >: std::false_type{};

        template<typename T>
        struct IsAoS< Volume<T> >: std::true_type{};

    }  // namespace traits

    namespace policies {

        template< class T_Data >
        struct GetVolumeExtents: boost::noncopyable
        {
            using Data = T_Data;

            GetVolumeExtents(const Data& data): m_data(data){}

            unsigned operator[](unsigned dimIdx) const
            {
                switch(dimIdx){
                case 0:
                    return m_data.zDim();
                case 1:
                    return m_data.yDim();
                case 2:
                    return m_data.xDim();
                }
                throw std::logic_error("Invalid dimension");
            }
        protected:
            const Data& m_data;
        };

        template<typename T>
        struct GetExtentsImpl< Volume<T> >: GetVolumeExtents< Volume<T> >
        {
            using Parent = GetVolumeExtents< Volume<T> >;
            using Parent::Parent;
        };

    }  // namespace policies
}  // namespace LiFFT

