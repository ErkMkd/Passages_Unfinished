#ifndef DKSJONGLEURVECTORBALLS_H_INCLUDED
#define DKSJONGLEURVECTORBALLS_H_INCLUDED

#include "dksPersonnage.h"
#include "dksJongleur.h"
#include "../Volumes/dksSphere.h"
#include "../../Utiles/OrganisationCouleurs.h"
// Erreurs:
#define DKSJONGLEURVECTORBALLS_OK 0
#define DKSJONGLEURVECTORBALLS_ERREUR_NBR_VECTORBALLS 1 //Pas assez de vectorballs dans la bibliothèque

/*
            Matérialisation d'un jongleur avec des sphères
            Le nombre de sphères par partie de membre est spécifié avec l'articulation supérieure exclue.
            Ex: 3 sphères pour la jambe: sphère 1 située au niveau du genou, 2ème au tiers, 3ème au 2 tiers. L'articulation au niveau de la hanche n'est pas matérialisé.
            -Sauf pour les pieds (de la plante au talon)

*/

#define DKSJONGLEURVECTORBALLS_VB_MAX 16 //Le nombre maximum de sphères par section de membre.

//===========================================================================================================
//                  Définition de la classe
//          Les sphères doivent être des enfants du jongleur.
//===========================================================================================================
class dksJongleurVectorBalls: public dksJongleur
{
    private:
        dksSphere** vectorBalls;
        uint16_t nbrVectorBalls;

        uint16_t nbrVectorBalls_utilisees;
        uint16_t nbrBalles_max; //Calculé en fonction de ce qui reste dans la bibliothèque de départ après formation du corps.
        uint16_t indice_balles; //Indice de départ des balles

