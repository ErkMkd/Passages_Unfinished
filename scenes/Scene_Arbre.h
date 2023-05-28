#ifndef SCENE_ARBRE_H_INCLUDED
#define SCENE_ARBRE_H_INCLUDED

/*

            Scène bucolique, avec un arbre à vector balls, un papillon...

*/


#include "Scene.h"
#include "../chargeurs_images/Image.h"
#include "../dksMoteur3d/Volumes/dksPlan.h"
#include "../dksMoteur3d/Volumes/dksSphere.h"
#include "../dksMoteur3d/Articules/Papillon.h"

class Scene_Arbre : public Scene
{
    public:
    Scene_Starfield* scene_starfield;

    dksPlan* sol_arbre;
    dksCube* cube_arriere_plan;

    dksChargeLWO2* arbre_lwo2;
    dksConvertionLWO2* arbre_converti;

    dksVolume* volume_positions_pommes;
    dksVolume* arbre;

    #define NBR_VECTORBALLS 66
    dksSphere* vectorBalls[NBR_VECTORBALLS];
    dksParticulesVolumes* particules_vectorBalls;

    dksOeil* oeil_1;
    dksConteneur* conteneur_oeil_1;

    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;

    dksMoteur3d* scene;

    Image* textures_arriere_plan[6];
    dksConteneur* conteneurs[10];
    dksVolume* volumes[10];
    dksOeil* yeux[10];
    dksSourceLumineuse* sources[10];
    float ouverture_camera;

    Papillon* papillon;

    //-------- Textures:
    Image* sol_relief;
    Image* sol_couleur;
    dksTexture* test_tex;

    //-------- Trajectoires:
    dksTrajectoire trajectoire_papillon;    //Suite de la trajectoire du vaisseau
    dksTrajectoire trajectoire_oeil_01; //Suite de la trajectoire de poursuite

    //--------
    Musique* la_musique;

    //Drapeau d'initialisation des phases
    enum {
            ARBRE_DEVELOPPEMENT
            };
    bool etapes[1];

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
    Scene_Arbre(char* p_nom,dksEcran* p_ecran,Musique* p_musique, Scene_Starfield* p_scene_starfield):Scene(p_nom,p_ecran,p_musique)
    {
        uint32_t i;

        scene_starfield=p_scene_starfield;

        sol_arbre=NULL;
        oeil_1=NULL;
        source_1=NULL;
        source_2=NULL;
        scene=NULL;
        sol_relief=NULL;
        sol_couleur=NULL;

        test_tex=NULL;
        cube_arriere_plan=NULL;

        arbre_lwo2=NULL;
        arbre_converti=NULL;

        particules_vectorBalls=NULL;

        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) textures_arriere_plan[i]=NULL;
        for (i=0;i<NBR_VECTORBALLS;i++) vectorBalls[i]=NULL;

        papillon=NULL;

        //------------- Chargement des volumes:

