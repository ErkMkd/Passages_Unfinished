#ifndef SHADER_ONDES_TEXTURE_COULEUR_H_INCLUDED
#define SHADER_ONDES_TEXTURE_COULEUR_H_INCLUDED

#include "dksShader.h"

class Shader_Ondes_Texture_Couleur: public dksShader
{
    public:
        GLint drapeaux_sources;
        GLint intensite;
        GLint frequence;
        GLint echelle_texture_bruit;
        GLint niveau_perturbations;
        GLint texture_bruit;
        GLint texture_couleur;
        GLint vitesse;
        GLint quantite;
        GLint amplitude;
        GLint position_temps;
        GLint centre_gravite_face;
        GLint tangente;
        GLint binormale;

    //======================================
    //  Constructeur
    //======================================
        Shader_Ondes_Texture_Couleur(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":
                drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");
                intensite=glGetUniformLocation(programme_id,"intensite_ondes");
                frequence=glGetUniformLocation(programme_id,"frequence_ondes");
                echelle_texture_bruit=glGetUniformLocation(programme_id,"echelle_texture_bruit_ondes");
                niveau_perturbations=glGetUniformLocation(programme_id,"niveau_perturbations_ondes");
                texture_bruit=glGetUniformLocation(programme_id,"texture_bruit");
                texture_couleur=glGetUniformLocation(programme_id,"texture_couleur");
                vitesse=glGetUniformLocation(programme_id,"vitesse_ondes");
                quantite=glGetUniformLocation(programme_id,"quantite_ondes");
                amplitude=glGetUniformLocation(programme_id,"amplitude_ondes");
                position_temps=glGetUniformLocation(programme_id,"position_temps");
                //centre_gravite_face=glGetUniformLocation(programme_id,"centre_gravite_face");
                tangente=glGetUniformLocation(programme_id,"tangente");
                binormale=glGetUniformLocation(programme_id,"binormale");

                if (erreur_openGl("ERREUR dans Shader_Ondes_Texture_Couleur::Shader_Ondes_Texture_Couleur() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
        ~Shader_Ondes_Texture_Couleur()
        {

        }

};

#endif // SHADER_ONDES_TEXTURE_COULEUR_H_INCLUDED
