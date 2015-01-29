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


struct Chunk {
    unsigned int type;
    unsigned int length;
    std::vector<BYTE> data;
    unsigned int crc;
};

struct Header {
    bool from_chunk(const Chunk&);
};

bool Header::from_chunk(const Chunk& chunk)
{
    if (chunk.type != IHDR) 
    {
        std::cout << "Wrong header chunk" << std::endl;
        return false;
    }
    std::cout << "Parsing header" << std::endl;

    return true;
}



// PNG implementation

struct PNGImage::Impl {
    Header head;
    std::vector<BYTE> data;

    
    bool read_file(std::fstream& fs);

    bool is_png_file(std::fstream& fs);
    Chunk read_chunk(std::fstream& fs);

};

bool PNGImage::Impl::read_file(std::fstream& fs)
{
    if (fs && fs.is_open() && is_png_file(fs))
    {
        fs.seekg(SIGNATURE_SIZE, fs.beg);   // skip signature

        if (!head.from_chunk(read_chunk(fs))) return false;

        while (fs && fs.peek() != EOF)      // read image data
        {                 
            Chunk chunk = read_chunk(fs);
            std::cout << "chunk: " << chunk.type << std::endl;

            if (chunk.type == IEND)
            {
                std::cout << "END" << std::endl;
                if (fs.peek() != EOF)  std::cout << "Wrong file ending" << std::endl;          
            }
        }   
        
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

Chunk PNGImage::Impl::read_chunk(std::fstream& fs)
{
    Chunk chunk;

    chunk.length = read_value<unsigned int> (fs);
    chunk.type = read_value<unsigned int> (fs);

    chunk.data.resize(chunk.length);
    if (chunk.length) 
    {
        fs.read(reinterpret_cast<char*>(&chunk.data[0]), chunk.length);
    }
    
    chunk.crc = read_value<unsigned int> (fs);          
    return chunk;
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
