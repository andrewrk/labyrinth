#include "MazeView.h"

MazeView::MazeView(Maze & maze, Vec3<float> pos, Vec3<float> size) :
    m_maze(maze),
    m_pos(pos),
    m_size(size)
{
    m_sectorSize.x = size.x / (float)maze.width();
    m_sectorSize.y = size.y / (float)maze.height();
    m_sectorSize.z = size.z;
}

MazeView::~MazeView() {

}

void MazeView::display() {
    // display the floor
    // rectangle from (
    
    // display each wall

}
