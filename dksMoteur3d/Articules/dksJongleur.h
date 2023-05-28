#ifndef DKSJONGLEUR_H_INCLUDED
#define DKSJONGLEUR_H_INCLUDED

/*
        Animation d'un personnage, pour en faire un jongleur.
        Une période est la durée entre deux lancé d'une main.
        Une période contient donc 2 temps de siteswap.

*/

#include "dksPersonnage.h"
#include "../../Utiles/Interpolations.h"

//Drapeaux de débuguage:
#define DKSJONGLEUR_DEBUG_LIGNES false
// Erreurs:
#define DKSJONGLEUR_OK 0

//Phase des bras:
#define JONGLEUR_PHASE_PAUSE 0
#define JONGLEUR_PHASE_LANCE 1
#define JONGLEUR_PHASE_POS_RECEPTION 2
#define JONGLEUR_PHASE_RECEPTION 3
#define JONGLEUR_PHASE_POS_LANCE 4

//Données des balles:
#define JONGLEUR_BALLES_MAX 27

//Données du séquenceur:
#define JONGLEUR_SEQ_OCTETS_LIGNE 6 //Taille en mémoire d'une ligne
#define JONGLEUR_TAILLE_SEQUENCE_MAX 256    //Nombre max de lignes dans une séquence
#define JONGLEUR_NBR_PHASES 4   //Nombre de phases entre deux lancés d'une même main

#define JONGLEUR_SEQ_PHASE_ANIM_DROIT 0 //Numéros des octets dans une phase du séquenceur
#define JONGLEUR_SEQ_BALLE_ID_DROIT 1
#define JONGLEUR_SEQ_DUREE_VOL_DROIT 2
#define JONGLEUR_SEQ_PHASE_ANIM_GAUCHE 3
#define JONGLEUR_SEQ_BALLE_ID_GAUCHE 4
#define JONGLEUR_SEQ_DUREE_VOL_GAUCHE 5



//Codes sitesswaps ** Provisoire **
#define JONGLEUR_SITESWAP_3 1
#define JONGLEUR_SITESWAP_5 2
#define JONGLEUR_SITESWAP_9 3
#define JONGLEUR_SITESWAP_75300 4

//===========================================================================
//      Les balles sont des particules
//===========================================================================
//Phases:
#define BALLE_JONGLAGE_SORTIE 0
#define BALLE_JONGLAGE_MAIN_DROITE 1
#define BALLE_JONGLAGE_VOL_DROITE_GAUCHE 2
#define BALLE_JONGLAGE_MAIN_GAUCHE 3
#define BALLE_JONGLAGE_VOL_GAUCHE_DROITE 4

class balle_jonglage
{
  public:
        double position[3];
        float rayon;
        uint8_t phase_id;
        uint8_t duree_vol; //Nombre de temps durant la phase de vol
        float t_depart_vol;    //La date du dernier lancé
        uint32_t couleur;
};
//=================================================================================================================================
//                          Définition de la classe
//=================================================================================================================================
class dksJongleur:public dksPersonnage
{
    protected:
        //Animations du lancé main droite:
        Suite_Interpolations* lance_poignet_droit;
        Suite_Interpolations* lance_coude_droit;
        Suite_Interpolations* lance_epaule_droite_a;
        Suite_Interpolations* lance_epaule_droite_e;
        Suite_Interpolations* lance_epaule_droite_r;

        Suite_Interpolations* lance_poignet_gauche;
        Suite_Interpolations* lance_coude_gauche;
        Suite_Interpolations* lance_epaule_gauche_a;
        Suite_Interpolations* lance_epaule_gauche_e;
        Suite_Interpolations* lance_epaule_gauche_r;

        Courbe_Bezier_Plan* courbe_lance;
        Courbe_Bezier_Plan* courbe_reception;
        Courbe_Bezier_Plan* tableau_courbes_lances[2];
        Courbe_Bezier_Plan* tableau_courbes_receptions[2];

        float valeurs_lance_poignets[4];
        float valeurs_lance_coudes[4];
        float valeurs_lance_epaules_a[4];
        float valeurs_lance_epaules_e[4];
        float valeurs_lance_epaules_r[4];
        uint8_t types_lance[2];

        bool drapeau_lance_main_droite;
        bool drapeau_lance_main_gauche;

        //Animation du positionnement pour la réception:
        Suite_Interpolations* pos_reception_poignet_droit;
        Suite_Interpolations* pos_reception_coude_droit;
        Suite_Interpolations* pos_reception_epaule_droite_a;
        Suite_Interpolations* pos_reception_epaule_droite_e;
        Suite_Interpolations* pos_reception_epaule_droite_r;

        Suite_Interpolations* pos_reception_poignet_gauche;
        Suite_Interpolations* pos_reception_coude_gauche;
        Suite_Interpolations* pos_reception_epaule_gauche_a;
        Suite_Interpolations* pos_reception_epaule_gauche_e;
        Suite_Interpolations* pos_reception_epaule_gauche_r;

        float valeurs_pos_reception_poignets[4];
        float valeurs_pos_reception_coudes[4];
        float valeurs_pos_reception_epaules_a[4];
        float valeurs_pos_reception_epaules_e[4];
        float valeurs_pos_reception_epaules_r[4];
        uint8_t types_pos_reception[2];

        bool drapeau_pos_reception_main_droite;
        bool drapeau_pos_reception_main_gauche;


        //Animations de la réception main droite:
        Suite_Interpolations* reception_poignet_droit;
        Suite_Interpolations* reception_coude_droit;
        Suite_Interpolations* reception_epaule_droite_a;
        Suite_Interpolations* reception_epaule_droite_e;
        Suite_Interpolations* reception_epaule_droite_r;

        Suite_Interpolations* reception_poignet_gauche;
        Suite_Interpolations* reception_coude_gauche;
        Suite_Interpolations* reception_epaule_gauche_a;
        Suite_Interpolations* reception_epaule_gauche_e;
        Suite_Interpolations* reception_epaule_gauche_r;

        float valeurs_reception_poignets[4];
        float valeurs_reception_coudes[4];
        float valeurs_reception_epaules_a[4];
        float valeurs_reception_epaules_e[4];
        float valeurs_reception_epaules_r[4];
        uint8_t types_reception[2];

        bool drapeau_reception_main_droite;
        bool drapeau_reception_main_gauche;

        //Animation du positionnement pour le lancé:
        Suite_Interpolations* pos_lance_poignet_droit;
        Suite_Interpolations* pos_lance_coude_droit;
        Suite_Interpolations* pos_lance_epaule_droite_a;
        Suite_Interpolations* pos_lance_epaule_droite_e;
        Suite_Interpolations* pos_lance_epaule_droite_r;

        Suite_Interpolations* pos_lance_poignet_gauche;
        Suite_Interpolations* pos_lance_coude_gauche;
        Suite_Interpolations* pos_lance_epaule_gauche_a;
        Suite_Interpolations* pos_lance_epaule_gauche_e;
        Suite_Interpolations* pos_lance_epaule_gauche_r;

