#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <cassert>

#include "PNGImage.h"

namespace png {

typedef unsigned int   uint_t;
typedef unsigned short ushort_t;
typedef unsigned char  byte_t;


const static int SIGNATURE_SIZE    = 8;
const static int CHUNK_TYPE_SIZE   = 4;
const static int CHUNK_LENGTH_SIZE = 4;
const static int CHUNK_CRC_SIZE    = 4;

const static byte_t PNG_SIGNATURE[] = { 0x89, 0x50, 0x4E, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

const static uint_t crc_table[] = 
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

static const ushort_t bit_mask[] = {
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000F, 0x001F, 0x003F, 0x007F,
    0x00FF, 0x01FF, 0x03FF, 0x07FF,
    0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
    0xFFFF
};


enum class ChunkType : uint_t
{
    IHDR = 0x49484452,  // Image header
    PLTE = 0x504c5445,  // Palette table
    IDAT = 0x49444154,  // Image data
    IEND = 0x49454e44,  // Image trailer

    tRNS = 0x74524e53,  // Transparency information

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
    
    tIME = 0x74494d45  // Time stamp
};

enum class ColourType : byte_t
{
    Greyscale   = 0,
    TrueColour  = 2,
    Indexed     = 3,
    AGreyscale  = 4,
    ATrueColour = 6
};

struct Code {
    size_t code;
    size_t length;
    Code (size_t c, size_t l) : code(c), length(l)
    {}
};

bool operator< (const Code& lhs, const Code& rhs)
{
    return lhs.length < rhs.length || lhs.code < rhs.code;
}

bool operator== (const Code& lhs, const Code& rhs)
{
    return lhs.length == rhs.length && lhs.code == rhs.code;
}

// --------------------------------------------------------
// File read / write support

class ImageFile {
public:
    ImageFile() : ifs(), crc(0)
    {}

    ~ImageFile() { close(); } 

    bool open (const std::string& file) 
    {
        ifs.open(file, std::ios::in | std::ios::binary);
        return is_open();
    }
    
    void close() { if (ifs.is_open()) ifs.close(); }

    void reset_crc() { crc = 0xFFFFFFFF; }
    uint_t get_crc() const { return crc ^ 0xffffffffL; }
  
    bool eof() { return ifs.eof() || ifs.peek() == EOF; }
    bool is_open() const { return ifs && ifs.is_open(); }
    
    template <typename T>
    void read(T& val);

    void read(std::vector<byte_t>& data, size_t size);
    
    void skip(size_t count);

private:
    void update_crc(byte_t val);
    
    std::fstream ifs;
    uint_t       crc;
};

void ImageFile::update_crc(byte_t val)
{
    crc = crc_table[(crc ^ val) & 0xff] ^ (crc >> 8);
}

template <typename T>
void ImageFile::read(T& val)
{
    if (!eof())
    {   
        byte_t data[sizeof(T)];
        ifs.read(reinterpret_cast<char*>(&data[0]), sizeof(T));
        val = {};
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            val = (val << 8) | static_cast<T>(data[i]); 
            update_crc(data[i]);
        }
    }   
}

template <typename T>
T reverce_bits(const T& v, size_t count)
{
	T buf{};
	for (size_t i = 0; i < count; ++i)
	{
		buf = (buf << 1) + ((v >> i) & 1);
	}
		
	return buf;
}


template <>
void ImageFile::read(ChunkType& val)
{
    uint_t type; read(type);
    val = static_cast<ChunkType>(type);
}

template <>
void ImageFile::read(ColourType& val)
{
    byte_t type; read(type);
    val = static_cast<ColourType>(type);
}

void ImageFile::read(std::vector<byte_t>& data, size_t size)
{
    if (!eof())
    {
        data.resize(size);
        ifs.read(reinterpret_cast<char*>(&data[0]), size);
	    if (ifs) for (auto& v : data) update_crc(v);
    }
}

void ImageFile::skip(size_t count)
{
    std::cout << std::dec << "\tskip " <<  count << "\n";
    ifs.seekg(count, std::ios::cur);
}

template <typename T>
ImageFile& operator >> (ImageFile& f, T& val)
{
    f.read(val);
    return f;
}

bool check_crc(ImageFile& file)
{   
    uint_t data_crc = file.get_crc();
    uint_t file_crc; file >> file_crc;
    return file_crc == data_crc;
}


struct BitStream {
    BitStream(std::vector<byte_t> f) : file(f), buf(0), bpos(0), dpos(0)
    {
        if (!eof()) buf += file[dpos++];
        if (!eof()) buf += (file[dpos++] << 8);
        if (!eof()) buf += (file[dpos++] << 16);
        if (!eof()) buf += (file[dpos++] << 24);
        std::cout << std::hex << buf << std::endl;
    }

