#ifndef SHADER_TRANSITION_TEXTURES_COULEUR_AXE_H_INCLUDED
#define SHADER_TRANSITION_TEXTURES_COULEUR_AXE_H_INCLUDED

#include "dksShader.h"

class Shader_Transition_Textures_Couleur_Axe: public dksShader
{
    public:
        GLint drapeaux_sources;
        GLint texture_depart;
        GLint texture_destination;
        GLint zmin;
        GLint zmax;
        GLint niveau;
        GLint largeur_fondu;
        GLint drapeaux_textures;

    //======================================
    //  Constructeur
    //======================================
    public:
        Shader_Transition_Textures_Couleur_Axe(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":
                drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");
                texture_depart=glGetUniformLocation(programme_id,"texture_depart");
                texture_destination=glGetUniformLocation(programme_id,"texture_destination");
                niveau=glGetUniformLocation(programme_id,"niveau_transition");
                largeur_fondu=glGetUniformLocation(programme_id,"largeur_fondu");
                zmin=glGetUniformLocation(programme_id,"zmin");
                zmax=glGetUniformLocation(programme_id,"zmax");
                drapeaux_textures=glGetUniformLocation(programme_id,"drapeaux_textures");


                if (erreur_openGl("ERREUR dans Shader_Transition_Textures_Couleur_Axe::Shader_Transition_Textures_Couleur_Axe() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
        ~Shader_Transition_Textures_Couleur_Axe()
        {

        }
};


#endif // SHADER_TRANSITION_TEXTURES_COULEUR_AXE_H_INCLUDED
