#ifndef MATRICE4X4_H_INCLUDED
#define MATRICE4X4_H_INCLUDED

//Structure de calcul de matrices:
//Cet incongru d'OpenGl inverse les lignes et les colonnes.
class Matrice4x4
{
    public:
        GLdouble matrice[16];
    private:
        GLdouble lignes[4][4];
        GLfloat matrice_f[16];

    public:

        //===================================
        //      CONSTRUCTEUR
        //===================================
        Matrice4x4(GLdouble a=1., GLdouble b=0., GLdouble c=0., GLdouble d=0.,
                   GLdouble e=0., GLdouble f=1., GLdouble g=0., GLdouble h=0.,
                   GLdouble i=0., GLdouble j=0., GLdouble k=1., GLdouble l=0.,
                   GLdouble m=0., GLdouble n=0., GLdouble o=0., GLdouble p=1.)
        {
            matrice[0]=a;
            matrice[4]=b;
            matrice[8]=c;
            matrice[12]=d;

            matrice[1]=e;
            matrice[5]=f;
            matrice[9]=g;
            matrice[13]=h;

            matrice[2]=i;
            matrice[6]=j;
            matrice[10]=k;
            matrice[14]=l;

            matrice[3]=m;
            matrice[7]=n;
            matrice[11]=o;
            matrice[15]=p;
        }

        //Matrice identité:
        void identite()
        {
            matrice[0]=1.;matrice[4]=0.; matrice[8]=0.;matrice[12]=0.;
            matrice[1]=0.;matrice[5]=1.; matrice[9]=0.;matrice[13]=0.;
            matrice[2]=0.;matrice[6]=0.; matrice[10]=1.;matrice[14]=0.;
            matrice[3]=0.;matrice[7]=0.; matrice[11]=0.;matrice[15]=1.;
        }

        //Initialise une matrice de rotation (angle,axe)
        void initialise_rotation(double angle, double ax, double ay, double az)
        {
            double cos_a = cos(angle);
            double un_moins_cos = 1. - cos_a;

            double sin_a= sin(angle);

            matrice[0] = cos_a + ax * ax * un_moins_cos;
            matrice[4] = -sin_a * az + un_moins_cos * ax * ay;
            matrice[8] = sin_a * ay + un_moins_cos * ax * az;
            matrice[12] = 0.;

            matrice[1] = az * sin_a + un_moins_cos * ax * ay;
            matrice[5] = cos_a + un_moins_cos * ay * ay;
            matrice[9] = -sin_a * ax + un_moins_cos * az * ay;
            matrice[13] = 0.;

            matrice[2] = -sin_a * ay + un_moins_cos * az * ax;
            matrice[6] = ax * sin_a + un_moins_cos * ay * az;
            matrice[10] = cos_a + un_moins_cos * az * az;
            matrice[14] = 0.;

            matrice[3]=0.;matrice[7]=0.; matrice[11]=0.;matrice[15]=1.;
        }

        //Initialise une matrice de rotation puis transition (angle,axe,transition)
        void initialise_transition(double dx,double dy,double dz)
        {
            matrice[0] = 1.;
            matrice[4] = 0.;
            matrice[8] = 0.;
            matrice[12] = dx;

            matrice[1] = 0.;
            matrice[5] = 1.;
            matrice[9] = 0.;
            matrice[13] = dy;

            matrice[2] = 0.;
            matrice[6] = 0.;
            matrice[10] = 1.;
            matrice[14] = dz;

            matrice[3]=0.;matrice[7]=0.; matrice[11]=0.;matrice[15]=1.;
        }

