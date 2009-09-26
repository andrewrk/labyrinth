#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#include "GL/freeglut.h"

#include "Mesh.h"

Mesh::Mesh(const char* file) {
    loadFile(file);
}

void Mesh::loadFile(const char * file) {
	ifstream infile(file);

	if(infile.fail())
        throw "unable to open file";

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
                    m_normals.push_back(Vec3f(x, y, z));
                    break;

                case 't':
                    //sscanf(current_line, "vt %f %f %f", &x,&y,&z); // z component is not used
                    sscanf(current_line, "vt %f %f", &x,&y); // z component is not used
                    m_textureCoords.push_back(Vec2f(x, y));
                    break;

                default:
                    sscanf(current_line, "v %f %f %f", &x,&y,&z);
                    m_vertices.push_back(Vec3f(x, y, z));
                    break;
                }
                break;
            case 'f':
                vector<string> faces = split_string(current_line, " \t.\r\n");

                int vnt[3][3] = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
                string::size_type begin, end;

                for(int i = 0;i < 2;i++)
                {
                    //cout<<"f: "<<i<<" "<<faces[i+1]<<endl;
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
                    //cout<<"f: "<<i-1<<" "<<faces[i]<<endl;
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
                        m_vertexIndices.push_back(vnt[j][0]);

                        if(vnt[j][1] != -1)
                            m_textureCoordIndices.push_back(vnt[j][1]);
                        if(vnt[j][2] != -1)
                            m_normalIndices.push_back(vnt[j][2]);
                    }

                    memcpy(&vnt[1], &vnt[2], sizeof(int)*3);
                }
                break;
		}
	}
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
            Vec3<float> v = m_vertices[m_vertexIndices[i]];
            glVertex3f(v.x, v.y, v.z);
        }
    glEnd();
}

