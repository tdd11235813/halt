/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>
#include <cassert>
#include <tiffio.h>
#include <boost/utility.hpp>
#include "tiffWriter/ImageFormat.hpp"
#include "tiffWriter/FormatTraits.hpp"
#include <memory>

namespace tiffWriter
{
    /**
     * Allocator that uses the TIFF functions
     */
    struct TiffAllocator
    {
        template< typename T >
        void
        malloc(T*& p, size_t size)
        {
            p = static_cast<T*>(_TIFFmalloc(size));
        }

        template< typename T >
        void
        free(T* ptr)
        {
            _TIFFfree(ptr);
        }
    };

    /**
     * Wrapper for reading TIFF images from the file system
     * \tparam T_Allocator Allocator(::malloc, ::free) used for managing the raw memory
     */
    template< ImageFormat T_imgFormat = ImageFormat::ARGB, class T_Allocator = TiffAllocator >
    class Image
    {
        using Allocator = T_Allocator;
        static constexpr ImageFormat imgFormat = T_imgFormat;

        using DataType = typename PixelType<imgFormat>::type;
        using ChannelType = typename PixelType<imgFormat>::ChannelType;
        using Ref = DataType&;
        using ConstRef = const DataType&;

        std::string m_filepath;
        std::unique_ptr<TIFF, void(*)(TIFF*)> m_handle;
        std::unique_ptr<DataType[], void(*)(DataType*)> m_data;
        bool m_isReadable, m_isWriteable, m_dataWritten;
        unsigned m_width, m_height;
        bool originIsAtTop;
        uint16 samplesPerPixel, bitsPerSample, tiffSampleFormat, photometric;

        void openHandle(const std::string& filePath, const char* mode);
        void closeHandle();
        void allocData();
        void loadData();
        template<typename T>
        void checkedWrite(uint16 tag, T value);

        template< uint16_t T_numChannels, bool T_minIsBlack >
        void
        convert(char* tmp);

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
    public:

        /**
         * Creates an invalid image using the standard allocator.
         * Before accessing it you need to call \ref open(..)
         */
        Image():
            m_filepath(""),
            m_handle(nullptr, TIFFClose),
            m_data(nullptr, [](DataType* p){ Allocator().free(p);}),
            m_isReadable(false),
            m_isWriteable(false),
            m_dataWritten(false),
            m_width(0), m_height(0), originIsAtTop(true)
        {}
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        /**
         * Opens the image at the given filePath for reading
         *
         * @param filePath Path to the image to load
         * @param loadData True if the image data should be loaded or only its memory allocated.
         *          The data can be (re)loaded with \ref load()
         */
        Image(const std::string& filePath, bool bLoadData = true): Image()
        {
            open(filePath, bLoadData);
        }

        /**
         * Opens the image at the given filePath for writing
         * Overwrites or creates it
         *
         * @param filePath Path to the image to save to
         * @param w Width of the new image
         * @param h Height of the new image
         */
        Image(const std::string& filePath, unsigned w, unsigned h): Image()
        {
            open(filePath, w, h);
        }

        ~Image()
        {
            close();
        }

        /**
         * Opens the image at the given filePath for reading
         * Implicitly closes an open image
         *
         * @param filePath Path to the image to load
         * @param loadData True if the image data should be loaded or only its memory allocated.
         *          The data can be (re)loaded with \ref load()
         */
        void open(const std::string& filePath, bool loadData = true);

        /**
         * Opens the image at the given filePath for writing
         * Overwrites or creates it
         * Implicitly closes an open image
         *
         * @param filePath Path to the image to save to
         * @param w Width of the new image
         * @param h Height of the new image
         * @param isOriginAtTop true to set origin to top, false for bottom (left)
         */
        void open(const std::string& filePath, unsigned w, unsigned h, bool isOriginAtTop = true);

        /**
         * Closes the current image freeing all memory
         */
        void close();

        /**
         * Flushes the current write buffer to file invalidating the file handle
         * You have to use saveTo afterwards!
         */
        void flush(){ closeHandle(); }

        /**
         * Loads the image data into memory
         * Throws an exception if the image is not opened for reading
         */
        void load();

        /**
         * Saves the image data to file. This is NOT done in the destructor!
         * Throws an exception if the image is not opened for writing
         *
         * @param compress Whether to compress the file or not
         * @param saveAsARGB Whether to save ARGB files as ARGB (true) or RGB only (ignored for monochromatic files)
         */
        void save(bool compress = true, bool saveAsARGB = true);

        /**
         * Saves the image data to file at the given path and opens it for writing
         * Can be used to write modified data to a file (might be the same as the current one)
         *
         * @param filePath Path to the image to save to
         * @param compress Whether to compress the file or not
         * @param saveAsARGB Whether to save ARGB files as ARGB (true) or RGB only (ignored for monochromatic files)
         */
        void saveTo(const std::string& filePath, bool compress = true, bool saveAsARGB = true);

        /**
         * Returns whether the image is open and its data can be read
         * @return True if the image is open
         */
        bool isOpen() const
        {
            return (m_handle != nullptr);
        }

        unsigned getWidth() const
        {
            assert(isOpen() || m_data);
            return m_width;
        }

        unsigned getHeight() const
        {
            assert(isOpen() || m_data);
            return m_height;
        }

        /**
         * Returns true, if the origin of the image is at the top (left)
         * False if it is at the bottom (left)
         * Note: Only valid after image is opened for writing or data was loaded
         */
        bool isOriginAtTop() const
        {
            assert(isOpen() || m_data);
            return originIsAtTop;
        }

        /**
         * Returns the total size of the used memory for the image data
         * @return size in bytes
         */
        size_t getDataSize() const
        {
            assert(isOpen() || m_data);
            return sizeof(DataType) * m_width * m_height;
        }

        /**
         * Accesses the pixel at the given location (read-write)
         * @param x
         * @param y
         * @return Reference to the pixel value
         */
        Ref
        operator()(unsigned x, unsigned y)
        {
            assert(isOpen() || m_data);
            return m_data[y * m_width + x];
        }

        /**
         * Accesses the pixel at the given location (read-only)
         * @param x
         * @param y
         * @return The pixel value
         */
        ConstRef
        operator()(unsigned x, unsigned y) const
        {
            assert(isOpen() || m_data);
            return m_data[y * m_width + x];
        }
    };

    /**
     * Monochrome image where each pixel is represented by 1 float value
     * (A)RGB images will be converted and scaled to [0,1]
     */
    template< class T_Allocator = TiffAllocator >
    using FloatImage = Image< ImageFormat::Float, T_Allocator >;

    /**
     * Monochrome image where each pixel is represented by 1 double value
     * (A)RGB images will be converted and scaled to [0,1]
     */
    template< class T_Allocator = TiffAllocator >
    using DoubleImage = Image< ImageFormat::Double, T_Allocator >;

    /** Gets the type of a monochrome image with the given floating point type */
    template<typename T_FP>
    struct GetMonochromeImageType;

    template<>
    struct GetMonochromeImageType<float>
    {
        using type = FloatImage<>;
    };
    template<>
    struct GetMonochromeImageType<double>
    {
        using type = DoubleImage<>;
    };

}  // namespace tiffWriter

#include "tiffWriter/image.tpp"
