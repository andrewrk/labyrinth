#include <iostream>
using namespace std;

#include "GL/glut.h"


#include "Maze.h"


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

int main(int argc, char *argv[]) {
    // maze test
    Maze maze(20,20);
    maze.print();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGB);
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
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);
    glRectf(-0.1, -0.1, 0.1, 0.1);

    glPopMatrix();

    glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y) {
}

void keyUp(unsigned char key, int x, int y) {
}

void specialKeyDown(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_UP:
            break;
        case GLUT_KEY_DOWN:
            break;
    }
}

void specialKeyUp(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_UP:
            break;
        case GLUT_KEY_DOWN:
            break;
    }
}

void nextFrame() {
    glutPostRedisplay();
}

void reshape(GLint w, GLint h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
