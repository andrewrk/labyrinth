#include <iostream>
using namespace std;

#include "GL/glut.h"
#include "ImathVec.h"
using namespace Imath;

#include "Maze.h"
#include "MazeView.h"
#include "Camera.h"

int main(int argc, char *argv[]);
void init();
void reshape(GLint w, GLint h);
void mouse(GLint button, GLint action, GLint x, GLint y);
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void motion(GLint x, GLint y);
void display();
void nextFrame();

Maze * maze;
MazeView * mazeView;
Camera * camera;

bool keyState[256] = {0};
bool specialKeyState[256] = {0};

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Labyrinth");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion); // mouse motion
    glutReshapeFunc(reshape);
    glutIdleFunc(nextFrame);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);

    glutMainLoop();
    return 0;
}

void init() {
    // initialize opengl
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glutIgnoreKeyRepeat(true);

    // initialize program
    // create maze object
    maze = new Maze(10, 10);
    mazeView = new MazeView(*maze, Vec3<float>(0, 0, 0), Vec3<float>(20,20,1));

    // create camera
    camera = new Camera(Vec3<float>(0, 0, 0.5),
        Vec3<float>(0,0,1),
        Vec3<float>(1,1,0));

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Build Camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->applyTransformations();

    // Transform and Render Objects
    mazeView->display();
    
    glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y) {
    keyState[key] = true;
    switch(key){
        case 'p':
            maze->print();
            break;
    }
}

void keyUp(unsigned char key, int x, int y) {
    keyState[key] = false;
}

void specialKeyDown(int key, int x, int y) {
    specialKeyState[key] = true;
}

void specialKeyUp(int key, int x, int y) {
    specialKeyState[key] = false;
}

void nextFrame() {
    if( keyState[','] ) {
        // move camera forward in the direction it is facing
        camera->moveForward(0.01);
    } else if( keyState['o'] ) {
        // move camera backward in the direction it is facing
        camera->moveBackward(0.01);
    }

    if( keyState['-'] ) {
        camera->moveDown(0.01);
    } else if( keyState['+'] ) {
        camera->moveUp(0.01);
    }

    if( keyState['a'] ) {
        // strafe camera left
        camera->moveLeft(0.01);
    } else if( keyState['e'] ) {
        // strafe camera right
        camera->moveRight(0.01);
    }

    if( specialKeyState[GLUT_KEY_LEFT] ) {
        // rotate camera to the left
        camera->pointLeft(0.01);
    } else if( specialKeyState[GLUT_KEY_RIGHT] ) {
        // rotate camera to the right
        camera->pointRight(0.01);
    }

    if( specialKeyState[GLUT_KEY_UP] ) {
        // rotate camera up
        camera->pointUp(0.01);
    } else if( specialKeyState[GLUT_KEY_DOWN] ) {
        // rotate camera down
        camera->pointDown(0.01);
    }
    
    glutPostRedisplay();
}

void reshape(GLint w, GLint h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    // build projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1.33, 0.01, 100);

}

void mouse(GLint button, GLint action, GLint x, GLint y) {
    switch(button){
        case GLUT_LEFT_BUTTON:
            if( action == GLUT_DOWN )
                cout << "left down" << endl;
            else
                cout << "left up" << endl;
            break;
        case GLUT_RIGHT_BUTTON:
            if( action == GLUT_DOWN )
                cout << "right down" << endl;
            else
                cout << "right up" << endl;
            break;
        case GLUT_MIDDLE_BUTTON:
            if( action == GLUT_DOWN )
                cout << "middle down" << endl;
            else
                cout << "middle up" << endl;
            break;
    }
}

void motion( int x, int y ){
}