        float valeurs_pos_lance_poignets[4];
        float valeurs_pos_lance_coudes[4];
        float valeurs_pos_lance_epaules_a[4];
        float valeurs_pos_lance_epaules_e[4];
        float valeurs_pos_lance_epaules_r[4];
        uint8_t types_pos_lance[2];

        bool drapeau_pos_lance_main_droite;
        bool drapeau_pos_lance_main_gauche;

        //Animations de la flexion des jambes pour l'élan lors du lancé:
        Suite_Interpolations* flexion_chevilles;
        Suite_Interpolations* flexion_genoux;
        Suite_Interpolations* flexion_hanches;
        Suite_Interpolations* abduction_hanches;

        float valeurs_flexion_chevilles[4];
        float valeurs_flexion_genoux[4];
        float valeurs_flexion_hanches[4];
        float valeurs_abduction_hanches[4];
        uint8_t types_flexion_jambes[2];

        bool drapeau_flexion_jambes;

        //Gestion des balles:

        uint16_t nbr_balles;
        balle_jonglage balles[3*JONGLEUR_BALLES_MAX];
        double depart_balle_droite[3];
        double arrivee_balle_droite[3];
        double depart_balle_gauche[3];
        double arrivee_balle_gauche[3];

        //Sequenceur:
        //Une ligne du sequenceur a le format suivant:
        //  octet 0: id de la phase d'animation du bras droit
        //  octet 1: numéro de la balle à lancer de la main droite
        //  octet 2: durée de vol de la balle lancée par la main droite (en temps)
        //  octet 3: id de la phase d'animation du bras gauche
        //  octet 4: numéro de la balle à lancer de la main gauche
        //  octet 5: durée de vol de la balle lancée par la main gauche
        bool drapeau_jonglage_en_cours;
        double jonglage_t0;
        float duree_temps;      //Durée(en s) entre deux lancés. La durée d'une phase est: duree_temps/2
        uint16_t taille_sequence; //Nombre de lignes dans la séquence (nombre de temps*2)
        uint16_t ligne_courante; //pointeur de phase dans la séquence
        uint8_t phase_courante_droite;
        uint8_t phase_courante_gauche;
        uint8_t balle_id_droite;  //Balles courantes parées pour le lancé
        uint8_t balle_id_gauche;
        bool drapeau_depart_balle_droite;
        bool drapeau_depart_balle_gauche;
        uint8_t sequence[JONGLEUR_TAILLE_SEQUENCE_MAX*JONGLEUR_SEQ_OCTETS_LIGNE];

    public:
        double hauteur_pieds;   //Sert à calculer la position du personnage par rapport au pliage des jambes.
        float hauteur_balles_temps;   //Hauteur à laquelle vont les balles durant un temps. Valeur multipliée par la durée de vol des balles, -1 (une balle à 1 temps ne s'élève pas)

