/**
 * @file ImageCPU.cpp
 * @author DM8AT
 * @brief implement the CPU image
 * @version 0.1
 * @date 2026-01-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the CPU image
#include "Graphic/ImageCPU.h"

//include exceptions
#include "Core/Exception.h"

//use the graphic namespace
using namespace GLGE::Graphic;

/**
 * @brief define a type that can store all pixel formats
 */
struct GeneralColor {
    /**
     * @brief contain the data for the color format in a blob
     */
    alignas(alignof(std::max_align_t)) GLGE::u8 blob[32];
};

/**
 * @brief a helper function to determine the maximum value for a specific bit
 * 
 * @param bits the amount of bits to compute the maximum for
 * @return `constexpr uint64_t` the maximum value for the amount of bits
 */
static constexpr uint64_t maxValueForBits(GLGE::u8 bits) {
    //quick exist for 0 bits
    if (bits == 0) return 0;
    //quick exist for 64 bits
    if (bits == 64) return ~uint64_t(0);
    //compute the maximum by using 2^bits - 1
    return (1ull << bits) - 1ull;
}

/**
 * @brief a helper function to check if a format has a red channel
 * 
 * @param f the format to check
 * @return `true` if a red channel exists, `false` otherwise
 */
static constexpr bool hasR(const PixelFormat& f) { return f.r_Bitcount > 0; }
/**
 * @brief a helper function to check if a format has a green channel
 * 
 * @param f the format to check
 * @return `true` if a green channel exists, `false` otherwise
 */
static constexpr bool hasG(const PixelFormat& f) { return f.g_Bitcount > 0; }
/**
 * @brief a helper function to check if a format has a blue channel
 * 
 * @param f the format to check
 * @return `true` if a blue channel exists, `false` otherwise
 */
static constexpr bool hasB(const PixelFormat& f) { return f.b_Bitcount > 0; }
/**
 * @brief a helper function to check if a format has an alpha channel
 * 
 * @param f the format to check
 * @return `true` if a alpha channel exists, `false` otherwise
 */
static constexpr bool hasA(const PixelFormat& f) { return f.a_Bitcount > 0; }


/**
 * @brief a helper function to read an unsigned integer from a buffer
 * 
 * @param src the buffer to read from
 * @param bits the amount of bits the number occupies
 * @return `GLGE::f64` the value stored by the unsigned integer
 */
inline static GLGE::f64 readUINT(const GLGE::u8* src, GLGE::u8 bits) {
    //store the actual value
    GLGE::u64 v = 0;
    //copy over the value stored in the source buffer
    memcpy(&v, src, bits / 8);
    //convert to a GLGE::f64 and return
    return GLGE::f64(v);
}
/**
 * @brief a helper function to read a normalized unsigned integer from a buffer
 * 
 * @param src the buffer to read from
 * @param bits the amount of bits the number occupies
 * @return `GLGE::f64` the value stored by the normalized integer
 */
inline static GLGE::f64 readUNORM(const GLGE::u8* src, GLGE::u8 bits) {
    //just read an unsigned integer and then normalize it and return the value
    return (readUINT(src, bits) / maxValueForBits(bits));
}
/**
 * @brief a helper function to read any float format
 * 
 * @param src the buffer to read from
 * @param bits the amount of bits the number occupies
 * @return `GLGE::f64` the value stored by the float format
 */
inline static GLGE::f64 readFLOAT(const GLGE::u8* src, GLGE::u8 bits) {
    //check for a 16 bit float format (half float)
    if (bits == 16) {
        //read the float from the buffer
        GLGE::u16 h;
        memcpy(&h, src, 2);

        //extract the components of the half
        GLGE::u32 sign = (h & 0x8000u) << 16;
        GLGE::u32 exp  = (h & 0x7C00u) >> 10;
        GLGE::u32 mant = (h & 0x03FFu);

        //store the actual float
        GLGE::u32 f;
        //Optimized half to float conversion
        if (exp == 0) {
            if (mant == 0) {
                f = sign;
            } else {
                exp = 127 - 15 + 1;
                while ((mant & 0x0400u) == 0) {
                    mant <<= 1;
                    exp--;
                }
                mant &= 0x03FFu;
                f = sign | (exp << 23) | (mant << 13);
            }
        } else if (exp == 31) {
            f = sign | 0x7F800000u | (mant << 13);
        } else {
            exp = exp - 15 + 127;
            f = sign | (exp << 23) | (mant << 13);
        }

        //convert the float to a GLGE::f64 and return it
        GLGE::f32 ff;
        memcpy(&ff, &f, 4);
        return GLGE::f64(ff);
    }

    //if it is 32 bits, just copy into a float, convert to a GLGE::f64 and return
    if (bits == 32) {
        GLGE::f32 f;
        memcpy(&f, src, 4);
        return GLGE::f64(f);
    }

    //lastly, just assume 64 bit float
    //just read the value directly and return it
    GLGE::f64 d;
    memcpy(&d, src, 8);
    return d;
}

