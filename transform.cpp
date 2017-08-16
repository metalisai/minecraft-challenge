#include "transform.h"

Transform::Transform()
{
    this->position = Vec3(0.0f, 0.0f, 0.0f);
    this->rotation = Quaternion::Identity();
    this->scale = Vec3(1.0f, 1.0f, 1.0f);
}

Mat4 Transform::getModelMatrix()
{
    return Mat4::TRS(this->position, this->rotation, this->scale);
}

