#ifndef DKSSPRITES_H_INCLUDED
#define DKSSPRITES_H_INCLUDED

//**********************************************************************************************
//
//                           Gestion de l'affichage des sprites
//
//          Les sprites de base (classe dksSprite3d) sont gérés au sein des listes d'affichage
//          via les particules.
//
//**********************************************************************************************

#include "dksParticules.h"
#include "dksSprite3d.h"

//*****************************************************************************
//          Particule
//*****************************************************************************
class Element_Sprite : public Element_particule
{
  public:
    dksSprite3d* sprite;
    float alpha;

    Element_Sprite(uint32_t indice,dksSprite3d* p_sprite=NULL):Element_particule(indice)
    {
        sprite=p_sprite;
        alpha=1.;
    }

};

//***********************************************************************************************
//              Classe principale
//***********************************************************************************************

class dksSprites : public dksParticules
{
    public:

    //===========================================================
    //      Constructeur et Destructeur
    //===========================================================
    dksSprites(char* p_nom="Sprites",
               double dx=0., double dy=0., double dz=0.,
               uint32_t p_nbrSprites=0.,
               dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
               )
               :dksParticules(p_nom,dx,dy,dz,0,axe1,axe2,axe3)
    {
        determine_nombre_de_points(p_nbrSprites);
        determine_rendu_faces_transparentes(true);  //De toutes façons le champ de particules ne contient pas de faces....
                                                ///Trouver une solution pour afficher le champs de particules APRES les éléments opaques de la scène.
    }

    ~dksSprites()
    {

    }

    //=========================================================
    //          Gestion des images des sprites
    //=========================================================

    //Lie un sprite à une particule
    bool determine_sprite(uint32_t indice,dksSprite3d* sprite)
    {
        if (indice>=nbrPoints) return false;
        Element_Sprite* particule=(Element_Sprite*)particules[indice];
        particule->sprite=sprite;
        return true;
    }

    //Le même sprite est lié à toutes les particules:
    void determine_sprites(dksSprite3d* sprite)
    {
        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Element_Sprite* particule=(Element_Sprite*)particules[i];
            particule->sprite=sprite;
        }
    }

    //=========================================================
    //          Gestion des particules
    //=========================================================
    virtual void creation_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++) particules[i]=new Element_Sprite(i,NULL);
    }

    virtual void destruction_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++)
            {
                if (particules[i] != NULL) delete particules[i];
            }
    }

    //=========================================================
    //          Gestion des affichages
    //=========================================================

    virtual void affiche(dksOeil* oeil)
    {
        tri_points(oeil);
        for (uint32_t i=0;i<nbrPoints;i++)
        {

            uint32_t indice=indices_points[i].indice_point;
            Element_Sprite* sprite=(Element_Sprite*)particules[indice];
            sprite->sprite->affiche_texture(points[3*indice],points[3*indice+1],points[3*indice+2],oeil,sprite->alpha);
        }
    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {

    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {

    }

};


#endif // DKSSPRITES_H_INCLUDED
