#include <iostream>
#include <fstream>
#include <vector>

#include "PNGImage.h"

namespace png {

typedef unsigned char BYTE;

const static int SIGNATURE_SIZE = 8;
const static int CHUNK_TYPE_SIZE = 4;
const static int CHUNK_LENGTH_SIZE = 4;
const static int CHUNK_CRC_SIZE = 4;

const static char PNG_SIGNATURE[] = { -119, 80, 78, 71, 13, 10, 26, 10 };

enum ChunkType : unsigned int {
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

template<typename T, long N = sizeof(T)>
T read_value (std::fstream& fs)
{
    T value {};
    BYTE data[N];
    fs.read(reinterpret_cast<char*>(&data[0]), N);

    for (size_t i = 0; i < N; ++i)
    {
        value = (value << 8) | static_cast<T>(data[i]);
    }
    return value;
}


struct Header {
    
};


// PNG implementation

struct PNGImage::Impl {
    Header head;
    std::vector<BYTE> data;

    
    bool read_file(std::fstream& fs);

    bool is_png_file(std::fstream& fs);
    bool read_chunk(std::fstream& fs);

    void process();

};



bool PNGImage::Impl::read_file(std::fstream& fs)
{
    if (fs && fs.is_open() && is_png_file(fs))
    {
        fs.seekg(SIGNATURE_SIZE, fs.beg);   // skip signature
        while (fs && read_chunk(fs)) std::cout << "read_chunk\n";   // read image data
        
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

    std::vector<BYTE> chunk_data(length);
    fs.read(reinterpret_cast<char*>(&chunk_data[0]), length);
    
    unsigned int crc = read_value<unsigned int> (fs);
    
    switch (chunk_type)
    {
        case IHDR : std::cout << "chunk: IHDR" << " len: " << length << std::endl; process(); break; 
        case PLTE : std::cout << "chunk: PLTE" << " len: " << length << std::endl; process(); break; 
        case IDAT : std::cout << "chunk: IDAT" << " len: " << length << std::endl; process(); break; 
        case IEND : std::cout << "chunk: IEND" << " len: " << length << std::endl; process(); break; 
        case tRNS : std::cout << "chunk: tRNS" << " len: " << length << std::endl; process(); break; 

        case cHRM : std::cout << "chunk: cHRM" << " len: " << length << std::endl; process(); break; 
        case gAMA : std::cout << "chunk: gAMA" << " len: " << length << std::endl; process(); break; 
        case iCCP : std::cout << "chunk: iCCP" << " len: " << length << std::endl; process(); break; 
        case sBIT : std::cout << "chunk: sBIT" << " len: " << length << std::endl; process(); break; 
        case sRGB : std::cout << "chunk: sRGB" << " len: " << length << std::endl; process(); break; 

        case iTXt : std::cout << "chunk: iTXt" << " len: " << length << std::endl; process(); break; 
        case tEXt : std::cout << "chunk: tEXt" << " len: " << length << std::endl; process(); break; 
        case zTXt : std::cout << "chunk: zTXt" << " len: " << length << std::endl; process(); break; 

        case bKGD : std::cout << "chunk: bKGD" << " len: " << length << std::endl; process(); break;  
        case hIST : std::cout << "chunk: hIST" << " len: " << length << std::endl; process(); break;  
        case pHYs : std::cout << "chunk: pHYs" << " len: " << length << std::endl; process(); break;  
        case sPLT : std::cout << "chunk: sPLT" << " len: " << length << std::endl; process(); break;  
        case tIME : std::cout << "chunk: tIME" << " len: " << length << std::endl; process(); break;  

        default: 
            // nothing 
        break;
    };
          
    return true;
}

void PNGImage::Impl::process()
{

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
    std::fstream fs;
    fs.open(file_name, std::ios::in | std::ios::binary );
    return fs.good() && pImpl->read_file(fs);
}

bool PNGImage::save_as(const std::string& file_name)
{
    throw NotImplemented();
    return false;
}

}; // namespace png
