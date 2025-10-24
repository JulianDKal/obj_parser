#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include "GL/glew.h"
#include "glm/glm.hpp"

struct Point {
    int x, y;
    Point(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
    bool operator ==(Point other) {
        return (other.x == this->x && other.y == this->y);
    }
    Point() {
        x = 0;
        y = 0;
    }
};

inline std::ostream& operator <<(std::ostream& os, const Point& p) {
    os << p.x << " " << p.y;
    return os;
}

// const inline void printStreamHexadecimal(void* stream, unsigned int length, unsigned int lineLength){
//     Uint8* byteStream = (Uint8*)stream;
//     std::cout << std::hex;
//     int counter = 0;
    
//     for (int i = 0; i < length; i++)
//     {
//         std::cout << "0x" << (unsigned int)byteStream[i] << " ";

//         if(++counter == lineLength) {
//             std::cout << '\n';
//             counter = 0;
//         }
//     }
    
// }

// inline SDL_Color getPixelColor(SDL_Surface* surface, int x, int y) {

//     const int bpp = SDL_BYTESPERPIXEL(surface->format); //bytes per pixel, 4 for a 32 bit depth
//     //std::cout << bpp << std::endl; 

//     /*
// 	Retrieve the address to a specific pixel
// 	pSurface->pixels	= an array containing the SDL_Surface' pixels
// 	pSurface->pitch		= the length of a row of pixels (in bytes)
// 	X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
// 	*/
//     Uint8* pixelAddress = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
//     //Cast the Uint8 address to a Uint32 address, so that when we dereference it we can the next 32 bit at the pixel address
//     Uint32 pixelData = *(Uint32*)pixelAddress;
//     //printStreamHexadecimal(surface->pixels, surface->h * surface->pitch, surface->pitch);
//     SDL_Color returnColor = {0, 0, 0, 0};
//     const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(surface->format);
//     SDL_GetRGBA(pixelData, details, NULL, &returnColor.r, &returnColor.g, &returnColor.b, &returnColor.a);
//     //std::cout << (int)returnColor.r << " " << (int)returnColor.g << " " << (int)returnColor.b << std::endl;

//     return returnColor;
// }

inline GLenum getErrorCode_(const char* file, int line) {
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }

    return errorCode;
}

#define getErrorCode() getErrorCode_(__FILE__, __LINE__)

// std::string getShaderText(std::filesystem::path path){
//     std::ifstream fileStr;
//     fileStr.open(path);

//     std::stringstream sstream;
//     sstream << fileStr.rdbuf();

//     char c;
//     std::string returnStr;

//     // while (fileStr.get(c))
//     // {
//     //     returnStr += c;
//     // }
//     returnStr = sstream.str();
//     return returnStr;
    
// }
