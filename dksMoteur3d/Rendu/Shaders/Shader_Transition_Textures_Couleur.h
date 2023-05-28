#ifndef SHADER_TRANSITION_TEXTURES_COULEUR_H_INCLUDED
#define SHADER_TRANSITION_TEXTURES_COULEUR_H_INCLUDED

#include "dksShader.h"

class Shader_Transition_Textures_Couleur: public dksShader
{
    public:
        GLint drapeaux_sources;
        GLint texture_depart;
        GLint texture_destination;
        GLint texture_transition;
        GLint niveau;

    //======================================
    //  Constructeur
    //======================================
    public:
        Shader_Transition_Textures_Couleur(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":
                texture_depart=glGetUniformLocation(programme_id,"texture_depart");
                texture_destination=glGetUniformLocation(programme_id,"texture_destination");
                texture_transition=glGetUniformLocation(programme_id,"texture_transition");
                niveau=glGetUniformLocation(programme_id,"niveau_transition");


                if (erreur_openGl("ERREUR dans Shader_Transition_Textures_Couleur::Shader_Transition_Textures_Couleur() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
        ~Shader_Transition_Textures_Couleur()
        {

        }
};

#endif // SHADER_TRANSITION_TEXTURES_COULEUR_H_INCLUDED