    //=====================================
    //      Constructeur
    //=====================================
    public:
        dksJongleur(char* p_nom,double px=0.,double py=0.,double pz=0.):dksPersonnage(p_nom,px,py,pz)
        {
            lance_poignet_droit=NULL;
            lance_coude_droit=NULL;
            lance_epaule_droite_a=NULL;
            lance_epaule_droite_e=NULL;
            lance_epaule_droite_r=NULL;

            lance_poignet_gauche=NULL;
            lance_coude_gauche=NULL;
            lance_epaule_gauche_a=NULL;
            lance_epaule_gauche_e=NULL;
            lance_epaule_gauche_r=NULL;

            pos_reception_poignet_droit=NULL;
            pos_reception_coude_droit=NULL;
            pos_reception_epaule_droite_a=NULL;
            pos_reception_epaule_droite_e=NULL;
            pos_reception_epaule_droite_r=NULL;

            pos_reception_poignet_gauche=NULL;
            pos_reception_coude_gauche=NULL;
            pos_reception_epaule_gauche_a=NULL;
            pos_reception_epaule_gauche_e=NULL;
            pos_reception_epaule_gauche_r=NULL;

            reception_poignet_droit=NULL;
            reception_coude_droit=NULL;
            reception_epaule_droite_a=NULL;
            reception_epaule_droite_e=NULL;
            reception_epaule_droite_r=NULL;

            reception_poignet_gauche=NULL;
            reception_coude_gauche=NULL;
            reception_epaule_gauche_a=NULL;
            reception_epaule_gauche_e=NULL;
            reception_epaule_gauche_r=NULL;

            pos_lance_poignet_droit=NULL;
            pos_lance_coude_droit=NULL;
            pos_lance_epaule_droite_a=NULL;
            pos_lance_epaule_droite_e=NULL;
            pos_lance_epaule_droite_r=NULL;

            pos_lance_poignet_gauche=NULL;
            pos_lance_coude_gauche=NULL;
            pos_lance_epaule_gauche_a=NULL;
            pos_lance_epaule_gauche_e=NULL;
            pos_lance_epaule_gauche_r=NULL;

            flexion_chevilles=NULL;
            flexion_genoux=NULL;
            flexion_hanches=NULL;
            abduction_hanches=NULL;

            courbe_lance=NULL;
            courbe_reception=NULL;

            //Courbe d'accélération des lancés:
            courbe_lance=new Courbe_Bezier_Plan(0.,0., 0.5,0., .75,1.-0.5, 1.,1.);
            tableau_courbes_lances={courbe_lance,NULL};
            //Courbe d'accélération des réceptions:
            courbe_reception=new Courbe_Bezier_Plan(0.,0., 0.25,0.5, 0.5,1., 1.,1.);
            tableau_courbes_receptions={courbe_reception,NULL};

            //Creation lancé main droite:

            valeurs_lance_poignets={-20,0.2,-35,0.2};
            valeurs_lance_coudes={-50,0.2,-80,0.2};
            valeurs_lance_epaules_a={-10,0.2,5,0.2};
            valeurs_lance_epaules_e={5.,0.2,0.,0.2};
            valeurs_lance_epaules_r={-30,0.2,-35,0.2};
            types_lance={INTERPOLATION_BEZIER,INTERPOLATION_COSINUS};

            lance_poignet_droit=new Suite_Interpolations(-1,valeurs_lance_poignets,types_lance,2,false,tableau_courbes_lances);
            lance_coude_droit=new Suite_Interpolations(-1,valeurs_lance_coudes,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_droite_a=new Suite_Interpolations(-1,valeurs_lance_epaules_a,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_droite_e=new Suite_Interpolations(-1,valeurs_lance_epaules_e,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_droite_r=new Suite_Interpolations(-1,valeurs_lance_epaules_r,types_lance,2,false,tableau_courbes_lances);
            lance_poignet_droit->stop();
            lance_coude_droit->stop();
            lance_epaule_droite_a->stop();
            lance_epaule_droite_e->stop();
            lance_epaule_droite_r->stop();

            lance_poignet_gauche=new Suite_Interpolations(-1,valeurs_lance_poignets,types_lance,2,false,tableau_courbes_lances);
            lance_coude_gauche=new Suite_Interpolations(-1,valeurs_lance_coudes,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_gauche_a=new Suite_Interpolations(-1,valeurs_lance_epaules_a,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_gauche_e=new Suite_Interpolations(-1,valeurs_lance_epaules_e,types_lance,2,false,tableau_courbes_lances);
            lance_epaule_gauche_r=new Suite_Interpolations(-1,valeurs_lance_epaules_r,types_lance,2,false,tableau_courbes_lances);
            lance_poignet_gauche->stop();
            lance_coude_gauche->stop();
            lance_epaule_gauche_a->stop();
            lance_epaule_gauche_e->stop();
            lance_epaule_gauche_r->stop();

            drapeau_lance_main_droite=false;
            drapeau_lance_main_gauche=false;

            //Creation positionnement réception main droite:

            valeurs_pos_reception_poignets={-35,0.2,-35,0.2};
            valeurs_pos_reception_coudes={-80,0.2,-60,0.2};
            valeurs_pos_reception_epaules_a={5,0.2,5,0.2};
            valeurs_pos_reception_epaules_e={0.,0.2,5.,0.2};
            valeurs_pos_reception_epaules_r={-35,0.2,15,0.2};
            types_pos_reception={INTERPOLATION_LINEAIRE,INTERPOLATION_LINEAIRE};

            pos_reception_poignet_droit=new Suite_Interpolations(-1,valeurs_pos_reception_poignets,types_pos_reception,2,false);
            pos_reception_coude_droit=new Suite_Interpolations(-1,valeurs_pos_reception_coudes,types_pos_reception,2,false);
            pos_reception_epaule_droite_a=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_a,types_pos_reception,2,false);
            pos_reception_epaule_droite_e=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_e,types_pos_reception,2,false);
            pos_reception_epaule_droite_r=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_r,types_pos_reception,2,false);
            pos_reception_poignet_droit->stop();
            pos_reception_coude_droit->stop();
            pos_reception_epaule_droite_a->stop();
            pos_reception_epaule_droite_e->stop();
            pos_reception_epaule_droite_r->stop();

            pos_reception_poignet_gauche=new Suite_Interpolations(-1,valeurs_pos_reception_poignets,types_pos_reception,2,false);
            pos_reception_coude_gauche=new Suite_Interpolations(-1,valeurs_pos_reception_coudes,types_pos_reception,2,false);
            pos_reception_epaule_gauche_a=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_a,types_pos_reception,2,false);
            pos_reception_epaule_gauche_e=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_e,types_pos_reception,2,false);
            pos_reception_epaule_gauche_r=new Suite_Interpolations(-1,valeurs_pos_reception_epaules_r,types_pos_reception,2,false);
            pos_reception_poignet_gauche->stop();
            pos_reception_coude_gauche->stop();
            pos_reception_epaule_gauche_a->stop();
            pos_reception_epaule_gauche_e->stop();
            pos_reception_epaule_gauche_r->stop();

            drapeau_pos_reception_main_droite=false;
            drapeau_pos_reception_main_gauche=false;

            //Creation réception main droite:

            valeurs_reception_poignets={-35,0.2,-55,0.2};
            valeurs_reception_coudes={-60,0.2,-20,0.2};
            valeurs_reception_epaules_a={5,0.2,0,0.2};
            valeurs_reception_epaules_e={5.,0.2,-5.,0.2};
            valeurs_reception_epaules_r={15.,0.2,15.,0.2};
            types_reception={INTERPOLATION_BEZIER,INTERPOLATION_COSINUS};

            reception_poignet_droit=new Suite_Interpolations(-1,valeurs_reception_poignets,types_reception,2,false,tableau_courbes_receptions);
            reception_coude_droit=new Suite_Interpolations(-1,valeurs_reception_coudes,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_droite_a=new Suite_Interpolations(-1,valeurs_reception_epaules_a,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_droite_e=new Suite_Interpolations(-1,valeurs_reception_epaules_e,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_droite_r=new Suite_Interpolations(-1,valeurs_reception_epaules_r,types_reception,2,false,tableau_courbes_receptions);
            reception_poignet_droit->stop();
            reception_coude_droit->stop();
            reception_epaule_droite_a->stop();
            reception_epaule_droite_e->stop();
            reception_epaule_droite_r->stop();

            reception_poignet_gauche=new Suite_Interpolations(-1,valeurs_reception_poignets,types_reception,2,false,tableau_courbes_receptions);
            reception_coude_gauche=new Suite_Interpolations(-1,valeurs_reception_coudes,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_gauche_a=new Suite_Interpolations(-1,valeurs_reception_epaules_a,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_gauche_e=new Suite_Interpolations(-1,valeurs_reception_epaules_e,types_reception,2,false,tableau_courbes_receptions);
            reception_epaule_gauche_r=new Suite_Interpolations(-1,valeurs_reception_epaules_r,types_reception,2,false,tableau_courbes_receptions);
            reception_poignet_gauche->stop();
            reception_coude_gauche->stop();
            reception_epaule_gauche_a->stop();
            reception_epaule_gauche_e->stop();
            reception_epaule_gauche_r->stop();

            drapeau_reception_main_droite=false;
            drapeau_reception_main_gauche=false;

            //Creation positionnement lancé main droite:

            valeurs_pos_lance_poignets={-55,0.2,-20,0.2};
            valeurs_pos_lance_coudes={-20,0.2,-50,0.2};
            valeurs_pos_lance_epaules_a={0.,0.2,-10.,0.2};
            valeurs_pos_lance_epaules_e={-5.,0.2,5.,0.2};
            valeurs_pos_lance_epaules_r={15.,0.2,-30.,0.2};
            types_pos_lance={INTERPOLATION_COSINUS,INTERPOLATION_COSINUS};

            pos_lance_poignet_droit=new Suite_Interpolations(-1,valeurs_pos_lance_poignets,types_pos_lance,2,false);
            pos_lance_coude_droit=new Suite_Interpolations(-1,valeurs_pos_lance_coudes,types_pos_lance,2,false);
            pos_lance_epaule_droite_a=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_a,types_pos_lance,2,false);
            pos_lance_epaule_droite_e=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_e,types_pos_lance,2,false);
            pos_lance_epaule_droite_r=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_r,types_pos_lance,2,false);
            pos_lance_poignet_droit->stop();
            pos_lance_coude_droit->stop();
            pos_lance_epaule_droite_a->stop();
            pos_lance_epaule_droite_e->stop();
            pos_lance_epaule_droite_r->stop();

            pos_lance_poignet_gauche=new Suite_Interpolations(-1,valeurs_pos_lance_poignets,types_pos_lance,2,false);
            pos_lance_coude_gauche=new Suite_Interpolations(-1,valeurs_pos_lance_coudes,types_pos_lance,2,false);
            pos_lance_epaule_gauche_a=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_a,types_pos_lance,2,false);
            pos_lance_epaule_gauche_e=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_e,types_pos_lance,2,false);
            pos_lance_epaule_gauche_r=new Suite_Interpolations(-1,valeurs_pos_lance_epaules_r,types_pos_lance,2,false);
            pos_lance_poignet_gauche->stop();
            pos_lance_coude_gauche->stop();
            pos_lance_epaule_gauche_a->stop();
            pos_lance_epaule_gauche_e->stop();
            pos_lance_epaule_gauche_r->stop();

            drapeau_pos_lance_main_droite=false;
            drapeau_pos_lance_main_gauche=false;

            //Création flexion des jambes:
            valeurs_flexion_hanches={0.,0.2,-15,0.2};
            valeurs_abduction_hanches={8.,0.2,9.,0.25};
            valeurs_flexion_genoux={0.,0.2,30,0.2};
            valeurs_flexion_chevilles={0.,0.2,-15,0.2};
            types_flexion_jambes={INTERPOLATION_COSINUS,INTERPOLATION_COSINUS};

            hauteur_pieds=0.;

            flexion_chevilles=new Suite_Interpolations(-1,valeurs_flexion_chevilles,types_flexion_jambes,2,true);
            flexion_genoux=new Suite_Interpolations(-1,valeurs_flexion_genoux,types_flexion_jambes,2,true);
            flexion_hanches=new Suite_Interpolations(-1,valeurs_flexion_hanches,types_flexion_jambes,2,true);
            abduction_hanches=new Suite_Interpolations(-1,valeurs_abduction_hanches,types_flexion_jambes,2,true);
            flexion_chevilles->stop();
            flexion_genoux->stop();
            flexion_hanches->stop();
            abduction_hanches->stop();

            drapeau_flexion_jambes=false;

            //Initialisations finales:
            hauteur_balles_temps=1.;
            for(uint8_t i=0;i<JONGLEUR_BALLES_MAX;i++)
            {
                balles[i].rayon=0.05;
                balles[i].couleur=(((rand()/128)<<16)|((rand()/128)<<8)|(rand()/128))|0x707070;
            }
            determine_duree_temps(.5);
            drapeau_jonglage_en_cours=false;
            initialise_trajectoires_balles();
            determine_siteswap(JONGLEUR_SITESWAP_3);
        }