/**
 * @brief a helper function to read a single channel
 * 
 * @param src the buffer to read from
 * @param bits the amount of bits to read
 * @param t the type of the channel
 * @return `GLGE::f64` the stored value
 */
inline static GLGE::f64 readChannel(const GLGE::u8* src, GLGE::u8 bits, PixelFormat::Type t) {
    //early return for 0 bits
    if (bits == 0) return 0.0;

    //switch over the type and call the correct read function
    switch(t) {
        case PixelFormat::Type::UNORM: 
            return readUNORM(src, bits);
            break;
        case PixelFormat::Type::UINT:  
            return readUINT(src, bits);
            break;
        case PixelFormat::Type::FLOAT: 
            return readFLOAT(src, bits);
            break;

        //as default always return 0
        default: 
            return 0.0;
            break;
    }
}

/**
 * @brief a structure that has the maximum supported channel count and the maximum value range for the colors
 * 
 * It also stores which channels are used
 */
struct CanonicalColor {
    /**
     * @brief store the value for the red channel
     */
    GLGE::f64 r = 0.0;
    /**
     * @brief store the value for the green channel
     */
    GLGE::f64 g = 0.0;
    /**
     * @brief store the value for the blue channel
     */
    GLGE::f64 b = 0.0;
    /**
     * @brief store the value for the alpha channel
     */
    GLGE::f64 a = 1.0;

    /**
     * @brief store if the red channel exists
     */
    bool hasR = false;
    /**
     * @brief store if the green channel exists
     */
    bool hasG = false;
    /**
     * @brief store if the blue channel exists
     */
    bool hasB = false;
    /**
     * @brief store if the alpha channel exists
     */
    bool hasA = false;
};

inline static void readCanonicalColor(CanonicalColor& C, const GLGE::u8* p, const PixelFormat& f) {
    //store the bit counts in shorthands
    //any compiler should be able to optimize this out
    GLGE::u8 rb = f.r_Bitcount;
    GLGE::u8 gb = f.g_Bitcount;
    GLGE::u8 bb = f.b_Bitcount;
    GLGE::u8 ab = f.a_Bitcount;

    //store a helper lambda that is used to read some amount of bits and then increment the pointer
    auto take = [&](int n) {
        //read the current position
        const GLGE::u8* r = p;
        //step up the pointer
        p += n;
        //return the previous position
        return r;
    };

    //switch over the order to get the correct one
    //for each channel take the amount of bytes and then read the channel
    //also, then store that the channel exists
    switch(f.order)
    {
        case PixelFormat::Order::R:
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            break;

        case PixelFormat::Order::RG:
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            break;

        case PixelFormat::Order::RGB:
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            break;

        case PixelFormat::Order::RGBA:
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            C.a = readChannel(take(ab/8), ab, f.type); C.hasA = true;
            break;

        case PixelFormat::Order::ARGB:
            C.a = readChannel(take(ab/8), ab, f.type); C.hasA = true;
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            break;

        case PixelFormat::Order::BGR:
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            break;

        case PixelFormat::Order::BGRA:
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            C.a = readChannel(take(ab/8), ab, f.type); C.hasA = true;
            break;

        case PixelFormat::Order::ABGR:
            C.a = readChannel(take(ab/8), ab, f.type); C.hasA = true;
            C.b = readChannel(take(bb/8), bb, f.type); C.hasB = true;
            C.g = readChannel(take(gb/8), gb, f.type); C.hasG = true;
            C.r = readChannel(take(rb/8), rb, f.type); C.hasR = true;
            break;

        case PixelFormat::Order::INDEXED:
            C.r = readChannel(take(rb/8), rb, f.type);
            C.hasR = true;
            break;

        default:
            break;
    }
}



