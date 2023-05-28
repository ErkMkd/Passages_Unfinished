#ifndef SCENE_CADUCE_H_INCLUDED
#define SCENE_CADUCE_H_INCLUDED


#define SCENE_CADUCE_OK 0

#include "Scene.h"
#include "../dksMoteur3d/Particules/dksParticulesVolumes.h"
#include "../dksMoteur3d/Volumes/Serpent.h"
#include "../dksMoteur3d/Particules/dksSprite3d.h"
#include "../dksMoteur3d/Particules/dksStarfield.h"

class Scene_Caduce: public Scene
{

    public:

        Scene_Tunnel* scene_tunnel;

        dksChargeLWO2* volumes_lwo2;
        dksConvertionLWO2* volumes_convertis;

        dksSourceLumineuse* source_soleil;
        dksSourceLumineuse* source_1;
        dksSourceLumineuse* source_2;

        dksVolume* caducee_serpent_1;
        dksVolume* traj_position_serpent;
        dksVolume* machoire;
        dksVolume* dents;
        dksVolume* oeil_droit;
        dksVolume* oeil_gauche;
        dksVolume* sol_eau;
        dksVolume* tas_cailloux;
        dksVolume* caducee_baton;
        dksOeil* oeil_1;
        dksOeil* oeil_1_starfield;

        Serpent* mon_serpent;

        dksCube* cube_arriere_plan;
        Image* textures_arriere_plan[6];

        //------------Données de la scène de transition:
        #define Z_DEPART_STARFIELD -5000.
            dksMoteur3d* scene_transition;
            dksSprite3d* etoile_1;
            dksStarfield* le_starfield;
            dksCube* cube_arriere_plan_starfield;
            Image* textures_arriere_plan_starfield[6];
            dksTrajectoire trajectoire_oeil_01_starfield;
            Courbe_Bezier_Plan* courbe_trajectoire_oeil_01_starfield;
            Interpolation_Parametre* position_trajectoire_oeil_1_starfield; ///Détruite par la scène du starfield
        //------------------------------------


        #define NBRVOLUMESPARTICULES 50
        dksParticulesVolumes* mes_particules;
        dksVolume* volumes_particules[NBRVOLUMESPARTICULES];

        dksMoteur3d* scene;
        dksVolume* volumes[20];
        dksSourceLumineuse* sources[10];
        dksOeil* yeux[10];

        float ouverture_camera;
        float alpha_scene;

        double point_vise_0[3];
        double point_vise_1[3];
        double distance_point_vise;
        float position[3];
        float position_trajectoire_camera;
        float position_trajectoire_serpent;
        dksTrajectoire* trajectoire_camera_caducee_01;
        dksTrajectoire trajectoire_camera_caducee_02;
        dksTrajectoire trajectoire_caducee;    //Trajectoire récupérée de la scène du tunnel
        dksTrajectoire trajectoire_deplacement_serpent; //déplacement du serpent

        dksRepere repere_arrivee_serpent;

        //================= Animations ======================
        Interpolation_Parametre* camera_pos_trajectoire_01; //récupérée dans la scène précédente
        //Etapes de l'animation:
        enum    {
                    SERPENT,
                    TRAVELLING_CADUCEE,
                    VUE_ENSEMBLE_CADUCEE,
                    OUVERTURE_MACHOIRE,
                    DERNIERE_ETAPE
                };
        bool etapes[5];

        Courbe_Bezier_Plan* courbe_interpolation_serpent;

        //Interpolations de paramètre simple:
        enum    {
                    CAMERA_POS_TRAJECTOIRE_02,
                    OUVERTURE_SOURCE_SOLEIL,
                    INTENSITE_SOURCE_SOLEIL,
                    INTENSITE_SOURCES_DECREMENTE,
                    INTENSITE_SOURCES_INCREMENTE,
                    SERPENT_TRAJECTOIRE,
                    ECHELLE_EAU,
                    MACHOIRE,
                    DENTS
                };
        Interpolation_Parametre* interpolations_parametres[9];
        //Interpolations de position
        enum    {
                    POINT_VISE_SERPENT_1,
                    POINT_VISE_CENTRE_SERPENTS,
                    POINT_VISE_SCENE
                };
        Interpolation_Parametre_3d* interpolations_positions[3];

        //Interpolations d'orientation de repères:
        enum    {
                    VISE_SERPENT
                };
        Interpolation_Repere* interpolations_reperes[1];

        //============= Données de l'éditeur:

        //dksCube* mon_cube;
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
        bool drapeau_alignement_conteneur_trajectoire;

        //=================================================
        //              Constructeur
        //=================================================
        Scene_Caduce(char* p_nom,dksEcran* p_ecran,Musique* p_musique,Scene_Tunnel* p_scene_tunnel):Scene(p_nom,p_ecran,p_musique)
        {
            uint32_t i;
            scene_tunnel=p_scene_tunnel;
            volumes_lwo2=NULL;
            volumes_convertis=NULL;

            scene=NULL;

            caducee_serpent_1=NULL;
            caducee_baton=NULL;
            traj_position_serpent=NULL;
            sol_eau=NULL;
            tas_cailloux=NULL;

            oeil_1=NULL;

            mes_particules=NULL;

            mon_serpent=NULL;

            trajectoire_camera_caducee_01=NULL;
            courbe_interpolation_serpent=NULL;

            cube_arriere_plan=NULL;

            ouverture_camera=60.;
            distance_point_vise=1000./scene_tunnel->echelle_caducee;

            for (i=0;i<sizeof(yeux)/sizeof(dksOeil*);i++) yeux[i]=NULL;
            for (i=0;i<sizeof(sources)/sizeof(dksSourceLumineuse*);i++) sources[i]=NULL;
            for (i=0;i<sizeof(volumes_particules)/sizeof(dksVolume*);i++) volumes_particules[i]=NULL;
            for (i=0;i<sizeof(etapes)/sizeof(bool);i++) etapes[i]=false;
            for (i=0;i<sizeof(interpolations_reperes)/sizeof(Interpolation_Repere*);i++) interpolations_reperes[i]=NULL;
            for (i=0;i<sizeof(interpolations_positions)/sizeof(Interpolation_Parametre_3d*);i++) interpolations_positions[i]=NULL;
            for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres[i]=NULL;
            for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) textures_arriere_plan[i]=NULL;

