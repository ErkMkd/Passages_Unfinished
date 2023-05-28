#ifndef DKSPARTICULESVOLUMES_H_INCLUDED
#define DKSPARTICULESVOLUMES_H_INCLUDED

/*
*
*       A chaque point est associé un volume
*
*/

#include "dksParticules.h"

#include <vector>

//-------------------------------------------------------
//Structure de gestion des paramètres des volumes:
//-------------------------------------------------------
class Particule_Volume : public Element_particule
{
    public:
    float roulis;   //Angles en radians
    float tangage;
    float lacet;
    Bruit* perturbation_roulis;
    Bruit* perturbation_tangage;
    Bruit* perturbation_lacet;
    double amplitude_perlin_RTL;
    double intervalle_perlin_RTL;
    float vitesse_roulis;
    float vitesse_tangage;
    float vitesse_lacet;
    float echelle_x;
    float echelle_y;
    float echelle_z;
    dksVolume* volume;

    Particule_Volume(uint32_t index):Element_particule(index)
    {
        roulis=0.;
        tangage=0.;
        lacet=0.;
        vitesse_roulis=M_PI/180.*15.;
        vitesse_tangage=M_PI/180.*15.;
        vitesse_lacet=M_PI/180.*15.;
        echelle_x=1.;
        echelle_y=1.;
        echelle_z=1.;
        volume=NULL;

        perturbation_roulis=NULL;
        perturbation_tangage=NULL;
        perturbation_lacet=NULL;

    }

    ~Particule_Volume()
    {
        if (perturbation_roulis!=NULL)delete perturbation_roulis;
        if (perturbation_tangage!=NULL)delete perturbation_tangage;
        if (perturbation_lacet!=NULL)delete perturbation_lacet;
    }

    //------------ Fonctions de gestion de la particule:

    void init_perturbations_RTL()
    {
        perturbation_roulis=new Bruit(); //Prévoir les erreurs...
        perturbation_tangage=new Bruit();
        perturbation_lacet=new Bruit();
        amplitude_perlin_RTL=2.;
        intervalle_perlin_RTL=.1;

    }

    void maj_RTL(float delta_t)
    {
        roulis+=vitesse_roulis*delta_t;
        tangage+=vitesse_tangage*delta_t;
        lacet+=vitesse_lacet*delta_t;
        if (volume!=NULL) volume->determine_RTL(roulis,tangage,lacet);
    }

    void maj_echelle()
    {
        if (volume!=NULL) volume->determine_echelle_repere(double(echelle_x),double(echelle_y),double(echelle_z));
    }
};

//===========================================================
//          Gestion des particules de volumes
//===========================================================
class dksParticulesVolumes : public dksParticules
{
    protected:

    //============================================
    //              Constructeur
    //============================================
    public:
    dksParticulesVolumes(char* p_nom="Particules",
                    double px=0.,double py=0.,double pz=0., //Centre du champ
                    dksVolume** p_volumes=NULL,
                    uint32_t p_nbrVolumes=0,
                    dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                    ):dksParticules(p_nom,px,py,pz,0,axe1,axe2,axe3)
    {
        if (!determine_volumes(p_volumes,p_nbrVolumes))
        {
            cout<<"ERREUR dans dksParticulesVolumes() - Impossible de créer les particules"<<endl;
        }
    }

    ~dksParticulesVolumes()
    {

    }

    //------------------------------------------------
    //      Gestion des volumes
    //      Les indices des volumes dans la table
    //      correspondent aux indices des points.
    //------------------------------------------------