            arbre_lwo2=new dksChargeLWO2("LWO2/arbre_01.lwo","textures/arbre/");
            if (arbre_lwo2==NULL || arbre_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            arbre_converti=new dksConvertionLWO2(arbre_lwo2);
            if (arbre_converti==NULL || arbre_converti->erreur!=DKSCONVERTIONLWO2_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            arbre=arbre_converti->renvoie_volume_nom("arbre");
            volume_positions_pommes=arbre_converti->renvoie_volume_nom("positions_pommes");



        //----------------- Création des textures
        sol_relief=new Image("textures/arbre/sol_relief.tif");
        sol_couleur=new Image("textures/arbre/sol_couleur.tif");
        //sol_relief->determine_contraste(-0.8);
        //sol_relief->format_pixels_LUMINANCE();
        if(sol_relief==NULL || sol_relief->erreur!=IMAGE_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }
        if(sol_couleur==NULL || sol_couleur->erreur!=IMAGE_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }
        //----------------- Création des éléments de la scène:
        //Conteneurs:

            conteneur_oeil_1=new dksConteneur("conteneur_travelling",0.,0.,0.);
        //Yeux:
            oeil_1=new dksOeil("Oeil_1",0.,0.,-10.);
            double axes[9]={1,0,0,0,0.984573,-0.174974,0,0.174974,0.984573};
            oeil_1->determine_axes(axes);
            oeil_1->determine_position(0., 9.26971, -10.125);
            oeil_courant=oeil_1;
            conteneur_oeil_1->ajoute_enfant(oeil_1);

        //Création des sources:
            source_1=new dksSourceLumineuse("Soleil",100.,100.,-10.);
            source_1->determine_attenuation(1.,0.,0.);
            source_2=new dksSourceLumineuse("Clair_obscure",-100.,-100.,-10.);
            source_2->determine_attenuation(1.,0.,0.);
            source_2->determine_couleur_diffusion(0xff80e0a0);
            source_2->determine_couleur_speculaire(0x407050);

        //arbres:
            arbre->determine_position(0.,6.25,0.);

        //Sol:

            sol_arbre=new dksPlan("Sol",0.,0.,0.,
                                    150.,150.,        //Taille
                                    15,15,          //Mailles
                                    false,  //triangles/rectangles
                                    true    //recto/verso
                                    );
            if(sol_arbre==NULL || sol_arbre->erreur!=DKSPLAN_OK)
            {
                cout<<"ERREUR Lors de la création du sol de la scène: "<<nom<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            sol_arbre->surface_plan_recto->specularite=0.;
            sol_arbre->determine_texture_recto(sol_couleur);
            sol_arbre->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_TEXTURE_COULEUR;
            sol_arbre->determine_relief(sol_relief,7.,0.);

        //Papillon:
            papillon=new Papillon("Papy","LWO2/papillon.lwo",NULL,-1.37773, 8.84183, -6.80747);
            if(papillon==NULL || papillon->erreur!=PAPILLON_OK)
            {
                cout<<"ERREUR Lors de la création du papillon "<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }

        //-------------- Vector balls:
            for (i=0;i<NBR_VECTORBALLS;i++)
            {
                vectorBalls[i]=new dksSphere("baballe",3.,10.,0.,
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

            particules_vectorBalls=new dksParticulesVolumes("Particules_vectorBalls",arbre->x,arbre->y,arbre->z,(dksVolume**)vectorBalls,NBR_VECTORBALLS);
            particules_vectorBalls->determine_points(volume_positions_pommes->renvoie_sommets(),volume_positions_pommes->nbrSommets);
            particules_vectorBalls->determine_vitesses_rotations(0.,0.,0.);


        //------------------ Assignations:

            conteneurs[0]=conteneur_oeil_1;
            conteneurs[1]=papillon;
            volumes[0]=arbre;
            volumes[1]=sol_arbre;
            volumes[2]=particules_vectorBalls;
            sources[0]=source_1;
            sources[1]=source_2;
            yeux[0]=oeil_1;

        //--------------- Arrière plan:

            cube_arriere_plan=new dksCube(*scene_starfield->cube_arriere_plan_scene_arbre);

            /*
            cube_arriere_plan=new dksCube("Cube_arriere_plan_arbre",0.,0.,0.,10.,10.,10.,0xffffffff);
            textures_arriere_plan[0]=new Image("textures/arriere_plans/arbre_FR.jpg",false,true);
            textures_arriere_plan[1]=new Image("textures/arriere_plans/arbre_BK.jpg",false,true);
            textures_arriere_plan[2]=new Image("textures/arriere_plans/arbre_RT.jpg",false,true);
            textures_arriere_plan[3]=new Image("textures/arriere_plans/arbre_LF.jpg",false,true);
            textures_arriere_plan[4]=new Image("textures/arriere_plans/arbre_UP.jpg",false,true);
            textures_arriere_plan[5]=new Image("textures/arriere_plans/arbre_DN.jpg",false,true);


            cube_arriere_plan->ajoute_textures_arriere_plan(textures_arriere_plan[0],textures_arriere_plan[1],textures_arriere_plan[2],
                                                               textures_arriere_plan[3],textures_arriere_plan[4],textures_arriere_plan[5]);

            cube_arriere_plan->inverse_sens_polygones();
            cube_arriere_plan->determine_affecte_tempon_profondeur(false);
            */

        //------------------Création de la scene:

            scene=new dksMoteur3d("Arbre",ecran,
                            conteneurs,2,
                            volumes,3,
                            yeux,1,
                            sources,2,
                            oeil_1);

        scene->ajoute_volumes((dksVolume**)vectorBalls,NBR_VECTORBALLS,particules_vectorBalls);
        scene->ajoute_volumes((dksVolume**)papillon->volumes,PAPILLON_NBR_VOLUMES,NULL);

        scene->determine_cube_arriere_plan(cube_arriere_plan);

        //scene->determine_rendu_ombres(true,source_1,DKSECRAN_TEMPON_PROFONDEUR_2048);

        initialise_animations();
        initialise_listes_affichage();
        #if EDITEUR_ACTIF
        initialise_editeur();
        #endif
    }

    //=====================================================
    //          DESTRUCTEUR
    //=====================================================
    ~Scene_Arbre()
    {
        uint32_t i;
        if  (scene!=NULL) delete scene;
        if  (sol_arbre!=NULL) delete sol_arbre;
        if  (oeil_1!=NULL) delete oeil_1;
        if  (conteneur_oeil_1!=NULL) delete conteneur_oeil_1;
        if  (source_1!=NULL) delete source_1;
        if  (source_2!=NULL) delete source_2;
        if  (test_tex!=NULL) delete test_tex;
        if  (sol_relief!=NULL) delete sol_relief;
        if  (sol_couleur!=NULL) delete sol_couleur;
        if  (particules_vectorBalls!=NULL) delete particules_vectorBalls;
        for (i=0;i<NBR_VECTORBALLS;i++) if (vectorBalls[i]!=NULL) delete vectorBalls[i];
        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) if (textures_arriere_plan[i]!=NULL) delete textures_arriere_plan[i];
        if  (arbre_converti!=NULL) delete arbre_converti;
        if  (arbre_lwo2!=NULL) delete arbre_lwo2;
        if  (papillon!=NULL) delete papillon;
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
        particules_vectorBalls->maj_volumes();
        scene->maj(t);
        scene->affiche(oeil_courant);
        //papillon->maj_squelette();
        //papillon->maj_volumes();
        //papillon->affiche(oeil_courant);

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
        ouverture_camera=60.;
        oeil_courant=oeil_1;
        //cube_arriere_plan->rotation(-90.,1.,0.,0.);
    }

    public:

    //----------------------------------------------------------------------------
    //                  Etat de la scène au départ de l'animation
    //----------------------------------------------------------------------------
    virtual void initialise_t0(double t0)
    {
        this->Scene::initialise_t0(t0);
        oeil_1->determine_limites_profondeur(0.2,150.);
        oeil_1->determine_ouverture(scene_starfield->oeil_1->renvoie_ouverture());
        //scene->t0=t0;

        //oeil_1->copie_repere(scene_starfield->oeil_1->repere_absolu);
        //oeil_1->rotation(90.,1.,0.,0.);
        papillon->demarre_vol();
        papillon->determine_contrainte_verticale(true);
        papillon->determine_contrainte_roulis_rotation(true);


    }

    void initialise_listes_affichage()
    {
        //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_ECRAN,DKSRENDU3D_COULEURS);
        //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_QUART,DKSRENDU3D_SURFACES_LUMINESCENTES);
    }


    //======================================================
    //               Attente du retour du faisceau
    //              Gestion des animations
    //=======================================================

    virtual void attente_retour_faisceau()
    {
        uint32_t i;
        if (!drapeau_initialisee || drapeau_scene_terminee) return;



        if (position_temps_scene<10.)
            {
                if (!etapes[ARBRE_DEVELOPPEMENT])
                {
                    etapes[ARBRE_DEVELOPPEMENT]=true;
                }
                conteneur_oeil_1->rotation(0.005,0.,1.,0.);


            }
            else
            {
                drapeau_scene_terminee=true;
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
            vitesse_deplacement=10;
            vitesse_rotation=0.1;

            oeil_1->masse=10.;
            oeil_1->determine_coefficient_frottement(1.,100.);

            particules_vectorBalls->masse=10.;
            particules_vectorBalls->determine_coefficient_frottement(1.,100.);

            papillon->masse=10.;
            papillon->determine_coefficient_frottement(1.,100.);


            sol_arbre->masse=10.;
            sol_arbre->determine_coefficient_frottement(1.,100.);

            position_point=0.1;

            position_trajectoire=0.;
            pas_position_trajectoire=0.005;
            fichier_trajectoire={"trajectoires/trajectoire_papillon_01.traj"};

            conteneurs_selectionnables.push_back(oeil_1);
            conteneurs_selectionnables.push_back(particules_vectorBalls);
            conteneurs_selectionnables.push_back(papillon);

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
            float pas_angle=1.; //Sert pour l'éditeur d'animation du papillon
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
                #if EDITEUR_TRAJECTOIRE
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
                #endif

                //----------------------- Edition du papillon
                #if EDITEUR_PERSONNAGE


                case 'a':
                    papillon->pattes[INSECTE_PARD].angle_Fa-=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Fa-=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Fa-=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Fa-=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Fa-=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Fa-=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Fa<<endl;
                    break;

                 case 'z':
                    papillon->pattes[INSECTE_PARD].angle_Fa+=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Fa+=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Fa+=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Fa+=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Fa+=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Fa+=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Fa<<endl;
                    break;

                case 'e':
                    papillon->pattes[INSECTE_PARD].angle_Fr-=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Fr-=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Fr-=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Fr-=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Fr-=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Fr-=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Fr<<endl;
                    break;
                case 'r':
                    papillon->pattes[INSECTE_PARD].angle_Fr+=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Fr+=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Fr+=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Fr+=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Fr+=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Fr+=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Fr<<endl;
                    break;

                case 't':
                    papillon->pattes[INSECTE_PARD].angle_Ta-=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Ta-=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Ta-=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Ta-=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Ta-=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Ta-=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Ta<<endl;
                    break;
                case 'y':
                    papillon->pattes[INSECTE_PARD].angle_Ta+=pas_angle;
                    papillon->pattes[INSECTE_PARG].angle_Ta+=pas_angle;
                    papillon->pattes[INSECTE_PAVD].angle_Ta+=pas_angle;
                    papillon->pattes[INSECTE_PAVG].angle_Ta+=pas_angle;
                    papillon->pattes[INSECTE_PMD].angle_Ta+=pas_angle;
                    papillon->pattes[INSECTE_PMG].angle_Ta+=pas_angle;
                    cout<<papillon->pattes[INSECTE_PARD].angle_Ta<<endl;
                    break;

                case 'u':
                    papillon->angle_AntAd-=pas_angle;
                    papillon->angle_AntAg-=pas_angle;
                    break;
                case 'i':
                    papillon->angle_AntAd+=pas_angle;
                    papillon->angle_AntAg+=pas_angle;
                    break;

                case 'o':
                    papillon->angle_AntEd-=pas_angle;
                    papillon->angle_AntEg-=pas_angle;
                    break;
                case 'p':
                    papillon->angle_AntEd+=pas_angle;
                    papillon->angle_AntEg+=pas_angle;
                    break;

                case 'l':
                    papillon->angle_AntRd-=pas_angle;
                    papillon->angle_AntRg-=pas_angle;
                    break;
                case 'm':
                    papillon->angle_AntRd+=pas_angle;
                    papillon->angle_AntRg+=pas_angle;
                    break;

                case 'w':
                    cout<<"Fa="<<papillon->pattes[INSECTE_PARD].angle_Fa<<endl;
                    cout<<"Fr="<<papillon->pattes[INSECTE_PARD].angle_Fr<<endl;
                    cout<<"Ta="<<papillon->pattes[INSECTE_PARD].angle_Ta<<endl;
                    break;
                case 'x':

                    break;

                case 'c':

                    break;
                case 'v':
                    if(!papillon->drapeau_vol_plane) papillon->demarre_vol_plane();
                    else papillon->arrete_vol_plane();
                    break;

                case 'b':
                    if(!papillon->drapeau_vol) papillon->demarre_vol();
                    else papillon->arrete_vol();
                    break;




                #endif
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

#endif // SCENE_ARBRE_H_INCLUDED
