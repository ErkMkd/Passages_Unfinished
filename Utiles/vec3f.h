#ifndef VEC3F_H_INCLUDED
#define VEC3F_H_INCLUDED

class vec3f
{
    public:
        float x;
        float y;
        float z;

        vec3f(float px=0., float py=0., float pz)
        {
            x=px;y=py;z=pz;
        }
};

#endif // VEC3F_H_INCLUDED
