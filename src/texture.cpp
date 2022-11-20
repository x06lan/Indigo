#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"
Texture::Texture(const int width, const int height,textureType type,textureFormat format) {
    LOG_TRACE("Creating Texture");

    m_Format=format;
    glCreateTextures(format, 1, &m_TextureID);
    glBindTexture(format, m_TextureID);

    if(format==textureFormat::CUBE)
        for(int i=0;i<6;i++)
            glTexImage2D(         //
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,    // target
                0,                // level
                type,           // internal format
                width,            //
                height,           //
                0,                // border
                type,           // format
                GL_UNSIGNED_BYTE, // type
                NULL              //
            );

    else
    glTexImage2D(         //
        format,    // target
        0,                // level
        type,           // internal format
        width,            //
        height,           //
        0,                // border
        type,           // format
        GL_UNSIGNED_BYTE, // type
        NULL              //
    );

    glTexParameteri(format, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(format, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(const std::string &textureFilepath) {
    LOG_TRACE("Creating Texture");

    m_Format=textureFormat::TEXTURE;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

    LoadImage(textureFilepath);
}

Texture::~Texture() {
    LOG_TRACE("Deleting Texture");
    glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind(unsigned int slot) {
    LOG_TRACE("Binding Texture");
    glBindTextureUnit(slot, m_TextureID);
}
void Texture::Unbind() {
    glBindTexture(m_Format, 0);
}

void Texture::LoadImage(const std::string &textureFilepath) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    m_Format=textureFormat::TEXTURE;

    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data =
        stbi_load(textureFilepath.c_str(), &width, &height, &channels, 0);

    if (data == NULL) {
        LOG_ERROR("Failed Opening File: '{}'", textureFilepath);
        throw;
    }

    glTexImage2D(                           //
        GL_TEXTURE_2D,                      // target
        0,                                  // level
        channels == 4 ? GL_RGBA8 : GL_RGB8, // internal format
        width,                              //
        height,                             //
        0,                                  // border
        channels == 4 ? GL_RGBA : GL_RGB,   // format
        GL_UNSIGNED_BYTE,                   // type
        data                                //
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

GLuint Texture::GetTextureLocation(const GLuint &programID,
                                   const std::string &uniformName) {
    return glGetUniformLocation(programID, uniformName.c_str());
}