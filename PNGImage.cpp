#include <iostream>
#include <fstream>
#include <vector>

#include "PNGImage.h"

namespace png {


const static int SIGNATURE_SIZE = 8;
const static int CHUNK_TYPE_SIZE = 4;
const static int CHUNK_LENGTH_SIZE = 4;
const static int CHUNK_CRC_SIZE = 4;

const static char PNG_SIGNATURE[] = { -119, 80, 78, 71, 13, 10, 26, 10 };

enum class ChunkType : unsigned int {
    IHDR = 0x49484452,   // Image header
    PLTE = 0x504c5445,   // Palette table
    IDAT = 0x49444154,   // Image data
    IEND = 0x49454e44,   // Image trailer

    tRNS = 0x74524e53,   // Transparency information

                         // Colour space information
    cHRM = 0x6348524d,
    gAMA = 0x67414d41,
    iCCP = 0x69434350,
    sBIT = 0x73424954,
    sRGB = 0x73524742,
                         // Textual information
    iTXt = 0x69545874,
    tEXt = 0x74455874,
    zTXt = 0x7a545874,
                         // Miscellaneous information
    bKGD = 0x624b4744,
    hIST = 0x68495354,
    pHYs = 0x70485973,
    sPLT = 0x73504c54,
    
    tIME = 0x74494d45    // Time stamp
};

template<typename T>
T read_value (std::fstream& fs)
{
    size_t size = sizeof(T);
    T value {};
    std::unique_ptr<char[]> data( new char [size] );
    fs.read(data.get(), size);
    for (size_t i = 0; i < size; ++i)
        value = (value << 8) | static_cast<T>(data[i]);
        
    return value;
}


struct Header {
    
};


// PNG implementation

struct PNGImage::Impl {
    Header head;
    std::vector<char> data;

    
    bool read_file(std::fstream& fs);

    bool is_png_file(std::fstream& fs);
    bool read_chunk(std::fstream& fs);


};



bool PNGImage::Impl::read_file(std::fstream& fs)
{
    if (fs && fs.is_open() && is_png_file(fs))
    {
        fs.seekg(SIGNATURE_SIZE, fs.beg);   // skip signature
        while (read_chunk(fs)) std::cout << "read_chunk\n";   // read image data
        
        return true;
    }
    return false;
}

bool PNGImage::Impl::is_png_file(std::fstream& fs)
{
    fs.seekg (0, fs.beg);
    char file_sign[SIGNATURE_SIZE];
    fs.read (file_sign, SIGNATURE_SIZE);
    
    return std::equal(file_sign, file_sign + SIGNATURE_SIZE, PNG_SIGNATURE);
}

bool PNGImage::Impl::read_chunk(std::fstream& fs)
{
    if (!fs) { return false; }

    unsigned int length = read_value<unsigned int> (fs);
    unsigned int chunk_type = read_value<unsigned int> (fs);

    std::unique_ptr<char[]> chunk_data( new char [length] );
    fs.read(chunk_data.get(), length);
    
    unsigned int crc = read_value<unsigned int> (fs);
    
    switch (chunk_type)
    {
        case IHDR : process(); break; 
        case PLTE : process(); break; 
        case IDAT : process(); break; 
        case IEND : process(); break; 
        case tRNS : process(); break; 

        case cHRM : process(); break; 
        case gAMA : process(); break; 
        case iCCP : process(); break; 
        case sBIT : process(); break; 
        case sRGB : process(); break; 

        case iTXt : process(); break; 
        case tEXt : process(); break; 
        case zTXt : process(); break; 

        case bKGD : process(); break;  
        case hIST : process(); break;  
        case pHYs : process(); break;  
        case sPLT : process(); break;  
        case tIME : process(); break;  

        default: 
            // nothing 
    };
          
    return true;
}

// PNGImage interface

PNGImage::PNGImage() noexcept : pImpl(nullptr) 
{
    
}

PNGImage::PNGImage(const PNGImage& other) : pImpl(new Impl(*other.pImpl.get()))
{

}

PNGImage::PNGImage(PNGImage&& other) noexcept : pImpl(nullptr)
{
    pImpl.swap(other.pImpl);
}

PNGImage& PNGImage::operator= (const PNGImage& other)
{
    if (&other == this) return *this;
    
    PNGImage tmp(other);
    pImpl.swap(tmp.pImpl);
    
    return *this;
}

PNGImage& PNGImage::operator= (PNGImage&& other) noexcept
{
    if (&other == this) return *this;
    
    pImpl.reset();
    std::swap(pImpl, other.pImpl);
    
    return *this;
}

PNGImage::~PNGImage()
{
    // nothing
}

bool PNGImage::create(size_t Width, size_t height)
{
    throw NotImplemented();
    return false;
}

bool PNGImage::open(const std::string& file_name)
{
    std::fstream f;
    f.open(file_name, std::ios::in | std::ios::binary );
    return pImpl->read_file(f);
}

bool PNGImage::save_as(const std::string& file_name)
{
    throw NotImplemented();
    return false;
}

}; // namespace png