        //La matrice d'entrée m est située à droite:
        void multiplication_droite(Matrice4x4& m, Matrice4x4& res)
        {
            res.matrice[0]=matrice[0]*m.matrice[0]+matrice[4]*m.matrice[1]+matrice[8]*m.matrice[2]+matrice[12]*m.matrice[3];
            res.matrice[4]=matrice[0]*m.matrice[4]+matrice[4]*m.matrice[5]+matrice[8]*m.matrice[6]+matrice[12]*m.matrice[7];
            res.matrice[8]=matrice[0]*m.matrice[8]+matrice[4]*m.matrice[9]+matrice[8]*m.matrice[10]+matrice[12]*m.matrice[11];
            res.matrice[12]=matrice[0]*m.matrice[12]+matrice[4]*m.matrice[13]+matrice[8]*m.matrice[14]+matrice[12]*m.matrice[15];

            res.matrice[1]=matrice[1]*m.matrice[0]+matrice[5]*m.matrice[1]+matrice[9]*m.matrice[2]+matrice[13]*m.matrice[3];
            res.matrice[5]=matrice[1]*m.matrice[4]+matrice[5]*m.matrice[5]+matrice[9]*m.matrice[6]+matrice[13]*m.matrice[7];
            res.matrice[9]=matrice[1]*m.matrice[8]+matrice[5]*m.matrice[9]+matrice[9]*m.matrice[10]+matrice[13]*m.matrice[11];
            res.matrice[13]=matrice[1]*m.matrice[12]+matrice[5]*m.matrice[13]+matrice[9]*m.matrice[14]+matrice[13]*m.matrice[15];

            res.matrice[2]=matrice[2]*m.matrice[0]+matrice[6]*m.matrice[1]+matrice[10]*m.matrice[2]+matrice[14]*m.matrice[3];
            res.matrice[6]=matrice[2]*m.matrice[4]+matrice[6]*m.matrice[5]+matrice[10]*m.matrice[6]+matrice[14]*m.matrice[7];
            res.matrice[10]=matrice[2]*m.matrice[8]+matrice[6]*m.matrice[9]+matrice[10]*m.matrice[10]+matrice[14]*m.matrice[11];
            res.matrice[14]=matrice[2]*m.matrice[12]+matrice[6]*m.matrice[13]+matrice[10]*m.matrice[14]+matrice[14]*m.matrice[15];

            res.matrice[3]=matrice[3]*m.matrice[0]+matrice[7]*m.matrice[1]+matrice[11]*m.matrice[2]+matrice[15]*m.matrice[3];
            res.matrice[7]=matrice[3]*m.matrice[4]+matrice[7]*m.matrice[5]+matrice[11]*m.matrice[6]+matrice[15]*m.matrice[7];
            res.matrice[11]=matrice[3]*m.matrice[8]+matrice[7]*m.matrice[9]+matrice[11]*m.matrice[10]+matrice[15]*m.matrice[11];
            res.matrice[15]=matrice[3]*m.matrice[12]+matrice[7]*m.matrice[13]+matrice[11]*m.matrice[14]+matrice[15]*m.matrice[15];


        }

        //Le vecteur d'entrée [x,y,z,w] est situé à droite:
        void multiplication_droite(double* vect,double* res)
        {
            res[0]=vect[0]*matrice[0]+vect[1]*matrice[4]+vect[2]*matrice[8]+vect[3]*matrice[12];
            res[1]=vect[0]*matrice[1]+vect[1]*matrice[5]+vect[2]*matrice[9]+vect[3]*matrice[13];
            res[2]=vect[0]*matrice[2]+vect[1]*matrice[6]+vect[2]*matrice[10]+vect[3]*matrice[14];
            res[3]=vect[0]*matrice[3]+vect[1]*matrice[7]+vect[2]*matrice[11]+vect[3]*matrice[15];
        }