/**
 * @brief a helper function to write a unsigned integer
 * 
 * @param dst the buffer to write to
 * @param v the value to write
 * @param bits the amount of bits to write
 */
inline static void writeUINT(GLGE::u8* dst, GLGE::f64 v, uint8_t bits) {
    //store the value and round correctly
    GLGE::u64 iv = (v < 0) ? 0ull : GLGE::u64(v + 0.5);
    //copy the data to the buffer
    memcpy(dst, &iv, bits/8);
}
/**
 * @brief a helper function to write a normalized integer
 * 
 * @param dst the buffer to write to
 * @param v the value to write
 * @param bits the amount of bits to write
 */
inline static void writeUNORM(GLGE::u8* dst, GLGE::f64 v, uint8_t bits) {
    //make sure to clamp the value
    GLGE::f64 clamped = std::clamp(v, 0.0, 1.0);
    //store the maximum value
    GLGE::u64 maxv = maxValueForBits(bits);
    //now store the clamped value in an unsigned integer
    GLGE::u64 iv = GLGE::u64(clamped * GLGE::f64(maxv) + 0.5);
    //copy the unsigned integer to the buffer
    memcpy(dst, &iv, bits/8);
}
/**
 * @brief a helper function to write a float format
 * 
 * @param dst the buffer to write to
 * @param v the value to write
 * @param bits the amount of bits to write
 */
inline static void writeFLOAT(GLGE::u8* dst, GLGE::f64 v, uint8_t bits) {
    //check for 16 bits (half float)
    if (bits == 16) {
        //read the float
        GLGE::f32 f = GLGE::f32(v);
        GLGE::u32 fb;
        memcpy(&fb, &f, 4);

        //extract the components
        GLGE::u32 sign = (fb >> 16) & 0x8000;
        GLGE::i32 exp = ((fb >> 23) & 0xFF) - 127 + 15;
        GLGE::u32 mant = fb & 0x7FFFFF;

        //store the final half value
        GLGE::u16 h;

        //Float to Half optimized conversion
        if (exp <= 0) {
            if (exp < -10) {
                h = GLGE::u16(sign);
            } else {
                mant |= 0x800000;
                GLGE::u32 t = mant >> (1 - exp + 13);
                h = GLGE::u16(sign | t);
            }
        } else if (exp == 0x1F) {
            h = GLGE::u16(sign | 0x7C00 | (mant ? 0x200 : 0));
        } else if (exp > 30) {
            h = GLGE::u16(sign | 0x7C00);
        } else {
            h = GLGE::u16(sign | (exp << 10) | (mant >> 13));
        }

        //copy over the final half value
        memcpy(dst, &h, 2);
        return;
    }

    //for 32 bits use a float directly
    if (bits == 32) {
        GLGE::f32 f = GLGE::f32(v);
        memcpy(dst, &f, 4);
        return;
    }

    //lastly, just assume 64 bits
    GLGE::f64 d = v;
    memcpy(dst, &d, 8);
}

/**
 * @brief a helper function to write a value to a channel
 * 
 * @param dst the destination to write to
 * @param v the value to write
 * @param bits the amount of bits to write
 * @param t the type to write
 */
inline static void writeChannel(GLGE::u8* dst, GLGE::f64 v, uint8_t bits, PixelFormat::Type t) {
    //switch over the type and write the correct type
    switch(t) {
        case PixelFormat::Type::UNORM: 
            writeUNORM(dst, v, bits); 
            break;
        case PixelFormat::Type::UINT:  
            writeUINT(dst, v, bits); 
            break;
        case PixelFormat::Type::FLOAT: 
            writeFLOAT(dst, v, bits); 
            break;

        //default: do nothing
        default: 
            break;
    }
}