            oeil_1_starfield=NULL;
            le_starfield=NULL;
            cube_arriere_plan_starfield=NULL;
            for (i=0;i<sizeof(textures_arriere_plan_starfield)/sizeof(Image*);i++) textures_arriere_plan_starfield[i]=NULL;
            scene_transition=NULL;
            etoile_1=NULL;

            //------------- Chargement des volumes:


            volumes_lwo2=new dksChargeLWO2("LWO2/caducee.lwo");
            if (volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
            if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }
            //--------------Création du serpent animé:


            mon_serpent=new Serpent("mon_serpent",0.,0.,0.,"LWO2/snake.lwo",73,3.35,0.26,SERPENT_SEGMENTS_EGAUX,false);
            if (mon_serpent==NULL || mon_serpent->erreur!=SERPENT_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            mon_serpent->determine_echelle_geometrique(10.);
            mon_serpent->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);



            //-----------------------------------------------Particules:
                for (i=0;i<sizeof(volumes_particules)/sizeof(dksVolume*);i++)
                {
                    volumes_particules[i]=new dksCube("cube_particule",0.,0.,0.,.5,2.,.5,0xff8080);
                    volumes_particules[i]->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                    volumes_particules[i]->determine_recoit_ombres(false);
                }

                mes_particules=new dksParticulesVolumes("Particules_volumes",0.,0.,0.,volumes_particules,sizeof(volumes_particules)/sizeof(dksVolume*));

                mes_particules->initialise_flux(-100,100,-1,100.,-100,100,HAUT);
                dksVolume* repartition=volumes_convertis->renvoie_volume_nom("repartition_particules");

                if (repartition==NULL)
                {
                    erreur=SCENE_ERREUR_INIT;
                    return;
                }

                mes_particules->determine_points_XZ(repartition->renvoie_sommets(),NBRVOLUMESPARTICULES);

                mes_particules->determine_vitesses_rotations(0.,0.,M_PI/180.*2.);

            //----------- Arrière-plan:


                textures_arriere_plan[0]=scene_tunnel->textures_arriere_plan_transition[0];
                textures_arriere_plan[1]=scene_tunnel->textures_arriere_plan_transition[1];
                textures_arriere_plan[2]=scene_tunnel->textures_arriere_plan_transition[2];
                textures_arriere_plan[3]=scene_tunnel->textures_arriere_plan_transition[3];
                textures_arriere_plan[4]=scene_tunnel->textures_arriere_plan_transition[4];
                textures_arriere_plan[5]=scene_tunnel->textures_arriere_plan_transition[5];


                cube_arriere_plan=new dksCube("Cube_arriere_plan",0.,0.,0.,10.,10.,10.,0xfffffff);

                cube_arriere_plan->ajoute_textures_arriere_plan(textures_arriere_plan[0],textures_arriere_plan[1],textures_arriere_plan[2],
                                                                    textures_arriere_plan[3],textures_arriere_plan[4],textures_arriere_plan[5]);


                cube_arriere_plan->inverse_sens_polygones();

                cube_arriere_plan->determine_affecte_tempon_profondeur(false);

                cube_arriere_plan->determine_mode_rendu(DKSVOLUME_MODE_LISTE);


            //--------------- Création de la scène de transition du starfield:

                //Création du starfield:
                etoile_1=new dksSprite3d("etoile_1","textures/starfield/etoile_b00.tif");
                le_starfield=new dksStarfield("Champ_etoiles",0.,0.,0.,2000,1000.,30.,etoile_1);
                //mes_particules->determine_point(0,1.,1.,80.);
                le_starfield->random_points(-1000.,1000.,-1000.,1000.,-1000.+Z_DEPART_STARFIELD,1000.+Z_DEPART_STARFIELD);

                //Arrière plan:
                cube_arriere_plan_starfield=new dksCube("Cube_arriere_plan_starfield",0.,0.,0.,10.,10.,10.,0xffffffff);

                textures_arriere_plan_starfield[0]=new Image("textures/arriere_plans/cosmos_FR.jpg",false,true);
                textures_arriere_plan_starfield[1]=new Image("textures/arriere_plans/cosmos_BK.jpg",false,true);
                textures_arriere_plan_starfield[2]=new Image("textures/arriere_plans/cosmos_RT.jpg",false,true);
                textures_arriere_plan_starfield[3]=new Image("textures/arriere_plans/cosmos_LF.jpg",false,true);
                textures_arriere_plan_starfield[4]=new Image("textures/arriere_plans/cosmos_UP.jpg",false,true);
                textures_arriere_plan_starfield[5]=new Image("textures/arriere_plans/cosmos_DN.jpg",false,true);


                cube_arriere_plan_starfield->ajoute_textures_arriere_plan(textures_arriere_plan_starfield[0],textures_arriere_plan_starfield[1],textures_arriere_plan_starfield[2],
                                                                   textures_arriere_plan_starfield[3],textures_arriere_plan_starfield[4],textures_arriere_plan_starfield[5]);

                cube_arriere_plan_starfield->inverse_sens_polygones();
                cube_arriere_plan_starfield->determine_affecte_tempon_profondeur(false);

                oeil_1_starfield=new dksOeil("Oeil_1_starfield",0.,0.,Z_DEPART_STARFIELD);
                oeil_1_starfield->determine_contrainte_verticale(true);
                oeil_1_starfield->determine_point_vise(0.,0.,-2000.);
                oeil_1_starfield->determine_mode_visee(DKSOEIL_VISEE_POINT);

                yeux[0]=oeil_1_starfield;
                volumes[0]=le_starfield;
                scene_transition=new dksMoteur3d("Transition_Caduce_Starfield",ecran,
                            NULL,0,
                            volumes,1,
                            yeux,1,
                            NULL,0,
                            oeil_1_starfield);

                scene_transition->determine_cube_arriere_plan(cube_arriere_plan_starfield);
                scene_transition->drapeau_activation_rendu=true;
                scene_transition->active_brouillard();
                scene_transition->determine_limites_brouillard(50.,800.);
                scene_transition->determine_couleur_brouillard(0xff0c0a18);

