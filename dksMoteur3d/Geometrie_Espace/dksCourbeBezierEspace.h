#ifndef DKSCOURBEBEZIERESPACE_H_INCLUDED
#define DKSCOURBEBEZIERESPACE_H_INCLUDED

#include "vec3d.h"

class dksCourbeBezierEspace
{
    private:
        double P0_x; //Position du point 0
        double P0_y;
        double P0_z;

        double P1_x; //Position du point 1
        double P1_y;
        double P1_z;

        double P2_x; //Position du point 2
        double P2_y;
        double P2_z;

        double P3_x; //Position du point 3
        double P3_y;
        double P3_z;

        double tangenteA_x;
        double tangenteA_y;
        double tangenteA_z;
        double tangenteB_x;
        double tangenteB_y;
        double tangenteB_z;

        double facteur; //Facteur multiplucateur des tangentes.

        //Tempons pour les translations de repères:
        vec3d P0;
        vec3d P1;
        vec3d P2;
        vec3d P3;

    public:

        //=================================================================
        //                          CONSTRUCTEUR
        //=================================================================

        dksCourbeBezierEspace(vec3d pP0, vec3d pP1, vec3d pP2, vec3d pP3 , double fact = 1.)
        {
            facteur = fact;
            P0.determine(pP0.x,pP0.y,pP0.z);
            P1.determine(pP1.x,pP1.y,pP1.z);
            P2.determine(pP2.x,pP2.y,pP2.z);
            P3.determine(pP3.x,pP3.y,pP3.z);
            initialise_coordonnees();
            determine_facteur(fact);
        }

        dksCourbeBezierEspace(vec3d* pP0=NULL, vec3d* pP1=NULL, vec3d* pP2=NULL, vec3d* pP3=NULL , double fact = 1.)
        {
            facteur = fact;
            if (pP0 == NULL) P0.determine(0.,0.,0.); else P0.determine(pP0->x,pP0->y,pP0->z);
            if (pP1 == NULL) P1.determine(0.,0.,1.); else P1.determine(pP1->x,pP1->y,pP1->z);
            if (pP2 == NULL) P2.determine(1.,1.,2.); else P2.determine(pP2->x,pP2->y,pP2->z);
            if (pP3 == NULL) P3.determine(1.,1.,1.); else P3.determine(pP3->x,pP3->y,pP3->z);
            initialise_coordonnees();
            determine_facteur(fact);
        }

        dksCourbeBezierEspace(
                            double pP0_x,double pP0_y,double pP0_z,
                            double pP1_x,double pP1_y,double pP1_z,
                            double pP2_x,double pP2_y,double pP2_z,
                            double pP3_x,double pP3_y,double pP3_z,
                            double fact = 1.)
        {
            facteur = fact;
            P0.determine(pP0_x,pP0_y,pP0_z);
            P1.determine(pP1_x,pP1_y,pP1_z);
            P2.determine(pP2_x,pP2_y,pP2_z);
            P3.determine(pP3_x,pP3_y,pP3_z);
            initialise_coordonnees();
            determine_facteur(fact);
        }

    private:
        void initialise_coordonnees()
        {
            P0_x = P0.x;
            P0_y = P0.y;
            P0_z = P0.z;
            P1_x = P1.x;
            P1_y = P1.y;
            P1_z = P1.z;
            P2_x = P2.x;
            P2_y = P2.y;
            P2_z = P2.z;
            P3_x = P3.x;
            P3_y = P3.y;
            P3_z = P3.z;
        }


        //========================================================================
        //                      Gestion de la courbe:
        //========================================================================

    public:
        void determine_P0(vec3d* P)
        {
            P0_x = P->x;
            P0_y = P->y;
            P0_z = P->z;
            tangenteA_x = (P1_x - P0_x) * facteur + P0_x;
            tangenteA_y = (P1_y - P0_y) * facteur + P0_y;
            tangenteA_z = (P1_z - P0_z) * facteur + P0_z;
        }

        void determine_P1(vec3d* P)
        {
            P1_x = P->x;
            P1_y = P->y;
            P1_z = P->z;
            tangenteA_x = (P1_x - P0_x) * facteur + P0_x;
            tangenteA_y = (P1_y - P0_y) * facteur + P0_y;
            tangenteA_z = (P1_z - P0_z) * facteur + P0_z;
        }

        void determine_P2(vec3d* P)
        {
            P2_x = P->x;
            P2_y = P->y;
            P2_z = P->z;
            tangenteB_x = (P2_x - P3_x) * facteur + P3_x;
            tangenteB_y = (P2_y - P3_y) * facteur + P3_y;
            tangenteB_z = (P2_z - P3_z) * facteur + P3_z;
        }

