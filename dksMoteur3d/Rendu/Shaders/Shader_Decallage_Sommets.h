#ifndef SHADER_DECALLAGE_SOMMETS_H_INCLUDED
#define SHADER_DECALLAGE_SOMMETS_H_INCLUDED

#include "dksShader.h"

class Shader_Decallage_Sommets: public dksShader
{
  public:
    GLint drapeaux_sources;
    GLint deplacement;
    GLint niveau_eclairage;
    GLint tangente_face;
    GLint normale_face;
    GLint binormale_face;
    GLint centre_gravite_face;


    //======================================
    //  Constructeur
    //======================================
    Shader_Decallage_Sommets(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        if(erreur==DKSSHADER_OK)
        {
            //--------- Création des liens avec les variables type "uniform":

            deplacement=glGetUniformLocation(programme_id,"deplacement");
            niveau_eclairage=glGetUniformLocation(programme_id,"niveau_eclairage");

            //--------- Création des liens avec les variables type "attribute":

            tangente_face=glGetAttribLocation(programme_id,"tangente_face");
            normale_face=glGetAttribLocation(programme_id,"normale_face");
            binormale_face=glGetAttribLocation(programme_id,"binormale_face");
            centre_gravite_face=glGetAttribLocation(programme_id,"centre_gravite_face");

            if (erreur_openGl("ERREUR dans Shader_Decallage_Sommets::Shader_Decallage_Sommets() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Decallage_Sommets()
    {

    }

};


#endif // SHADER_DECALLAGE_SOMMETS_H_INCLUDED
