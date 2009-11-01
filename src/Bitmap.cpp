#include "Bitmap.h"
#include "bitmap.h"

Bitmap::Bitmap(string file) :
    m_bits(NULL),
    m_info(NULL)
{
    m_bits = LoadDIBitmap(file.c_str(), &m_info);
}

Bitmap::~Bitmap() {
    if( m_bits )
        delete m_bits;

    if( m_info )
        delete m_info; 
}


