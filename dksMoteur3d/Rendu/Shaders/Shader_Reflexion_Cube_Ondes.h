#ifndef SHADER_REFLEXION_CUBE_ONDES_H_INCLUDED
#define SHADER_REFLEXION_CUBE_ONDES_H_INCLUDED

#include "dksShader.h"

class Shader_Reflexion_Cube_Ondes: public dksShader
{
    public:
        GLint drapeaux_sources;
        GLint matrice_volume;
        GLint texture_cube;
        GLint position_oeil;
        GLint niveau_reflexion;

        GLint intensite;
        GLint frequence;
        GLint echelle_texture_bruit;
        GLint niveau_perturbations;
        GLint texture_bruit;
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
    public:
        Shader_Reflexion_Cube_Ondes(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":
                matrice_volume=glGetUniformLocation(programme_id,"matrice_volume");
                texture_cube=glGetUniformLocation(programme_id,"cube_arriere_plan");
                position_oeil=glGetUniformLocation(programme_id,"position_oeil");
                drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");
                niveau_reflexion=glGetUniformLocation(programme_id,"niveau_reflexion");

                intensite=glGetUniformLocation(programme_id,"intensite_ondes");
                frequence=glGetUniformLocation(programme_id,"frequence_ondes");
                echelle_texture_bruit=glGetUniformLocation(programme_id,"echelle_texture_bruit_ondes");
                niveau_perturbations=glGetUniformLocation(programme_id,"niveau_perturbations_ondes");
                texture_bruit=glGetUniformLocation(programme_id,"texture_bruit");
                vitesse=glGetUniformLocation(programme_id,"vitesse_ondes");
                quantite=glGetUniformLocation(programme_id,"quantite_ondes");
                amplitude=glGetUniformLocation(programme_id,"amplitude_ondes");
                position_temps=glGetUniformLocation(programme_id,"position_temps");
                centre_gravite_face=glGetUniformLocation(programme_id,"centre_gravite_face");
                tangente=glGetUniformLocation(programme_id,"tangente");
                binormale=glGetUniformLocation(programme_id,"binormale");


                if (erreur_openGl("ERREUR dans Shader_Reflexion_Cube_Ondes::Shader_Reflexion_Cube_Ondes() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
        ~Shader_Reflexion_Cube_Ondes()
        {

        }
};

#endif // SHADER_REFLEXION_CUBE_ONDES_H_INCLUDED
