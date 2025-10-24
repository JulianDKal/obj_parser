#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include <iostream>
#include "SDL3_ttf/SDL_ttf.h"
#include "ft2build.h"
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"
#include "Helpers.h"
#include "Text.h"

class Button
{
using ClickHandler = std::function<void(void* data)>;

private:
    float width, height, xPos = -1, yPos = -1;
    glm::vec3 backgroundColor, textColor;
    std::string text;
    std::string textFont = "./../src/fonts/Montserrat-Regular.ttf";
    int _fontSize = 40;
    Shader backgroundShader, textShader;
    Text textObject;
    
    unsigned int vao, vbo;
    float vertices[6][3];

    void HandleMouseMotion(const SDL_MouseMotionEvent& e);
    void HandleMouseClick(const SDL_MouseButtonEvent& e, void* data);

    ClickHandler clickHandler;
    
public:
    Button(float w, float h, glm::vec3 bgColor, glm::vec3 tColor, const char* buttonText, int fontSize);
    void reset(); //resets color values etc. so that the button doesn't stay changed after reentering the scene
    void setClickHandler(ClickHandler newHandler);
    void Render(float x, float y);
    void HandleEvent(const SDL_Event& e, void* data);
    bool isWithinBounds(int x, int y);
};
