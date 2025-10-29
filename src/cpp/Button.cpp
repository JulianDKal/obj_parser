#include "Button.h"

extern float wWidth;
extern float wHeight;

Button::Button(float w, float h, glm::vec3 bgColor, glm::vec3 tColor, const char* buttonText, int fontSize)
: backgroundColor(bgColor), textColor(tColor), text(buttonText), _fontSize(fontSize), 
    textObject(fontSize, "./../src/fonts/Montserrat-Regular.ttf")
{
    backgroundShader.createProgram("./../src/shaders/standard_vertex.glsl", "./../src/shaders/diffuse_fragment.glsl");
    textShader.createProgram("./../src/shaders/glyph_vertex.glsl", "./../src/shaders/glyph_fragment.glsl");
    width = w;
    height = h;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //Buffer is empty now, gets filled in the render method with the appropriate x and y values
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3, NULL, GL_STATIC_DRAW);
    // std::cout << sizeof(vertices) << std::endl;
    

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    backgroundShader.setVec3f("color", bgColor);
    

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    getErrorCode();
}

void defaultClick(void* data)
{
    SDL_LogMessage(0, SDL_LOG_PRIORITY_INFO, "Hey, I was clicked :)");
}

void Button::HandleMouseMotion(const SDL_MouseMotionEvent &e)
{
    //std::cout << e.x << " | " << wHeight - e.y << std::endl;
    bool isHovered{isWithinBounds(e.x, wHeight - e.y)};
    //if(isHovered) SDL_LogInfo(0, "Button is hovered");
}

void Button::HandleMouseClick(const SDL_MouseButtonEvent& e, void* data)
{
    if(isWithinBounds(e.x, wHeight - e.y)){
        std::cout << "I was clicked!" << std::endl;
        if(!clickHandler) clickHandler = defaultClick;
        clickHandler(data);
    }
}

void Button::reset()
{
    
}

void Button::setClickHandler(ClickHandler newClickHandler)
{
    clickHandler = newClickHandler;
}

void Button::Render(float x, float y)
{
    glDisable(GL_DEPTH_TEST);

    //Update the button position
    if(xPos != x || yPos != y) {
        xPos = x;
        yPos = y;
        float verticesData[6][3] = {
        { xPos,     yPos + height,   1.0f},            
        { xPos,     yPos,       1.0f},
        { xPos + width, yPos,       1.0f},

        { xPos,     yPos + height,   1.0f},
        { xPos + width, yPos,       1.0f},
        { xPos + width, yPos + height,   1.0f} 
    };

    //std::cout << "xPos: " << xPos << " | " << "yPos: " << yPos << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 3 * sizeof(float), verticesData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    glm::mat4 ortho_projection = glm::ortho(0.0f, wWidth, 0.0f, wHeight);
    glUseProgram(backgroundShader.ID);
    backgroundShader.setMatrix4("projection", ortho_projection);
    glBindVertexArray(vao);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    textShader.setMatrix4("projection", ortho_projection);
    textObject.renderText(textShader, text, xPos + 10, yPos + 10, 1, textColor);

    glEnable(GL_DEPTH_TEST);
}

void Button::HandleEvent(const SDL_Event &e, void* data)
{
    if (e.type == SDL_EVENT_MOUSE_MOTION) {
      //std::cout << "mouse motion event handled" << std::endl;
      HandleMouseMotion(e.motion);
    }

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if(e.button.button == SDL_BUTTON_LEFT){
            HandleMouseClick(e.button, data);
        }
    }
}

bool Button::isWithinBounds(int x, int y)
{
    // Too far left
    if (x < xPos) return false;
    // Too far right
    if (x > xPos + width) return false;
    // Too high
    if (y < yPos) return false;
    // Too low
    if (y > yPos + height) return false;
    // Within bounds
    //SDL_LogDebug(0, "is within bounds");
    return true;
}
