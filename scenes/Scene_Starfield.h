
/*

            Scène avec starfield, nébuleuses (nuages volumiques?), planètes et vaisseau spatial

*/


#include "Scene.h"
#include "../dksMoteur3d/Volumes/dksCube.h"
#include "../dksMoteur3d/Particules/dksStarfield.h"
#include "../dksMoteur3d/Particules/dksSprite3d.h"
#include "../dksMoteur3d/Particules/dksSprites.h"
#include "../dksMoteur3d/Particules/dksParticulesVolumes.h"
#include "../dksMoteur3d/Particules/dksAnneauElectrique.h"
#include "../dksMoteur3d/Particules/dksJetParticules.h"
#include "../chargeurs_images/Image.h"
#include "../dksMoteur3d/Articules/Papillon.h"
#include "../dksMoteur3d/Rendu/Effets_Speciaux/ES_TransitionTexturesCouleurAxe.h"


class Scene_Starfield : public Scene
{
    public:
        dksChargeLWO2* volumes_lwo2;
        dksConvertionLWO2* volumes_convertis;
        dksConteneur* conteneur_vaisseau;

        dksOeil* oeil_1;
        dksOeil* oeil_conteneur;    //Le conteneur de l'oeil

        dksOeil* oeil_debug;

        dksSourceLumineuse* source_1;
        dksSourceLumineuse* source_2;
        dksMoteur3d* scene;
        Element_PostRendu* element_rendu_scene;
        dksStarfield* le_starfield;
        dksVolume* le_vaisseau;

        //------Surfaces du vaisseau:
        enum
        {
            VAISSEAU_carlingue_epaisseur_ailes,
            VAISSEAU_dessous_petites_ailes,
            VAISSEAU_dessus_petites_ailes,
            VAISSEAU_bout,
            VAISSEAU_verriere,
            VAISSEAU_carlingue,
            VAISSEAU_carlingue_dessus,
            VAISSEAU_carlingue_epaisseur,
            VAISSEAU_interieur_cabine,
            VAISSEAU_sol_cabine,
            VAISSEAU_carlingue_dessus_cokpit,
            VAISSEAU_epaisseur_aileron,
            VAISSEAU_cotes_aileron,
            VAISSEAU_dessus_grandes_ailes,
            VAISSEAU_dessous_grandes_ailes,
            VAISSEAU_reacteurs,
            VAISSEAU_tuyeres
        };
        dksSurface* surfaces_vaisseau[17];


        dksCube* cube_arriere_plan;

        dksSprites* sprites;
        dksSprite3d* mon_sprite;

        dksAnneauElectrique* mon_anneau;
        dksJetParticules* etincelles;

        dksVolume* volumes[10];
        dksOeil* yeux[10];
        dksSourceLumineuse* sources[2];
        dksConteneur* conteneurs[1];
        float ouverture_camera;

        Scene_Caduce* scene_caduce;

        Annelide* mon_ver;
        Image* images_ver_1[4];
        Image* images_ver_2[4];
        Image* images_ver_3[4];
        float ver_amplitude_x;
        float ver_amplitude_y;
        float ver_amplitude_perturbation_x;
        float ver_amplitude_perturbation_y;


        ES_TransitionTexturesCouleurAxe* transition_ver;
        ES_TransitionTexturesCouleurAxe* transition_vaisseau;

        //Portes:
        #define NOMBRE_MODELES_PORTES 4
        dksChargeLWO2* portes_lwo[NOMBRE_MODELES_PORTES+1];
        dksConvertionLWO2* portes_dks[NOMBRE_MODELES_PORTES+1];
        double echelle_portes;


        #define NOMBRE_PORTES 50
        dksParticulesVolumes* particules_portes;
        dksVolume* cadres[NOMBRE_PORTES];

        dksVolume* battants[NOMBRE_PORTES];
        dksVolume* the_cadre; //Le cadre de la porte du passage à la scène suivante
        dksVolume* the_battant; //Le battant de la porte du passage à la scène suivante
        dksVolume* surface_passage; //La surface servatn de cache à l'affichage de la scène suivante
        Element_PostRendu* element_rendu_surface_passage;
        Particule_Volume* the_porte_particule; //La particule de la porte de passage

        //============ Trajectoires:
        Courbe_Bezier_Plan* courbe_trajectoire_oeil_01;
        dksTrajectoire* trajectoire_oeil_01;
        dksTrajectoire trajectoire_vaisseau;
        dksTrajectoire trajectoire_poursuite_vaisseau;

        //======================== Scène de transition avec la scène suivante:
        dksMoteur3d* scene_transition;
        dksCube* cube_arriere_plan_scene_arbre;
        dksOeil* oeil_arbre;
        dksSourceLumineuse* source_1_arbre;
        dksSourceLumineuse* source_2_arbre;
        dksVolume* copie_vaisseau;
        Papillon* papillon;
        dksTrajectoire trajectoire_vaisseau_transition;

        Image* textures_arriere_plan_arbre[6];


        //----------------------------------------------

        //============ Données d'animation

        double pos_anneau;      //Varie entre 0 et 1

        //Interpolations de paramètre simple:
        enum    {
                    OUVERTURE_PORTE,
                    TRAJECTOIRE_OEIL_1,
                    POSITION_ANNEAU,
                    FACTEUR_ONDULATION_VER,
                    FACTEUR_LUMINESCENCE_VER,
                    FACTEUR_LUMINESCENCE_VAISSEAU,
                    FACTEUR_VITESSE_ROTATION_OEIL_1,
                    LUMINOSITE_REACTEUR,
                    TRAJECTOIRE_VAISSEAU,
                    TRAJECTOIRE_POURSUITE_VAISSEAU,
                    FACTEUR_ROTATION_OEIL,
                    TRAJECTOIRE_VAISSEAU_TRANSITION
                };

        Interpolation_Parametre* interpolations_parametres[12];

        //Interpolations de position
        enum    {
                    POSITIONNE_OEIL
                };
        Interpolation_Parametre_3d* interpolations_positions[1];

        //============= Données de l'éditeur:

        dksCube* mon_cube;

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
        dksConteneur* conteneur_mem;

        //Drapeau d'initialisation des phases
        double vitesse_rotation_oeil_transformation;
    enum {
            REJOINT_VER,
            TRANSFORMATION_VER_FILS,
            TRANSFORMATION_VER_VAISSEAU,
            TRANSFORMATION_VAISSEAU_CARLINGUE,
            ARRET_OEIL,
            DEMARRAGE_VAISSEAU,
            PARCOURS_VAISSEAU,
            DEPART_OUVERTURE_PORTE,
            TRANSITION,
            TRANSITION_FLASH
            };
    bool etapes[9];