                if(!trajectoire_oeil_01_starfield.charge_trajectoire("trajectoires/camera_starfield_01.traj"))
                {
                    erreur=SCENE_ERREUR_INIT;
                    return;
                }
                courbe_trajectoire_oeil_01_starfield=new Courbe_Bezier_Plan(0.,0., 0.1,0.1, 0.5,.985, 1.,1.);
                position_trajectoire_oeil_1_starfield=new Interpolation_Parametre(-1.,0.,1.,15.,INTERPOLATION_BEZIER,courbe_trajectoire_oeil_01_starfield);


            //----------- Assignations:

                volumes[0]=volumes_convertis->renvoie_volume_nom("serpent_1"); //Serpent 1
                volumes[1]=volumes_convertis->renvoie_volume_nom("baton"); //Baton
                volumes[2]=volumes_convertis->renvoie_volume_nom("objet_sol"); //Sol_eau
                volumes[3]=volumes_convertis->renvoie_volume_nom("tas_cailloux"); //tas_cailloux
                volumes[4]=mes_particules;
                volumes[5]=mon_serpent->renvoie_machoire(); //machoire
                volumes[6]=mon_serpent->renvoie_oeil_droit(); //oeil droit
                volumes[7]=mon_serpent->renvoie_oeil_gauche(); //oeil gauche
                volumes[8]=mon_serpent;
                volumes[9]=mon_serpent->renvoie_dents(); //Serpent 1

                caducee_serpent_1=volumes[0];
                machoire=volumes[5];
                dents=volumes[9];
                oeil_droit=volumes[6];
                oeil_gauche=volumes[7];
                caducee_baton=volumes[1];
                traj_position_serpent=volumes_convertis->renvoie_volume_nom("position_serpent");

                sol_eau=volumes[2];
                tas_cailloux=volumes[3];

                //Paramètres de l'eau:

                sol_eau->determine_angle_seuil_ombres(90.);
                sol_eau->determine_recoit_ombres(false);
                dksSurface* surface_eau=sol_eau->renvoie_surface_nom("eau");
                if (surface_eau!=NULL)
                {
                    //surface_eau->reflexion=0.;
                    surface_eau->intensite_ondes=0.1;
                    surface_eau->quantite_ondes=.1;
                    surface_eau->echelle_perturbation_ondes=100.;
                    surface_eau->vitesse_ondes=10.;
                }

