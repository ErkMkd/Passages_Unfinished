#ifndef DKSSHADERS_H_INCLUDED
#define DKSSHADERS_H_INCLUDED

#include "../../Utiles/Gestion_Shaders.h"
#include "../../Utiles/Bruit.h"

/*
//*******************************************************************

                    Gestion des shaders au sein du moteur 3d

//*******************************************************************
*/
//------------- Shaders:
#include "shaders/Shader_Ondes.h"
#include "shaders/Shader_Ondes_Texture_Couleur.h"
#include "shaders/Shader_Couleur_Rugosite.h"
#include "shaders/Shader_Decallage_Sommets.h"
#include "shaders/Shader_Eclairage_Base.h"
#include "shaders/Shader_Floutage.h"
#include "shaders/Shader_Ombres.h"
#include "shaders/Shader_Reflexion_Cube.h"
#include "shaders/Shader_Reflexion_Cube_Ondes.h"
#include "shaders/Shader_Rugosite.h"
#include "shaders/Shader_Transition_Textures_Couleur.h"
#include "shaders/Shader_Transition_Textures_Couleur_Axe.h"
#include "shaders/Shader_Masque_2_Textures.h"
#include "shaders/Shader_Aplat_Brut.h"

//------------- Compatibilité:
#define DKSSHADERS_FABRIQUANT_ATI 1
#define DKSSHADERS_FABRIQUANT_NVIDIA 2

//------------ Codes d'erreur:

#define DKSSHADERS_DEBUG true

#define DKSSHADERS_OK 0
#define DKSSHADERS_ERREUR_SHADER 1
#define DKSSHADERS_ERREUR_FABRICANT 2
#define DKSSHADERS_ERREUR_TEXTURES 3


class dksInitialiseShaders: public Gestion_Shaders
{
    public:
        uint16_t erreur;
        uint8_t id_fabriquant;  //La marque de la carte graphique

        GLuint texture_bruit_id;    //La texture de bruit est utile pour certains shaders

        //Les shaders:
        Shader_Aplat_Brut* aplat_brut;
        Shader_Eclairage_Base* eclairage_base;
        Shader_Eclairage_Base* eclairage_phong;
        Shader_Rugosite* rugosite;
        Shader_Couleur_Rugosite* couleur_rugosite;
        Shader_Reflexion_Cube* reflexion_cube;
        Shader_Ombres* ombres;
        Shader_Ombres* phong_ombres;
        Shader_Ondes* ondes;
        Shader_Ondes_Texture_Couleur* ondes_texture_couleur;
        Shader_Reflexion_Cube_Ondes* reflexion_cube_ondes;
        Shader_Transition_Textures_Couleur* transition_textures_couleur;
        Shader_Transition_Textures_Couleur_Axe* transition_textures_couleur_axe;

        Shader_Floutage* floutage_x;
        Shader_Floutage* floutage_y;
        Shader_Masque_2_Textures* masque_2_textures;
        Shader_Decallage_Sommets* decallage_sommets;


    private:
        char chemin_sources[256];
        char nom_fichier_a[256];
        char nom_fichier_b[256];
        char *nom_fichier;

        #define DKSSHADERS_TAILLE_TEXTURE_BRUIT 256
        uint8_t texture_bruit[DKSSHADERS_TAILLE_TEXTURE_BRUIT*DKSSHADERS_TAILLE_TEXTURE_BRUIT];


    //=================================================================
    //                          Constructeur
    //=================================================================

