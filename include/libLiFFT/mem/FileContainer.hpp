/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "libLiFFT/mem/RealValues.hpp"
#include "libLiFFT/mem/ComplexAoSValues.hpp"
#include "libLiFFT/mem/DataContainer.hpp"
#include "libLiFFT/traits/IsStrided.hpp"
#include "libLiFFT/traits/IntegralType.hpp"
#include "libLiFFT/policies/GetExtents.hpp"
#include "libLiFFT/accessors/DataContainerAccessor.hpp"
#include "libLiFFT/policies/GetNumElements.hpp"
#include "libLiFFT/policies/Copy.hpp"
#include "libLiFFT/c++14_types.hpp"

#include <boost/mpl/apply.hpp>

namespace bmpl = boost::mpl;

namespace LiFFT {
    namespace mem {

        struct FileContainerAccessor
        {
            accessors::DataContainerAccessor<> m_acc;

            template< class T_Index, class T_Data >
            auto
            operator()(T_Index&& idx, T_Data& data) const
            -> decltype(m_acc(idx, data.m_data))
            {
                return m_acc(idx, data.m_data);
            }
        };

        /**
         * A container that can load a file to an internal (contiguous) memory
         *
         * \tparam T_FileHandler File class. Must support open(string), isOpen(), close(), and a specialization for GetExtents
         * \tparam T_FileAccessor Either an Array- or StreamAccessor that should provide an operator([index,] TFileHandler&) which
         *          gets an element from the file
         * \tparam T_Accuracy The internal datatype used (float or double) [float]
         * \tparam T_isComplex Whether the values are complex [false]
         * \tparam T_numDims number of dimensions [Number of dimensions supported by the FileHandler]
         */
        template<
            typename T_FileHandler,
            typename T_FileAccessor,
            typename T_Accuracy = float,
            bool T_isComplex = false,
            unsigned T_numDims = traits::NumDims< T_FileHandler >::value
        >
        class FileContainer
        {
        public:
            using FileHandler = T_FileHandler;
            using FileAccessor = T_FileAccessor;
            using Accuracy = T_Accuracy;
            static constexpr bool isComplex = T_isComplex;
            static constexpr unsigned numDims = T_numDims;

            using IdentityAccessor = FileContainerAccessor;
            friend IdentityAccessor;
        private:
            using DataAccessor = accessors::DataContainerAccessor<>;
            using CopyPolicy = policies::Copy< FileAccessor, DataAccessor >;
            using ArrayType = std::conditional_t< isComplex, ComplexAoSValues<Accuracy>, RealValues<Accuracy> >;
            using ElementType = typename ArrayType::Value;
            using Ptr = ElementType*;

            using Data = DataContainer< numDims, ArrayType >;
            using ExtentsVec = decltype(std::declval<Data>().getExtents());

            FileHandler m_fileHandler;
            Data m_data;
            std::string m_filePath;
            bool m_gotData;

            void
            allocData()
            {
                assert(m_fileHandler.isOpen());
                if(m_data.getData())
                    return;
                policies::GetExtents< FileHandler > fileExtents(m_fileHandler);
                typename Data::IdxType extents;
                for(unsigned i=0; i<numDims; ++i)
                    extents[i] = fileExtents[i];
                m_data.allocData(extents);
            }

            void freeData()
            {
                m_data.freeData();
            }

        public:
            FileContainer(): FileContainer(""){}
            explicit FileContainer(const std::string& filePath)
            {
                setFilePath(filePath);
            }

            ~FileContainer()
            {
                setFilePath("");
            }

            void setFilePath(const std::string& filePath)
            {
                if(!m_filePath.empty())
                    m_fileHandler.close();
                m_filePath = filePath;
                freeData();
                m_gotData = false;
                if(!filePath.empty()){
                    m_fileHandler.open(filePath);
                    if(m_fileHandler.isOpen()){
                        allocData();
                    }
                }
            }

            const ExtentsVec&
            getExtents() const
            {
                assert(m_fileHandler.isOpen());
                return m_data.getExtents();
            }

            Ptr
            getAllocatedMemory()
            {
                allocData();
                return m_data.getData();
            }

            size_t
            getMemSize() const
            {
                return m_data.getMemSize();
            }

            Data&
            getData()
            {
                loadData();
                return m_data;
            }

            void
            loadData(bool forceReload = false)
            {
                if(m_gotData && !forceReload)
                    return;
                m_gotData = true;
                allocData();
                CopyPolicy()(m_fileHandler, m_data);
            }
        };

    }  // namespace mem

    namespace traits {

        template< typename T >
        struct IntegralTypeImpl< T, void_t< typename T::Accuracy > >: IntegralType< typename T::Accuracy >{};

        template<
            typename T_FileHandler,
            typename T_FileReaderPolicy,
            typename T_Accuracy,
            bool T_isComplex,
            unsigned T_numDims
        >
        struct IsStrided< mem::FileContainer< T_FileHandler, T_FileReaderPolicy, T_Accuracy, T_isComplex, T_numDims > >
        : std::integral_constant< bool, false>{};

    }  // namespace traits

}  // namespace LiFFT
