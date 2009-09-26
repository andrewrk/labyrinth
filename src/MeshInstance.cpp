#include "GL/freeglut.h"

#include "MeshInstance.h"

MeshInstance::MeshInstance(Mesh * mesh, Vec3<float> pos, float scale,
                            Vec3<float> up, Vec3<float> forward) :
    m_mesh(mesh),
    m_pos(pos),
    m_scale(scale),
    m_up(up),
    m_forward(forward)
{
    m_mesh->init();
}

MeshInstance::MeshInstance(Mesh * mesh) :
    m_mesh(mesh),
    m_pos(Vec3<float>(0,0,0)),
    m_scale(1.0f),
    m_up(Vec3<float>(0, 0, 1)),
    m_forward(Vec3<float>(-1, -1, 0))
{
    m_mesh->init();
}

// setters
// how to scale the mesh
void MeshInstance::setScale(float scale) {
    m_scale = scale;
}

// vector pointing up
void MeshInstance::setUp(Vec3<float> up) {
    m_up = up;
}

// vector pointing forward
void MeshInstance::setForward(Vec3<float> forward) {
    m_forward = forward;
}

void MeshInstance::render() {
    // scale the mesh and draw it 
    glPushMatrix();

    glTranslatef(m_pos.x, m_pos.y, m_pos.z);
    glScalef(m_scale, m_scale, m_scale);
    m_mesh->draw();

    glPopMatrix();
}

void MeshInstance::draw() {
    // we can't cache stuff, just do the render
    render();
}

