#ifndef DKSETAT_H_INCLUDED
#define DKSETAT_H_INCLUDED

#include <map>
#include <string.h>
#include <algorithm>

//*************************************************************************
//          État du pipeline fixe d'OpenGL
//
//  Le profil Core n'a plus de pipeline fixe (éclairage par sommet, modes de
//  texture, brouillard, sprites...). Ces fonctions reprennent l'interface de
//  glEnable, glBindTexture, glTexEnv, glFog, glUseProgram... et tiennent cet
//  état en C++, pour qu'un shader d'émulation le reproduise quand le moteur
//  trace sans shader.
//  Pendant le portage, chaque appel est aussi transmis à OpenGL.
//*************************************************************************

#define DKSETAT_NBR_UNITES 8

class dksEtat
{
    public:
        inline static GLuint programme=0;      //Programme demandé par le moteur (0: pipeline fixe)

        //------- Capacités du pipeline fixe:
        inline static bool eclairage=false;
        inline static bool brouillard=false;
        inline static bool couleur_materiau=false;
        inline static bool normalisation=false;
        inline static bool points_lisses=false;
        inline static bool lignes_lisses=false;
        inline static bool polygones_lisses=false;
        inline static bool sprites=false;
        inline static unsigned int sources=0;      //Sources activées (bit n: GL_LIGHTn)
        inline static unsigned int textures=0;     //Unités où GL_TEXTURE_2D est activé (bit n: unité n)

        //------- Unités de texture:
        inline static GLuint unite=0;              //Unité active
        inline static GLuint textures_liees[DKSETAT_NBR_UNITES]={};
        inline static GLint modes_textures[DKSETAT_NBR_UNITES]={GL_MODULATE,GL_MODULATE,GL_MODULATE,GL_MODULATE,
                                                                 GL_MODULATE,GL_MODULATE,GL_MODULATE,GL_MODULATE};
        inline static bool remplacement_coordonnees[DKSETAT_NBR_UNITES]={};  //GL_COORD_REPLACE des sprites
        inline static std::map<GLuint,GLint> formats_textures;  //Format de base de chaque texture

        //------- Brouillard:
        inline static GLint mode_brouillard=GL_EXP;
        inline static GLfloat densite_brouillard=1.f;
        inline static GLfloat debut_brouillard=0.f;
        inline static GLfloat fin_brouillard=1.f;
        inline static GLfloat couleur_brouillard[4]={0.f,0.f,0.f,0.f};

        //------- Ombrage, couleur des matériaux, points:
        inline static GLenum modele_ombrage=GL_SMOOTH;
        inline static GLenum faces_couleur_materiau=GL_FRONT_AND_BACK;
        inline static GLenum composante_couleur_materiau=GL_AMBIENT_AND_DIFFUSE;
        inline static GLfloat taille_points=1.f;
        inline static GLfloat taille_points_min=0.f;
        inline static GLfloat taille_points_max=1e6f;
        inline static GLfloat attenuation_points[3]={1.f,0.f,0.f};

    private:
        static bool* capacite_fixe(GLenum capacite)
        {
            switch (capacite)
            {
                case GL_LIGHTING: return &eclairage;
                case GL_FOG: return &brouillard;
                case GL_COLOR_MATERIAL: return &couleur_materiau;
                case GL_NORMALIZE: return &normalisation;
                case GL_POINT_SMOOTH: return &points_lisses;
                case GL_LINE_SMOOTH: return &lignes_lisses;
                case GL_POLYGON_SMOOTH: return &polygones_lisses;
                case GL_POINT_SPRITE: return &sprites;
                default: return NULL;
            }
        }