/**
 * @brief a helper function to write a pixel from a canonical pixel
 * 
 * @param out the general color to write to
 * @param C the canonical color to write to the general color
 * @param to the format of the general color
 */
void static writePixel(GeneralColor& out, const CanonicalColor& C, const PixelFormat& to) {
    //store the destination to write to
    //as the blob of the output color
    GLGE::u8* dst = out.blob;

    //a helper lambda to write a value that has a 
    //specific amount of bytes. This just saves inputting
    //the type every time and also steps the write position
    auto W = [&](GLGE::f64 v, uint8_t bits){
        //write the channel
        writeChannel(dst, v, bits, to.type);
        //step the writing destination forward
        dst += bits/8;
    };

    //switch over the order to write in the correct order
    //for each order write all channels using the helper lambda
    switch(to.order)
    {
        case PixelFormat::Order::R:
            W(C.r, to.r_Bitcount);
            break;

        case PixelFormat::Order::RG:
            W(C.r, to.r_Bitcount);
            W(C.g, to.g_Bitcount);
            break;

        case PixelFormat::Order::RGB:
            W(C.r, to.r_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.b, to.b_Bitcount);
            break;

        case PixelFormat::Order::RGBA:
            W(C.r, to.r_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.b, to.b_Bitcount);
            W(C.a, to.a_Bitcount);
            break;

        case PixelFormat::Order::ARGB:
            W(C.a, to.a_Bitcount);
            W(C.r, to.r_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.b, to.b_Bitcount);
            break;

        case PixelFormat::Order::BGR:
            W(C.b, to.b_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.r, to.r_Bitcount);
            break;

        case PixelFormat::Order::BGRA:
            W(C.b, to.b_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.r, to.r_Bitcount);
            W(C.a, to.a_Bitcount);
            break;

        case PixelFormat::Order::ABGR:
            W(C.a, to.a_Bitcount);
            W(C.b, to.b_Bitcount);
            W(C.g, to.g_Bitcount);
            W(C.r, to.r_Bitcount);
            break;

        case PixelFormat::Order::INDEXED:
            W(C.r, to.r_Bitcount);
            break;

        default:
            break;
    }
}

/**
 * @brief define a function that can convert color between all color formats
 * 
 * @param pixel the pixel to source the color from
 * @param from the color format for the source pixel
 * @param to the color format to convert the color to
 * @return `GeneralColor` a general color wrapper that contains the color
 */
inline static GeneralColor convertPixelLayout(const GLGE::u8* pixel, const PixelFormat& from, const PixelFormat& to) {
    //convert the inputted pixel to a canonical pixel
    CanonicalColor canon;
    readCanonicalColor(canon, pixel, from);
    //convert the canonical color to the output
    GeneralColor out;
    writePixel(out, canon, to);
    //return the general color
    return out;
}

ImageCPU::ImageCPU(void* data, const PixelFormat& format, const uvec2& size) 
 : m_format(format), m_size(size)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::ImageCPU")

    //in debug sanity check the format
    #if GLGE_DEBUG
    if ((m_format.r_Bitcount + m_format.g_Bitcount + m_format.b_Bitcount + m_format.a_Bitcount) % 8 != 0)
    {throw GLGE::Exception("Tried to create a CPU Image using an invalid format", "GLGE::Graphic::ImageCPU::ImageCPU");}
    #endif

    //compute the bytes per pixel
    m_bytesPerPixel = (m_format.r_Bitcount + m_format.g_Bitcount + m_format.b_Bitcount + m_format.a_Bitcount) / 8;
    //compute the size of the data
    size_t s = m_bytesPerPixel * m_size.x * m_size.y;
    //create a new data buffer
    m_data = new u8[s];

    //if data is nullptr, just zero out the data
    //else, copy over the data
    if (data) {
        //copy over the data
        memcpy(m_data, data, s);
    } else {
        //zero the data
        memset(m_data, 0, s);
    }
}

ImageCPU::ImageCPU(const ImageCPU& other) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::ImageCPU")

    //copy format, size and bytes per pixel
    m_format = other.m_format;
    m_size = other.m_size;
    m_bytesPerPixel = other.m_bytesPerPixel;
    //compute the size of the data
    size_t s = m_bytesPerPixel *  m_size.x * m_size.y;
    //create a new data buffer
    m_data = new u8[s];
    //copy over the data
    memcpy(m_data, other.m_data, s);
}

