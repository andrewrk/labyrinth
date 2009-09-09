#ifndef _MESH_H_
#define _MESH_H_

#include "Drawable.h"

#include <vector>
#include <string>
using namespace std;

#include "ImathVec.h"
using namespace Imath;

class Mesh : public Drawable {
    public:
        Mesh(const char* file);
        ~Mesh();

    private:
        vector< Vec3<float> > m_v; // array of vertices
        vector< Vec3<float> > m_n; // array of normals
        vector< Vec2<float> > m_texc; // array of 2d texture coordinates

        vector<int> m_vi; // vertex indices
        vector<int> m_ni; // normal indices
        vector<int> m_ti; // textcoord indices

};

#endif