                caducee_serpent_1->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                caducee_baton->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                tas_cailloux->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                sol_eau->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);


            // Sources lumineuses:
                source_soleil=new dksSourceLumineuse("Soleil",-85.3011, 84.4501, 177.137);
                source_1=new dksSourceLumineuse("Source_1",100.,100.,10.);
                source_2=new dksSourceLumineuse("Source_2",-100.,-100.,10.);

                sources[0]=source_soleil;
                sources[1]=source_1;
                sources[2]=source_2;
                source_soleil->determine_mode_visee(DKSOEIL_VISEE_POINT);
                source_soleil->determine_contrainte_verticale(true);

            //zyeux:
                oeil_1=new dksOeil("Oeil_1");
                yeux[0]=oeil_1;
                oeil_courant=oeil_1;


            //----------- Initialisations:
                double axes[9];
                source_soleil->determine_intensite(0.);

                axes={-0.964869,0,0.262733,0.00217545,0.999966,0.00798921,-0.262724,0.0082801,-0.964836};
                repere_arrivee_serpent.determine_axes(axes);

                //---------- Trajectoires de caméra:
                trajectoire_camera_caducee_01=new dksTrajectoire(scene_tunnel->trajectoire_camera_caducee_01);
                trajectoire_camera_caducee_01->determine_echelle_trajectoire(1./scene_tunnel->echelle_caducee);
                camera_pos_trajectoire_01=scene_tunnel->renvoie_interpolation_camera_pos_trajectoire_01();

                if (!trajectoire_camera_caducee_02.charge_trajectoire("trajectoires/camera_caducee_02.traj"))
                {
                    cout<<"ERREUR - Trajectoire "<<"trajectoires/camera_caducee_02.traj"<<" impossible à charger"<<endl;
                    erreur=SCENE_ERREUR_INIT;
                    return;
                }
                trajectoire_camera_caducee_02.determine_drapeau_longueurs(true);


                caducee_serpent_1->determine_position(0.,0.,0.);
                caducee_baton->determine_position(0.,0.,0.);


                //---------- Positionnement du serpent animé:
                // Je ne reprend pas l'objet de la scene du tunnel, car elle sert
                // pour le tunnel. Elle est donc réorientée selon la direction du vecteur générateur.
                // Et puis elle correspond au serpent 1. Ici, c'est le serpent 2 qui bouge...
                //mon_serpent->determine_position(2.16771, 17.0808, -2.11968);
                //axes={0.210557,-0.0779609,0.974468,0.697258,0.710656,-0.0938043,-0.685198,0.699207,0.203992};
                //mon_serpent->determine_axes(axes);

                mon_serpent->transfere_colonne_repere_parent();

                traj_position_serpent->determine_position(0.,0.,0.);    ///ATTENTION si la position est autre que (0.,0.,0.),
                                                                        ///il faut replacer les points dans le repère du parent du serpent !

                trajectoire_caducee.determine_drapeau_longueurs(false);    //Pour éviter des calculs inutiles
                trajectoire_caducee.determine_points(traj_position_serpent->renvoie_sommets(),traj_position_serpent->nbrSommets);
                trajectoire_caducee.tri_points(DKSTRAJECTOIRE_AXE_Y,true);
                trajectoire_caducee.determine_drapeau_longueurs(true);
                mon_serpent->determine_trajectoire_colonne_vertebrale(&trajectoire_caducee,.95,false);


                /*
                ma_trajectoire.determine_drapeau_longueurs(false);    //Pour éviter des calculs inutiles
                ma_trajectoire.determine_points(traj_position_serpent->renvoie_sommets(),traj_position_serpent->nbrSommets);
                ma_trajectoire.tri_points(DKSTRAJECTOIRE_AXE_Y,true);
                ma_trajectoire.determine_drapeau_longueurs(true);
                */


                //------------- Initialise l'animation du serpent:
                if (!trajectoire_deplacement_serpent.charge_trajectoire("trajectoires/serpent_caducee.traj"))
                {
                    cout<<"ERREUR - Trajectoire "<<"trajectoires/serpent_caducee.traj"<<" impossible à charger"<<endl;
                    erreur=SCENE_ERREUR_INIT;
                    return;
                }
                float point_copie[3];
                point_copie={float(mon_serpent->x),float(mon_serpent->y),float(mon_serpent->z)};

                trajectoire_deplacement_serpent.determine_point(0,&point_copie[0]);

                trajectoire_caducee.renvoie_point(trajectoire_caducee.renvoie_nbrPoints()-1,point_copie);
                trajectoire_deplacement_serpent.determine_point(1,&point_copie[0]);
                trajectoire_deplacement_serpent.determine_drapeau_longueurs(true);

                //Alignement du serpent à la trajectoire:
                trajectoire_deplacement_serpent.renvoie_tangente(0.,point_copie);
                double nouvel_axez[3]={double(point_copie[0]),double(point_copie[1]),double(point_copie[2])};
                mon_serpent->initialisation_repere_orthonorme(NULL,NULL,nouvel_axez);
                mon_serpent->contrainte_trajectoire=true;

                /*
                ma_trajectoire.determine_drapeau_longueurs(false);
                ma_trajectoire.determine_points(trajectoire_deplacement_serpent.renvoie_points(),trajectoire_deplacement_serpent.renvoie_nbrPoints());//Pour éviter des calculs inutiles
                ma_trajectoire.determine_drapeau_longueurs(true);
                */


            //------------Création de la scène:scene->ajoute_volumes(volumes_particules,sizeof(volumes_particules)/sizeof(dksVolume*),mes_particules);



                scene=new dksMoteur3d("Scene_Caduce",ecran,NULL,0,volumes,10,yeux,1,sources,3,oeil_1);
                if (scene==NULL || scene->erreur!=DKSM3D_OK)
                {
                    erreur=SCENE_ERREUR_INIT;
                    return;
                }

                scene->ajoute_volumes(volumes_particules,sizeof(volumes_particules)/sizeof(dksVolume*),mes_particules);

                scene->determine_cube_arriere_plan(cube_arriere_plan);

                scene->determine_rendu_ombres(true,source_soleil,DKSECRAN_TEMPON_PROFONDEUR_2048,.75);



            //------ Rendus des surfaces en aplat:
            dksSurface* gueule=mon_serpent->renvoie_surface_nom("gueule");
            gueule->mode_rendu=DKSSURFACE_RENDU_APLAT;
            gueule=machoire->renvoie_surface_nom("gueule");
            gueule->mode_rendu=DKSSURFACE_RENDU_APLAT;

            //------ Initialise les animations
                 //initialise_t0();
                initialise_animations();
                initialise_listes_affichage();

                //Couleur du masque:
                dksSurface* surface_masque=machoire->renvoie_surface_nom("gueule");
                surface_masque->determine_couleur_diffusion(0x00ff00,1.);
                surface_masque=mon_serpent->renvoie_surface_nom("gueule");
                surface_masque->determine_couleur_diffusion(0x00ff00,1.);

                post_rendu->couleur_masque={0.,1.,0.};

                #if EDITEUR_ACTIF
                initialise_editeur();
                #endif

            //---------------- Initd brutale le temps du développement -
                scene->active_arriere_plan();

        }

        //==========================================================
        //      Destructeur
        //==========================================================
        ~Scene_Caduce()
        {
            uint32_t i;
            cout<<"Destruction de la scene Caducee"<<endl;

            if (scene!=NULL) delete scene;
            if (volumes_convertis!=NULL) delete volumes_convertis;
            if (volumes_lwo2!=NULL) delete volumes_lwo2;
            if (trajectoire_camera_caducee_01!=NULL) delete trajectoire_camera_caducee_01;
            if (mes_particules!=NULL) delete mes_particules;
            if (mon_serpent!=NULL) delete mon_serpent;
            for (i=0;i<sizeof(yeux)/sizeof(dksOeil*);i++) if (yeux[i]!=NULL) delete(yeux[i]);
            for (i=0;i<sizeof(sources)/sizeof(dksSourceLumineuse*);i++) if (sources[i]!=NULL) delete(sources[i]);
            for (i=0;i<sizeof(interpolations_positions)/sizeof(Interpolation_Parametre_3d*);i++) if (interpolations_positions[i]!=NULL) delete interpolations_positions[i];
            for (i=0;i<sizeof(interpolations_reperes)/sizeof(Interpolation_Repere*);i++) if (interpolations_reperes[i]!=NULL) delete interpolations_reperes[i];
            for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) if (interpolations_parametres[i]!=NULL) delete interpolations_parametres[i];
            delete volumes_particules[0];
            volumes_particules[0]=NULL;
            for (i=0;i<sizeof(volumes_particules)/sizeof(dksVolume*);i++) if (volumes_particules[i]!=NULL) delete volumes_particules[i];
            if(courbe_interpolation_serpent!=NULL) delete courbe_interpolation_serpent;

            ///Les images sont détruites par la scène précédente:
            //for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) if(textures_arriere_plan[i]!=NULL) delete textures_arriere_plan[i];

            if(cube_arriere_plan!=NULL)delete cube_arriere_plan;

            if(scene_transition!=NULL) delete scene_transition;
            if(oeil_1_starfield!=NULL) delete oeil_1_starfield;
            for (i=0;i<sizeof(textures_arriere_plan_starfield)/sizeof(Image*);i++) if(textures_arriere_plan_starfield[i]!=NULL) delete textures_arriere_plan_starfield[i];
            if (le_starfield!=NULL) delete le_starfield;
        }


        //==========================================================
        //                      Fonctions
        //==========================================================
        virtual void active()
        {
            this->Scene::active();

            scene->active();
        }

        virtual void desactive()
        {
            scene->desactive();
        }

        //----------------------------------------------------------------------------
        //                 Mise à jour de la fenêtre, si on est pas en plein ecran
        //----------------------------------------------------------------------------
        virtual void maj_dimensions_fenetre()
        {
            scene->maj_resolutions_yeux();
        }

        //===============================================================================
        //                          AFFICHAGE
        //===============================================================================

        virtual void affiche(double t)
        {
            this->Scene::affiche(t);
            glDepthMask(GL_TRUE);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mes_particules->maj_flux(4.);
            mes_particules->maj_volumes();


            post_rendu->alpha_tempon_ecran=1.;

            //Rendu de la scene 1:
                scene->active();
                scene->maj(t);
                post_rendu->affiche_tempon_ecran_1(oeil_1);
                affiche_trajectoire(&ma_trajectoire);
                scene->desactive();
            //Rendu de la scene 2:

                scene_transition->active();
                le_starfield->maj_champ_etoiles_base(oeil_1_starfield->repere_absolu->x,oeil_1_starfield->repere_absolu->y,oeil_1_starfield->repere_absolu->z);
                scene_transition->maj(t);
                post_rendu->affiche_tempon_ecran_2(oeil_1_starfield);
                scene_transition->desactive();
            //Superpose les scènes:
                /*
                post_rendu->depart_superposition_tempons();
                post_rendu->superpose_tempon(DKSECRAN_TEMPON_ECRAN_1);
                post_rendu->superpose_tempon(DKSECRAN_TEMPON_ECRAN_2);
                post_rendu->fin_superposition_tempons();
                */

                post_rendu->masque_2_tempons(TX_ECRAN_1,TX_ECRAN_2,0);
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
            double e=scene_tunnel->echelle_caducee;
            interpolations_reperes[VISE_SERPENT]=new Interpolation_Repere(-1.,oeil_1,&repere_arrivee_serpent,10.,INTERPOLATION_COSINUS);
            point_vise_0[0]=oeil_1->direction_regard->x*distance_point_vise+oeil_1->repere_absolu->x;
            point_vise_0[1]=oeil_1->direction_regard->y*distance_point_vise+oeil_1->repere_absolu->y;
            point_vise_0[2]=oeil_1->direction_regard->z*distance_point_vise+oeil_1->repere_absolu->z;
            point_vise_1={-389.963/e,2614.03/e,-80.1843/e};
            interpolations_positions[POINT_VISE_SERPENT_1]=new Interpolation_Parametre_3d(-1.,point_vise_0,point_vise_1,3.,INTERPOLATION_COSINUS);
            point_vise_0={-0.3,18.1,-1.1}; //Centre des serpents
            source_soleil->determine_point_vise(point_vise_0);
            interpolations_positions[POINT_VISE_CENTRE_SERPENTS]=new Interpolation_Parametre_3d(-1.,point_vise_1,point_vise_0,3.,INTERPOLATION_COSINUS);
            point_vise_1={29.5438, 16.842, 29.8626}; //Point de visée du regard de la vue d'ensemble
            interpolations_positions[POINT_VISE_SCENE]=new Interpolation_Parametre_3d(-1.,point_vise_0,point_vise_1,10.,INTERPOLATION_COSINUS);

            interpolations_parametres[CAMERA_POS_TRAJECTOIRE_02]=new Interpolation_Parametre(-1.,0,1.,15.,INTERPOLATION_COSINUS);

            courbe_interpolation_serpent=new Courbe_Bezier_Plan(0.,0., 0.4,0., 0.70,0.9, 1.,1.);
            interpolations_parametres[SERPENT_TRAJECTOIRE]=new Interpolation_Parametre(14.,0,1.,20.,INTERPOLATION_BEZIER,courbe_interpolation_serpent);
            //interpolations_parametres[SERPENT_TRAJECTOIRE]=new Interpolation_Parametre(-1.,0,1.,21.25,INTERPOLATION_COSINUS);
            interpolations_parametres[OUVERTURE_SOURCE_SOLEIL]=new Interpolation_Parametre(-1.,5.5,18,15.,INTERPOLATION_LINEAIRE);
            interpolations_parametres[INTENSITE_SOURCE_SOLEIL]=new Interpolation_Parametre(-1.,0.,3.,10.,INTERPOLATION_COSINUS);
            interpolations_parametres[INTENSITE_SOURCES_DECREMENTE]=new Interpolation_Parametre(-1.,1.,0.25,5.,INTERPOLATION_COSINUS);
            interpolations_parametres[INTENSITE_SOURCES_INCREMENTE]=new Interpolation_Parametre(-1.,.25,1.,5.,INTERPOLATION_COSINUS);
            interpolations_parametres[ECHELLE_EAU]=new Interpolation_Parametre(-1.,0.001,1.,5.,INTERPOLATION_COSINUS);
            interpolations_parametres[MACHOIRE]=new Interpolation_Parametre(-1.,0.,-45./180.*M_PI,2.,INTERPOLATION_COSINUS);
            interpolations_parametres[DENTS]=new Interpolation_Parametre(-1.,-90./180.*M_PI,0.,2.,INTERPOLATION_COSINUS);

            sol_eau->determine_echelle_repere(0.001,1.,0.001);

            dents->determine_RTL(0.,-90./180.*M_PI,0.);

        }

        void initialise_listes_affichage()
        {
            post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
            post_rendu->ajoute_rendu(scene_transition,DKSECRAN_TEMPON_ECRAN_2,DKSRENDU_COULEURS);
            //post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_QUART,DKSRENDU_COULEURS_LUMINESCENTES);
            //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_QUART,DKSRENDU3D_COULEURS);
        }


        public:
        //----------------------------------------------------------------------------
        //                  Etat de la scène au départ de l'animation
        //----------------------------------------------------------------------------
        virtual void initialise_t0(double t0)
        {
            this->Scene::initialise_t0(t0);
            alpha_scene=1.;
            double e=scene_tunnel->echelle_caducee;

            oeil_courant=oeil_1;

            source_soleil->determine_couleur_diffusion(0xfff0a0);

            dksOeil* oeil_tunnel=scene_tunnel->oeil_2;
            ouverture_camera=oeil_tunnel->renvoie_ouverture();

            //source_1->determine_intensite(0.1);
            //source_2->determine_intensite(0.1);
            source_soleil->determine_ouverture(5.5);
            //source_soleil->determine_mode_visee(DKSOEIL_VISEE_CONTENEUR);
            //source_soleil->determine_conteneur_vise(oeil_1);

            ///----------------------------------------------------------------------------------------------------------------- activer lors de la démo intégrale
            //Copie les dernières positions de la scène précédente:


                dksRepere* repere_transition=scene_tunnel->transition_caducee_particules->repere_absolu;
                dksRepere::determine_repere_relatif(oeil_tunnel->repere_absolu,repere_transition,oeil_1);
                oeil_1->calcul_repere_absolu();
                oeil_1->a_ete_deplace();

                source_1->copie_parametres(scene_tunnel->source_1);
                source_2->copie_parametres(scene_tunnel->source_2);
                source_1->copie(scene_tunnel->source_1->repere_absolu);
                source_2->copie(scene_tunnel->source_2->repere_absolu);

                dksRepere::determine_repere_relatif(scene_tunnel->source_1->repere_absolu,repere_transition,source_1);
                dksRepere::determine_repere_relatif(scene_tunnel->source_2->repere_absolu,repere_transition,source_2);
                source_1->calcul_repere_absolu();
                source_2->calcul_repere_absolu();
                scene->supprime_enfant(source_1);
                scene->supprime_enfant(source_2);
                oeil_1->ajoute_enfant_absolu(source_1);
                oeil_1->ajoute_enfant_absolu(source_2);

                source_1->produit_reel(1./e);
                source_2->produit_reel(1./e);
                oeil_1->produit_reel(1./e);
                source_1->a_ete_deplace();
                source_2->a_ete_deplace();



                //Pour obtenir les dernières coordonnées de la scène précédente: (raccord)
                /*
                oeil_1->toString();
                cout<<endl;
                source_1->toString();
                cout<<endl;
                source_2->toString();
                cout<<camera_pos_trajectoire_01->t0<<" "<<position_temps<<endl;
                */



                ///----------------------------------------------------------------------------------------------------- activer lors de la sequence seule
                /*
                scene->conteneur_principal.supprime_enfant(source_1);
                scene->conteneur_principal.supprime_enfant(source_2);
                oeil_1->ajoute_enfant(source_1);
                oeil_1->ajoute_enfant(source_2);
                camera_pos_trajectoire_01->t0=0.;
                double axeX[3],axeY[3],axeZ[3];
                axeX={-0.57211,0.0271839,0.819727};
                axeY={0.141298, 0.987774, 0.065859};
                axeZ={-0.807914,0.153504,-0.568956};
                oeil_1->initialisation_repere_orthonorme(axeX,axeY,axeZ);
                oeil_1->determine_position(89.8487/e,1570.81/e,-342.185/e);
                source_1->determine_position(-25.7505/e,48.4791/e,-130.716/e);
                source_2->determine_position(17.2564/e,-31.5495/e,137.138/e);

                oeil_1->a_ete_deplace();
                source_1->a_ete_deplace();
                source_2->a_ete_deplace();
                */




            source_1->determine_attenuation(scene_tunnel->source_1->renvoie_attenuation_constante(),
                                                scene_tunnel->source_1->renvoie_attenuation_lineaire()*e,
                                                scene_tunnel->source_1->renvoie_attenuation_carree());
            source_2->determine_attenuation(scene_tunnel->source_2->renvoie_attenuation_constante(),
                                            scene_tunnel->source_2->renvoie_attenuation_lineaire()*e,
                                            scene_tunnel->source_2->renvoie_attenuation_carree());

            //...Et voilà, on a le même point de vue!

            oeil_1->determine_ouverture(ouverture_camera);
            oeil_courant=oeil_1;

            interpolations_reperes[VISE_SERPENT]->determine_parametres(-1.,oeil_1,&repere_arrivee_serpent,10.,INTERPOLATION_COSINUS);

            /*
            point_vise_0[0]=oeil_1->direction_regard->x*distance_point_vise+oeil_1->repere_absolu->x;
            point_vise_0[1]=oeil_1->direction_regard->y*distance_point_vise+oeil_1->repere_absolu->y;
            point_vise_0[2]=oeil_1->direction_regard->z*distance_point_vise+oeil_1->repere_absolu->z;
            interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[0]=point_vise_0[0];
            interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[1]=point_vise_0[1];
            interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[2]=point_vise_0[2];
            */
            /*
            oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
            oeil_1->determine_point_vise(interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart);
            */


            scene->determine_couleur_fond(0.0,0.0,0.0,0.);

            //---------- Particules au premier plan:
            double* coordonnees_points=mes_particules->renvoie_points();
            coordonnees_points[0*3+1]=65.;
            coordonnees_points[1*3+1]=60;
            coordonnees_points[2*3+1]=0.;
            coordonnees_points[3*3+1]=5.;

            //--------- Post rendu:
            post_rendu->alpha_tempon_quart=1.;
            post_rendu->alpha_tempon_ecran=1.;
            post_rendu->largeur_flou=25.;
            post_rendu->hauteur_flou=25.;
            post_rendu->saturation_horizontale_flou=2.;
            post_rendu->saturation_verticale_flou=2.;

            //scene->t0=t0;

        }



        //======================================================
        //               Attente du retour du faisceau
        //              Gestion des animations
        //=======================================================

        virtual void attente_retour_faisceau()
        {
            if (!drapeau_initialisee || drapeau_scene_terminee) return;
            //return;

            float intensite_sources;



            if (position_temps_scene<15)
            {
                if (!etapes[SERPENT])
                {

                    point_vise_0[0]=oeil_1->direction_regard->x*distance_point_vise+oeil_1->repere_absolu->x;
                    point_vise_0[1]=oeil_1->direction_regard->y*distance_point_vise+oeil_1->repere_absolu->y;
                    point_vise_0[2]=oeil_1->direction_regard->z*distance_point_vise+oeil_1->repere_absolu->z;
                    interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[0]=point_vise_0[0];
                    interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[1]=point_vise_0[1];
                    interpolations_positions[POINT_VISE_SERPENT_1]->valeur_depart[2]=point_vise_0[2];

                    oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
                    oeil_1->determine_contrainte_verticale(false);
                    etapes[SERPENT]=true;
                }
                interpolations_positions[POINT_VISE_SERPENT_1]->renvoie_valeur_parametre(position_temps_scene,point_vise_0);
                oeil_1->determine_point_vise(point_vise_0);
                //mon_cube->determine_position(point_vise_0);

                position_trajectoire_camera=camera_pos_trajectoire_01->renvoie_valeur_parametre(position_temps); ///+14 a retirer après le développement !!!
                //cout<<"T:"<<position_temps<<"position scene caducee: "<<position_trajectoire_camera<<endl;

                trajectoire_camera_caducee_01->renvoie_position(position_trajectoire_camera,position);
                oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));
                interpolations_reperes[VISE_SERPENT]->renvoie_orientation_repere(position_temps_scene,oeil_1);
                source_soleil->determine_intensite(interpolations_parametres[INTENSITE_SOURCE_SOLEIL]->renvoie_valeur_parametre(position_temps_scene));
                intensite_sources=interpolations_parametres[INTENSITE_SOURCES_DECREMENTE]->renvoie_valeur_parametre(position_temps_scene);
                source_1->determine_intensite(intensite_sources);
                source_2->determine_intensite(intensite_sources);
                //Serpent:
                position_trajectoire_serpent=interpolations_parametres[SERPENT_TRAJECTOIRE]->renvoie_valeur_parametre(position_temps_scene);
                trajectoire_deplacement_serpent.renvoie_position(position_trajectoire_serpent,position);
                mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));
                double echelle_surface_eau=interpolations_parametres[ECHELLE_EAU]->renvoie_valeur_parametre(position_temps_scene);
                sol_eau->determine_echelle_repere(echelle_surface_eau,1.,echelle_surface_eau);

            }

            else if (position_temps_scene<30)
            {
                if (!etapes[TRAVELLING_CADUCEE])
                {
                    oeil_1->determine_contrainte_verticale(true);
                    source_soleil->determine_mode_visee(DKSOEIL_VISEE_LIBRE);
                    //oeil_1->supprime_enfant_absolu(source_1);
                    //oeil_1->supprime_enfant_absolu(source_2);
                    //scene->conteneur_principal.ajoute_enfant_absolu(source_1);
                    //scene->conteneur_principal.ajoute_enfant_absolu(source_2);
                    //source_1->a_ete_deplace();
                    //source_2->a_ete_deplace();
                    //camera_pos_trajectoire_01->enveloppe->type_interpolation=INTERPOLATION_COSINUS;
                    etapes[TRAVELLING_CADUCEE]=true;
                }
                intensite_sources=interpolations_parametres[INTENSITE_SOURCES_INCREMENTE]->renvoie_valeur_parametre(position_temps_scene);
                source_1->determine_intensite(intensite_sources);
                source_2->determine_intensite(intensite_sources);
                if (!interpolations_positions[POINT_VISE_CENTRE_SERPENTS]->drapeau_terminee)
                {
                    interpolations_positions[POINT_VISE_CENTRE_SERPENTS]->renvoie_valeur_parametre(position_temps_scene,point_vise_0);
                    oeil_1->determine_point_vise(point_vise_0);
                    //mon_cube->determine_position(point_vise_0);
                }
                else
                {
                    interpolations_positions[POINT_VISE_SCENE]->renvoie_valeur_parametre(position_temps_scene,point_vise_0);
                    oeil_1->determine_point_vise(point_vise_0);
                    //mon_cube->determine_position(point_vise_0);

                }
                source_soleil->determine_ouverture(interpolations_parametres[OUVERTURE_SOURCE_SOLEIL]->renvoie_valeur_parametre((position_temps_scene)));

                position_trajectoire_camera=interpolations_parametres[CAMERA_POS_TRAJECTOIRE_02]->renvoie_valeur_parametre(position_temps_scene);
                trajectoire_camera_caducee_02.renvoie_position(position_trajectoire_camera,position);
                oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));
                //Serpent:
                position_trajectoire_serpent=interpolations_parametres[SERPENT_TRAJECTOIRE]->renvoie_valeur_parametre(position_temps_scene);
                trajectoire_deplacement_serpent.renvoie_position(position_trajectoire_serpent,position);
                mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));
            }


            //Temps d'immobilité de l'oeil avant la scène suivante:
            else if (position_temps_scene<32)
            {
                if (!etapes[VUE_ENSEMBLE_CADUCEE])
                {
                    etapes[VUE_ENSEMBLE_CADUCEE]=true;
                }
                //point_vise_0={mon_cube->x,mon_cube->y,mon_cube->z};
                //oeil_1->determine_point_vise(point_vise_0);
                //Serpent:

                position_trajectoire_serpent=interpolations_parametres[SERPENT_TRAJECTOIRE]->renvoie_valeur_parametre(position_temps_scene);
                trajectoire_deplacement_serpent.renvoie_position(position_trajectoire_serpent,position);
                mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));

            }

            //Ouverture de la machoire:
            else if (position_temps_scene<34)
            {
                if (!etapes[OUVERTURE_MACHOIRE])
                {
                    etapes[OUVERTURE_MACHOIRE]=true;
                    oeil_1->determine_limites_profondeur(0.1,600.);
                }
                //point_vise_0={mon_cube->x,mon_cube->y,mon_cube->z};
                //oeil_1->determine_point_vise(point_vise_0);
                //Serpent:

                position_trajectoire_serpent=interpolations_parametres[SERPENT_TRAJECTOIRE]->renvoie_valeur_parametre(position_temps_scene);
                trajectoire_deplacement_serpent.renvoie_position(position_trajectoire_serpent,position);
                mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));
                //Ouverture de la machoire:
                machoire->determine_RTL(0.,interpolations_parametres[MACHOIRE]->renvoie_valeur_parametre(position_temps_scene),0.);
                dents->determine_RTL(0.,interpolations_parametres[DENTS]->renvoie_valeur_parametre(position_temps_scene),0.);


                float position_parcours_oeil_starfield=position_trajectoire_oeil_1_starfield->renvoie_valeur_parametre(position_temps);
                trajectoire_oeil_01_starfield.renvoie_position(position_parcours_oeil_starfield,position);
                oeil_1_starfield->determine_position(position);

            }


            else
            {

                if (!etapes[DERNIERE_ETAPE])
                {
                        /*
                        trajectoire_camera_caducee_02.renvoie_position(1.,position);
                        oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));
                        //oeil_1->determine_position(22.5692, 14.2831, -28.9301);
                        source_soleil->determine_intensite(3.);
                        source_soleil->determine_ouverture(18.);
                        oeil_1->determine_contrainte_verticale(true);

                        oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
                        oeil_1->determine_point_vise(interpolations_positions[POINT_VISE_SCENE]->valeur_fin);
                        oeil_1->determine_mode_visee(DKSOEIL_VISEE_LIBRE);
                        machoire->determine_RTL(0.,interpolations_parametres[MACHOIRE]->valeur_fin,0.);
                        //trajectoire_deplacement_serpent.renvoie_position(1.,position);
                        //mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));
                        oeil_1->determine_limites_profondeur(0.1,600.);
                        */

                    drapeau_scene_terminee=true;
                    etapes[DERNIERE_ETAPE]=true;
                }
                        /*
                        position_trajectoire_serpent=interpolations_parametres[SERPENT_TRAJECTOIRE]->renvoie_valeur_parametre(position_temps_scene);
                        trajectoire_deplacement_serpent.renvoie_position(position_trajectoire_serpent,position);
                        mon_serpent->determine_position(double(position[0]),double(position[1]),double(position[2]));
                        */

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
            vitesse_deplacement=1.0;
            vitesse_rotation=0.1;

            mon_serpent->masse=40.;
            mon_serpent->determine_coefficient_frottement(.1,20.);

            oeil_1_starfield->valeur_propulsion_horizontale=5000.;
            oeil_1_starfield->masse=10.;
            oeil_1_starfield->determine_coefficient_frottement(1.,100.);

            position_point=0.1;

            position_trajectoire=0.;
            pas_position_trajectoire=0.001;
            fichier_trajectoire={"trajectoires/serpent_caducee.traj"};
            conteneur_selectionne=NULL;

            conteneurs_selectionnables.push_back(oeil_1);
            conteneurs_selectionnables.push_back(oeil_1_starfield);
            conteneurs_selectionnables.push_back(mon_serpent);
            conteneurs_selectionnables.push_back(machoire);
            conteneurs_selectionnables.push_back(oeil_droit);
            conteneurs_selectionnables.push_back(oeil_gauche);
            conteneurs_selectionnables.push_back(source_soleil);

            conteneur_selectionne=oeil_1;

            drapeau_alignement_conteneur_trajectoire=true;

        }

        //===================================================
        //          Affiche la trajectoire éditée
        //===================================================
        void affiche_trajectoire(dksTrajectoire* trajectoire)
        {
            //glLoadIdentity();
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            trajectoire->affiche_courbe(20,0x40ffff00);
            trajectoire->affiche_points(0x80ff0000,5);
            trajectoire->affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        //===================================================
        //          Affiche l'origine du conteneur sélectionné
        //===================================================
        void affiche_origine_conteneur(dksConteneur* cont)
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glPushMatrix();
            glMultMatrixd(cont->matrice.matrice);
            glPointSize(6);
            glColor3f(1.,0.,0.);
            glBegin(GL_POINTS);
                glVertex3d(0.,0.,0.);
            glEnd();
            glPopMatrix();
            glEnable(GL_LIGHTING);
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
                    if (ma_trajectoire.renvoie_longueur_totale()>0.)
                    {if (ma_trajectoire.sauve_trajectoire(fichier_trajectoire)) cout<<"Trajectoire "<<fichier_trajectoire<<" sauvée avec succès!"<<endl;}
                    else cout<<"Sauvegarde impossible-pas de trajectoire en mémoire!"<<endl;
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
                    vitesse_deplacement+=0.005; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '-':
                    vitesse_deplacement-=0.005; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '*':
                    vitesse_rotation+=0.01; cout<<"Rotation: "<<vitesse_rotation<<endl;
                    break;
                case '/':
                    vitesse_rotation-=0.01; cout<<"Rotation: "<<vitesse_rotation<<endl;
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

                //............ Déplacement sur la trajectoire:
                case '&':
                     if( drapeau_alignement_conteneur_trajectoire ) conteneur_selectionne->contrainte_trajectoire=true;
                     else conteneur_selectionne->contrainte_trajectoire=false;
                    position_trajectoire-=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;


                    break;
                case 233:// é
                    if( drapeau_alignement_conteneur_trajectoire ) conteneur_selectionne->contrainte_trajectoire=true;
                    else conteneur_selectionne->contrainte_trajectoire=false;
                    position_trajectoire+=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
                    break;

                //............... Affiche les coordonnées:
                case 'r':
                    cout<<"Repère relatif: "<<conteneur_selectionne->axe_x->x<<","<<conteneur_selectionne->axe_x->y<<","<<conteneur_selectionne->axe_x->z<<","
                                            <<conteneur_selectionne->axe_y->x<<","<<conteneur_selectionne->axe_y->y<<","<<conteneur_selectionne->axe_y->z<<","
                                            <<conteneur_selectionne->axe_z->x<<","<<conteneur_selectionne->axe_z->y<<","<<conteneur_selectionne->axe_z->z<<endl;

                    cout<<"Repère absolu: "<<rep->axe_x->x<<","<<rep->axe_x->y<<","<<rep->axe_x->z<<","
                                            <<rep->axe_y->x<<","<<rep->axe_y->y<<","<<rep->axe_y->z<<","
                                            <<rep->axe_z->x<<","<<rep->axe_z->y<<","<<rep->axe_z->z<<endl;
                    break;

                case 178: // ²
                    if(drapeau_alignement_conteneur_trajectoire) drapeau_alignement_conteneur_trajectoire=false;
                    else drapeau_alignement_conteneur_trajectoire=true;
                    cout<<"Alignement trajectoire: "<<drapeau_alignement_conteneur_trajectoire<<endl;
                    break;

                case 'o':
                    if (scene->renvoie_oeil_courant()==oeil_1) {scene->determine_oeil_courant(source_soleil); conteneur_selectionne=source_soleil;}
                    else {scene->determine_oeil_courant(oeil_1);conteneur_selectionne=oeil_1;}
                    break;

                case 'y':
                    source_soleil->determine_ouverture(source_soleil->renvoie_ouverture()+0.5);
                    cout<<source_soleil->renvoie_ouverture()<<endl;
                    break;
                case 't':
                    source_soleil->determine_ouverture(source_soleil->renvoie_ouverture()-0.5);
                    cout<<source_soleil->renvoie_ouverture()<<endl;
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
#endif // SCENE_CADUCE_H_INCLUDED