ImageCPU::ImageCPU(ImageCPU&& other) noexcept
    : m_format(other.m_format),
      m_size(other.m_size),
      m_bytesPerPixel(other.m_bytesPerPixel),
      m_data(other.m_data)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::ImageCPU")

    //zero out the other element
    other.m_data = nullptr;
    other.m_size = {0,0};
    other.m_format = PixelFormat{};
    other.m_bytesPerPixel = 0;
}


ImageCPU::~ImageCPU() {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::~ImageCPU")

    //clean up the data
    delete[] m_data;
    //zero out everything
    m_data = nullptr;
    m_size = {0,0};
    m_format = PixelFormat{};
    m_bytesPerPixel = 0;
}

ImageCPU& ImageCPU::operator=(const ImageCPU& other) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::operator=")

    //early ot if copying to self
    if (this == &other)
    {return *this;}

    //copy format, size and bytes per pixel
    m_format = other.m_format;
    m_size = other.m_size;
    m_bytesPerPixel = other.m_bytesPerPixel;
    //clean the own data
    delete[] m_data;
    //compute the size of the data
    size_t s = m_bytesPerPixel *  m_size.x * m_size.y;
    //create a new data buffer
    m_data = new u8[s];
    //copy over the data
    memcpy(m_data, other.m_data, s);

    //return a reference to self
    return *this;
}

ImageCPU& ImageCPU::operator=(ImageCPU&& other) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::operator=")

    //only continue if this is not self
    if (this != &other) {
        //clean up the own data
        delete[] m_data;

        //copy the data over
        m_format = other.m_format;
        m_size = other.m_size;
        m_bytesPerPixel = other.m_bytesPerPixel;
        m_data = other.m_data;

        //clean up the other element
        other.m_data = nullptr;
        other.m_size = {0,0};
        other.m_format = PixelFormat{};
        other.m_bytesPerPixel = 0;
    }
    //return this
    return *this;
}

ImageCPU ImageCPU::toFormat(const PixelFormat& format) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::toFormat")

    //in debug sanity check the format
    #if GLGE_DEBUG
    if ((m_format.r_Bitcount + m_format.g_Bitcount + m_format.b_Bitcount + m_format.a_Bitcount) % 8 != 0)
    {throw GLGE::Exception("Tried to create a CPU Image using an invalid format", "GLGE::Graphic::ImageCPU::ImageCPU");}
    #endif

    //create the output image
    ImageCPU out;
    //copy over the size, format and bytes per pixel
    out.m_size = m_size;
    out.m_format = format;
    out.m_bytesPerPixel = std::ceil((format.r_Bitcount + format.g_Bitcount + format.b_Bitcount + format.a_Bitcount) / 8.);
    //compute the size of the data
    size_t s = out.m_bytesPerPixel * m_size.x * m_size.y;
    //allocate the image buffer
    out.m_data = new u8[s];
    
    //compute the amount of pixels to write
    size_t todo = m_size.x * m_size.y;
    //store iteration pointers
    u8* from = m_data;
    u8* to = out.m_data;
    //iterate over all pixels and copy them over with the correct format
    for (size_t i = 0; i < todo; ++i) {
        //convert the pixel to the correct color space
        GeneralColor col = convertPixelLayout(from, m_format, format);
        //write the color
        memcpy(to, &col.blob, out.m_bytesPerPixel);
        //advance the iterators
        from += m_bytesPerPixel;
        to += out.m_bytesPerPixel;
    }

    //return the finalized image
    return out;
}

const void* ImageCPU::readTexel(const uvec2& texel) const {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::readTexel")

    //make sure the texel is sane
    GLGE::u32 x = std::clamp<GLGE::u32>(texel.x, 0, m_size.x-1);
    GLGE::u32 y = std::clamp<GLGE::u32>(texel.y, 0, m_size.y-1);
    //return the address to the texel
    return (const void*)(m_data + ((y * m_size.x + x) * m_bytesPerPixel));
}

