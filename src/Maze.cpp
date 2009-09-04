#include "Maze.h"

#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include <cstdlib>
#include <ctime>


Maze::Maze(int width, int height) :
    m_startX(0),
    m_startY(0),
    m_endX(width-1),
    m_endY(height-1),
    m_width(width),
    m_height(height),
    m_maze(width, height)
{
    generateMaze();
}

Maze::Maze(int width, int height, int startX, int startY,
                int endX, int endY) :
    m_startX(startX),
    m_startY(startY),
    m_endX(endX),
    m_endY(endY),
    m_width(width),
    m_height(height),
    m_maze(width, height)
{
    generateMaze();
}

Maze::~Maze() {
}

void Maze::generateMaze() {
    srand(time(NULL));
    vector<MazeCell *> cellStack;

    m_maze.redim(m_width, m_height);
    // fill the maze with walls
    for(int x=0; x<m_width; ++x){
        for(int y=0; y<m_height; ++y){
            m_maze.get(x,y).x = x;
            m_maze.get(x,y).y = y;
            for(int i=0; i<4; ++i)
                m_maze.get(x,y).wall[i] = true;
        }
    }

    int totalCells = m_width * m_height;
    int visitedCells = 1;
    MazeCell * curr = &m_maze.get(rand() % m_width, rand() % m_height);
    while(visitedCells < totalCells) {
        // find all neighbors of current cell with all walls intact
        vector<MazeCell *> neighbors;
        int nx[] = {-1+curr->x,  0+curr->x, 1+curr->x, 0+curr->x};
        int ny[] = { 0+curr->y, -1+curr->y, 0+curr->y, 1+curr->y};
        for(int i=0; i<4; ++i) {
            int nxi = nx[i], nyi = ny[i];
            if( nxi >= 0 && nxi < m_width &&
                nyi >= 0 && nyi < m_height )
            {
                if( m_maze.get(nxi,nyi).wall[North] &&
                    m_maze.get(nxi,nyi).wall[South] &&
                    m_maze.get(nxi,nyi).wall[East] &&
                    m_maze.get(nxi,nyi).wall[West] )
                {
                    neighbors.push_back(&m_maze.get(nxi,nyi));
                }
            }
        }
        
        // if one or more found
        if( neighbors.size() > 0 ) {
            // choose one at random
            MazeCell * neigh = neighbors[rand() % neighbors.size()];

            // knock down the wall between it and current cell
            if( curr->x > neigh->x ) {
                curr->wall[West] = false;
                neigh->wall[East] = false;
            } else if( curr->x < neigh->x ) {
                curr->wall[East] = false;
                neigh->wall[West] = false;
            } else if( curr->y > neigh->y ) {
                curr->wall[North] = false;
                neigh->wall[South] = false;
            } else if( curr->y < neigh->y ) {
                curr->wall[South] = false;
                neigh->wall[South] = false;
            } else {
                cerr << "error: generateMaze() neighbor equal to current"
                    << endl;
            }
            
            // push current cell on the stack
            cellStack.push_back(curr);

            // make the new cell current cell
            curr = neigh;

            ++visitedCells;
        } else {
            // no neighbors with all walls intact
            // pop the most recent cell entry off the stack and make it curr
            curr = cellStack.back();
            cellStack.pop_back();
        }
    }
}

void Maze::print() const {
    // trace the maze
    string mapStr = "";
    for(int y=0;y<m_height;++y){
        for(int x=0;x<m_width;++x){
            mapStr += m_maze.get(x,y).wall[South] ? "_" : " ";
            mapStr += m_maze.get(x,y).wall[East] ? "|" : " ";
        }
        mapStr += "\n";
    }
    cout << mapStr;
}

bool Maze::cellHasWall(int x, int y, Maze::Direction dir) const {
    return m_maze.get(x,y).wall[dir];
}

