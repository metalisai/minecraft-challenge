#pragma once

#include <functional>

struct IVec3
{
    IVec3() {}

    IVec3(int x, int y, int z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    bool operator==(const IVec3 &other) const
    { 
        return (x == other.x
            && y == other.y
            && z == other.z);
    }

    int x,y,z;
};

inline IVec3 operator + (IVec3 const &l, IVec3 const &r)
{
    IVec3 ret;
    ret.x = l.x + r.x;
    ret.y = l.y + r.y;
    ret.z = l.z + r.z;
    return ret;
}

namespace std
{
    template<> struct less<IVec3>
    {
       bool operator() (const IVec3& lhs, const IVec3& rhs) const
       {
           float lhsd = lhs.x+lhs.y+lhs.z;
           float rhsd = rhs.x+rhs.y+rhs.z;
           return lhsd < rhsd;
       }
    };

    template <>
    struct hash<IVec3>
    {
        std::size_t operator()(const IVec3& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            return ((hash<int>()(k.x)
            ^ (hash<int>()(k.y) << 1)) >> 1)
            ^ (hash<int>()(k.z) << 1);
        }
    };
}