    //========================================
    //          Destructeur
    //========================================
    public:
        ~dksJongleur()
        {
            if(lance_poignet_droit!=NULL)delete lance_poignet_droit;
            if(lance_coude_droit!=NULL) delete lance_coude_droit;
            if(lance_epaule_droite_a!=NULL) delete lance_epaule_droite_a;
            if(lance_epaule_droite_e!=NULL) delete lance_epaule_droite_e;
            if(lance_epaule_droite_r!=NULL) delete lance_epaule_droite_r;

            if(lance_poignet_gauche!=NULL)delete lance_poignet_gauche;
            if(lance_coude_gauche!=NULL) delete lance_coude_gauche;
            if(lance_epaule_gauche_a!=NULL) delete lance_epaule_gauche_a;
            if(lance_epaule_gauche_e!=NULL) delete lance_epaule_gauche_e;
            if(lance_epaule_gauche_r!=NULL) delete lance_epaule_gauche_r;

            if(pos_reception_poignet_droit!=NULL)delete pos_reception_poignet_droit;
            if(pos_reception_coude_droit!=NULL) delete pos_reception_coude_droit;
            if(pos_reception_epaule_droite_a!=NULL) delete pos_reception_epaule_droite_a;
            if(pos_reception_epaule_droite_e!=NULL) delete pos_reception_epaule_droite_e;
            if(pos_reception_epaule_droite_r!=NULL) delete pos_reception_epaule_droite_r;

            if(pos_reception_poignet_gauche!=NULL)delete pos_reception_poignet_gauche;
            if(pos_reception_coude_gauche!=NULL) delete pos_reception_coude_gauche;
            if(pos_reception_epaule_gauche_a!=NULL) delete pos_reception_epaule_gauche_a;
            if(pos_reception_epaule_gauche_e!=NULL) delete pos_reception_epaule_gauche_e;
            if(pos_reception_epaule_gauche_r!=NULL) delete pos_reception_epaule_gauche_r;

            if(reception_poignet_droit!=NULL)delete reception_poignet_droit;
            if(reception_coude_droit!=NULL) delete reception_coude_droit;
            if(reception_epaule_droite_a!=NULL) delete reception_epaule_droite_a;
            if(reception_epaule_droite_e!=NULL) delete reception_epaule_droite_e;
            if(reception_epaule_droite_r!=NULL) delete reception_epaule_droite_r;

            if(reception_poignet_gauche!=NULL)delete reception_poignet_gauche;
            if(reception_coude_gauche!=NULL) delete reception_coude_gauche;
            if(reception_epaule_gauche_a!=NULL) delete reception_epaule_gauche_a;
            if(reception_epaule_gauche_e!=NULL) delete reception_epaule_gauche_e;
            if(reception_epaule_gauche_r!=NULL) delete reception_epaule_gauche_r;

            if(pos_lance_poignet_droit!=NULL)delete pos_lance_poignet_droit;
            if(pos_lance_coude_droit!=NULL) delete pos_lance_coude_droit;
            if(pos_lance_epaule_droite_a!=NULL) delete pos_lance_epaule_droite_a;
            if(pos_lance_epaule_droite_e!=NULL) delete pos_lance_epaule_droite_e;
            if(pos_lance_epaule_droite_r!=NULL) delete pos_lance_epaule_droite_r;

            if(pos_lance_poignet_gauche!=NULL)delete pos_lance_poignet_gauche;
            if(pos_lance_coude_gauche!=NULL) delete pos_lance_coude_gauche;
            if(pos_lance_epaule_gauche_a!=NULL) delete pos_lance_epaule_gauche_a;
            if(pos_lance_epaule_gauche_e!=NULL) delete pos_lance_epaule_gauche_e;
            if(pos_lance_epaule_gauche_r!=NULL) delete pos_lance_epaule_gauche_r;


            if(flexion_chevilles!=NULL) delete flexion_chevilles;
            if(flexion_genoux!=NULL) delete flexion_genoux;
            if(flexion_hanches!=NULL) delete flexion_hanches;
            if(abduction_hanches!=NULL) delete flexion_hanches;

            if(courbe_lance!=NULL) delete courbe_lance;

        }

