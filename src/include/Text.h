#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include "Shader.h"
#include "ft2build.h"
#include "Helpers.h"

#include FT_FREETYPE_H  

struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    signed long Advance;    // Offset to advance to next glyph
};

/*!
    @class Text
    @brief A wrapper for one text element, includes a hashmap for generated with the freetype library to render different characters

    See here for more information: 
    @ref https://freetype.org/
*/
class Text
{
private:
    int textHeight;
    std::unordered_map<char, Character> charMap;
    unsigned int VAO, VBO;
public:
    /*!
    @brief renders the text
    *
    @param x position of the text on the screen in x direction
    @param y position of the text on the screen in y direction (higher number means lower on the screen)
    @param scale The width and height of the rendered quad get scaled by this value, 1 means no change in scale 
    */
    void renderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color);
    Text() = delete;
    Text(int size, const char* fontPath);
    ~Text();
};
