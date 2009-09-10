#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

#include "GL/freeglut.h"
#include "ImathVec.h"
using namespace Imath;

#include "Maze.h"
#include "MazeView.h"
#include "Camera.h"

enum MenuItem {
    MenuUseGlLists,
        MenuUseGlListsOn,
        MenuUseGlListsOff,
    MenuShadeModel,
        MenuShadeModelSmooth,
        MenuShadeModelFlat,
    MenuQuit
};

int main(int argc, char *argv[]);
void init();
void reshape(GLint w, GLint h);
void mouse(GLint button, GLint action, GLint x, GLint y);
void entry(int state);
void motion(int x, int y);
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void display();
void nextFrame();
void activateWindow();
void deactivateWindow();
void setListRendering(bool value);

void menu(int value);
void initMenus();

Maze * maze;
MazeView * mazeView;
Camera * camera;

bool keyState[256] = {0};
bool specialKeyState[256] = {0};

int formWidth = 800, formHeight = 600;

bool outsideWindow = true;

clock_t prevTicks = 0;
int numFramesDrawn = 0;
int fps = 0;

int menuId;
int menuUseGlListsId;
int menuShadeModelId;

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(formWidth, formHeight);
    glutCreateWindow("Labyrinth");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutEntryFunc(entry);
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
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutIgnoreKeyRepeat(true);

    // initialize menus
    initMenus();

    // initialize program
    // create maze object
    maze = new Maze(10, 10);
    mazeView = new MazeView(*maze, Vec3<float>(0, 0, 0),
        Vec3<float>(200,200,10));
    mazeView->init();

    // create camera
    camera = new Camera(Vec3<float>(10, 10, 1),
        Vec3<float>(0,0,1),
        Vec3<float>(1,-1,0));

}

void initMenus() {
    menuUseGlListsId = glutCreateMenu(menu);
    glutAddMenuEntry("On", MenuUseGlListsOn);
    glutAddMenuEntry("Off", MenuUseGlListsOff);

    menuShadeModelId = glutCreateMenu(menu);
    glutAddMenuEntry("Smooth", MenuShadeModelSmooth);
    glutAddMenuEntry("Flat", MenuShadeModelFlat);

    menuId = glutCreateMenu(menu);
    glutAddSubMenu("Use glLists", menuUseGlListsId);
    glutAddSubMenu("Shade model", menuShadeModelId);
    glutAddMenuEntry("Quit", MenuQuit);


    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setListRendering(bool value){
    mazeView->setListRendering(value);
}

void menu(int value) {
    switch(value){
        case MenuUseGlListsOn:
            setListRendering(true);
            break;
        case MenuUseGlListsOff:
            setListRendering(false);
            break;
        case MenuShadeModelSmooth:
            glShadeModel(GL_SMOOTH);
            break;
        case MenuShadeModelFlat:
            glShadeModel(GL_FLAT);
            break;
        case MenuQuit:
            exit(0);
            break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // build projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, (float)formWidth / (float)formHeight, 1, 1000);

    // Build Camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->applyTransformations();

    // Transform and Render Objects
    mazeView->draw();

    // control panel
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, formWidth, formHeight, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // fps counter
    clock_t nowTicks = clock();
    clock_t delta = nowTicks - prevTicks;
    if( delta >= CLOCKS_PER_SEC ) {
        delta -= CLOCKS_PER_SEC;
        prevTicks = clock() - delta;
        prevTicks = nowTicks;
        fps = numFramesDrawn;
        numFramesDrawn = 0;
    }
    ++numFramesDrawn;

    stringstream ss;
    ss << fps << " fps";
    
    glColor3f(0, 0, 0);
    glRasterPos2f(20, formHeight - 20);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char *) ss.str().c_str());


    // animate
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
        camera->moveForward(0.3);
    } else if( keyState['o'] ) {
        // move camera backward in the direction it is facing
        camera->moveBackward(0.3);
    }

    if( keyState['-'] ) {
        camera->moveDown(0.3);
    } else if( keyState['+'] ) {
        camera->moveUp(0.3);
    }

    if( keyState['a'] ) {
        // strafe camera left
        camera->moveLeft(0.3);
    } else if( keyState['e'] ) {
        // strafe camera right
        camera->moveRight(0.3);
    }
    
    glutPostRedisplay();
}

void reshape(GLint w, GLint h) {
    formWidth = w;
    formHeight = h;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

}

void mouse(GLint button, GLint action, GLint x, GLint y) {
    if( outsideWindow ){
        activateWindow();
        return;
    }

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

void motion(int x, int y) {
    int centerX = formWidth / 2, centerY = formHeight /2;
    float deltaX = x-centerX, deltaY = y-centerY;

    if( x != centerX || y != centerY) {
        glutWarpPointer(centerX, centerY);

        camera->pointRight(deltaX/500);
        camera->pointDown(deltaY/500);
    }
}

void entry(int state) {
    if( state == GLUT_LEFT )
        deactivateWindow();
}

void activateWindow() {
    outsideWindow = false;
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutWarpPointer(formWidth / 2, formHeight / 2);
}

void deactivateWindow() {
    outsideWindow = true;
    glutMotionFunc(NULL);
    glutPassiveMotionFunc(NULL);
}
