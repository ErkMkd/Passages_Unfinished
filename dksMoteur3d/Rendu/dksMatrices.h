#ifndef DKSMATRICES_H_INCLUDED
#define DKSMATRICES_H_INCLUDED

#include <vector>
#include <string.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//*************************************************************************
//          Piles de matrices du moteur
//
//  Remplacent celles d'OpenGL (glMatrixMode, glPushMatrix, glMultMatrix...),
//  absentes du profil Core. Mêmes conventions qu'OpenGL: matrices rangées par
//  colonnes, angles en degrés, chaque transformation multiplie la matrice
//  courante à droite.
//  Pendant le portage, chaque changement est recopié dans OpenGL, pour le
//  pipeline fixe et les shaders qui lisent encore gl_ModelViewMatrix.
//
//  Une seule pile de texture: elle s'applique à l'unité de texture active,
//  comme dans OpenGL (le moteur n'en utilise qu'une).
//*************************************************************************

class dksMatrices
{
    private:
        enum { MODELE_VUE, PROJECTION, TEXTURE, NBR_PILES };
        inline static std::vector<glm::dmat4> piles[NBR_PILES]={{glm::dmat4(1.)},{glm::dmat4(1.)},{glm::dmat4(1.)}};
        inline static int pile_courante=MODELE_VUE;

        static glm::dmat4& courante()
        {
            return piles[pile_courante].back();
        }

        //Recopie la matrice courante dans OpenGL (le mode d'OpenGL suit celui des piles).
        static void synchronise()
        {
            glLoadMatrixd(glm::value_ptr(courante()));
        }

    public:
        //-------------------------------------------------------------
        //      Équivalents des fonctions d'OpenGL et de GLU
        //-------------------------------------------------------------
        static void mode(GLenum mode_gl)    //glMatrixMode
        {
            switch (mode_gl)
            {
                case GL_PROJECTION: pile_courante=PROJECTION; break;
                case GL_TEXTURE: pile_courante=TEXTURE; break;
                default: pile_courante=MODELE_VUE; break;
            }
            glMatrixMode(mode_gl);
        }

        static void empile()    //glPushMatrix
        {
            piles[pile_courante].push_back(courante());
        }

        static void depile()    //glPopMatrix
        {
            if (piles[pile_courante].size()>1) piles[pile_courante].pop_back();
            synchronise();
        }

        static void identite()  //glLoadIdentity
        {
            courante()=glm::dmat4(1.);
            synchronise();
        }

        static void charge(const GLdouble* m)   //glLoadMatrixd
        {
            courante()=glm::make_mat4(m);
            synchronise();
        }

        static void charge(const GLfloat* m)    //glLoadMatrixf
        {
            courante()=glm::dmat4(glm::make_mat4(m));
            synchronise();
        }

        static void multiplie(const GLdouble* m)    //glMultMatrixd
        {
            courante()*=glm::make_mat4(m);
            synchronise();
        }

        static void multiplie(const GLfloat* m)     //glMultMatrixf
        {
            courante()*=glm::dmat4(glm::make_mat4(m));
            synchronise();
        }

        static void translation(double x,double y,double z)     //glTranslate
        {
            courante()=glm::translate(courante(),glm::dvec3(x,y,z));
            synchronise();
        }

        static void rotation(double angle,double x,double y,double z)   //glRotate (angle en degrés)
        {
            courante()=glm::rotate(courante(),glm::radians(angle),glm::dvec3(x,y,z));
            synchronise();
        }

        static void echelle(double x,double y,double z)     //glScale
        {
            courante()=glm::scale(courante(),glm::dvec3(x,y,z));
            synchronise();
        }

        static void ortho(double gauche,double droite,double bas,double haut,double proche,double lointain)    //glOrtho
        {
            courante()*=glm::ortho(gauche,droite,bas,haut,proche,lointain);
            synchronise();
        }

        static void ortho2D(double gauche,double droite,double bas,double haut)    //gluOrtho2D
        {
            ortho(gauche,droite,bas,haut,-1.,1.);
        }

        static void frustum(double gauche,double droite,double bas,double haut,double proche,double lointain)  //glFrustum
        {
            courante()*=glm::frustum(gauche,droite,bas,haut,proche,lointain);
            synchronise();
        }

        static void perspective(double ouverture,double rapport_xy,double zmin,double zmax)    //gluPerspective (ouverture en degrés)
        {
            courante()*=glm::perspective(glm::radians(ouverture),rapport_xy,zmin,zmax);
            synchronise();
        }

        static void regarde(double oeil_x,double oeil_y,double oeil_z,
                            double cible_x,double cible_y,double cible_z,
                            double haut_x,double haut_y,double haut_z)   //gluLookAt
        {
            courante()*=glm::lookAt(glm::dvec3(oeil_x,oeil_y,oeil_z),glm::dvec3(cible_x,cible_y,cible_z),glm::dvec3(haut_x,haut_y,haut_z));
            synchronise();
        }

        static void lit(GLenum quelle,GLdouble* m)  //glGetDoublev(GL_xxx_MATRIX)
        {
            int pile= quelle==GL_PROJECTION_MATRIX ? PROJECTION : quelle==GL_TEXTURE_MATRIX ? TEXTURE : MODELE_VUE;
            memcpy(m,glm::value_ptr(piles[pile].back()),16*sizeof(GLdouble));
        }

        //-------------------------------------------------------------
        //      Matrices courantes, pour les shaders
        //-------------------------------------------------------------
        static const glm::dmat4& modele_vue()
        {
            return piles[MODELE_VUE].back();
        }

        static const glm::dmat4& projection()
        {
            return piles[PROJECTION].back();
        }

        static const glm::dmat4& texture()
        {
            return piles[TEXTURE].back();
        }
};

#endif // DKSMATRICES_H_INCLUDED
