#ifndef SCENE_GREETINGS_H_INCLUDED
#define SCENE_GREETINGS_H_INCLUDED

/*

            Scène des greetings

*/


#include "Scene.h"
#include "../dksMoteur3d/Texte/dksPolice3d.h"
#include "../dksMoteur3d/Texte/dksTexte3d.h"
#include "../Utiles/Bruit.h"

class Scene_Greetings : public Scene
{
    public:
    Scene_Arbre* scene_arbre;
    dksChargeLWO2* lettres_lwo2;
    dksConvertionLWO2* lettres_converties;

    dksPolice3d* police_groupes;
    #define GREETS_NBR_GROUPES 16
    uint16_t groupe_actuel;
    float duree_groupe;
    float t_depart_groupe;
    float t_groupe;
    uint16_t etape_groupe;
    dksTexte3d* groupes[GREETS_NBR_GROUPES];
    char* noms_groupes[GREETS_NBR_GROUPES];
    Bruit perlin_roulis;
    Bruit perlin_tangage;
    Bruit perlin_lacet;

    Bruit perlin_roulis_oeil;
    Bruit perlin_tangage_oeil;
    Bruit perlin_lacet_oeil;

    dksSphere* vectorBalls[NBR_VECTORBALLS];
    dksParticulesVolumes* particules_vectorBalls;
    dksConteneur* conteneur_mvt_balls_1;
    dksConteneur* conteneur_mvt_balls_2;
    double centre_gravite_balls_RTL[3];
    double vitesse_centre_gravite_balls_RTL[3];

    dksPlan* plan_ondulant;
    Image* texture_plan;
    #define TAILLE_TEXTURE_ONDULATIONS_GREETS 256
    uint8_t bruit_ondulations[TAILLE_TEXTURE_ONDULATIONS_GREETS*TAILLE_TEXTURE_ONDULATIONS_GREETS];
    Image* image_bruit_ondulation;

    dksCube* mon_cube;
    dksCube* cube_arriere_plan;
    Image* textures_arriere_plan[6];

    dksOeil* oeil_1;
    dksConteneur* conteneur_oeil_1;

    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;
    dksSourceLumineuse* source_3;

    dksMoteur3d* scene;

    dksVolume* volumes[10];
    dksConteneur* conteneurs[10];
    #define GREETS_NBR_LETTRES 93
    dksVolume* lettres[GREETS_NBR_LETTRES];
    dksOeil* yeux[10];
    dksSourceLumineuse* sources[10];
    float ouverture_camera;

    //Drapeau d'initialisation des phases
    enum {
            GREETS_DEVELOPPEMENT
            };
    bool etapes[1];

    //============ Données d'animation

        //Interpolations de paramètre simple:
        enum    {
                    ARRIVEE_GROUPE,
                    DEPART_GROUPE
                };

        Interpolation_Parametre* interpolations_parametres[2];

    //============= Données de l'éditeur:

        double vitesse_rotation;
        double vitesse_deplacement;
        float position_point;
        vector<dksConteneur*> conteneurs_selectionnables;
        dksConteneur* conteneur_selectionne;
        char* fichier_trajectoire;
        dksTrajectoire ma_trajectoire;
        uint32_t point_actuel;
        float position_trajectoire;
        float coordonnees_trajectoire[3];
        float pas_position_trajectoire;

    //============================================================================
    //              CONSTRUCTEUR
    //============================================================================
    Scene_Greetings(char* p_nom,dksEcran* p_ecran,Musique* p_musique,Scene_Arbre* p_scene_arbre):Scene(p_nom,p_ecran,p_musique)
    {
        scene_arbre=p_scene_arbre;
        mon_cube=NULL;
        oeil_1=NULL;
        source_1=NULL;
        source_2=NULL;
        source_3=NULL;
        scene=NULL;
        lettres_lwo2=NULL;
        lettres_converties=NULL;
        police_groupes=NULL;
        plan_ondulant=NULL;
        particules_vectorBalls=NULL;
        conteneur_oeil_1=NULL;
        groupe_actuel=0;
        cube_arriere_plan=NULL;
        texture_plan=NULL;
        image_bruit_ondulation=NULL;
        conteneur_mvt_balls_1=NULL;
        conteneur_mvt_balls_2=NULL;


        uint32_t i;
        for(i=0;i<GREETS_NBR_LETTRES;i++) lettres[i]=NULL;
        for(i=0;i<GREETS_NBR_GROUPES;i++) groupes[i]=NULL;
        for (i=0;i<NBR_VECTORBALLS;i++) vectorBalls[i]=NULL;

        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres[i]=NULL;
        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) textures_arriere_plan[i]=NULL;


