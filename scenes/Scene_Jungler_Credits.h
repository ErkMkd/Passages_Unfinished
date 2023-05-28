#ifndef SCENE_JUNGLER_CREDITS_H_INCLUDED
#define SCENE_JUNGLER_CREDITS_H_INCLUDED

/*

            Scène du jongleur et des crédits

*/


#include "Scene.h"
#include "../dksMoteur3d/Volumes/dksDamier.h"
#include "../dksMoteur3d/Articules/dksJongleurVectorBalls.h"

class Scene_Jungler_Credits : public Scene
{
    public:
    Scene_Greetings* scene_greets;

    dksJongleurVectorBalls* mon_bonhomme;
    dksCube* mon_cube;

    dksConteneur* conteneur_oeil_1;
    dksOeil* oeil_1;

    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;

    dksSphere* vectorBalls[NBR_VECTORBALLS];
    dksParticulesVolumes* particules_vectorBalls;

    dksDamier* damier;

    dksMoteur3d* scene;

    dksConteneur* conteneurs[10];
    dksVolume* volumes[10];
    dksOeil* yeux[10];
    dksSourceLumineuse* sources[10];
    float ouverture_camera;

    //Drapeaux d'initialisationx des phases
    enum {
            CREDITS_DEVELOPPEMENT
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
    Scene_Jungler_Credits(char* p_nom,dksEcran* p_ecran,Musique* p_musique,Scene_Greetings* p_scene_greets):Scene(p_nom,p_ecran,p_musique)
    {
        scene_greets=p_scene_greets;

        mon_cube=NULL;
        oeil_1=NULL;
        source_1=NULL;
        source_2=NULL;
        scene=NULL;
        damier=NULL;
        mon_bonhomme=NULL;
        conteneur_oeil_1=NULL;
        particules_vectorBalls=NULL;

        uint32_t i;
        for (i=0;i<NBR_VECTORBALLS;i++) vectorBalls[i]=NULL;

        //-------------- Vector balls:
            for (i=0;i<NBR_VECTORBALLS;i++)
            {
                vectorBalls[i]=new dksSphere("baballe_juggler",3.,10.,0.,
                                   8,8,
                                   .1,
                                   0xffff4040);
                if(vectorBalls[i]==NULL || vectorBalls[i]->erreur!=DKSVOLUME_OK)
                {
                    cout<<"ERREUR lors de la création des vectorBalls"<<endl;
                    erreur=SCENE_ERREUR_INIT;
                    return ;
                }

                vectorBalls[i]->surfaces_sphere[0]->determine_phong(true);
                //vectorBalls[i]->determine_recoit_ombres(false);
                //vectorBalls[i]->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
            }

            //particules_vectorBalls=new dksParticulesVolumes("Particules_vectorBalls_jungler",10.,0.,0.,(dksVolume**)vectorBalls,NBR_VECTORBALLS);
            //particules_vectorBalls->determine_vitesses_rotations(0.,0.,0.);

        //----------------- Création du damier:
            damier=new dksDamier("Damier_jongleur",0.,0.,0.,
                                    100.,100.,        //Taille
                                    15,15,          //Mailles
                                    false,  //triangles/rectangles
                                    false,    //recto/verso
                                    0xfffe99,
                                    0xf6c2d6
                                    );
            if(damier==NULL || damier->erreur!=DKSPLAN_OK)
            {
                cout<<"ERREUR Lors de la création du damier de la scène: "<<nom<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            //damier->determine_mode_remplissage(DKSVOLUME_REMPLISSAGE_ARETES);
            damier->surface_plan_recto_1->specularite=0.;
            damier->surface_plan_recto_1->brillance=100.;
            damier->surface_plan_recto_2->specularite=0.;
            damier->surface_plan_recto_2->brillance=100.;
            //damier->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_COULEUR_PHONG;
            damier->surface_plan_recto_1->drapeau_adoucissement=true;
            damier->surface_plan_recto_2->drapeau_adoucissement=true;
            damier->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);

        //---------------- Création du jongleur:


            mon_bonhomme=new dksJongleurVectorBalls("Paul",0.,0.,0.,vectorBalls,NBR_VECTORBALLS);
            if(mon_bonhomme->erreur!=DKSJONGLEUR_OK)
            {
                cout<<"ERREUR Lors de la création du jongleur "<<endl;
                erreur=SCENE_ERREUR_INIT;
                return;
            }

            mon_bonhomme->determine_siteswap(JONGLEUR_SITESWAP_5);
            mon_bonhomme->determine_echelle_geometrique(5.);
            mon_bonhomme->rotation(M_PI,0.,1.,0.);


        //----------------- Création des éléments de la scène:
        //Yeux:
            conteneur_oeil_1=new dksConteneur("conteneur_travelling",0.,0.,0.);
            oeil_1=new dksOeil("Oeil_1",0.,0.,-10.);
            oeil_courant=oeil_1;
            conteneur_oeil_1->ajoute_enfant(oeil_1);

            oeil_1->determine_position(-3.63412, 6.20803, -11.7939);
            //oeil_1->determine_position(-6.56084, 6.08285, -20.1927);
            double axes[9]={0.944307,-2.89037e-017,-0.329065,0.0260855,0.996853,0.0748566,0.328029,-0.0792715,0.941336};
            oeil_1->determine_axes(axes);

        //Création des sources:
            source_1=new dksSourceLumineuse("Source_1",-9.06262, 29.3366, -15.7745);
            source_1->determine_attenuation(1.,0.005,0.);
            axes={0.867633,0.0304161,-0.496274,0.376909,0.610733,0.69638,0.324272,-0.791252,0.518428};
            source_1->determine_axes(axes);

            source_2=new dksSourceLumineuse("Source_2",-50.,-50.,10.);
            source_2->determine_attenuation(1.,0.,0.);
            source_2->determine_couleur_diffusion(0x603080);
            source_2->determine_couleur_speculaire(0x402060);

        //Cubes:
            mon_cube=new dksCube("Cube_test",0.,0.,2.,1.,1.,4.,0xffffff00);
            mon_cube->determine_recoit_ombres(false);

        //Assignations:
            volumes[0]=damier;
            volumes[1]=mon_cube;
            sources[0]=source_1;
            sources[1]=source_2;
            yeux[0]=oeil_1;
            conteneurs[0]=mon_bonhomme;
            conteneurs[1]=conteneur_oeil_1;

            source_1->ajoute_enfant(mon_cube);

        //Création de la scene:
            scene=new dksMoteur3d("Credits",ecran,
                            conteneurs,2,
                            volumes,2,
                            yeux,1,
                            sources,2,
                            oeil_1);

            scene->ajoute_volumes((dksVolume**)vectorBalls,NBR_VECTORBALLS,mon_bonhomme);

            scene->determine_rendu_ombres(true,source_1,DKSECRAN_TEMPON_PROFONDEUR_2048,0.5);
            scene->determine_lumiere_ambiante(0.,0.,0.,1.);

        initialise_animations();
        initialise_listes_affichage();
        #if EDITEUR_ACTIF
        initialise_editeur();
        #endif
    }

