#ifndef VEC3D_H_INCLUDED
#define VEC3D_H_INCLUDED

class vec3d
{
    public:
        double x;
        double y;
        double z;

        vec3d(double px=0., double py=0., double pz=0.)
        {
            x=px;y=py;z=pz;
        }

        void toString()
        {
            cout<<"----> vec3d x:"<<x<<" y:"<<y<<" z:"<<z<<endl;
        }

};

#endif // VEC3D_H_INCLUDED
