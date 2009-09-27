#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#include "ImathGL.h"

#include "Mesh.h"

Mesh::Mesh() :
    m_haveNormals(false),
    m_haveTexCoords(false),
    m_haveColors(false)
{
    
}

Mesh * Mesh::loadFile(const char * file) {
	ifstream infile(file);

	if(infile.fail()) {
        cerr << "Error opening file " << file << endl;
        return NULL;
    }

    Mesh * mesh = new Mesh;

	char current_line[1024];

	while (!infile.eof()) {
		infile.getline(current_line, 1024);

		switch (current_line[0]) {		
            case 'v':
                float x, y, z;
                switch(current_line[1])
                {
                case 'n':
                    sscanf(current_line, "vn %f %f %f", &x,&y,&z);
                    mesh->m_normals.push_back(Vec3f(x, y, z));
                    break;

                case 't':
                    sscanf(current_line, "vt %f %f", &x,&y);
                    mesh->m_textureCoords.push_back(Vec2f(x, y));
                    break;

                default:
                    sscanf(current_line, "v %f %f %f", &x,&y,&z);
                    mesh->m_vertices.push_back(Vec3f(x, y, z));
                    break;
                }
                break;
            case 'f':
                vector<string> faces = split_string(current_line, " \t.\r\n");

                int vnt[3][3] = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
                string::size_type begin, end;

                for(int i = 0;i < 2;i++)
                {
                    begin = 0;
                    int j = 0;
                    do
                    {
                        end = faces[i+1].find_first_of("/", begin);

                        if(begin < end)
                        {
                            vnt[i][j] = StrToInt(faces[i+1].substr(begin, end-begin))-1;
                        }
                        else
                            vnt[i][j] = -1;
                        begin = end+1;
                        j++;
                    }while(end != string::npos);
                }

                for(unsigned int i = 3;i < faces.size();i++)
                {
                    begin = 0;
                    int j = 0;
                    do
                    {
                        end = faces[i].find_first_of("/", begin);

                        if(begin < end)
                        {
                            vnt[2][j] = StrToInt(faces[i].substr(begin, end-begin))-1;
                        }
                        else
                            vnt[2][j] = -1;
                        begin = end+1;
                        j++;

                    }while(end != string::npos);

                    for(int j = 0;j < 3;j++)
                    {
                        mesh->m_vertexIndices.push_back(vnt[j][0]);

                        if(vnt[j][1] != -1)
                            mesh->m_textureCoordIndices.push_back(vnt[j][1]);
                        if(vnt[j][2] != -1)
                            mesh->m_normalIndices.push_back(vnt[j][2]);
                    }

                    memcpy(&vnt[1], &vnt[2], sizeof(int)*3);
                }
                break;
		}
	}

    mesh->m_haveNormals =
        mesh->m_normalIndices.size() == mesh->m_vertexIndices.size();
    mesh->m_haveTexCoords =
        mesh->m_textureCoordIndices.size() == mesh->m_vertexIndices.size();
    return mesh;
}

Mesh::~Mesh() {
    
}

int Mesh::StrToInt(const string &str) 
{
    stringstream ss;
	int i;
    ss << str;
    ss >> i;
    return i;
}

vector<string> Mesh::split_string(const string& str, const string& split_str) 
{
	vector<string> stlv_string;
	string part_string("");
	string::size_type i;
	
	i=0;
	while(i < str.size()) 
	{
		if(split_str.find(str[i]) != string::npos) 
		{
			stlv_string.push_back(part_string);
			part_string="";
			while(split_str.find(str[i]) != string::npos) 
			{
				++i;
			}
		}
		else 
		{
			part_string += str[i];
			++i;
		}
	}
	if (!part_string.empty())
		stlv_string.push_back(part_string);
	return stlv_string;
}

void Mesh::render() {
    glBegin(GL_TRIANGLES);
        for(unsigned int i=0; i<m_vertexIndices.size(); ++i) {
            if( m_haveColors )
                glColor(m_colors[m_colorIndices[i]]);
            if( m_haveNormals )
                glNormal(m_normals[m_normalIndices[i]]);
            if( m_haveTexCoords )
                glTexCoord(m_textureCoords[m_textureCoordIndices[i]]);
            glVertex(m_vertices[m_vertexIndices[i]]);
        }
    glEnd();
}

