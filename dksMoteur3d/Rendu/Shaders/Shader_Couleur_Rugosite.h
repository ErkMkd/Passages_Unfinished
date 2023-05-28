#ifndef SHADER_COULEUR_RUGOSITE_H_INCLUDED
#define SHADER_COULEUR_RUGOSITE_H_INCLUDED

#include "dksShader.h"


class Shader_Couleur_Rugosite: public dksShader
{
  public:
        GLint texture_couleur;
        GLint texture_rugosite;
        GLint tangente;
        GLint binormale;

    //======================================
    //  Constructeur
    //======================================
        Shader_Couleur_Rugosite(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            //--------- Cr�ation des liens avec les variables type "uniform":
            if(erreur==DKSSHADER_OK)
            {
                //--------- Cr�ation des liens avec les variables type "uniform":

                texture_couleur=glGetUniformLocation(programme_id,"texture_couleur");
                texture_rugosite=glGetUniformLocation(programme_id,"texture_rugosite");

                //--------- Cr�ation des liens avec les variables type "attribute":

                tangente=glGetAttribLocation(programme_id,"tangente");
                binormale=glGetAttribLocation(programme_id,"binormale");

                if (erreur_openGl("ERREUR dans Shader_Couleur_Rugosite::Shader_Couleur_Rugosite() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Couleur_Rugosite()
    {

    }

};

#endif // SHADER_COULEUR_RUGOSITE_H_INCLUDED
