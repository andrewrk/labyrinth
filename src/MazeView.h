#ifndef _MAZE_VIEW_H_
#define _MAZE_VIEW_H_

#include "Maze.h"

#include "ImathVec.h"
using namespace Imath;

class MazeView {
    public:
        // size.x - x component of the 2d maze
        // size.y - y compenent of the 2d maze
        // size.z - how tall the walls go up
        MazeView(Maze & maze, Vec3<float> pos, Vec3<float> size);
        ~MazeView();

        // render with opengl
        void display();

    private:
        Maze & m_maze;
        Vec3<float> m_pos;
        Vec3<float> m_size;
        Vec3<float> m_sectorSize;
};

#endif

