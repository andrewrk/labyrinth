#include "MazeView.h"

#include "GL/glut.h"

MazeView::MazeView(const Maze &maze, const Vec3<float> &pos,
        const Vec3<float> & size) :
    m_maze(maze),
    m_pos(pos),
    m_size(size)
{
    m_sectorSize.x = size.x / (float)maze.width();
    m_sectorSize.y = size.y / (float)maze.height();
    m_sectorSize.z = size.z;

    m_wallSize.x = m_sectorSize.x / 10;
    m_wallSize.y = m_sectorSize.y / 10;
    m_wallSize.z = m_sectorSize.z;
}

MazeView::~MazeView() {

}

void MazeView::display() {
    // display the floor

    // display each wall
    for(int x=0; x<m_maze.width(); ++x) {
        for(int y=0; y<m_maze.height(); ++y) {
            // rectangle from m_pos+x*m_sectorSize to m_pos+(x+1)*sectorSize
            Vec3<float> loc( m_pos.x + ((float)x) * m_sectorSize.x,
                m_pos.y + ((float)y) * m_sectorSize.y, 0);

            // floor
            //glColor3f(0.1, 0.1, 0.9);
            //glBegin(GL_POLYGON);
                //glVertex3f(loc.x, loc.y, loc.z);
                //glVertex3f(loc.x, loc.y+m_sectorSize.y, loc.z);
                //glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y, loc.z);
                //glVertex3f(loc.x+m_sectorSize.x, loc.y, loc.z);
            //glEnd();
            
            //// north
            //if( m_maze.cellHasWall(x,y,Maze::North) ) {
                //glColor3f(0.9, 0.1, 0.1);
                //glBegin(GL_POLYGON);
                    //glVertex3f(loc.x, loc.y, loc.z);
                    //glVertex3f(loc.x+m_sectorSize.x, loc.y, loc.z);
                    //glVertex3f(loc.x+m_sectorSize.x, loc.y,
                        //loc.z+m_sectorSize.z);
                    //glVertex3f(loc.x, loc.y, loc.z+m_sectorSize.z);
                //glEnd();
            //}

            // south
            if( m_maze.cellHasWall(x,y,Maze::South) ) {
                glColor3f(0.1, 0.9, 0.1);
                glBegin(GL_POLYGON);
                    glVertex3f(loc.x, loc.y+m_sectorSize.y, loc.z);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y,
                        loc.z);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y,
                        loc.z+m_sectorSize.z);
                    glVertex3f(loc.x, loc.y+m_sectorSize.y,
                        loc.z+m_sectorSize.z);
                glEnd();
            }

            // east
            if( m_maze.cellHasWall(x,y,Maze::East) ) {
                glColor3f(0.4, 0.4, 0.1);
                glBegin(GL_POLYGON);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y, loc.z);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y,
                        loc.z);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y,
                        loc.z+m_sectorSize.z);
                    glVertex3f(loc.x+m_sectorSize.x, loc.y,
                        loc.z+m_sectorSize.z);
                glEnd();
            }
            
            //// west
            //if( m_maze.cellHasWall(x,y,Maze::East) ) {
                //glColor3f(0.1, 0.4, 0.4);
                //glBegin(GL_POLYGON);
                    //glVertex3f(loc.x, loc.y, loc.z);
                    //glVertex3f(loc.x, loc.y+m_sectorSize.y, loc.z);
                    //glVertex3f(loc.x, loc.y+m_sectorSize.y,
                        //loc.z+m_sectorSize.z);
                    //glVertex3f(loc.x, loc.y, loc.z+m_sectorSize.z);
                //glEnd();
            //}
        }
    }

}
