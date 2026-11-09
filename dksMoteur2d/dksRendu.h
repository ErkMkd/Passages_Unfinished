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
        //dksMatrices::mode(GL_TEXTURE);
        //dksMatrices::identite();
        for (i=0;i<nbr_unites_textures_max;i++)
            {
                dksEtat::unite_active(GL_TEXTURE0+i);
                //dksMatrices::mode(GL_TEXTURE0+i);
                //dksMatrices::identite();
                dksEtat::desactive(GL_TEXTURE_2D);
                dksTableaux::unite(GL_TEXTURE0+i);
                dksTableaux::desactive(GL_TEXTURE_COORD_ARRAY);
            }
        dksMatrices::mode(GL_MODELVIEW);
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