        //Structure des membres:
        uint16_t indices_pieds[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_pieds;

        uint16_t indices_jambes_anterieures[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_jambes_anterieures;

        uint16_t indices_jambes[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_jambes;

        uint16_t indices_abdomen[DKSJONGLEURVECTORBALLS_VB_MAX];
        uint16_t nbrVectorBalls_abdomen;

        uint16_t indices_thorax[DKSJONGLEURVECTORBALLS_VB_MAX];
        uint16_t nbrVectorBalls_thorax;

        uint16_t indices_cou[DKSJONGLEURVECTORBALLS_VB_MAX];
        uint16_t nbrVectorBalls_cou;

        uint16_t indices_bras[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_bras;

        uint16_t indices_avant_bras[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_avant_bras;

        uint16_t indices_mains[DKSJONGLEURVECTORBALLS_VB_MAX*2];
        uint16_t nbrVectorBalls_mains;

        uint16_t indice_epaule_droite;
        uint16_t indice_epaule_gauche;
        uint16_t indice_hanche_droite;
        uint16_t indice_hanche_gauche;
        uint16_t indice_tete;
        uint16_t indice_oeil_droit;
        uint16_t indice_oeil_gauche;

    public:
        float rayons_vectorBalls_pieds[2];
        float rayons_vectorBalls_jambes_anterieures[2];
        float rayons_vectorBalls_jambes[2];
        float rayons_vectorBalls_hanches;
        float rayons_vectorBalls_abdomen[2];
        float rayons_vectorBalls_thorax[2];
        float rayons_vectorBalls_cou[2];
        float rayons_vectorBalls_epaules;
        float rayons_vectorBalls_bras[2];
        float rayons_vectorBalls_avant_bras[2];
        float rayons_vectorBalls_mains[2];
        float rayon_vectorBall_tete;
        float rayon_vectorBall_oeil_droit;
        float rayon_vectorBall_oeil_gauche;

        float couleur_pieds[2][3];
        float couleur_jambes_anterieures[2][3];
        float couleur_jambes[2][3];
        float couleur_hanches[3];
        float couleur_abdomen[2][3];
        float couleur_thorax[2][3];
        float couleur_cou[2][3];
        float couleur_epaules[3];
        float couleur_bras[2][3];
        float couleur_avant_bras[2][3];
        float couleur_mains[2][3];
        float couleur_tete[3];
        float couleur_oeil_droit[3];
        float couleur_oeil_gauche[3];

        float couleurs_balles[JONGLEUR_BALLES_MAX][3];    //Couleurs par défaut des 9 premières balles de jonglage

    //==============================================
    //          Constructeur
    //==============================================
    public:
        dksJongleurVectorBalls(char* p_nom,double px=0.,double py=0.,double pz=0.,dksSphere** p_vectorBalls=NULL,uint16_t p_nbrVectorBalls=0):dksJongleur(p_nom,px,py,pz)
        {
            vectorBalls=p_vectorBalls;
            nbrVectorBalls=p_nbrVectorBalls;

            //Valeurs par défaut:

            rayons_vectorBalls_pieds={0.09/2.,0.11/2.};
            rayons_vectorBalls_jambes_anterieures={0.17/2.,0.13/2.};
            rayons_vectorBalls_jambes={0.13/2.,0.17/2.};
            rayons_vectorBalls_hanches=0.18/2.;
            rayons_vectorBalls_abdomen={0.30/2.,0.30/2.};
            rayons_vectorBalls_thorax={0.25/2.,0.45/2.};
            rayons_vectorBalls_cou={0.10/2.,0.85/2.};
            rayons_vectorBalls_epaules=0.15/2.;
            rayons_vectorBalls_bras={0.10/2.,0.14/2.};
            rayons_vectorBalls_avant_bras={0.08/2.,0.095/2.};
            rayons_vectorBalls_mains={0.07/2.,0.06/2.};
            rayon_vectorBall_tete=0.22/2.;
            rayon_vectorBall_oeil_droit=0.03/2.;
            rayon_vectorBall_oeil_gauche=0.03/2.;

            couleur_pieds={{1.,1.,0.2},{1.,0.25,0.25}};
            couleur_jambes_anterieures={{1.,0.25,0.25},{0.25,1.,1.}};
            couleur_jambes={{0.25,1.,1.},{1.,0.5,0.15}};
            couleur_hanches={1.,0.6,0.20};
            couleur_abdomen={{1.,0.65,0.45},{1.,0.7,0.7}};
            couleur_thorax={{1.,0.7,0.5},{1.,0.5,0.7}};
            couleur_cou={{1.,0.25,0.25},{1.,0.1,1.}};
            couleur_epaules={1.,0.25,1.};
            couleur_bras={{1.,1.,0.1},{1.,0.35,1.}};
            couleur_avant_bras={{.25,1.,0.25},{1.,1.,0.2}};
            couleur_mains={{0.25,1.,0.25},{1.,1.,0.1}};
            couleur_tete={1.,.5,0.5};
            couleur_oeil_droit={0.25,0.5,1.};
            couleur_oeil_gauche={0.25,0.5,1.};

            couleurs_balles={
                                {1.,1.,1.},
                                {0.,1.,0.},
                                {0.,0.,1.},
                                {1.,1.,0.},
                                {1.,0.,1.},
                                {0.,1.,1.},
                                {1.,0.,0.},
                                {1.,.5,0.},
                                {0.5,1.,0.}
                            };


            if(vectorBalls!=NULL && nbrVectorBalls>0)
            {
                if (!determine_nbrVectorBalls(4,4,4,1,2,1,4,4,4))
                {
                    return;
                }
                determine_rayon_balles(0.05);
                maj_couleurs_vectorBalls();
                maj_couleurs_balles();
            }
        }

    //=============================================
    //      Destructeur
    //=============================================
        ~dksJongleurVectorBalls()
        {

        }
    //=========================================
    //      Paramétrages
    //=========================================
    public:
        bool determine_nbrVectorBalls(uint16_t p_nbr_pieds,uint16_t p_nbr_jambes_anterieures,uint16_t p_nbr_jambes,
                                        uint16_t p_nbr_abdomen,uint16_t p_nbr_thorax,uint16_t p_nbr_cou,
                                        uint16_t p_nbr_bras,uint16_t p_nbr_avant_bras,uint16_t p_nbr_mains)
        {
            uint32_t total_requis=p_nbr_pieds*2+p_nbr_jambes_anterieures*2+p_nbr_jambes*2+p_nbr_abdomen+p_nbr_thorax+p_nbr_cou+p_nbr_bras*2+p_nbr_avant_bras*2+p_nbr_mains*2+7; //+7 pour les parties uniques(yeux...)
            if(total_requis>nbrVectorBalls)
            {
                erreur=DKSJONGLEURVECTORBALLS_ERREUR_NBR_VECTORBALLS;
                cout<<"ERREUR - dksJongleurVectorBalls::determine_nbrVectorBalls - Nombre de vectorsBalls insuffisant dans la bibliothèque!"<<endl;
                return false;
            }
            nbrVectorBalls_utilisees=total_requis;
            nbrBalles_max=nbrVectorBalls-total_requis;
            cout<<"Nombre de sphères utilisées: "<<nbrVectorBalls_utilisees<<endl;
            cout<<"Nombre de balles disponibles: "<<nbrBalles_max<<endl;

            nbrVectorBalls_pieds=               (p_nbr_pieds<=DKSJONGLEURVECTORBALLS_VB_MAX)                ? p_nbr_pieds:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_jambes_anterieures=  (p_nbr_jambes_anterieures<=DKSJONGLEURVECTORBALLS_VB_MAX)   ? p_nbr_jambes_anterieures:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_jambes=              (p_nbr_jambes<=DKSJONGLEURVECTORBALLS_VB_MAX)               ? p_nbr_jambes:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_abdomen=             (p_nbr_abdomen<=DKSJONGLEURVECTORBALLS_VB_MAX)              ? p_nbr_abdomen:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_thorax=              (p_nbr_thorax<=DKSJONGLEURVECTORBALLS_VB_MAX)               ? p_nbr_thorax:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_cou=                 (p_nbr_cou<=DKSJONGLEURVECTORBALLS_VB_MAX)                  ? p_nbr_cou:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_bras=                (p_nbr_bras<=DKSJONGLEURVECTORBALLS_VB_MAX)                 ? p_nbr_bras:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_avant_bras=          (p_nbr_avant_bras<=DKSJONGLEURVECTORBALLS_VB_MAX)           ? p_nbr_avant_bras:DKSJONGLEURVECTORBALLS_VB_MAX;
            nbrVectorBalls_mains=               (p_nbr_mains<=DKSJONGLEURVECTORBALLS_VB_MAX)                ? p_nbr_mains:DKSJONGLEURVECTORBALLS_VB_MAX;

            //Répartition des indices:
            uint16_t indice=0;
            uint16_t i;
            for(i=0;i<nbrVectorBalls_pieds*2;i++) indices_pieds[i]=indice++;
            for(i=0;i<nbrVectorBalls_jambes_anterieures*2;i++) indices_jambes_anterieures[i]=indice++;
            for(i=0;i<nbrVectorBalls_jambes*2;i++) indices_jambes[i]=indice++;
            indice_hanche_droite=indice++;indice_hanche_gauche=indice++;
            for(i=0;i<nbrVectorBalls_abdomen;i++) indices_abdomen[i]=indice++;
            for(i=0;i<nbrVectorBalls_thorax;i++) indices_thorax[i]=indice++;
            for(i=0;i<nbrVectorBalls_cou;i++) indices_cou[i]=indice++;
            indice_epaule_droite=indice++;
            indice_epaule_gauche=indice++;
            for(i=0;i<nbrVectorBalls_bras*2;i++) indices_bras[i]=indice++;
            for(i=0;i<nbrVectorBalls_avant_bras*2;i++) indices_avant_bras[i]=indice++;
            for(i=0;i<nbrVectorBalls_mains*2;i++) indices_mains[i]=indice++;
            indice_tete=indice++;
            indice_oeil_droit=indice++;
            indice_oeil_gauche=indice++;

            indice_balles=indice;

            maj_echelles_vectorballs();



            return true;
        }

    //------------------------------------
    //      Mise à l'échelle des sphères:
    //------------------------------------
    private:
        void maj_echelles_vectorballs()
        {
            uint16_t i;
            dksSphere* sphere;
            float r;

            for(i=0;i<nbrVectorBalls_pieds;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_pieds[0],rayons_vectorBalls_pieds[1],1./nbrVectorBalls_pieds*i);
                vectorBalls[indices_pieds[i]]->determine_rayon(r);
                vectorBalls[indices_pieds[i+nbrVectorBalls_pieds]]->determine_rayon(r);
            }

            for(i=0;i<nbrVectorBalls_jambes_anterieures;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_jambes_anterieures[0],rayons_vectorBalls_jambes_anterieures[1],1./nbrVectorBalls_jambes_anterieures*i);
                vectorBalls[indices_jambes_anterieures[i]]->determine_rayon(r);
                vectorBalls[indices_jambes_anterieures[i+nbrVectorBalls_jambes_anterieures]]->determine_rayon(r);
            }

            for(i=0;i<nbrVectorBalls_jambes;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_jambes[0],rayons_vectorBalls_jambes[1],1./nbrVectorBalls_jambes*i);
                vectorBalls[indices_jambes[i]]->determine_rayon(r);
                vectorBalls[indices_jambes[i+nbrVectorBalls_jambes]]->determine_rayon(r);
            }

            vectorBalls[indice_hanche_droite]->determine_rayon(rayons_vectorBalls_hanches);
            vectorBalls[indice_hanche_gauche]->determine_rayon(rayons_vectorBalls_hanches);

            for(i=0;i<nbrVectorBalls_abdomen;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_abdomen[0],rayons_vectorBalls_abdomen[1],1./nbrVectorBalls_abdomen*i);
                vectorBalls[indices_abdomen[i]]->determine_rayon(r);
            }

            for(i=0;i<nbrVectorBalls_thorax;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_thorax[0],rayons_vectorBalls_thorax[1],1./nbrVectorBalls_thorax*i);
                vectorBalls[indices_thorax[i]]->determine_rayon(r);
            }

            for(i=0;i<nbrVectorBalls_cou;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_cou[0],rayons_vectorBalls_cou[1],1./nbrVectorBalls_cou*i);
                vectorBalls[indices_cou[i]]->determine_rayon(r);
            }

