#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <filesystem>
#include <math.h>
#include "Shader.h"
#include "ObjParser.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "Helpers.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "Text.h"
#include "Button.h"
#include "filesystem"
#include "ErrorMessage.h"
#include "Texture.h"

const double PI = 3.14159265;

struct Light {
    glm::vec3 color;
    glm::vec3 pos;
};

//Used only for passing it into the void* of the fileDialogHandler function
struct passArguments {
    SDL_Window* window;
    bool* newFileReady;
    std::string* newFilePath;
};

float wWidth = 1920.0f;
float wHeight = 1200.0f;

GLenum mode = GL_FILL;

static const SDL_DialogFileFilter filters[] = {
    {"3D Models", "obj;fbx;gltf"}
};

static void SDLCALL callback(void* userdata, const char* const* filelist, int filter)
{
    if (!filelist) {
        SDL_Log("An error occured: %s", SDL_GetError());
        return;
    } else if (!*filelist) {
        SDL_Log("The user did not select any file.");
        SDL_Log("Most likely, the dialog was canceled.");
        return;
    }

    passArguments* arguments = (passArguments*)userdata;
    *arguments->newFileReady = true;
    *arguments->newFilePath = *filelist;

    if (filter < 0) {
        SDL_Log("The current platform does not support fetching "
                "the selected filter, or the user did not select"
                " any filter.");
        return;
    } else if (filter < SDL_arraysize(filters)) {
        SDL_Log("The filter selected by the user is '%s' (%s).",
                filters[filter].pattern, filters[filter].name);
        return;
    }
}


void wireFrameClickHandler(void* data) {
    if(mode == GL_FILL) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mode = GL_LINE;
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mode = GL_FILL;
    }
}

