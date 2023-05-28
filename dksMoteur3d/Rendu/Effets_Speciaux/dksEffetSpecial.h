#ifndef DKSEFFETSPECIAL_H_INCLUDED
#define DKSEFFETSPECIAL_H_INCLUDED

/*
    Rendu d'une surface avec un effet special. Les effets sp�ciaux sont les mode de rendu n�cessitant des
    donn�es plus complexes que les rendus simples (phong, ombres, bump, r�flexions...)
*/
class dksSurface;

//============================================================================================
//              D�finition de la classe
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