            vectorBalls[indice_epaule_droite]->determine_rayon(rayons_vectorBalls_epaules);
            vectorBalls[indice_epaule_gauche]->determine_rayon(rayons_vectorBalls_epaules);

            for(i=0;i<nbrVectorBalls_bras;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_bras[0],rayons_vectorBalls_bras[1],1./nbrVectorBalls_bras*i);
                vectorBalls[indices_bras[i]]->determine_rayon(r);
                vectorBalls[indices_bras[i+nbrVectorBalls_bras]]->determine_rayon(r);
            }
            for(i=0;i<nbrVectorBalls_avant_bras;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_avant_bras[0],rayons_vectorBalls_avant_bras[1],1./nbrVectorBalls_avant_bras*i);
                vectorBalls[indices_avant_bras[i]]->determine_rayon(r);
                vectorBalls[indices_avant_bras[i+nbrVectorBalls_avant_bras]]->determine_rayon(r);
            }

            for(i=0;i<nbrVectorBalls_mains;i++)
            {
                r=Interpolations::interpolation_lineaire_f(rayons_vectorBalls_mains[0],rayons_vectorBalls_mains[1],1./nbrVectorBalls_mains*i);
                vectorBalls[indices_mains[i]]->determine_rayon(r);
                vectorBalls[indices_mains[i+nbrVectorBalls_mains]]->determine_rayon(r);
            }

            vectorBalls[indice_tete]->determine_rayon(rayon_vectorBall_tete);
            vectorBalls[indice_oeil_droit]->determine_rayon(rayon_vectorBall_oeil_droit);
            vectorBalls[indice_oeil_gauche]->determine_rayon(rayon_vectorBall_oeil_gauche);
        }

        //-----------------------------------------
        //      Mise à jour des couleurs
        //-----------------------------------------
        void maj_couleurs_vectorBalls()
        {
            uint16_t i;
            for(i=0;i<nbrVectorBalls_pieds;i++)
            {
                vectorBalls[indices_pieds[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_pieds,float(i)/(nbrVectorBalls_pieds-1)));
                vectorBalls[indices_pieds[i+nbrVectorBalls_pieds]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_pieds,float(i)/(nbrVectorBalls_pieds-1)));
            }
            for(i=0;i<nbrVectorBalls_jambes_anterieures;i++)
            {
                vectorBalls[indices_jambes_anterieures[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_jambes_anterieures,float(i)/nbrVectorBalls_jambes_anterieures));
                vectorBalls[indices_jambes_anterieures[i+nbrVectorBalls_jambes_anterieures]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_jambes_anterieures,float(i)/nbrVectorBalls_jambes_anterieures));
            }
            for(i=0;i<nbrVectorBalls_jambes;i++)
            {
                vectorBalls[indices_jambes[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_jambes,float(i)/nbrVectorBalls_jambes));
                vectorBalls[indices_jambes[i+nbrVectorBalls_jambes]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_jambes,float(i)/nbrVectorBalls_jambes));
            }

            vectorBalls[indice_hanche_droite]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_hanches));
            vectorBalls[indice_hanche_gauche]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_hanches));

            for(i=0;i<nbrVectorBalls_abdomen;i++)
            {
                vectorBalls[indices_abdomen[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_abdomen,float(i)/nbrVectorBalls_abdomen));
            }
            for(i=0;i<nbrVectorBalls_thorax;i++)
            {
                vectorBalls[indices_thorax[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_thorax,float(i)/nbrVectorBalls_thorax));
            }
            for(i=0;i<nbrVectorBalls_cou;i++)
            {
                vectorBalls[indices_cou[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_cou,float(i)/nbrVectorBalls_cou));
            }

            vectorBalls[indice_epaule_droite]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_epaules));
            vectorBalls[indice_epaule_gauche]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_epaules));

            for(i=0;i<nbrVectorBalls_bras;i++)
            {
                vectorBalls[indices_bras[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_bras,float(i)/nbrVectorBalls_bras));
                vectorBalls[indices_bras[i+nbrVectorBalls_bras]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_bras,float(i)/nbrVectorBalls_bras));
            }
            for(i=0;i<nbrVectorBalls_avant_bras;i++)
            {
                vectorBalls[indices_avant_bras[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_avant_bras,float(i)/nbrVectorBalls_avant_bras));
                vectorBalls[indices_avant_bras[i+nbrVectorBalls_avant_bras]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_avant_bras,float(i)/nbrVectorBalls_avant_bras));
            }
            for(i=0;i<nbrVectorBalls_mains;i++)
            {
                vectorBalls[indices_mains[i]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_mains,float(i)/(nbrVectorBalls_mains-1)));
                vectorBalls[indices_mains[i+nbrVectorBalls_mains]]->determine_couleur(OrganisationCouleurs::renvoie_couleur_interpolee_3f_uint32(couleur_mains,float(i)/(nbrVectorBalls_mains-1)));
            }

            vectorBalls[indice_oeil_droit]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_oeil_droit));
            vectorBalls[indice_oeil_gauche]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_oeil_gauche));
            vectorBalls[indice_tete]->determine_couleur(OrganisationCouleurs::convertion_3f_uint32(couleur_tete));

        }

        //------------------------------------
        // Mise à l'échelle géométrique
        //------------------------------------
    public:
        virtual void determine_echelle_geometrique(double p_echelle)
            {
                double echelle_prec=echelle_geometrique;

                this->dksJongleur::determine_echelle_geometrique(p_echelle);

                for(uint16_t i=0;i<nbrVectorBalls;i++)
                {
                    vectorBalls[i]->determine_echelle_geometrique(echelle_geometrique);
                }
            }

        //-----------------------------------------
        //      Détermine la séquence de jonglage
        //-----------------------------------------
        virtual void determine_siteswap(uint8_t p_siteswap)
        {
            this->dksJongleur::determine_siteswap(p_siteswap);
            if(nbr_balles<nbrBalles_max)
            {
                for(uint16_t i=0;i<nbrBalles_max;i++)
                {
                    if (i<nbr_balles) vectorBalls[indice_balles+i]->drapeau_affiche=true;
                    else vectorBalls[indice_balles+i]->drapeau_affiche=false;
                }
            }
        }

        //------------------------------------------
        //      Rayons des balles de jonglage
        //------------------------------------------
        void determine_rayon_balles(float r)
        {
            for(uint16_t i=0;i<nbrBalles_max;i++)
            {
                balles[i].rayon=r;
                vectorBalls[indice_balles+i]->determine_rayon(r);
            }
        }

        void maj_couleurs_balles()
        {
            for(uint16_t i=0;i<nbrBalles_max;i++)
            {
                balles[i].couleur=OrganisationCouleurs::convertion_3f_uint32(couleurs_balles[i]);
                vectorBalls[indice_balles+i]->determine_couleur(balles[i].couleur);
            }
        }


    //====================================================================================
    //              Affichages
    //====================================================================================
    public:
        virtual void maj_jongleur(double t)
        {
            this->dksJongleur::maj_jongleur(t);
            uint16_t i;
            double px,py,pz;
            double *pa,*pb;

            place_partie_entiere(PERSONNAGE_PLANTE_DROITE,PERSONNAGE_TALON_DROIT,nbrVectorBalls_pieds,indices_pieds);
            place_partie_entiere(PERSONNAGE_PLANTE_GAUCHE,PERSONNAGE_TALON_GAUCHE,nbrVectorBalls_pieds,&indices_pieds[nbrVectorBalls_pieds]);
            place_partie(PERSONNAGE_CHEVILLE_DROITE,PERSONNAGE_GENOU_DROIT,nbrVectorBalls_jambes_anterieures,indices_jambes_anterieures);
            place_partie(PERSONNAGE_CHEVILLE_GAUCHE,PERSONNAGE_GENOU_GAUCHE,nbrVectorBalls_jambes_anterieures,&indices_jambes_anterieures[nbrVectorBalls_jambes_anterieures]);

            place_partie(PERSONNAGE_GENOU_DROIT,PERSONNAGE_HANCHE_DROITE,nbrVectorBalls_jambes,indices_jambes);
            place_partie(PERSONNAGE_GENOU_GAUCHE,PERSONNAGE_HANCHE_GAUCHE,nbrVectorBalls_jambes,&indices_jambes[nbrVectorBalls_jambes]);
            vectorBalls[indice_hanche_droite]->determine_position(&squelette[PERSONNAGE_HANCHE_DROITE*3]);
            vectorBalls[indice_hanche_gauche]->determine_position(&squelette[PERSONNAGE_HANCHE_GAUCHE*3]);
            place_partie(PERSONNAGE_ARTICULATION_ABDOMEN,PERSONNAGE_ARTICULATION_THORAX,nbrVectorBalls_abdomen,indices_abdomen);
            place_partie(PERSONNAGE_ARTICULATION_THORAX,PERSONNAGE_ARTICULATION_COU,nbrVectorBalls_thorax,indices_thorax);
            place_partie(PERSONNAGE_ARTICULATION_COU,PERSONNAGE_ARTICULATION_TETE,nbrVectorBalls_cou,indices_cou);
            vectorBalls[indice_epaule_droite]->determine_position(&squelette[PERSONNAGE_EPAULE_DROITE*3]);
            vectorBalls[indice_epaule_gauche]->determine_position(&squelette[PERSONNAGE_EPAULE_GAUCHE*3]);

            place_partie(PERSONNAGE_COUDE_DROIT,PERSONNAGE_EPAULE_DROITE,nbrVectorBalls_bras,indices_bras);
            place_partie(PERSONNAGE_COUDE_GAUCHE,PERSONNAGE_EPAULE_GAUCHE,nbrVectorBalls_bras,&indices_bras[nbrVectorBalls_bras]);
            place_partie(PERSONNAGE_POIGNET_DROIT,PERSONNAGE_COUDE_DROIT,nbrVectorBalls_avant_bras,indices_avant_bras);
            place_partie(PERSONNAGE_POIGNET_GAUCHE,PERSONNAGE_COUDE_GAUCHE,nbrVectorBalls_avant_bras,&indices_avant_bras[nbrVectorBalls_avant_bras]);

            place_partie_entiere(PERSONNAGE_MAIN_DROITE,PERSONNAGE_POIGNET_DROIT,nbrVectorBalls_mains,indices_mains);
            place_partie_entiere(PERSONNAGE_MAIN_GAUCHE,PERSONNAGE_POIGNET_GAUCHE,nbrVectorBalls_mains,&indices_mains[nbrVectorBalls_mains]);
            vectorBalls[indice_oeil_droit]->determine_position(&squelette[PERSONNAGE_OEIL_DROIT*3]);
            vectorBalls[indice_oeil_gauche]->determine_position(&squelette[PERSONNAGE_OEIL_GAUCHE*3]);
            vectorBalls[indice_tete]->determine_position(&squelette[PERSONNAGE_ARTICULATION_TETE*3]);


            for(i=0;i<nbr_balles;i++) vectorBalls[indice_balles+i]->determine_position(balles[i].position);


        }

    private:

    void place_partie_entiere(uint16_t a0,uint16_t a1, uint16_t nbr,uint16_t *indices)
    {
            double *pa=&squelette[a0*3];
            double *pb=&squelette[a1*3];
            double px=(pb[0]-pa[0])/(nbr-1);
            double py=(pb[1]-pa[1])/(nbr-1);
            double pz=(pb[2]-pa[2])/(nbr-1);
            for(uint16_t i=0;i<nbr;i++) vectorBalls[indices[i]]->determine_position(pa[0]+px*i,pa[1]+py*i,pa[2]+pz*i);
    }

    void place_partie(uint16_t a0,uint16_t a1, uint16_t nbr,uint16_t *indices)
    {
            double *pa=&squelette[a0*3];
            double *pb=&squelette[a1*3];
            double px=(pb[0]-pa[0])/(nbr);
            double py=(pb[1]-pa[1])/(nbr);
            double pz=(pb[2]-pa[2])/(nbr);
            for(uint16_t i=0;i<nbr;i++) vectorBalls[indices[i]]->determine_position(pa[0]+px*i,pa[1]+py*i,pa[2]+pz*i);
    }




};


#endif // DKSJONGLEURVECTORBALLS_H_INCLUDED
