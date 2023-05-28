#ifndef SHADER_ECLAIRAGE_BASE_H_INCLUDED
#define SHADER_ECLAIRAGE_BASE_H_INCLUDED

#include "dksShader.h"


class Shader_Eclairage_Base: public dksShader
{
  public:
    GLint drapeaux_sources;

    //======================================
    //  Constructeur
    //======================================
    Shader_Eclairage_Base(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        //--------- Création des liens avec les variables type "uniform":
        if(erreur==DKSSHADER_OK)
        {
            drapeaux_sources=glGetUniformLocation(programme_id,"drapeaux_sources");

            if (erreur_openGl("ERREUR dans Shader_Eclairage_Base::Shader_Eclairage_Base() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Eclairage_Base()
    {

    }

};

#endif // SHADER_ECLAIRAGE_BASE_H_INCLUDED
