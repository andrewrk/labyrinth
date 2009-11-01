#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <string>
using namespace std;

#include "bitmap.h"

class Bitmap {
    public: 
        Bitmap(string file);
        ~Bitmap();

        inline unsigned char * buffer() { return m_bits; }
        inline BITMAPINFO * info() { return m_info; }

    private:

        unsigned char * m_bits;
        BITMAPINFO * m_info;
};

#endif