    //=========================================================
    //      Paramétrages
    //=========================================================
    public:
        void determine_duree_temps(float t)
        {
            duree_temps=t;
            t=duree_temps*2/JONGLEUR_NBR_PHASES;

            valeurs_lance_poignets[1]=t;
            valeurs_lance_coudes[1]=t;
            valeurs_lance_epaules_a[1]=t;
            valeurs_lance_epaules_e[1]=t;
            valeurs_lance_epaules_r[1]=t;

            valeurs_pos_lance_poignets[1]=t;
            valeurs_pos_lance_coudes[1]=t;
            valeurs_pos_lance_epaules_a[1]=t;
            valeurs_pos_lance_epaules_e[1]=t;
            valeurs_pos_lance_epaules_r[1]=t;

            valeurs_pos_reception_poignets[1]=t;
            valeurs_pos_reception_coudes[1]=t;
            valeurs_pos_reception_epaules_a[1]=t;
            valeurs_pos_reception_epaules_e[1]=t;
            valeurs_pos_reception_epaules_r[1]=t;

            valeurs_reception_poignets[1]=t;
            valeurs_reception_coudes[1]=t;
            valeurs_reception_epaules_a[1]=t;
            valeurs_reception_epaules_e[1]=t;
            valeurs_reception_epaules_r[1]=t;

            //Rafraichissement des durées:
            lance_poignet_droit->maj_duree_totale();
            lance_poignet_gauche->maj_duree_totale();
            lance_coude_droit->maj_duree_totale();
            lance_coude_gauche->maj_duree_totale();
            lance_epaule_droite_a->maj_duree_totale();
            lance_epaule_droite_e->maj_duree_totale();
            lance_epaule_droite_r->maj_duree_totale();
            lance_epaule_gauche_a->maj_duree_totale();
            lance_epaule_gauche_e->maj_duree_totale();
            lance_epaule_gauche_r->maj_duree_totale();

            pos_lance_poignet_droit->maj_duree_totale();
            pos_lance_poignet_gauche->maj_duree_totale();
            pos_lance_coude_droit->maj_duree_totale();
            pos_lance_coude_gauche->maj_duree_totale();
            pos_lance_epaule_droite_a->maj_duree_totale();
            pos_lance_epaule_droite_e->maj_duree_totale();
            pos_lance_epaule_droite_r->maj_duree_totale();
            pos_lance_epaule_gauche_a->maj_duree_totale();
            pos_lance_epaule_gauche_e->maj_duree_totale();
            pos_lance_epaule_gauche_r->maj_duree_totale();

            pos_reception_poignet_droit->maj_duree_totale();
            pos_reception_poignet_gauche->maj_duree_totale();
            pos_reception_coude_droit->maj_duree_totale();
            pos_reception_coude_gauche->maj_duree_totale();
            pos_reception_epaule_droite_a->maj_duree_totale();
            pos_reception_epaule_droite_e->maj_duree_totale();
            pos_reception_epaule_droite_r->maj_duree_totale();
            pos_reception_epaule_gauche_a->maj_duree_totale();
            pos_reception_epaule_gauche_e->maj_duree_totale();
            pos_reception_epaule_gauche_r->maj_duree_totale();

            reception_poignet_droit->maj_duree_totale();
            reception_poignet_gauche->maj_duree_totale();
            reception_coude_droit->maj_duree_totale();
            reception_coude_gauche->maj_duree_totale();
            reception_epaule_droite_a->maj_duree_totale();
            reception_epaule_droite_e->maj_duree_totale();
            reception_epaule_droite_r->maj_duree_totale();
            reception_epaule_gauche_a->maj_duree_totale();
            reception_epaule_gauche_e->maj_duree_totale();
            reception_epaule_gauche_r->maj_duree_totale();
        }

        virtual void determine_echelle_geometrique(double p_echelle)
        {
            double echelle_prec=echelle_geometrique;

            this->dksPersonnage::determine_echelle_geometrique(p_echelle);
            for(uint8_t i=0;i<JONGLEUR_BALLES_MAX;i++)balles[i].rayon*=echelle_geometrique/echelle_prec;

            depart_balle_droite[0]*=echelle_geometrique/echelle_prec;
            depart_balle_droite[1]*=echelle_geometrique/echelle_prec;
            depart_balle_droite[2]*=echelle_geometrique/echelle_prec;

            arrivee_balle_droite[0]*=echelle_geometrique/echelle_prec;
            arrivee_balle_droite[1]*=echelle_geometrique/echelle_prec;
            arrivee_balle_droite[2]*=echelle_geometrique/echelle_prec;

            depart_balle_gauche[0]*=echelle_geometrique/echelle_prec;
            depart_balle_gauche[1]*=echelle_geometrique/echelle_prec;
            depart_balle_gauche[2]*=echelle_geometrique/echelle_prec;

            arrivee_balle_gauche[0]*=echelle_geometrique/echelle_prec;
            arrivee_balle_gauche[1]*=echelle_geometrique/echelle_prec;
            arrivee_balle_gauche[2]*=echelle_geometrique/echelle_prec;
        }

        void determine_rayon_balles(float p_rayon)
        {
            for(uint8_t i=0;i<JONGLEUR_BALLES_MAX;i++)balles[i].rayon=p_rayon*echelle_geometrique;
        }

        //------------------------------------------------------------------------
        //  Fonction qui initialise le séquenceur en fonction du code SiteSwap
        /// Pour le moment, siteswap 333
        //------------------------------------------------------------------------
        virtual void determine_siteswap(uint8_t p_siteswap)
        {

            uint16_t i;

            switch(p_siteswap)
            {
                case JONGLEUR_SITESWAP_3:
                    taille_sequence=3*JONGLEUR_NBR_PHASES/2 *2; //Multiplication par deux des mouvements assymétriques
                        sequence={  JONGLEUR_PHASE_LANCE,1,3,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,2,3,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,3,3,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,1,3,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,2,3,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,3,3,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0
                                };
                    //----- Initialise les balles:
                    nbr_balles=3;
                    balles[0].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[1].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[2].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    break;

                case JONGLEUR_SITESWAP_5:
                    taille_sequence=5*JONGLEUR_NBR_PHASES/2 *2; //Multiplication par deux des mouvements assymétriques
                        sequence={
                                    JONGLEUR_PHASE_LANCE,1,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,2,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,3,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,4,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,5,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,


                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,1,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,2,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,3,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,4,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,5,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0

                                };
                        //----- Initialise les balles:
                        nbr_balles=5;
                        balles[0].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                        balles[1].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                        balles[2].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                        balles[3].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                        balles[4].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                        break;

                    case JONGLEUR_SITESWAP_9:
                        taille_sequence=9*JONGLEUR_NBR_PHASES/2 *2; //Multiplication par deux des mouvements assymétriques
                        sequence={
                                    JONGLEUR_PHASE_LANCE,1,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,2,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,3,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,4,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,5,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,6,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,7,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,8,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,9,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,1,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,2,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,3,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,4,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,5,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,6,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,7,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,8,9,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,9,9,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0

                                };
                    //----- Initialise les balles:
                    nbr_balles=9;
                    balles[0].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[1].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[2].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[3].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[4].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[5].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[6].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[7].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[8].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    break;

                case JONGLEUR_SITESWAP_75300:
                    taille_sequence=5*JONGLEUR_NBR_PHASES/2 *2; //Multiplication par deux des mouvements assymétriques
                        sequence={
                                    JONGLEUR_PHASE_LANCE,1,7,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,2,5,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,3,3,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,0,0,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,0,0,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,3,7,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,2,5,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,1,3,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0,

                                    JONGLEUR_PHASE_LANCE,0,0,JONGLEUR_PHASE_RECEPTION,0,0,
                                    JONGLEUR_PHASE_POS_RECEPTION,0,0,JONGLEUR_PHASE_POS_LANCE,0,0,

                                    JONGLEUR_PHASE_RECEPTION,0,0,JONGLEUR_PHASE_LANCE,0,0,
                                    JONGLEUR_PHASE_POS_LANCE,0,0,JONGLEUR_PHASE_POS_RECEPTION,0,0

                                };
                    //----- Initialise les balles:
                    nbr_balles=3;
                    balles[0].phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                    balles[1].phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                    balles[2].phase_id=BALLE_JONGLAGE_MAIN_DROITE;

                    break;
            }

        }