GLGE::vec4 ImageCPU::readTexelRGBA(const uvec2& texel) const {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::readTexelRGBA")

    //read the texel and convert it to RGBA 32F
    GeneralColor col = convertPixelLayout((const u8*)readTexel(texel), m_format, PIXEL_FORMAT_RGBA_32_FLOAT);
    //return the color as a vec4
    vec4 out;
    memcpy(&out, &col.blob, sizeof(out));
    return out;
}

void ImageCPU::writeTexel(const uvec2& texel, void* value) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::writeTexel")

    //simply copy the value from the input to the texel
    memcpy(m_data + ((texel.y * m_size.x + texel.x) * m_bytesPerPixel), value, m_bytesPerPixel);
}

void ImageCPU::writeTexelRGBA(const uvec2& texel, const vec4& value) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::writeTexelRGBA")

    //convert the inputted value to the internal format
    GeneralColor col = convertPixelLayout((const u8*)&value, PIXEL_FORMAT_RGBA_32_FLOAT, m_format);
    //store the texel
    writeTexel(texel, &col.blob);
}

inline static GLGE::f32 wrapCoord(GLGE::f32 u, GLGE::Graphic::SamplerCPU::Wrap wrap) {
    //switch over the wrap mode to determine the correct one
    switch (wrap)
    {
    case GLGE::Graphic::SamplerCPU::WRAP_REPEAT:
        //return only the fractional part
        return (u - floor(u));
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_MIRROR_REPEAT: {
            //compute the value in a repeating space from 0 to 2
            GLGE::f32 t = fmodf(fabs(u), 2.f);
            //if it is over 1, return it mirrored
            return (t <= 1.f) ? t : (2.f - t);
        }
        break;
    //clamp is also the default
    //so no extra check required
    default:
        //just clamp it
        return std::clamp(u, 0.f, 1.f);
        break;
    }
}

inline static GLGE::vec4 fetchTexel(GLGE::vec2 uv, GLGE::vec2 offs, GLGE::Graphic::SamplerCPU::Wrap wrap, const GLGE::Graphic::ImageCPU* img, GLGE::uvec2* texel = nullptr) {
    //check for wrap mode to black
    if (wrap == GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_BLACK) {
        //if UV lies outside the range, return 0
        if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f) 
        {return {0,0,0,0};}
        //else, the UV value is sane
    } else {
        //for all other wrap modes, use the wrap coord function
        uv.x = wrapCoord(uv.x, wrap);
        uv.y = wrapCoord(uv.y, wrap);
    }
    //convert the uv value to a texel value
    //this rounds down the floating point value to the nearest texel and uses that to fetch it
    GLGE::uvec2 tex = {
        GLGE::u32(uv.x * GLGE::f32(img->getSize().x - 1) + offs.x),
        GLGE::u32(uv.y * GLGE::f32(img->getSize().y - 1) + offs.y)
    };
    //if the texel is set, store the texel value
    if (texel)
    {*texel = tex;}
    //read the texel in RGBA 32F value
    return img->readTexelRGBA(tex);
}

inline static GLGE::vec4 sampleNearest(GLGE::vec2 uv, GLGE::Graphic::SamplerCPU::Wrap wrap, const GLGE::Graphic::ImageCPU* img) {
    //add a half to use correct rounding when sampling the texel
    return fetchTexel(uv, {0.5, 0.5}, wrap, img);
}