    ushort_t get (size_t count)
    {   		
        assert(count >= 0 && count <= sizeof(ushort_t) * 8);
		if (count == 0) return 0;
		if (eof()) return 0;
		
        ushort_t res = static_cast<ushort_t>((buf >> bpos) & bit_mask[count]);
        bpos += count;
        while (bpos > 7)
        {
            buf >>= 8; bpos -= 8;
            if (dpos < file.size()) 
			{
				uint_t bb = (file[dpos++] << 24);
				buf += bb;
			}
        }

        return res;
    }
	
	ushort_t get_huffman(size_t count)
	{
		return reverce_bits(get(count), count);
	}
	

    bool eof() { return dpos >= file.size() && bpos >= sizeof(uint_t) * 8; }

private:
    std::vector<byte_t> file;
    uint_t buf;
    size_t bpos;
    size_t dpos;
};


// --------------------------------------------------------
// Image header

struct Header {
    uint_t width;       
    uint_t height;      
    byte_t bit_depth;
    ColourType colour_type;
    byte_t compression;  
    byte_t filter;       
    byte_t interlace;    

    Header() : width(0), height (0), bit_depth(0), colour_type(ColourType::ATrueColour), compression(0), filter(0), interlace(0)
    {}

    bool from_file(ImageFile& file);
};

bool Header::from_file(ImageFile& file)
{
    if (!file.is_open() || file.eof()) return false;

    uint_t length, type;
    file.read(length);
    file.reset_crc();
    file.read(type);
    
    if (length <= 0) 
    {
        std::cout << "Wrong header chunk size" << std::endl;
        return false;
    }

    if (static_cast<ChunkType>(type) != ChunkType::IHDR) 
    {
        std::cout << "Wrong header chunk type" << std::endl;
        return false;
    }

    std::cout << "Parsing header" << std::endl;

    file >> width 
         >> height
         >> bit_depth  
         >> colour_type
         >> compression
         >> filter     
         >> interlace;   
    
    // check image size
    if (!check_crc(file))
    {
        std::cout << "Checksum does not match" << std::endl;
        return false;   
    }

    if (width <= 0 || height <= 0) 
    {
        std::cout << "Wrong image size" << std::endl;
        return false; 
    }

    // check color type and bit depth
    std::vector<byte_t> allowed_bit_depths;

    switch (colour_type)
    {
        case ColourType::Greyscale : 
            allowed_bit_depths = {1,2,4,8,16};  
            break;
        case ColourType::Indexed :     
            allowed_bit_depths = {1,2,4,8};
            break;
        case ColourType::TrueColour : 
        case ColourType::AGreyscale :         
        case ColourType::ATrueColour : 
            allowed_bit_depths = {8,16};
            break;
    };

    if (std::find(allowed_bit_depths.begin(), allowed_bit_depths.end(), bit_depth) == allowed_bit_depths.end())
    {
        std::cout << "Not allowed bit depth" << std::endl;
        return false; 
    }

    // only compression method 0 (deflate/inflate) is defined in International Standard
    if (compression != 0)
    {
        std::cout << "Compressoin type " << compression << " not allowed" << std::endl;
        return false; 
    }

    // only filter method 0 (adaptive filtering with five basic filter types) is defined in International Standard
    if (filter != 0)
    {
        std::cout << "Filter type " << filter << " not allowed" << std::endl;
        return false; 
    }

    // transmission order of the image data : 0 (no interlace) or 1 (Adam7 interlace)
    if (interlace != 0 && interlace != 1)
    {
        std::cout << "Interlace method " << interlace << " not allowed" << std::endl;
        return false; 
    }

    std::cout << "\twidth: "       << std::dec << width            << "\n"
              << "\theight: "      << std::dec << height           << "\n"
              << "\tbit_depth: "   << std::hex << (int)bit_depth   << "\n"
              << "\tcolour_type: " << std::hex << (int)colour_type << "\n"
              << "\tcompression: " << std::hex << (int)compression << "\n"
              << "\tfilter: "      << std::hex << (int)filter      << "\n"
              << "\tinterlace: "   << std::hex << (int)interlace   << std::endl;

    return true;
}



struct Palette { 
};

// --------------------------------------------------------
// PNG implementation

struct PNGImage::Impl {
    Header head;
    Palette palette;
    std::vector<byte_t> data;


