/* 
    Maze.h - algorithmically generates a 2d maze to navigate
*/

#ifndef _MAZE_H_
#define _MAZE_H_

#include "Array2D.h"

class Maze {
    public:
        Maze(int width, int height);
        Maze(int width, int height, int startX, int startY, int endX, int endY);
        ~Maze();

        inline int startX() const { return m_startX; }
        inline int startY() const { return m_startY; }
        inline int endX() const { return m_endX; }
        inline int endY() const { return m_endY; }

        void print() const;

    private:
        typedef struct {
            bool backtrack;
            bool solution;
            bool wall;
        } CellWall;

        typedef struct {
            CellWall n;
            CellWall s;
            CellWall e;
            CellWall w;
            int x;
            int y;
        } MazeCell;
        
        int m_startX, m_startY;
        int m_endX, m_endY;

        int m_width, m_height;

        Array2D<MazeCell> m_maze;

        void generateMaze();
        
};

#endif
