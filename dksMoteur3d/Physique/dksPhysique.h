#ifndef PHYSIQUE_H_INCLUDED
#define PHYSIQUE_H_INCLUDED

#include <math.h>
#include "Geometrie_Espace/Vecteur.h"
#include "Volumes/Boite_Collision.h"

//=================================================================================
//              Définition de la classe
//=================================================================================

class Physique
{
    public:
        uint32_t collision_face;    //Numéro de la face sur laquelle une collision a été détectée
        uint16_t collision_arete;   //Numéro de l'arête coupant la face
        double facteur; //paramètre d'intersection entre l'arête et la face

        double t0;          //La date de l'image précédente
        double delta_t;     //L'intervalle de temps actuel
    private:
        double t_collision; //Date de collision entre deux volumes
        Vecteur point_collision; //Point de collision entre deux volumes

        Vecteur tangente;   //Trajectoire tengante à la rotation d'un conteneur

        Vecteur A,B,O,U,V;    //Vecteurs de travail.

        //Plan de la scène:
        uint32_t nbr_systemes_immobiles;
        uint32_t nbr_systemes_mobiles;
        Conteneur** systemes_immobiles;
        Conteneur** systemes_mobiles;


        //======================================================================
        //                          CONSTRUCTEUR
        //======================================================================
    public:
        Physique(
                    Conteneur** p_systemes_mobiles=NULL,uint32_t p_nbr_systemes_mobiles=0,
                    Conteneur** p_systemes_immobiles=NULL,uint32_t p_nbr_systemes_immobiles=0)
        {
            t0=0;
            delta_t=0;
            systemes_mobiles=p_systemes_mobiles;
            systemes_immobiles=p_systemes_immobiles;
            nbr_systemes_mobiles=p_nbr_systemes_mobiles;
            nbr_systemes_immobiles=p_nbr_systemes_immobiles;
        }

        ~Physique()
        {
        }


        //======================================================================
        //                          Gestion des listes de tests de collisions
        //======================================================================
        void determine_systemes_mobiles(Conteneur** p_systemes_mobiles, uint32_t p_nbr_systemes_mobiles)
        {
            systemes_mobiles=p_systemes_mobiles;
            nbr_systemes_mobiles=p_nbr_systemes_mobiles;
        }

        void determine_volumes_immobiles(Conteneur** p_systemes_immobiles, uint32_t p_nbr_systemes_immobiles)
        {
            systemes_immobiles=p_systemes_immobiles;
            nbr_systemes_immobiles=p_nbr_systemes_immobiles;
        }

        //======================================================================
        //                          Gestion des collisions
        //======================================================================
        //Test si le systeme A touche le systeme B :
        //Collision simple: En cas de contact, la date de collision n'est pas recherchée.

        bool collision_simple(Conteneur* systeme_A, Conteneur* systeme_B,Vecteur* point_collision=NULL)
        {
            Volume* volume_A;
            Volume* volume_B;
            uint32_t i,j;
            uint32_t nbrEnfantsA=systeme_A->renvoie_nbr_enfants();
            uint32_t nbrEnfantsB=systeme_B->renvoie_nbr_enfants();
            for (i=0;i<nbrEnfantsA;i++)
            {
                if (systeme_A->enfants[i]->classe_id==CLASSE_VOLUME_ID)
                {
                    volume_A=(Volume*)systeme_A->enfants[i];

                    for (j=0;j<nbrEnfantsB;j++)
                    {
                        if (systeme_B->enfants[j]->classe_id==CLASSE_VOLUME_ID)
                        {
                            volume_B=(Volume*)systeme_B->enfants[j];
                            if (volume_A->boite_collision!=NULL && volume_B->boite_collision!=NULL)
                            {
                                if (test_intersection_aretes_volume(volume_A->boite_collision, volume_B->boite_collision,point_collision)) return true;
                                else if (test_intersection_aretes_volume(volume_B->boite_collision,volume_A->boite_collision,point_collision)) return true;
                            }
                        }
                    }
                }
            }
            return false;

        }


       //Test si le système A touche le système B,
       //Calcul la date de collision le cas échéant.
       //Pour récupérer la date: renvoie_date_collision().
        bool collision_avec_date(Conteneur* systeme_A, Conteneur* systeme_B,Vecteur* point_collision=NULL)
        {
            Volume* volume_A;
            Volume* volume_B;
            uint32_t i,j;
            uint32_t nbrEnfantsA=systeme_A->renvoie_nbr_enfants();
            uint32_t nbrEnfantsB=systeme_B->renvoie_nbr_enfants();

            for (i=0;i<nbrEnfantsA;i++)
            {
                if (systeme_A->enfants[i]->classe_id==CLASSE_VOLUME_ID)
                {
                    volume_A=(Volume*)systeme_A->enfants[i];
                    for (j=0;j<nbrEnfantsB;j++)
                    {
                        if (systeme_B->enfants[j]->classe_id==CLASSE_VOLUME_ID)
                        {
                            volume_B=(Volume*)systeme_B->enfants[j];
                            if (volume_A->boite_collision!=NULL && volume_B->boite_collision!=NULL)
                            {
                                if (test_intersection_aretes_volume(volume_A->boite_collision, volume_B->boite_collision,point_collision)) return true;
                                else if (test_intersection_aretes_volume(volume_B->boite_collision,volume_A->boite_collision,point_collision)) return true;
                            }
                        }
                    }
                }
            }
            return false;

        }

