#ifndef BATIMENT_H_INCLUDED
#define BATIMENT_H_INCLUDED

#include "../Elements_Fondamentaux/Volume.h"

//Drapeaux pour la libération de la mémoire lors de la destruction:

#define BATIMENT_VOLUMES 0x0000001


//===================================================================================
//
//      ATTENTION: Les volumes ne doivent pas faire partis d'une hiérarchie externe,
//                  Sinon la duplication ne marche pas bien.
//
//===================================================================================

//================ Définition de la classe:
class Batiment :public Conteneur
{
    public:
        Volume** volumes; //Les volumes constituants le batiment
        unsigned int nbrVolumes;

        unsigned int drapeaux_liberation_memoire;


        //===================================================================
        //                          CONSTRUCTEUR
        //===================================================================

        Batiment(char* p_nom,Volume** p_volumes,unsigned int p_nbrVolumes,
                    unsigned int p_drapeaux_liberation_memoire=0,
                    double px = 0., double py = 0., double pz = 0.,
                    Vecteur_Unitaire* axe1 = NULL, Vecteur_Unitaire* axe2 = NULL, Vecteur_Unitaire* axe3 = NULL)
                    :Conteneur(p_nom,px,py,pz,axe1,axe2,axe3)
        {
            volumes=p_volumes;
            nbrVolumes=p_nbrVolumes;
            drapeaux_liberation_memoire=p_drapeaux_liberation_memoire;

            unsigned int i;

            //Ajoute les volumes dans la table des enfants.
            for (i=0;i<nbrVolumes;i++)
            {
                if (volumes[i]->parent==NULL)
                {
                    ajoute_enfant(volumes[i]);
                }
            }
        }

        //================ DESTRUCTEUR ======================

        ~Batiment()
        {
            unsigned int i;
            if((drapeaux_liberation_memoire & BATIMENT_VOLUMES)!=0)
            {
                if (volumes!=NULL)
                {
                    for (i=0;i<nbrVolumes;i++)
                    {
                        if (volumes[i]!=NULL) delete(volumes[i]);
                    }
                    free(volumes);
                }
            }
        }

        //======================================================================
        //                      Duplique le batiment
        //======================================================================
        Batiment* duplique()
        {
            Volume** volumes_copie=(Volume**)calloc(nbrVolumes,4);
            if(volumes_copie==NULL)return NULL;

            unsigned int i,j;

            //Duplique les volumes:
            for(i=0;i<nbrVolumes;i++)
            {
                Volume* volume_copie=volumes[i]->duplique();
                if (volume_copie==NULL)
                {
                    for (j=0;j<i;j++) { delete(volumes_copie[j]); }
                    free(volumes_copie);
                    return NULL;
                }
                volumes_copie[i]=volume_copie;
            }

            //Copie la hiérarchie:
            for(i=0;i<nbrVolumes;i++)
            {
                if(volumes[i]->parent==this)volumes_copie[i]->parent=NULL;
                else
                {
                    for(j=0;j<nbrVolumes;j++)
                    {
                        if(volumes[i]->parent==volumes[j])
                        {
                            volumes_copie[j]->ajoute_enfant(volumes_copie[i]);
                            break;
                        }
                    }
                }
            }

            //Crée le clone du batiment:
            Batiment* clone= new Batiment(
                                        nom,volumes_copie,nbrVolumes,BATIMENT_VOLUMES,
                                        x,y,z,
                                        new Vecteur_Unitaire(axe_x->x,axe_x->y,axe_x->z),
                                        new Vecteur_Unitaire(axe_y->x,axe_y->y,axe_y->z),
                                        new Vecteur_Unitaire(axe_z->x,axe_z->y,axe_z->z)
                                        );
            return clone;
        }

        //======================================================================
        //                      Gestion des collisions
        //======================================================================
        virtual bool initialise_boites_collisions(double facteur_x=1.0,double facteur_y=1.0, double facteur_z=1.0)
        {
            uint32_t i;
            bool drapeau=true;
            for (i=0;i< nbrVolumes;i++)
            {
                drapeau = drapeau&&volumes[i]->initialise_boite_collisions(facteur_x,facteur_y,facteur_z);
            }
            return drapeau;
        }
};


#endif // BATIMENT_H_INCLUDED
