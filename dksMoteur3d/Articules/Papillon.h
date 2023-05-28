#ifndef PAPILLON_H_INCLUDED
#define PAPILLON_H_INCLUDED

/*
        Papillon animé
*/

#include "dksInsecte.h"
#include "../LWO/dksChargeLWO2.h"
#include "../LWO/dksConvertionLWO2.h"
#include "../../Utiles/Interpolations.h"
#include "../../Utiles/Bruit.h"

//--------------- Erreurs:
#define PAPILLON_OK 0
#define PAPILLON_ERREUR_LWO2 1

//-------------- Parametres:
#define PAPILLON_NBR_VOLUMES 31 //Le nombre de volumes qui constituent le papillon

//==============================================
//      Classe de définition des pattes:
//==============================================
class patte_papillon
{
  public:
    dksVolume* coxa;
    dksVolume* femur;
    dksVolume* tibia;
    dksVolume* tarse;
    uint16_t id;    //L'identifiant de la patte
    Patte_Insecte* squelette;

    void initialise_volumes(dksVolume* p_coxa, dksVolume* p_femur, dksVolume* p_tibia, dksVolume* p_tarse, Patte_Insecte* p_squelette)
    {
        coxa=p_coxa;
        femur=p_femur;
        tibia=p_tibia;
        tarse=p_tarse;
        squelette=p_squelette;
    }

};

//=======================================================================
//          Définition de la classe
/// Pour afficher le papillon, il faut ajouter tout les volumes
/// qui le constitue dans la scène.
//=======================================================================
class Papillon: public dksInsecte
{
    private:
        dksChargeLWO2* volume_LWO2;
        dksConvertionLWO2* volumes_dks;
        dksAxeRotation axe_volume;
        double axe1[3];
        double axe2[3];
    public:

        dksVolume* corps;
        dksVolume* aile_avant_droite;
        dksVolume* aile_avant_gauche;
        dksVolume* aile_arriere_droite;
        dksVolume* aile_arriere_gauche;
        dksVolume* antenne_droite;
        dksVolume* antenne_gauche;
        patte_papillon volumes_pattes[6];

        dksVolume* volumes[PAPILLON_NBR_VOLUMES]; //La tableau des volumes du papillon, pour ajouter ses volumes à une scène

        //Parametres d'animation:
        Interpolation_Parametre* interpolation_ailes_avants;
        Interpolation_Parametre* interpolation_ailes_arrieres;
        Interpolation_Parametre_2f* interpolation_pattes_femurs;    //Angles axe et rotation
        Interpolation_Parametre* interpolation_pattes_tibias;

        Bruit bruit_Aavd;   //Tremblement des ailes lors du vol plané
        Bruit bruit_Aavg;
        Bruit bruit_Aard;
        Bruit bruit_Aarg;


        //Gestion de l'animation des ailes:
        #define DKSPAPILLON_PHASE_AILE_REPOS 0    //ailes au repos
        #define DKSPAPILLON_PHASE_AILE_DESCEND 1
        #define DKSPAPILLON_PHASE_AILE_MONTE 2
        #define DKSPAPILLON_PHASE_AILE_RETOUR 3 //Retour en pause
        bool drapeau_vol;   //True tant que dure le battement.
        uint16_t phase_ailes;   //Identifiant de la phase d'animation des ailes
        float vol_duree_descente; //Durées des phases
        float vol_duree_montee;
        float vol_duree_retour;
        float vol_angle_repos;  //Angles max des différentes phases
        float vol_angle_descente;
        float vol_angle_montee;
        float vol_amplitude_corps;  //Amplitude du mouvement rétroactif du battement des ailes sur le corps

        //Gestion du vol plané:
        //L'identifiant "phase_ailes" sert à indiquer la phase du vol plané, puisque celui-ci court-circuite le vol
        #define DKSPAPILLON_PHASE_PLANE_VOL 4
        #define DKSPAPILLON_PHASE_PLANE_POSITIONNEMENT 5
        #define DKSPAPILLON_PHASE_PLANE_RETOUR 6
        bool drapeau_vol_plane;
        float vol_plane_angle_avant;
        float vol_plane_angle_arriere;
        float vol_plane_duree_positionnement;
        float vol_plane_duree_retour;
        float amplitude_bruit_Aav;
        float amplitude_bruit_Aar;

