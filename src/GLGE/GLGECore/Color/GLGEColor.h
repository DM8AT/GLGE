/**
 * @file GLGEColor.h
 * @author DM8AT
 * @brief wrapper for colors in different formats
 * @version 0.1
 * @date 2025-04-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_COLOR_
#define _GLGE_COLOR_

//include the settings
#include "../GLGESettings.h"
//include vectors
#include "../../GLGEMath/Vectors/GLGEFloatVec.h"

/**
 * @brief define the color space of the color
 */
typedef enum e_ColorSpace {
    /**
     * @brief say that the color space is unknown. This is reserved as a default or error value. 
     */
    COLOR_SPACE_UNDEFINED = 0,
    /**
     * @brief say that the color space uses the RGB space and is mapped as follows:
     * x = Red,
     * y = Green,
     * z = Blue,
     * w = Alpha
     */
    COLOR_SPACE_RGBA,
    /**
     * @brief say that the color space uses the RGB space and is mapped as follows:
     * x = Alpha,
     * y = Red,
     * z = Green,
     * w = Blue
     */
    COLOR_SPACE_ARGB,
    /**
     * @brief say that the HSV (Hue, Saturation, Value) color space is used and is mapped as follows:
     * x = Hue,
     * y = Saturation,
     * z = Value,
     * w = Alpha
     */
    COLOR_SPACE_HSVA,
    /**
     * @brief say that the HSV (Hue, Saturation, Value) color space is used and is mapped as follows:
     * x = Alpha,
     * y = Hue,
     * z = Saturation,
     * w = Value
     */
    COLOR_SPACE_AHSV,
    /**
     * @brief say that the HSL (Hue, Saturation, Lightness) color space is used and is mapped as follows:
     * x = Hue,
     * y = Saturation,
     * z = Lightness,
     * w = Alpha
     */
    COLOR_SPACE_HSLA,
    /**
     * @brief say that the HSL (Hue, Saturation, Lightness) color space is used and is mapped as follows:
     * x = Alpha,
     * y = Hue,
     * z = Saturation,
     * w = Lightness
     */
    COLOR_SPACE_AHSL
} ColorSpace;

/**
 * @brief convert a color space enum into a c string
 * 
 * @return const char* the C string for the color space
 */
const char* GLGE_C_FUNC(colorSpaceToString)(ColorSpace space);

//check if this is C++
#if GLGE_CPP

/**
 * @brief print a color space into the output stream
 * 
 * @param os the output steam to print to
 * @param space the space to print
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, ColorSpace space) {return os << GLGE_C_FUNC(colorSpaceToString)(space);}

/**
 * @brief store the color of an object in a specific color space
 * 
 * for low dynamic range all values are expected to be in range 0 to 1, for high dynamic range the values can range from 0 to +Inf
 */
class Color
{
public:

    /**
     * @brief Construct a new Color
     */
    Color() = default;

    /**
     * @brief Construct a new Color
     * 
     * @param x the x value in respect to the correct color space mapping
     * @param y the y value in respect to the correct color space mapping
     * @param z the z value in respect to the correct color space mapping
     * @param w the w value in respect to the correct color space mapping
     * @param colorSpace the color space for the color
     */
    Color(float x, float y, float z, float w, ColorSpace colorSpace) : m_values(vec4(x,y,z,w)), m_space(colorSpace) {}

    /**
     * @brief Construct a new Color
     * 
     * @param values the values in the correct mapping and color space
     * @param colorSpace the color space for the color
     */
    Color(const vec4& values, ColorSpace colorSpace) : m_values(values), m_space(colorSpace) {}

    /**
     * @brief Get the values of the color
     * 
     * @return const vec4& the values of the color in respect to the color space
     */
    inline const vec4& getValues() const noexcept {return m_values;}

    /**
     * @brief Get the color space of the color
     * 
     * @return ColorSpace the color space of the color
     */
    inline ColorSpace getColorSpace() const noexcept {return m_space;}

