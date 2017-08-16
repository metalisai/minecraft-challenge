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

    Mat4 rotate = Mat4::Rotation(this->transform.rotation.inverse());
    Mat4 translate = Mat4::Identity();
    translate.m14 = -transform.position.x;
    translate.m24 = -transform.position.y;
    translate.m34 = -transform.position.z;

    Mat4 world_to_view = rotate * translate;
    return view_to_clip*world_to_view;
}

