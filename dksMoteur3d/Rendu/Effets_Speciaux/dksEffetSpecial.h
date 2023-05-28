#ifndef DKSEFFETSPECIAL_H_INCLUDED
#define DKSEFFETSPECIAL_H_INCLUDED

/*
    Rendu d'une surface avec un effet special. Les effets spéciaux sont les mode de rendu nécessitant des
    données plus complexes que les rendus simples (phong, ombres, bump, réflexions...)
*/
class dksSurface;

//============================================================================================
//              Définition de la classe
//============================================================================================
class dksEffetSpecial
{
    public:
      //======================
      //    Constructeur
      //======================
      dksEffetSpecial()
      {
      }
      //======================
      //       Destructeur
      //======================
      ~dksEffetSpecial(){}
      //=========================
      //    Fonctions de base:
      //=========================
      virtual void initialisation(dksSurface* surface)=0;
      virtual void rendu_opaque_immediat(dksSurface* surface)=0;
      virtual void rendu_transparente_immediat(dksFace* face)=0;
};
#endif // DKSEFFETSPECIAL_H_INCLUDED
