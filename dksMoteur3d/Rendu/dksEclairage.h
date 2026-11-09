#ifndef DKSECLAIRAGE_H_INCLUDED
#define DKSECLAIRAGE_H_INCLUDED

#include <math.h>
#include <string.h>

//*************************************************************************
//          État de l'éclairage: sources lumineuses, modèle d'éclairage, matériau
//
//  Remplace l'état d'éclairage d'OpenGL (glLight, glLightModel, glMaterial),
//  absent du profil Core. Mêmes conventions qu'OpenGL: positions et directions
//  sont transformées par la matrice modèle-vue courante au moment où elles sont
//  données, et conservées dans le repère de l'oeil.
//
//  Les shaders lisent cet état dans deux blocs d'uniforms (disposition std140):
//      binding 1: Eclairage   (8 sources et couleur ambiante globale)
//      binding 2: Materiau    (matériau des faces avant, comme gl_FrontMaterial)
//  Pendant le portage, chaque changement est aussi transmis à OpenGL, pour le pipeline fixe.
//*************************************************************************

#define DKSECLAIRAGE_NBR_SOURCES 8
#define DKSECLAIRAGE_BLOC_ECLAIRAGE 1
#define DKSECLAIRAGE_BLOC_MATERIAU 2

class dksEclairage
{
    public:
        //------- Disposition std140, identique à celle des blocs des shaders:
        struct Source
        {
            GLfloat position[4];            //Repère de l'oeil
            GLfloat ambiante[4];
            GLfloat diffuse[4];
            GLfloat speculaire[4];
            GLfloat direction_spot[4];      //Repère de l'oeil
            GLfloat exposant_spot;
            GLfloat ouverture_spot;         //En degrés, 180: pas de spot
            GLfloat cos_ouverture_spot;
            GLfloat attenuation_constante;
            GLfloat attenuation_lineaire;
            GLfloat attenuation_quadratique;
            GLfloat remplissage[2];
        };

        struct Bloc_Eclairage
        {
            Source sources[DKSECLAIRAGE_NBR_SOURCES];
            GLfloat ambiante_globale[4];
        };

        struct Bloc_Materiau
        {
            GLfloat ambiante[4];
            GLfloat diffuse[4];
            GLfloat speculaire[4];
            GLfloat emission[4];
            GLfloat brillance;
            GLfloat remplissage[3];
        };

    private:
        static void copie(GLfloat* destination,GLfloat a,GLfloat b,GLfloat c,GLfloat d)
        {
            destination[0]=a; destination[1]=b; destination[2]=c; destination[3]=d;
        }

        //Valeurs initiales d'OpenGL:
        static Bloc_Eclairage eclairage_initial()
        {
            Bloc_Eclairage e;
            memset(&e,0,sizeof(e));
            for (int i=0;i<DKSECLAIRAGE_NBR_SOURCES;i++)
            {
                Source& s=e.sources[i];
                GLfloat composante= i==0 ? 1.f : 0.f;   //La source 0 est blanche, les autres noires
                copie(s.position,0.f,0.f,1.f,0.f);
                copie(s.ambiante,0.f,0.f,0.f,1.f);
                copie(s.diffuse,composante,composante,composante,1.f);
                copie(s.speculaire,composante,composante,composante,1.f);
                copie(s.direction_spot,0.f,0.f,-1.f,0.f);
                s.exposant_spot=0.f;
                s.ouverture_spot=180.f;
                s.cos_ouverture_spot=-1.f;
                s.attenuation_constante=1.f;
            }
            copie(e.ambiante_globale,.2f,.2f,.2f,1.f);
            return e;
        }

        static Bloc_Materiau materiau_initial()
        {
            Bloc_Materiau m;
            memset(&m,0,sizeof(m));
            copie(m.ambiante,.2f,.2f,.2f,1.f);
            copie(m.diffuse,.8f,.8f,.8f,1.f);
            copie(m.speculaire,0.f,0.f,0.f,1.f);
            copie(m.emission,0.f,0.f,0.f,1.f);
            return m;
        }

        inline static Bloc_Eclairage eclairage=eclairage_initial();
        inline static Bloc_Materiau materiau_avant=materiau_initial();
        inline static GLuint tampon_eclairage=0;
        inline static GLuint tampon_materiau=0;

        //Transmet un bloc aux shaders (le tampon est créé à la première utilisation):
        static void transmet(GLuint& tampon,GLuint indice_bloc,const void* donnees,GLsizeiptr taille)
        {
            if (tampon==0)
            {
                glCreateBuffers(1,&tampon);
                glNamedBufferData(tampon,taille,donnees,GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_UNIFORM_BUFFER,indice_bloc,tampon);
            }
            else glNamedBufferSubData(tampon,0,taille,donnees);
        }

    public:
        static const Bloc_Eclairage& etat_eclairage()
        {
            return eclairage;
        }