    //=====================================================
    //          DESTRUCTEUR
    //=====================================================
    ~Scene_Jungler_Credits()
    {
        uint32_t i;
        if (scene!=NULL) delete scene;
        if (mon_cube!=NULL) delete mon_cube;
        if (oeil_1!=NULL) delete oeil_1;
        if (source_1!=NULL) delete source_1;
        if (source_2!=NULL) delete source_2;
        if  (particules_vectorBalls!=NULL) delete particules_vectorBalls;
        for (i=0;i<NBR_VECTORBALLS;i++) if (vectorBalls[i]!=NULL) delete vectorBalls[i];
        if (damier!=NULL)delete damier;
        if(mon_bonhomme!=NULL) delete mon_bonhomme;
        if(conteneur_oeil_1!=NULL) delete conteneur_oeil_1;
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
        //particules_vectorBalls->maj_volumes();
        scene->affiche(oeil_courant);
        mon_bonhomme->maj_jongleur(t);
        //mon_bonhomme->applique_mouvements(t,scene->delta_t);
        //mon_bonhomme->affiche(oeil_courant);

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
        //particules_vectorBalls->determine_points(scene_greets->particules_vectorBalls->renvoie_points(),NBR_VECTORBALLS);
        ouverture_camera=60.;
        oeil_courant=oeil_1;
    }