    Impl() : head(), palette(), data()
    {}
    
    bool from_file(ImageFile& file);

    bool is_png_file(ImageFile& file);

    bool inflate(ImageFile& file, size_t length);

};

bool PNGImage::Impl::from_file(ImageFile& file)
{
    if (file.is_open())
    {
        if (!is_png_file(file))                    // check signature 
        {
            std::cout << "Is not PNG file" << std::endl;
            return false;      
        }
        if (!head.from_file(file)) return false;   // read header

        bool has_IEND = false;
        bool has_IDAT = false;
        bool has_PLTE = false;
        while (!file.eof() && file.is_open())      // read image data
        {           
            uint_t length; file.read(length);
            file.reset_crc();
            ChunkType type; file.read(type);

            std::cout << "Chunk: " << std::hex << static_cast<uint_t>(type) 
                      << " Size: "  << std::dec << length << std::endl;

            switch (type)
            {
                case ChunkType::IDAT :
                    has_IDAT = inflate(file, length);
                    break;

                case ChunkType::IEND : 
                    std::cout << "Find IEND" << std::endl;
                    has_IEND = check_crc(file);
                    break;

                default:
                    std::cout << "\tNo implemented action" << std::endl;
                    file.skip(length + CHUNK_CRC_SIZE);
                    
            break;
            }
        }   
        
        if (!has_IDAT) 
        {
            std::cout << "Image data was not found" << std::endl;
            return false;
        }

        if (!has_IEND)
        {
            std::cout << "Wrong file ending" << std::endl;  
            return false;
        }

        std::cout << "END" << std::endl;
        return true;

    }
    std::cout << "File not open" << std::endl; 
    return false;
}

bool PNGImage::Impl::is_png_file(ImageFile& file)
{
    std::vector<byte_t> file_sign;
    file.read(file_sign, SIGNATURE_SIZE);
    return std::equal(file_sign.begin(), file_sign.end(), PNG_SIGNATURE);
}

bool PNGImage::Impl::inflate(ImageFile& file, size_t length)
{
    static bool all_data = false;
    
    static const size_t uncompressed_block_limit = 65535;
    
    const size_t MAX_WINDOW_SIZE = 32768;
    std::cout << "Process IDAT chunk" << std::endl;
    if (file.is_open() && !file.eof())
    {

        /*
          do
               read block header from input stream.
               if stored with no compression
                  skip any remaining bits in current partially processed byte
                  read LEN and NLEN (see next section)
                  copy LEN bytes of data to output
               otherwise
                  if compressed with dynamic Huffman codes
                     read representation of code trees (see
                        subsection below)
                  loop (until end of block code recognized)
                     decode literal/length value from input stream
                     if value < 256
                        copy value (literal byte) to output stream
                     otherwise
                        if value = end of block (256)
                           break from loop
                        otherwise (value = 257..285)
                           decode distance from input stream

                           move backwards distance bytes in the output
                           stream, and copy length bytes from this
                           position to the output stream.
                  end loop
            while not last block
         */



        
        
        byte_t cmf, flg;
        file >> cmf >> flg;
        byte_t cm     = (byte_t) (cmf & 0x0F);
        byte_t cinfo  = (byte_t) ((cmf >> 4) & 0x0F);
        byte_t fcheck = (byte_t) (flg & 0x1F);
        byte_t fdict  = (byte_t) ((flg >> 5) & 1);
        byte_t flevel = (byte_t) ((flg >> 6) & 0x03);

        size_t window_size = (size_t) pow(2, (cinfo + 8));
        
        const int BFINAL = 1;

        enum {BTYPE_NO, BTYPE_FIXED, BTYPE_DYNAMIC, BTYPE_ERROR };
        
        // CM = 8 denotes the "deflate" compression method
        // fdict = 0, The additional flags shall not specify a preset dictionary 
        if ((cmf * 256 + flg) % 31 != 0 || cm != 8 || window_size > MAX_WINDOW_SIZE || fdict != 0)
        {
            std::cout << "Wrong compression params" << std::endl;
        }

        std::cout << "\tcm: "    << std::hex << (int)cm     << "\n"
                  << "\tcinfo: "  << std::hex << (int)cinfo  << "\n"
                  << "\tfcheck: " << std::hex << (int)fcheck << "\n"
                  << "\tfdict: "  << std::hex << (int)fdict  << "\n"
                  << "\tflevel: " << std::hex << (int)flevel << "\n";

        file.skip(length - 2 + CHUNK_CRC_SIZE);
        
        
        /*std::vector<byte_t> data = {0b01110011, 0b01001001, 0b01001101, 0b11001011, 
									0b01001001, 0b00101100, 0b01001001, 0b01010101, 
									0b00000000, 0b00010001, 0b00000000};*/
									
									
		std::vector<byte_t> data = {
			0b00001100, 0b11001000, 0b01000001, 0b00001010, 0b10000000, 0b00100000, 0b00010000, 0b00000101,
			0b11010000, 0b01111101, 0b11010000, 0b00011101, 0b11111110, 0b00001001, 0b10111010, 0b10000100,
			0b11101011, 0b10100000, 0b00101011, 0b01001100, 0b11111010, 0b10110101, 0b00000001, 0b00011101,
			0b00100001, 0b00100111, 0b10100001, 0b11011011, 0b11010111, 0b01011011, 0b10111110, 0b11010000,
			0b10101101, 0b11011100, 0b11100010, 0b01001111, 0b00010101, 0b11010111, 0b01101110, 0b00000011,
			0b11011101, 0b01110000, 0b00110010, 0b11110110, 0b10100110, 0b01010110, 0b00100000, 0b10000110,
			0b00111101, 0b00011100, 0b00011011, 0b10001110, 0b01001010, 0b00011001, 0b11111100, 0b00011111,
			0b10010010, 0b10100110, 0b00001110, 0b00100110, 0b11111000, 0b00100101, 0b00001110, 0b11100110,
			0b11001100, 0b11101000, 0b00111010, 0b00001001, 0b01101101, 0b10001101, 0b01001001, 0b11000101,
			0b01011001, 0b11011111, 0b01110101, 0b11111001, 0b00000110, 0b00000000
		};
        
        byte_t btype = BTYPE_ERROR;
        std::vector<byte_t> result;

        BitStream bs(data);
		
		//read block header from input stream.
        bool last_block = bs.get(1) && BFINAL;
        btype = bs.get(2);


        if (btype == BTYPE_NO)   // stored with no compression
        {
            std::cout << "BTYPE_NO" << std::endl;
			// skip any remaining bits in current partially processed byte
			// read LEN and NLEN (see next section)
			// copy LEN bytes of data to output
/*

		Any bits of input up to the next byte boundary are ignored.
         The rest of the block consists of the following information:

              0   1   2   3   4...
            +---+---+---+---+================================+
            |  LEN  | NLEN  |... LEN bytes of literal data...|
            +---+---+---+---+================================+

         LEN is the number of data bytes in the block.  NLEN is the
         one's complement of LEN.
*/

        } else
        if (btype == BTYPE_FIXED)
        {
            std::cout << "BTYPE_FIXED" << std::endl;
			static const byte_t length_extra_bits[] = {
				0,  0,0,0,0, 0,0,0,0,  1,1,1,1,     2,2,2,2,     3,3,3,3,     4,4,4,4,      5,5,5,5,         0
			};
			//  256 257 --------- 264  265 --- 268  269 --- 272  273 --- 276  277 ---- 280  281 ------- 284  285
			//  ^- bound valuse, not useed;
			static const ushort_t length_values[] = {
				0,  3,4,5,6, 7,8,9,10, 11,13,15,17, 19,23,27,31, 35,43,51,59, 67,83,99,115, 131,163,195,227, 258
			};
			
			static const size_t DIST_CODE_SIZE = 5;
			static const byte_t dist_extra_bits[] = {
				0,0, 0,0, 1,1, 2,2,  3,3,   4,4,   5,5,   6,6,     7,7,     8,8,     9,9,       10,10,     11,11,     12,12,      13,13
			};
			//  0 1  2 3  4 5  6 7   8  9   10 11  12 13  14  15   16  17   18  19   20   21    22   23    24   25    26   27     28    29
			static const ushort_t dist_values[] = {
				1,2, 3,4, 5,7, 9,13, 17,25, 33,49, 65,97, 129,193, 257,385, 513,769, 1025,1537, 2049,3073, 4097,6145, 8193,12289, 16385,24577   
			};
			
			while (!bs.eof())
			{
				ushort_t code = bs.get_huffman(7);   
				if (code == 0)
				{ 
					for (auto& x : result) std::cout << x;
					std::cout << std::endl;
					return last_block; // exit from decoding
				}
				
				bool code_is_length = (code >= 0b0000000 && code <= 0b0010111);  // 7 bit code, Lit Value 256 - 279
				if (!code_is_length) 
				{
					code = (code << 1) + bs.get_huffman(1);         // add 1 more bit
					code_is_length = (code >= 0b11000000 && code <= 0b11000111);  // 8 bit code, Lit Value 280 - 287
				}

				if (code_is_length)  
				{
					byte_t length_extra_bits_count = length_extra_bits[code];
					size_t length = length_values[code] + bs.get(length_extra_bits_count);
					
					byte_t dist_code = bs.get(DIST_CODE_SIZE);
					byte_t dist_extra_bits_count = dist_extra_bits[dist_code];
					size_t dist = dist_values[dist_code] + bs.get(dist_extra_bits_count);

					if (dist < result.size())
					{
						for (size_t i = 0, pos = result.size() - dist, j = 0; i < length; ++i, j = 0)
						{
							while (j < dist && i < length) result.push_back(result[pos + j++]), ++i;
						}
					}

					std::cout << "lenght: " << length << std::endl;   
					std::cout << "dist: " << dist << std::endl;

				} else 
				if (code >= 0b00110000 && code <= 0b10111111)       // 8 bit code, Lit Value 0 - 143 
				{
					result.push_back(code - 0x30);
					std::cout << result.back() << std::endl;
				} else
				{
					code = (code << 1) + bs.get_huffman(1);         // add 1 more bit
				
					if (code >= 0b110010000 && code <= 0b111111111) // 9 bit code, Lit Value 144 - 255 
					{
						result.push_back(code - 0x100);
						std::cout << result.back() << std::endl;
						continue;
					} else 
					{
						std::cout << " DATA READ ERRROR!!!!!" << std::endl;
						exit(0);
					}
				}
				
			}
			std::cout << "complete" << std::endl;

        } else
        if (btype == BTYPE_DYNAMIC)
        {
			
			/*
			 * code lengths
			        0 - 15: Represent code lengths of 0 - 15
                   16: Copy the previous code length 3 - 6 times.
                       The next 2 bits indicate repeat length
                             (0 = 3, ... , 3 = 6)
                          Example:  Codes 8, 16 (+2 bits 11),
                                    16 (+2 bits 10) will expand to
                                    12 code lengths of 8 (1 + 6 + 5)
                   17: Repeat a code length of 0 for 3 - 10 times.
                       (3 bits of length)
                   18: Repeat a code length of 0 for 11 - 138 times
                       (7 bits of length)

 * 
 *             5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
               5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
               4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
			    * 
				*  (HCLEN + 4) x 3 bits: code lengths for the code length
                  alphabet given just above, in the order: 16, 17, 18,
                  0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15

                  These code lengths are interpreted as 3-bit integers
                  (0-7); as above, a code length of 0 means the
                  corresponding symbol (literal/length or distance code
                  length) is not used.

               HLIT + 257 code lengths for the literal/length alphabet,
                  encoded using the code length Huffman code

               HDIST + 1 code lengths for the distance alphabet,
                  encoded using the code length Huffman code

               The actual compressed data of the block,
                  encoded using the literal/length and distance Huffman
                  codes

               The literal/length symbol 256 (end of data),
                  encoded using the literal/length Huffman code
			 * */
			std::cout << "BTYPE_DYNAMIC" << std::endl;
			 
			 
			size_t HLIT = bs.get(5) + 257;
			size_t HDIST = bs.get(5) + 1;
			size_t HCLEN = bs.get(4) + 4;
			std::cout << "HLIT " << std::dec << HLIT << "\n"
					  << "HDIST " << std::dec <<  HDIST << "\n"
					  << "HCLEN " << std::dec <<  HCLEN << std::endl;
            static const byte_t code_length_indexes [] = {
                16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
            };
            static const size_t MAX_CODE = 19;

            std::vector<byte_t> code_lengths_for_code_lengths(MAX_CODE, 0);

            // Read HCLEN * 3 bits for code lengths for code length alphabet
            size_t max_bits = 0;
            for (size_t i = 0; i < HCLEN; ++i) {
                byte_t cl = (byte_t) bs.get(3);
                if (cl > max_bits) max_bits = cl;
                code_lengths_for_code_lengths[code_length_indexes[i]] = cl;
            }

            // Count the number of codes for each code length
            std::vector<size_t> bl_count(max_bits + 1, 0);
            for (const auto& l : code_lengths_for_code_lengths)
                if (l) bl_count[l]++;
            bl_count[0] = 0;

            // Find the numerical value of the smallest code for each code length
            std::vector<size_t> next_code(max_bits + 1);
            for (size_t bits = 1, code = 0; bits <= max_bits; ++bits) {
                code = (code + bl_count[bits - 1]) << 1;
                next_code[bits] = code;
            }

            // Assign numerical values to all codes
            std::map<Code, size_t> code_lengths_alphabet;
            for (size_t n = 0;  n < MAX_CODE; ++n) {
                byte_t len = code_lengths_for_code_lengths[n];
                if (len != 0) {
                    Code c(next_code[len], len);

                    if (code_lengths_alphabet.count(c)){
                        auto f = code_lengths_alphabet.find(c);
                        code_lengths_alphabet[Code(next_code[len], len)] = n;
                    }
                    code_lengths_alphabet[Code(next_code[len], len)] = n;
                    next_code[len]++;
                }
            }




            return true;
        } else 
        {
            std::cout << "Error" << std::endl;
        }
    }

    return false;
}

// --------------------------------------------------------
// PNGImage interface

PNGImage::PNGImage() : pImpl(new Impl()) 
{

}

PNGImage::PNGImage(const PNGImage& other) : pImpl(new Impl(*other.pImpl.get()))
{

}

PNGImage::PNGImage(PNGImage&& other) : pImpl(new Impl())
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

PNGImage& PNGImage::operator= (PNGImage&& other)
{
    if (&other == this) return *this;
    
    pImpl.swap(other.pImpl);
    other.pImpl.reset(new Impl());

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
    ImageFile file;
    if (file.open(file_name))
    {
        PNGImage tmp;
        if (tmp.pImpl->from_file(file))
        {
            pImpl.swap(tmp.pImpl);  
            return true; 
        }
    }
    return false;
}

bool PNGImage::save_as(const std::string& file_name)
{
    throw NotImplemented();
    return false;
}

}; // namespace png
