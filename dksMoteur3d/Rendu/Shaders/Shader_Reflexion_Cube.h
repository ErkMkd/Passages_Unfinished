#ifndef SHADER_REFLEXION_CUBE_H_INCLUDED
#define SHADER_REFLEXION_CUBE_H_INCLUDED

#include "dksShader.h"

class Shader_Reflexion_Cube: public dksShader
{
  public:
    GLint drapeaux_sources;
    GLint matrice_volume;
    GLint texture;
    GLint position_oeil;
    GLint niveau;

    //======================================
    //  Constructeur
    //======================================
    Shader_Reflexion_Cube(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        if(erreur==DKSSHADER_OK)
        {
            //--------- Création des liens avec les variables type "uniform":

            matrice_volume=glGetUniformLocation(programme_id,"matrice_volume");
            texture=glGetUniformLocation(programme_id,"cube_arriere_plan");
            position_oeil=glGetUniformLocation(programme_id,"position_oeil");
            drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");
            niveau=glGetUniformLocation(programme_id,"niveau_reflexion");

            if (erreur_openGl("ERREUR dans Shader_Reflexion_Cube::Shader_Reflexion_Cube() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Reflexion_Cube()
    {

    }

};

#endif // SHADER_REFLEXION_CUBE_H_INCLUDED
