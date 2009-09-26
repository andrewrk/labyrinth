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

        void render();

    private:
        typedef Vec3<float> Vec3f;
        typedef Vec2<float> Vec2f;

        vector< Vec3<float> > m_vertices;
        vector< Vec3<float> > m_normals;
        vector< Vec2<float> > m_textureCoords;

        vector<int> m_vertexIndices;
        vector<int> m_normalIndices;
        vector<int> m_textureCoordIndices;

        void loadFile(const char * file);

        static vector<string> split_string(const string& str,
            const string& split_str);
        static int StrToInt(const string &str);
};

#endif
