/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Volume.hpp"
#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/policies/GetExtents.hpp"

template<typename T>
class VolumeAdapter{
protected:
    T& m_obj;
public:
    using value_type = typename T::value_type;
    static constexpr unsigned numDims = 3;

    VolumeAdapter(T& obj):m_obj(obj){}

    size_t xDim() const{ return m_obj.xDim(); }
    size_t yDim() const{ return m_obj.yDim(); }
    size_t zDim() const{ return m_obj.zDim(); }
};

template<typename T>
class SymetricAdapter: public Volume<T>{
    const size_t realXDim;
public:
    using parent_type = Volume<T>;
    static constexpr unsigned numDims = 3;

    SymetricAdapter(size_t xDim, size_t yDim, size_t zDim): parent_type(xDim/2+1, yDim, zDim), realXDim(xDim){}
    SymetricAdapter(size_t xDim, size_t yDim, size_t zDim, T* data): parent_type(xDim/2+1, yDim, zDim, data), realXDim(xDim){}
    SymetricAdapter(size_t realXDim, parent_type& data): parent_type(data.xDim(), data.yDim(), data.zDim(), data.data()), realXDim(realXDim){}

    T& operator()(size_t x, size_t y=0, size_t z=0){
        x = (x>=parent_type::xDim()) ? realXDim-x : x;
        return parent_type::operator ()(x, y, z);
    }
    const T& operator()(size_t x, size_t y=0, size_t z=0) const{
        x = (x>=parent_type::xDim()) ? realXDim-x : x;
        return parent_type::operator ()(x, y, z);
    }
    size_t xDim() const{ return realXDim; }
};

template<typename T>
class TransposeAdapter: public VolumeAdapter<T>{
public:
    using value_type = typename T::value_type;
    TransposeAdapter(T& obj):VolumeAdapter<T>(obj){}
    value_type& operator()(size_t x, size_t y=0, size_t z=0){
        x = (x>=this->xDim()/2) ? x-this->xDim()/2 : x+this->xDim()/2;
        y = (y>=this->yDim()/2) ? y-this->yDim()/2 : y+this->yDim()/2;
        z = (z>=this->zDim()/2) ? z-this->zDim()/2 : z+this->zDim()/2;
        return this->m_obj(x, y, z);
    }
    const value_type& operator()(size_t x, size_t y=0, size_t z=0) const{
        x = (x>=this->xDim()/2) ? x-this->xDim()/2 : x+this->xDim()/2;
        y = (y>=this->yDim()/2) ? y-this->yDim()/2 : y+this->yDim()/2;
        z = (z>=this->zDim()/2) ? z-this->zDim()/2 : z+this->zDim()/2;
        return this->m_obj(x, y, z);
    }
};

template<typename T> inline
TransposeAdapter<T>
makeTransposeAdapter(T& obj){
    return TransposeAdapter<T>(obj);
}

namespace LiFFT{
    namespace policies {

        template<typename T>
        struct GetExtentsImpl< SymetricAdapter<T> >: GetVolumeExtents< SymetricAdapter<T> >
        {
            using Parent = GetVolumeExtents< SymetricAdapter<T> >;
            using Parent::Parent;
        };

        template<typename T>
        struct GetExtentsImpl< TransposeAdapter<T> >: GetVolumeExtents< TransposeAdapter<T> >
        {
            using Parent = GetVolumeExtents< TransposeAdapter<T> >;
            using Parent::Parent;
        };

    }  // namespace policies
}  // namespace LiFFT