        //Gestion de l'animation des pattes:
        #define DKSPAPILLON_PHASE_PATTES_REPOS 0    //Pattes en repos (debout posé)
        #define DKSPAPILLON_PHASE_PATTES_ENVOL 1    //Repliage des pattes lors de l'envol
        #define DKSPAPILLON_PHASE_PATTES_VOL 2      //Pattes en position vol
        #define DKSPAPILLON_PHASE_PATTES_RETOUR_REPOS 3   //Pattes en retour vers la position de repos
        bool drapeau_pattes_position_vol;
        uint16_t phase_pattes;
        float pattes_duree_envol;
        float pattes_duree_retour_repos;
        float pattes_angle_repos_Fa;
        float pattes_angle_repos_Fr;
        float pattes_angle_repos_Ta;
        float pattes_angle_vol_Fa;
        float pattes_angle_vol_Fr;
        float pattes_angle_vol_Ta;

        float pattes_amplitude_Fa;
        float pattes_amplitude_Fr;
        float pattes_amplitude_Ta;
        float v_corps;  //Vitesse de déplacement inertiel du corps
        float pos_corps;    //Position du corps lors du vol (varie entre 0 et 1)
        float pos_corps_precedant;  //La position verticale du corps à l'image précédante (entre 0 et 1). Sert pour le calcul de l'inertie des pattes et des antennes lors du vol.

        //Gestion de l'animation des antennes:
        float antennes_angle_repos_a;
        float antennes_amplitude_a;  //Inertie des antennes lors du vol


