/**
 * @file GLGEColor.cpp
 * @author DM8AT
 * @brief implement the color class
 * @version 0.1
 * @date 2025-04-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the color class
#include "GLGEColor.h"
//include math
#include <cmath>

//map for the output stream
const char* COLOR_SPACE_MAP[] = {
    "COLOR_SPACE_UNDEFINED",
    "COLOR_SPACE_RGBA",
    "COLOR_SPACE_ARGB",
    "COLOR_SPACE_HSVA",
    "COLOR_SPACE_AHSV"
};

const char* GLGE_C_FUNC(colorSpaceToString)(ColorSpace space)
{
    //sanity check the input
    if (((uint32_t)space) >= (sizeof(COLOR_SPACE_MAP) / sizeof(*COLOR_SPACE_MAP)))
    {
        //if it is not sane, return 0
        return 0;
    }
    //return the color space mapping
    return COLOR_SPACE_MAP[(uint32_t)space];
}

void Color::convertTo(ColorSpace space)
{
    //set the values in the requested color space
    m_values = convertColorSpace(m_space, space, m_values);
    //store the new color space
    m_space = space;
}

//convert different color spaces when one of them is RGB
static vec4 rgbConverter(const vec4& values, const vec3& in, ColorSpace from, ColorSpace to)
{
    //store the output vector
    vec4 out;

    //check if the conversion is from RGB to HSV
    if (((from == COLOR_SPACE_RGBA) || (from == COLOR_SPACE_ARGB)) && ((to == COLOR_SPACE_HSVA) || (to == COLOR_SPACE_AHSV)))
    {
        //convert from RGB to HSV
        using namespace std;

        // Normalize RGB values (HDR values are allowed, so no clamping to [0, 1])
        float r = in.x;
        float g = in.y;
        float b = in.z;

        // Find the maximum and minimum values among R, G, and B using ternary operators
        float max = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
        float min = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
        float delta = max - min;

        // Calculate Hue
        float h = 0.0f;
        if (delta > 0.0f)
        {
            if (max == r)
            {
            h = 60.0f * (std::fmod(((g - b) / delta), 6.0f));
            }
            else if (max == g)
            {
            h = 60.0f * (((b - r) / delta) + 2.0f);
            }
            else if (max == b)
            {
            h = 60.0f * (((r - g) / delta) + 4.0f);
            }
        }
        if (h < 0.0f)
        {
            h += 360.0f;
        }

        // Calculate Saturation
        float s = (max == 0.0f) ? 0.0f : (delta / max);

        // Calculate Value (HDR values are preserved)
        float v = max;

        // Store the HSV values in the output vector
        out = vec4(h / 360., s, v, values.w);
    }
    if (((from == COLOR_SPACE_HSVA) || (from == COLOR_SPACE_AHSV)) && ((to == COLOR_SPACE_RGBA) || (to == COLOR_SPACE_ARGB)))
    {
        //convert HSV to RGB
        float h = std::fmod(in.x, 1) * 360.0f; // Convert hue to degrees
        float s = in.y;
        float v = in.z;

        float c = v * s; // Chroma
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r = 0.0f, g = 0.0f, b = 0.0f;

        if (h >= 0.0f && h < 60.0f)
        {
            r = c;
            g = x;
            b = 0.0f;
        }
        else if (h >= 60.0f && h < 120.0f)
        {
            r = x;
            g = c;
            b = 0.0f;
        }
        else if (h >= 120.0f && h < 180.0f)
        {
            r = 0.0f;
            g = c;
            b = x;
        }
        else if (h >= 180.0f && h < 240.0f)
        {
            r = 0.0f;
            g = x;
            b = c;
        }
        else if (h >= 240.0f && h < 300.0f)
        {
            r = x;
            g = 0.0f;
            b = c;
        }
        else if (h >= 300.0f && h < 360.0f)
        {
            r = c;
            g = 0.0f;
            b = x;
        }

        // Add m to each component to match the value
        out = vec4(r + m, g + m, b + m, values.w);
    }
    else if (((from == COLOR_SPACE_RGBA) || (from == COLOR_SPACE_ARGB)) && ((to == COLOR_SPACE_HSLA) || (to == COLOR_SPACE_AHSL)))
    {
        // Convert RGB to HSL
        float r = in.x;
        float g = in.y;
        float b = in.z;

        float max = std::max({r, g, b});
        float min = std::min({r, g, b});
        float delta = max - min;

        float h = 0.0f;
        if (delta > 0.0f)
        {
            if (max == r)
            {
                h = 60.0f * (std::fmod(((g - b) / delta), 6.0f));
            }
            else if (max == g)
            {
                h = 60.0f * (((b - r) / delta) + 2.0f);
            }
            else if (max == b)
            {
                h = 60.0f * (((r - g) / delta) + 4.0f);
            }
        }
        if (h < 0.0f)
        {
            h += 360.0f;
        }

        float l = (max + min) / 2.0f;
        float s = (delta == 0.0f) ? 0.0f : (delta / (1.0f - std::fabs(2.0f * l - 1.0f)));

        out = vec4(h / 360.0f, s, l, values.w);
    }
    else if (((from == COLOR_SPACE_HSLA) || (from == COLOR_SPACE_AHSL)) && ((to == COLOR_SPACE_RGBA) || (to == COLOR_SPACE_ARGB)))
    {
        // Convert HSL to RGB
        float h = std::fmod(in.x, 1) * 360.0f; // Convert hue to degrees
        float s = in.y;
        float l = in.z;

        float c = (1.0f - std::fabs(2.0f * l - 1.0f)) * s;
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;

        float r = 0.0f, g = 0.0f, b = 0.0f;

        if (h >= 0.0f && h < 60.0f)
        {
            r = c;
            g = x;
            b = 0.0f;
        }
        else if (h >= 60.0f && h < 120.0f)
        {
            r = x;
            g = c;
            b = 0.0f;
        }
        else if (h >= 120.0f && h < 180.0f)
        {
            r = 0.0f;
            g = c;
            b = x;
        }
        else if (h >= 180.0f && h < 240.0f)
        {
            r = 0.0f;
            g = x;
            b = c;
        }
        else if (h >= 240.0f && h < 300.0f)
        {
            r = x;
            g = 0.0f;
            b = c;
        }
        else if (h >= 300.0f && h < 360.0f)
        {
            r = c;
            g = 0.0f;
            b = x;
        }

        out = vec4(r + m, g + m, b + m, values.w);
    }
    //return the output
    return out;
}

vec4 Color::convertColorSpace(ColorSpace from, ColorSpace to, const vec4& values)
{
    //check if the color spaces are the same
    if (from == to)
    {
        //return the values
        return values;
    }

    //store if the spaces are the same
    bool same = false;
    //switch over the from space and check if the basic space of the other is the same
    switch (from)
    {
    case COLOR_SPACE_RGBA:
        same = (to == COLOR_SPACE_ARGB);
        break;
    case COLOR_SPACE_ARGB:
        same = (to == COLOR_SPACE_RGBA);
        break;
    case COLOR_SPACE_HSLA:
        same = (to == COLOR_SPACE_AHSL);
        break;
    case COLOR_SPACE_HSVA:
        same = (to == COLOR_SPACE_AHSV);
        break;
    case COLOR_SPACE_AHSV:
        same = (to == COLOR_SPACE_HSVA);
        break;
    case COLOR_SPACE_AHSL:
        same = (to == COLOR_SPACE_HSLA);
        break;
    default:
        break;
    }

    //check if the space are the same
    if (same)
    {
        //calculate the shift direction
        int8_t shift = ((uint8_t)from) - ((uint8_t)to);

        //check if the spaces are the same just shifted
        if (shift == -1)
        {
            //return the color channels shifted
            return vec4(values.w, values.x, values.y, values.z);
        }
        //check for a shift in the other direction
        else if (shift == 1)
        {
            //return the color channels shifted back
            return vec4(values.y, values.z, values.w, values.x);
        }
        //else, something went wrong
        else
        {
            //just return 0
            return 0;
        }
    }

    //extract the main 3 color components
    vec3 in = getColor(values, from);
    //store the 3 colors for the output
    vec4 out = 0;

    //check if one space is an RGB space
    if (((from == COLOR_SPACE_RGBA) || (from == COLOR_SPACE_ARGB)) || ((to == COLOR_SPACE_RGBA) || (to = COLOR_SPACE_ARGB)))
    {
        //use direct mapping
        out = rgbConverter(values, in, from, to);
    }
    else
    {
        //else, use an intermediate mapping to RGBA
        vec4 tmp = rgbConverter(values, in, from, COLOR_SPACE_RGBA);
        //map to the final space
        out = rgbConverter(values, vec3(tmp.x, tmp.y, tmp.z), COLOR_SPACE_RGBA, to);
    }

    //check if the alpha channel should be shifted
    if ((to == COLOR_SPACE_ARGB) || (to == COLOR_SPACE_AHSV) || (to == COLOR_SPACE_AHSL))
    {
        //shift the alpha channel
        out = vec4(out.w, out.x, out.y, out.z);
    }
    //return the output
    return out;
}

vec3 Color::getColor(vec4 color, ColorSpace space)
{
    //store the color components
    vec3 out;

    //switch over the color space
    switch (space)
    {
    case COLOR_SPACE_RGBA:
    case COLOR_SPACE_HSVA:
    case COLOR_SPACE_HSLA:
        //use the front most 3 components
        out = vec3(color.x, color.y, color.z);
        break;
    case COLOR_SPACE_ARGB:
    case COLOR_SPACE_AHSV:
    case COLOR_SPACE_AHSL:
        //use the back 3 components
        out = vec3(color.y, color.z, color.w);
        break;
    
    default:
        break;
    }

    //return the color components
    return out;
}

std::string Color::textColor() const noexcept
{
    //convert the color to RGB
    vec4 rgb = getInSpace(COLOR_SPACE_RGBA);
    //return the escape code to set the color
    return std::string("\x1b[38;2;") + std::to_string((uint8_t)(rgb.x * 255.)) + ";" + std::to_string((uint8_t)(rgb.y * 255.)) + ";" + std::to_string((uint8_t)(rgb.z*255.)) + "m";
}

std::string Color::textBgColor() const noexcept
{
    //convert the color to RGB
    vec4 rgb = getInSpace(COLOR_SPACE_RGBA);
    //return the escape code to set the color
    return std::string("\x1b[48;2;") + std::to_string((uint8_t)(rgb.x * 255.)) + ";" + std::to_string((uint8_t)(rgb.y * 255.)) + ";" + std::to_string((uint8_t)(rgb.z*255.)) + "m";
}