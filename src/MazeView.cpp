#include "MazeView.h"

#include <iostream>
using namespace std;

#include "GL/freeglut.h"

MazeView::MazeView(const Maze &maze, const Vec3<float> &pos,
        const Vec3<float> & size) :
    m_maze(maze),
    m_pos(pos),
    m_size(size)
{
    m_sectorSize.x = size.x / (float)maze.width();
    m_sectorSize.y = size.y / (float)maze.height();
    m_sectorSize.z = size.z;

    m_postSize.x = m_sectorSize.y / 10;
    m_postSize.y = m_sectorSize.x / 10;
    m_postSize.z = m_sectorSize.z * 1.2;

    createPosts();
}

MazeView::~MazeView() {
    
}

Vec3<float> MazeView::getCornerLoc(int x, int y){
    return Vec3<float>(
        m_pos.x + ((float)x) * m_sectorSize.x,
        m_pos.y + ((float)y) * m_sectorSize.y, 0);
}

void MazeView::createPosts() {
    m_posts.clear();
    int x,y;
    Vec3<float> loc;
    for(x=0; x<m_maze.width(); ++x) {
        for(y=0; y<=m_maze.height(); ++y) {
            if( (y > 0 && m_maze.cellHasWall(x,y-1,Maze::West)    ) ||
                (y > 0 && (m_maze.cellHasWall(x,y-1,Maze::South)) ) ||
                (y < m_maze.height() && m_maze.cellHasWall(x,y,Maze::West)) ||
                (x > 0 && y > 0 && (m_maze.cellHasWall(x-1,y-1,Maze::South))))
            {
                m_posts.push_back(getCornerLoc(x,y));
            }
        }
        y = 0;
        if( m_maze.cellHasWall(x,y,Maze::North) ||
            (x > 0 && (m_maze.cellHasWall(x-1,y,Maze::South))) )
        {
            m_posts.push_back(getCornerLoc(x,y));
        }
    }
    x = m_maze.width()-1;
    for(y=0; y<=m_maze.height(); ++y){
        if( (y > 0 && m_maze.cellHasWall(x,y-1,Maze::East)) ||
            (y < m_maze.height() && m_maze.cellHasWall(x,y,Maze::East)) )
        {
            m_posts.push_back(getCornerLoc(x+1,y));
        }
    }
}

void MazeView::render() {
    // display each wall
    for(int x=0; x<m_maze.width(); ++x) {
        for(int y=0; y<m_maze.height(); ++y) {
            // rectangle from m_pos+x*m_sectorSize to m_pos+(x+1)*sectorSize
            Vec3<float> loc(
                m_pos.x + ((float)x) * m_sectorSize.x,
                m_pos.y + ((float)y) * m_sectorSize.y, 0);

            // floor
            glColor3f(0.1, 0.1, 0.5);
            glBegin(GL_POLYGON);
                glVertex3f(loc.x+m_sectorSize.x, loc.y, loc.z);
                glVertex3f(loc.x+m_sectorSize.x, loc.y+m_sectorSize.y,
                    loc.z);
                glVertex3f(loc.x, loc.y+m_sectorSize.y, loc.z);
                glVertex3f(loc.x, loc.y, loc.z);
            glEnd();
            
            // south
            if( m_maze.cellHasWall(x,y,Maze::South) ) {
                glColor3f(0.2, 0.8, 0.2);
                horizWall(loc+Vec3<float>(0, m_sectorSize.y, 0));
            }
            // north
            if( y == 0 && m_maze.cellHasWall(x,y,Maze::North) ) {
                glColor3f(0.2, 0.8, 0.2);
                horizWall(loc);
            }

            // east
            if( m_maze.cellHasWall(x,y,Maze::East) ) {
                glColor3f(0.4, 0.4, 0.1);
                vertWall(loc+Vec3<float>(m_sectorSize.x, 0, 0));
            }
            // west
            if( x == 0 && m_maze.cellHasWall(x,y,Maze::West) ) {
                glColor3f(0.4, 0.4, 0.1);
                vertWall(loc);
            }
        }
    }

    // render posts
    glColor3f(0.59, 0.65, 0.74);
    for(unsigned int i=0; i<m_posts.size(); ++i)
        renderPost(m_posts[i]);
}

void MazeView::cuboid(Vec3<float> basePt1, Vec3<float> basePt2,
    Vec3<float> basePt3, Vec3<float> basePt4, float height)
{
    // box
    glBegin(GL_QUAD_STRIP);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z+height);
        glVertex3f(basePt2.x, basePt2.y, basePt2.z);
        glVertex3f(basePt2.x, basePt2.y, basePt2.z+height);
        glVertex3f(basePt3.x, basePt3.y, basePt3.z);
        glVertex3f(basePt3.x, basePt3.y, basePt3.z+height);
        glVertex3f(basePt4.x, basePt4.y, basePt4.z);
        glVertex3f(basePt4.x, basePt4.y, basePt4.z+height);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z+height);
    glEnd();

    // put on caps
    glBegin(GL_POLYGON);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z);
        glVertex3f(basePt2.x, basePt2.y, basePt2.z);
        glVertex3f(basePt3.x, basePt3.y, basePt3.z);
        glVertex3f(basePt4.x, basePt4.y, basePt4.z);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex3f(basePt4.x, basePt4.y, basePt4.z+height);
        glVertex3f(basePt3.x, basePt3.y, basePt3.z+height);
        glVertex3f(basePt2.x, basePt2.y, basePt2.z+height);
        glVertex3f(basePt1.x, basePt1.y, basePt1.z+height);
    glEnd();
}

void MazeView::horizWall(Vec3<float> loc) {
    cuboid(
        loc+Vec3<float>(m_postSize.x, m_postSize.y, 0),
        loc+Vec3<float>(m_sectorSize.x-m_postSize.x, m_postSize.y,
            0),
        loc+Vec3<float>(m_sectorSize.x-m_postSize.x, -m_postSize.y,
            0),
        loc+Vec3<float>(m_postSize.x, -m_postSize.y, 0),
        m_sectorSize.z);
}

void MazeView::vertWall(Vec3<float> loc) {
    cuboid(
        loc+Vec3<float>(-m_postSize.x, m_sectorSize.y-m_postSize.y, 0),
        loc+Vec3<float>(m_postSize.x, m_sectorSize.y-m_postSize.y, 0),
        loc+Vec3<float>(m_postSize.x, m_postSize.y, 0),
        loc+Vec3<float>(-m_postSize.x, m_postSize.y, 0),
        m_sectorSize.z);
}

void MazeView::renderPost(Vec3<float> loc) {
    cuboid(
        loc+Vec3<float>(-m_postSize.x, m_postSize.y, 0),
        loc+Vec3<float>(m_postSize.x, m_postSize.y, 0),
        loc+Vec3<float>(m_postSize.x, -m_postSize.y, 0),
        loc+Vec3<float>(-m_postSize.x, -m_postSize.y, 0),
        m_postSize.z);
}
