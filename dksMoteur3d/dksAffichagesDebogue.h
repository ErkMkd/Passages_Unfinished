#ifndef DKSAFFICHAGESDEBOGUE_H_INCLUDED
#define DKSAFFICHAGESDEBOGUE_H_INCLUDED


#include <math.h>

class dksAffichagesDebogue
{
    public:
        //======================================================================
        //                        Affichages pour d�bugages
        //======================================================================

        static void affiche_point(double Ax,double Ay, double Az, uint32_t couleur)
        {
            //Sauve les �tats d'opengl:
            GLboolean GL_CULL_FACE_mem=glIsEnabled(GL_CULL_FACE);
            GLboolean GL_LIGHTING_mem=glIsEnabled(GL_LIGHTING);
            if (GL_CULL_FACE_mem==GL_TRUE) glDisable(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE) glDisable(GL_LIGHTING);
            GLint GL_POLYGON_MODE_mem[2];
            glGetIntegerv(GL_POLYGON_MODE,GL_POLYGON_MODE_mem);
            GLfloat taille_points_mem[2];
            glGetFloatv(GL_POINT_SIZE,taille_points_mem);

            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            glPointSize(4.);
            glBegin(GL_TRIANGLES);
            glColor3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            glVertex3d(Ax,Ay,Az);
            glVertex3d(Ax,Ay,Az);
            glVertex3d(Ax,Ay,Az);
            glEnd();

            //Restitue les �tats d'openGl:
            if (GL_CULL_FACE_mem==GL_TRUE) glEnable(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE)glEnable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK,GL_POLYGON_MODE_mem[0]);
            glPointSize(taille_points_mem[0]);

        }

        static void affiche_segment(double Ax,double Ay, double Az,
                                double Bx,double By, double Bz, uint32_t couleur)
        {
            //Sauve les �tats d'opengl:
            GLboolean GL_CULL_FACE_mem=glIsEnabled(GL_CULL_FACE);
            GLboolean GL_LIGHTING_mem=glIsEnabled(GL_LIGHTING);
            if (GL_CULL_FACE_mem==GL_TRUE) glDisable(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE) glDisable(GL_LIGHTING);
            GLint GL_POLYGON_MODE_mem[2];
            glGetIntegerv(GL_POLYGON_MODE,GL_POLYGON_MODE_mem);
            GLfloat taille_points_mem[2];
            glGetFloatv(GL_POINT_SIZE,taille_points_mem);

            //Affiche le segment:
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glBegin(GL_LINES);
            glColor3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            glVertex3d(Ax,Ay,Az);
            glVertex3d(Bx,By,Bz);
            glEnd();

            //Affiche les extr�mit�s:
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            glPointSize(4.);
            glBegin(GL_POINTS);
            glColor3f(1.,0,0);glVertex3d(Ax,Ay,Az);
            glColor3f(0,1.,0);glVertex3d(Bx,By,Bz);
            glEnd();

            //Restitue les �tats d'openGl:
            if (GL_CULL_FACE_mem==GL_TRUE) glEnable(GL_CULL_FACE);
            if (GL_LIGHTING_mem==GL_TRUE)glEnable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK,GL_POLYGON_MODE_mem[0]);
            glPointSize(taille_points_mem[0]);
        }


        static void affiche_polygone(vector<uint32_t>* aretes,uint16_t nbrSommets, vector<double>* tabSommets,uint32_t couleur)
        {
            glDisable(GL_CULL_FACE);

            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glBegin(GL_POLYGON);
            glColor3i((couleur<<7)&0x7f800000,(couleur<<15)&0x7f800000,(couleur<<23)&0x7f800000);
            for (uint16_t i=0 ;i<nbrSommets;i++)
            {
                uint32_t index=(*aretes)[i]*3;
                glVertex3d((*tabSommets)[index],(*tabSommets)[index+1],(*tabSommets)[index+2]);
            }
            glEnd();
            glEnable(GL_CULL_FACE);
        }
};

#endif // DKSAFFICHAGESDEBOGUE_H_INCLUDED
