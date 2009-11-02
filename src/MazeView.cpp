#include "MeshInstance.h"
#include "Mesh.h"

#include "resources.h"

#include <iostream>
#include <ctime>
using namespace std;

#include "ImathVecAlgo.h"
using namespace Imath;

#include "GL/freeglut.h"


#include "MazeView.h"

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
    m_reqY(reqY),
    m_happy(false),
    m_rnd(new Rand32(time(NULL))),
    m_meshWallVert(NULL),
    m_meshWallHoriz(NULL),
    m_meshPost(NULL),
    m_meshFloor(NULL),
    m_meshStartFloor(NULL),
    m_meshFinishFloor(NULL),
    m_drawables(),
    m_meshes()
{
    m_sectorSize.x = size.x / (float)maze.width();
    m_sectorSize.y = size.y / (float)maze.height();
    m_sectorSize.z = size.z;

    m_postSize.x = m_sectorSize.y / 10.0f;
    m_postSize.y = m_sectorSize.x / 10.0f;
    m_postSize.z = m_sectorSize.z * 1.2f;

    createDrawables();
}

MazeView::~MazeView() {
    if(m_rnd) delete m_rnd;

    for(unsigned int i=0; i<m_drawables.size(); ++i){
        delete m_drawables[i];
    }
}

