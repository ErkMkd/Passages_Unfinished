#ifndef SHADER_APLAT_BRUT_H_INCLUDED
#define SHADER_APLAT_BRUT_H_INCLUDED

#include "dksShader.h"

/*
    Un tel shader permet le rendu des surfaces en aplats sans �clairages,
    sans avoir � toucher � l'activation/d�sactivation de l'�clairage, qui cause des
    petits probl�mes d'inits avec les listes OpenGl.
*/

class Shader_Aplat_Brut: public dksShader
{
  public:

    //======================================
    //  Constructeur
    //======================================
    Shader_Aplat_Brut(char* p_nom,char* p_source_vertex,char* p_source_fragment):dksShader(p_nom,p_source_vertex,p_source_fragment)
    {
        //--------- Cr�ation des liens avec les variables type "uniform":
        if(erreur!=DKSSHADER_OK)
        {
            //if (erreur_openGl("ERREUR dans Shader_Aplat_Brut::Shader_Aplat_Brut() :"))erreur=DKSSHADER_ERREUR_VARIABLES;
            cout<<"!!! ERREUR lors de la cr�ation du shader Aplat_Brut"<<endl;
        }
    }

    //=========================================
    //      Destructeur
    //=========================================
    ~Shader_Aplat_Brut()
    {

    }

};


#endif // SHADER_APLAT_BRUT_H_INCLUDED