        void determine_P3(vec3d* P)
        {
            P3_x = P->x;
            P3_y = P->y;
            P3_z = P->z;
            tangenteB_x = (P2_x - P3_x) * facteur + P3_x;
            tangenteB_y = (P2_y - P3_y) * facteur + P3_y;
            tangenteB_z = (P2_z - P3_z) * facteur + P3_z;
        }

        void determine_facteur(double f)
        {
            facteur = f;
            tangenteA_x = (P1_x - P0_x) * facteur + P0_x;
            tangenteA_y = (P1_y - P0_y) * facteur + P0_y;
            tangenteA_z = (P1_z - P0_z) * facteur + P0_z;
            tangenteB_x = (P2_x - P3_x) * facteur + P3_x;
            tangenteB_y = (P2_y - P3_y) * facteur + P3_y;
            tangenteB_z = (P2_z - P3_z) * facteur + P3_z;
        }

        //Redéfini le point P0, en gardant les coordonnées relatives du point P1 (tangente)
        void determine_A0(vec3d* A0)
        {
            P1_x = P1_x - P0_x + A0->x;
            P1_y = P1_y - P0_y + A0->y;
            P1_z = P1_z - P0_z + A0->z;
            tangenteA_x += A0->x - P0_x;
            tangenteA_y += A0->y - P0_y;
            tangenteA_z += A0->z - P0_z;
            P0_x = A0->x;
            P0_y = A0->y;
            P0_z = A0->z;
        }

        //Redéfini le point P3, en gardant les coordonnées relatives du point P2 (tangente)
        void determine_B0(vec3d* B0)
        {
            P2_x = P2_x - P3_x + B0->x;
            P2_y = P2_y - P3_y + B0->y;
            P2_z = P2_z - P3_z + B0->z;
            tangenteB_x += B0->x - P3_x;
            tangenteB_y += B0->y - P3_y;
            tangenteB_z += B0->z - P3_z;
            P3_x = B0->x;
            P3_y = B0->y;
            P3_z = B0->z;
        }

        void renvoie_P0(vec3d* resP0)
        {
                resP0->x = P0_x;
                resP0->y = P0_y;
                resP0->z = P0_z;
        }

        void renvoie_P1(vec3d* resP)
        {
                resP->x = P1_x;
                resP->y = P1_y;
                resP->z = P1_z;
        }

        void renvoie_P2(vec3d* resP)
        {
                resP->x = P2_x;
                resP->y = P2_y;
                resP->z = P2_z;
        }

        void renvoie_P3(vec3d* resP)
        {
                resP->x = P3_x;
                resP->y = P3_y;
                resP->z = P3_z;
        }

        double renvoie_facteur()
        {
            return facteur;
        }

        //Renvoie la postion d'un point de la courbe pour une valeur de t comprise entre 0 (point A) et 1 (point B)
        void renvoie_position(double t, vec3d* res)
        {
            if (t > 1.) { t = 1.; }
            else if (t < 0.) { t = 0.; }

            double t1 = 1 - t;
            double t12  = t1 * t1;
            double t13  = t12 * t1;

            double t2 = t * t;
            double t3 = t2 * t;

            res->x = P0_x * t13 + 3 * tangenteA_x * t * t12 + 3 * tangenteB_x * t2 * t1 + P3_x * t3;
            res->y = P0_y * t13 + 3 * tangenteA_y * t * t12 + 3 * tangenteB_y * t2 * t1 + P3_y * t3;
            res->z = P0_z * t13 + 3 * tangenteA_z * t * t12 + 3 * tangenteB_z * t2 * t1 + P3_z * t3;
        }

