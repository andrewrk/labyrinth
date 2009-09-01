#include <iostream>
using namespace std;

#include "GL/glut.h"

int main(int argc, char *argv[]);
void init();
void mouse(GLint button, GLint action, GLint x, GLint y);
void motion(GLint x, GLint y);
void display();


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    //glutInitWindowPosition(0,0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Labyrinth");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    //glutMotionFunc(motion); // mouse motion

    glutMainLoop();
    return 0;
}

void init() {
    glClearColor(0, 0, 0, 0);
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glutSwapBuffers();

}

void motion( int x, int y ){
    cout << "x: " << x << " y: " << y << endl;
}
