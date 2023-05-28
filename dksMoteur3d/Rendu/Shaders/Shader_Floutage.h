#ifndef SHADER_FLOUTAGE_H_INCLUDED
#define SHADER_FLOUTAGE_H_INCLUDED

#include "dksShader.h"

/*
    Sert aussi bien pour le flou horizontal que vertical (seules les sources changent, les paramètres sont les mêmes)
*/

class Shader_Floutage: public dksShader
{
  public:
        GLint texture;
        GLint taille;
        GLint saturation;

    //======================================
    //  Constructeur
    //======================================
    Shader_Floutage(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        //--------- Création des liens avec les variables type "uniform":
        if(erreur==DKSSHADER_OK)
        {
            //--------- Création des liens avec les variables type "uniform":

            texture=glGetUniformLocation(programme_id,"texture");
            taille=glGetUniformLocation(programme_id,"taille_flou");
            saturation=glGetUniformLocation(programme_id,"saturation_flou");

            if (erreur_openGl("ERREUR dans Shader_Floutage::Shader_Floutage() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Floutage()
    {

    }

};


#endif // SHADER_FLOUTAGE_H_INCLUDED
