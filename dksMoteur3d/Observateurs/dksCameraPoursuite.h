#ifndef CAMERA_POURSUITE_H_INCLUDED
#define CAMERA_POURSUITE_H_INCLUDED


#include "Elements_Fondamentaux/Oeil.h"

//============== Codes d'erreur à récupérer dans .erreur:

#define CAMERA_POURSUITE_OK 0
#define CAMERA_POURSUITE_MEMOIRE 1

//======== Structure de mémorisation d'une position instantannée:
typedef struct
{
    double temps;       //Le pointeur de temps de la mémorisation
    double x;           //Position
    double y;
    double z;
    double axe_x_x;     //Repère
    double axe_x_y;
    double axe_x_z;
    double axe_y_x;
    double axe_y_y;
    double axe_y_z;
    double axe_z_x;
    double axe_z_y;
    double axe_z_z;
}position_instannee;

//================ Définition de la classe:
//  La caméra de poursuite fonctionne un peu comme un enfant de la cible,
//  à ceci près qu'elle se déplace avec un décallage dans le temps par rapport au conteneur ciblé.
//

class Camera_Poursuite :public Oeil
{
    public:
        unsigned short erreur;
        Conteneur* conteneur_cible;
        Vecteur* position;         //Position par rapport à la cible

        unsigned int nbrPositions;
        unsigned int position_actuelle;
        position_instannee* table_positions; //La table où sont stockées les positions de la cible. Sert pour le décallage temporel.
                                // 4 doubles par instantanné: 0=Temps, 1=X, 2=Y, 3=Z

        double duree_decallage; //En secondes

        bool vue_dessus;    //true si la caméra est une vue de dessus de la cible

        //==============================================================
        //                          CONSTRUCTEUR
        //==============================================================

        Camera_Poursuite(char* p_nom="Camera_poursuite",
                        Conteneur* p_cible=NULL,double p_pos_x=0.,double p_pos_y=0.,double p_pos_z=0.,
                        double p_duree_decallage=1.,
                        Vecteur_Unitaire* p_direction_regard=NULL,Vecteur_Unitaire* p_direction_haut=NULL)
                        :Oeil(p_nom,p_pos_x,p_pos_y,p_pos_z,p_direction_regard,p_direction_haut)
        {
            conteneur_cible=p_cible;
            duree_decallage=p_duree_decallage;
            table_positions=NULL;
            position_actuelle=0;
            vue_dessus=false;
            position=new Vecteur(p_pos_x,p_pos_y,p_pos_z);
            if (position==NULL)
            {
                printf("Camera_Poursuite() - Impossible de créer le vecteur position\n");
                erreur=CAMERA_POURSUITE_MEMOIRE;
                return;
            }
            //Allocation de la table des positions:
            nbrPositions=256;
            table_positions=(position_instannee*)calloc(nbrPositions,sizeof(position_instannee));
            if (table_positions==NULL)
            {
                printf ("Camera_Poursuite() - Impossible d'allouer la table des positions\n");
                erreur=CAMERA_POURSUITE_MEMOIRE;
                return;
            }

            //Position initiale:
            x=conteneur_cible->x+position->x;
            y=conteneur_cible->y+position->y;
            z=conteneur_cible->z+position->z;

        }

        //================ Destructeur:

        ~Camera_Poursuite()
        {
            if (position!=NULL)delete(position);
            if (table_positions!=NULL)free(table_positions);
        }

        //========================================================================
        //                      Gestion du decallage temporel
        //========================================================================
        void memorise_position(double temps)
        {
            table_positions[position_actuelle].temps=temps;
            table_positions[position_actuelle].x=conteneur_cible->x;
            table_positions[position_actuelle].y=conteneur_cible->y;
            table_positions[position_actuelle].z=conteneur_cible->z;
            table_positions[position_actuelle].axe_x_x=conteneur_cible->axe_x->x;
            table_positions[position_actuelle].axe_x_y=conteneur_cible->axe_x->y;
            table_positions[position_actuelle].axe_x_z=conteneur_cible->axe_x->z;
            table_positions[position_actuelle].axe_y_x=conteneur_cible->axe_y->x;
            table_positions[position_actuelle].axe_y_y=conteneur_cible->axe_y->y;
            table_positions[position_actuelle].axe_y_z=conteneur_cible->axe_y->z;
            table_positions[position_actuelle].axe_z_x=conteneur_cible->axe_z->x;
            table_positions[position_actuelle].axe_z_y=conteneur_cible->axe_z->y;
            table_positions[position_actuelle].axe_z_z=conteneur_cible->axe_z->z;

            position_actuelle++;
            if (position_actuelle>=nbrPositions)position_actuelle=0;
        }

        void rafraichi_position(double temps)
        {
            unsigned int i,j;
            bool position_trouvee=false;
            memorise_position(temps);
            if (position_actuelle==0)j=nbrPositions-1;
            else j=position_actuelle-1;

            for(i=0;i<nbrPositions;i++)
            {
                if ( duree_decallage <= temps-table_positions[j].temps)
                {
                    determine_position(table_positions[j].x+position->x,
                                        table_positions[j].y+position->y,
                                        table_positions[j].z+position->z);
                    if (vue_dessus)
                    {
                        axe_x->x=table_positions[j].axe_x_x;
                        axe_x->y=table_positions[j].axe_x_y;
                        axe_x->z=table_positions[j].axe_x_z;
                        axe_y->produit_vectoriel(axe_z,axe_x);
                    }

                    position_trouvee=true;
                    break;
                }
                if (j==0)j=nbrPositions-1;
                else j--;
            }

            if(!position_trouvee) determine_position(conteneur_cible->x+position->x,
                                                    conteneur_cible->y+position->y,
                                                    conteneur_cible->z+position->z);
        }

};
#endif // CAMERA_POURSUITE_H_INCLUDED
