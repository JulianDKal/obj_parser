#include "ErrorMessage.h"

ErrorMessage::ErrorMessage()
: messageText(40,  "./../src/fonts/Montserrat-Regular.ttf"), textShader("./../src/shaders/glyph_vertex.glsl", "./../src/shaders/glyph_fragment.glsl")
{
    errorMap = {
        std::pair<unsigned int, std::string>{1, "The selected file path is not valid!"},
        std::pair<unsigned int, std::string>{2, "The selected file is not an .obj file!"},
        std::pair<unsigned int, std::string>{3, "Something went wrong while parsing the .obj file"}
    };
}

void ErrorMessage::Activate(unsigned int errorCode)
{
    if(!active) {
        active = true;
        startLifeTime = SDL_GetTicks();
        message = errorMap[errorCode];
        std::cout << "ERROR: " << message << std::endl;
    }
}

void ErrorMessage::Render(float xPos, float yPos)
{
    currentLifeTime = SDL_GetTicks();
    if((currentLifeTime - startLifeTime) > maxLifeTime) active = false;

    if(active){ 
        glm::mat4 ortho_projection = glm::ortho(0.0f, wWidth, 0.0f, wHeight);
        textShader.setMatrix4("projection", ortho_projection);
        messageText.renderText(textShader, message, xPos, yPos, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }   
}

ErrorMessage::~ErrorMessage()
{
}