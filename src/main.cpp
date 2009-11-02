#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
using namespace std;

#include "GL/freeglut.h"
#include "ImathVec.h"
using namespace Imath;

#include "Mesh.h"
#include "MeshInstance.h"
#include "Maze.h"
#include "MazeView.h"
#include "Camera.h"
#include "Texture.h"

#include "version.h"
#include "resources.h"

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
    MenuAnimateSpeed,
        MenuAnimateSpeedSlow,
        MenuAnimateSpeedMedium,
        MenuAnimateSpeedFast,
    MenuMiniMap,
        MenuMiniMapOn,
        MenuMiniMapOff,
    MenuTexturing,
        MenuTexturingReplace,
        MenuTexturingBlend,
        MenuTexturingOff,
    MenuFiltering,
        MenuFilteringSimple,
        MenuFilteringSmooth,
    MenuNormals,
        MenuNormalsPerFace,
        MenuNormalsPerVertexAvg,
        MenuNormalsPerVertexWeightedAvg,
    MenuNormalArrows,
        MenuNormalArrowsOn,
        MenuNormalArrowsOff,
    MenuMaterial,
        MenuMaterialDiffuse,
        MenuMaterialSpecular,
        MenuMaterialDesign,
    MenuQuit
};

enum Actions {
    ActionMoveForward,
    ActionMoveBackward,
    ActionStrafeLeft,
    ActionStrafeRight,
    ActionRotateSunCW,
    ActionRotateSunCCW,

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

enum LightMaterial {
    MaterialDiffuse,
    MaterialSpecular,
    MaterialDesign
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
void refreshLists();
void setNormalMode(Mesh::CalcNormalMethod mode);
void setNormalArrows(bool value);

void moveLight(Vec3<float> pos);

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

int prevTicks = 0;
int numFramesDrawn = 0;
int fps = 0;

int menuId;
int menuUseGlListsId;
int menuShadeModelId;
int menuGameModeId;
int menuAnimateSpeedId;
int menuMiniMapId;
int menuTexturingId;
int menuFilteringId;
int menuNormalsId;
int menuNormalArrowsId;
int menuMaterialId;

// milliseconds in between frames 
const int frameDelay = 16;

unsigned char keyActions[NumActions];

int startX, startY;
int finishX, finishY;
int reqX, reqY;

int moveMode;
Vec3<float> playerPos;
float playerRadius = 2.0f;

// clock() when the game starts
int startTime;
float finishTime; // in seconds
float prevFrameTicks;

float walkSpeed = 0.4f;
const int countDownSeconds = 4;
int countDownStop;
int countDownLeft; // seconds till game starts

int gameState;

const GLfloat goodAmbientLight[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat goodDiffuseLight[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat goodSpecLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat noLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLubyte shininess = 128;

vector<Drawable *> drawables;
vector<MeshCalculations *> meshCalcs;

Vec3<float> sectorSize;

float orbitSpeed = 1.0f;

float orbit1Angle = 0;
float orbit1Speed = 0.01f;
float orbit1Radius = 10;

float orbit2Angle = 0;
float orbit2Speed = 0.1f;
float orbit2Radius = 1;

bool miniMapOn = false;
bool showNormalArrows = false;

bool usingTextures = true;
bool usingListRendering = true;

float lightPos[4];

Vec3<float> sunPos;
float sunAngle = M_PI;
float sunRadius = 50.0f;

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    // determine name of config file
    string rcfile = ".";
    rcfile = rcfile + PROGRAM_NAME + "rc";

    keyActions[ActionMoveForward] = ',';
    keyActions[ActionStrafeLeft] = 'a';
    keyActions[ActionMoveBackward] = 'o';
    keyActions[ActionStrafeRight] = 'e';
    keyActions[ActionRotateSunCW] = 'l';
    keyActions[ActionRotateSunCCW] = 'r';

    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(formWidth, formHeight);

    glutCreateWindow(PROGRAM_NAME " " VERSION_STRING);

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

void setUsingTextures(bool value) {
    if( value ) {
        glEnable(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
}

void setMaterial(int material) {
    const GLfloat diffuse_spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat diffuse_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat diffuse_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    const GLfloat spec_spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat spec_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat spec_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    const GLfloat design_spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat design_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat design_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glMateriali(GL_FRONT, GL_SHININESS, shininess);
    switch(material){
        case MaterialDiffuse:
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glMaterialfv(GL_FRONT, GL_SPECULAR, diffuse_spec);
            glMaterialfv(GL_FRONT, GL_AMBIENT, diffuse_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_diffuse);
            break;
        case MaterialSpecular:
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glMaterialfv(GL_FRONT, GL_SPECULAR, spec_spec);
            glMaterialfv(GL_FRONT, GL_AMBIENT, spec_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, spec_diffuse);
            break;
        case MaterialDesign:
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glMaterialfv(GL_FRONT, GL_SPECULAR, design_spec);
            glMaterialfv(GL_FRONT, GL_AMBIENT, design_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, design_diffuse);
            break;
    }
}

void init() {
    // initialize opengl
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    setUsingTextures(true);

    // lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, noLight);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, noLight);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, noLight);
    Texture::setFilterMode(Texture::FilterModeSimple);
    setMaterial(MaterialDiffuse);

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

    sunPos = Vec3<float>(sectorSize.x * ((float)maze->width() / 2.0f),
        sectorSize.y * ((float)maze->height() / 2.0f), 0.0f);
    moveLight(Vec3<float>(sunPos.x, sunPos.y, sunRadius));

    // load meshes
    person = Mesh::loadFile(RESOURCE_DIR "/obj/lady.obj");
    lady = Mesh::loadFile(RESOURCE_DIR "/obj/mongrolian.obj");
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
    drawables.push_back(orb2);

    meshCalcs.push_back(person);
    meshCalcs.push_back(lady);
    meshCalcs.push_back(mazeView);

    moveMode = PlayMode;
    gameState = PreGame;
    countDownStop = glutGet(GLUT_ELAPSED_TIME) + 1000 * countDownSeconds;

    // create camera
    playerPos = Vec3<float>(10, 10, 2);
    camera = new Camera(playerPos, Vec3<float>(0,0,1), Vec3<float>(1,1,0));

    setListRendering(true);
    activateWindow();
}

void moveLight(Vec3<float> new_pos) {
    cout << "move light to " << new_pos << endl;
    glPushMatrix();
        glLoadIdentity();
        lightPos[0] = new_pos.x;
        lightPos[1] = new_pos.y;
        lightPos[2] = new_pos.z;
        lightPos[3] = 0.0f;
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glPopMatrix();
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

    menuAnimateSpeedId = glutCreateMenu(menu);
    glutAddMenuEntry("Slow", MenuAnimateSpeedSlow);
    glutAddMenuEntry("Medium", MenuAnimateSpeedMedium);
    glutAddMenuEntry("Fast", MenuAnimateSpeedFast);

    menuMiniMapId = glutCreateMenu(menu);
    glutAddMenuEntry("On", MenuMiniMapOn);
    glutAddMenuEntry("Off", MenuMiniMapOff);

    menuTexturingId = glutCreateMenu(menu);
    glutAddMenuEntry("Replace", MenuTexturingReplace);
    glutAddMenuEntry("Blend", MenuTexturingBlend);
    glutAddMenuEntry("Off", MenuTexturingOff);

    menuFilteringId = glutCreateMenu(menu);
    glutAddMenuEntry("Simple", MenuFilteringSimple);
    glutAddMenuEntry("Smooth", MenuFilteringSmooth);

    menuNormalsId = glutCreateMenu(menu);
    glutAddMenuEntry("Per Surface", MenuNormalsPerFace);
    glutAddMenuEntry("Per Vertex - Average", MenuNormalsPerVertexAvg);
    glutAddMenuEntry("Per Vertex - Weighted Average",
        MenuNormalsPerVertexWeightedAvg);

    menuNormalArrowsId = glutCreateMenu(menu);
    glutAddMenuEntry("On", MenuNormalArrowsOn);
    glutAddMenuEntry("Off", MenuNormalArrowsOff);

    menuMaterialId = glutCreateMenu(menu);
    glutAddMenuEntry("Diffuse", MenuMaterialDiffuse);
    glutAddMenuEntry("Specular", MenuMaterialSpecular);
    glutAddMenuEntry("Design", MenuMaterialDesign);

    menuId = glutCreateMenu(menu);
    glutAddSubMenu("Use glLists", menuUseGlListsId);
    glutAddSubMenu("Shade model", menuShadeModelId);
    glutAddSubMenu("Game mode", menuGameModeId);
    glutAddSubMenu("Animation speed", menuAnimateSpeedId);
    glutAddSubMenu("Mini map", menuMiniMapId);
    glutAddSubMenu("Texturing", menuTexturingId);
    glutAddSubMenu("Filtering", menuFilteringId);
    glutAddSubMenu("Normals", menuNormalsId);
    glutAddSubMenu("Normal Visualization", menuNormalArrowsId);
    glutAddSubMenu("Material", menuMaterialId);
    glutAddMenuEntry("Quit", MenuQuit);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setListRendering(bool value){
    usingListRendering = value;
    for(unsigned int i=0; i<drawables.size(); ++i){
        drawables[i]->setListRendering(value);
    }
}

void setNormalMode(MeshCalculations::CalcNormalMethod mode) {
    for(unsigned int i=0; i<meshCalcs.size(); ++i){
        meshCalcs[i]->calculateNormals(mode);
    }
    refreshLists();
}


void setNormalArrows(bool value) {
    for(unsigned int i=0; i<meshCalcs.size(); ++i){
        meshCalcs[i]->setShowNormals(value);
    }
    refreshLists();
}

void refreshLists(){
    for(unsigned int i=0; i<drawables.size(); ++i){
        drawables[i]->refreshList();
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
        case MenuAnimateSpeedSlow:
            orbitSpeed = 0.5f;
            break;
        case MenuAnimateSpeedMedium:
            orbitSpeed = 1.0f;
            break;
        case MenuAnimateSpeedFast:
            orbitSpeed = 2.0f;
            break;
        case MenuMiniMapOn:
            miniMapOn = true;
            break;
        case MenuMiniMapOff:
            miniMapOn = false;
            break;
        case MenuTexturingReplace:
            Texture::setMode(Texture::ModeReplace);
            refreshLists();
            break;
        case MenuTexturingBlend:
            Texture::setMode(Texture::ModeBlend);
            refreshLists();
            break;
        case MenuTexturingOff:
            Texture::setMode(Texture::ModeOff);
            refreshLists();
            break;
        case MenuFilteringSimple:
            Texture::setFilterMode(Texture::FilterModeSimple);
            refreshLists();
            break;
        case MenuFilteringSmooth:
            Texture::setFilterMode(Texture::FilterModeSmooth);
            refreshLists();
            break;
        case MenuNormalsPerFace:
            setNormalMode(Mesh::Surface);
            break;
        case MenuNormalsPerVertexAvg:
            setNormalMode(Mesh::Average);
            break;
        case MenuNormalsPerVertexWeightedAvg:
            setNormalMode(Mesh::WeightedAverage);
            break;
        case MenuNormalArrowsOn:
            setNormalArrows(true);
            break;
        case MenuNormalArrowsOff:
            setNormalArrows(false);
            break;
        case MenuMaterialDiffuse:
            setMaterial(MaterialDiffuse);
            break;
        case MenuMaterialSpecular:
            setMaterial(MaterialSpecular);
            break;
        case MenuMaterialDesign:
            setMaterial(MaterialDesign);
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
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
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
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if( miniMapOn )
        mazeView->draw();

    // fps counter
    int nowTicks = glutGet(GLUT_ELAPSED_TIME);
    int delta = nowTicks - prevTicks;
    if( delta >= 1000 ) {
        delta -= 1000;
        prevTicks = nowTicks - delta;
        fps = numFramesDrawn;
        numFramesDrawn = 0;
    }
    ++numFramesDrawn;

    stringstream ss;
    ss << fps << " fps";

    glColor3f(1, 1, 1);
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

    int nowTicks = glutGet(GLUT_ELAPSED_TIME);
    float fpsRate = 1.0f +
        ((nowTicks-prevFrameTicks) - frameDelay)/(float)frameDelay;

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

    if( keyState[keyActions[ActionRotateSunCW]] || keyState[keyActions[ActionRotateSunCCW]] ){
        float delta = keyState[ActionRotateSunCW] ? 0.1 : -0.1;
        sunAngle += delta;
        moveLight(Vec3<float>(sunPos.x+cosf(sunAngle)*sunRadius, sunPos.y, sunPos.z+sinf(sunAngle)));
    }
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
                    walkDir *= walkSpeed * fpsRate;
                    movePlayer(walkDir);
                } else if( keyState[keyActions[ActionMoveBackward]] &&
                            !keyState[keyActions[ActionMoveForward]] )
                {
                    // walk backward
                    Vec3<float> walkDir = camera->look();
                    walkDir.z = 0;
                    walkDir.normalize();
                    walkDir *= -walkSpeed * fpsRate;
                    movePlayer(walkDir);
                }

                if( keyState[keyActions[ActionStrafeLeft]] &&
                    !keyState[keyActions[ActionStrafeRight]] )
                {
                    // strafe left

                    movePlayer(camera->look().cross(Vec3<float>(0, 0, 1)).normalize()
                        * -walkSpeed * fpsRate);
                } else if( keyState[keyActions[ActionStrafeRight]] &&
                            !keyState[keyActions[ActionStrafeLeft]] )
                {
                    // strafe right
                    movePlayer(camera->look().cross(Vec3<float>(0, 0, 1)).normalize()
                        * walkSpeed * fpsRate);
                }

                camera->moveTo(playerPos);
            } else if ( moveMode == SpecMode ) {
                if( keyState[keyActions[ActionMoveForward]] &&
                    !keyState[keyActions[ActionMoveBackward]] )
                {
                    // move camera forward in the direction it is facing
                    camera->moveForward(0.7 * fpsRate);
                } else if( keyState[keyActions[ActionMoveBackward]] &&
                            !keyState[keyActions[ActionMoveForward]] )
                {
                    // move camera backward in the direction it is facing
                    camera->moveBackward(0.7 * fpsRate);
                }

                if( keyState[keyActions[ActionStrafeLeft]] &&
                    !keyState[keyActions[ActionStrafeRight]] )
                {
                    // strafe camera left
                    camera->moveLeft(0.7 * fpsRate);
                } else if( keyState[keyActions[ActionStrafeRight]] &&
                            !keyState[keyActions[ActionStrafeLeft]] )
                {
                    // strafe camera right
                    camera->moveRight(0.7 * fpsRate);
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
            camera->pointLeft(0.1 * fpsRate);
            camera->moveUp(3 * fpsRate);
            break;
    }
    
    prevFrameTicks = nowTicks;
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
    setListRendering(true);
    walkSpeed = 2;
}