        static void change(GLenum capacite,bool valeur)
        {
            bool* c=capacite_fixe(capacite);
            if (c!=NULL) *c=valeur;
            else if (capacite>=GL_LIGHT0 && capacite<GL_LIGHT0+8)
            {
                unsigned int bit=1u<<(capacite-GL_LIGHT0);
                sources= valeur ? (sources|bit) : (sources&~bit);
            }
            else if (capacite==GL_TEXTURE_2D && unite<DKSETAT_NBR_UNITES)
            {
                unsigned int bit=1u<<unite;
                textures= valeur ? (textures|bit) : (textures&~bit);
            }
            if (valeur) glEnable(capacite);
            else glDisable(capacite);
        }

        //Format de base d'un format interne de texture:
        static GLint format_de_base(GLint format)
        {
            switch (format)
            {
                case 1: case GL_LUMINANCE: case GL_LUMINANCE4: case GL_LUMINANCE8: case GL_LUMINANCE16:
                    return GL_LUMINANCE;
                case 2: case GL_LUMINANCE_ALPHA: case GL_LUMINANCE4_ALPHA4: case GL_LUMINANCE8_ALPHA8: case GL_LUMINANCE16_ALPHA16:
                    return GL_LUMINANCE_ALPHA;
                case GL_ALPHA: case GL_ALPHA4: case GL_ALPHA8: case GL_ALPHA16:
                    return GL_ALPHA;
                case GL_INTENSITY: case GL_INTENSITY4: case GL_INTENSITY8: case GL_INTENSITY16:
                    return GL_INTENSITY;
                case 3: case GL_RGB: case GL_RGB4: case GL_RGB5: case GL_RGB8: case GL_RGB16:
                    return GL_RGB;
                case GL_DEPTH_COMPONENT: case GL_DEPTH_COMPONENT16: case GL_DEPTH_COMPONENT24: case GL_DEPTH_COMPONENT32:
                    return GL_DEPTH_COMPONENT;
                default:
                    return GL_RGBA;
            }
        }

    public:
        //-------------------------------------------------------------
        //      Équivalents de glEnable, glDisable et glIsEnabled
        //-------------------------------------------------------------
        static void active(GLenum capacite) { change(capacite,true); }
        static void desactive(GLenum capacite) { change(capacite,false); }

        static GLboolean est_active(GLenum capacite)
        {
            bool* c=capacite_fixe(capacite);
            if (c!=NULL) return *c;
            if (capacite>=GL_LIGHT0 && capacite<GL_LIGHT0+8) return (sources>>(capacite-GL_LIGHT0))&1;
            if (capacite==GL_TEXTURE_2D && unite<DKSETAT_NBR_UNITES) return (textures>>unite)&1;
            return glIsEnabled(capacite);
        }

        //-------------------------------------------------------------
        //      Équivalent de glUseProgram
        //-------------------------------------------------------------
        static void utilise_programme(GLuint p)
        {
            programme=p;
            glUseProgram(p);
        }

        //-------------------------------------------------------------
        //      Textures: glActiveTexture, glBindTexture, glTexImage2D, glTexEnv
        //-------------------------------------------------------------
        static void unite_active(GLenum unite_texture)
        {
            unite=unite_texture-GL_TEXTURE0;
            glActiveTexture(unite_texture);
        }

        static void lie_texture(GLenum cible,GLuint texture)
        {
            if (cible==GL_TEXTURE_2D && unite<DKSETAT_NBR_UNITES) textures_liees[unite]=texture;
            glBindTexture(cible,texture);
        }

        static void image_texture(GLenum cible,GLint niveau,GLint format_interne,GLsizei largeur,GLsizei hauteur,
                                  GLint bord,GLenum format,GLenum type,const void* pixels)
        {
            if (cible==GL_TEXTURE_2D && niveau==0 && unite<DKSETAT_NBR_UNITES)
                formats_textures[textures_liees[unite]]=format_de_base(format_interne);
            glTexImage2D(cible,niveau,format_interne,largeur,hauteur,bord,format,type,pixels);
        }

