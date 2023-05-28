#ifndef SHADER_OMBRES_H_INCLUDED
#define SHADER_OMBRES_H_INCLUDED

#include "dksShader.h"

/*
    Sert aussi pour les ombres avec éclairage en Phong
*/

class Shader_Ombres: public dksShader
{
  public:
    GLint drapeaux_sources;
    GLint texture_profondeur;
    GLint matrice_source_uv;
    GLint matrice_projection_vision_source;
    GLint matrice_vision_source;
    GLint composantes_convertion;
    GLint seuil;
    GLint intensite;

    //======================================
    //  Constructeur
    //======================================
    Shader_Ombres(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        //--------- Création des liens avec les variables type "uniform":
        if(erreur==DKSSHADER_OK)
        {
            //--------- Création des liens avec les variables type "uniform":

            drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");
            texture_profondeur=glGetUniformLocation(programme_id,"texture_profondeur");
            matrice_source_uv=glGetUniformLocation(programme_id,"matrice_source_uv");
            matrice_projection_vision_source=glGetUniformLocation(programme_id,"matrice_projection_vision_source");
            matrice_vision_source=glGetUniformLocation(programme_id,"matrice_vision_source");
            composantes_convertion=glGetUniformLocation(programme_id,"composantes_convertion");
            seuil=glGetUniformLocation(programme_id,"seuil_ombres");
            intensite=glGetUniformLocation(programme_id,"intensite_ombres");

            if (erreur_openGl("ERREUR dans Shader_Ombres::Shader_Ombres() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Ombres()
    {

    }

};

#endif // SHADER_OMBRES_H_INCLUDED