        static const Bloc_Materiau& etat_materiau()
        {
            return materiau_avant;
        }

        //-------------------------------------------------------------
        //      Équivalents de glLightfv et glLightf
        //-------------------------------------------------------------
        static void source(GLenum id_openGl,GLenum parametre,const GLfloat* valeurs)
        {
            Source& s=eclairage.sources[id_openGl-GL_LIGHT0];
            switch (parametre)
            {
                case GL_AMBIENT: memcpy(s.ambiante,valeurs,4*sizeof(GLfloat)); break;
                case GL_DIFFUSE: memcpy(s.diffuse,valeurs,4*sizeof(GLfloat)); break;
                case GL_SPECULAR: memcpy(s.speculaire,valeurs,4*sizeof(GLfloat)); break;
                case GL_POSITION:
                {
                    glm::dvec4 p=dksMatrices::modele_vue()*glm::dvec4(valeurs[0],valeurs[1],valeurs[2],valeurs[3]);
                    copie(s.position,GLfloat(p.x),GLfloat(p.y),GLfloat(p.z),GLfloat(p.w));
                    break;
                }
                case GL_SPOT_DIRECTION:
                {
                    glm::dvec3 d=glm::dmat3(dksMatrices::modele_vue())*glm::dvec3(valeurs[0],valeurs[1],valeurs[2]);
                    copie(s.direction_spot,GLfloat(d.x),GLfloat(d.y),GLfloat(d.z),0.f);
                    break;
                }
                case GL_SPOT_EXPONENT: s.exposant_spot=valeurs[0]; break;
                case GL_SPOT_CUTOFF:
                    s.ouverture_spot=valeurs[0];
                    s.cos_ouverture_spot=GLfloat(cos(double(valeurs[0])*M_PI/180.));
                    break;
                case GL_CONSTANT_ATTENUATION: s.attenuation_constante=valeurs[0]; break;
                case GL_LINEAR_ATTENUATION: s.attenuation_lineaire=valeurs[0]; break;
                case GL_QUADRATIC_ATTENUATION: s.attenuation_quadratique=valeurs[0]; break;
            }
            transmet(tampon_eclairage,DKSECLAIRAGE_BLOC_ECLAIRAGE,&eclairage,sizeof(eclairage));
            glLightfv(id_openGl,parametre,valeurs);
        }

        static void source(GLenum id_openGl,GLenum parametre,GLfloat valeur)
        {
            source(id_openGl,parametre,&valeur);
        }

        //-------------------------------------------------------------
        //      Équivalents de glLightModelfv et glLightModeli
        //  Seule la couleur ambiante globale concerne les shaders: les autres
        //  paramètres ne servent qu'au pipeline fixe.
        //-------------------------------------------------------------
        static void modele(GLenum parametre,const GLfloat* valeurs)
        {
            if (parametre==GL_LIGHT_MODEL_AMBIENT)
            {
                memcpy(eclairage.ambiante_globale,valeurs,4*sizeof(GLfloat));
                transmet(tampon_eclairage,DKSECLAIRAGE_BLOC_ECLAIRAGE,&eclairage,sizeof(eclairage));
            }
            glLightModelfv(parametre,valeurs);
        }

        static void modele(GLenum parametre,GLint valeur)
        {
            glLightModeli(parametre,valeur);
        }

        //-------------------------------------------------------------
        //      Équivalents de glMaterialfv et glMaterialf
        //  Les shaders ne lisent que le matériau des faces avant.
        //-------------------------------------------------------------
        static void materiau(GLenum face,GLenum parametre,const GLfloat* valeurs)
        {
            if (face!=GL_BACK)
            {
                switch (parametre)
                {
                    case GL_AMBIENT: memcpy(materiau_avant.ambiante,valeurs,4*sizeof(GLfloat)); break;
                    case GL_DIFFUSE: memcpy(materiau_avant.diffuse,valeurs,4*sizeof(GLfloat)); break;
                    case GL_AMBIENT_AND_DIFFUSE:
                        memcpy(materiau_avant.ambiante,valeurs,4*sizeof(GLfloat));
                        memcpy(materiau_avant.diffuse,valeurs,4*sizeof(GLfloat));
                        break;
                    case GL_SPECULAR: memcpy(materiau_avant.speculaire,valeurs,4*sizeof(GLfloat)); break;
                    case GL_EMISSION: memcpy(materiau_avant.emission,valeurs,4*sizeof(GLfloat)); break;
                    case GL_SHININESS: materiau_avant.brillance=valeurs[0]; break;
                }
                transmet(tampon_materiau,DKSECLAIRAGE_BLOC_MATERIAU,&materiau_avant,sizeof(materiau_avant));
            }
            glMaterialfv(face,parametre,valeurs);
        }

        static void materiau(GLenum face,GLenum parametre,GLfloat valeur)
        {
            materiau(face,parametre,&valeur);
        }
};

#endif // DKSECLAIRAGE_H_INCLUDED
