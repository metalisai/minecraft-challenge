#pragma once

#include <math.h>
#define TUT_DEG2RAD_F 0.0174532925f

// Data structures

struct Vec2
{
public:
	Vec2() {}

	Vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float x, y;
};

struct Vec3
{
public:
	Vec3() {}

	Vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	static Vec3 Cross(Vec3 &l, Vec3 &r)
	{
		return Vec3(l.y*r.z - l.z*r.y, l.z*r.x - l.x*r.z, l.x*r.y - l.y*r.x);
	}

	static float Dot(Vec3 &l, Vec3 &r)
	{
		return l.x*r.x + l.y*r.y + l.z*r.z;
	}

	float x, y, z;
};

struct Vec4
{
public:
    Vec4() {}

    Vec4(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    union {
        struct  {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
        float v[4];
    };
};

struct Mat4
{
public:
	Mat4() {}

	Mat4(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44)
	{
		this->m11 = m11; this->m12 = m12; this->m13 = m13; this->m11 = m14;
		this->m21 = m21; this->m22 = m22; this->m23 = m23; this->m21 = m24;
		this->m31 = m31; this->m32 = m32; this->m33 = m33; this->m31 = m34;
		this->m41 = m41; this->m42 = m42; this->m43 = m43; this->m41 = m44;
	}

	static Mat4 Perspective(float fov, float aspect, float zNear, float zFar);
	static Mat4 Identity();
	static Mat4 Rotation(struct Quaternion const &q);
	static Mat4 TRS(Vec3 const &t, struct Quaternion const &r, Vec3 const &s);

	union
	{
		struct // memory
		{
			float m[16];
		};

		struct // colum-row
		{
			float cr[4][4];
		};

		struct // memory layout independent version
		{
			float m11, m21, m31, m41;
			float m12, m22, m32, m42;
			float m13, m23, m33, m43;
			float m14, m24, m34, m44;
		};
	};
};

struct Quaternion
{
	Quaternion() {}

	Quaternion(float w, float x, float y, float z)
	{
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;
	}

    static Quaternion Identity()
    {
        return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    }

	static Quaternion AngleAxis(float angleDeg, Vec3 axis)
	{
		float halfAngleRad = (TUT_DEG2RAD_F*angleDeg)/2.0f;
		float sinHAR = sinf(halfAngleRad);
		return Quaternion(cosf(halfAngleRad), axis.x * sinHAR, axis.y * sinHAR, axis.z * sinHAR);
	}

    Quaternion inverse()
    {
        return Quaternion(-this->w, this->x, this->y, this->z);
    }

	float w, x, y, z;
};

// Function declarations

inline Vec2 operator += (Vec2 &l, Vec2 const &r)
{
	l.x += r.x;
	l.y += r.y;
	return l;
}

inline Vec3 operator - (Vec3 const &l, Vec3 const &r)
{
	return Vec3(l.x-r.x, l.y-r.y, l.z-r.z);
}

inline Vec3 operator + (Vec3 const &l, Vec3 const &r)
{
	return Vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

inline Vec3 operator * (Vec3 const &l, float const &r)
{
	return Vec3(l.x*r, l.y*r, l.z*r);
}

inline Vec3 operator * (float const &r, Vec3 const &l)
{
	return Vec3(l.x*r, l.y*r, l.z*r);
}

inline Vec3 operator -= (Vec3 &l, Vec3 const &r)
{
	l.x -= r.x;
	l.y -= r.y;
	l.z -= r.z;
	return l;
}

inline Vec3 operator += (Vec3 &l, Vec3 const &r)
{
	l.x += r.x;
	l.y += r.y;
	l.z += r.z;
	return l;
}

inline Vec3 operator *= (Vec3 &l, float r)
{
    l.x *= r;
    l.y *= r;
    l.z *= r;
    return l;
}

inline Mat4 operator * (Mat4 const &l, Mat4 const &r)
{
	Mat4 ret;
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 4; row++)
		{
			ret.cr[col][row] = 0.0f;
			for (int n = 0; n < 4; n++)
			{
				ret.cr[col][row] += l.cr[n][row] * r.cr[col][n];
			}
		}
	}
	return ret;
}


inline Vec4 operator * (Mat4 const &m, Vec4 const &v)
{
    Vec4 ret;
    ret.x = v.x*m.m11+v.y*m.m12+v.z*m.m13+v.w*m.m14;
    ret.y = v.x*m.m21+v.y*m.m22+v.z*m.m23+v.w*m.m24;
    ret.z = v.x*m.m31+v.y*m.m32+v.z*m.m33+v.w*m.m34;
    ret.w = v.x*m.m41+v.y*m.m42+v.z*m.m43+v.w*m.m44;
    return ret;
}

inline Mat4 operator *= (Mat4 &l, Mat4 const &r)
{
	l = l*r;
	return l;
}

inline Mat4 Mat4::Perspective(float fov, float aspect, float zNear, float zFar)
{
	Mat4 ret;
	const float h = 1.0f / (float)tan(fov*TUT_DEG2RAD_F*0.5f);
	float neg_depth = zNear - zFar;

	// row 1
	ret.m11 = h / aspect;
	ret.m12 = 0.0f;
	ret.m13 = 0.0f;
	ret.m14 = 0.0f;

	ret.m21 = 0.0f;
	ret.m22 = h;
	ret.m23 = 0.0f;
	ret.m24 = 0.0f;

	ret.m31 = 0.0f;
	ret.m32 = 0.0f;
	ret.m33 = (zFar + zNear) / neg_depth;
	ret.m34 = 2.0f*(zNear*zFar) / neg_depth;

	ret.m41 = 0.0f;
	ret.m42 = 0.0f;
	ret.m43 = -1.0f;
	ret.m44 = 0.0f;

	return ret;
}

inline Mat4 Mat4:: Rotation(struct Quaternion const &q)
{
	Mat4 ret;
	ret.m11 = 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z;
	ret.m12 = 2.0f*q.x*q.y - 2.0f*q.z*q.w;
	ret.m13 = 2.0f*q.x*q.z + 2.0f*q.y*q.w;
	ret.m14 = 0.0f;

	ret.m21 = 2.0f*q.x*q.y + 2.0f*q.z*q.w;
	ret.m22 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z;
	ret.m23 = 2.0f*q.y*q.z - 2.0f*q.x*q.w;
	ret.m24 = 0.0f;

	ret.m31 = 2.0f*q.x*q.z - 2.0f*q.y*q.w;
	ret.m32 = 2.0f*q.y*q.z + 2.0f*q.x*q.w;
	ret.m33 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y;
	ret.m34 = 0.0f;

	ret.m41 = 0.0f;
	ret.m42 = 0.0f;
	ret.m43 = 0.0f;
	ret.m44 = 1.0f;

	return ret;
}

inline Mat4 Mat4::TRS(Vec3 const &t, struct Quaternion const &r, Vec3 const &s)
{
	Mat4 translate = Mat4::Identity();
	translate.m14 = t.x;
	translate.m24 = t.y;
	translate.m34 = t.z;

	Mat4 rotate = Mat4::Rotation(r);

	Mat4 scale = Mat4::Identity();
	scale.m11 = s.x;
	scale.m22 = s.y;
	scale.m33 = s.z;

	return translate*rotate*scale;
}

inline Mat4 Mat4::Identity()
{
	Mat4 ret;
	ret.m11 = 1.0f;
	ret.m12 = 0.0f;
	ret.m13 = 0.0f;
	ret.m14 = 0.0f;

	ret.m21 = 0.0f;
	ret.m22 = 1.0f;
	ret.m23 = 0.0f;
	ret.m24 = 0.0f;

	ret.m31 = 0.0f;
	ret.m32 = 0.0f;
	ret.m33 = 1.0f;
	ret.m34 = 0.0f;

	ret.m41 = 0.0f;
	ret.m42 = 0.0f;
	ret.m43 = 0.0f;
	ret.m44 = 1.0f;

	return ret;
}


inline Quaternion operator * (Quaternion const &l, Quaternion const &r)
{
	Quaternion ret;
	ret.w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;
	ret.x = l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y;
	ret.y = l.w * r.y - l.x * r.z + l.y * r.w + l.z * r.x;
	ret.z = l.w * r.z + l.x * r.y - l.y * r.x + l.z * r.w;
	return ret;
}

inline Quaternion operator *= (Quaternion &l, Quaternion const &r)
{
	l = l*r;
	return l;
}