    public:
        dksInitialiseShaders(uint8_t p_id_fabriquant)
        {
            erreur=DKSSHADERS_OK;

            aplat_brut=NULL;
            eclairage_base=NULL;
            eclairage_phong=NULL;
            rugosite=NULL;
            couleur_rugosite=NULL;
            reflexion_cube=NULL;
            ombres=NULL;
            phong_ombres=NULL;
            ondes=NULL;
            ondes_texture_couleur=NULL;
            reflexion_cube_ondes=NULL;
            transition_textures_couleur=NULL;
            transition_textures_couleur_axe=NULL;

            floutage_x=NULL;
            floutage_y=NULL;
            decallage_sommets=NULL;
            masque_2_textures=NULL;

            //Gestion carte graphique:
            id_fabriquant=p_id_fabriquant;
            if (id_fabriquant==DKSSHADERS_FABRIQUANT_ATI) strcpy(chemin_sources,"sources_shaders/ATI/");
            else if (id_fabriquant==DKSSHADERS_FABRIQUANT_NVIDIA) strcpy(chemin_sources,"sources_shaders/NVIDIA/");
            else
            {
                erreur=DKSSHADERS_ERREUR_FABRICANT;
                cout<<"ERREUR dans dksInitialiseShaders::dksInitialiseShaders() - Carte graphique inconnue!"<<endl;
                return;
            }

            //Création des shaders:
            if(!creation_shaders())
            {
                erreur=DKSSHADERS_ERREUR_SHADER;
                return;
            }

            //Création des textures utilisées par les shaders:
            if(!creation_textures()) return;
        }
    //=================================================================
    //                          Destructeur
    //=================================================================
        ~dksInitialiseShaders()
        {
            if(aplat_brut!=NULL) delete aplat_brut;
            if(eclairage_base!=NULL) delete eclairage_base;
            if(eclairage_phong!=NULL) delete eclairage_phong;
            if(rugosite!=NULL) delete rugosite;
            if(couleur_rugosite!=NULL) delete couleur_rugosite;
            if(reflexion_cube!=NULL) delete reflexion_cube;
            if(ombres!=NULL) delete ombres;
            if(phong_ombres!=NULL) delete phong_ombres;
            if(ondes!=NULL) delete ondes;
            if(ondes_texture_couleur!=NULL) delete ondes_texture_couleur;
            if(reflexion_cube_ondes!=NULL) delete reflexion_cube_ondes;
            if(transition_textures_couleur!=NULL) delete transition_textures_couleur;
            if(transition_textures_couleur_axe!=NULL) delete transition_textures_couleur_axe;

            if(floutage_x!=NULL) delete floutage_x;
            if(floutage_y!=NULL) delete floutage_y;
            if(decallage_sommets!=NULL) delete decallage_sommets;
            if(masque_2_textures!=NULL) delete masque_2_textures;
        }

    //========================================================================
    //          Création des shaders:
    //========================================================================
    private:
        bool creation_shaders()
        {
            aplat_brut=new Shader_Aplat_Brut("aplat_brut",renvoie_nom_fichier("aplat_brut_vertex.txt"),renvoie_nom_fichier("aplat_brut_fragment.txt"));
            if(aplat_brut->erreur!=DKSSHADER_OK) return false;

            eclairage_base=new Shader_Eclairage_Base("eclairage_base",renvoie_nom_fichier("eclairage_base_vertex.txt"),renvoie_nom_fichier("eclairage_base_fragment.txt"));
            if(eclairage_base->erreur!=DKSSHADER_OK) return false;

            eclairage_phong=new Shader_Eclairage_Base("eclairage_phong",renvoie_nom_fichier("eclairage_phong_vertex.txt"),renvoie_nom_fichier("eclairage_phong_fragment.txt"));
            if(eclairage_phong->erreur!=DKSSHADER_OK) return false;

            rugosite=new Shader_Rugosite("rugosité",renvoie_nom_fichier("rugosite_vertex.txt"),renvoie_nom_fichier("rugosite_fragment.txt"));
            if(rugosite->erreur!=DKSSHADER_OK) return false;

            couleur_rugosite=new Shader_Couleur_Rugosite("rugosite_couleur",renvoie_nom_fichier("rugosite_couleur_vertex.txt"),renvoie_nom_fichier("rugosite_couleur_fragment.txt"));
            if(couleur_rugosite->erreur!=DKSSHADER_OK) return false;

            reflexion_cube=new Shader_Reflexion_Cube("reflexion_cube",renvoie_nom_fichier("reflexion_cube_vertex.txt"),renvoie_nom_fichier("reflexion_cube_fragment.txt"));
            if(reflexion_cube->erreur!=DKSSHADER_OK) return false;

            ombres=new Shader_Ombres("ombres",renvoie_nom_fichier("ombres_vertex.txt"),renvoie_nom_fichier("ombres_fragment.txt"));
            if(ombres->erreur!=DKSSHADER_OK) return false;

            phong_ombres=new Shader_Ombres("phong_ombres",renvoie_nom_fichier("phong_ombres_vertex.txt"),renvoie_nom_fichier("phong_ombres_fragment.txt"));
            if(phong_ombres->erreur!=DKSSHADER_OK) return false;

            ondes=new Shader_Ondes("ondes",renvoie_nom_fichier("ondes_vertex.txt"),renvoie_nom_fichier("ondes_fragment.txt"));
            if(ondes->erreur!=DKSSHADER_OK) return false;

            ondes_texture_couleur=new Shader_Ondes_Texture_Couleur("ondes_texture_couleur",renvoie_nom_fichier("ondes_texture_couleur_vertex.txt"),renvoie_nom_fichier("ondes_texture_couleur_fragment.txt"));
            if(ondes_texture_couleur->erreur!=DKSSHADER_OK) return false;

            reflexion_cube_ondes=new Shader_Reflexion_Cube_Ondes("reflexion_cube_ondes",renvoie_nom_fichier("reflexion_cube_ondes_vertex.txt"),renvoie_nom_fichier("reflexion_cube_ondes_fragment.txt"));
            if(reflexion_cube_ondes->erreur!=DKSSHADER_OK) return false;

            transition_textures_couleur=new Shader_Transition_Textures_Couleur("transition_textures_couleur",renvoie_nom_fichier("transition_textures_couleur_vertex.txt"),renvoie_nom_fichier("transition_textures_couleur_fragment.txt"));
            if(transition_textures_couleur->erreur!=DKSSHADER_OK) return false;

            transition_textures_couleur_axe=new Shader_Transition_Textures_Couleur_Axe("transition_textures_couleur_axe",renvoie_nom_fichier("transition_textures_couleur_axe_vertex.txt"),renvoie_nom_fichier("transition_textures_couleur_axe_fragment.txt"));
            if(transition_textures_couleur->erreur!=DKSSHADER_OK) return false;

            floutage_x=new Shader_Floutage("floutage_x",NULL,renvoie_nom_fichier("flou_x.txt"));
            if(floutage_x->erreur!=DKSSHADER_OK) return false;
            floutage_y=new Shader_Floutage("floutage_y",NULL,renvoie_nom_fichier("flou_y.txt"));
            if(floutage_y->erreur!=DKSSHADER_OK) return false;

            decallage_sommets=new Shader_Decallage_Sommets("decallage_sommets",renvoie_nom_fichier("decallage_sommets_vertex.txt"),renvoie_nom_fichier("decallage_sommets_fragment.txt"));
            if(decallage_sommets->erreur!=DKSSHADER_OK) return false;

            masque_2_textures=new Shader_Masque_2_Textures("masque_2_textures",renvoie_nom_fichier("masque_2_textures_vertex.txt"),renvoie_nom_fichier("masque_2_textures_fragment.txt"));
            if(masque_2_textures->erreur!=DKSSHADER_OK) return false;

            return true;
        }