        //Translate la courbe par un repère, et renvoie le résultat dans "courbe_translatee":
        void translation(dksRepere* repere, dksCourbeBezierEspace* courbe_translatee)
        {
            double ax_x = repere->axe_x->x * repere->echelle_x;
            double ax_y = repere->axe_x->y * repere->echelle_x;
            double ax_z = repere->axe_x->z * repere->echelle_x;

            double ay_x = repere->axe_y->x * repere->echelle_y;
            double ay_y = repere->axe_y->y * repere->echelle_y;
            double ay_z = repere->axe_y->z * repere->echelle_y;

            double az_x = repere->axe_z->x * repere->echelle_z;
            double az_y = repere->axe_z->y * repere->echelle_z;
            double az_z = repere->axe_z->z * repere->echelle_z;

            P0.x = repere->x + P0_x * ax_x + P0_y * ay_x + P0_z * az_x;
            P0.y = repere->y + P0_x * ax_y + P0_y * ay_y + P0_z * az_y;
            P0.z = repere->z + P0_x * ax_z + P0_y * ay_z + P0_z * az_z;

            P1.x = repere->x + P1_x * ax_x + P1_y * ay_x + P1_z * az_x;
            P1.y = repere->y + P1_x * ax_y + P1_y * ay_y + P1_z * az_y;
            P1.z = repere->z + P1_x * ax_z + P1_y * ay_z + P1_z * az_z;

            P2.x = repere->x + P2_x * ax_x + P2_y * ay_x + P2_z * az_x;
            P2.y = repere->y + P2_x * ax_y + P2_y * ay_y + P2_z * az_y;
            P2.z = repere->z + P2_x * ax_z + P2_y * ay_z + P2_z * az_z;

            P3.x = repere->x + P3_x * ax_x + P3_y * ay_x + P3_z * az_x;
            P3.y = repere->y + P3_x * ax_y + P3_y * ay_y + P3_z * az_y;
            P3.z = repere->z + P3_x * ax_z + P3_y * ay_z + P3_z * az_z;

            courbe_translatee->determine_P0(&P0);
            courbe_translatee->determine_P1(&P1);
            courbe_translatee->determine_P2(&P2);
            courbe_translatee->determine_P3(&P3);
        }

        //Détermine la position relative de la courbe à un repère à partir d'une position absolue
        //Renvoie la position dans courbe_relative
        void determine_coordonnees_relatives(dksRepere* repere,dksCourbeBezierEspace* courbe_relative)
        {
            double xTemp = (P0_x - repere->x) ;
            double yTemp = (P0_y - repere->y) ;
            double zTemp = (P0_z - repere->z) ;
            P0.x = (repere->axe_x->x * xTemp + repere->axe_x->y * yTemp + repere->axe_x->z * zTemp) / repere->echelle_x;
            P0.y = (repere->axe_y->x * xTemp + repere->axe_y->y * yTemp + repere->axe_y->z * zTemp) / repere->echelle_y;
            P0.z = (repere->axe_z->x * xTemp + repere->axe_z->y * yTemp + repere->axe_z->z * zTemp) / repere->echelle_z;

            xTemp = (P1_x - repere->x) ;
            yTemp = (P1_y - repere->y) ;
            zTemp = (P1_z - repere->z) ;
            P1.x = (repere->axe_x->x * xTemp + repere->axe_x->y * yTemp + repere->axe_x->z * zTemp) / repere->echelle_x;
            P1.y = (repere->axe_y->x * xTemp + repere->axe_y->y * yTemp + repere->axe_y->z * zTemp) / repere->echelle_y;
            P1.z = (repere->axe_z->x * xTemp + repere->axe_z->y * yTemp + repere->axe_z->z * zTemp) / repere->echelle_z;

            xTemp = (P2_x - repere->x) ;
            yTemp = (P2_y - repere->y) ;
            zTemp = (P2_z - repere->z) ;
            P2.x = (repere->axe_x->x * xTemp + repere->axe_x->y * yTemp + repere->axe_x->z * zTemp) / repere->echelle_x;
            P2.y = (repere->axe_y->x * xTemp + repere->axe_y->y * yTemp + repere->axe_y->z * zTemp) / repere->echelle_y;
            P2.z = (repere->axe_z->x * xTemp + repere->axe_z->y * yTemp + repere->axe_z->z * zTemp) / repere->echelle_z;

            xTemp = (P3_x - repere->x) ;
            yTemp = (P3_y - repere->y) ;
            zTemp = (P3_z - repere->z) ;
            P3.x = (repere->axe_x->x * xTemp + repere->axe_x->y * yTemp + repere->axe_x->z * zTemp) / repere->echelle_x;
            P3.y = (repere->axe_y->x * xTemp + repere->axe_y->y * yTemp + repere->axe_y->z * zTemp) / repere->echelle_y;
            P3.z = (repere->axe_z->x * xTemp + repere->axe_z->y * yTemp + repere->axe_z->z * zTemp) / repere->echelle_z;

            courbe_relative->determine_P0(&P0);
            courbe_relative->determine_P1(&P1);
            courbe_relative->determine_P2(&P2);
            courbe_relative->determine_P3(&P3);

        }

        void toString()
        {
            cout<<"---- Courbe de Bézier spatiale:"<<endl;
            cout<<"P0: "<< P0_x <<" "<< P0_y<< " " << P0_z<<endl
            <<"P1: "<<P1_x <<" " << P1_y << " " << P1_z<<endl
            <<"P2: "<< P2_x <<" " << P2_y << " " << P2_z<<endl
            <<"P3: "<< P3_x <<" " << P3_y << " " << P3_z<<endl;
        }

};

#endif // DKSCOURBEBEZIERESPACE_H_INCLUDED
