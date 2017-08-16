#include "camera.h"

Camera::Camera()
{
    targetWidth = 1.0f;
    targetHeight = 1.0f;
}

Camera::Camera(float width, float height)
{
    targetWidth = width;
    targetHeight = height;
}

Mat4 Camera::getViewProjectionMatrix()
{
    float aspect = this->targetWidth / this->targetHeight;
    Mat4 view_to_clip = Mat4::Perspective(this->FOV, aspect, this->zNear, this->zFar);

    Mat4 world_to_view = Mat4::TRS(-1.0f * this->transform.position, this->transform.rotation.inverse(), Vec3(1.0f, 1.0f, 1.0f));
    return view_to_clip*world_to_view;
}

