#ifndef DKSRENDU_H_INCLUDED
#define DKSRENDU_H_INCLUDED

//=====================================================================================================================
//          Classe de base pour les rendus openGl dans un tempon
//          Cette classe sert à standardiser les fonctions pour les traitements post-rendus
//=====================================================================================================================

//#include "../dksMoteur3d/Elements_Fondamentaux/dksOeil.h"
class dksOeil;

//====== Identifiants des modes de rendu:

#define DKSRENDU_COULEURS 1
#define DKSRENDU_COULEURS_LUMINESCENTES 2
#define DKSRENDU_PROFONDEUR 3

class dksRendu
{
    public:

    static double delta_t;  //Intervalle de temps commun à tout les objets d'affichage
    static double position_temps;  //compteur de temps général

    bool drapeau_affiche;   //False si l'objet doit être caché
    static GLint nbr_unites_textures_max;

    //====================================================
    //      Fonctions statiques
    //====================================================

    static void desactive_textures()
    {
        uint16_t i;
        //glMatrixMode(GL_TEXTURE);
        //glLoadIdentity();
        for (i=0;i<nbr_unites_textures_max;i++)
            {
                glActiveTexture(GL_TEXTURE0+i);
                //glMatrixMode(GL_TEXTURE0+i);
                //glLoadIdentity();
                glDisable(GL_TEXTURE_2D);
                glClientActiveTexture(GL_TEXTURE0+i);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glMatrixMode(GL_MODELVIEW);
    }

    //=====================================
    //              Constructeur
    //=====================================

    dksRendu()
    {
            drapeau_affiche=true;
    }

    //===============================================
    //          Fonctions standards:
    //===============================================

    //----- Fonctions appelées avant et après chaque affichage
    virtual void determine_drapeau_affiche(bool d){drapeau_affiche=d;}
    virtual void init_affichage(){}
    virtual void fin_affichage(){}

    virtual void affiche(dksOeil* oeil)=0;
    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)=0;
    virtual void affiche_profondeur(dksOeil* oeil)=0;

};

#endif // DKSRENDU_H_INCLUDED