    //=============================
    //  Constructeur
    //=============================
    Papillon(char* p_nom, char* nom_fichier_LWO, char* chemin_images=NULL, double px=0.,double py=0.,double pz=0.):dksInsecte(p_nom,px,py,pz)
    {
        erreur=PAPILLON_OK;
        volumes_lwo2=NULL;
        volumes_convertis=NULL;
        interpolation_ailes_avants=NULL;
        interpolation_ailes_arrieres=NULL;
        interpolation_pattes_femurs=NULL;
        interpolation_pattes_tibias=NULL;

        volumes_lwo2=new dksChargeLWO2(nom_fichier_LWO,chemin_images);
        if(volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
        {
            erreur=PAPILLON_ERREUR_LWO2;
            return;
        }

        volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
        if(volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
        {
            erreur=PAPILLON_ERREUR_LWO2;
            return;
        }

        corps=volumes_convertis->renvoie_volume_nom("corps");
        aile_avant_droite=volumes_convertis->renvoie_volume_nom("aile_avant_droite");
        aile_avant_gauche=volumes_convertis->renvoie_volume_nom("aile_avant_gauche");
        aile_arriere_droite=volumes_convertis->renvoie_volume_nom("aile_arriere_droite");
        aile_arriere_gauche=volumes_convertis->renvoie_volume_nom("aile_arriere_gauche");
        antenne_droite=volumes_convertis->renvoie_volume_nom("antenne_droite");
        antenne_gauche=volumes_convertis->renvoie_volume_nom("antenne_gauche");
        volumes_pattes[INSECTE_PAVD].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_avant_droite"),
                                                        volumes_convertis->renvoie_volume_nom("femur_avant_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_avant_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_avant_droit"),&pattes[INSECTE_PAVD]);
        volumes_pattes[INSECTE_PAVG].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_avant_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("femur_avant_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_avant_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_avant_gauche"),&pattes[INSECTE_PAVG]);
        volumes_pattes[INSECTE_PMD].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_mediane_droite"),
                                                        volumes_convertis->renvoie_volume_nom("femur_median_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_median_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_median_droit"),&pattes[INSECTE_PMD]);
        volumes_pattes[INSECTE_PMG].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_mediane_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("femur_median_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_median_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_median_gauche"),&pattes[INSECTE_PMG]);
        volumes_pattes[INSECTE_PARD].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_arriere_droite"),
                                                        volumes_convertis->renvoie_volume_nom("femur_arriere_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_arriere_droit"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_arriere_droit"),&pattes[INSECTE_PARD]);
        volumes_pattes[INSECTE_PARG].initialise_volumes(volumes_convertis->renvoie_volume_nom("coxa_arriere_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("femur_arriere_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tibia_arriere_gauche"),
                                                        volumes_convertis->renvoie_volume_nom("tarse_arriere_gauche"),&pattes[INSECTE_PARG]);

        //------- Initialise la table des volumes:
        uint16_t p=0;
        volumes[p++]=corps;
        volumes[p++]=aile_avant_droite;
        volumes[p++]=aile_avant_gauche;
        volumes[p++]=aile_arriere_droite;
        volumes[p++]=aile_arriere_gauche;
        volumes[p++]=antenne_droite;
        volumes[p++]=antenne_gauche;

        volumes[p++]=volumes_pattes[INSECTE_PAVD].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PAVD].femur;
        volumes[p++]=volumes_pattes[INSECTE_PAVD].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PAVD].tarse;

        volumes[p++]=volumes_pattes[INSECTE_PAVG].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PAVG].femur;
        volumes[p++]=volumes_pattes[INSECTE_PAVG].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PAVG].tarse;

        volumes[p++]=volumes_pattes[INSECTE_PMD].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PMD].femur;
        volumes[p++]=volumes_pattes[INSECTE_PMD].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PMD].tarse;

        volumes[p++]=volumes_pattes[INSECTE_PMG].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PMG].femur;
        volumes[p++]=volumes_pattes[INSECTE_PMG].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PMG].tarse;

        volumes[p++]=volumes_pattes[INSECTE_PARD].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PARD].femur;
        volumes[p++]=volumes_pattes[INSECTE_PARD].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PARD].tarse;

        volumes[p++]=volumes_pattes[INSECTE_PARG].coxa;
        volumes[p++]=volumes_pattes[INSECTE_PARG].femur;
        volumes[p++]=volumes_pattes[INSECTE_PARG].tibia;
        volumes[p++]=volumes_pattes[INSECTE_PARG].tarse;

        //Hiérarchie:
        for(p=0;p<PAPILLON_NBR_VOLUMES;p++)
        {
            if(volumes[p]==corps) ajoute_enfant(volumes[p]);
            else
            {
                corps->ajoute_enfant(volumes[p]);
            }
            //cout<<"Position:"<<volumes[p]->nom<<" - ";
            //volumes[p]->affiche_position();
        }

        //Animations:
        #define FACTEUR_ANIM 3.
        drapeau_vol=false;
        vol_duree_descente=.75/FACTEUR_ANIM;
        vol_duree_montee=1./FACTEUR_ANIM;
        vol_duree_retour=.5/FACTEUR_ANIM;
        vol_angle_repos=85.;    //!!!Ajouter une phase si l'angle est différent de l'angle max de montée
        vol_angle_montee=85.;
        vol_angle_descente=-45.;
        vol_amplitude_corps=0.2;
        phase_ailes=DKSPAPILLON_PHASE_AILE_REPOS;
        Courbe_Bezier_Plan courbe_ailes_avant(0.,0., .25,0., 0.,.75, 1.,1.);
        Courbe_Bezier_Plan courbe_ailes_arriere(0.,0., .6,0., 0.,.4, 1.,1.);
        interpolation_ailes_avants=new Interpolation_Parametre(-1.,0.,vol_angle_repos,vol_duree_descente,INTERPOLATION_BEZIER,&courbe_ailes_avant);
        interpolation_ailes_arrieres=new Interpolation_Parametre(-1.,0.,vol_angle_repos,vol_duree_descente,INTERPOLATION_BEZIER,&courbe_ailes_arriere);
        angle_Aavd=vol_angle_repos;
        angle_Aavg=vol_angle_repos;
        angle_Aard=vol_angle_repos;
        angle_Aarg=vol_angle_repos;

        drapeau_vol_plane=false;
        vol_plane_duree_positionnement=0.5/FACTEUR_ANIM;
        vol_plane_duree_retour=0.5/FACTEUR_ANIM;
        vol_plane_angle_avant=0.;
        vol_plane_angle_arriere=10.;
        amplitude_bruit_Aav=3.;
        amplitude_bruit_Aar=2.;

        drapeau_pattes_position_vol=false;
        pattes_angle_repos_Fa=0.;
        pattes_angle_repos_Fr=0.;
        pattes_angle_repos_Ta=0.;
        pattes_angle_vol_Fa=-40.;
        pattes_angle_vol_Fr=30.;
        pattes_angle_vol_Ta=10.;
        pattes_duree_envol=0.5;
        pattes_duree_retour_repos=vol_duree_retour;
        phase_pattes=DKSPAPILLON_PHASE_PATTES_REPOS;
        float angles_femurs_depart[2]={pattes_angle_repos_Fa,pattes_angle_repos_Fr};
        float angles_femurs_fin[2]={pattes_angle_vol_Fa,pattes_angle_vol_Fr};
        interpolation_pattes_femurs=new Interpolation_Parametre_2f(-1.,angles_femurs_depart,angles_femurs_fin,pattes_duree_envol,INTERPOLATION_COSINUS);
        interpolation_pattes_tibias=new Interpolation_Parametre(-1.,pattes_angle_repos_Ta,pattes_angle_vol_Ta,pattes_duree_envol,INTERPOLATION_COSINUS);
        pattes_amplitude_Fa=70./FACTEUR_ANIM;
        pattes_amplitude_Fr=70./FACTEUR_ANIM;
        pattes_amplitude_Ta=50./FACTEUR_ANIM;

        antennes_angle_repos_a=0.;
        antennes_amplitude_a=200./FACTEUR_ANIM;
        v_corps=0.;

    }

    //==============================
    //      Destructeur
    //==============================
    ~Papillon()
    {
        if(volumes_convertis!=NULL) delete volumes_convertis;
        if(volumes_lwo2!=NULL) delete volumes_lwo2;
        if(interpolation_ailes_avants!=NULL) delete interpolation_ailes_avants;
        if(interpolation_ailes_arrieres!=NULL) delete interpolation_ailes_arrieres;
        if(interpolation_pattes_femurs!=NULL) delete interpolation_pattes_femurs;
        if(interpolation_pattes_tibias!=NULL) delete interpolation_pattes_tibias;

    }
    //====================================================================
    //      Gestion affichage
    //====================================================================
    public:
    virtual void determine_drapeau_affiche(bool d)
    {
        this->dksRendu::determine_drapeau_affiche(d);
        for(uint16_t i=0;i<PAPILLON_NBR_VOLUMES;i++)
        {
            volumes[i]->determine_drapeau_affiche(d);
        }
    }

    //====================================================================
    //      Gestion animation
    //====================================================================
    public:
    //------------------------------------------
    //      Vole:
    //------------------------------------------
    void demarre_vol()
    {
        drapeau_vol=true;
        pattes_position_vol();
    }

    void arrete_vol()
    {
        drapeau_vol=false;
        pattes_position_repos();
    }

    void demarre_vol_plane()
    {
        drapeau_vol_plane=true;
    }

    void arrete_vol_plane()
    {
        drapeau_vol_plane=false;
    }

    void maj_mouvements_ailes()
    {
        //Vol actif:
        if(drapeau_vol)
        {
            if(phase_ailes==DKSPAPILLON_PHASE_AILE_REPOS)
            {
                phase_ailes=DKSPAPILLON_PHASE_AILE_DESCEND;
                interpolation_ailes_avants->determine_parametres(-1.,vol_angle_repos,vol_angle_descente,vol_duree_descente);
                interpolation_ailes_arrieres->determine_parametres(-1.,vol_angle_repos,vol_angle_descente,vol_duree_descente);
            }
            else if(phase_ailes==DKSPAPILLON_PHASE_AILE_DESCEND)
            {
                angle_Aavd=interpolation_ailes_avants->renvoie_valeur_parametre(position_temps);
                angle_Aavg=angle_Aavd;
                angle_Aard=interpolation_ailes_arrieres->renvoie_valeur_parametre(position_temps);
                angle_Aarg=angle_Aard;
                if(interpolation_ailes_avants->drapeau_terminee)
                {
                    phase_ailes=DKSPAPILLON_PHASE_AILE_MONTE;
                    interpolation_ailes_avants->determine_parametres(-1.,vol_angle_descente,vol_angle_montee,vol_duree_montee);
                    interpolation_ailes_arrieres->determine_parametres(-1.,vol_angle_descente,vol_angle_montee,vol_duree_montee);
                }
            }
            else if(phase_ailes==DKSPAPILLON_PHASE_AILE_MONTE)
            {
                angle_Aavd=interpolation_ailes_avants->renvoie_valeur_parametre(position_temps);
                angle_Aavg=angle_Aavd;
                angle_Aard=interpolation_ailes_arrieres->renvoie_valeur_parametre(position_temps);
                angle_Aarg=angle_Aard;
                if(interpolation_ailes_avants->drapeau_terminee)
                {
                    if(drapeau_vol_plane)
                    {
                        phase_ailes=DKSPAPILLON_PHASE_PLANE_POSITIONNEMENT;
                        interpolation_ailes_avants->determine_parametres(-1.,vol_angle_montee,vol_plane_angle_avant,vol_plane_duree_positionnement);
                        interpolation_ailes_arrieres->determine_parametres(-1.,vol_angle_montee,vol_plane_angle_arriere,vol_plane_duree_positionnement);
                    }
                    else
                    {
                        phase_ailes=DKSPAPILLON_PHASE_AILE_DESCEND;
                        interpolation_ailes_avants->determine_parametres(-1.,vol_angle_montee,vol_angle_descente,vol_duree_descente);
                        interpolation_ailes_arrieres->determine_parametres(-1.,vol_angle_montee,vol_angle_descente,vol_duree_descente);
                    }
                }
            }
            else if(phase_ailes==DKSPAPILLON_PHASE_AILE_RETOUR)
            {
                phase_ailes=DKSPAPILLON_PHASE_AILE_DESCEND;
                interpolation_ailes_avants->determine_parametres(-1.,angle_Aavd,vol_angle_descente,vol_duree_descente);
                interpolation_ailes_arrieres->determine_parametres(-1.,angle_Aard,vol_angle_descente,vol_duree_descente);
            }
            //Gestion du vol plané:
            else
            {
                if(drapeau_vol_plane)
                {
                    if(phase_ailes==DKSPAPILLON_PHASE_PLANE_POSITIONNEMENT)
                    {
                        angle_Aavd=interpolation_ailes_avants->renvoie_valeur_parametre(position_temps);
                        angle_Aavg=angle_Aavd;
                        angle_Aard=interpolation_ailes_arrieres->renvoie_valeur_parametre(position_temps);
                        angle_Aarg=angle_Aard;
                        if(interpolation_ailes_avants->drapeau_terminee) phase_ailes=DKSPAPILLON_PHASE_PLANE_VOL;
                    }
                    else if(phase_ailes==DKSPAPILLON_PHASE_PLANE_VOL)
                    {
                        double perlin_Aavd=bruit_Aavd.valeur_aleatoire_temporelle(position_temps,0.1)*2.-1.;
                        double perlin_Aavg=bruit_Aavg.valeur_aleatoire_temporelle(position_temps,0.1)*2.-1.;
                        double perlin_Aard=bruit_Aard.valeur_aleatoire_temporelle(position_temps,0.1)*2.-1.;
                        double perlin_Aarg=bruit_Aarg.valeur_aleatoire_temporelle(position_temps,0.1)*2.-1.;
                        angle_Aavd=vol_plane_angle_avant+perlin_Aavd*amplitude_bruit_Aav;
                        angle_Aavg=vol_plane_angle_avant+perlin_Aavg*amplitude_bruit_Aav;
                        angle_Aard=vol_plane_angle_arriere+perlin_Aard*amplitude_bruit_Aar;
                        angle_Aarg=vol_plane_angle_arriere+perlin_Aarg*amplitude_bruit_Aar;
                    }
                    else if(phase_ailes==DKSPAPILLON_PHASE_PLANE_RETOUR)
                    {
                        phase_ailes=DKSPAPILLON_PHASE_PLANE_POSITIONNEMENT;
                        interpolation_ailes_avants->determine_parametres(-1.,angle_Aavd,vol_plane_angle_avant,vol_plane_duree_positionnement);
                        interpolation_ailes_arrieres->determine_parametres(-1.,angle_Aard,vol_plane_angle_arriere,vol_plane_duree_positionnement);
                    }
                }
                else
                {
                    if(phase_ailes==DKSPAPILLON_PHASE_PLANE_POSITIONNEMENT)
                    {
                        phase_ailes=DKSPAPILLON_PHASE_PLANE_RETOUR;
                        interpolation_ailes_avants->determine_parametres(-1.,angle_Aavd,vol_angle_descente,vol_plane_duree_retour);
                        interpolation_ailes_arrieres->determine_parametres(-1.,angle_Aard,vol_angle_descente,vol_plane_duree_retour);
                    }
                    else if(phase_ailes==DKSPAPILLON_PHASE_PLANE_VOL)
                    {
                        phase_ailes=DKSPAPILLON_PHASE_PLANE_RETOUR;
                        interpolation_ailes_avants->determine_parametres(-1.,angle_Aavd,vol_angle_descente,vol_plane_duree_retour);
                        interpolation_ailes_arrieres->determine_parametres(-1.,angle_Aard,vol_angle_descente,vol_plane_duree_retour);
                    }
                    else if(phase_ailes==DKSPAPILLON_PHASE_PLANE_RETOUR)
                    {
                        angle_Aavd=interpolation_ailes_avants->renvoie_valeur_parametre(position_temps);
                        angle_Aavg=angle_Aavd;
                        angle_Aard=interpolation_ailes_arrieres->renvoie_valeur_parametre(position_temps);
                        angle_Aarg=angle_Aard;
                        if(interpolation_ailes_avants->drapeau_terminee)
                        {
                            interpolation_ailes_avants->determine_parametres(-1.,vol_angle_descente,vol_angle_montee,vol_duree_montee);
                            interpolation_ailes_arrieres->determine_parametres(-1.,vol_angle_descente,vol_angle_montee,vol_duree_montee);
                            phase_ailes=DKSPAPILLON_PHASE_AILE_MONTE;
                        }
                    }
                }
            }
        }

        //Vol inactif, mais le mouvement n'est pas terminé
        else
        {
            if(drapeau_vol_plane)drapeau_vol_plane=false;

            if(phase_ailes==DKSPAPILLON_PHASE_AILE_REPOS)
            {
               return;
            }
            else if(phase_ailes==DKSPAPILLON_PHASE_AILE_RETOUR)
            {
                angle_Aavd=interpolation_ailes_avants->renvoie_valeur_parametre(position_temps);
                angle_Aavg=angle_Aavd;
                angle_Aard=interpolation_ailes_arrieres->renvoie_valeur_parametre(position_temps);
                angle_Aarg=angle_Aard;
                if(interpolation_ailes_avants->drapeau_terminee)
                {
                    phase_ailes=DKSPAPILLON_PHASE_AILE_REPOS;
                }

            }

            else
            {
                phase_ailes=DKSPAPILLON_PHASE_AILE_RETOUR;
                interpolation_ailes_avants->determine_parametres(-1.,angle_Aavd,vol_angle_repos,vol_duree_retour);
                interpolation_ailes_arrieres->determine_parametres(-1.,angle_Aard,vol_angle_repos,vol_duree_retour);
            }

        }
    }
    //---------------------------------
    //Mouvement du corps:
    //---------------------------------
    void maj_mouvements_corps()
    {
        float corps_y;

        if(phase_ailes==DKSPAPILLON_PHASE_AILE_REPOS) corps_y=0.;
        ///Il faudra différencier suivant les phases pour des mouvements plus fins
        else if(phase_ailes==DKSPAPILLON_PHASE_AILE_RETOUR)
        {
            corps_y=(1.-(angle_Aavd-vol_angle_descente)/(vol_angle_montee-vol_angle_descente))*vol_amplitude_corps;
        }
        else
        {
            corps_y=(1.-(angle_Aavd-vol_angle_descente)/(vol_angle_montee-vol_angle_descente))*vol_amplitude_corps;
        }

        corps->determine_position(0.,corps_y,0.);
        pos_corps=corps->y/vol_amplitude_corps;
        v_corps=pos_corps-pos_corps_precedant;
    }

    //------------------------------------------
    //      Pattes:
    //------------------------------------------
    //----- Replie les pattes lors du vol:
    void pattes_position_vol()
    {
        drapeau_pattes_position_vol=true;
    }

    //----- Ramène les pattes en position de repos (debout, posé)
    void pattes_position_repos()
    {
        drapeau_pattes_position_vol=false;
    }

    //----- Rafraichissement de l'animation des pattes:
    void maj_mouvements_pattes()
    {
        float femurs_angles_depart[2];
        float femurs_angles_fin[2];
        float femurs_angles[2];

        //Position vol activé:
        if(drapeau_pattes_position_vol)
        {
            if(phase_pattes==DKSPAPILLON_PHASE_PATTES_REPOS)
            {
                femurs_angles_depart={pattes_angle_repos_Fa,pattes_angle_repos_Fr};
                femurs_angles_fin={pattes_angle_vol_Fa,pattes_angle_vol_Fr};
                interpolation_pattes_femurs->determine_parametres(-1.,femurs_angles_depart,femurs_angles_fin,pattes_duree_envol);
                interpolation_pattes_tibias->determine_parametres(-1.,pattes_angle_repos_Ta,pattes_angle_vol_Ta,pattes_duree_envol);
                phase_pattes=DKSPAPILLON_PHASE_PATTES_ENVOL;
            }
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_ENVOL)
            {
                interpolation_pattes_femurs->renvoie_valeur_parametre(position_temps,femurs_angles);
                determine_angles_pattes(femurs_angles[0],femurs_angles[1],interpolation_pattes_tibias->renvoie_valeur_parametre(position_temps));
                if(interpolation_pattes_femurs->drapeau_terminee)
                {
                    phase_pattes=DKSPAPILLON_PHASE_PATTES_VOL;
                }
            }
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_VOL)
            {
                ///Inertie des pattes:

                determine_angles_pattes(pattes_angle_vol_Fa+pattes_amplitude_Fa*-v_corps,
                                        pattes_angle_vol_Fr+pattes_amplitude_Fr*v_corps,
                                        pattes_angle_vol_Ta+pattes_amplitude_Ta*-v_corps);
            }
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_RETOUR_REPOS)
            {
                femurs_angles_depart={pattes[0].angle_Fa,pattes[0].angle_Fr};
                femurs_angles_fin={pattes_angle_vol_Fa,pattes_angle_vol_Fr};
                interpolation_pattes_femurs->determine_parametres(-1.,femurs_angles_depart,femurs_angles_fin,pattes_duree_envol);
                interpolation_pattes_tibias->determine_parametres(-1.,pattes[0].angle_Ta,pattes_angle_vol_Ta,pattes_duree_envol);
                phase_pattes=DKSPAPILLON_PHASE_PATTES_ENVOL;
            }
        }

        //Position vol désactivée:
        else
        {
            if(phase_pattes==DKSPAPILLON_PHASE_PATTES_REPOS) return;
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_ENVOL)
            {
                femurs_angles_depart={pattes[0].angle_Fa,pattes[0].angle_Fr};
                femurs_angles_fin={pattes_angle_repos_Fa,pattes_angle_repos_Fr};
                interpolation_pattes_femurs->determine_parametres(-1.,femurs_angles_depart,femurs_angles_fin,pattes_duree_retour_repos);
                interpolation_pattes_tibias->determine_parametres(-1.,pattes[0].angle_Ta,pattes_angle_repos_Ta,pattes_duree_retour_repos);
                phase_pattes=DKSPAPILLON_PHASE_PATTES_RETOUR_REPOS;
            }
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_VOL)
            {
                femurs_angles_depart={pattes[0].angle_Fa,pattes[0].angle_Fr};
                femurs_angles_fin={pattes_angle_repos_Fa,pattes_angle_repos_Fr};
                interpolation_pattes_femurs->determine_parametres(-1.,femurs_angles_depart,femurs_angles_fin,pattes_duree_retour_repos);
                interpolation_pattes_tibias->determine_parametres(-1.,pattes[0].angle_Ta,pattes_angle_repos_Ta,pattes_duree_retour_repos);
                phase_pattes=DKSPAPILLON_PHASE_PATTES_RETOUR_REPOS;
            }
            else if(phase_pattes==DKSPAPILLON_PHASE_PATTES_RETOUR_REPOS)
            {
                interpolation_pattes_femurs->renvoie_valeur_parametre(position_temps,femurs_angles);
                determine_angles_pattes(femurs_angles[0],femurs_angles[1],interpolation_pattes_tibias->renvoie_valeur_parametre(position_temps));
                if(interpolation_pattes_femurs->drapeau_terminee)
                {
                    phase_pattes=DKSPAPILLON_PHASE_PATTES_REPOS;
                }
            }
        }
    }

    //---------------------------
    //  Antennes
    //---------------------------
    void maj_mouvements_antennes()
    {
        angle_AntAd=antennes_angle_repos_a+antennes_amplitude_a*-v_corps;
        angle_AntAg=angle_AntAd;
    }

    //--------------------------------------------------------
    //  Rafraichi l'animation
    //--------------------------------------------------------
    virtual void applique_mouvements()
    {
        this->dksConteneur::applique_mouvements();
        maj_mouvements_ailes();
        maj_mouvements_corps();
        maj_mouvements_pattes();
        maj_mouvements_antennes();
        maj_squelette();
        maj_volumes();
        pos_corps_precedant=pos_corps;
    }
    //--------------------------------------------------------
    //  Positionne les volumes par rapport au squelette
    //--------------------------------------------------------


    void maj_volumes()
    {
        uint16_t i;
        for(i=0;i<6;i++)
        {
            maj_volumes_patte(&volumes_pattes[i]);
        }

        //Rotation antenne droite:
        double* point1_a=&squelette_base[3*INSECTE_ANTENNE_DROITE];
        double* point2_a=&squelette_base[3*INSECTE_ANTENNE_FLAGELLE_DROITE];
        double* point1_b=&squelette[3*INSECTE_ANTENNE_DROITE];
        double* point2_b=&squelette[3*INSECTE_ANTENNE_FLAGELLE_DROITE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        antenne_droite->raz_axes();
        antenne_droite->rotation(axe_volume.angle,&axe_volume);

        //Rotation antenne gauche:
        point1_a=&squelette_base[3*INSECTE_ANTENNE_GAUCHE];
        point2_a=&squelette_base[3*INSECTE_ANTENNE_FLAGELLE_GAUCHE];
        point1_b=&squelette[3*INSECTE_ANTENNE_GAUCHE];
        point2_b=&squelette[3*INSECTE_ANTENNE_FLAGELLE_GAUCHE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        antenne_gauche->raz_axes();
        antenne_gauche->rotation(axe_volume.angle,&axe_volume);

        //Rotation aile avant droite:
        point1_a=&squelette_base[3*INSECTE_AILE_AVANT_DROITE];
        point2_a=&squelette_base[3*INSECTE_BOUT_AILE_AVANT_DROITE];
        point1_b=&squelette[3*INSECTE_AILE_AVANT_DROITE];
        point2_b=&squelette[3*INSECTE_BOUT_AILE_AVANT_DROITE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        aile_avant_droite->raz_axes();
        aile_avant_droite->rotation(axe_volume.angle,&axe_volume);

        //Rotation aile avant gauche:
        point1_a=&squelette_base[3*INSECTE_AILE_AVANT_GAUCHE];
        point2_a=&squelette_base[3*INSECTE_BOUT_AILE_AVANT_GAUCHE];
        point1_b=&squelette[3*INSECTE_AILE_AVANT_GAUCHE];
        point2_b=&squelette[3*INSECTE_BOUT_AILE_AVANT_GAUCHE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        aile_avant_gauche->raz_axes();
        aile_avant_gauche->rotation(axe_volume.angle,&axe_volume);

        //Rotation aile arriere droite:
        point1_a=&squelette_base[3*INSECTE_AILE_ARRIERE_DROITE];
        point2_a=&squelette_base[3*INSECTE_BOUT_AILE_ARRIERE_DROITE];
        point1_b=&squelette[3*INSECTE_AILE_ARRIERE_DROITE];
        point2_b=&squelette[3*INSECTE_BOUT_AILE_ARRIERE_DROITE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        aile_arriere_droite->raz_axes();
        aile_arriere_droite->rotation(axe_volume.angle,&axe_volume);

        //Rotation aile arriere gauche:
        point1_a=&squelette_base[3*INSECTE_AILE_ARRIERE_GAUCHE];
        point2_a=&squelette_base[3*INSECTE_BOUT_AILE_ARRIERE_GAUCHE];
        point1_b=&squelette[3*INSECTE_AILE_ARRIERE_GAUCHE];
        point2_b=&squelette[3*INSECTE_BOUT_AILE_ARRIERE_GAUCHE];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        aile_arriere_gauche->raz_axes();
        aile_arriere_gauche->rotation(axe_volume.angle,&axe_volume);

    }

    private:
    void maj_volumes_patte(patte_papillon* patte)
    {
        patte->femur->raz();
        patte->tibia->raz();
        patte->tarse->raz();
        patte->femur->determine_position(&squelette[3*patte->squelette->id_femur]);
        patte->tibia->determine_position(&squelette[3*patte->squelette->id_tibia]);
        patte->tarse->determine_position(&squelette[3*patte->squelette->id_tarse]);

        //Rotation du tarse:
        double* point1_a=&squelette_base[3*patte->squelette->id_tarse];
        double* point2_a=&squelette_base[3*patte->squelette->id_contact];
        double* point1_b=&squelette[3*patte->squelette->id_tarse];
        double* point2_b=&squelette[3*patte->squelette->id_contact];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        patte->tarse->rotation(axe_volume.angle,&axe_volume);

        //rotation tibia:
        point1_a=&squelette_base[3*patte->squelette->id_tibia];
        point2_a=&squelette_base[3*patte->squelette->id_tarse];
        point1_b=&squelette[3*patte->squelette->id_tibia];
        point2_b=&squelette[3*patte->squelette->id_tarse];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        patte->tibia->rotation(axe_volume.angle,&axe_volume);

        //rotation femur:
        point1_a=&squelette_base[3*patte->squelette->id_femur];
        point2_a=&squelette_base[3*patte->squelette->id_tibia];
        point1_b=&squelette[3*patte->squelette->id_femur];
        point2_b=&squelette[3*patte->squelette->id_tibia];

        axe1={point2_a[0]-point1_a[0],point2_a[1]-point1_a[1],point2_a[2]-point1_a[2]};
        axe2={point2_b[0]-point1_b[0],point2_b[1]-point1_b[1],point2_b[2]-point1_b[2]};
        axe_volume.calcul_rotation_entre_deux_vecteurs(axe1,axe2);

        patte->femur->rotation(axe_volume.angle,&axe_volume);

    }

};

#endif // PAPILLON_H_INCLUDED
