#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Bitmap.h"
#include "bitmap.h"

class Texture {
    public:
        Texture(Bitmap * bitmap);
        ~Texture();

        inline Bitmap * bitmap() { return m_bmp; }
        void bind();
    private:
        Bitmap * m_bmp;
        unsigned int m_id;
};

#endif

