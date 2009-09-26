#include "MazeView.h"

#include <iostream>
using namespace std;

#include "ImathVecAlgo.h"
using namespace Imath;

#include "GL/freeglut.h"

#define PI 3.14159265358979
#define MAX(x,y) ((x > y) ? (x) : (y))
#define MIN(x,y) ((x < y) ? (x) : (y))

MazeView::MazeView(const Maze &maze, const Vec3<float> &pos,
        const Vec3<float> & size, int startX, int startY,
        int finishX, int finishY, int reqX, int reqY) :
    m_maze(maze),
    m_pos(pos),
    m_size(size),
    m_startX(startX),
    m_startY(startY),
    m_finishX(finishX),
    m_finishY(finishY),
    m_reqX(reqX),
    m_reqY(reqY)
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

Vec3<float> MazeView::getSectorLoc(int x, int y) const {
    Vec3<float> loc(
        m_pos.x + (float)x * m_sectorSize.x,
        m_pos.y + (float)y * m_sectorSize.y, 0);
    return loc;
}

void MazeView::render() {
    // display each wall
    for(int x=0; x<m_maze.width(); ++x) {
        for(int y=0; y<m_maze.height(); ++y) {
            // rectangle from m_pos+x*m_sectorSize to m_pos+(x+1)*sectorSize
            Vec3<float> loc = getSectorLoc(x,y);

            // floor
            if( x == m_startX && y == m_startY ) {
                glColor3f(0.98, 1.0, 0.21);
            } else if( x== m_finishX && y == m_finishY ) {
                glColor3f(0.84, 0.12, 0.12);
            } else if( x== m_reqX && y == m_reqY ) {
                glColor3f(0.51, 0.51, 0.51);
            } else {
                glColor3f(0.1, 0.1, 0.5);
            }

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

void MazeView::cylinder(Vec3<float> basePt, float radius, float height,
    int numSides)
{
    float step = 2 * PI / (float) numSides;

    // wrap
    glBegin(GL_QUAD_STRIP);
        for(int i = 0; i <= numSides; ++i ) {
            float rad = i * step;
            float x = radius * cos(rad);
            float y = radius * sin(rad);

            // top
            glVertex3f(basePt.x+x, basePt.y+y, basePt.z+height);

            // bottom
            glVertex3f(basePt.x+x, basePt.y+y, basePt.z);
        }
    glEnd();

    // put caps on it
    // top
    glBegin(GL_TRIANGLE_FAN);
        // center point
        glVertex3f(basePt.x, basePt.y, basePt.z+height);
        
        // outside
        for(int i = 0; i <= numSides; ++i ) {
            float rad = i * step;
            float x = radius * cos(rad);
            float y = radius * sin(rad);

            glVertex3f(basePt.x+x, basePt.y+y, basePt.z+height);
        }

    glEnd();

    // bottom
    glBegin(GL_TRIANGLE_FAN);
        // center point
        glVertex3f(basePt.x, basePt.y, basePt.z);
        
        // outside
        for(int i = numSides; i >= 0; --i ) {
            float rad = i * step;
            float x = radius * cos(rad);
            float y = radius * sin(rad);

            glVertex3f(basePt.x+x, basePt.y+y, basePt.z);
        }
    glEnd();
    
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
    cylinder(loc, m_postSize.x, m_postSize.z);
}

const Vec2<int> MazeView::coordinates(const Vec3<float> & location) const {
    Vec3<float> coords = (location - m_pos) / m_sectorSize;
    return Vec2<int>((int)coords.x, (int)coords.y);
}

const Vec3<float> MazeView::sectorSize() const {
    return m_sectorSize;
}

const Vec3<float> MazeView::resolveSphereCollision(Vec3<float> pos, 
    float radius, Vec3<float> delta ) const
{
    Vec2<int> coord = coordinates(pos);
    Vec3<float> newPos = pos + delta;

    // boundaries
    if( newPos.z - radius > m_pos.z+m_size.z ||
        newPos.z + radius < m_pos.z ||
        newPos.y - radius > m_pos.y+m_size.y ||
        newPos.y + radius < m_pos.y ||
        newPos.x - radius > m_pos.x+m_size.x || 
        newPos.x + radius < m_pos.x )
    {
        // no collision
        return newPos;
    }

    Vec3<float> loc = getSectorLoc(coord.x, coord.y);

    // north
    if( m_maze.cellHasWall(coord.x, coord.y, Maze::North) &&
        rectCollide(newPos.x-radius, newPos.y-radius,
                    newPos.x+radius, newPos.y+radius,
                    loc.x + m_postSize.x,
                    loc.y + m_postSize.y,
                    loc.x + m_sectorSize.x - m_postSize.x,
                    loc.y - m_postSize.y)
                                                                  )
    {
        newPos = pos + project(Vec3<float>(1, 0, 0), delta);
    }

    // south
    if( m_maze.cellHasWall(coord.x, coord.y, Maze::South) &&
        rectCollide(newPos.x-radius, newPos.y-radius,
                    newPos.x+radius, newPos.y+radius,
                    loc.x + m_postSize.x,
                    loc.y + m_sectorSize.y + m_postSize.y,
                    loc.x + m_sectorSize.x - m_postSize.x,
                    loc.y + m_sectorSize.y - m_postSize.y)
                                                                  )
    {
        newPos = pos + project(Vec3<float>(1, 0, 0), delta);
    }

    // west
    if( m_maze.cellHasWall(coord.x, coord.y, Maze::West) &&
        rectCollide(newPos.x-radius, newPos.y-radius,
                    newPos.x+radius, newPos.y+radius,
                    loc.x - m_postSize.x,
                    loc.y + m_sectorSize.y-m_postSize.y,
                    loc.x + m_postSize.x,
                    loc.y + m_postSize.y))
    {
        // project movement onto west wall plane
        newPos = pos + project(Vec3<float>(0, 1, 0), delta);
    }

    // east
    if( m_maze.cellHasWall(coord.x, coord.y, Maze::East) &&
        rectCollide(newPos.x-radius, newPos.y-radius,
                    newPos.x+radius, newPos.y+radius,
                    loc.x + m_sectorSize.x - m_postSize.x,
                    loc.y + m_sectorSize.y - m_postSize.y,
                    loc.x + m_sectorSize.x + m_postSize.x,
                    loc.y + m_postSize.y))
    {
        newPos = pos + project(Vec3<float>(0, 1, 0), delta);
    }

    return newPos;
}

bool MazeView::rectCollide(float obj1x1, float obj1y1, float obj1x2,
    float obj1y2, float obj2x1, float obj2y1, float obj2x2, float obj2y2) const
{
    float o1x1 = MIN(obj1x1, obj1x2);
    float o1x2 = MAX(obj1x1, obj1x2);
    float o1y1 = MIN(obj1y1, obj1y2);
    float o1y2 = MAX(obj1y1, obj1y2);

    float o2x1 = MIN(obj2x1, obj2x2);
    float o2x2 = MAX(obj2x1, obj2x2);
    float o2y1 = MIN(obj2y1, obj2y2);
    float o2y2 = MAX(obj2y1, obj2y2);

    //cout << "o1x1=" << o1x1 << " o1x2=" << o1x2 << " o1y1=" << o1y1 << " o1y2="
        //<< o1y2;
    //cout << " o2x1=" << o2x1 << " o2x2=" << o2x2 << " o2y1=" << o2y1 << " o2y2="
        //<< o2y2 << endl;

    return rectCollideUnsafe(o1x1, o1y1, o1x2, o1y2, o2x1, o2y1, o2x2, o2y2);
}

bool MazeView::rectCollideUnsafe(float obj1x1, float obj1y1, float obj1x2,
    float obj1y2, float obj2x1, float obj2y1, float obj2x2, float obj2y2) const
{
    return !( obj1y1 > obj2y2 || obj2y1 > obj1y2 ||
        obj1x1 > obj2x2 || obj2x1 > obj1x2 );
}