Mesh * Mesh::createUnitCylinder(Vec3<float> color, int numSides) {
    Mesh * mesh = new Mesh();
    // vectices
    for (float side = -1.0; side <= 1.0; side += 2.0) {
        float z = 0.5 * side;
        mesh->m_vertices.push_back(Vec3<float>(0.0f, 0.0f, z)); // center
        for (int i = 0; i < numSides; i++) {
            float angle = 2 * M_PI * i / numSides * side;
            mesh->m_vertices.push_back(Vec3<float>(cosf(angle) / 2.0f, sinf(angle) / 2.0f, z));
        }
    }
    // index caps
    for (int side = 0; side <= numSides + 1; side += numSides + 1) {
        for (int i = 0; i < numSides - 1; i++) {
            //mesh->m_normals.push_back(Vec3<float>(0, 0, 1));
            //mesh->m_normalIndices.push_back(mesh->m_normals.size()-1);
            triangle(mesh->m_vertexIndices, side, side + i + 1, side + i + 2);
        }
        triangle(mesh->m_vertexIndices, side, side + numSides, side + 1);
    }
    // index sides
    quad(mesh->m_vertexIndices, 1, numSides + 2, 2 * numSides + 1, 2);
    for (int i = 1; i < numSides - 1; i++)
        quad(mesh->m_vertexIndices, i + 1, 2 * numSides + 2 - i, 2 * numSides + 2 - i - 1, i + 1 + 1);
    quad(mesh->m_vertexIndices, numSides, numSides + 3, numSides + 2, 1);

    // color
    mesh->m_colors.push_back(color);
    for (int i = 0; i < numSides * 6 * 2; i++)
        mesh->m_colorIndices.push_back(0);

    mesh->m_haveColors = true;
    mesh->m_haveNormals = false;
    mesh->m_haveTexCoords = false;
    return mesh;
}

Mesh * Mesh::createUnitCube(Vec3<float> color) {
    Mesh* mesh = new Mesh();
    // baseball lace pattern
    mesh->m_vertices.push_back(Vec3<float>(-0.5f, -0.5f, -0.5f));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f, -0.5f, -0.5f));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f,  0.5f, -0.5f));
    mesh->m_vertices.push_back(Vec3<float>(-0.5f,  0.5f, -0.5f));
    mesh->m_vertices.push_back(Vec3<float>(-0.5f,  0.5f,  0.5f));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f,  0.5f,  0.5f));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f, -0.5f,  0.5f));
    mesh->m_vertices.push_back(Vec3<float>(-0.5f, -0.5f,  0.5f));

    quad(mesh->m_vertexIndices, 3, 2, 1, 0); // -z
    quad(mesh->m_vertexIndices, 3, 0, 7, 4); // -x
    quad(mesh->m_vertexIndices, 0, 1, 6, 7); // -y
    quad(mesh->m_vertexIndices, 1, 2, 5, 6); // +x
    quad(mesh->m_vertexIndices, 2, 3, 4, 5); // +y
    quad(mesh->m_vertexIndices, 5, 4, 7, 6); // +z

    // all the colors are the same
    mesh->m_colors.push_back(color);
    for (unsigned int i = 0; i < mesh->m_vertexIndices.size(); i++)
        mesh->m_colorIndices.push_back(0);

    mesh->m_haveColors = true;
    mesh->m_haveNormals = false;
    mesh->m_haveTexCoords = false;
    return mesh;
}

Mesh * Mesh::createUnitPlane(Vec3<float> color) {
    Mesh * mesh = new Mesh();

    mesh->m_vertices.push_back(Vec3<float>(-0.5f, -0.5f, 0));
    mesh->m_vertices.push_back(Vec3<float>(-0.5f,  0.5f, 0));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f,  0.5f, 0));
    mesh->m_vertices.push_back(Vec3<float>( 0.5f, -0.5f, 0));

    quad(mesh->m_vertexIndices, 3, 2, 1, 0);

    // same colors and normals
    mesh->m_colors.push_back(color);
    mesh->m_normals.push_back(Vec3<float>(0, 0, 1));
    for(unsigned int i=0; i < mesh->m_vertexIndices.size(); ++i){
        mesh->m_colorIndices.push_back(0);
        mesh->m_normalIndices.push_back(0);
    }

    mesh->m_haveColors = true;
    mesh->m_haveNormals = true;
    mesh->m_haveTexCoords = false;
    return mesh;
}

void Mesh::quad(vector<int> & vertexIndices, int v1, int v2, int v3, int v4) {
    triangle(vertexIndices, v1, v2, v3);
    triangle(vertexIndices, v1, v3, v4);
}

void Mesh::triangle(vector<int> & vertexIndices, int v1, int v2, int v3) {
    vertexIndices.push_back(v1);
    vertexIndices.push_back(v2);
    vertexIndices.push_back(v3);
}