        //Renvoie la date de collision:
        double renvoie_date_collision()
        {
            return t_collision;
        }

        //======================================================================
        //                Calculs de collisions et des réactions
        //======================================================================
        void maj_collisions()
        {
            uint32_t i,j;
            for (i=0;i< nbr_systemes_mobiles;i++)
            {
                for (j=0;j<nbr_systemes_immobiles;j++)
                {
                    if ( collision_simple(systemes_mobiles[i],systemes_immobiles[j],&point_collision))
                    {
                        //cout<<"COLLISION: x:"<<point_collision.x<< "y:"<<point_collision.y<<" z:"<<point_collision.z<<endl;
                        systemes_mobiles[i]->determine_interpolation_instantannee(0.);
                        systemes_mobiles[i]->vitesse->inverse();
                        systemes_mobiles[i]->vitesse_circulaire->inverse();
                    }
                }
            }
        }

        //======================================================================
        //                          Fonctions de base
        //======================================================================
        //Test si les aretes du systeme A interceptent les faces du systeme B:
        //Si il y a une collsiion, les indexes de la face et de l'arête sont
        //stockés dans collision_face et collision_arete.
        //Le point est stocké dans le vecteur d'entrée si il y en a un.
        //Renvoie false si il n'y a pas d'intersection:
        bool test_intersection_aretes_volume(Boite_Collision* volume_A, Boite_Collision* volume_B,Vecteur* point_collision=NULL)
        {
            uint32_t i,indexA,indexB,indexO,indexU,indexV;
            uint16_t j;
            double px,py,pz;

            //Test les intersections entre les aretes du volume A et les faces du volume B:
            vector<double>* tab_sommets_A=&(volume_A->sommets_absolus);
            vector<double>* tab_sommets_B=&(volume_B->sommets_absolus);

            for (i=0;i<volume_B->nbrFaces;i++)
            {
                Face_collision* face_B=&(volume_B->faces[i]);
                //affiche_face(face_B,tab_sommets,0xffff00);
                indexO=face_B->aretes[0]*3;
                indexU=face_B->aretes[1]*3;
                indexV=face_B->aretes[2]*3;

                O.x=(*tab_sommets_B)[indexO];
                O.y=(*tab_sommets_B)[indexO+1];
                O.z=(*tab_sommets_B)[indexO+2];

                U.x=(*tab_sommets_B)[indexU];
                U.y=(*tab_sommets_B)[indexU+1];
                U.z=(*tab_sommets_B)[indexU+2];

                V.x=(*tab_sommets_B)[indexV];
                V.y=(*tab_sommets_B)[indexV+1];
                V.z=(*tab_sommets_B)[indexV+2];

                for(j=0;j<volume_A->nbrAretes;j++)
                {
                    indexA=volume_A->aretes[2*j]*3;
                    indexB=volume_A->aretes[2*j+1]*3;
                    //affiche_segment( tab_sommets[indexA],tab_sommets[indexA+1],tab_sommets[indexA+2],
                                    //tab_sommets[indexB],tab_sommets[indexB+1],tab_sommets[indexB+2],0xffffff);
                    A.x=(*tab_sommets_A)[indexA];
                    A.y=(*tab_sommets_A)[indexA+1];
                    A.z=(*tab_sommets_A)[indexA+2];
                    B.x=(*tab_sommets_A)[indexB];
                    B.y=(*tab_sommets_A)[indexB+1];
                    B.z=(*tab_sommets_A)[indexB+2];

                    facteur=Vecteur::intersection_droite_plan(&A,&B,&O,&U,&V,&face_B->normale);

                    //if (facteur!=NAN) affiche_point(A.x+(B.x-A.x)*facteur,A.y+(B.y-A.y)*facteur,A.z+(B.z-A.z)*facteur,0xff);

                    if (facteur>0.f && facteur<1.f)
                    {
                        px=A.x+(B.x-A.x)*facteur;
                        py=A.y+(B.y-A.y)*facteur;
                        pz=A.z+(B.z-A.z)*facteur;

                        if( Vecteur::inclus_dans_polygone(&(face_B->aretes),
                                                            tab_sommets_B,
                                                            face_B->nbrSommets,
                                                            &face_B->normale,
                                                            px,py,pz)
                            )
                        {
                            collision_face=i;
                            collision_arete=j;
                            if (point_collision!=NULL)
                            {
                                point_collision->x=px;
                                point_collision->y=py;
                                point_collision->z=pz;
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        }



};
#endif // PHYSIQUE_H_INCLUDED
