#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Bitmap.h"
#include "bitmap.h"

class Texture {
    public:
        enum Mode {
            ModeReplace,
            ModeBlend,
            ModeOff
        };

        Texture(Bitmap * bitmap);
        ~Texture();

        static void setMode(Mode mode);

        inline Bitmap * bitmap() { return m_bmp; }
        void bind();
    private:
        Bitmap * m_bmp;
        unsigned int m_id;
        static Mode s_mode;
};

#endif