    //=========================================================================
    //      Concaténation du chemin des sources avec les noms des fichiers
    //=========================================================================
        char* renvoie_nom_fichier(char* nom_source)
        {
            if(nom_fichier==nom_fichier_a)nom_fichier=nom_fichier_b;
            else if(nom_fichier==nom_fichier_b)nom_fichier=nom_fichier_a;
            else nom_fichier=nom_fichier_a;
            for(uint16_t i=0;i<256;i++)nom_fichier[i]=0;
            strcpy(nom_fichier,chemin_sources);
            strcat(nom_fichier,nom_source);
            return nom_fichier;
        }
    //================================================================================
    //          Création des textures:
    //================================================================================
        bool creation_textures()
        {
            //---------- Génère la texture de bruit:
            if (!Bruit::genere_bruit_de_Perlin_2D_8bits(4,4,DKSSHADERS_TAILLE_TEXTURE_BRUIT,DKSSHADERS_TAILLE_TEXTURE_BRUIT,12,0.75,texture_bruit))
            {
                cout<<"ERREUR dans dksShaders::creation_textures() - Impossible de créer la texture de bruit"<<endl;
                erreur=DKSSHADERS_ERREUR_TEXTURES;
                return false;
            }

            glGenTextures(1,&texture_bruit_id);
            glBindTexture(GL_TEXTURE_2D,texture_bruit_id);

            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    //Il semble que ce paramètre active la texture.....
            glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,DKSSHADERS_TAILLE_TEXTURE_BRUIT,DKSSHADERS_TAILLE_TEXTURE_BRUIT,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,texture_bruit);

            //glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);


            //--------- Erreur OpenGl:
            if (erreur_openGl("ERREUR dans dksShaders::creation_textures() :"))
            {
                erreur=DKSSHADERS_ERREUR_TEXTURES;
                return false;
            }
            else return true;
        }
};

class dksShaders
{
    public: static dksInitialiseShaders* bibliotheque;


};

#endif // DKSSHADERS_H_INCLUDED