        //----------------- Chargement des volumes:
            lettres_lwo2=new dksChargeLWO2("LWO2/fonts/Aharoni_700.lwo");
            if (lettres_lwo2==NULL || lettres_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            lettres_converties=new dksConvertionLWO2(lettres_lwo2);
            if (lettres_converties==NULL || lettres_converties->erreur!=DKSCONVERTIONLWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }



        //--------------- Création des textes 3d:
            for(i=0;i<GREETS_NBR_LETTRES;i++)
            {
                lettres[i]=lettres_converties->renvoie_volume_index(i);
                //lettres[i]->determine_mode_rendu(DKSVOLUME_MODE_LISTE); //Mode LISTE activé lors de la création de la police.
            }
            police_groupes=new dksPolice3d(lettres,GREETS_NBR_LETTRES,1.,.15);
            police_groupes->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);

            noms_groupes={"Onslaught","Mahoney","Hack N Trade","Judas","Up Rough","TRSI","Nostalgia","Booze Design","Offence","Triad","Nuance","Wrath Designs",
                            "Hokuto Force","Vision","Noice","Panda Design"};

            #define POSITION_GROUPS_Y 6.
            #define POSITION_GROUPS_Z -2.
            for(i=0;i<GREETS_NBR_GROUPES;i++)
            {
                groupes[i]=new dksTexte3d(police_groupes,noms_groupes[i],noms_groupes[i],0.,POSITION_GROUPS_Y,POSITION_GROUPS_Z);
                groupes[i]->init_perturbations_Perlin();
                groupes[i]->drapeau_affiche=false;
            }

            /*
            for(i=0;i<GREETS_NBR_GROUPES;i++)
            {
                cout<<groupes[i]->renvoie_texte()<<endl;
            }
            */

        //-------------- Création du plan ondulant:


            double axes[9]={0.55479,0,-0.83199,0,1,0,0.83199,0,0.55479};

