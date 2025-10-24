#include "Texture.h"

Texture::Texture()
{
    textureHandle = 0;
    textureUnitIndex = 0;
}

void Texture::initialize(const std::string& imagePath, unsigned int texUnit)
{
    getErrorCode();
    textureUnitIndex = texUnit;

    if(!std::filesystem::exists(imagePath)) {
        std::cerr << "The image path specified could not be found: " << imagePath << std::endl;
    }
    glGenTextures(1, &textureHandle);
    glActiveTexture(GL_TEXTURE0 + textureUnitIndex);

    getErrorCode();
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    getErrorCode();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* gridImageData = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 3);

    if(gridImageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, gridImageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    getErrorCode();
    }
    else {
        std::cerr << "STB_image error: " << stbi_failure_reason() << std::endl;
    }

    stbi_image_free(gridImageData);
}

unsigned int Texture::getTextureUnit() const
{
    return textureUnitIndex;
}

unsigned int Texture::getHandle() const
{
    return textureHandle;
}
