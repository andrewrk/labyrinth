#include "Drawable.h"

#include <iostream>
using namespace std;

#include "GL/freeglut.h"

Drawable::Drawable() :
    m_useListRendering(true)
{
    // allocate a glList
    m_listId = glGenLists(1);
}

Drawable::~Drawable() {
    // deallocate the glList
    glDeleteLists(m_listId, 1);
}

void Drawable::init() {
    // pre-render the object
    glNewList(m_listId, GL_COMPILE);
        render();
    glEndList();
}

void Drawable::draw() {
    if( m_useListRendering )
        glCallList(m_listId);
    else
        render();
}
