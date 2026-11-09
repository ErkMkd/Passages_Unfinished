#ifndef DKSAFFICHAGESDEBOGUE_H_INCLUDED
#define DKSAFFICHAGESDEBOGUE_H_INCLUDED


#include <math.h>

class dksAffichagesDebogue
{
    public:
        //======================================================================
        //                        Affichages pour débugages
        //======================================================================

        static void affiche_point(double Ax,double Ay, double Az, uint32_t couleur)
        {
            //Sauve les états d'opengl:
            GLboolean GL_CULL_FACE_mem=dksEtat::est_active(GL_CULL_FACE);
            GLboolean GL_LIGHTING_mem=dksEtat::est_active(GL_LIGHTING);
            if (GL_CULL_FACE_mem==GL_TRUE) dksEtat::desactive(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE) dksEtat::desactive(GL_LIGHTING);
            GLint GL_POLYGON_MODE_mem[2];
            glGetIntegerv(GL_POLYGON_MODE,GL_POLYGON_MODE_mem);
            GLfloat taille_points_mem[2];
            glGetFloatv(GL_POINT_SIZE,taille_points_mem);

            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            dksEtat::taille_point(4.);
            dksImmediat::debut(GL_TRIANGLES);
            dksImmediat::couleur3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            dksImmediat::sommet3d(Ax,Ay,Az);
            dksImmediat::sommet3d(Ax,Ay,Az);
            dksImmediat::sommet3d(Ax,Ay,Az);
            dksImmediat::fin();

            //Restitue les états d'openGl:
            if (GL_CULL_FACE_mem==GL_TRUE) dksEtat::active(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE)dksEtat::active(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK,GL_POLYGON_MODE_mem[0]);
            dksEtat::taille_point(taille_points_mem[0]);

        }

        static void affiche_segment(double Ax,double Ay, double Az,
                                double Bx,double By, double Bz, uint32_t couleur)
        {
            //Sauve les états d'opengl:
            GLboolean GL_CULL_FACE_mem=dksEtat::est_active(GL_CULL_FACE);
            GLboolean GL_LIGHTING_mem=dksEtat::est_active(GL_LIGHTING);
            if (GL_CULL_FACE_mem==GL_TRUE) dksEtat::desactive(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE) dksEtat::desactive(GL_LIGHTING);
            GLint GL_POLYGON_MODE_mem[2];
            glGetIntegerv(GL_POLYGON_MODE,GL_POLYGON_MODE_mem);
            GLfloat taille_points_mem[2];
            glGetFloatv(GL_POINT_SIZE,taille_points_mem);

            //Affiche le segment:
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            dksImmediat::debut(GL_LINES);
            dksImmediat::couleur3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            dksImmediat::sommet3d(Ax,Ay,Az);
            dksImmediat::sommet3d(Bx,By,Bz);
            dksImmediat::fin();

            //Affiche les extrémités:
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            dksEtat::taille_point(4.);
            dksImmediat::debut(GL_POINTS);
            dksImmediat::couleur3f(1.,0,0);dksImmediat::sommet3d(Ax,Ay,Az);
            dksImmediat::couleur3f(0,1.,0);dksImmediat::sommet3d(Bx,By,Bz);
            dksImmediat::fin();

            //Restitue les états d'openGl:
            if (GL_CULL_FACE_mem==GL_TRUE) dksEtat::active(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE)dksEtat::active(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK,GL_POLYGON_MODE_mem[0]);
            dksEtat::taille_point(taille_points_mem[0]);
        }


        static void affiche_polygone(vector<uint32_t>* aretes,uint16_t nbrSommets, vector<double>* tabSommets,uint32_t couleur)
        {
            dksEtat::desactive(GL_CULL_FACE);

            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            dksImmediat::debut(GL_POLYGON);
            dksImmediat::couleur3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            for (uint16_t i=0 ;i<nbrSommets;i++)
            {
                uint32_t index=(*aretes)[i]*3;
                dksImmediat::sommet3d((*tabSommets)[index],(*tabSommets)[index+1],(*tabSommets)[index+2]);
            }
            dksImmediat::fin();
            dksEtat::active(GL_CULL_FACE);
        }
};

#endif // DKSAFFICHAGESDEBOGUE_H_INCLUDED