inline static GLGE::vec4 sampleLinear(GLGE::vec2 uv, GLGE::Graphic::SamplerCPU::Wrap wrap, const GLGE::Graphic::ImageCPU* img) {
    //fetch the four surrounding texels
    //create storage for the texel positions
    GLGE::uvec2 t0;
    GLGE::uvec2 t1;
    GLGE::uvec2 t2;
    GLGE::uvec2 t3;
    //fetch the colors
    GLGE::vec4 c0 = fetchTexel(uv, {0,0}, wrap, img, &t0);
    GLGE::vec4 c1 = fetchTexel(uv, {1,0}, wrap, img, &t1);
    GLGE::vec4 c2 = fetchTexel(uv, {0,1}, wrap, img, &t2);
    GLGE::vec4 c3 = fetchTexel(uv, {1,1}, wrap, img, &t3);
    //compute the UV values of the texels
    GLGE::vec2 uv_tex0 = {t0.x / GLGE::f32(img->getSize().x - 1), t0.y / GLGE::f32(img->getSize().y - 1)};
    GLGE::vec2 uv_tex1 = {t1.x / GLGE::f32(img->getSize().x - 1), t1.y / GLGE::f32(img->getSize().y - 1)};
    GLGE::vec2 uv_tex2 = {t2.x / GLGE::f32(img->getSize().x - 1), t2.y / GLGE::f32(img->getSize().y - 1)};
    GLGE::vec2 uv_tex3 = {t3.x / GLGE::f32(img->getSize().x - 1), t3.y / GLGE::f32(img->getSize().y - 1)};
    //apply the wrapping to the inputted uv
    GLGE::vec2 wrapped = {
        wrapCoord(uv.x, wrap),
        wrapCoord(uv.y, wrap)
    };
    //lerp the top and bottom most colors
    GLGE::vec4 top = c0 * (1.f - (wrapped.x - uv_tex0.x)) + c1 * (1.f - (uv_tex1.x - wrapped.x));
    GLGE::vec4 bot = c2 * (1.f - (wrapped.x - uv_tex2.x)) + c3 * (1.f - (uv_tex3.x - wrapped.x));
    //lerp the interpolated colors
    GLGE::vec4 fin = top * (1.f - (wrapped.y - uv_tex1.y)) + bot * (1.f - (uv_tex2.y - wrapped.y));

    //return the interpolated value
    return fin;
}

GLGE::vec4 ImageCPU::sample(vec2 uv, const SamplerCPU& sampler) const {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::sample")

    //switch over the sampling mode to determine how to sample
    switch (sampler.getFilterMinification())
    {
    case GLGE::Graphic::SamplerCPU::FILTER_NEAREST:
        return sampleNearest(uv, sampler.getWrap(), this);
        break;
    case GLGE::Graphic::SamplerCPU::FILTER_LINEAR:
        return sampleLinear(uv, sampler.getWrap(), this);
        break;
    
    default:
        //just return black
        return {0,0,0,0};
        break;
    }
}

GLGE::vec4 ImageCPU::samplePerspective(vec2 uv, vec4 derivatives, const SamplerCPU& sampler) const {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::ImageCPU::samplePerspective")

    //create aliases for the derivatives
    f32 ux = derivatives.x;
    f32 uy = derivatives.y;
    f32 vx = derivatives.z;
    f32 vy = derivatives.w;

    //compute gradient magnitudes
    f32 lenU = sqrtf(ux*ux + uy*uy);
    f32 lenV = sqrtf(vx*vx + vy*vy);

    //determine major/minor gradients
    f32 majorLen = std::max(lenU, lenV);
    f32 minorLen = std::min(lenU, lenV);

    //sanity check to prevent division by 0
    if (minorLen < 1e-12f)
    {minorLen = 1e-12f;}

    //anisotropy ratio is the the axis ratio
    f32 ratio = majorLen / minorLen;

    //clamp the ratio by sampler's anisotropy
    f32 aniso = std::clamp(ratio, 1.0f, f32(sampler.getAnisotropy()));

    //select the major direction vector
    vec2 majorDir = (lenU > lenV) ? vec2(ux, uy) : vec2(vx, vy);

    //normalize major direction
    f32 majorNorm = (lenU > lenV) ? lenU : lenV;
    //prevent division by 0
    if (majorNorm < 1e-12f) majorNorm = 1e-12f;
    majorDir /= majorNorm;

    //effective filter radius along the major axis
    f32 filterRadius = majorLen * 0.5f;

    //compute the number of samples by rounding up
    int samples = int(ceil(aniso));

    //accumulator for the samples
    vec4 accum = vec4(0);
    //accumulate the samples
    for (int i = 0; i < samples; i++)
    {
        //the offset scalar must be between -1 and 1
        f32 t = ((i + 0.5f) / samples) * 2.0f - 1.0f;
        //compute the actual offset
        vec2 offset = majorDir * (t * filterRadius);
        //sample and accumulate
        accum += sample(uv + offset, sampler);
    }

    //return the average result
    return accum / f32(samples);
}