#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
using namespace std;

#include "GL/freeglut.h"
#include "ImathVec.h"
using namespace Imath;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Mesh.h"
#include "MeshInstance.h"
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
    MenuGameMode,
        MenuGameModeNormal,
        MenuGameModeSpec,
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

enum MovementModes {
    PlayMode,
    SpecMode,

    // length of the enum
    NumMovementModes
};

enum GameState {
    PreGame,
    GamePlay,
    GameWon,
    GameLost,

    // length
    NumGameStates
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
void victory();

Maze * maze;
MazeView * mazeView;
Camera * camera;
Mesh * person;
Mesh * lady;
MeshInstance * stillPerson;
MeshInstance * orbitingPerson;
MeshInstance * orb2;

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
int menuGameModeId;

// milliseconds in between frames 
const int frameDelay = 16;

po::variables_map vm;

unsigned char keyActions[NumActions];

int startX, startY;
int finishX, finishY;
int reqX, reqY;

int moveMode;
Vec3<float> playerPos;
float playerRadius = 2.0f;

// clock() when the game starts
clock_t startTime;
float finishTime; // in seconds

float walkSpeed = 0.4f;
const int countDownSeconds = 4;
clock_t countDownStop;
int countDownLeft; // seconds till game starts

int gameState;

const GLfloat AmbientLight[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat DiffuseLight[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat SpecularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat SpecRef[]       = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLubyte Shine = 128;

vector<Drawable *> drawables;

Vec3<float> sectorSize;

float orbitSpeed = 1.0f;

float orbit1Angle = 0;
float orbit1Speed = 0.01f;
float orbit1Radius = 10;

float orbit2Angle = 0;
float orbit2Speed = 0.1f;
float orbit2Radius = 1;

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

    string title = PROGRAM_NAME;
    title += " ";
    title += VERSION_STRING;

    glutCreateWindow(title.c_str());

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
    // initialize opengl
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // lighting
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, SpecRef);
    glMateriali(GL_FRONT, GL_SHININESS, Shine);
    glEnable(GL_NORMALIZE);
    

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
    sectorSize = mazeView->sectorSize();

    // load meshes
    person = Mesh::loadFile("resources/obj/mongrolian.obj");
    lady = Mesh::loadFile("resources/obj/lady.obj");
    Vec3<float> reqSector = mazeView->getSectorLoc(finishX, finishY);
    Vec3<float> personLoc = reqSector + 
        Vec3<float>(0.5*sectorSize.x,0.5*sectorSize.y, 2*person->size().z);
    stillPerson = new MeshInstance(person, personLoc,
        Vec3<float>(1, 1, 1), Vec3<float>(0,-1,0), Vec3<float>(0, 0, 0));
    Vec3<float> ladyScale = Vec3<float>(0.5, 0.5, 0.5);
    Vec3<float> orbiterLoc = personLoc + lady->size() * ladyScale;
    orbitingPerson = new MeshInstance(lady, orbiterLoc, ladyScale,
        Vec3<float>(0,0,1), Vec3<float>(1,0,0) );
    orb2 = new MeshInstance(person, personLoc, Vec3<float>(0.1, 0.1, 0.1),
        Vec3<float>(0, -1, 0), Vec3<float>(0, 1, 0));

    drawables.push_back(mazeView);
    drawables.push_back(stillPerson);
    drawables.push_back(orbitingPerson);

    moveMode = PlayMode;
    gameState = PreGame;
    countDownStop = glutGet(GLUT_ELAPSED_TIME) + 1000 * countDownSeconds;

    // create camera
    playerPos = Vec3<float>(10, 10, 2);
    camera = new Camera(playerPos, Vec3<float>(0,0,1), Vec3<float>(1,1,0));

    setListRendering(true);
    activateWindow();
}

void initMenus() {
    menuUseGlListsId = glutCreateMenu(menu);
    glutAddMenuEntry("On", MenuUseGlListsOn);
    glutAddMenuEntry("Off", MenuUseGlListsOff);

    menuShadeModelId = glutCreateMenu(menu);
    glutAddMenuEntry("Smooth", MenuShadeModelSmooth);
    glutAddMenuEntry("Flat", MenuShadeModelFlat);

    menuGameModeId = glutCreateMenu(menu);
    glutAddMenuEntry("Normal", MenuGameModeNormal);
    glutAddMenuEntry("Spectator", MenuGameModeSpec);

    menuId = glutCreateMenu(menu);
    glutAddSubMenu("Use glLists", menuUseGlListsId);
    glutAddSubMenu("Shade model", menuShadeModelId);
    glutAddSubMenu("Game mode", menuGameModeId);
    glutAddMenuEntry("Quit", MenuQuit);


    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setListRendering(bool value){
    for(unsigned int i=0; i<drawables.size(); ++i){
        mazeView->setListRendering(value);
    }
}

void setHappyColoring(bool value){
    mazeView->setHappyColoring(value);
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
        case MenuGameModeNormal:
            moveMode = PlayMode;
            break;
        case MenuGameModeSpec:
            moveMode = SpecMode;
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
    gluPerspective(60, (float)formWidth / (float)formHeight, 1, 1000);

    // Build Camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->applyTransformations();

    // Transform and Render Objects
    glEnable(GL_DEPTH_TEST);
    mazeView->draw();
    glColor3f(1, 1, 1);
    stillPerson->draw();
    orbitingPerson->draw();
    orb2->draw();


    // control panel
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, formWidth, formHeight, 0);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // fps counter
    clock_t nowTicks = glutGet(GLUT_ELAPSED_TIME);
    clock_t delta = nowTicks - prevTicks;
    if( delta >= 1000 ) {
        delta -= 1000;
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

    // timer
    stringstream timer_ss;

    if( gameState == GamePlay ) {
        float seconds = (nowTicks-startTime) / 1000.0f;
        timer_ss << "GO! " << seconds;
        glRasterPos2f(formWidth - 100, formHeight - 20);
    } else if( gameState == PreGame ) {
        timer_ss << "READY? " << (int) countDownLeft;
        glRasterPos2f(formWidth / 2 - 40, formHeight / 2 - 40);
    } else {
        timer_ss << "YOU WIN! Time: " << finishTime;
        glRasterPos2f(formWidth / 2 - 40, formHeight / 2 - 40);
    }

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char *) timer_ss.str().c_str());

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
    // TODO: make this an array
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

// check for collisions and move the player to a valid position
void movePlayer(const Vec3<float> & delta) {
    playerPos = mazeView->resolveSphereCollision(playerPos,playerRadius,delta);
}

void nextFrame(int value) {
    glutTimerFunc(frameDelay, nextFrame, 0);

    // perform orbiting
    orbit1Angle += orbit1Speed * orbitSpeed;
    orbit2Angle += orbit2Speed * orbitSpeed;
    Vec3<float> center1 = stillPerson->pos();
    orbitingPerson->setPos(
        Vec3<float>(center1.x+orbit1Radius*cosf(orbit1Angle), 
        center1.y+orbit1Radius*sinf(orbit1Angle), center1.z));
    Vec3<float> center2 = orbitingPerson->pos();
    orb2->setPos(Vec3<float>(center2.x,
                             center2.y+orbit2Radius*cosf(orbit2Angle),
                             center2.z+orbit2Radius*sinf(orbit2Angle)));

    switch(gameState) {
        case PreGame:
            // countdown timer
            countDownLeft = (countDownStop - glutGet(GLUT_ELAPSED_TIME)) / 1000;

            if( countDownLeft <= 0 ) {
                gameState = GamePlay;
                startTime = glutGet(GLUT_ELAPSED_TIME);
            }
            break;
        case GamePlay:
        case GameWon:
            if( moveMode == PlayMode ) {
                // move the player 

                if( keyState[keyActions[ActionMoveForward]] &&
                    !keyState[keyActions[ActionMoveBackward]] )
                {
                    // walk forward
                    Vec3<float> walkDir = camera->look();
                    walkDir.z = 0;
                    walkDir.normalize();
                    walkDir *= walkSpeed;
                    movePlayer(walkDir);
                } else if( keyState[keyActions[ActionMoveBackward]] &&
                            !keyState[keyActions[ActionMoveForward]] )
                {
                    // walk backward
                    Vec3<float> walkDir = camera->look();
                    walkDir.z = 0;
                    walkDir.normalize();
                    walkDir *= -walkSpeed;
                    movePlayer(walkDir);
                }

                if( keyState[keyActions[ActionStrafeLeft]] &&
                    !keyState[keyActions[ActionStrafeRight]] )
                {
                    // strafe left

                    movePlayer(camera->look().cross(Vec3<float>(0, 0, 1)).normalize()
                        * -walkSpeed);
                } else if( keyState[keyActions[ActionStrafeRight]] &&
                            !keyState[keyActions[ActionStrafeLeft]] )
                {
                    // strafe right
                    movePlayer(camera->look().cross(Vec3<float>(0, 0, 1)).normalize()
                        * walkSpeed);
                }

                camera->moveTo(playerPos);
            } else if ( moveMode == SpecMode ) {
                if( keyState[keyActions[ActionMoveForward]] &&
                    !keyState[keyActions[ActionMoveBackward]] )
                {
                    // move camera forward in the direction it is facing
                    camera->moveForward(0.7);
                } else if( keyState[keyActions[ActionMoveBackward]] &&
                            !keyState[keyActions[ActionMoveForward]] )
                {
                    // move camera backward in the direction it is facing
                    camera->moveBackward(0.7);
                }

                if( keyState[keyActions[ActionStrafeLeft]] &&
                    !keyState[keyActions[ActionStrafeRight]] )
                {
                    // strafe camera left
                    camera->moveLeft(0.7);
                } else if( keyState[keyActions[ActionStrafeRight]] &&
                            !keyState[keyActions[ActionStrafeLeft]] )
                {
                    // strafe camera right
                    camera->moveRight(0.7);
                }
            }

            if( gameState == GamePlay ) { 
                Vec2<int> coord = mazeView->coordinates(playerPos);
                if( coord.x == finishX && coord.y == finishY )
                    victory();
            }

            break;
        case GameLost:
            // spin
            camera->pointLeft(0.1);
            camera->moveUp(3);
            break;
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
                ;
            else
                ;
            break;
        case GLUT_RIGHT_BUTTON:
            if( action == GLUT_DOWN )
                    ;
                else
                    ;
            break;
        case GLUT_MIDDLE_BUTTON:
            if( action == GLUT_DOWN )
                ;
            else
                ;
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

// gets called when they win
void victory() {
    gameState = GameWon;
    finishTime = (glutGet(GLUT_ELAPSED_TIME) - startTime)/1000.0f;
    setListRendering(false);
    setHappyColoring(true);
    walkSpeed = 2;
}