    /**
     * @brief change the color space of the color to a new color space
     * 
     * @param space the new color space for the color
     */
    void convertTo(ColorSpace space);

    /**
     * @brief Get the color in a specific color space
     * 
     * @param space the space for the colors
     * @return vec4 the color of the color instance in the requested color space
     */
    inline vec4 getInSpace(ColorSpace space) const noexcept {return convertColorSpace(m_space, space, m_values);}

    /**
     * @brief Set the the color to a specifc color from a different color space without changing the color space the color instance is in
     * 
     * @param values the values in the color space
     * @param space the color space for the values
     */
    inline void setTo(const vec4& values, ColorSpace space) noexcept {m_values = convertColorSpace(space, m_space, values);}

    /**
     * @brief Set the the color to a specifc color from a different color space without changing the color space the color instance is in
     * 
     * @param x the x value for color in the color space and mapping
     * @param y the y value for color in the color space and mapping
     * @param z the z value for color in the color space and mapping
     * @param w the w value for color in the color space and mapping
     * @param space the color space for the values
     */
    inline void setTo(float x, float y, float z, float w, ColorSpace space) {setTo(vec4(x, y, z, w), space);}


    /**
     * @brief change the color space of some values
     * 
     * @param from the original color space
     * @param to the new color space
     * @param values the values in the original color space
     * @return vec4 the values in the new color space
     */
    static vec4 convertColorSpace(ColorSpace from, ColorSpace to, const vec4& values);

    /**
     * @brief Get the main 3 color values from a color discarding the alpha channel
     * 
     * @param color the color with alpha
     * @param space the color space of the color
     * @return vec3 the color components
     */
    static vec3 getColor(vec4 color, ColorSpace space);

    /**
     * @brief print a color to the console
     * 
     * @param os the output stream to print to
     * @param c the color to print
     * @return std::ostream& the filled out stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Color& c) {return os << "color{values: " << c.m_values << ", space: " << c.m_space << "}";}

    /**
     * @brief use this color to set the text color in the console
     * 
     * @return std::string the string to set the color
     */
    std::string textColor() const noexcept;

    /**
     * @brief ust this color to set the background color in the console
     * 
     * @return std::string the string to set the background color
     */
    std::string textBgColor() const noexcept;

    /**
     * @brief make the text to print bold
     * 
     * @return const char* the mode to set bold
     */
    inline static const char* textBold() noexcept {return "\x1b[1m";}

    /**
     * @brief make the text faint
     * 
     * @return const char* the mode to set faint
     */
    inline static const char* textFaint() noexcept {return "\x1b[2m";}

    /**
     * @brief make the text no longer print bold anf faint
     * 
     * @return const char* the reset code for bold and faint
     */
    inline static const char* textNoBoldFaint() noexcept {return "\x1b[22m";}

    /**
     * @brief make the text appear italic
     * 
     * @return const char* the mode to set italic
     */
    inline static const char* textItalic() noexcept {return "\x1b[3m";}

    /**
     * @brief make the text no longer appear italic
     * 
     * @return const char* the reset code for italic
     */
    inline static const char* textNoItalic() noexcept {return "\x1b[3m";}

    /**
     * @brief reset the printed color
     * 
     * @return const char* the reset code
     */
    inline static const char* textReset() noexcept {return "\x1b[0m";}

    /**
     * @brief Set the values for the color
     * 
     * @param values the values for the color in the own color space
     */
    inline void setValues(const vec4& values) noexcept {m_values = values;}

protected:

    /**
     * @brief store the values of the color in the defined color space
     */
    vec4 m_values = 0;
    /**
     * @brief the color space of the values
     */
    ColorSpace m_space = COLOR_SPACE_UNDEFINED;

};

//start a C section
extern "C" {
#endif //C++ section

/**
 * @brief store a reference of the color class
 */
typedef struct s_Color Color_t;

//end a potential C section
#if GLGE_CPP
}
#endif //end of C section

#endif