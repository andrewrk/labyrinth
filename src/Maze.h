/* 
    Maze.h - algorithmically generates a 2d maze to navigate
*/

#ifndef _MAZE_H_
#define _MAZE_H_

#include "Array2D.h"

class Maze {
    public:
        // types
        enum Direction {
            North = 0,
            South,
            East,
            West
        };

        // ctor
        Maze(int width, int height);
        ~Maze();

        // accessors
        inline int width() const { return m_width; }
        inline int height() const { return m_height; }

        // does the cell have a wall in direction
        bool cellHasWall(int x, int y, Direction dir) const;

        // print ascii to stdout
        void print() const;

    private:
        typedef struct {
            bool wall[4]; //n,s,e,w
            int x;
            int y;
        } MazeCell;
        
        int m_width, m_height;

        Array2D<MazeCell> m_maze;

        void generateMaze();
};

#endif