        //============================================================================
        //              CONSTRUCTEUR
        //============================================================================
        Scene_Starfield(char* p_nom,dksEcran* p_ecran,Musique* p_la_musique,Scene_Caduce* p_scene_caduce):Scene(p_nom,p_ecran,p_la_musique)
        {
            uint32_t i;

            mon_cube=NULL;

            oeil_1=NULL;
            oeil_conteneur=NULL;
            scene=NULL;
            le_starfield=NULL;
            source_1=NULL;
            source_2=NULL;
            papillon=NULL;
            conteneur_vaisseau=NULL;

            oeil_debug=NULL;

            mon_sprite=NULL;
            sprites=NULL;
            mon_ver=NULL;
            for(i=0;i<sizeof(images_ver_1)/sizeof(Image*);i++) images_ver_1[i]=NULL;
            for(i=0;i<sizeof(images_ver_2)/sizeof(Image*);i++) images_ver_2[i]=NULL;
            for(i=0;i<sizeof(images_ver_3)/sizeof(Image*);i++) images_ver_3[i]=NULL;

            transition_ver=NULL;
            transition_vaisseau=NULL;

            mon_anneau=NULL;
            etincelles=NULL;

            particules_portes=NULL;

            cube_arriere_plan=NULL;

            courbe_trajectoire_oeil_01=NULL;

            scene_transition=NULL;
            oeil_arbre=NULL;
            source_1_arbre=NULL;
            source_2_arbre=NULL;
            copie_vaisseau=NULL;
            cube_arriere_plan_scene_arbre=NULL;


            for (i=0;i<sizeof(cadres)/sizeof(dksVolume*);i++) cadres[i]=NULL;
            for (i=0;i<sizeof(battants)/sizeof(dksVolume*);i++) battants[i]=NULL;

            for (i=0;i<sizeof(portes_lwo)/sizeof(dksChargeLWO2*);i++) portes_lwo[i]=NULL;
            for (i=0;i<sizeof(portes_dks)/sizeof(dksConvertionLWO2*);i++) portes_dks[i]=NULL;

            for (i=0;i<sizeof(etapes)/sizeof(bool);i++) etapes[i]=false;

            for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres[i]=NULL;
            for (i=0;i<sizeof(interpolations_positions)/sizeof(Interpolation_Parametre_3d*);i++) interpolations_positions[i]=NULL;


            le_vaisseau=NULL;
            volumes_lwo2=NULL;
            volumes_convertis=NULL;

            ouverture_camera=60.; ///!!! remis à jour par initialise_t0

            scene_caduce=p_scene_caduce;


            mon_cube=new dksCube("Cube_edition");


            //------------- Chargement des volumes:

            volumes_lwo2=new dksChargeLWO2("LWO2/vaisseau.lwo","textures/vaisseau/");
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


            //----------- Chargement des portes:
            char fichiers_portes[NOMBRE_MODELES_PORTES+1][64]=
            {
                {"LWO2/portes/porte_01.lwo"},
                {"LWO2/portes/porte_02.lwo"},
                {"LWO2/portes/porte_03.lwo"},
                {"LWO2/portes/porte_04.lwo"},
                {"LWO2/portes/porte_05.lwo"}
            };

            for (i=0;i<NOMBRE_MODELES_PORTES+1;i++)
            {
                portes_lwo[i]=new dksChargeLWO2(fichiers_portes[i]);
                if (portes_lwo[i]==NULL || portes_lwo[i]->erreur!=DKSCHARGELWO2_OK) { erreur=SCENE_ERREUR_INIT; return; }
                portes_dks[i]=new dksConvertionLWO2(portes_lwo[i]);
                if (portes_dks[i]==NULL || portes_dks[i]->erreur!=DKSCONVERTIONLWO2_OK) { erreur=SCENE_ERREUR_INIT; return; }
            }

            //Création des occurences des portes:
            #define INDICE_PORTE_PASSAGE 4
            echelle_portes=50.;
            for(i=0;i<NOMBRE_PORTES;i++)
            {
                dksVolume* vol;
                uint32_t indice_porte;
                double pivot[3];
                double pivot_cadre[3];

                //La porte de passage à la scène suivante:
                if(i==INDICE_PORTE_PASSAGE)
                {
                    indice_porte=4;
                    vol=portes_dks[indice_porte]->renvoie_volume_nom("cadre"); // On copie quand même pour pas compliquer les inits
                    cadres[i]=new dksVolume(*vol);
                    vol=portes_dks[indice_porte]->renvoie_volume_nom("porte");
                    battants[i]=new dksVolume(*vol);

                    //cadres[i]->determine_affecte_par_brouillard(false);
                    //battants[i]->determine_affecte_par_brouillard(false);

                    surface_passage=portes_dks[indice_porte]->renvoie_volume_nom("passage");
                    the_battant=battants[i];
                    the_cadre=cadres[i];

                    surface_passage->determine_echelle_geometrique(echelle_portes);
                    portes_dks[indice_porte]->renvoie_pivot_nom("cadre",pivot_cadre);
                    portes_dks[indice_porte]->renvoie_pivot_nom("passage",pivot);
                    pivot[0]=echelle_portes*(pivot_cadre[0]-pivot[0]);
                    pivot[1]=-echelle_portes*(pivot_cadre[1]-pivot[1]);
                    pivot[2]=-echelle_portes*(pivot_cadre[2]-pivot[2]);
                    //cout<<pivot[0]<<" "<<pivot[1]<<" "<<pivot[2]<<endl;
                    surface_passage->determine_position(pivot);
                    //cadres[i]->ajoute_enfant(surface_passage); ///Effectué APRES la création de la scène...

                    //Couleur du masque:
                    surface_passage->determine_affecte_par_brouillard(false);
                    dksSurface* passage=surface_passage->renvoie_surface_nom("passage");
                    passage->mode_rendu=DKSSURFACE_RENDU_APLAT;
                    passage->determine_couleur_diffusion(0x00ffff,1.);
                    post_rendu->couleur_masque={0.,1.,1.};
                    surface_passage->determine_mode_rendu(DKSVOLUME_MODE_LISTE);
                }

                //Les autres portes:
                else
                {
                    indice_porte=i & 0x3;
                    vol=portes_dks[indice_porte]->renvoie_volume_nom("cadre");
                    cadres[i]=new dksVolume(*vol);
                    vol=portes_dks[indice_porte]->renvoie_volume_nom("porte");
                    battants[i]=new dksVolume(*vol);
                }

                cadres[i]->determine_echelle_geometrique(echelle_portes);
                battants[i]->determine_echelle_geometrique(echelle_portes);
                battants[i]->determine_rendu_faces_transparentes(false);
                //cadres[i]->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
                //battants[i]->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);

                cadres[i]->determine_mode_rendu(DKSVOLUME_MODE_LISTE);
                battants[i]->determine_mode_rendu(DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES);

                portes_dks[indice_porte]->renvoie_pivot_nom("cadre",pivot);
                pivot[0]=pivot[0]*echelle_portes;
                pivot[1]=-pivot[1]*echelle_portes;
                pivot[2]=-pivot[2]*echelle_portes;
                //cout<<pivot[0]<<" "<<pivot[1]<<" "<<pivot[2]<<endl;
                battants[i]->determine_position(pivot);
                cadres[i]->ajoute_enfant(battants[i]);

                /*
                for(uint16_t j=0;j<battants[i]->nbrSurfaces;j++)
                {
                    dksSurface* surf=battants[i]->surfaces[j];
                    surf->luminescence=.1;
                    cout<<"luminescence:"<<surf->luminescence<<" - R:"<<surf->couleur_luminescence[0]<<" V:"<<surf->couleur_luminescence[1]<<" B:"<<surf->couleur_luminescence[2]<<endl;
                }
                */
            }

            //battants[0]->drapeau_debug=true;
            //Création du champ de particules pour la répartition des portes:
            particules_portes=new dksParticulesVolumes("Particules_portes",0.,0.,0.,cadres,NOMBRE_PORTES);

            //particules_portes->random_points(-500,500,-500,500.,0,1500.);
            //particules_portes->sauve_points("particules/portes.crd");
            if(!particules_portes->charge_points("particules/portes.crd"))
            {
                cout<<"************* ERREUR LORS DU CHARGEMENT DES COORDONNEES DES PORTES !!!! ***********************"<<endl;
            }
            //particules_portes->determine_echelle_geometrique(echelle_portes/50.);

            //particules_portes->random_orientations();
            //particules_portes->sauve_orientations("particules/portes.rtl");
            if(!particules_portes->charge_orientations("particules/portes.rtl"))
            {
                cout<<"************* ERREUR LORS DU CHARGEMENT DES ANGLES RTL DES PORTES !!!! ***********************"<<endl;
            }

            //particules_portes->random_rotations(M_PI/180.*0.5,M_PI/180.*3.);
            //particules_portes->sauve_rotations("particules/porte.vrot");
            if(!particules_portes->charge_rotations("particules/porte.vrot"))
            {
                cout<<"************* ERREUR LORS DU CHARGEMENT DES VITESSES DE ROTATIONS DES PORTES !!!! ***********************"<<endl;
            }

            //Initialisation de la porte de passage à la scène suivante:
            particules_portes->determine_vitesse_rotation_particule(INDICE_PORTE_PASSAGE,0.,0.,0.);
            the_porte_particule=(Particule_Volume*)particules_portes->renvoie_particule(INDICE_PORTE_PASSAGE);
            the_porte_particule->lacet+=M_PI;


            //particules_portes->determine_vitesses_rotations(0.,0.,0.);

           // particules_portes->determine_position();

                //dksVolume* repartition=volumes_convertis->renvoie_volume_nom("repartition_particules");
                //if (repartition==NULL) {erreur=SCENE_ERREUR_INIT;return;}
                //mes_particules->determine_points_XZ(repartition->renvoie_sommets(),NBRVOLUMESPARTICULES);

                //mes_particules->determine_vitesses_rotations(0.,0.,M_PI/180.*2.);

            //---------- Création des éléments:

            //Création du starfield:

            le_starfield=new dksStarfield("Champ_etoiles",0.,0.,0.,2000,1000.,30.,scene_caduce->etoile_1);
            le_starfield->drapeau_destruction_image=false;

            //Création des sprites:
            mon_sprite=new dksSprite3d("test_sprite","textures/starfield/nebuleuse_02.tif",.02);
            sprites=new dksSprites("Sprites_de_la_scene",0.,0.,0.,1);
            sprites->determine_sprite(0,mon_sprite);
            sprites->determine_point(0,0.,3.,100.);

            //Création des sources:
            source_1=new dksSourceLumineuse("Source_1",100.,100.,-10.);
            source_1->determine_attenuation(1.,0.,0.);

            //Yeux:
            oeil_1=new dksOeil("Oeil_1",0.,0.,0.);
            oeil_conteneur=new dksOeil("Oeil_conteneur",0.,0.,0.);
            oeil_conteneur->ajoute_enfant(oeil_1);
            oeil_courant=oeil_1;

            //----------- Initialise le ver:

            if(!initialise_ver()) return;


            // ----------- Anneau:
            mon_anneau=new dksAnneauElectrique("Anneau_electrique_starfield",0.,0.,8.,
                                               50,
                                               1.,3.);

            mon_anneau->luminescence=0.75;
            mon_anneau->couleur_points=0xff44ffff;
            mon_anneau->couleur_min=0xff008080;
            mon_anneau->vitesse_eclair=0.2;
            mon_anneau->marge_vitesse_eclair=0.1;
            mon_anneau->rayon_central=3.;

            etincelles=new dksJetParticules("etincelles",0.,0.,0.,2000);
            etincelles->random_points(-5.,5.,-5.,5.,-5.,5.);
            etincelles->drapeau_vie_limitee=true;
            etincelles->random_durees_vie(1.,3.);  //1 3
            etincelles->desactive_particules();
            etincelles->ouverture=10.;
            etincelles->epaisseur_points=2.;
            etincelles->epaisseur_points_luminescents=1.;
            etincelles->vitesse_jet=2.;
            //etincelles->determine_durees_vie(10.);
            float etapes_couleurs_etincelles[12]={   1.,1.,1.,
                                                    0.,1.,1.,
                                                    0.,0.,1.,
                                                    0.,0.,1.
                                                    };
            etincelles->determine_etapes_couleurs(4,etapes_couleurs_etincelles);
            etincelles->debit=100.;
            etincelles->random_amplitudes_perlin(0.1,5.);
            etincelles->random_intervalles_perlin(0.1,1.);


            //--------------- Arrière plan:
            cube_arriere_plan=scene_caduce->cube_arriere_plan_starfield;

            //--------------- Initialisation du vaisseau:
            le_vaisseau=volumes_convertis->renvoie_volume_index(0);

                if(!creation_scene_transition()) return;    ///On crée ici la scène de transition, avant les inits du vaisseau

            if(!initialise_vaisseau())return;
            #define FACTEUR_ECHELLE_TRANSITION_STARFIELD_ARBRE 0.1
            copie_vaisseau->determine_echelle_geometrique(le_vaisseau->echelle_geometrique*FACTEUR_ECHELLE_TRANSITION_STARFIELD_ARBRE);
            conteneur_vaisseau=new dksConteneur("Conteneur vaisseau");



            //----------------Assignation:
            conteneurs[0]=conteneur_vaisseau;
            volumes[0]=le_starfield;
            yeux[0]=oeil_1;
            yeux[1]=oeil_conteneur;
            sources[0]=source_1;
            volumes[1]=le_vaisseau;
            volumes[2]=particules_portes;
            volumes[3]=mon_ver;
            volumes[4]=mon_anneau;
            volumes[5]=etincelles;
            volumes[6]=surface_passage;
            volumes[7]=mon_cube;

            //oeil de débugage:
            oeil_debug=new dksOeil("Oeil_debug",0.,0.,0.);
            yeux[2]=oeil_debug;
            oeil_debug->masse=10.;
            oeil_debug->valeur_propulsion_horizontale=5000.;
            oeil_debug->determine_coefficient_frottement(1.,100.);
            //oeil_courant=oeil_debug;


            //-------------Initialisations positions:
            double axes[9];
            oeil_conteneur->determine_position(0., 0., Z_DEPART_STARFIELD);
            le_vaisseau->determine_position(0.,0.,0.);
            conteneur_vaisseau->determine_position(0.,0.,-2000.);
            conteneur_vaisseau->ajoute_enfant(le_vaisseau);
            mon_ver->determine_position(0.,0.,-2000.+le_vaisseau->boite_collision->zmax);


            //Création de la scene:
            scene=new dksMoteur3d("Starfield",ecran,
                            conteneurs,1,
                            volumes,7,
                            yeux,3,
                            sources,1,
                            oeil_1);



            scene->ajoute_volumes(cadres,NOMBRE_PORTES,particules_portes);
            scene->ajoute_volumes(battants,NOMBRE_PORTES,NULL);

            cadres[INDICE_PORTE_PASSAGE]->ajoute_enfant(surface_passage);
            //surface_passage->determine_position(0.,0.,-1900.);

            scene->determine_cube_arriere_plan(cube_arriere_plan);
            //scene->drapeau_brouillard_surfaces_lumineuses=false;


            for(i=0;i<NOMBRE_PORTES;i++)
            {
                if(battants[i]!=NULL) battants[i]->maj_listes();
                cadres[i]->maj_listes();
            }
            surface_passage->maj_listes();



            //--------------------------------------

            trajectoire_oeil_01=&scene_caduce->trajectoire_oeil_01_starfield;
            if(!trajectoire_vaisseau.charge_trajectoire("trajectoires/vaisseau_starfield.traj"))
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            trajectoire_vaisseau.determine_point(0,conteneur_vaisseau->x,conteneur_vaisseau->y,conteneur_vaisseau->z);

            if(!trajectoire_poursuite_vaisseau.charge_trajectoire("trajectoires/poursuite_vaisseau_starfield.traj"))
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }


            //--------------------------------------

            initialise_animations();

            float position[3];
            trajectoire_poursuite_vaisseau.renvoie_position(1.,position);
            oeil_debug->determine_position(position);

            initialise_listes_affichage();

            #if EDITEUR_ACTIF
            initialise_editeur();
            #endif

        }

        //=====================================================
        //          DESTRUCTEUR
        //=====================================================
        ~Scene_Starfield()
        {
            uint32_t i;
            cout<<"Destruction de la scene Starfield"<<endl;
            if (scene!=NULL) delete scene;
            if (oeil_1!=NULL) delete oeil_1;
            if (oeil_conteneur!=NULL) delete oeil_conteneur;
            if (le_starfield!=NULL) delete le_starfield;
            if(source_1!=NULL) delete source_1;
            if(source_2!=NULL) delete source_2;

            if (sprites!=NULL) delete sprites;
            if(mon_ver!=NULL) delete mon_ver;
            if (mon_sprite!=NULL) delete mon_sprite;
            if (cube_arriere_plan!=NULL) delete cube_arriere_plan;

            if (volumes_convertis!=NULL) delete volumes_convertis;
            if (volumes_lwo2!=NULL) delete volumes_lwo2;
            for (i=0;i<sizeof(cadres)/sizeof(dksVolume*);i++) if (cadres[i]!=NULL)delete cadres[i];
            for (i=0;i<sizeof(battants)/sizeof(dksVolume*);i++) if (battants[i]!=NULL)delete battants[i];


            for (i=0;i<sizeof(portes_lwo)/sizeof(dksChargeLWO2*);i++) if (portes_lwo[i]!=NULL) delete portes_lwo[i];
            for (i=0;i<sizeof(portes_dks)/sizeof(dksConvertionLWO2*);i++) if(portes_dks[i]!=NULL) delete portes_dks[i];

            if (particules_portes!=NULL) delete particules_portes;
            if(mon_anneau!=NULL) delete mon_anneau;
            if(etincelles!=NULL) delete etincelles;

            for(i=0;i<sizeof(images_ver_1)/sizeof(Image*);i++) if(images_ver_1[i]!=NULL) delete images_ver_1[i];
            for(i=0;i<sizeof(images_ver_2)/sizeof(Image*);i++) if(images_ver_2[i]!=NULL) delete images_ver_2[i];
            //for(i=0;i<sizeof(images_ver_3)/sizeof(Image*);i++) if(images_ver_3[i]!=NULL) delete images_ver_3[i];

            if(transition_ver!=NULL)delete transition_ver;
            if(transition_vaisseau!=NULL)delete transition_vaisseau;

            //if(courbe_trajectoire_oeil_01!=NULL)delete courbe_trajectoire_oeil_01; //La trajectoire est créée à la construction...
            //if(trajectoire_vaisseau!=NULL)delete trajectoire_vaisseau;

            if(mon_cube!=NULL)delete mon_cube;

            for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) if (interpolations_parametres[i]!=NULL) delete interpolations_parametres[i];
            for (i=0;i<sizeof(interpolations_positions)/sizeof(Interpolation_Parametre_3d*);i++) if (interpolations_positions[i]!=NULL) delete interpolations_positions[i];

            //--------- Scene de transition:
            if(scene_transition!=NULL) delete scene_transition;
            if(oeil_arbre!=NULL) delete oeil_arbre;
            if(source_1_arbre!=NULL) delete source_1_arbre;
            if(source_2_arbre!=NULL) delete source_2_arbre;
            if(copie_vaisseau!=NULL) delete copie_vaisseau;
            if(cube_arriere_plan_scene_arbre!=NULL) delete cube_arriere_plan_scene_arbre;

            if(oeil_debug!=NULL) delete oeil_debug;

            if  (papillon!=NULL) delete papillon;
            if(conteneur_vaisseau!=NULL) delete conteneur_vaisseau;
        }

        //-----------------------------------------------------------------------
        //      Création de la scène de transition:
        //-----------------------------------------------------------------------
        bool creation_scene_transition()
        {
            cube_arriere_plan_scene_arbre=new dksCube("Cube_arriere_plan_transition_arbre",0.,0.,0.,10.,10.,10.,0xffffffff);

            textures_arriere_plan_arbre[0]=new Image("textures/arriere_plans/arbre_FR.jpg",false,true);
            textures_arriere_plan_arbre[1]=new Image("textures/arriere_plans/arbre_BK.jpg",false,true);
            textures_arriere_plan_arbre[2]=new Image("textures/arriere_plans/arbre_RT.jpg",false,true);
            textures_arriere_plan_arbre[3]=new Image("textures/arriere_plans/arbre_LF.jpg",false,true);
            textures_arriere_plan_arbre[4]=new Image("textures/arriere_plans/arbre_UP.jpg",false,true);
            textures_arriere_plan_arbre[5]=new Image("textures/arriere_plans/arbre_DN.jpg",false,true);


            cube_arriere_plan_scene_arbre->ajoute_textures_arriere_plan(textures_arriere_plan_arbre[0],
                                                                        textures_arriere_plan_arbre[1],
                                                                        textures_arriere_plan_arbre[2],
                                                                        textures_arriere_plan_arbre[3],
                                                                        textures_arriere_plan_arbre[4],
                                                                        textures_arriere_plan_arbre[5]);

            cube_arriere_plan_scene_arbre->inverse_sens_polygones();
            cube_arriere_plan_scene_arbre->determine_affecte_tempon_profondeur(false);

            oeil_arbre=new dksOeil("Oeil_transition_arbre",-20.4601,5.99343,15.5467);
            source_1_arbre=new dksSourceLumineuse("Source_1_transition_arbre",100.,100.,-10.);
            source_2_arbre=new dksSourceLumineuse("Source_2_transition_arbre",-100.,-100.,-10.);
            copie_vaisseau=new dksVolume(*le_vaisseau);
            copie_vaisseau->determine_rendu_faces_transparentes(true);
            copie_vaisseau->determine_mode_rendu(DKSVOLUME_MODE_LISTE);

            double axes[9]={-0.832831,-0.126711,0.538829, -0.0346283,0.983466,0.177749, -0.552443,0.129376,-0.823449};
            copie_vaisseau->determine_axes(axes);
            copie_vaisseau->determine_position(-25.3366,6.38337,8.32157);
            axes={-0.821585,0.00387746,0.570072,0.0223068,0.99943,0.0253507,-0.569649,0.0335442,-0.821203};
            oeil_arbre->determine_axes(axes);
            //Papillon:
            papillon=new Papillon("Papy","LWO2/papillon.lwo",NULL,-1.37773, 8.84183, -6.80747);

            if(papillon==NULL || papillon->erreur!=PAPILLON_OK)
            {
                cout<<"ERREUR Lors de la création du papillon "<<endl;
                erreur=SCENE_ERREUR_INIT;
                return false;
            }

            papillon->copie_repere(copie_vaisseau);

            volumes[0]=copie_vaisseau;
            conteneurs[0]=papillon;
            yeux[0]=oeil_arbre;
            sources[0]=source_1_arbre;
            sources[1]=source_2_arbre;

            scene_transition=new dksMoteur3d("Transition_Arbre",ecran,
                            conteneurs,1,
                            volumes,1,
                            yeux,1,
                            sources,2,
                            oeil_arbre);
            if(scene_transition==NULL || scene_transition->erreur!=DKSM3D_OK)
            {
                cout<<"ERREUR lors de la création de la scène de transition entre le starfield et l'arbre"<<endl;
                return false;
            }

            scene_transition->determine_cube_arriere_plan(cube_arriere_plan_scene_arbre);
            scene_transition->ajoute_volumes((dksVolume**)papillon->volumes,PAPILLON_NBR_VOLUMES,NULL);


            copie_vaisseau->contrainte_trajectoire=true;
            copie_vaisseau->determine_contrainte_verticale(true);
            papillon->contrainte_trajectoire=true;
            papillon->determine_contrainte_verticale(true);

            papillon->determine_drapeau_affiche(false);
            copie_vaisseau->determine_drapeau_affiche(false);

            if(!trajectoire_vaisseau_transition.charge_trajectoire("trajectoires/vaisseau_transition.traj"))
            {
                cout<<"ERREUR lors du chargement de la trajectoire de transition du vaisseau"<<endl;
                return false;
            }

            return true;

        }

        //--------------------------------------------------------------------------
        //      Initialisations du ver:
        //--------------------------------------------------------------------------
        bool initialise_ver()

        {
            mon_ver=new Annelide("mon_ver_starfield",0.,0.,0.,
                             16,6,
                             .25,1.,1.5,
                             20.,
                             /*
                             64,12,
                             .5,2.,1.5,
                             30.,
                             */

                             0xff806000,true);
            mon_ver->determine_double_faces(true);
            mon_ver->intervalle_perturbation_x=1.3;
            mon_ver->intervalle_perturbation_y=.57;
            mon_ver->vitesse_ondulatoire_x=.4 * M_PI*2.;
            mon_ver->vitesse_ondulatoire_y=.0375 * M_PI*2.;
            mon_ver->elasticite=7.5;

            ver_amplitude_x=1.1;
            ver_amplitude_y=1.9333;
            ver_amplitude_perturbation_x=2.5;
            ver_amplitude_perturbation_y=1.57;

            mon_ver->amplitude_ondulatoire_x=ver_amplitude_x;
            mon_ver->amplitude_ondulatoire_y=ver_amplitude_y;
            mon_ver->amplitude_perturbation_x=ver_amplitude_perturbation_x;
            mon_ver->amplitude_perturbation_y=ver_amplitude_perturbation_y;

            images_ver_1[0]=new Image("textures/annelide/tete.tif",true,false);
            images_ver_1[1]=new Image("textures/annelide/queue.tif",true,false);
            images_ver_1[2]=new Image("textures/annelide/peau_dos_01.tif");
            images_ver_1[3]=new Image("textures/annelide/peau_ventre_01.tif");

            images_ver_2[0]=new Image("textures/annelide/tete_2.tif",true,false);
            images_ver_2[1]=new Image("textures/annelide/queue_2.tif",true,false);
            images_ver_2[2]=new Image("textures/annelide/peau_dos_02.tif");
            images_ver_2[3]=new Image("textures/annelide/peau_ventre_02.tif");

            images_ver_3[0]=dksTexture::image_transparente;
            images_ver_3[1]=dksTexture::image_transparente;
            images_ver_3[2]=dksTexture::image_transparente;
            images_ver_3[3]=dksTexture::image_transparente;


            uint32_t i;
            for(i=0;i<4;i++)
            {
                if(
                    images_ver_1[i]==NULL || images_ver_1[i]->erreur!=IMAGE_OK
                    || images_ver_2[i]==NULL || images_ver_2[i]->erreur!=IMAGE_OK
                    )
                {
                    erreur=SCENE_ERREUR_INIT;
                    cout<<"ERREUR lors de l'initialisation du ver - Impossible de charger les textures"<<endl;
                    return false;
                }
            }

            //Texture 0: peau
            mon_ver->ajoute_texture_couleur_sections(images_ver_1[0],ANNELIDE_SURFACE_TETE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_1[1],ANNELIDE_SURFACE_QUEUE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_1[2],ANNELIDE_SURFACE_DOS);
            mon_ver->ajoute_texture_couleur_sections(images_ver_1[3],ANNELIDE_SURFACE_VENTRE);

            //Texture 1: squelette
            mon_ver->ajoute_texture_couleur_sections(images_ver_2[0],ANNELIDE_SURFACE_TETE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_2[1],ANNELIDE_SURFACE_QUEUE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_2[2],ANNELIDE_SURFACE_DOS);
            mon_ver->ajoute_texture_couleur_sections(images_ver_2[3],ANNELIDE_SURFACE_VENTRE);

            //Texture 2:transparence
            mon_ver->ajoute_texture_couleur_sections(images_ver_3[0],ANNELIDE_SURFACE_TETE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_3[1],ANNELIDE_SURFACE_QUEUE);
            mon_ver->ajoute_texture_couleur_sections(images_ver_3[2],ANNELIDE_SURFACE_DOS);
            mon_ver->ajoute_texture_couleur_sections(images_ver_3[3],ANNELIDE_SURFACE_VENTRE);

            if (!mon_ver->initialise_textures())
            {
                erreur=SCENE_ERREUR_INIT;
                cout<<"ERREUR lors de l'initialisation du ver - échec de l'init des textures"<<endl;
                return false;
            }

            //mon_ver->initialise_modes_rendus_surfaces();
            transition_ver=new ES_TransitionTexturesCouleurAxe();
            transition_ver->largeur_transition=0.1;
            transition_ver->initialise_limites(mon_ver);

            for(uint8_t id=0;id<4;id++)
            {
                dksSurface* surface=mon_ver->renvoie_surface_id(id);
                surface->indice_texture_depart=1;
                surface->indice_texture_destination=0;
                surface->determine_effet_special(transition_ver);
                surface->active_effet_special();
                surface->determine_couleur_luminescence(0x8080ff,.1);
            }
            mon_ver->maj_listes();
            return true;
        }

        //------------------------------------------------------
        //  Change les indices pour les surfaces du ver
        //  Ca allège un peu le code
        //------------------------------------------------------
        void initialise_ver_fils_transparent()
        {
            for(uint8_t id=0;id<4;id++)
            {
                dksSurface* surface=mon_ver->renvoie_surface_id(id);
                surface->indice_texture_depart=2;
                surface->indice_texture_destination=1;
            }
        }


        //--------------------------------------------------------------------------
        //      Initialisations du vaisseau:
        //      Les textures transparentes des surfaces sont générées
        //--------------------------------------------------------------------------
        bool initialise_vaisseau()
        {
            //le_vaisseau->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
            double e=(mon_ver->boite_collision->zmax-mon_ver->boite_collision->zmin)/(le_vaisseau->boite_collision->zmax-le_vaisseau->boite_collision->zmin);
            le_vaisseau->determine_echelle_geometrique(e);
            le_vaisseau->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT); ///Semble être plus rapide que le mode TABLEAUX (arrays) ??????

            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]=le_vaisseau->renvoie_surface_nom("carlingue_epaisseur_ailes");
            surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]=le_vaisseau->renvoie_surface_nom("dessous_petites_ailes");
            surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]=le_vaisseau->renvoie_surface_nom("dessus_petites_ailes");
            surfaces_vaisseau[VAISSEAU_bout]=le_vaisseau->renvoie_surface_nom("bout");
            surfaces_vaisseau[VAISSEAU_verriere]=le_vaisseau->renvoie_surface_nom("verriere");
            surfaces_vaisseau[VAISSEAU_carlingue]=le_vaisseau->renvoie_surface_nom("carlingue");
            surfaces_vaisseau[VAISSEAU_carlingue_dessus]=le_vaisseau->renvoie_surface_nom("carlingue_dessus");
            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur]=le_vaisseau->renvoie_surface_nom("carlingue_epaisseur");
            surfaces_vaisseau[VAISSEAU_interieur_cabine]=le_vaisseau->renvoie_surface_nom("interieur_cabine");
            surfaces_vaisseau[VAISSEAU_sol_cabine]=le_vaisseau->renvoie_surface_nom("sol_cabine");
            surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]=le_vaisseau->renvoie_surface_nom("carlingue_dessus_cokpit");
            surfaces_vaisseau[VAISSEAU_epaisseur_aileron]=le_vaisseau->renvoie_surface_nom("epaisseur_aileron");
            surfaces_vaisseau[VAISSEAU_cotes_aileron]=le_vaisseau->renvoie_surface_nom("cotes_aileron");
            surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]=le_vaisseau->renvoie_surface_nom("dessus_grandes_ailes");
            surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]=le_vaisseau->renvoie_surface_nom("dessous_grandes_ailes");
            surfaces_vaisseau[VAISSEAU_reacteurs]=le_vaisseau->renvoie_surface_nom("reacteurs");
            surfaces_vaisseau[VAISSEAU_tuyeres]=le_vaisseau->renvoie_surface_nom("tuyeres");

            dksTexture* texture_disparition;
            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_cotes_aileron]->renvoie_texture_nom("Texture_aileron"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_cotes_aileron]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_bout]->renvoie_texture_nom("Texture_bout"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_bout]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_interieur_cabine]->renvoie_texture_nom("Texture_cabine"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_interieur_cabine]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]->renvoie_texture_nom("Texture_carlingue_cokpit"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]->renvoie_texture_nom("Texture_dessous_ailerons"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]->renvoie_texture_nom("Texture_dessous_ailes"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_carlingue]->renvoie_texture_nom("Texture_dessous_carlingue"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_carlingue]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]->renvoie_texture_nom("Texture_dessus_ailerons"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]->renvoie_texture_nom("Texture_dessus_ailes"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_carlingue_dessus]->renvoie_texture_nom("Texture_dessus_carlingue"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_carlingue_dessus]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_epaisseur_aileron]->renvoie_texture_nom("Texture_epaisseur_aileron"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_epaisseur_aileron]->ajoute_texture(texture_disparition);

            texture_disparition=new dksTexture(*surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]->renvoie_texture_nom("Texture_epaisseur_ailes"));
            texture_disparition->determine_image(dksTexture::image_transparente);
            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]->ajoute_texture(texture_disparition);

            //Les textures sans image:
            dksSurface* surface;
            surface=surfaces_vaisseau[VAISSEAU_carlingue_epaisseur];
            texture_disparition=surface->renvoie_texture_nom("Texture_epaisseur_cokpit");
            texture_disparition->determine_image(dksTexture::image_transparente);
            surface->active_texture(texture_disparition);

            surface=surfaces_vaisseau[VAISSEAU_reacteurs];
            texture_disparition=surface->renvoie_texture_nom("Texture_reacteurs");
            texture_disparition->determine_image(dksTexture::image_transparente);
            surface->active_texture(texture_disparition);

            surface=surfaces_vaisseau[VAISSEAU_sol_cabine];
            texture_disparition=surface->renvoie_texture_nom("Texture_sol");
            texture_disparition->determine_image(dksTexture::image_transparente);
            surface->active_texture(texture_disparition);

            surface=surfaces_vaisseau[VAISSEAU_tuyeres];
            texture_disparition=surface->renvoie_texture_nom("Texture_tuyeres");
            texture_disparition->determine_image(dksTexture::image_transparente);
            surface->active_texture(texture_disparition);

            surface=surfaces_vaisseau[VAISSEAU_verriere];
            texture_disparition=surface->textures_couleur_inactives[1];
            texture_disparition->determine_image(dksTexture::image_transparente);
            surface->active_texture(texture_disparition);


            if (!le_vaisseau->initialise_textures())
            {
                cout<<"ERREUR lors de l'initialisation des textures de transparence du vaisseau !!!"<<endl;
               erreur=SCENE_ERREUR_INIT;
                return false;
            }


            //--------- Effet de transition axiale des textures:
            transition_vaisseau=new ES_TransitionTexturesCouleurAxe();
            transition_vaisseau->largeur_transition=0.1;
            transition_vaisseau->initialise_limites(le_vaisseau);

            surfaces_vaisseau[VAISSEAU_bout]->active_texture(surfaces_vaisseau[VAISSEAU_bout]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_carlingue]->active_texture(surfaces_vaisseau[VAISSEAU_carlingue]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_carlingue_dessus]->active_texture(surfaces_vaisseau[VAISSEAU_carlingue_dessus]->textures_couleur_inactives[0]);

            surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]->active_texture(surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]->textures_couleur_inactives[0]);

            surfaces_vaisseau[VAISSEAU_cotes_aileron]->active_texture(surfaces_vaisseau[VAISSEAU_cotes_aileron]->textures_couleur_inactives[0]);

            surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]->active_texture(surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]->textures_couleur_inactives[0]);

            surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]->active_texture(surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]->active_texture(surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]->active_texture(surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_interieur_cabine]->active_texture(surfaces_vaisseau[VAISSEAU_interieur_cabine]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]->active_texture(surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_epaisseur_aileron]->active_texture(surfaces_vaisseau[VAISSEAU_epaisseur_aileron]->textures_couleur_inactives[0]);
            surfaces_vaisseau[VAISSEAU_verriere]->active_texture(surfaces_vaisseau[VAISSEAU_verriere]->textures_couleur_inactives[0]);

            initialise_vaisseau_transparent_fils();

            for(uint16_t i=0;i<sizeof(surfaces_vaisseau)/sizeof(dksSurface*);i++)
            {
                surfaces_vaisseau[i]->determine_effet_special(transition_vaisseau);
                surfaces_vaisseau[i]->active_effet_special();
                //surfaces_vaisseau[i]->determine_luminescence(0.1);
            }

            surfaces_vaisseau[VAISSEAU_tuyeres]->luminescence=0.;

            le_vaisseau->determine_liste_luminescences(false);
            return true;
        }

        //------------------------------------------------------
        //  Change les indices pour les surfaces du vaisseau
        //  Ca allège un peu le code
        //------------------------------------------------------

        void initialise_vaisseau_transparent_fils()
        {
            //0:texture carlingue - 1:texture transparente - 2:fils de fer
            initialise_effet_transition_textures_vaisseau(VAISSEAU_bout,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_dessus,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_dessus_cokpit,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_cotes_aileron,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessous_grandes_ailes,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessous_petites_ailes,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessus_grandes_ailes,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessus_petites_ailes,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_epaisseur_ailes,2,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_epaisseur_aileron,2,1);

            //0:texture transparente - 1:fils de fer
            initialise_effet_transition_textures_vaisseau(VAISSEAU_verriere,1,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_interieur_cabine,1,0);

            //0:transparente
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_epaisseur,0,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_reacteurs,0,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_sol_cabine,0,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_tuyeres,0,0);
        }

        void initialise_vaisseau_fils_carlingue()
        {
            //0:texture carlingue - 1:texture transparente - 2:fils de fer
            initialise_effet_transition_textures_vaisseau(VAISSEAU_bout,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_dessus,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_dessus_cokpit,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_cotes_aileron,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessous_grandes_ailes,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessous_petites_ailes,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessus_grandes_ailes,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_dessus_petites_ailes,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_epaisseur_ailes,0,2);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_epaisseur_aileron,0,2);

            //0:texture transparente - 1:fils de fer
            initialise_effet_transition_textures_vaisseau(VAISSEAU_verriere,-1,1);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_interieur_cabine,-1,1);

            //0:transparente
            initialise_effet_transition_textures_vaisseau(VAISSEAU_carlingue_epaisseur,-1,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_reacteurs,-1,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_sol_cabine,-1,0);
            initialise_effet_transition_textures_vaisseau(VAISSEAU_tuyeres,-1,0);
        }

        void initialise_effet_transition_textures_vaisseau(uint16_t id,int32_t depart,int32_t destination)
        {
            dksSurface* surface=surfaces_vaisseau[id];
            surface->indice_texture_depart=depart;
            surface->indice_texture_destination=destination;

        }

        void initialise_vaisseau_rendu_vol()
        {
            surfaces_vaisseau[VAISSEAU_bout]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_carlingue]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_carlingue_dessus]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur_ailes]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_carlingue_dessus_cokpit]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_cotes_aileron]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_dessous_grandes_ailes]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_dessous_petites_ailes]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_dessus_grandes_ailes]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_dessus_petites_ailes]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;
            surfaces_vaisseau[VAISSEAU_epaisseur_aileron]->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR;

            surfaces_vaisseau[VAISSEAU_interieur_cabine]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
            surfaces_vaisseau[VAISSEAU_carlingue_epaisseur]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
            surfaces_vaisseau[VAISSEAU_verriere]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
            surfaces_vaisseau[VAISSEAU_sol_cabine]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
            surfaces_vaisseau[VAISSEAU_reacteurs]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
            surfaces_vaisseau[VAISSEAU_tuyeres]->mode_rendu==DKSSURFACE_RENDU_COULEUR;
        }



        //==========================================================
        //                      Fonctions
        //==========================================================
        virtual void active()
        {
            this->Scene::active();
            scene->active();
            scene->active_brouillard();
        }

        virtual void desactive()
        {
            scene->desactive();
        }

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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //------------------------------------- Affiche la scène principale:
            if(!etapes[TRANSITION])
            {
                //-------------- Scene starfield:
                    scene->active();

                    if(particules_portes->drapeau_affiche) particules_portes->maj_volumes();

                    //-----------Avant la mise à jour de la scène:
                    if (mon_anneau->drapeau_affiche)anneau_longe_repere_axeZ(mon_ver,-20.,0.,4.,4.,pos_anneau);

                    //Mise à jour de la scène (calculs des repères absolus pour ce rendu:
                    scene->maj(t);
                    oeil_conteneur->maj_visee();

                    //-----------Après la mise à jour de la scène:
                    if (mon_anneau->drapeau_affiche)
                    {
                        mon_anneau->genere_eclairs();
                        genere_etincelles_anneau();
                    }

                    if(etincelles->drapeau_affiche) etincelles->maj_jet_discontinu();
                    if(le_starfield->drapeau_affiche) le_starfield->maj_champ_etoiles_base(oeil_1->repere_absolu->x,oeil_1->repere_absolu->y,oeil_1->repere_absolu->z);


                    //------- Affichage scène starfield:
                    if(surface_passage->drapeau_affiche)
                    {
                        //post_rendu->affiche_flou_256(oeil_courant,DKSECRAN_TEMPON_ECRAN_2,DKSECRAN_TEMPON_ECRAN_1);
                        scene->drapeau_affiche=true;
                        surface_passage->drapeau_affiche=false;

                        post_rendu->affiche_tempon_256(oeil_courant);
                        post_rendu->floutage_tempon_256();
                        post_rendu->affiche_tempon_ecran_1(oeil_courant);
                        post_rendu->depart_superposition_tempons(DKSECRAN_TEMPON_ECRAN_1,false);
                        post_rendu->superpose_tempon(DKSECRAN_TEMPON_256);
                        post_rendu->fin_superposition_tempons();

                        scene->drapeau_affiche=false;
                        surface_passage->drapeau_affiche=true;
                        glMatrixMode(GL_MODELVIEW);
                        glPushMatrix();
                        glMultMatrixd(surface_passage->matrice.matrice);
                        post_rendu->affiche_tempon_ecran_1(oeil_courant,false);
                        glPopMatrix();
                    }
                    else
                    {
                        /*
                        post_rendu->affiche_tempon_256(oeil_courant);
                        post_rendu->floutage_tempon_256();
                        post_rendu->affiche_tempon_ecran_1(oeil_courant);
                        post_rendu->depart_superposition_tempons(DKSECRAN_TEMPON_ECRAN_1,false);
                        post_rendu->superpose_tempon(DKSECRAN_TEMPON_256);
                        post_rendu->fin_superposition_tempons();
                        */
                        post_rendu->affiche_flou_256(oeil_courant);
                    }

                        /*
                        post_rendu->depart_superposition_tempons(DKSECRAN_TEMPON_DEFAUT,true);
                        post_rendu->superpose_tempon(DKSECRAN_TEMPON_ECRAN_1);
                        post_rendu->fin_superposition_tempons();
                        */

                        //affiche_trajectoire();
                    scene->desactive();


                    //-------------- Scène de transition:
                    if(surface_passage->drapeau_affiche)
                    {
                        scene_transition->active();

                        oeil_arbre->copie_repere(oeil_1->repere_absolu);
                        oeil_arbre->produit_reel(FACTEUR_ECHELLE_TRANSITION_STARFIELD_ARBRE);
                        if(!interpolations_parametres[TRAJECTOIRE_VAISSEAU]->drapeau_terminee)
                        {
                            copie_vaisseau->copie_repere(conteneur_vaisseau);
                            copie_vaisseau->produit_reel(FACTEUR_ECHELLE_TRANSITION_STARFIELD_ARBRE);
                        }

                        scene_transition->maj(t);
                        post_rendu->affiche_tempon_ecran_2(oeil_arbre);
                        scene_transition->desactive();

                        //-------------- Masquage des deux scènes:
                        post_rendu->masque_2_tempons(TX_ECRAN_1,TX_ECRAN_2);
                    }
            }
            //------------------------------ Affiche uniquement la scène de transition:
            else
            {
                scene_transition->active();
                scene_transition->maj(t);
                post_rendu->affiche_couleur_tempon_ecran_2(oeil_arbre);
                scene_transition->desactive();
                affiche_trajectoire();
            }



            //affiche_trajectoire_vaisseau();
            //affiche_trajectoire_poursuite();
            //affiche_conteneur_selectionne();
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
                vitesse_rotation_oeil_transformation=-4;

                interpolations_parametres[OUVERTURE_PORTE]=new Interpolation_Parametre(-1.,0.,-80./180.*M_PI,2.,INTERPOLATION_COSINUS);
                interpolations_parametres[POSITION_ANNEAU]=new Interpolation_Parametre(-1.,1.,0.,3.,INTERPOLATION_COSINUS);
                interpolations_parametres[FACTEUR_ONDULATION_VER]=new Interpolation_Parametre(-1.,1.,0.,1.,INTERPOLATION_LINEAIRE);
                interpolations_parametres[FACTEUR_LUMINESCENCE_VER]=new Interpolation_Parametre(-1.,1.,0.,4.,INTERPOLATION_LINEAIRE);
                interpolations_parametres[FACTEUR_LUMINESCENCE_VAISSEAU]=new Interpolation_Parametre(-1.,0.,1.,4.,INTERPOLATION_LINEAIRE);
                interpolations_parametres[FACTEUR_VITESSE_ROTATION_OEIL_1]=new Interpolation_Parametre(-1.,1.,0.,2.,INTERPOLATION_LINEAIRE);
                interpolations_parametres[FACTEUR_ROTATION_OEIL]=new Interpolation_Parametre(-1.,0.,1.,2.,INTERPOLATION_LINEAIRE);
                interpolations_parametres[LUMINOSITE_REACTEUR]=new Interpolation_Parametre(-1.,0.,1.,1.,INTERPOLATION_COSINUS);

                Courbe_Bezier_Plan* courbe_trajectoire_vaisseau=new Courbe_Bezier_Plan(0.,0.,0.1,0.,0.5,0.5,1.,1.);
                interpolations_parametres[TRAJECTOIRE_VAISSEAU]=new Interpolation_Parametre(-1.,0.,1.,60.,INTERPOLATION_BEZIER,courbe_trajectoire_vaisseau);
                Courbe_Bezier_Plan* courbe_trajectoire_poursuite_vaisseau=new Courbe_Bezier_Plan(0.,0.,0.155,0.,0.5,0.5,1.,1.);
                interpolations_parametres[TRAJECTOIRE_POURSUITE_VAISSEAU]=new Interpolation_Parametre(-1.,0.,1.,62.,INTERPOLATION_BEZIER,courbe_trajectoire_poursuite_vaisseau);

                interpolations_parametres[TRAJECTOIRE_OEIL_1]=scene_caduce->position_trajectoire_oeil_1_starfield;
                    //interpolations_parametres[TRAJECTOIRE_OEIL_1]->duree=2.; ///Retirer après développement

                float position[3];
                double position_0[3],position_1[3];
                trajectoire_poursuite_vaisseau.renvoie_position(0.,position);
                position_0={0.,0.,0.};  ///A initialiser au vol
                position_1={double(position[0]),double(position[1]),double(position[2])};
                interpolations_positions[POSITIONNE_OEIL] =new Interpolation_Parametre_3d
                                                            (-1.,position_0,position_1,
                                                             interpolations_parametres[LUMINOSITE_REACTEUR]->duree,
                                                             INTERPOLATION_COSINUS);
                determine_affiche_portes(false);
                determine_affiche_the_porte(false);
                surface_passage->drapeau_affiche=false;
                oeil_arbre->determine_ouverture(oeil_1->renvoie_ouverture());
                cube_arriere_plan_scene_arbre->rotation(-90.,1.,0.,0.);

                interpolations_parametres[TRAJECTOIRE_VAISSEAU_TRANSITION]=new Interpolation_Parametre(-1.,0.,1.,10.,INTERPOLATION_LINEAIRE);
        }

        void initialise_listes_affichage()
        {
            element_rendu_scene=post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
            element_rendu_surface_passage=post_rendu->ajoute_rendu(surface_passage,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
            post_rendu->ajoute_rendu(scene_transition,DKSECRAN_TEMPON_ECRAN_2,DKSRENDU_COULEURS);
            //post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_QUART,DKSRENDU_COULEURS_LUMINESCENTES);
            post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_256,DKSRENDU_COULEURS_LUMINESCENTES);
            //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_QUART,DKSRENDU3D_COULEURS);
        }

        public:
        //----------------------------------------------------------------------------
        //                  Etat de la scène au départ de l'animation
        //----------------------------------------------------------------------------
        virtual void initialise_t0(double t0)
        {
            double axes[9];
            this->Scene::initialise_t0(t0);
            dksOeil* oeil_caducee=scene_caduce->oeil_courant;
            ouverture_camera=oeil_caducee->renvoie_ouverture();
            oeil_1->determine_ouverture(ouverture_camera);
            oeil_conteneur->determine_conteneur_vise(conteneur_vaisseau);
            oeil_conteneur->determine_mode_visee(DKSOEIL_VISEE_CONTENEUR);
            oeil_conteneur->determine_contrainte_verticale(true);
            oeil_conteneur->copie_repere(scene_caduce->oeil_1_starfield);
            oeil_conteneur->distance_poursuite_min=30.;
            oeil_conteneur->inertie_poursuite=10.;

            conteneur_vaisseau->contrainte_trajectoire=true;
            conteneur_vaisseau->determine_contrainte_verticale(true);
            /*
            oeil_vaisseau->distance_poursuite_min=20.;
            oeil_vaisseau->inertie_poursuite=10.;
            oeil_vaisseau->determine_conteneur_vise(conteneur_poursuite);
            oeil_vaisseau->determine_mode_visee(DKSOEIL_VISEE_CONTENEUR);
            oeil_vaisseau->determine_contrainte_verticale(true);
            oeil_vaisseau->active_visee_inertielle();
            oeil_vaisseau->determine_inertie_visee(5.);
            oeil_vaisseau->maj_visee();
            */


            //post_rendu->alpha_tempon_quart=1.;
            post_rendu->alpha_tempon_256=1.;
            post_rendu->alpha_tempon_ecran=1.;
            post_rendu->largeur_flou=20.;
            post_rendu->hauteur_flou=20.;
            post_rendu->saturation_horizontale_flou=2.;
            post_rendu->saturation_verticale_flou=2.;

            scene->determine_limites_brouillard(50.,800.);
            scene->determine_couleur_brouillard(0xff0c0a18);
            mon_sprite->determine_affecte_par_brouillard(false);

            le_starfield->determine_points(scene_caduce->le_starfield->renvoie_points(),le_starfield->renvoie_nombre_de_points());

            //--------DEBUGAGE:
            le_starfield->reset();

            //-------- HORS DEBUGAGE:
            /*
            double cp[3];
            le_starfield->desactive_reset();
            scene_caduce->le_starfield->renvoie_centre_precedent(cp);
            le_starfield->determine_centre_precedent(cp);
            */

            //-----------------------

            //scene->t0=t0;

            mon_anneau->drapeau_affiche=false;
            pos_anneau=1.;
            transition_ver->niveau=pos_anneau;
            transition_vaisseau->niveau=pos_anneau;
            le_vaisseau->drapeau_affiche=false;

                //particules_portes->maj_volumes();///Le temps de l'édition !!!!!!!!!!!!!!!!!!

            //------ A supprimer après développement:
            /*
            the_battant->determine_RTL(0.,0.,-75./180.*M_PI);
            determine_affiche_portes(true);
            determine_affiche_the_porte(true);
            surface_passage->drapeau_affiche=false;
            */
        }


        //========================================================
        //      Gestion des portes:
        //========================================================
        void determine_affiche_portes(bool d)
        {
            uint32_t i;
            particules_portes->drapeau_affiche=d;
            for(i=0;i<NOMBRE_PORTES;i++)
            {
                if(i!=INDICE_PORTE_PASSAGE)
                {
                   cadres[i]->drapeau_affiche=d;
                    battants[i]->drapeau_affiche=d;
                }

            }
        }

        void determine_affiche_the_porte(bool d)
        {
            the_battant->drapeau_affiche=d;
            the_cadre->drapeau_affiche=d;
        }

        //========================================================
        //          Interactions entre les objets
        //========================================================

        //---------- Génère les étincelles dues à l'anneau électrique:
        /// Appeler après la mise à jour de la scène, pour pouvoir travailler avec les repères absolus
        void genere_etincelles_anneau()
        {
            uint32_t i,nbr;
            double point[3];
            double* points_anneau=mon_anneau->renvoie_points();
            nbr=5;//mon_anneau->renvoie_nombre_de_points()/10;
            for(i=0;i<nbr;i++)
            {
                dksRepere::translation_axe(&points_anneau[3*i],mon_anneau->repere_absolu,point);
                etincelles->determine_direction_jet(float(point[0]),float(point[1]),float(point[2]));
                dksRepere::translation_point(&points_anneau[3*i],mon_anneau->repere_absolu,point);
                etincelles->position_jet={float(point[0]),float(point[1]),float(point[2])};
                etincelles->ejection_particules(5);
            }
        }

        //-------- L'anneau électrique longe l'axe Z d'un repère:
        //  pos varie entre 0 et 1 et représente la position entre zmin et zmax
        void anneau_longe_repere_axeZ(dksRepere* repere, double zmin,double zmax, float rayon_max,double saturation_rayon, double pos)
        {
            if (pos<0. || pos>1.) return;
            mon_anneau->rayon_central=rayon_max*(sin(pos*M_PI)*saturation_rayon);
            if (mon_anneau->rayon_central>rayon_max)mon_anneau->rayon_central=rayon_max;
            mon_anneau->copie_repere(repere);
            double position_anneau=(zmin+(zmax-zmin)*pos);
            mon_anneau->x+=mon_anneau->axe_z->x*position_anneau;
            mon_anneau->y+=mon_anneau->axe_z->y*position_anneau;
            mon_anneau->z+=mon_anneau->axe_z->z*position_anneau;
            mon_anneau->a_ete_deplace();

        }

        //======================================================
        //               Attente du retour du faisceau
        //              Gestion des animations
        //=======================================================

        virtual void attente_retour_faisceau()
        {

            uint32_t i;
            float position[3];
            double position_d[3];
            float position_parcours;

            if (!drapeau_initialisee || drapeau_scene_terminee) return;

            #if JOUE_MUSIQUE
            la_musique->renvoie_FFT();
                mon_anneau->facteur_rayon_section=5.*la_musique->tempon_FFT[1];
                mon_anneau->facteur_couleur=8.*la_musique->tempon_FFT[1];
            #endif


            float decal_temps=-2; //-13 lors du développement


            if (position_temps_scene<15.+decal_temps)   //15
            {
                if (!etapes[REJOINT_VER])
                {
                    etapes[REJOINT_VER]=true;

                }
                position_parcours=interpolations_parametres[TRAJECTOIRE_OEIL_1]->renvoie_valeur_parametre(position_temps);
                trajectoire_oeil_01->renvoie_position(position_parcours,position);
                oeil_conteneur->determine_position(position);
            }


            else if (position_temps_scene<19.+decal_temps)  //19
            {
                if (!etapes[TRANSFORMATION_VER_FILS])
                {
                    etapes[TRANSFORMATION_VER_FILS]=true;
                    mon_anneau->drapeau_affiche=true;

                }
                oeil_conteneur->deplace(DROITE,vitesse_rotation_oeil_transformation*delta_t);
                pos_anneau=interpolations_parametres[POSITION_ANNEAU]->renvoie_valeur_parametre(position_temps);
                if(interpolations_parametres[POSITION_ANNEAU]->drapeau_terminee) mon_anneau->drapeau_affiche=false;
                transition_ver->niveau=pos_anneau;
                float facteur_ondulation=interpolations_parametres[FACTEUR_ONDULATION_VER]->renvoie_valeur_parametre(position_temps);
                mon_ver->amplitude_ondulatoire_x=ver_amplitude_x*facteur_ondulation;
                mon_ver->amplitude_ondulatoire_y=ver_amplitude_y*facteur_ondulation;
                mon_ver->amplitude_perturbation_x=ver_amplitude_perturbation_x*facteur_ondulation;
                mon_ver->amplitude_perturbation_y=ver_amplitude_perturbation_y*facteur_ondulation;
                mon_ver->facteur_luminescence=interpolations_parametres[FACTEUR_LUMINESCENCE_VER]->renvoie_valeur_parametre(position_temps);
            }


            else if (position_temps_scene<23.+decal_temps)  //23
            {
                if (!etapes[TRANSFORMATION_VER_VAISSEAU])
                {
                    interpolations_parametres[POSITION_ANNEAU]->reset();
                    etapes[TRANSFORMATION_VER_VAISSEAU]=true;
                    mon_anneau->drapeau_affiche=true;
                    le_vaisseau->drapeau_affiche=true;
                    le_vaisseau->determine_liste_luminescences(false);
                    le_vaisseau->facteur_luminescence=0.;
                    initialise_ver_fils_transparent();

                }
                oeil_conteneur->deplace(DROITE,vitesse_rotation_oeil_transformation*delta_t);
                pos_anneau=interpolations_parametres[POSITION_ANNEAU]->renvoie_valeur_parametre(position_temps);
                if(interpolations_parametres[POSITION_ANNEAU]->drapeau_terminee) mon_anneau->drapeau_affiche=false;
                transition_ver->niveau=pos_anneau;
                transition_vaisseau->niveau=pos_anneau;
            }


            else if (position_temps_scene<27.+decal_temps)  //27
            {
                if (!etapes[TRANSFORMATION_VAISSEAU_CARLINGUE])
                {
                    interpolations_parametres[POSITION_ANNEAU]->reset();
                    etapes[TRANSFORMATION_VAISSEAU_CARLINGUE]=true;
                    mon_anneau->drapeau_affiche=true;
                    mon_ver->drapeau_affiche=false;
                    initialise_vaisseau_fils_carlingue();
                    //le_vaisseau->determine_liste_luminescences(false);

                }
                oeil_conteneur->deplace(DROITE,vitesse_rotation_oeil_transformation*delta_t);
                pos_anneau=interpolations_parametres[POSITION_ANNEAU]->renvoie_valeur_parametre(position_temps);
                if(interpolations_parametres[POSITION_ANNEAU]->drapeau_terminee) mon_anneau->drapeau_affiche=false;
                //transition_ver->niveau=pos_anneau;
                transition_vaisseau->niveau=pos_anneau;
                le_vaisseau->facteur_luminescence=interpolations_parametres[FACTEUR_LUMINESCENCE_VAISSEAU]->renvoie_valeur_parametre(position_temps);

            }

            else if (position_temps_scene<29.+decal_temps)  //27
            {
                if (!etapes[ARRET_OEIL])
                {
                    etapes[ARRET_OEIL]=true;
                    initialise_vaisseau_rendu_vol();    //Désactiver le shader de transition, en activer d'autres...
                    //le_vaisseau->determine_liste_luminescences(true);
                }

                /// A MODIFIER !!! => Interpolation de la position actuelle de l'oeil à la position de départ de sa trajectoire
                float facteur=interpolations_parametres[FACTEUR_VITESSE_ROTATION_OEIL_1]->renvoie_valeur_parametre(position_temps);
                oeil_conteneur->deplace(DROITE,facteur*vitesse_rotation_oeil_transformation*delta_t);

            }

            else if (position_temps_scene<30.+decal_temps)  //27
            {
                if (!etapes[DEMARRAGE_VAISSEAU])
                {
                    etapes[DEMARRAGE_VAISSEAU]=true;
                    interpolations_positions[POSITIONNE_OEIL]->valeur_depart={oeil_conteneur->x,oeil_conteneur->y,oeil_conteneur->z};
                }
                //************Ajouter un tremblement de l'oeil
                interpolations_positions[POSITIONNE_OEIL]->renvoie_valeur_parametre(position_temps,position_d);
                oeil_conteneur->determine_position(position_d);
                surfaces_vaisseau[VAISSEAU_tuyeres]->luminescence=interpolations_parametres[LUMINOSITE_REACTEUR]->renvoie_valeur_parametre(position_temps);
            }



            else if (position_temps_scene<30+62+decal_temps)
            {
                if (!etapes[PARCOURS_VAISSEAU])
                {
                    etapes[PARCOURS_VAISSEAU]=true;
                    determine_affiche_portes(true);
                    //determine_affiche_the_porte(false);
                    le_vaisseau->determine_liste_luminescences(true);
                    oeil_conteneur->active_visee_inertielle();
                    oeil_conteneur->determine_inertie_visee(10.);
                }



                if(interpolations_parametres[TRAJECTOIRE_VAISSEAU]->drapeau_terminee)
                {
                    position_parcours=interpolations_parametres[TRAJECTOIRE_VAISSEAU_TRANSITION]->renvoie_valeur_parametre(position_temps);
                    trajectoire_vaisseau_transition.renvoie_position(position_parcours,position);
                    copie_vaisseau->determine_position(position);
                }
                else
                {
                    position_parcours=interpolations_parametres[TRAJECTOIRE_VAISSEAU]->renvoie_valeur_parametre(position_temps);
                    trajectoire_vaisseau.renvoie_position(position_parcours,position);
                    conteneur_vaisseau->determine_position(position);
                }


                //---------Trajectoire de poursuite du vaisseau:

                position_parcours=interpolations_parametres[TRAJECTOIRE_POURSUITE_VAISSEAU]->renvoie_valeur_parametre(position_temps);
                trajectoire_poursuite_vaisseau.renvoie_position(position_parcours,position);
                oeil_conteneur->determine_position(position);


                //Rotation de l'oeil sur l'axe Z

                if(position_temps_scene>65.+decal_temps)
                {
                    float f=interpolations_parametres[FACTEUR_ROTATION_OEIL]->renvoie_valeur_parametre(position_temps);
                    oeil_1->rotation(f*15./180.*M_PI*delta_t,0.,0.,1.);
                    //le_vaisseau->rotation(f*15./180.*M_PI*delta_t,0.,0.,1.);
                }

                //Affichage de la porte de passage:
                if(!surface_passage->drapeau_affiche && position_temps_scene>84.+decal_temps)
                {
                   surface_passage->drapeau_affiche=true;
                   determine_affiche_the_porte(true);
                }

                //Ouverture de la porte:
                if (position_temps_scene>86.+decal_temps)
                {

                    if(!etapes[DEPART_OUVERTURE_PORTE])
                    {
                        etapes[DEPART_OUVERTURE_PORTE]=true;
                        interpolations_parametres[FACTEUR_ROTATION_OEIL]->determine_parametres(-1.,1.,0.,4.,INTERPOLATION_LINEAIRE);
                    }

                    float angle=interpolations_parametres[OUVERTURE_PORTE]->renvoie_valeur_parametre(position_temps_scene);
                    the_battant->determine_RTL(0.,0.,double(angle));
                    if(interpolations_parametres[OUVERTURE_PORTE]->drapeau_terminee)
                    {
                        le_vaisseau->determine_drapeau_affiche(false);
                        copie_vaisseau->determine_drapeau_affiche(true);
                    }
                }

            }


            else if(position_temps_scene<5+92+decal_temps)  //+92+decal_temps
            {
                if(!etapes[TRANSITION])
                {
                    etapes[TRANSITION]=true;
                    papillon->copie_repere(copie_vaisseau);
                    //papillon->toString();
                    //copie_vaisseau->toString();
                    //oeil_arbre->toString();
                }

                position_parcours=interpolations_parametres[TRAJECTOIRE_VAISSEAU_TRANSITION]->renvoie_valeur_parametre(position_temps);
                trajectoire_vaisseau_transition.renvoie_position(position_parcours,position);
                //copie_vaisseau->determine_position(position);

            }


            else if(position_temps_scene<10+92+decal_temps) //+92+decal_temps
            {
                if(!etapes[TRANSITION_FLASH])
                {
                    etapes[TRANSITION_FLASH]=true;
                    papillon->determine_drapeau_affiche(true);
                    papillon->demarre_vol();
                    copie_vaisseau->determine_drapeau_affiche(false);
                }

                position_parcours=interpolations_parametres[TRAJECTOIRE_VAISSEAU_TRANSITION]->renvoie_valeur_parametre(position_temps);
                trajectoire_vaisseau_transition.renvoie_position(position_parcours,position);
                //papillon->determine_position(position);


            }



            /*
            else
            {
                drapeau_scene_terminee=true;

            }
            */









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
            vitesse_deplacement=1.;
            vitesse_rotation=0.1;
            position_point=0.1;

            oeil_1->valeur_propulsion_horizontale=1000.;
            oeil_1->masse=10.;
            oeil_1->determine_coefficient_frottement(1.,100.);

            particules_portes->masse=10.;
            particules_portes->determine_coefficient_frottement(1.,100.);

            etincelles->masse=10.;
            etincelles->determine_coefficient_frottement(1.,100.);

            mon_anneau->masse=10.;
            mon_anneau->determine_coefficient_frottement(1.,100.);

            mon_ver->valeur_propulsion_horizontale=10000.;
            mon_ver->masse=10.;
            mon_ver->determine_coefficient_frottement(1.,100.);

            //conteneur_poursuite->valeur_propulsion_horizontale=10000.;
            //conteneur_poursuite->masse=10.;
            //conteneur_poursuite->determine_coefficient_frottement(1.,100.);

            copie_vaisseau->valeur_propulsion_horizontale=1000.;
            copie_vaisseau->masse=10.;
            copie_vaisseau->determine_coefficient_frottement(1.,100.);

            oeil_arbre->valeur_propulsion_horizontale=1000.;
            oeil_arbre->masse=10.;
            oeil_arbre->determine_coefficient_frottement(1.,100.);


            position_trajectoire=0.;
            pas_position_trajectoire=0.0005;
            //fichier_trajectoire={"trajectoires/poursuite_vaisseau_starfield.traj"};
            fichier_trajectoire={"trajectoires/vaisseau_transition.traj"};

            conteneurs_selectionnables.push_back(oeil_arbre);
            conteneurs_selectionnables.push_back(copie_vaisseau);
            conteneurs_selectionnables.push_back(papillon);
            conteneurs_selectionnables.push_back(oeil_1);
            //conteneurs_selectionnables.push_back(mon_anneau);
            //conteneurs_selectionnables.push_back(mon_ver);

            // Pour pouvoir éditer les portes:
            /*
            for(uint16_t i=0;i<NOMBRE_PORTES;i++)
            {
                conteneurs_selectionnables.push_back(cadres[i]);
                cadres[i]->masse=10.;
                cadres[i]->determine_coefficient_frottement(1.,100.);
            }
            */

            conteneur_selectionne=oeil_1;
            conteneur_mem=NULL;
        }

        //===================================================
        //          Affiche les éléments édités
        //===================================================
        void affiche_trajectoire()
        {
            //glLoadIdentity();
            glDisable(GL_FOG);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            ma_trajectoire.affiche_courbe(20,0x40ffff00);
            ma_trajectoire.affiche_points(0x80ff0000,5);
            ma_trajectoire.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        void affiche_trajectoire_vaisseau()
        {
            //glLoadIdentity();
            glDisable(GL_FOG);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            trajectoire_vaisseau.affiche_courbe(20,0x4000ffff);
            trajectoire_vaisseau.affiche_points(0x80ff00ff,5);
            //trajectoire_vaisseau.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            //glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        void affiche_trajectoire_poursuite()
        {
            //glLoadIdentity();
            glDisable(GL_FOG);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            trajectoire_poursuite_vaisseau.affiche_courbe(20,0x4000ffff);
            trajectoire_poursuite_vaisseau.affiche_points(0x80ff00ff,5);
            //trajectoire_vaisseau.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            //glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        void affiche_conteneur_selectionne()
        {
            //glLoadIdentity();
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_DEPTH_TEST);
            glDisable(GL_FOG);

            glColor4ub(0xff,0xff,0x00,0x80);
            glPointSize(50);
            glBegin(GL_POINTS);
                 glVertex3f(conteneur_selectionne->repere_absolu->x,conteneur_selectionne->repere_absolu->y,conteneur_selectionne->repere_absolu->z);
            glEnd();

            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
            //glEnable(GL_DEPTH_TEST);
        }

        void affiche_numero_porte()
        {
            for(uint32_t i=0;i<NOMBRE_PORTES;i++)
            {
                if(conteneur_selectionne==cadres[i])
                {
                    cout<<"indice porte: "<<i<<endl;
                    break;
                }
            }
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
            uint32_t nbr;
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
                case 1: //F1 : Sauve la trajectoire
                    if (ma_trajectoire.sauve_trajectoire(fichier_trajectoire)) cout<<"Trajectoire "<<fichier_trajectoire<<" sauvée avec succès!"<<endl;
                    break;

                case 2: //F2 : Charge la trajectoire
                    if (ma_trajectoire.charge_trajectoire(fichier_trajectoire))
                    {
                        cout<<"Trajectoire "<<fichier_trajectoire<<" chargée avec succès!"<<endl;
                        point_actuel=0;
                    }

                case 3:
                    /*
                    if(particules_portes->sauve_points("particules/portes.crd"))
                    {
                       cout<<"Coordonnees "<<"particules/portes.crd"<<" sauvées avec succès!"<<endl;
                    }
                    */
                    break;

                // flèche retour
                case 106:
                    for (uint32_t i=0;i<conteneurs_selectionnables.size();i++)
                    {
                        if (conteneurs_selectionnables[i]==conteneur_selectionne)
                        {
                            if (i==conteneurs_selectionnables.size()-1) conteneur_selectionne=conteneurs_selectionnables[0];
                            else conteneur_selectionne=conteneurs_selectionnables[i+1];
                            cout<<"Conteneur sélectionné: "<<conteneur_selectionne->nom<<endl;
                            if(strcmp(conteneur_selectionne->nom,"cadre")==0) affiche_numero_porte();
                            break;
                        }
                    }
                    break;
                // touche "fin"
                case 107:
                    nbr=conteneurs_selectionnables.size();
                    for (uint32_t i=0;i<nbr;i++)
                    {
                        if (conteneurs_selectionnables[i]==conteneur_selectionne)
                        {
                            if (i==0) conteneur_selectionne=conteneurs_selectionnables[nbr-1];
                            else conteneur_selectionne=conteneurs_selectionnables[i-1];
                            cout<<"Conteneur sélectionné: "<<conteneur_selectionne->nom<<endl;
                            if(strcmp(conteneur_selectionne->nom,"cadre")==0) affiche_numero_porte();
                            break;
                        }
                    }
                    break;

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

                case 'o':
                    if(conteneur_mem==NULL)
                    {
                        conteneur_mem=conteneur_selectionne;
                        conteneur_selectionne=oeil_courant;
                    }
                    else
                    {
                       conteneur_selectionne=conteneur_mem;
                       conteneur_mem=NULL;
                    }
                    cout<<"Conteneur sélectionné: "<<conteneur_selectionne->nom<<endl;
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
                case 13: //ENTREE - Ajoute un point à l'emplacement du conteneur sélectionné
                    ma_trajectoire.ajoute_point(conteneur_selectionne->x,conteneur_selectionne->y,conteneur_selectionne->z);
                    point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    break;
                case 8: //SUPPR - Supprime le point sélectionné
                    ma_trajectoire.supprime_point(point_actuel);
                    if (point_actuel>=ma_trajectoire.renvoie_nombre_points())point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    break;
                    // Déplacement du point sélectionné:
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
                    //Déplacement du conteneur sélectionné le long de la trajectoire:
                case '&':
                    position_trajectoire-=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    trajectoire_vaisseau.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
                    break;
                case 233: //'é'
                    position_trajectoire+=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    trajectoire_vaisseau.renvoie_position(position_trajectoire,coordonnees_trajectoire);
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

                case 'g':
                    pos_anneau-=0.05;
                    break;
                case 'h':
                    pos_anneau+=0.05;
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
