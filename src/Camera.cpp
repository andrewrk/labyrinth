#include "Camera.h"

#include <iostream>
#include <cmath>
using namespace std;

#include "GL/glut.h"


Camera::Camera(const Vec3<float> & pos, const Vec3<float> & up,
        const Vec3<float> & look) :
    m_pos(pos),
    m_up(up),
    m_look(look)
{
    calcRefPoint();
}

Camera::~Camera() {

}

void Camera::applyTransformations() {
    gluLookAt(m_pos.x, m_pos.y, m_pos.z, 
       m_refPoint.x, m_refPoint.y, m_refPoint.z,
       m_up.x, m_up.y, m_up.z);
}

void Camera::moveBy(const Vec3<float> & delta) {
    m_pos += delta;
    calcRefPoint();
}

void Camera::moveTo(const Vec3<float> & newPos) {
    m_pos = newPos;
    calcRefPoint();
}

const Vec3<float> & Camera::pos() {
    return m_pos;
}

void Camera::pointAt(const Vec3<float> & vec) {
    m_look = vec;
    calcRefPoint();
}

void Camera::pointBy(const Vec3<float> & delta) {
    m_look += delta;
    calcRefPoint();
}

void Camera::moveLeft(float distance) {
    moveRight(-distance);
}

void Camera::moveRight(float distance) {
    // get the vector pointing to the left
    // multiply by distance
    // add to m_pos
    m_pos += m_look.cross(m_up).normalize() * distance;
    calcRefPoint();
}

void Camera::moveUp(float distance) {
    m_pos += m_up.normalized() * distance;
    calcRefPoint();
}

void Camera::moveDown(float distance) {
    moveUp(-distance);
}

void Camera::moveForward(float distance) {
    m_pos += m_look.normalized() * distance;
    calcRefPoint();
}

void Camera::moveBackward(float distance) {
    moveForward(-distance);
}

void Camera::rotateVecAboutVec(Vec3<float> & vec,
    const Vec3<float> & aboutVec, float angle)
{
    Vec3<float> normAbout = aboutVec.normalized();
    Matrix33<float> I;
    Matrix33<float> L(
        0, normAbout.z, -normAbout.y,
        -normAbout.z, 0, normAbout.x,
        normAbout.y, -normAbout.x, 0
    );
    float d = normAbout.length();
    vec *= ( I + (sin(angle)/d) * L + (((1 - cos(angle))/(d*d)) * (L * L) ));
}

void Camera::pointLeft(float radians) {
    rotateVecAboutVec(m_look, Vec3<float>(0,0,1), radians);
    calcRefPoint();
}

void Camera::pointRight(float radians) {
    pointLeft(-radians);
}

void Camera::pointUp(float radians) {
    // rotate about left vector
    rotateVecAboutVec(m_look, m_look.cross(m_up), radians);
    calcRefPoint();
}

void Camera::pointDown(float radians) {
    pointUp(-radians);
}