    //=======================================================================
    //          Animations
    //=======================================================================
    public:
        virtual void maj_jongleur(double t)
        {

            if(drapeau_jonglage_en_cours)
            {
                if(jonglage_t0<0.)
                {
                    jonglage_t0=t;
                    ligne_courante=0;
                    lecture_ligne_actuelle();
                }
                else
                {
                    float duree_phase=duree_temps/2.;
                    float t_ligne_courante=ligne_courante*duree_phase;
                    if(t-jonglage_t0>t_ligne_courante+duree_phase)
                    {
                        ligne_courante++;
                        if(ligne_courante>=taille_sequence)
                        {
                            ligne_courante=0;
                            jonglage_t0=t;
                        }
                        lecture_ligne_actuelle();
                    }
                }
                //Animation:
                maj_animation(t);
            }

            maj_squelette();
            maj_balles(t);

            //Ajuste la position par rapport au pliage des jambes:
            double hauteur_min=squelette[PERSONNAGE_TALON_DROIT*3+1];
            if(squelette[PERSONNAGE_TALON_GAUCHE*3+1]<hauteur_min) hauteur_min=squelette[PERSONNAGE_TALON_GAUCHE*3+1];
            if(squelette[PERSONNAGE_PLANTE_DROITE*3+1]<hauteur_min) hauteur_min=squelette[PERSONNAGE_PLANTE_DROITE*3+1];
            if(squelette[PERSONNAGE_PLANTE_GAUCHE*3+1]<hauteur_min) hauteur_min=squelette[PERSONNAGE_PLANTE_GAUCHE*3+1];
            determine_position(x,hauteur_pieds-hauteur_min,z);
        }
    //----------------------------------------------------------
    //  Lecture des donnée de la ligne actuelle du séquenceur:
    //----------------------------------------------------------
    private:
        void lecture_ligne_actuelle()
        {
            //Déclenchement des lancés:
            if((phase_courante_droite==JONGLEUR_PHASE_LANCE) && (balle_id_droite!=0)) drapeau_depart_balle_droite=true;   //Drapeaux utilisés par l'animation des balles
            if((phase_courante_gauche==JONGLEUR_PHASE_LANCE) && (balle_id_gauche!=0)) drapeau_depart_balle_gauche=true;   //Rabaissés par l'animateur des balles.

            //Lecture ligne courante:
            uint8_t* ligne_sequence=&sequence[ligne_courante*JONGLEUR_SEQ_OCTETS_LIGNE];

            phase_courante_droite=ligne_sequence[JONGLEUR_SEQ_PHASE_ANIM_DROIT];
            phase_courante_gauche=ligne_sequence[JONGLEUR_SEQ_PHASE_ANIM_GAUCHE];

            //Init balle:
            balle_id_droite=ligne_sequence[JONGLEUR_SEQ_BALLE_ID_DROIT];
            balle_id_gauche=ligne_sequence[JONGLEUR_SEQ_BALLE_ID_GAUCHE];
            balle_jonglage* balle_droite;
            balle_jonglage* balle_gauche;
            if(balle_id_droite>0)
            {
                balle_droite=&balles[balle_id_droite-1];
            }
            if(balle_id_gauche>0)
            {
                balle_gauche=&balles[balle_id_gauche-1];
            }

            #if DKSJONGLEUR_DEBUG_LIGNES
            cout<<ligne_courante;
            cout<<" balles: "<<uint16_t(balle_id_droite)<<":"<<uint16_t(ligne_sequence[JONGLEUR_SEQ_DUREE_VOL_DROIT]);
            cout<<" "<<uint16_t(balle_id_gauche)<<":"<<uint16_t(ligne_sequence[JONGLEUR_SEQ_DUREE_VOL_GAUCHE]);
            cout<<endl;
            #endif

            //Interprétation:
            switch(phase_courante_droite)
            {
                case JONGLEUR_PHASE_LANCE:
                    depart_lance_main_droite();
                    depart_flexion_jambes();
                    if(balle_id_droite!=0) balle_droite->duree_vol=ligne_sequence[JONGLEUR_SEQ_DUREE_VOL_DROIT];
                    break;
                case JONGLEUR_PHASE_POS_RECEPTION: depart_pos_reception_main_droite(); break;
                case JONGLEUR_PHASE_RECEPTION: depart_reception_main_droite(); break;
                case JONGLEUR_PHASE_POS_LANCE: depart_pos_lance_main_droite(); break;
            }
            switch(phase_courante_gauche)
            {
                case JONGLEUR_PHASE_LANCE:
                    depart_lance_main_gauche();
                    depart_flexion_jambes();
                    if(balle_id_gauche!=0) balle_gauche->duree_vol=ligne_sequence[JONGLEUR_SEQ_DUREE_VOL_GAUCHE];
                    break;
                case JONGLEUR_PHASE_POS_RECEPTION: depart_pos_reception_main_gauche(); break;
                case JONGLEUR_PHASE_RECEPTION: depart_reception_main_gauche(); break;
                case JONGLEUR_PHASE_POS_LANCE: depart_pos_lance_main_gauche(); break;
            }
        }

