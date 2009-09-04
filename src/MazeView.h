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
        MazeView(const Maze &maze, const Vec3<float> &pos,
            const Vec3<float> &size);
        ~MazeView();

        // render with opengl
        void display();

    private:

        const Maze & m_maze;
        const Vec3<float> & m_pos;
        const Vec3<float> & m_size;
        Vec3<float> m_sectorSize;
        Vec3<float> m_postSize;

        void cuboid(Vec3<float> basePt1, Vec3<float> basePt2,
            Vec3<float> basePt3, Vec3<float> basePt4, float height);
};

#endif

