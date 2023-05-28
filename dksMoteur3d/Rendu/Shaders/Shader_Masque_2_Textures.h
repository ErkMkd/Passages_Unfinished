#ifndef SHADER_MASQUE_2_TEXTURES_H_INCLUDED
#define SHADER_MASQUE_2_TEXTURES_H_INCLUDED

#include "dksShader.h"

class Shader_Masque_2_Textures: public dksShader
{
    public:
        GLint texture_1;
        GLint texture_2;
        GLint couleur_masque;

    //======================================
    //  Constructeur
    //======================================
    public:
        Shader_Masque_2_Textures(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
        {
            if(erreur==DKSSHADER_OK)
            {
                //--------- Création des liens avec les variables type "uniform":
                texture_1=glGetUniformLocation(programme_id,"texture_1");
                texture_2=glGetUniformLocation(programme_id,"texture_2");
                couleur_masque=glGetUniformLocation(programme_id,"couleur_masque");


                if (erreur_openGl("ERREUR dans Shader_Masque_2_Textures::Shader_Masque_2_Textures() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            }
        }

    //=========================================
    //      Destructeur
    //=========================================
        ~Shader_Masque_2_Textures()
        {

        }
};

#endif // SHADER_MASQUE_2_TEXTURES_H_INCLUDED