    //---------------------------------
    //  Rafraichissement des angles:
    //---------------------------------
    private:
        void maj_animation(double t)
        {
            //Animation des jambes:
                    if(drapeau_flexion_jambes)
                    {
                        angle_Vd=flexion_chevilles->renvoie_valeur(t);
                        angle_Vg=angle_Vd;
                        angle_Gd=flexion_genoux->renvoie_valeur(t);
                        angle_Gg=angle_Gd;
                        angle_Hde=flexion_hanches->renvoie_valeur(t);
                        angle_Hge=angle_Hde;
                        angle_Hda=abduction_hanches->renvoie_valeur(t);
                        angle_Hga=angle_Hda;
                        if(flexion_chevilles->drapeau_terminee)drapeau_flexion_jambes=false;
                    }

                    //Animation du lancé main droite:
                    if(drapeau_lance_main_droite)
                    {
                        angle_Pda=lance_poignet_droit->renvoie_valeur(t);
                        angle_Cd=lance_coude_droit->renvoie_valeur(t);
                        angle_Eda=lance_epaule_droite_a->renvoie_valeur(t);
                        angle_Ede=lance_epaule_droite_e->renvoie_valeur(t);
                        angle_Edr=lance_epaule_droite_r->renvoie_valeur(t);
                        if(lance_poignet_droit->drapeau_terminee)drapeau_lance_main_droite=false;
                    }

                    //Animation du positionnement main droite:
                    if(drapeau_pos_reception_main_droite)
                    {
                        angle_Pda=pos_reception_poignet_droit->renvoie_valeur(t);
                        angle_Cd=pos_reception_coude_droit->renvoie_valeur(t);
                        angle_Eda=pos_reception_epaule_droite_a->renvoie_valeur(t);
                        angle_Ede=pos_reception_epaule_droite_e->renvoie_valeur(t);
                        angle_Edr=pos_reception_epaule_droite_r->renvoie_valeur(t);
                        if(pos_reception_poignet_droit->drapeau_terminee) drapeau_pos_reception_main_droite=false;
                    }

                    //Animation de la réception main droite:
                    if(drapeau_reception_main_droite)
                    {
                        angle_Pda=reception_poignet_droit->renvoie_valeur(t);
                        angle_Cd=reception_coude_droit->renvoie_valeur(t);
                        angle_Eda=reception_epaule_droite_a->renvoie_valeur(t);
                        angle_Ede=reception_epaule_droite_e->renvoie_valeur(t);
                        angle_Edr=reception_epaule_droite_r->renvoie_valeur(t);
                        if(reception_poignet_droit->drapeau_terminee)drapeau_reception_main_droite=false;
                    }

                    //Animation du positionnement du lancé main droite:
                    if(drapeau_pos_lance_main_droite)
                    {
                        angle_Pda=pos_lance_poignet_droit->renvoie_valeur(t);
                        angle_Cd=pos_lance_coude_droit->renvoie_valeur(t);
                        angle_Eda=pos_lance_epaule_droite_a->renvoie_valeur(t);
                        angle_Ede=pos_lance_epaule_droite_e->renvoie_valeur(t);
                        angle_Edr=pos_lance_epaule_droite_r->renvoie_valeur(t);
                        if(pos_lance_poignet_droit->drapeau_terminee) drapeau_pos_lance_main_droite=false;
                    }

                    //Animation du lancé main gauche:
                    if(drapeau_lance_main_gauche)
                    {
                        angle_Pga=lance_poignet_gauche->renvoie_valeur(t);
                        angle_Cg=lance_coude_gauche->renvoie_valeur(t);
                        angle_Ega=lance_epaule_gauche_a->renvoie_valeur(t);
                        angle_Ege=lance_epaule_gauche_e->renvoie_valeur(t);
                        angle_Egr=lance_epaule_gauche_r->renvoie_valeur(t);
                        if(lance_poignet_gauche->drapeau_terminee)drapeau_lance_main_gauche=false;
                    }

                    //Animation du positionnement main gauche:
                    if(drapeau_pos_reception_main_gauche)
                    {
                        angle_Pga=pos_reception_poignet_gauche->renvoie_valeur(t);
                        angle_Cg=pos_reception_coude_gauche->renvoie_valeur(t);
                        angle_Ega=pos_reception_epaule_gauche_a->renvoie_valeur(t);
                        angle_Ege=pos_reception_epaule_gauche_e->renvoie_valeur(t);
                        angle_Egr=pos_reception_epaule_gauche_r->renvoie_valeur(t);
                        if(pos_reception_poignet_gauche->drapeau_terminee) drapeau_pos_reception_main_gauche=false;
                    }

                    //Animation de la réception main gauche:
                    if(drapeau_reception_main_gauche)
                    {
                        angle_Pga=reception_poignet_gauche->renvoie_valeur(t);
                        angle_Cg=reception_coude_gauche->renvoie_valeur(t);
                        angle_Ega=reception_epaule_gauche_a->renvoie_valeur(t);
                        angle_Ege=reception_epaule_gauche_e->renvoie_valeur(t);
                        angle_Egr=reception_epaule_gauche_r->renvoie_valeur(t);
                        if(reception_poignet_gauche->drapeau_terminee)drapeau_reception_main_gauche=false;
                    }

                    //Animation du positionnement du lancé main gauche:
                    if(drapeau_pos_lance_main_gauche)
                    {
                        angle_Pga=pos_lance_poignet_gauche->renvoie_valeur(t);
                        angle_Cg=pos_lance_coude_gauche->renvoie_valeur(t);
                        angle_Ega=pos_lance_epaule_gauche_a->renvoie_valeur(t);
                        angle_Ege=pos_lance_epaule_gauche_e->renvoie_valeur(t);
                        angle_Egr=pos_lance_epaule_gauche_r->renvoie_valeur(t);
                        if(pos_lance_poignet_gauche->drapeau_terminee) drapeau_pos_lance_main_gauche=false;
                    }
        }

    //===============================================
    //          Animations des balles
    //===============================================
    private:
        void initialise_trajectoires_balles()
        {
            angle_Pdr=80.;
            angle_Pda=valeurs_lance_poignets[2];
            angle_Cd=valeurs_lance_coudes[2];
            angle_Eda=valeurs_lance_epaules_a[2];
            angle_Ede=valeurs_lance_epaules_e[2];
            angle_Edr=valeurs_lance_epaules_r[2];

            maj_squelette();

            depart_balle_droite={squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};
            depart_balle_gauche={-squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};

            angle_Pda=valeurs_reception_poignets[0];
            angle_Cd=valeurs_reception_coudes[0];
            angle_Eda=valeurs_reception_epaules_a[0];
            angle_Ede=valeurs_reception_epaules_e[0];
            angle_Edr=valeurs_reception_epaules_r[0];

            maj_squelette();

            arrivee_balle_droite={squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};
            arrivee_balle_gauche={-squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};


            maj_squelette();
        }

