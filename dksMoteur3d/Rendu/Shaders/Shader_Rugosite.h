#ifndef SHADER_RUGOSITE_H_INCLUDED
#define SHADER_RUGOSITE_H_INCLUDED

#include "dksShader.h"


class Shader_Rugosite: public dksShader
{
  public:
        GLint texture_rugosite;
        GLint tangente;
        GLint binormale;

    //======================================
    //  Constructeur
    //======================================
        Shader_Rugosite(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            //--------- Création des liens avec les variables type "uniform":
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":

                texture_rugosite=glGetUniformLocation(programme_id,"texture_rugosite");

                //--------- Création des liens avec les variables type "attribute":

                tangente=glGetAttribLocation(programme_id,"tangente");
                binormale=glGetAttribLocation(programme_id,"binormale");

                if (erreur_openGl("ERREUR dans Shader_Rugosite::Shader_Rugosite() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Rugosite()
    {

    }

};

#endif // SHADER_RUGOSITE_H_INCLUDED