        //La matrice d'entrée m est située à gauche:
        void multiplication_gauche(Matrice4x4& m, Matrice4x4& res)
        {
            res.matrice[0]=m.matrice[0]*matrice[0]+m.matrice[4]*matrice[1]+m.matrice[8]*matrice[2]+m.matrice[12]*matrice[3];
            res.matrice[4]=m.matrice[0]*matrice[4]+m.matrice[4]*matrice[5]+m.matrice[8]*matrice[6]+m.matrice[12]*matrice[7];
            res.matrice[8]=m.matrice[0]*matrice[8]+m.matrice[4]*matrice[9]+m.matrice[8]*matrice[10]+m.matrice[12]*matrice[11];
            res.matrice[12]=m.matrice[0]*matrice[12]+m.matrice[4]*matrice[13]+m.matrice[8]*matrice[14]+m.matrice[12]*matrice[15];

            res.matrice[1]=m.matrice[1]*matrice[0]+m.matrice[5]*matrice[1]+m.matrice[9]*matrice[2]+m.matrice[13]*matrice[3];
            res.matrice[5]=m.matrice[1]*matrice[4]+m.matrice[5]*matrice[5]+m.matrice[9]*matrice[6]+m.matrice[13]*matrice[7];
            res.matrice[9]=m.matrice[1]*matrice[8]+m.matrice[5]*matrice[9]+m.matrice[9]*matrice[10]+m.matrice[13]*matrice[11];
            res.matrice[13]=m.matrice[1]*matrice[12]+m.matrice[5]*matrice[13]+m.matrice[9]*matrice[14]+m.matrice[13]*matrice[15];

            res.matrice[2]=m.matrice[2]*matrice[0]+m.matrice[6]*matrice[1]+m.matrice[10]*matrice[2]+m.matrice[14]*matrice[3];
            res.matrice[6]=m.matrice[2]*matrice[4]+m.matrice[6]*matrice[5]+m.matrice[10]*matrice[6]+m.matrice[14]*matrice[7];
            res.matrice[10]=m.matrice[2]*matrice[8]+m.matrice[6]*matrice[9]+m.matrice[10]*matrice[10]+m.matrice[14]*matrice[11];
            res.matrice[14]=m.matrice[2]*matrice[12]+m.matrice[6]*matrice[13]+m.matrice[10]*matrice[14]+m.matrice[14]*matrice[15];

            res.matrice[3]=m.matrice[3]*matrice[0]+m.matrice[7]*matrice[1]+m.matrice[11]*matrice[2]+m.matrice[15]*matrice[3];
            res.matrice[7]=m.matrice[3]*matrice[4]+m.matrice[7]*matrice[5]+m.matrice[11]*matrice[6]+m.matrice[15]*matrice[7];
            res.matrice[11]=m.matrice[3]*matrice[8]+m.matrice[7]*matrice[9]+m.matrice[11]*matrice[10]+m.matrice[15]*matrice[11];
            res.matrice[15]=m.matrice[3]*matrice[12]+m.matrice[7]*matrice[13]+m.matrice[11]*matrice[14]+m.matrice[15]*matrice[15];


        }


        GLfloat* renvoie_matrice_f()
        {
            for (uint8_t i=0;i<16;i++)
            {
                matrice_f[i]=GLfloat(matrice[i]);
            }
            return matrice_f;
        }

        GLdouble* renvoie_ligne(uint16_t n)
        {
            if (n>3)n=3;
            GLdouble* ligne=lignes[n];

            ligne[0]=matrice[0+n];ligne[1]=matrice[4+n];ligne[2]=matrice[8+n]; ligne[3]=matrice[12+n];
            return ligne;
        }

        void affiche()
        {
            cout<<matrice[0]<<" , "<<matrice[4]<<" , "<<matrice[8]<<" , "<<matrice[12]<<endl;
            cout<<matrice[1]<<" , "<<matrice[5]<<" , "<<matrice[9]<<" , "<<matrice[13]<<endl;
            cout<<matrice[2]<<" , "<<matrice[6]<<" , "<<matrice[10]<<" , "<<matrice[14]<<endl;
            cout<<matrice[3]<<" , "<<matrice[7]<<" , "<<matrice[11]<<" , "<<matrice[15]<<endl;
            cout<<endl;
        }

};

#endif // MATRICE4X4_H_INCLUDED
