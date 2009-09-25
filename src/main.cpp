#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cctype>
using namespace std;

#include "GL/freeglut.h"
#include "ImathVec.h"
using namespace Imath;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Maze.h"
#include "MazeView.h"
#include "Camera.h"

#include "version.h"

enum MenuItem {
    MenuUseGlLists,
        MenuUseGlListsOn,
        MenuUseGlListsOff,
    MenuShadeModel,
        MenuShadeModelSmooth,
        MenuShadeModelFlat,
    MenuQuit
};

enum Actions {
    ActionMoveForward,
    ActionMoveBackward,
    ActionStrafeLeft,
    ActionStrafeRight,

    // the length of the enum
    NumActions
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
void nextFrame(int value);
void activateWindow();
void deactivateWindow();
void setListRendering(bool value);
void quitApp();
unsigned char getKeyFor(unsigned char key);

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

// milliseconds in between frames 
const int frameDelay = 17;

po::variables_map vm;

unsigned char keyActions[NumActions];

int startX, startY;
int finishX, finishY;
int reqX, reqY;

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    // determine name of config file
    string rcfile = ".";
    rcfile = rcfile + PROGRAM_NAME + "rc";

    // parse command line and config file
    po::options_description cmdDesc("Allowed options");
    cmdDesc.add_options()
        ("help", "produce help message")
    ;
    po::options_description confDesc("Configuration file options");
    confDesc.add_options()
        ("keys.forward",
            po::value<unsigned char>(
                &keyActions[ActionMoveForward])->default_value('w'),
            "key combo for moving forward")
        ("keys.backward",
            po::value<unsigned char>(
                &keyActions[ActionMoveBackward])->default_value('s'),
            "key combo for moving backward")
        ("keys.strafeLeft",
            po::value<unsigned char>(
                &keyActions[ActionStrafeLeft])->default_value('a'),
            "key combo for strafing left")
        ("keys.strafeRight",
            po::value<unsigned char>(
                &keyActions[ActionStrafeRight])->default_value('d'),
            "key combo for strafing right")
    ;
    cmdDesc.add(confDesc);
    po::store(po::parse_command_line(argc, argv, cmdDesc), vm);
    ifstream in(rcfile.c_str(), ifstream::in);
    po::store(po::parse_config_file(in, cmdDesc), vm);
    po::notify(vm);

    if( vm.count("help") ){
        cout << cmdDesc << endl;
        return 1;
    }

    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(formWidth, formHeight);
    glutCreateWindow(PROGRAM_NAME);

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutEntryFunc(entry);
    glutReshapeFunc(reshape);
    glutTimerFunc(frameDelay, nextFrame, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);

    glutMainLoop();
    return 0;
}

void init() {
    // output version
    cout << "Version: " << VERSION_STRING << endl;

    // initialize opengl
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glutIgnoreKeyRepeat(true);

    // initialize menus
    initMenus();

    // initialize program
    // create maze object
    startX = 0;
    startY = 0;
    finishX = 9;
    finishY = 9;
    reqX = 5;
    reqY = 5;
    maze = new Maze(10, 10);
    mazeView = new MazeView(*maze, Vec3<float>(0, 0, 0),
        Vec3<float>(200,200,10), startX, startY, finishX, finishY, reqX, reqY);
    mazeView->init();

    // create camera
    camera = new Camera(Vec3<float>(10, 10, 1),
        Vec3<float>(0,0,1),
        Vec3<float>(1,-1,0));

    activateWindow();
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
            quitApp();
            break;
    }
}

void quitApp() {
    if( maze ) delete maze;
    if( mazeView ) delete mazeView;
    if( camera ) delete camera;

    exit(0);
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
        prevTicks = nowTicks - delta;
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
    keyState[getKeyFor(key)] = true;
    switch(key){
        case 'p':
            maze->print();
            break;
        case '\e': // escape
            deactivateWindow();
            break;
    }
}

void keyUp(unsigned char key, int x, int y) {
    keyState[getKeyFor(key)] = false;
}

unsigned char getKeyFor(unsigned char key) {
    key = tolower(key);

    switch(key){
        case '~':
            key = '`';
            break;
        case '!':
            key = '1';
            break;
        case '@':
            key = '2';
            break;
        case '#':
            key = '3';
            break;
        case '$':
            key = '4';
            break;
        case '%':
            key = '5';
            break;
        case '^':
            key = '6';
            break;
        case '&':
            key = '7';
            break;
        case '*':
            key = '8';
            break;
        case '(':
            key = '9';
            break;
        case ')':
            key = '0';
            break;
        case '{':
            key = '[';
            break;
        case '}':
            key = ']';
            break;
        case '?':
            key = '/';
            break;
        case '+':
            key = '=';
            break;
        case ':':
            key = ';';
            break;
        case '_':
            key = '-';
            break;
        case '"':
            key = '\'';
            break;
        case '|':
            key = '\\';
            break;
        case '<':
            key = ',';
            break;
        case '>':
            key = '.';
            break;
    }

    return key;
}

void specialKeyDown(int key, int x, int y) {
    specialKeyState[key] = true;

    // check for alt+f4
    switch(key){
        case GLUT_KEY_F4:
            if( glutGetModifiers() & GLUT_ACTIVE_ALT )
                quitApp();
            break;
    }
}

void specialKeyUp(int key, int x, int y) {
    specialKeyState[key] = false;
}

void nextFrame(int value) {
    glutTimerFunc(frameDelay, nextFrame, 0);

    if( keyState[keyActions[ActionMoveForward]] &&
        !keyState[keyActions[ActionMoveBackward]] )
    {
        // move camera forward in the direction it is facing
        camera->moveForward(0.3);
    } else if( keyState[keyActions[ActionMoveBackward]] &&
                !keyState[keyActions[ActionMoveForward]] )
    {
        // move camera backward in the direction it is facing
        camera->moveBackward(0.3);
    }

    if( keyState[keyActions[ActionStrafeLeft]] &&
        !keyState[keyActions[ActionStrafeRight]] )
    {
        // strafe camera left
        camera->moveLeft(0.3);
    } else if( keyState[keyActions[ActionStrafeRight]] &&
                !keyState[keyActions[ActionStrafeLeft]] )
    {
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
    glutSetCursor(GLUT_CURSOR_NONE);
}

void deactivateWindow() {
    outsideWindow = true;
    glutMotionFunc(NULL);
    glutPassiveMotionFunc(NULL);
    glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
}