    virtual void determine_drapeau_affiche(bool d)
    {
        drapeau_affiche=d;
        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*)particules[i];
            particule->volume->determine_drapeau_affiche(d);
        }
    }

    bool determine_volumes(dksVolume** p_volumes, uint32_t p_nbrVolumes)
    {
        if (!determine_nombre_de_points(p_nbrVolumes)) return false;

        if (p_volumes!=NULL)
        {
            for (uint32_t i=0 ;i<nbrPoints;i++)
            {
                Particule_Volume* particule=(Particule_Volume*)particules[i];
                particule->volume=p_volumes[i];
            }
        }
        return true;
    }

    //--------------------------------------------
    //      Gestion des particules
    //--------------------------------------------
    virtual void creation_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=new Particule_Volume(i);
            particules[i]=particule;
        }
    }

    void random_orientations()
    {
        uint32_t i;
        for (i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*)particules[i];
            particule->roulis=float(rand())/32768.*M_PI*2.;
            particule->tangage=float(rand())/32768.*M_PI*2.;
            particule->lacet=float(rand())/32768.*M_PI*2.;

        }
    }

    void random_rotations(float vmin,float vmax)
    {
        uint32_t i;
        float vlim=vmax-vmin;
        for (i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*)particules[i];
            particule->vitesse_roulis=vmin+float(rand())/32768.*vlim;
            particule->vitesse_tangage=vmin+float(rand())/32768.*vlim;
            particule->vitesse_lacet=vmin+float(rand())/32768.*vlim;

        }
    }

    void determine_vitesses_rotations(float roulis,float tangage,float lacet)
    {
        uint32_t i;
        for (i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*)particules[i];
            particule->vitesse_roulis=roulis;
            particule->vitesse_tangage=tangage;
            particule->vitesse_lacet=lacet;
        }
    }

    void determine_vitesse_rotation_particule(uint32_t indice,float roulis,float tangage,float lacet)
    {
        if(indice>=nbrPoints)return;
        Particule_Volume* particule=(Particule_Volume*)particules[indice];
        particule->vitesse_roulis=roulis;
        particule->vitesse_tangage=tangage;
        particule->vitesse_lacet=lacet;

    }



    //--------------------------------------------
    //      Gestion des mouvements des volumes
    //--------------------------------------------

    virtual void maj_volumes()
    {
        uint32_t i;

        for(i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->maj_RTL(float(delta_t));
            particule->maj_echelle();
            if (particule->volume!=NULL)particule->volume->determine_position(&points[i*3]);
        }
    }

    //-------------------------------------------------------------------
    //  Sert le temps d'ajuster manuellement les positions des volumes
    //-------------------------------------------------------------------
    virtual void maj_volumes_edition()
    {
        uint32_t i;

        for(i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->maj_RTL(float(delta_t));
            particule->maj_echelle();
            if (particule->volume!=NULL)
            {
                points[i*3]=particule->volume->x;
                points[i*3+1]=particule->volume->y;
                points[i*3+2]=particule->volume->z;
                //particule->volume->determine_position(&points[i*3]);
            }
        }
    }

    //-------------------------------------------------------------------
    //  Gestion des perturbations de Perlin
    //-------------------------------------------------------------------
    void init_perturbations_Perlin()
    {
        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->init_perturbations_RTL();
        }
    }

    void maj_perturbations_RTL_relatives()
    {
        /*
        double px,py,pz;
            for (uint32_t i=0;i<nbrPoints;i++)
                {
                    Particule_Volume* particule=(Particule_Volume*) particules[i];
                    px=particule->amplitude_perlin*(particule->perturbation_x->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    py=particule->amplitude_perlin*(particule->perturbation_y->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    pz=particule->amplitude_perlin*(particule->perturbation_z->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    points[3*i]+=(px+particule->vitesse[0])*delta_t;
                    points[3*i+1]+=(py+particule->vitesse[1])*delta_t;
                    points[3*i+2]+=(pz+particule->vitesse[2])*delta_t;
                }
                */

    }

    //---------------------------------------
    //  Pas d'affichage.
    //---------------------------------------

    virtual void affiche(dksOeil* oeil)
    {

    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {

    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {
    }

    //===========================================================================================
    //      Gestion des fichiers
    //===========================================================================================
    bool sauve_orientations(char* nom_fichier)
    {
        float* orientations=(float*)malloc(nbrPoints*3*sizeof(float));
        if(orientations==NULL)return false;

        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            orientations[3*i]=particule->roulis;
            orientations[3*i+1]=particule->tangage;
            orientations[3*i+2]=particule->lacet;
            //cout<<orientations[3*i]<<" "<<orientations[3*i+1]<<" "<<orientations[3*i+2]<<endl;
        }
        bool d=true;
        if(!Fichiers::sauve_donnees_binaires((char*)orientations,nbrPoints*3*sizeof(float),nom_fichier)) d=false;
        free(orientations);
        return d;

    }

    ///Il faut que les points aient déjà été alloués!
    bool charge_orientations(char* nom_fichier)
    {
        //cout<<"CHARGEMENT"<<endl<<endl;
        //float* orientations=(float*)malloc(nbrPoints*3*sizeof(float));
        float* orientations=(float*)Fichiers::charge_donnees_binaires_allocation(nom_fichier);
        if (orientations==NULL) return false;
        /*
        if(Fichiers::charge_donnees_binaires((char*)orientations,sizeof(orientations),nom_fichier)==-1)
        {
            free (orientations);
            return false;
        }
        */

        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->roulis=orientations[3*i];
            particule->tangage=orientations[3*i+1];
            particule->lacet=orientations[3*i+2];
            //cout<<particule->roulis<<" "<<particule->tangage<<" "<<particule->lacet<<endl;
        }
        free(orientations);
        a_ete_deplace();
        return true;
    }

    bool sauve_rotations(char* nom_fichier)
    {
        float* rotations=(float*)malloc(nbrPoints*3*sizeof(float));
        if(rotations==NULL)return false;

        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            rotations[3*i]=particule->vitesse_roulis;
            rotations[3*i+1]=particule->vitesse_tangage;
            rotations[3*i+2]=particule->vitesse_lacet;
            //cout<<orientations[3*i]<<" "<<orientations[3*i+1]<<" "<<orientations[3*i+2]<<endl;
        }
        bool d=true;
        if(!Fichiers::sauve_donnees_binaires((char*)rotations,nbrPoints*3*sizeof(float),nom_fichier)) d=false;
        free(rotations);
        return d;

    }

    ///Il faut que les points aient déjà été alloués!
    bool charge_rotations(char* nom_fichier)
    {
        //cout<<"CHARGEMENT"<<endl<<endl;
        //float* orientations=(float*)malloc(nbrPoints*3*sizeof(float));
        float* rotations=(float*)Fichiers::charge_donnees_binaires_allocation(nom_fichier);
        if (rotations==NULL) return false;
        /*
        if(Fichiers::charge_donnees_binaires((char*)orientations,sizeof(orientations),nom_fichier)==-1)
        {
            free (orientations);
            return false;
        }
        */

        for(uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->vitesse_roulis=rotations[3*i];
            particule->vitesse_tangage=rotations[3*i+1];
            particule->vitesse_lacet=rotations[3*i+2];
            //cout<<particule->roulis<<" "<<particule->tangage<<" "<<particule->lacet<<endl;
        }
        free(rotations);
        return true;
    }


};

#endif // DKSPARTICULESVOLUMES_H_INCLUDED