            plan_ondulant=new dksPlan("Ondule",11.1935, -14, 158.919,
                                        300.,300.,        //Taille
                                        20,20,          //Mailles
                                        false,  //triangles/rectangles
                                        false    //recto/verso
                                        );
            if(plan_ondulant==NULL || plan_ondulant->erreur!=DKSPLAN_OK)
            {
                cout<<"ERREUR Lors de la création du plan ondulant de la scène: "<<nom<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            plan_ondulant->determine_axes(axes);


            plan_ondulant->surface_plan_recto->specularite=0.8;
            plan_ondulant->surface_plan_recto->brillance=40.;
            //plan_ondulant->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_PHONG;
            plan_ondulant->surface_plan_recto->drapeau_adoucissement=true;

            plan_ondulant->surface_plan_recto->intensite_ondes=0.25;
            plan_ondulant->surface_plan_recto->quantite_ondes=0.1;
            plan_ondulant->surface_plan_recto->echelle_perturbation_ondes=50.;
            plan_ondulant->surface_plan_recto->perturbation_ondes=5.;
            plan_ondulant->surface_plan_recto->vitesse_ondes=10.;
            plan_ondulant->surface_plan_recto->amplitude_ondes=.25;
            plan_ondulant->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
            //plan_ondulant->mode_remplissage=DKSVOLUME_REMPLISSAGE_ARETES;

            plan_ondulant->longueur_ondes_x=40.;
            plan_ondulant->vitesse_ondes_x=160.;
            plan_ondulant->amplitude_ondes_x=.5;

            plan_ondulant->longueur_ondes_z=50.;
            plan_ondulant->vitesse_ondes_z=120.;
            plan_ondulant->amplitude_ondes_z=2.;

            plan_ondulant->intensite_perturbations=0.06;
            plan_ondulant->echelle_perturbation_ondes=1.;

            plan_ondulant->vitesse_perturbation_x=10.;
            plan_ondulant->vitesse_perturbation_z=20.;


            //Texture du plan ondulant:

            texture_plan=new Image("textures/greets/plan_ondulant.tif");

            if(texture_plan==NULL || texture_plan->erreur!=IMAGE_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            plan_ondulant->determine_texture_recto(texture_plan);
            //plan_ondulant->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_TEXTURE_COULEUR;
            plan_ondulant->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_ONDES_TEXTURE_COULEUR;
            //---------- Génère la texture de bruit:
            if (!Bruit::genere_bruit_de_Perlin_2D_8bits(4,4,TAILLE_TEXTURE_ONDULATIONS_GREETS,TAILLE_TEXTURE_ONDULATIONS_GREETS,12,0.75,bruit_ondulations))
            {
                cout<<"ERREUR dans Scene_Greetings() - Impossible de créer la texture de bruit des ondulations"<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }
            //char* p_nom_image,uint8_t* p_image,uint32_t p_largeur,uint32_t p_hauteur, GLenum p_format_pixels, GLenum p_type_pixels, uint16_t p_bits_par_pixels,uint16_t p_octets_par_pixels
            image_bruit_ondulation=new Image(NULL,bruit_ondulations,
                                             TAILLE_TEXTURE_ONDULATIONS_GREETS,TAILLE_TEXTURE_ONDULATIONS_GREETS,
                                             GL_LUMINANCE8,GL_UNSIGNED_BYTE,8,1
                                             );
            plan_ondulant->texture_perturbation=image_bruit_ondulation;

        //------------------ Arrière plan:
            cube_arriere_plan=new dksCube("Cube_arriere_plan_greetings",0.,0.,0.,10.,10.,10.,0xffffffff);
            textures_arriere_plan[0]=new Image("textures/arriere_plans/greets_FR.jpg",false,true);
            textures_arriere_plan[1]=new Image("textures/arriere_plans/greets_BK.jpg",false,true);
            textures_arriere_plan[2]=new Image("textures/arriere_plans/greets_RT.jpg",false,true);
            textures_arriere_plan[3]=new Image("textures/arriere_plans/greets_LF.jpg",false,true);
            textures_arriere_plan[4]=new Image("textures/arriere_plans/greets_UP.jpg",false,true);
            textures_arriere_plan[5]=new Image("textures/arriere_plans/greets_DN.jpg",false,true);


            cube_arriere_plan->ajoute_textures_arriere_plan(textures_arriere_plan[0],textures_arriere_plan[1],textures_arriere_plan[2],
                                                               textures_arriere_plan[3],textures_arriere_plan[4],textures_arriere_plan[5]);

            cube_arriere_plan->inverse_sens_polygones();
            cube_arriere_plan->determine_affecte_tempon_profondeur(false);

        //-------------- Vector balls:
            for (i=0;i<NBR_VECTORBALLS;i++)
            {
                vectorBalls[i]=new dksSphere("baballe_greets",3.,10.,0.,
                                   6,5,
                                   .1,
                                   0xffff4040);
                if(vectorBalls[i]==NULL || vectorBalls[i]->erreur!=DKSVOLUME_OK)
                {
                    cout<<"ERREUR lors de la création des vectorBalls"<<endl;
                    erreur=SCENE_ERREUR_INIT;
                    return ;
                }

                vectorBalls[i]->surfaces_sphere[0]->mode_rendu=DKSSURFACE_RENDU_PHONG;
            }

            particules_vectorBalls=new dksParticulesVolumes("Particules_vectorBalls_Greets",0.,0.,0.,(dksVolume**)vectorBalls,NBR_VECTORBALLS);
            particules_vectorBalls->determine_vitesses_rotations(0.,0.,0.);
            ///A commenter après développement.
            particules_vectorBalls->determine_points(scene_arbre->particules_vectorBalls->renvoie_points(),NBR_VECTORBALLS);
            particules_vectorBalls->random_masses(1.,3.);
            particules_vectorBalls->random_amplitudes_perlin(0.25,1.);
            particules_vectorBalls->random_intervalles_perlin(0.1,.5);

            //Conteneurs utilisés pour générer les mouvements des shpères par gravité:
            conteneur_mvt_balls_1=new dksConteneur("conteneur_mouvement_balls_1",0.,POSITION_GROUPS_Y,POSITION_GROUPS_Z);
            conteneur_mvt_balls_2=new dksConteneur("conteneur_mouvement_balls_2",0.,4.,0.);
            conteneur_mvt_balls_1->ajoute_enfant(conteneur_mvt_balls_2);
            vitesse_centre_gravite_balls_RTL={35./180.*M_PI*1.,43./180.*M_PI*1.,-57./180*M_PI*1.};

        //----------------- Création des éléments de la scène:
        //Yeux:
            oeil_1=new dksOeil("Oeil_1",-0.0772596, 3.72469, -9.8722);
            oeil_courant=oeil_1;
            conteneur_oeil_1=new dksConteneur("conteneur_travelling",0.,0.,0.);
            conteneur_oeil_1->ajoute_enfant(oeil_1);
            axes={0.999978,-0.00327286,-0.0056919,0.00213878,0.981993,-0.188901,0.00620767,0.188884,0.981979};
            oeil_1->determine_axes(axes);

        //Création des sources:
            source_1=new dksSourceLumineuse("Source_1",96.4604, 42.5317, 264.856);
            source_1->determine_attenuation(1.,0.,0.);

            source_2=new dksSourceLumineuse("Source_2",0.4398, 7.7822, -5.6625,0xff1000,0x000000,0xff6000);
            source_2->determine_attenuation(1.,.075,0.);

            source_3=new dksSourceLumineuse("Source_3",0, 0, 0,0xff80f0,0x000000,0xff80f0);
            source_3->determine_attenuation(1.,.5,0.);

            conteneur_mvt_balls_2->ajoute_enfant(source_3);


        //Cubes:
            mon_cube=new dksCube("Cube_test",0.,0.,0.,.1,.1,.1,0xffff0000);//20.,16.,28.
            //conteneur_mvt_balls_2->ajoute_enfant(mon_cube);
            //mon_cube->ajoute_enfant(source_2);
            //mon_cube->determine_luminescence(1.);

        //Assignations:
            volumes[0]=plan_ondulant;
            volumes[1]=particules_vectorBalls;
            volumes[2]=mon_cube;
            sources[0]=source_1;
            sources[1]=source_2;
            sources[2]=source_3;
            yeux[0]=oeil_1;
            conteneurs[0]=conteneur_oeil_1;
            conteneurs[1]=conteneur_mvt_balls_1;
            conteneurs[2]=conteneur_mvt_balls_2;


        //Création de la scene:
            scene=new dksMoteur3d("Greetings",ecran,
                            conteneurs,3,
                            volumes,2,
                            yeux,1,
                            sources,3,
                            oeil_1);

            scene->ajoute_volumes((dksVolume**)groupes,GREETS_NBR_GROUPES);
            scene->ajoute_volumes((dksVolume**)vectorBalls,NBR_VECTORBALLS,particules_vectorBalls);
            scene->determine_cube_arriere_plan(cube_arriere_plan);

        initialise_animations();
        initialise_listes_affichage();
        #if EDITEUR_ACTIF
        initialise_editeur();
        #endif
    }

    //=====================================================
    //          DESTRUCTEUR
    //=====================================================
    ~Scene_Greetings()
    {
        uint32_t i;
        if (scene!=NULL) delete scene;
        if (mon_cube!=NULL) delete mon_cube;
        if(cube_arriere_plan!=NULL) delete cube_arriere_plan;
        if (oeil_1!=NULL) delete oeil_1;
        if (source_1!=NULL) delete source_1;
        if (source_2!=NULL) delete source_2;
        for(i=0;i<GREETS_NBR_GROUPES;i++) if(groupes[i]!=NULL) delete groupes[i];
        if(police_groupes!=NULL) delete police_groupes;
        if(lettres_converties!=NULL) delete lettres_converties;
        if(lettres_lwo2!=NULL) delete lettres_lwo2;
        if(plan_ondulant!=NULL) delete plan_ondulant;
        if  (particules_vectorBalls!=NULL) delete particules_vectorBalls;
        for (i=0;i<NBR_VECTORBALLS;i++) if (vectorBalls[i]!=NULL) delete vectorBalls[i];
        if(conteneur_oeil_1!=NULL) delete conteneur_oeil_1;

        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++)
            if (interpolations_parametres[i]!=NULL) delete interpolations_parametres[i];

        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) if(textures_arriere_plan[i]!=NULL) delete textures_arriere_plan[i];
        if (texture_plan!=NULL) delete texture_plan;
        if(image_bruit_ondulation!=NULL) delete image_bruit_ondulation;
        if(conteneur_mvt_balls_1!=NULL) delete conteneur_mvt_balls_1;
        if(conteneur_mvt_balls_2!=NULL) delete conteneur_mvt_balls_2;

    }

    //==========================================================
    //                      Fonctions
    //==========================================================
    virtual void active()
    {
        this->Scene::active();
        scene->active();
        //scene->active_brouillard();
    }

    virtual void desactive()
    {
        scene->desactive();
    }

    virtual void maj_dimensions_fenetre()
    {
        //scene->maj_resolutions_yeux();
    }

    //===============================================================================
    //                          AFFICHAGE
    //===============================================================================

    virtual void affiche(double t)
    {
        this->Scene::affiche(t);

        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->maj(t);
        double centre_balls[3]={conteneur_mvt_balls_2->repere_absolu->x,conteneur_mvt_balls_2->repere_absolu->y,conteneur_mvt_balls_2->repere_absolu->z};
        particules_vectorBalls->maj_gravite(centre_balls,10.,1.,1.);
        particules_vectorBalls->maj_positions_points_perturbation_perlin();
        particules_vectorBalls->maj_volumes();
        plan_ondulant->maj_ondulations_xz_bruit();
        scene->affiche(oeil_courant);

        //post_rendu->affiche_flou_quart(oeil_courant);

    }

    //========================================================================================================================================
    //========================================================================================================================================
    //                                                      Gestion animations
    //========================================================================================================================================
    //========================================================================================================================================
    //----------------------------------------------------------------------------
    //                  Initialise les données d'animation
    //----------------------------------------------------------------------------
    void initialise_animations()
    {
        ouverture_camera=65.;
        oeil_courant=oeil_1;

        Courbe_Bezier_Plan* courbe_apparition=new Courbe_Bezier_Plan(0.,0.,0.5,0.5,0.5,1.,1.,1.);
        Courbe_Bezier_Plan* courbe_disparition=new Courbe_Bezier_Plan(0.,0.,0.5,0.,0.5,0.5,1.,1.);
        interpolations_parametres[ARRIVEE_GROUPE]=new Interpolation_Parametre(-1.,-17.,0.,.25,INTERPOLATION_BEZIER,courbe_apparition);
        interpolations_parametres[DEPART_GROUPE]=new Interpolation_Parametre(-1.,0.,17.,.25,INTERPOLATION_BEZIER,courbe_disparition);

        for(uint16_t i=0;i<GREETS_NBR_GROUPES;i++)
        {
            groupes[i]->x=interpolations_parametres[ARRIVEE_GROUPE]->valeur_depart;
            groupes[i]->a_ete_deplace();
        }
    }

    public:
    //----------------------------------------------------------------------------
    //                  Etat de la scène au départ de l'animation
    //----------------------------------------------------------------------------
    virtual void initialise_t0(double t0)
    {
        this->Scene::initialise_t0(t0);
        oeil_1->determine_ouverture(ouverture_camera);
        particules_vectorBalls->determine_points(scene_arbre->particules_vectorBalls->renvoie_points(),NBR_VECTORBALLS);

        double* pts=particules_vectorBalls->renvoie_points();

        //scene->t0=t0;
        duree_groupe=1.;

    }

    void initialise_listes_affichage()
    {
        //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_ECRAN,DKSRENDU3D_COULEURS);
        //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_QUART,DKSRENDU3D_SURFACES_LUMINESCENTES);
    }

    //=====================================================

    //======================================================
    //               Attente du retour du faisceau
    //              Gestion des animations
    //=======================================================

    virtual void attente_retour_faisceau()
    {
        if (!drapeau_initialisee || drapeau_scene_terminee) return;

        if (position_temps_scene<25.)
            {
                if (!etapes[GREETS_DEVELOPPEMENT])
                {
                    etapes[GREETS_DEVELOPPEMENT]=true;
                    groupes[groupe_actuel]->drapeau_affiche=true;
                    etape_groupe=0;
                    t_depart_groupe=-1.;
                }
                //conteneur_oeil_1->rotation(0.005,0.,1.,0.);
                defilement_groupes();
                if (groupe_actuel<GREETS_NBR_GROUPES)
                groupes[groupe_actuel]->determine_RTL((perlin_roulis.valeur_aleatoire_temporelle(position_temps,0.20)-0.5)/4.,
                                                      (perlin_tangage.valeur_aleatoire_temporelle(position_temps,0.15)-0.5)/4.,
                                                      (perlin_lacet.valeur_aleatoire_temporelle(position_temps,0.25)-0.5)*2.);

                conteneur_oeil_1->determine_RTL((perlin_roulis_oeil.valeur_aleatoire_temporelle(position_temps,2.)-0.5)/4.,
                                                      (perlin_tangage_oeil.valeur_aleatoire_temporelle(position_temps,1.)-0.5)/20.,
                                                      (perlin_lacet_oeil.valeur_aleatoire_temporelle(position_temps,2)-0.5)*0.);

                conteneur_mvt_balls_1->determine_RTL(centre_gravite_balls_RTL[0],centre_gravite_balls_RTL[1],centre_gravite_balls_RTL[2]);
                centre_gravite_balls_RTL[0]+=vitesse_centre_gravite_balls_RTL[0]*delta_t;
                centre_gravite_balls_RTL[1]+=vitesse_centre_gravite_balls_RTL[1]*delta_t;
                centre_gravite_balls_RTL[2]+=vitesse_centre_gravite_balls_RTL[2]*delta_t;
            }
            else
            {
                drapeau_scene_terminee=true;
            }
    }

    //====================================================================================
    //      Gestion du défilement des noms des groupes
    //====================================================================================
    void defilement_groupes()
    {
        /*
        groupes[groupe_actuel]->x=interpolations_parametres[ARRIVEE_GROUPE]->valeur_fin;
        groupes[groupe_actuel]->a_ete_deplace();
        return;
        */


        if(groupe_actuel<GREETS_NBR_GROUPES)
            {
                if(etape_groupe==0)
                {
                    groupes[groupe_actuel]->x=interpolations_parametres[ARRIVEE_GROUPE]->renvoie_valeur_parametre(position_temps);
                    groupes[groupe_actuel]->a_ete_deplace();
                    if(interpolations_parametres[ARRIVEE_GROUPE]->drapeau_terminee) etape_groupe=1;
                }
                if(etape_groupe==1)    ///Pas Else if, sinon il peut y avoir des décallage dans la synchro
                {
                  if(t_depart_groupe<0.)
                    {
                        t_depart_groupe=position_temps_scene;

                    }
                    t_groupe=position_temps_scene-t_depart_groupe;
                    if(t_groupe>=duree_groupe)
                    {
                        t_depart_groupe=-1.;
                        etape_groupe=2;
                        interpolations_parametres[DEPART_GROUPE]->reset();
                    }
                }
                if(etape_groupe==2)    ///IDEM
                {
                    groupes[groupe_actuel]->x=interpolations_parametres[DEPART_GROUPE]->renvoie_valeur_parametre(position_temps);
                    groupes[groupe_actuel]->a_ete_deplace();
                    if(interpolations_parametres[DEPART_GROUPE]->drapeau_terminee)
                    {
                        groupes[groupe_actuel]->drapeau_affiche=false;
                        groupe_actuel++;
                        if(groupe_actuel<GREETS_NBR_GROUPES) //groupe_actuel=0;  //A SUPPRIMER APRES DEV
                        groupes[groupe_actuel]->drapeau_affiche=true;
                        interpolations_parametres[ARRIVEE_GROUPE]->reset();
                        etape_groupe=0;
                    }
                }


                    //animation
            }
    }

    //======================================================================================================================================
    //======================================================================================================================================
    //
    //                                                          Editeur
    //
    //======================================================================================================================================
    //======================================================================================================================================

    //============================================================================
    //                     Initialisations de l'éditeur
    //============================================================================

        virtual void initialise_editeur()
        {
            vitesse_deplacement=4.;
            vitesse_rotation=0.1;

            oeil_1->masse=10.;
            oeil_1->determine_coefficient_frottement(1.,100.);


            //particules_vectorBalls->masse=10.;
            //particules_vectorBalls->determine_coefficient_frottement(1.,100.);

            mon_cube->masse=10.;
            mon_cube->determine_coefficient_frottement(1.,100.);

            position_point=0.1;

            position_trajectoire=0.;
            pas_position_trajectoire=0.005;
            fichier_trajectoire={"trajectoires/camera_greetings_01.traj"};

            conteneurs_selectionnables.push_back(oeil_1);
            conteneurs_selectionnables.push_back(mon_cube);
            conteneurs_selectionnables.push_back(particules_vectorBalls);


            conteneur_selectionne=oeil_1;
        }

        //===================================================
        //          Affiche la trajectoire éditée
        //===================================================
        void affiche_trajectoire()
        {
            //glLoadIdentity();
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            ma_trajectoire.affiche_courbe(20,0x40ffff00);
            ma_trajectoire.affiche_points(0x80ff0000,5);
            ma_trajectoire.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        //==============================================
        //              Gestion souris
        //==============================================
        virtual void mouvement_souris(int x, int y)
        {

        }
        //=================================================
        //                     Gestion du clavier
        //=================================================
        virtual void touche_speciale_appuyee(int key, int x, int y)
        {
            switch (key)
            {
                 case 101:
                //flèche haut
                conteneur_selectionne->deplace(AVANT,vitesse_deplacement);
                cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                break;
                case 100:
                    //flèche gauche
                    conteneur_selectionne->deplace(GAUCHE,vitesse_deplacement);
                    cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                    break;
                case 103:
                    //flèche bas
                    conteneur_selectionne->deplace(ARRIERE,vitesse_deplacement);
                    cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                    break;
                case 102:
                    //flèche droite
                    conteneur_selectionne->deplace(DROITE,vitesse_deplacement);
                    cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                    break;

                //Touches haut et bas
                case 104:
                    point_actuel++;
                    if (point_actuel>=ma_trajectoire.renvoie_nombre_points())point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;

                case 105:
                    if (point_actuel!=0)point_actuel--;
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;

                //Touches F..
                case 1:
                    if (ma_trajectoire.sauve_trajectoire(fichier_trajectoire)) cout<<"Trajectoire "<<fichier_trajectoire<<" sauvée avec succès!"<<endl;
                    break;

                case 2:
                    if (ma_trajectoire.charge_trajectoire(fichier_trajectoire))
                    {
                        cout<<"Trajectoire "<<fichier_trajectoire<<" chargée avec succès!"<<endl;
                        point_actuel=0;
                    }
                default:
                    break;

            }
            glutPostRedisplay();
        }

        virtual void touche_speciale_relachee(int key, int x, int y)
        {
            switch (key)
            {
                case 101:

                case 100:

                case 103:

                case 102:

                    break;
                default:
                    break;

            }
            conteneur_selectionne->stop_propulsion();
            conteneur_selectionne->stop_propulsion_angulaire();

            glutPostRedisplay();
        }

        virtual void touche_appuyee(unsigned char key, int x, int y)
        {
            dksRepere* rep=conteneur_selectionne->repere_absolu;
            switch (key)
            {
                case 27 :

                    exit(0);
                    break;

                case '9':
                    conteneur_selectionne->deplace(HAUT,vitesse_deplacement);
                    cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                    break;

                case '3':
                    conteneur_selectionne->deplace(BAS,vitesse_deplacement);
                    cout<<conteneur_selectionne->x<<" "<<conteneur_selectionne->y<<" "<<conteneur_selectionne->z<<endl;
                    break;

                case '4':
                conteneur_selectionne->tourne(HAUT,vitesse_rotation);
                break;
                case '6':
                    conteneur_selectionne->tourne(BAS,vitesse_rotation);
                    break;
                case '7':
                    conteneur_selectionne->tourne(GAUCHE,vitesse_rotation);
                    break;
                case '1':
                    conteneur_selectionne->tourne(DROITE,vitesse_rotation);
                    break;
                case '0':
                    conteneur_selectionne->tourne(ARRIERE,vitesse_rotation);
                    break;
                case '.':
                    conteneur_selectionne->tourne(AVANT,vitesse_rotation);
                    break;

                case ' ':
                    for (uint32_t i=0;i<conteneurs_selectionnables.size();i++)
                    {
                        if (conteneurs_selectionnables[i]==conteneur_selectionne)
                        {
                            if (i==conteneurs_selectionnables.size()-1) conteneur_selectionne=conteneurs_selectionnables[0];
                            else conteneur_selectionne=conteneurs_selectionnables[i+1];
                            cout<<"Conteneur sélectionné: "<<conteneur_selectionne->nom<<endl;
                            break;
                        }
                    }
                    break;
                case '+':
                    vitesse_deplacement+=0.25; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '-':
                    vitesse_deplacement-=0.25; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '*':
                    vitesse_rotation+=0.0025; cout<<"Rotation: "<<vitesse_rotation<<endl;
                    break;
                case '/':
                    vitesse_rotation-=0.0025; cout<<"Rotation: "<<vitesse_rotation<<endl;
                    break;

                case ')':
                    ouverture_camera-=0.5; cout<<"Ouverture: "<<ouverture_camera<<endl;
                    oeil_1->determine_ouverture(ouverture_camera);
                    break;
                case '=':
                    ouverture_camera+=0.5; cout<<"Ouverture: "<<ouverture_camera<<endl;
                    oeil_1->determine_ouverture(ouverture_camera);
                    break;


                //---------- Edition des trajectoire:
                case 13: //ENTREE
                    ma_trajectoire.ajoute_point(conteneur_selectionne->x,conteneur_selectionne->y,conteneur_selectionne->z);
                    point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    break;
                case 8: //SUPPR
                    ma_trajectoire.supprime_point(point_actuel);
                    if (point_actuel>=ma_trajectoire.renvoie_nombre_points())point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    break;
                case 'q':
                    ma_trajectoire.deplace_point(point_actuel,-vitesse_deplacement,0.,0.);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case 's':
                    ma_trajectoire.deplace_point(point_actuel,vitesse_deplacement,0.,0.);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case 'a':
                    ma_trajectoire.deplace_point(point_actuel,0.,vitesse_deplacement,0.);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case 'w':
                    ma_trajectoire.deplace_point(point_actuel,0.,-vitesse_deplacement,0.);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case 'z':
                    ma_trajectoire.deplace_point(point_actuel,0.,0.,-vitesse_deplacement);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case 'x':
                    ma_trajectoire.deplace_point(point_actuel,0.,0.,vitesse_deplacement);
                    ma_trajectoire.affiche_position_point(point_actuel);
                    break;
                case '&':
                    position_trajectoire-=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
                    break;
                case 233: //²
                    position_trajectoire+=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
                    break;
                case 'r':
                    cout<<"Repère relatif: "<<conteneur_selectionne->axe_x->x<<","<<conteneur_selectionne->axe_x->y<<","<<conteneur_selectionne->axe_x->z<<","
                                            <<conteneur_selectionne->axe_y->x<<","<<conteneur_selectionne->axe_y->y<<","<<conteneur_selectionne->axe_y->z<<","
                                            <<conteneur_selectionne->axe_z->x<<","<<conteneur_selectionne->axe_z->y<<","<<conteneur_selectionne->axe_z->z<<endl;

                    cout<<"Repère absolu: "<<rep->axe_x->x<<","<<rep->axe_x->y<<","<<rep->axe_x->z<<","
                                            <<rep->axe_y->x<<","<<rep->axe_y->y<<","<<rep->axe_y->z<<","
                                            <<rep->axe_z->x<<","<<rep->axe_z->y<<","<<rep->axe_z->z<<endl;
                    break;

                default:
                    break;
            }
            glutPostRedisplay();
        }


        virtual void touche_relachee(unsigned char key, int x, int y)
        {
            switch (key)
            {

                case '9':
                case '3':

                    break;

                case 'a':
                case 'w':
                case 'q':
                case 's':
                case 'z':
                case 'e':

                    break;

                default:
                    break;
            }
        conteneur_selectionne->stop_propulsion();
        conteneur_selectionne->stop_propulsion_angulaire();
            glutPostRedisplay();
        }

};

#endif // SCENE_GREETINGS_H_INCLUDED