        //Format de base de la texture liée à une unité:
        static GLint format_texture(GLuint unite_texture)
        {
            std::map<GLuint,GLint>::iterator f=formats_textures.find(textures_liees[unite_texture]);
            return f!=formats_textures.end() ? f->second : GL_RGBA;
        }

        static void enveloppe_i(GLenum cible,GLenum parametre,GLint valeur)
        {
            if (unite<DKSETAT_NBR_UNITES)
            {
                if (cible==GL_TEXTURE_ENV && parametre==GL_TEXTURE_ENV_MODE) modes_textures[unite]=valeur;
                else if (cible==GL_POINT_SPRITE && parametre==GL_COORD_REPLACE) remplacement_coordonnees[unite]= valeur!=0;
            }
            glTexEnvi(cible,parametre,valeur);
        }

        static void enveloppe_f(GLenum cible,GLenum parametre,GLfloat valeur)
        {
            enveloppe_i(cible,parametre,GLint(valeur));
        }

        //-------------------------------------------------------------
        //      Brouillard: glFogf, glFogi, glFogfv
        //-------------------------------------------------------------
        static void brouillard_f(GLenum parametre,GLfloat valeur)
        {
            switch (parametre)
            {
                case GL_FOG_MODE: mode_brouillard=GLint(valeur); break;
                case GL_FOG_DENSITY: densite_brouillard=valeur; break;
                case GL_FOG_START: debut_brouillard=valeur; break;
                case GL_FOG_END: fin_brouillard=valeur; break;
            }
            glFogf(parametre,valeur);
        }

        static void brouillard_i(GLenum parametre,GLint valeur)
        {
            if (parametre==GL_FOG_MODE) mode_brouillard=valeur;
            glFogi(parametre,valeur);
        }

        static void brouillard_fv(GLenum parametre,const GLfloat* valeurs)
        {
            if (parametre==GL_FOG_COLOR) memcpy(couleur_brouillard,valeurs,4*sizeof(GLfloat));
            else brouillard_f(parametre,valeurs[0]);
            glFogfv(parametre,valeurs);
        }

        //-------------------------------------------------------------
        //      glShadeModel et glColorMaterial
        //-------------------------------------------------------------
        static void ombrage(GLenum mode)
        {
            modele_ombrage=mode;
            glShadeModel(mode);
        }

        static void couleur_materiau_mode(GLenum faces,GLenum composante)
        {
            faces_couleur_materiau=faces;
            composante_couleur_materiau=composante;
            glColorMaterial(faces,composante);
        }

        //-------------------------------------------------------------
        //      Points: glPointSize et glPointParameter
        //-------------------------------------------------------------
        static void taille_point(GLfloat taille)
        {
            taille_points=taille;
            glPointSize(taille);
        }

        static void parametre_points_f(GLenum parametre,GLfloat valeur)
        {
            if (parametre==GL_POINT_SIZE_MIN) taille_points_min=valeur;
            else if (parametre==GL_POINT_SIZE_MAX) taille_points_max=valeur;
            glPointParameterf(parametre,valeur);
        }

        static void parametre_points_i(GLenum parametre,GLint valeur)
        {
            glPointParameteri(parametre,valeur);
        }

        static void parametre_points_fv(GLenum parametre,const GLfloat* valeurs)
        {
            if (parametre==GL_POINT_DISTANCE_ATTENUATION) memcpy(attenuation_points,valeurs,3*sizeof(GLfloat));
            else if (parametre==GL_POINT_SIZE_MIN || parametre==GL_POINT_SIZE_MAX) parametre_points_f(parametre,valeurs[0]);
            glPointParameterfv(parametre,valeurs);
        }

        //=============================================================
        //      Émulation du pipeline fixe
        //  Quand le moteur trace sans shader, le programme pipeline_fixe est utilisé:
        //  il lit l'état ci-dessus dans le bloc d'uniforms "PipelineFixe" (binding 3).
        //=============================================================
        inline static GLuint programme_emulation=0;     //Programme pipeline_fixe, créé avec les shaders du moteur

