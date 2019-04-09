/* This file is part of liFFT.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "tiffWriter/exceptions.hpp"
#include "tiffWriter/converters.hpp"
#include "tiffWriter/AllocatorWrapper.hpp"
#include "tiffWriter/uvector.hpp"
#include <iostream>

namespace tiffWriter {

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::openHandle(const std::string& filePath, const char* mode)
    {
        closeHandle();
        m_handle.reset(TIFFOpen(filePath.c_str(), mode));
        if(!m_handle)
            throw std::runtime_error("Could not open "+filePath);
        m_filepath = filePath;
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::closeHandle()
    {
        if(m_handle){
            m_handle.reset();
            m_isReadable = false;
            m_isWriteable = false;
            m_dataWritten = false;
        }
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::open(const std::string& filePath, bool bLoadData)
    {
        closeHandle();
        openHandle(filePath, "r");
        m_isReadable = true;
        uint32 w, h;
        if(!TIFFGetField(m_handle.get(), TIFFTAG_IMAGEWIDTH, &w))
            throw InfoMissingException("Width");
        if(!TIFFGetField(m_handle.get(), TIFFTAG_IMAGELENGTH, &h))
            throw InfoMissingException("Height");
        if(w*h != m_width*m_height)
            m_data.reset(); // Reset data only if we need a differently sized chunk
        m_width = w; m_height = h;
        if(bLoadData)
            loadData();
        else
            allocData();
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::open(const std::string& filePath, unsigned w, unsigned h, bool bIsOriginAtTop)
    {
        close();
        openHandle(filePath, "w");
        m_isWriteable = true;
        m_width = w; m_height = h;
        originIsAtTop = bIsOriginAtTop;
        allocData();
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::close()
    {
        closeHandle();
        m_data.reset();
    }

    template< typename T_Data, uint16_t T_inStride = 1, uint16_t T_outStride = 1 >
    struct ReadTiff{
        static constexpr uint16_t inStride = T_inStride;
        static constexpr uint16_t outStride = T_outStride;

        TIFF* m_handle;
        unsigned m_width, m_height;
        T_Data* m_data;
        char* m_tmp;

        ReadTiff(TIFF* handle, unsigned w, unsigned h, T_Data* data, char* tmp):
            m_handle(handle), m_width(w), m_height(h), m_data(data), m_tmp(tmp){}

        template< typename T >
        void
        assign(T& dst, T&& src)
        {
            dst = std::forward<T>(src);
        }

        template< typename T, size_t T_numChannels >
        void
        assign(T& dst, std::array< T, T_numChannels >&& src)
        {
            *reinterpret_cast<std::array< T, T_numChannels >*>(&dst) = std::forward<std::array< T, T_numChannels >>(src);
        }

        template<typename T_Func>
        void operator()(T_Func func) {
            using SrcType = typename T_Func::Src;
            SrcType* srcChannels = reinterpret_cast<SrcType*>(m_tmp);
            for (unsigned y = 0; y < m_height; y++) {
                if(!TIFFReadScanline(m_handle, m_tmp, y))
                    throw std::runtime_error("Failed reading scanline\n");
                for (unsigned x = 0; x < m_width; x++) {
                    assign(m_data[(y*m_width + x)*outStride], func(srcChannels[x*inStride]));
                }
            }
        }
    };

    template< ImageFormat T_imgFormat, class T_Allocator >
    template< uint16_t T_numChannels, bool T_minIsBlack >
    void
    Image< T_imgFormat, T_Allocator >::convert(char* tmp)
    {
        static constexpr uint16_t numChannelsSrc = T_numChannels;
        static constexpr uint16_t numChannelsDest = SamplesPerPixel<T_imgFormat>::value;
        static constexpr bool minIsBlack = T_minIsBlack;
        ReadTiff<ChannelType, numChannelsSrc, numChannelsDest > read(m_handle.get(), m_width, m_height, reinterpret_cast<ChannelType*>(m_data.get()), tmp);
        //Mono pictures
        if(tiffSampleFormat == SAMPLEFORMAT_UINT && bitsPerSample == 8)
            read(Convert<uint8_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_UINT && bitsPerSample == 16)
            read(Convert<uint16_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_UINT && bitsPerSample == 32)
            read(Convert<uint32_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_INT && bitsPerSample == 8)
            read(Convert<int8_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_INT && bitsPerSample == 16)
            read(Convert<int16_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_INT && bitsPerSample == 32)
            read(Convert<int32_t, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_IEEEFP && bitsPerSample == 32)
            read(Convert<float, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else if(tiffSampleFormat == SAMPLEFORMAT_IEEEFP && bitsPerSample == 64)
            read(Convert<double, ChannelType, numChannelsSrc, numChannelsDest, minIsBlack>());
        else
            throw FormatException("Unimplemented format");
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::allocData()
    {
        if(m_data)
            return;
        DataType* p;
        Allocator().malloc(p, getDataSize());
        if(!p)
            throw std::runtime_error("Out of memory");
        m_data.reset(p);
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::load()
    {
        if(!m_isReadable)
            throw std::runtime_error("Cannot load file that is not opened for reading");
        loadData();
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    template< typename T>
    void
    Image< T_imgFormat, T_Allocator >::checkedWrite(uint16 tag, T value){
        if(!TIFFSetField(m_handle.get(), tag, value))
            throw InfoWriteException(std::to_string(tag));
    }


    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::saveTo(const std::string& filePath, bool compress, bool saveAsARGB)
    {
        openHandle(filePath, "w");
        m_isWriteable = true;
        save(compress, saveAsARGB);
    }

    template< ImageFormat T_imgFormat, bool T_EnableRGB = (T_imgFormat == ImageFormat::ARGB) >
    struct SavePolicy
    {
        static constexpr ImageFormat imgFormat = T_imgFormat;

        template<typename T, typename Allocator>
        static void
        save(bool /*saveAsRGB*/, TIFF* handle, T* data, const Allocator& /*alloc*/, unsigned w, unsigned h)
        {
            assert( TIFFScanlineSize(handle) >= 0 );
            if( sizeof(T)*w != (unsigned) TIFFScanlineSize(handle) )
                throw FormatException("Scanline size is unexpected");
            for (unsigned y = 0; y < h; y++)
            {
                if(!TIFFWriteScanline(handle, &data[y*w], y))
                    throw std::runtime_error("Failed writing scanline");
            }
        }
    };

    template<>
    struct SavePolicy< ImageFormat::ARGB, true >
    {
        static constexpr ImageFormat imgFormat = ImageFormat::ARGB;
        using ChannelType = typename PixelType<imgFormat>::ChannelType;

        template<typename T, typename Allocator>
        static void
        save(bool saveAsRGB, TIFF* handle, T* data, const Allocator& alloc, unsigned w, unsigned h)
        {
            if(!saveAsRGB)
            {
                SavePolicy< imgFormat, false >::save(false, handle, data, alloc, w, h);
                return;
            }
            // Convert from ARGB to RGB
            if(w * sizeof(ChannelType) * 3 != TIFFScanlineSize(handle))
                throw FormatException("Scanline size is unexpected");

            // Make sure memory is freed even in case of exceptions
            auto tmpAlloc = wrapAllocator<ChannelType>(alloc);
            ao::uvector< ChannelType, decltype(tmpAlloc) > tmpLine(TIFFScanlineSize(handle) / sizeof(ChannelType), tmpAlloc);

            for (unsigned y = 0; y < h; y++)
            {
                auto* lineData = &data[y*w];
                for(unsigned x = 0; x < w; x++)
                {
                    tmpLine[x*3] = static_cast<ChannelType>(TIFFGetR(lineData[x]));
                    tmpLine[x*3+1] = static_cast<ChannelType>(TIFFGetG(lineData[x]));
                    tmpLine[x*3+2] = static_cast<ChannelType>(TIFFGetB(lineData[x]));
                }
                if(!TIFFWriteScanline(handle, tmpLine.data(), y))
                    throw std::runtime_error("Failed writing scanline");
            }
        }
    };

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::save(bool compress, bool saveAsARGB)
    {
        if(!m_isWriteable)
            throw std::runtime_error("Cannot save to a file that is not opened for writing");
        // If we already wrote to the image we need to reopen it to be able to modify the tags
        if(m_dataWritten){
            openHandle(m_filepath, "w");
            m_isWriteable = true;
        }
        if(imgFormat == ImageFormat::ARGB && !saveAsARGB)
            checkedWrite(TIFFTAG_SAMPLESPERPIXEL, 3); // Write as RGB
        else
            checkedWrite(TIFFTAG_SAMPLESPERPIXEL, SamplesPerPixel<imgFormat>::value);
        checkedWrite(TIFFTAG_BITSPERSAMPLE, BitsPerSample<imgFormat>::value);
        checkedWrite(TIFFTAG_SAMPLEFORMAT, PixelType<imgFormat>::tiffType);
        checkedWrite(TIFFTAG_IMAGEWIDTH, m_width);
        checkedWrite(TIFFTAG_IMAGELENGTH, m_height);
        if(imgFormat == ImageFormat::ARGB)
            checkedWrite(TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        else
            checkedWrite(TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        checkedWrite(TIFFTAG_ROWSPERSTRIP, 1);
        checkedWrite(TIFFTAG_XRESOLUTION, 1.);
        checkedWrite(TIFFTAG_YRESOLUTION, 1.);
        checkedWrite(TIFFTAG_RESOLUTIONUNIT, 1);
        if(compress)
            checkedWrite(TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        else
            checkedWrite(TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        checkedWrite(TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        checkedWrite(TIFFTAG_ORIENTATION, originIsAtTop ? ORIENTATION_TOPLEFT : ORIENTATION_BOTLEFT);
        SavePolicy<imgFormat>::save(saveAsARGB, m_handle.get(), m_data.get(), Allocator(), m_width, m_height);
        m_dataWritten = true;
    }

    template< ImageFormat T_imgFormat, class T_Allocator >
    void
    Image< T_imgFormat, T_Allocator >::loadData()
    {
        allocData();

        if(!TIFFGetField(m_handle.get(), TIFFTAG_PHOTOMETRIC, &photometric))
            throw InfoMissingException("Photometric");
        if(photometric != PHOTOMETRIC_RGB && photometric != PHOTOMETRIC_MINISBLACK && photometric != PHOTOMETRIC_MINISWHITE)
            throw FormatException("Photometric is not supported: " + std::to_string(photometric));

        if(!TIFFGetField(m_handle.get(), TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel)){
            if(photometric == PHOTOMETRIC_MINISBLACK ||photometric == PHOTOMETRIC_MINISWHITE)
                samplesPerPixel = 1;
            else
                throw InfoMissingException("Samples per pixel");
        }
        if(!TIFFGetField(m_handle.get(), TIFFTAG_BITSPERSAMPLE, &bitsPerSample))
            throw InfoMissingException("Bits per sample");
        if(!TIFFGetField(m_handle.get(), TIFFTAG_SAMPLEFORMAT, &tiffSampleFormat)){
            std::cerr << "SampleFormat not found. Assuming unsigned" << std::endl;
            tiffSampleFormat = SAMPLEFORMAT_UINT;
        }
        uint16 orientation;
        if(!TIFFGetField(m_handle.get(), TIFFTAG_ORIENTATION, &orientation))
            orientation = ORIENTATION_TOPLEFT;
        if(orientation != ORIENTATION_TOPLEFT && orientation != ORIENTATION_BOTLEFT)
            throw FormatException("Origin is not at left side");
        originIsAtTop = orientation == ORIENTATION_TOPLEFT;
        uint16 planarConfig;
        if(!TIFFGetField(m_handle.get(), TIFFTAG_PLANARCONFIG, &planarConfig) || planarConfig!=PLANARCONFIG_CONTIG){
            throw FormatException("PlanarConfig missing or not 1");
        }

        if(needConversion<T_imgFormat>(tiffSampleFormat, samplesPerPixel, bitsPerSample))
        {
            size_t numBytes = samplesPerPixel*bitsPerSample/8*m_width;
            if(numBytes != static_cast<size_t>(TIFFScanlineSize(m_handle.get())))
                throw FormatException("Scanline size is unexpected: "+std::to_string(numBytes)+":"+std::to_string(TIFFScanlineSize(m_handle.get())));
            if(samplesPerPixel != 1 && samplesPerPixel != 3 && samplesPerPixel != 4)
                throw FormatException("Unsupported sample count");

            // Use a vector here to safely delete the memory
            using TmpAlloc = AllocatorWrapper<char, Allocator>;
            ao::uvector< char, TmpAlloc > tmp(numBytes);

            if(samplesPerPixel == 1)
            {
                if(photometric == PHOTOMETRIC_MINISWHITE)
                    convert<1, false>(tmp.data());
                else
                    convert<1, true>(tmp.data());
            }else if(samplesPerPixel == 3){
                if(photometric == PHOTOMETRIC_MINISWHITE)
                    convert<3, false>(tmp.data());
                else
                    convert<3, true>(tmp.data());
            }else{
                if(photometric == PHOTOMETRIC_MINISWHITE)
                    convert<4, false>(tmp.data());
                else
                    convert<4, true>(tmp.data());
            }
        }else{
            if(getDataSize() != static_cast<size_t>(TIFFScanlineSize(m_handle.get()))*m_height)
                throw FormatException("Scanline size is unexpected");
            for (unsigned y = 0; y < m_height; y++) {
                if(TIFFReadScanline(m_handle.get(), &m_data[y*m_width], y) != 1)
                    throw std::runtime_error("Failed reading scanline");
            }
        }
    }

}  // namespace tiffWriter
