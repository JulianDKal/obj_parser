#include "Text.h"
#include <chrono>
#include "Shader.h"
#include "SDL3/SDL.h"

extern float wWidth, wHeight;

class ErrorMessage
{
private:
    bool active = false;
    std::unordered_map<unsigned int, std::string> errorMap;
    std::string message;
    Text messageText;
    Shader textShader;
    Uint64 currentLifeTime, startLifeTime = 0, maxLifeTime = 3000;
public:
    void Activate(unsigned int errorCode);
    void Render(float xPos, float yPos);
    ErrorMessage();
    ~ErrorMessage();
};

