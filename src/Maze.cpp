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
    CellWall wall;
    wall.backtrack = false;
    wall.solution = false;
    wall.wall = true;

    for(int x=0; x<m_width; ++x){
        for(int y=0; y<m_height; ++y){
            m_maze.get(x,y).x = x;
            m_maze.get(x,y).y = y;
            m_maze.get(x,y).n = wall;
            m_maze.get(x,y).s = wall;
            m_maze.get(x,y).e = wall;
            m_maze.get(x,y).w = wall;
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
                if( m_maze.get(nxi,nyi).n.wall && m_maze.get(nxi,nyi).s.wall &&
                    m_maze.get(nxi,nyi).e.wall && m_maze.get(nxi,nyi).w.wall )
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
                curr->w.wall = false;
                neigh->e.wall = false;
            } else if( curr->x < neigh->x ) {
                curr->e.wall = false;
                neigh->w.wall = false;
            } else if( curr->y > neigh->y ) {
                curr->n.wall = false;
                neigh->s.wall = false;
            } else if( curr->y < neigh->y ) {
                curr->s.wall = false;
                neigh->s.wall = false;
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
    string mapStr = "_";
    for(int x=0; x<m_width; ++x)
        mapStr += "__";
    mapStr += "\n";
    for(int y=0;y<m_height;++y){
        mapStr += "|";
        for(int x=0;x<m_width;++x){
            mapStr += m_maze.get(x,y).s.wall ? "_" : " ";
            mapStr += m_maze.get(x,y).e.wall ? "|" : " ";
        }
        mapStr += "\n";
    }
    cout << mapStr;
}