    public:
    //----------------------------------------------------------------------------
    //                  Etat de la scène au départ de l'animation
    //----------------------------------------------------------------------------
    virtual void initialise_t0(double t0)
    {
        this->Scene::initialise_t0(t0);
        oeil_1->determine_ouverture(ouverture_camera);

        mon_bonhomme->depart_jonglage();
        //scene->t0=t0;
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
        if (!drapeau_initialisee || drapeau_scene_terminee) return;
        //conteneur_oeil_1->rotation(0.005,0.,1.,0.);



        if (position_temps_scene<15.)
            {
                if (!etapes[CREDITS_DEVELOPPEMENT])
                {
                    etapes[CREDITS_DEVELOPPEMENT]=true;
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
            position_point=0.1;

            mon_bonhomme->masse=10.;
            mon_bonhomme->determine_coefficient_frottement(1.,100.);

            source_1->masse=10.;
            source_1->determine_coefficient_frottement(1.,100.);

            position_trajectoire=0.;
            pas_position_trajectoire=0.005;
            fichier_trajectoire={"trajectoires/camera_jungler_01.traj"};

            conteneurs_selectionnables.push_back(oeil_1);
            conteneurs_selectionnables.push_back(mon_bonhomme);
            conteneurs_selectionnables.push_back(source_1);

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
            //cout<<uint16_t(key)<<endl;
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


                case '5':
                    cout<<"Repère relatif: "<<conteneur_selectionne->axe_x->x<<","<<conteneur_selectionne->axe_x->y<<","<<conteneur_selectionne->axe_x->z<<","
                                            <<conteneur_selectionne->axe_y->x<<","<<conteneur_selectionne->axe_y->y<<","<<conteneur_selectionne->axe_y->z<<","
                                            <<conteneur_selectionne->axe_z->x<<","<<conteneur_selectionne->axe_z->y<<","<<conteneur_selectionne->axe_z->z<<endl;

                    cout<<"Repère absolu: "<<rep->axe_x->x<<","<<rep->axe_x->y<<","<<rep->axe_x->z<<","
                                            <<rep->axe_y->x<<","<<rep->axe_y->y<<","<<rep->axe_y->z<<","
                                            <<rep->axe_z->x<<","<<rep->axe_z->y<<","<<rep->axe_z->z<<endl;
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

                //Editeur de source lumineuse
                case 'o':
                    if (scene->renvoie_oeil_courant()==oeil_1) {scene->determine_oeil_courant(source_1); conteneur_selectionne=source_1;}
                    else {scene->determine_oeil_courant(oeil_1);conteneur_selectionne=oeil_1;}
                    break;

                case 'y':
                    source_1->determine_ouverture(source_1->renvoie_ouverture()+0.5);
                    cout<<source_1->renvoie_ouverture()<<endl;
                    break;
                case 't':
                    source_1->determine_ouverture(source_1->renvoie_ouverture()-0.5);
                    cout<<source_1->renvoie_ouverture()<<endl;
                    break;

                #endif

                #if EDITEUR_PERSONNAGE

                //==== Contrôles personnage:

                //Bras:
                case 'a':
                    mon_bonhomme->angle_Pda-=5.;
                    mon_bonhomme->angle_Pga-=5.;
                    break;
                case 'z':
                    mon_bonhomme->angle_Pda+=5.;
                    mon_bonhomme->angle_Pga+=5.;
                    break;

                case 'e':
                    mon_bonhomme->angle_Cd-=5.;
                    mon_bonhomme->angle_Cg-=5.;
                    break;
                case 'r':
                    mon_bonhomme->angle_Cd+=5.;
                    mon_bonhomme->angle_Cg+=5.;
                    break;

                case 't':
                    mon_bonhomme->angle_Eda-=5.;
                    mon_bonhomme->angle_Ega-=5.;
                    break;
                case 'y':
                    mon_bonhomme->angle_Eda+=5.;
                    mon_bonhomme->angle_Ega+=5.;
                    break;
                case 'u':
                    mon_bonhomme->angle_Ede-=5.;
                    mon_bonhomme->angle_Ege-=5.;
                    break;
                case 'i':
                    mon_bonhomme->angle_Ede+=5.;
                    mon_bonhomme->angle_Ege+=5.;
                    break;
                case 'o':
                    mon_bonhomme->angle_Edr-=5.;
                    mon_bonhomme->angle_Egr-=5.;
                    break;
                case 'p':
                    mon_bonhomme->angle_Edr+=5.;
                    mon_bonhomme->angle_Egr+=5.;
                    break;

                //Jambes:
                case 'q':
                    mon_bonhomme->angle_Vd-=5.;
                    mon_bonhomme->angle_Vg-=5.;
                    break;
                case 's':
                    mon_bonhomme->angle_Vd+=5.;
                    mon_bonhomme->angle_Vg+=5.;
                    break;
                case 'd':
                    mon_bonhomme->angle_Gd-=5.;
                    mon_bonhomme->angle_Gg-=5.;
                    break;
                case 'f':
                    mon_bonhomme->angle_Gd+=5.;
                    mon_bonhomme->angle_Gg+=5.;
                    break;

                case 'g':
                    mon_bonhomme->angle_Hda-=5.;
                    mon_bonhomme->angle_Hga-=5.;
                    break;
                case 'h':
                    mon_bonhomme->angle_Hda+=5.;
                    mon_bonhomme->angle_Hga+=5.;
                    break;
                case 'j':
                    mon_bonhomme->angle_Hde-=5.;
                    mon_bonhomme->angle_Hge-=5.;
                    break;
                case 'k':
                    mon_bonhomme->angle_Hde+=5.;
                    mon_bonhomme->angle_Hge+=5.;
                    break;


                case 'w':
                    mon_bonhomme->angle_Pda-=5.;
                    mon_bonhomme->angle_Pga-=5.;
                    break;
                case 'x':
                    mon_bonhomme->angle_Pda+=5.;
                    mon_bonhomme->angle_Pga+=5.;
                    break;
                case 'c':
                    mon_bonhomme->angle_Pde-=5.;
                    mon_bonhomme->angle_Pge-=5.;
                    break;
                case 'v':
                    mon_bonhomme->angle_Pde+=5.;
                    mon_bonhomme->angle_Pge+=5.;
                    break;
                case 'b':
                    mon_bonhomme->angle_Pdr-=5.;
                    mon_bonhomme->angle_Pgr-=5.;
                    break;
                case 'n':
                    mon_bonhomme->angle_Pdr+=5.;
                    mon_bonhomme->angle_Pgr+=5.;
                    break;

                case '$':
                    mon_bonhomme->angles_to_string();
                    break;

                case '&':
                    mon_bonhomme->depart_jonglage();

                    break;
                case 233: //'é'
                   mon_bonhomme->stop_jonglage();
                    break;
                case 34: //'"'


                    break;
                case 39: // '

                    break;
                #endif

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

#endif // SCENE_JUNGLER_CREDITS_H_INCLUDED