    private:
        //Disposition std140, identique au bloc des shaders pipeline_fixe:
        struct Bloc_PipelineFixe
        {
            GLint eclairage;
            GLint sources_actives;
            GLint couleur_materiau;
            GLint deux_faces;
            GLint normalisation;
            GLint ombrage_plat;
            GLint textures_actives;
            GLint speculaire_separee;
            GLint observateur_local;
            GLint brouillard;
            GLfloat densite_brouillard;
            GLfloat debut_brouillard;
            GLfloat fin_brouillard;
            GLint primitive_points;
            GLint points_lisses;
            GLint sprites;
            GLfloat couleur_brouillard[4];
            GLint modes_textures[4];
            GLint formats_textures[4];
            GLfloat taille_points;
            GLfloat taille_points_min;
            GLfloat taille_points_max;
            GLint faces_couleur_materiau;
            GLfloat attenuation_points[4];
        };
        inline static GLuint tampon_pipeline=0;

    public:
        //Avant un tracé: utilise le programme d'émulation si le moteur n'a pas de shader.
        //Renvoie true s'il est utilisé.
        static bool prepare_trace(GLenum mode)
        {
            if (programme!=0 || programme_emulation==0) return false;

            Bloc_PipelineFixe b;
            b.eclairage=eclairage;
            b.sources_actives=sources;
            b.couleur_materiau= couleur_materiau ? GLint(composante_couleur_materiau) : 0;
            b.deux_faces=dksEclairage::deux_faces;
            b.normalisation=normalisation;
            b.ombrage_plat= modele_ombrage==GL_FLAT;
            b.textures_actives=0;
            b.sprites=0;
            for (int u=0;u<4;u++)
            {
                //Une unité sans texture est désactivée, comme dans le pipeline fixe (texture incomplète).
                if ((textures&(1u<<u)) && textures_liees[u]!=0) b.textures_actives|=1<<u;
                if (remplacement_coordonnees[u]) b.sprites|=1<<u;
                b.modes_textures[u]=modes_textures[u];
                b.formats_textures[u]=format_texture(u);
            }
            if (!sprites) b.sprites=0;
            b.speculaire_separee=dksEclairage::speculaire_separee;
            b.observateur_local=dksEclairage::observateur_local;
            b.brouillard= brouillard ? mode_brouillard : 0;
            b.densite_brouillard=densite_brouillard;
            b.debut_brouillard=debut_brouillard;
            b.fin_brouillard=fin_brouillard;
            b.primitive_points= mode==GL_POINTS;
            b.points_lisses=points_lisses;
            memcpy(b.couleur_brouillard,couleur_brouillard,4*sizeof(GLfloat));
            b.taille_points=taille_points;
            b.taille_points_min=taille_points_min;
            b.taille_points_max=taille_points_max;
            b.faces_couleur_materiau=GLint(faces_couleur_materiau);
            memcpy(b.attenuation_points,attenuation_points,3*sizeof(GLfloat));
            b.attenuation_points[3]=0.f;

            if (tampon_pipeline==0)
            {
                glCreateBuffers(1,&tampon_pipeline);
                glNamedBufferData(tampon_pipeline,sizeof(b),&b,GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_UNIFORM_BUFFER,3,tampon_pipeline);
            }
            else glNamedBufferSubData(tampon_pipeline,0,sizeof(b),&b);

            glUseProgram(programme_emulation);
            if (mode==GL_POINTS) glEnable(GL_PROGRAM_POINT_SIZE);
            return true;
        }

        //Après le tracé: le moteur retrouve le programme qu'il a demandé (0).
        static void termine_trace(GLenum mode,bool emulation)
        {
            if (!emulation) return;
            if (mode==GL_POINTS) glDisable(GL_PROGRAM_POINT_SIZE);
            glUseProgram(0);
        }
};

#endif // DKSETAT_H_INCLUDED