void fileDialogueHandler(void* data) {
    passArguments* userData = (passArguments*)data;
    SDL_ShowOpenFileDialog(callback, userData, userData->window, filters, SDL_arraysize(filters), "/Users/", true);
}    

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window = SDL_CreateWindow("OBJ Parser",
                                          wWidth, wHeight,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_SetWindowMinimumSize(window, 800, 600);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLEW after OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    float gridVertices[] = {
     // Positions          // UVs (texture coordinates)
    -250.0f, 0.0f,  250.0f,  0.0f, 25.0f,  // Top-left
     250.0f, 0.0f,  250.0f,  25.0f, 25.0f,  // Top-right
     250.0f, 0.0f, -250.0f,  25.0f, 0.0f,  // Bottom-right
    -250.0f, 0.0f, -250.0f,  0.0f, 0.0f   // Bottom-left
    };

    float lightVertices[] = {
        // Positions         // UVs
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top-right
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top-left
    };

    Shader teapotShader("./../src/shaders/teapot_vertex.glsl", "./../src/shaders/teapot_fragment.glsl");
    Shader groundShader("./../src/shaders/gridVertex.glsl", "./../src/shaders/gridFragment.glsl");
    Shader lightShader("./../src/shaders/gridVertex.glsl", "./../src/shaders/lightFragment.glsl");
    
    //Buffers for the grid
    unsigned int grid_VBO, grid_VAO;
    glGenVertexArrays(1, &grid_VAO);
    glGenBuffers(1, &grid_VBO);

    glBindVertexArray(grid_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int light_VBO, light_VAO;
    glGenVertexArrays(1, &light_VAO);
    glGenBuffers(1, &light_VBO);

    glBindVertexArray(light_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, light_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ObjParser objParser;
    objParser.Parse("./../src/models/penguin.obj");
    teapotShader.setFloat("scale", 1.0f);

    getErrorCode();

    ErrorMessage errMessage;
    Texture gridTexture;
    gridTexture.initialize("./../src/images/grid.jpg", 0);

    Texture lightTexture;
    lightTexture.initialize("./../src/images/sun.png", 0);

    getErrorCode();

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::mat4 object_model = glm::mat4(1.0f);
    glm::mat4 object_view = glm::mat4(1.0f);
    glm::mat4 object_projection = glm::mat4(1.0f);

    float modifierX = 0.0f;
    float modifierY = 0.0f;

    float rotationModifierX = 0.0f;
    float rotationModifierY = 0.0f;
    float scaleModifier = -3.0f;

    float radius = 8.0f;

    Light light = {glm::vec3(1.0, 1.0, 1.0), glm::vec3(5.0, 5.0, 5.0)};

    bool newFileReady = false;
    std::string filePath = "";

    passArguments arguments{ window, &newFileReady, &filePath};
    passArguments* argumentsPointer = &arguments;

    Button wireFrameButton(220.0f, 45.0f, glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), "Wireframe", 32);
    wireFrameButton.setClickHandler(wireFrameClickHandler);

    Button fileDialogueButton(220.0f, 45.0f, glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), "Open File", 32);
    fileDialogueButton.setClickHandler(fileDialogueHandler);
    // Main loop
    bool running = true;
    while (running) {
        SDL_Event event;

        if(*arguments.newFileReady) {
            std::cout << "New file path is: " << *arguments.newFilePath << std::endl;
            *arguments.newFileReady = false;

            if(!std::filesystem::exists(*arguments.newFilePath)) {
                errMessage.Activate(ERR_PATH_NOT_VALID);
                }
            if(std::filesystem::path(*arguments.newFilePath).extension() != ".obj") {
                errMessage.Activate(ERR_NOT_AN_OBJ_FILE);
                }

            int errorCode = objParser.Parse(*arguments.newFilePath);
            if(errorCode != 0) errMessage.Activate(errorCode);
        }

        while (SDL_PollEvent(&event)) {
            wireFrameButton.HandleEvent(event, (void*)window);
            fileDialogueButton.HandleEvent(event, (void*)argumentsPointer);

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.key == SDLK_W){
                    mode = (mode == GL_FILL) ? GL_LINE : GL_FILL;
                    glPolygonMode(GL_FRONT_AND_BACK, mode);
                }
                if(event.key.key == SDLK_RIGHT){
                    modifierX += 0.1f;
                }
                if(event.key.key == SDLK_LEFT){
                    modifierX -= 0.1f;
                }
                if(event.key.key == SDLK_UP){
                    modifierY += 0.1f;
                }
                if(event.key.key == SDLK_DOWN){
                    modifierY -= 0.1f;
                }
            }
            else if(event.type == SDL_EVENT_MOUSE_MOTION){
                if(event.motion.state == SDL_BUTTON_LEFT){
                    rotationModifierX -= event.motion.xrel * 0.005f;
                    rotationModifierY += event.motion.yrel * 0.005f;
                    rotationModifierY = glm::clamp((double)rotationModifierY, 0.0, PI / 2.05f);
                }
            }
            else if(event.type == SDL_EVENT_MOUSE_WHEEL) {
                scaleModifier += event.wheel.y * 0.4f;
                radius -= event.wheel.y * 0.4f;
            }
            else if(event.type == SDL_EVENT_WINDOW_RESIZED) {
                wWidth = event.window.data1;
                wHeight = event.window.data2;
                glViewport(0, 0, wWidth, wHeight);
            }

        }

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(SDL_GetTicks() * 0.05f), glm::vec3(1, 1, 0));
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(modifierX, modifierY, -3.0f)); 
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), wWidth / wHeight, 0.1f, 250.0f);

        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object_model = glm::mat4(1.0f);
        object_model = glm::translate(object_model, glm::vec3(0, -1.5, 0));

        //camera calculations
        
        float camX = sin(rotationModifierX) * cos(rotationModifierY) * radius;
        float camY = sin(rotationModifierY) * radius;
        float camZ = cos(rotationModifierX) * cos(rotationModifierY) * radius;
        
        glm::vec3 directionVector = glm::normalize( glm::vec3(0 - camX, 0 - camY, 0 - camZ));
        glm::vec3 globalUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 rightVector = glm::normalize(glm::vec3(glm::cross(directionVector, globalUp)));

        glm::vec3 upVector = glm::cross(rightVector, glm::vec3(-camX, -camY, -camZ));

        object_view = glm::mat4(1.0f);
        object_view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), upVector); 

        //perspective projection
        object_projection = glm::mat4(1.0f);
        object_projection = glm::perspective(glm::radians(45.0f), wWidth / wHeight, 0.1f, 100.0f);


        glUseProgram(teapotShader.ID);
        // teapotShader.setBool("useTexture", true);
        teapotShader.setMatrix4("model", object_model);
        teapotShader.setMatrix4("view", object_view);
        teapotShader.setMatrix4("projection", object_projection);
        teapotShader.setVec3f("lightColor", light.color);
        teapotShader.setVec3f("lightPos", light.pos);
        teapotShader.useTexture(objParser.getTexture(), "tex");
        objParser.Draw();
        
        getErrorCode();

        //Draw Grid and UI

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(grid_VAO);
        glUseProgram(groundShader.ID);
        groundShader.useTexture(gridTexture, "gridTexture");
        groundShader.setMatrix4("model", object_model);
        groundShader.setMatrix4("view", object_view);
        groundShader.setMatrix4("projection", object_projection);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        getErrorCode();

        glm::mat4 object_light(1.0f);
        object_light = glm::translate(object_light, glm::vec3(5.0, 5.0, 5.0));
        glm::mat4 view_light(1.0f);

        glBindVertexArray(light_VAO);
        glUseProgram(lightShader.ID);
        lightShader.useTexture(lightTexture, "lightTexture");
        lightShader.setMatrix4("model", object_light);
        lightShader.setMatrix4("view", object_view);
        lightShader.setMatrix4("projection", object_projection);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        wireFrameButton.Render(wWidth * 0.01f, wHeight - (wHeight * 0.1f));
        fileDialogueButton.Render(wWidth* 0.15, wHeight - (wHeight * 0.1f));
        errMessage.Render(wWidth * 0.25f, wHeight - (wHeight * 0.95f));
        glPolygonMode(GL_FRONT_AND_BACK, mode);


        getErrorCode();

        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
