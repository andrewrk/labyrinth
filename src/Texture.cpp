#include "Texture.h"

#include "Bitmap.h"

#include "GL/freeglut.h"

Texture::Texture(Bitmap * bmp)
    : m_bmp(bmp)
{
    glGenTextures(1, &m_id);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        bmp->info()->bmiHeader.biWidth,
        bmp->info()->bmiHeader.biHeight,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        bmp->buffer()
    );


}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}


void Texture::bind() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

