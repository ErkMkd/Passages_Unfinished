#ifndef SCENE_LOGOGP_H_INCLUDED
#define SCENE_LOGOGP_H_INCLUDED

#define SCENE_LOGOGP_OK 0
#define SCENE_LOGOGP_ERREUR_INITS 1

#include "../Tunnel.h"
#include "../Utiles/Interpolations.h"
#include "Scene.h"
#include "../dksMoteur2d/dksEcran.h"

class Scene_LogoGP: public Scene
{
    public:
    Tunnel* le_tunnel;
    dksMoteur3d* scene;
    dksSourceLumineuse* source_1;
    dksVolume* logo_GP;
    dksOeil* oeil_1;

    dksSourceLumineuse* sources[10];
    dksOeil* yeux[10];
    dksVolume* volumes[10];
    dksConteneur* conteneurs[10];

    dksChargeLWO2* logo_lwo2;
    dksConvertionLWO2* logo_converti;

    dksChargeLWO2* sections_tunnel_lwo2;
    dksConvertionLWO2* sections_tunnel_converties;

    dksCube* cube_test;

    float ouverture_camera;

    //================= Trajectoires ======================
    dksTrajectoire trajectoire_1_oeil;
    dksTrajectoire trajectoire_1_oeil_visee;
    dksTrajectoire trajectoire_2_oeil;

    //================= Animations ======================

    //Drapeau d'initialisation des phases
    enum {
            TRAVELLING_1,
            TRAVELLING_2
            };
    bool etapes[5];

    //Interpolations:
    enum {
            POS_OEIL_1,
            POS_OEIL_VISEE_1,
            POS_OEIL_2
        };
    Interpolation_Parametre* interpolations_parametres[3];

    float position_trajectoire_camera;
    float position_trajectoire_camera_visee;

    //============= Données de l'éditeur:

        double vitesse_rotation;
        double vitesse_deplacement;
        vector<dksConteneur*> conteneurs_selectionnables;
        dksConteneur* conteneur_selectionne;
        char* fichier_trajectoire;
        dksTrajectoire ma_trajectoire;
        uint32_t point_actuel;
        float position_trajectoire;
        float coordonnees_trajectoire[3];
        float pas_position_trajectoire;

    //========================================================
    //          Constructeur
    //========================================================


    Scene_LogoGP(char* p_nom,dksEcran* p_ecran,Musique* p_musique):Scene(p_nom,p_ecran,p_musique)
    {

        sections_tunnel_converties=NULL;
        sections_tunnel_lwo2=NULL;
        scene=NULL;
        source_1=NULL;
        logo_converti=NULL;
        logo_lwo2=NULL;
        oeil_1=NULL;

        cube_test=NULL;

        uint32_t i;

        for (i=0;i<sizeof(etapes)/sizeof(bool);i++) etapes[i]=false;
        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres[i]=NULL;

        //======Création des volumes LWO2:

        //Les volumes d'affichage du logo:
        logo_lwo2=new dksChargeLWO2("Logo_debut/logo_2048.lwo","Logo_debut/");
        if (logo_lwo2==NULL || logo_lwo2->erreur!=DKSCHARGELWO2_OK)
        {
            erreur=SCENE_LOGOGP_ERREUR_INITS;
            return;
        }

        logo_converti=new dksConvertionLWO2(logo_lwo2);
        if (logo_converti==NULL || logo_converti->erreur!=DKSCONVERTIONLWO2_OK)
        {
            erreur=SCENE_LOGOGP_ERREUR_INITS;
            return;
        }

        //Sections du tunnel:
        sections_tunnel_lwo2=new dksChargeLWO2("LWO2/cercles_tunnel.lwo","textures/tunnel/");
        if (sections_tunnel_lwo2==NULL || sections_tunnel_lwo2->erreur!=DKSCHARGELWO2_OK)
        {
            erreur=SCENE_LOGOGP_ERREUR_INITS;
            return;
        }

        sections_tunnel_converties=new dksConvertionLWO2(sections_tunnel_lwo2);
        if (sections_tunnel_converties==NULL || sections_tunnel_converties->erreur!=DKSCONVERTIONLWO2_OK)
        {
            erreur=SCENE_LOGOGP_ERREUR_INITS;
            return;
        }

        le_tunnel=new Tunnel(sections_tunnel_converties->renvoie_volumes_convertis(),sections_tunnel_converties->renvoie_nbrVolumes(),
                                50,25.);

        //====== Création de la scène 3d:


        cube_test=new dksCube("cube_test",0.,0.,200.,10.,10.,10.,0xa0a0ff);

        oeil_1=new dksOeil("oeil_1",0.,0.,-1000.);
        oeil_courant=oeil_1;
        oeil_1->determine_limites_profondeur(1.,5000.);
        logo_GP=logo_converti->renvoie_volume_nom("logo_2048");
        source_1=new dksSourceLumineuse("Source_1");

        yeux[0]=oeil_1;
        sources[0]=source_1;
        volumes[0]=logo_GP;
        volumes[1]=cube_test;

        scene=new dksMoteur3d("Scene_LogoGP",ecran,NULL,0,volumes,1,yeux,1,sources,1,oeil_1);
        if (scene==NULL || scene->erreur!=DKSM3D_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }

        //======== Préparation des animations:

        initialise_animations();
        initialise_listes_affichage();

        //======== Derniers inits
        #if EDITEUR_ACTIF
        initialise_editeur();
        #endif

    }

