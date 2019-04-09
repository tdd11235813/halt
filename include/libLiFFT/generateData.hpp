/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/traits/NumDims.hpp"
#include "libLiFFT/traits/IdentityAccessor.hpp"
#include "libLiFFT/policies/Copy.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/mem/DataContainer.hpp"

namespace LiFFT {

    struct GeneratorAccessor
    {
        template< class T_Idx, class T_Data >
        std::result_of_t<std::remove_pointer_t<T_Data>(T_Idx)>
        operator()(T_Idx&& idx, const T_Data& data)
        {
            return (*data)(std::forward<T_Idx>(idx));
        }
    };

    template< typename T, class Generator, class T_Accessor = LiFFT::traits::IdentityAccessor_t<T> >
    void generateData(T& data, const Generator& generator, const T_Accessor& acc = T_Accessor()){
        static constexpr unsigned numDims = LiFFT::traits::NumDims<T>::value;
        types::Vec<numDims> extents;
        policies::GetExtents<T> extentsData(data);
        for(unsigned i=0; i<numDims; i++)
            extents[i] = extentsData[i];
        mem::DataContainer< numDims, const Generator*, GeneratorAccessor, false > genContainer(&generator, extents);
        policies::makeCopy(LiFFT::traits::IdentityAccessor_t< decltype(genContainer) >(), acc)(genContainer, data);
    }

    namespace generators {

        template<typename T, typename U>
        typename std::common_type<T, U>::type absDiff(T a, U b) {
          return a > b ? a - b : b - a;
        }

        template<typename T>
        struct Spalt{
            const size_t m_size, m_middle;
            Spalt(size_t size, size_t middle):m_size(size), m_middle(middle){}

            template< class T_Idx >
            T
            operator()(T_Idx&& idx) const{
                static constexpr unsigned numDims = traits::NumDims<T_Idx>::value;

                return (absDiff(idx[numDims - 1], m_middle) <= m_size) ? 1 : 0;
            }
        };

        template<typename T>
        struct Cosinus{
            const size_t m_middle;
            const T m_factor;
            Cosinus(size_t period, size_t middle):m_middle(middle), m_factor(2 * M_PI / period){}

            template< class T_Idx >
            T
            operator()(T_Idx&& idx) const{
                static constexpr unsigned numDims = traits::NumDims<T_Idx>::value;
                static_assert(numDims >= 2, "Only >=2D data supported");

                auto dist = std::sqrt(
                        std::pow(absDiff(idx[numDims - 1], m_middle), 2)+
                        std::pow(absDiff(idx[numDims - 2], m_middle), 2));
                return std::cos( m_factor * dist );
            }
        };

        template<typename T>
        struct Rect{
            const size_t m_sizeX, m_sizeY, m_middleX, m_middleY;
            Rect(size_t sizeX, size_t middleX): Rect(sizeX, middleX, sizeX, middleX){}
            Rect(size_t sizeX, size_t middleX, size_t sizeY, size_t middleY):m_sizeX(sizeX), m_sizeY(sizeY), m_middleX(middleX), m_middleY(middleY){}

            template< class T_Idx >
            T
            operator()(T_Idx&& idx) const{
                static constexpr unsigned numDims = traits::NumDims<T_Idx>::value;
                static_assert(numDims >= 2, "Only >=2D data supported");

                return (absDiff(idx[numDims - 1], m_middleX) <= m_sizeX &&
                        absDiff(idx[numDims - 2], m_middleY) <= m_sizeY) ? 1 : 0;
            }
        };

        template<typename T>
        struct Circle{
            const size_t m_size, m_middle;
            Circle(size_t size, size_t middle):m_size(size), m_middle(middle){}

            template< class T_Idx >
            T
            operator()(T_Idx&& idx) const{
                static constexpr unsigned numDims = traits::NumDims<T_Idx>::value;
                static_assert(numDims >= 2, "Only >=2D data supported");

                return (std::pow(absDiff(idx[numDims - 1], m_middle), 2) +
                        std::pow(absDiff(idx[numDims - 2], m_middle), 2) <= m_size*m_size) ? 1 : 0;
            }
        };

        template<typename T>
        struct SetToConst{
            const T m_val;
            SetToConst(T val): m_val(val){}

            template< class T_Idx >
            T
            operator()(T_Idx&&) const{
                return m_val;
            }
        };

    }  // namespace generators

}  // namespace LiFFT