        //---------------------------------------
        //      Calcul la position des balles:
        //---------------------------------------
        void maj_balles(double t)
        {
            uint16_t i;
            float t_vol_balle,t_balle,rayon_vol;

            for(i=0;i<nbr_balles;i++)
            {
                balle_jonglage* balle=&balles[i];
                uint8_t phase_id=balle->phase_id;
                switch (phase_id)
                {
                    case BALLE_JONGLAGE_MAIN_DROITE:
                        if(drapeau_depart_balle_droite)
                        {
                            balle->phase_id=BALLE_JONGLAGE_VOL_DROITE_GAUCHE;
                            balle->t_depart_vol=t;
                            drapeau_depart_balle_droite=false;
                        }
                        else
                        {
                            balle->position={squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};
                        }
                    break;
                    case BALLE_JONGLAGE_VOL_DROITE_GAUCHE:
                        t_vol_balle=(balle->duree_vol-1)*duree_temps-(duree_temps/2.);
                        if(t-balle->t_depart_vol >= t_vol_balle)
                        {
                            balle->phase_id=BALLE_JONGLAGE_MAIN_GAUCHE;
                            balle->position={squelette[PERSONNAGE_MAIN_GAUCHE*3],squelette[PERSONNAGE_MAIN_GAUCHE*3+1],squelette[PERSONNAGE_MAIN_GAUCHE*3+2]};
                        }
                        else
                        {
                            t_balle=(t-balle->t_depart_vol)/t_vol_balle;
                            balle->position[0]=(arrivee_balle_gauche[0]-depart_balle_droite[0])*t_balle+depart_balle_droite[0];
                            balle->position[2]=(arrivee_balle_gauche[2]-depart_balle_droite[2])*t_balle+depart_balle_droite[2];

                            if(t_balle<0.5)
                            {
                                rayon_vol=(balle->duree_vol-1)*hauteur_balles_temps;
                                balle->position[1]=depart_balle_droite[1]+rayon_vol*sin(t_balle*M_PI);

                            }
                            else
                            {
                                rayon_vol=(balle->duree_vol-1)*hauteur_balles_temps+depart_balle_droite[1]-arrivee_balle_gauche[1];
                                balle->position[1]=arrivee_balle_gauche[1]+rayon_vol*sin(t_balle*M_PI);
                            }
                        }

                    break;
                    case BALLE_JONGLAGE_MAIN_GAUCHE:
                        if(drapeau_depart_balle_gauche)
                        {
                            balle->phase_id=BALLE_JONGLAGE_VOL_GAUCHE_DROITE;
                            balle->t_depart_vol=t;
                            drapeau_depart_balle_gauche=false;
                        }
                        else
                        {
                            balle->position={squelette[PERSONNAGE_MAIN_GAUCHE*3],squelette[PERSONNAGE_MAIN_GAUCHE*3+1],squelette[PERSONNAGE_MAIN_GAUCHE*3+2]};
                        }
                    break;
                    case BALLE_JONGLAGE_VOL_GAUCHE_DROITE:
                        t_vol_balle=(balle->duree_vol-1)*duree_temps-(duree_temps/2.);
                        if(t-balle->t_depart_vol >= t_vol_balle)
                        {
                            balle->phase_id=BALLE_JONGLAGE_MAIN_DROITE;
                            balle->position={squelette[PERSONNAGE_MAIN_DROITE*3],squelette[PERSONNAGE_MAIN_DROITE*3+1],squelette[PERSONNAGE_MAIN_DROITE*3+2]};
                        }
                        else
                        {
                            t_balle=(t-balle->t_depart_vol)/t_vol_balle;
                            balle->position[0]=(arrivee_balle_droite[0]-depart_balle_gauche[0])*t_balle+depart_balle_gauche[0];
                            balle->position[2]=(arrivee_balle_droite[2]-depart_balle_gauche[2])*t_balle+depart_balle_gauche[2];
                            if(t_balle<0.5)
                            {
                                rayon_vol=(balle->duree_vol-1)*hauteur_balles_temps;
                                balle->position[1]=depart_balle_gauche[1]+rayon_vol*sin(t_balle*M_PI);

                            }
                            else
                            {
                                rayon_vol=(balle->duree_vol-1)*hauteur_balles_temps+depart_balle_gauche[1]-arrivee_balle_droite[1];
                                balle->position[1]=arrivee_balle_droite[1]+rayon_vol*sin(t_balle*M_PI);
                            }
                        }
                    break;
                }
            }
        }

    //======================================================================
    //          Contrôls des mouvements élémentaires
    //======================================================================
    private:
        void depart_flexion_jambes()
        {
            abduction_hanches->start();
            flexion_chevilles->start();
            flexion_genoux->start();
            flexion_hanches->start();
            drapeau_flexion_jambes=true;
        }

        void depart_lance_main_droite()
        {
            angle_Pdr=80.;
            lance_poignet_droit->start();
            lance_coude_droit->start();
            lance_epaule_droite_a->start();
            lance_epaule_droite_e->start();
            lance_epaule_droite_r->start();

            drapeau_lance_main_droite=true;
        }

        void depart_pos_reception_main_droite()
        {
            angle_Pdr=80.;
            pos_reception_poignet_droit->start();
            pos_reception_coude_droit->start();
            pos_reception_epaule_droite_a->start();
            pos_reception_epaule_droite_e->start();
            pos_reception_epaule_droite_r->start();

            drapeau_pos_reception_main_droite=true;
        }

        void depart_reception_main_droite()
        {
            angle_Pdr=80.;
            reception_poignet_droit->start();
            reception_coude_droit->start();
            reception_epaule_droite_a->start();
            reception_epaule_droite_e->start();
            reception_epaule_droite_r->start();

            drapeau_reception_main_droite=true;
        }

        void depart_pos_lance_main_droite()
        {
            angle_Pdr=80.;
            pos_lance_poignet_droit->start();
            pos_lance_coude_droit->start();
            pos_lance_epaule_droite_a->start();
            pos_lance_epaule_droite_e->start();
            pos_lance_epaule_droite_r->start();

            drapeau_pos_lance_main_droite=true;
        }

        void depart_lance_main_gauche()
        {
            angle_Pgr=80.;
            lance_poignet_gauche->start();
            lance_coude_gauche->start();
            lance_epaule_gauche_a->start();
            lance_epaule_gauche_e->start();
            lance_epaule_gauche_r->start();

            drapeau_lance_main_gauche=true;
        }

        void depart_pos_reception_main_gauche()
        {
            angle_Pgr=80.;
            pos_reception_poignet_gauche->start();
            pos_reception_coude_gauche->start();
            pos_reception_epaule_gauche_a->start();
            pos_reception_epaule_gauche_e->start();
            pos_reception_epaule_gauche_r->start();

            drapeau_pos_reception_main_gauche=true;
        }

        void depart_reception_main_gauche()
        {
            angle_Pgr=80.;
            reception_poignet_gauche->start();
            reception_coude_gauche->start();
            reception_epaule_gauche_a->start();
            reception_epaule_gauche_e->start();
            reception_epaule_gauche_r->start();

            drapeau_reception_main_gauche=true;
        }

        void depart_pos_lance_main_gauche()
        {
            angle_Pgr=80.;
            pos_lance_poignet_gauche->start();
            pos_lance_coude_gauche->start();
            pos_lance_epaule_gauche_a->start();
            pos_lance_epaule_gauche_e->start();
            pos_lance_epaule_gauche_r->start();

            drapeau_pos_lance_main_gauche=true;
        }

    //======================================================================
    //          Contrôls de l'enchaînement des phases
    //======================================================================
    public:

        void depart_jonglage()
        {
            jonglage_t0=-1.;
            drapeau_jonglage_en_cours=true;
            drapeau_depart_balle_droite=false;
            drapeau_depart_balle_gauche=false;
            phase_courante_droite=JONGLEUR_PHASE_PAUSE;
            phase_courante_gauche=JONGLEUR_PHASE_PAUSE;
            balle_id_droite=0;
            balle_id_gauche=0;
        }

        void stop_jonglage()
        {
            drapeau_jonglage_en_cours=false;
        }

        //===============================================
    //          Affichages
    //===============================================
    public:
        virtual void affiche(dksOeil* oeil)
        {
            this->dksPersonnage::affiche(oeil);
            uint16_t i;
            GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);
            bool point_smooth_mem=glIsEnabled(GL_POINT_SMOOTH);

            glPushMatrix();
            glMultMatrixd(matrice.matrice);
            //Centres des balles:

            glPointSize(10.);
            glEnable(GL_POINT_SMOOTH);
            glBegin(GL_POINTS);
            for(i=0;i<nbr_balles;i++)
            {
                glColor3f(((balles[i].couleur>>16)&0xff)/255.,((balles[i].couleur>>8)&0xff)/255.,(balles[i].couleur&0xff)/255.);
                glVertex3dv((balles[i].position));
            }
            glEnd();

            if (lum_mem)glEnable(GL_LIGHTING);
            if (!point_smooth_mem) glDisable(GL_POINT_SMOOTH);
            glPopMatrix();
        }

};
#endif // DKSJONGLEUR_H_INCLUDED