void MazeView::createDrawables() {
    // ----- create meshes -------
    // create a maze wall section
    Bitmap * wallTexBmp =
        new Bitmap(string(RESOURCE_DIR "/textures/brick.bmp"));
    Texture * wallTexture = new Texture(wallTexBmp);
    m_meshWallVert = Mesh::createUnitCube(Vec3<float>(0.4, 0.4, 0.1),
        wallTexture);
    m_meshWallHoriz = Mesh::createUnitCube(Vec3<float>(0.8, 0.6, 0.1),
        wallTexture);

    // create a post
    m_meshPost = Mesh::createUnitCylinder(
        Vec3<float>(0.59, 0.65, 0.74),
        new Texture( new Bitmap(string(RESOURCE_DIR "/textures/wood.bmp"))),
        20
    );

    // create floor planes
    Texture * floorTex = new Texture(new Bitmap(
        string(RESOURCE_DIR "/textures/marble.bmp")));
    m_meshFloor = Mesh::createUnitPlane(Vec3<float>(0.1, 0.1, 0.5), floorTex);
    m_meshStartFloor = Mesh::createUnitPlane(Vec3<float>(0.2, 0.8, 0.2),
        floorTex);
    m_meshFinishFloor = Mesh::createUnitPlane(Vec3<float>(0.84, 0.12, 0.12),
        floorTex);

    m_meshes.clear();
    m_meshes.push_back(m_meshWallVert);
    m_meshes.push_back(m_meshWallHoriz);
    m_meshes.push_back(m_meshPost);
    m_meshes.push_back(m_meshFloor);
    m_meshes.push_back(m_meshStartFloor);
    m_meshes.push_back(m_meshFinishFloor);

    // ----- create mesh instances ------
    int x,y;
    Vec3<float> up(0, 0, 1);
    Vec3<float> forward(1, 0, 0);
    Vec3<float> scaleNone(1, 1, 1);
    // floor
    for(x=0; x<m_maze.width(); ++x) {
        for(y=0; y<m_maze.height(); ++y) {
            Vec3<float> loc = getSectorLoc(x,y);
            loc.x += m_sectorSize.x * 0.5;
            loc.y += m_sectorSize.y * 0.5;
            if( x == m_startX && y == m_startY ) {
                m_drawables.push_back(new MeshInstance(m_meshStartFloor,
                   loc, m_sectorSize, up, forward));
            } else if( x== m_finishX && y == m_finishY ) {
                m_drawables.push_back(new MeshInstance(m_meshFinishFloor,
                    loc, m_sectorSize, up, forward));
            } else {
                m_drawables.push_back(new MeshInstance(m_meshFloor,
                    loc, m_sectorSize, up, forward));
            }
            
        }
    }
    
    // posts
    Vec3<float> postScale = m_postSize;
    postScale.x *= 3;
    postScale.y *= 3;
    for(x=0; x<m_maze.width(); ++x) {
        for(y=0; y<=m_maze.height(); ++y) {
            if( (y > 0 && m_maze.cellHasWall(x,y-1,Maze::West)    ) ||
                (y > 0 && (m_maze.cellHasWall(x,y-1,Maze::South)) ) ||
                (y < m_maze.height() && m_maze.cellHasWall(x,y,Maze::West)) ||
                (x > 0 && y > 0 && (m_maze.cellHasWall(x-1,y-1,Maze::South))))
            {
                Vec3<float> loc = getSectorLoc(x,y);
                loc.z += m_postSize.z * 0.5;
                m_drawables.push_back(new MeshInstance(m_meshPost,
                    loc, postScale, up, forward));
            }
        }
        y = 0;
        if( m_maze.cellHasWall(x,y,Maze::North) ||
            (x > 0 && (m_maze.cellHasWall(x-1,y,Maze::South))) )
        {
            Vec3<float> loc = getSectorLoc(x,y);
            loc.z += m_postSize.z * 0.5;
            m_drawables.push_back(new MeshInstance(m_meshPost,
                loc, postScale, up, forward));
        }
    }
    x = m_maze.width()-1;
    for(y=0; y<=m_maze.height(); ++y){
        if( (y > 0 && m_maze.cellHasWall(x,y-1,Maze::East)) ||
            (y < m_maze.height() && m_maze.cellHasWall(x,y,Maze::East)) )
        {
            Vec3<float> loc = getSectorLoc(x+1,y);
            loc.z += m_postSize.z * 0.5;
            m_drawables.push_back(new MeshInstance(m_meshPost,
                loc, postScale, up, forward));
        }
    }

    // walls
    for(int x=0; x<m_maze.width(); ++x) {
        for(int y=0; y<m_maze.height(); ++y) {
            Vec3<float> loc = getSectorLoc(x,y);
            // south
            Vec3<float> horizScale;
            horizScale.x = m_sectorSize.x - m_postSize.x*2;
            horizScale.y = m_postSize.y*2;
            horizScale.z = m_sectorSize.z;
            if( m_maze.cellHasWall(x,y,Maze::South) ) {
                m_drawables.push_back(new MeshInstance(m_meshWallHoriz,
                    loc+Vec3<float>(horizScale.x*0.5+m_postSize.x,
                        m_sectorSize.y-horizScale.y*0.5+m_postSize.y,
                        horizScale.z*0.5),
                    horizScale, up, forward));
            }
            // north
            if( y == 0 && m_maze.cellHasWall(x,y,Maze::North) ) {
                m_drawables.push_back(new MeshInstance(m_meshWallHoriz,
                    loc+Vec3<float>(horizScale.x*0.5+m_postSize.x,
                        horizScale.y*0.5-m_postSize.y, horizScale.z*0.5),
                    horizScale, up, forward));
            }

            // east
            Vec3<float> vertScale;
            vertScale.x = m_postSize.x*2;
            vertScale.y = m_sectorSize.y - m_postSize.y*2;
            vertScale.z = m_sectorSize.z;
            if( m_maze.cellHasWall(x,y,Maze::East) ) {
                m_drawables.push_back(new MeshInstance(m_meshWallVert,
                    loc+Vec3<float>(m_sectorSize.x+vertScale.x*0.5-m_postSize.x,
                        vertScale.y*0.5+m_postSize.y, vertScale.z*0.5),
                    vertScale, up, forward));
            }

            // west
            if( x == 0 && m_maze.cellHasWall(x,y,Maze::West) ) {
                m_drawables.push_back(new MeshInstance(m_meshWallVert,
                    loc+Vec3<float>(vertScale.x*0.5-m_postSize.y,
                        vertScale.y*0.5+m_postSize.y, vertScale.z*0.5),
                    vertScale, up, forward));
            }
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
    for(unsigned int i=0; i<m_drawables.size(); ++i){
        m_drawables[i]->draw();
    }
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

    return rectCollideUnsafe(o1x1, o1y1, o1x2, o1y2, o2x1, o2y1, o2x2, o2y2);
}

bool MazeView::rectCollideUnsafe(float obj1x1, float obj1y1, float obj1x2,
    float obj1y2, float obj2x1, float obj2y1, float obj2x2, float obj2y2) const
{
    return !( obj1y1 > obj2y2 || obj2y1 > obj1y2 ||
        obj1x1 > obj2x2 || obj2x1 > obj1x2 );
}

void MazeView::setHappyColoring(bool value) {
    m_happy = value;
    m_meshFloor->superHappyFunTime();
    m_meshPost->superHappyFunTime();
    m_meshWallVert->superHappyFunTime();
    m_meshWallHoriz->superHappyFunTime();
}

void MazeView::setListRendering(bool value) {
    for(unsigned int i=0; i<m_drawables.size(); ++i) {
        m_drawables[i]->setListRendering(value);
    }
}

void MazeView::calculateNormals(MeshCalculations::CalcNormalMethod mode) {
    for(unsigned int i=0; i<m_meshes.size(); ++i) {
        m_meshes[i]->calculateNormals(mode);
    }
}