    //==========================================================
    //          Destructeur
    //==========================================================
    ~Scene_LogoGP()
    {
        uint32_t i;
        if (scene!=NULL) delete scene;
        if (source_1!=NULL)delete source_1;
        if(oeil_1!=NULL) delete oeil_1;
        if (le_tunnel!=NULL) delete le_tunnel;
        if (sections_tunnel_converties!=NULL) delete sections_tunnel_converties;
        if (sections_tunnel_lwo2!=NULL) delete sections_tunnel_lwo2;
        if (logo_converti!=NULL)delete logo_converti;
        if (logo_lwo2!=NULL)delete logo_lwo2;

        if (cube_test!=NULL) delete cube_test;

        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) if (interpolations_parametres[i]!=NULL) delete interpolations_parametres[i];
    }

    //============================================
    //  Initialisations
    //============================================


    //----------------------------------------------------------------------------
    //                Active la scène
    //----------------------------------------------------------------------------
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

    //----------------------------------------------------------------------------
    //                 Mise à jour de la fenêtre, si on est pas en plein ecran
    //----------------------------------------------------------------------------
    virtual void maj_dimensions_fenetre()
    {
        scene->maj_resolutions_yeux();
    }

    //=================================================
    //                     Affichage
    //=================================================
    virtual void affiche(double t)
    {

        this->Scene::affiche(t);

        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        le_tunnel->maj(0);
        scene->maj(t);

        //post_rendu->affiche_couleur_tempon_ecran(oeil_courant);
        post_rendu->affiche_flou_256(oeil_courant);

        //affiche_trajectoire(&ma_trajectoire);

        //ralenti();
    }



    //========================================================================================================================================
    //========================================================================================================================================
    //                                                      Gestion animations
    //========================================================================================================================================
    //========================================================================================================================================
    public:
    //----------------------------------------------------------------------------
    //                  Etat de la scène au départ de l'animation
    //----------------------------------------------------------------------------
    virtual void initialise_t0(double t0)
    {
        this->Scene::initialise_t0(t0);
        float position[3];
        logo_GP->determine_position(0,0,-50.*25.);
        //oeil_1->determine_position(0.,0.,-1000.);

        trajectoire_1_oeil.renvoie_position(0.,position);
        oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));
        oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
        trajectoire_1_oeil_visee.renvoie_position(0.,position);
        oeil_1->determine_point_vise(double(position[0]),double(position[1]),double(position[2]));

        source_1->determine_position(100.,100.,-2000.);
        source_1->determine_intensite(1.);
        le_tunnel->vitesse_vecteur_generateur=20.;
        le_tunnel->determine_mode_deplacement_vecteur_generateur(TUNNEL_MODE_PERLIN);

        scene->determine_couleur_brouillard(0x0005444c);
        //scene->determine_couleur_brouillard(0xff022226);
        scene->determine_limites_brouillard(10.,600.);
        //scene->t0=t0;
        oeil_courant=oeil_1;
    }

    //----------------------------------------------------------------------------
    //                Initialise les animations
    //----------------------------------------------------------------------------
    private:
    void initialise_animations()
    {
        trajectoire_1_oeil.charge_trajectoire("trajectoires/logoGP_oeil.traj");
        trajectoire_2_oeil.charge_trajectoire("trajectoires/logoGP_oeil_2.traj");
        trajectoire_1_oeil_visee.charge_trajectoire("trajectoires/logoGP_oeil_visee.traj");

        Courbe_Bezier_Plan courbe_1(0.,0., 0.1,0., .75,1.,1.,1.);
        Courbe_Bezier_Plan courbe_2(0.,0., 0.1,0., .15,.88, 1.,1.);
        interpolations_parametres[POS_OEIL_1]=new Interpolation_Parametre(-1.,0.,1.,7.8,INTERPOLATION_BEZIER,&courbe_1);
        interpolations_parametres[POS_OEIL_2]=new Interpolation_Parametre(-1.,0.,1.,10.,INTERPOLATION_BEZIER,&courbe_2);
        interpolations_parametres[POS_OEIL_VISEE_1]=new Interpolation_Parametre(-1.,0.,1.,3.7,INTERPOLATION_COSINUS);
    }

    void initialise_listes_affichage()
    {
            //post_rendu->ajoute_rendu3d(le_tunnel,DKSECRAN_TEMPON_ECRAN,DKSRENDU3D_SURFACES_LUMINESCENTES);
            //post_rendu->ajoute_rendu3d(scene,DKSECRAN_TEMPON_ECRAN,DKSRENDU3D_COULEURS);

            post_rendu->ajoute_rendu(le_tunnel,DKSECRAN_TEMPON_256,DKSRENDU_COULEURS_LUMINESCENTES);
            //post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_256,DKSRENDU_COULEURS);
            post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
            post_rendu->alpha_tempon_256=1.;
            post_rendu->alpha_tempon_ecran=1.;
    }

    //======================================================
    //               Attente du retour du faisceau
    //              Déclenchements des animations
    //=======================================================
    public:
    virtual void attente_retour_faisceau()
    {
        float position[3];

        if (!drapeau_initialisee || drapeau_scene_terminee) return;

        if (position_temps_scene<7.8)
        {
            if(!etapes[TRAVELLING_1])
            {
                etapes[TRAVELLING_1]=true;
                oeil_1->contrainte_trajectoire=false;
                oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
            }

            position_trajectoire_camera=interpolations_parametres[POS_OEIL_1]->renvoie_valeur_parametre(position_temps_scene);
            trajectoire_1_oeil.renvoie_position(position_trajectoire_camera,position);
            oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));
            position_trajectoire_camera_visee=interpolations_parametres[POS_OEIL_VISEE_1]->renvoie_valeur_parametre(position_temps_scene);
            trajectoire_1_oeil_visee.renvoie_position(position_trajectoire_camera_visee,position);
            oeil_1->determine_point_vise(double(position[0]),double(position[1]),double(position[2]));

        }


        else if (position_temps_scene<17.8)
        {
            if(!etapes[TRAVELLING_2])
            {
                etapes[TRAVELLING_2]=true;
                //oeil_1->contrainte_trajectoire=true;
                oeil_1->determine_mode_visee(DKSOEIL_VISEE_LIBRE);

            }

            position_trajectoire_camera=interpolations_parametres[POS_OEIL_2]->renvoie_valeur_parametre(position_temps_scene);
            trajectoire_2_oeil.renvoie_position(position_trajectoire_camera,position);
            oeil_1->determine_position(double(position[0]),double(position[1]),double(position[2]));

        }

        else
        {
            oeil_1->toString();
            drapeau_scene_terminee=true;
        }



    }

    //========================================================================================================================================
    //===============================================================================================================================
    //                                      Gestion de l'éditeur
    //=====================================================================================================================================
    //=========================================================================================================================================


        virtual void initialise_editeur()
        {
            vitesse_deplacement=100.0;
            vitesse_rotation=0.1;

            oeil_1->valeur_propulsion_horizontale=vitesse_deplacement*1000.;
            oeil_1->masse=40.;
            oeil_1->determine_coefficient_frottement(.1,20.);

            source_1->valeur_propulsion_horizontale=vitesse_deplacement*1000.;
            source_1->masse=40.;
            source_1->determine_coefficient_frottement(.1,20.);

            position_trajectoire=0.;
            pas_position_trajectoire=0.001;
            fichier_trajectoire={"trajectoires/logoGP_oeil_visee.traj"};

            conteneurs_selectionnables.push_back(oeil_1);
            //conteneurs_selectionnables.push_back(source_1);
            conteneurs_selectionnables.push_back(cube_test);

            conteneur_selectionne=oeil_1;
        }

        //===================================================
        //          Affiche la trajectoire éditée
        //===================================================
        void affiche_trajectoire(dksTrajectoire* trajectoire)
        {
            desactive_textures();
            glUseProgram(0);
            //glLoadIdentity();
            glDisable(GL_LIGHTING);
            trajectoire->affiche_courbe(20,0x40ffff00);
            trajectoire->affiche_points(0x80ff0000,5);
            trajectoire->affiche_point(point_actuel,0xffff6060,8);
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
                        ma_trajectoire.determine_point(1,0.,716.493,32.);
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
                    vitesse_deplacement+=1.; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    conteneur_selectionne->valeur_propulsion_horizontale=vitesse_deplacement*1000.;
                    break;
                case '-':
                    vitesse_deplacement-=1.; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    conteneur_selectionne->valeur_propulsion_horizontale=vitesse_deplacement*1000.;
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
                    position_trajectoire-=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    conteneur_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;


                    break;
                case 233:// é
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
#endif // SCENE_LOGOGP_H_INCLUDED
