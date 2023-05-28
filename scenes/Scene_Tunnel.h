#ifndef SCENE_TUNNEL_H_INCLUDED
#define SCENE_TUNNEL_H_INCLUDED


#define SCENE_TUNNEL_OK 0
#define SCENE_TUNNEL_ERREUR_INITS 1

#include "../Tunnel.h"
#include "../dksMoteur3d/Texte/dksTexteMap.h"
#include "../dksMoteur3d/Volumes/Annelide.h"
#include "../dksMoteur3d/Volumes/dksPlan.h"
#include "../dksMoteur3d/Volumes/VolumeFacesSeparees.h"
#include "../Utiles/Interpolations.h"
#include "Scene.h"
#include "Scene_LogoGP.h"
#include "../dksMoteur2d/dksEcran.h"
#include "../Utiles/Bruit.h"



class Scene_Tunnel: public Scene
{
    public:
    Scene_LogoGP* scene_logoGp;

    dksOeil* oeil_1;
    dksOeil* oeil_2;

    dksChargeLWO2* trajectoire_caducee_lwo2;
    dksConvertionLWO2* trajectoire_caducee_convertie;

    dksCube* cube_arriere_plan;
    Image* textures_arriere_plan[6];
    Image* textures_arriere_plan_transition[6]; //Les textures de l'arrière plan de la scène suivante
    #define TAILLE_TEXTURE_TRANSITION 128
    uint8_t texture_transition_arriere_plan[TAILLE_TEXTURE_TRANSITION*TAILLE_TEXTURE_TRANSITION];
    Image* image_transition_arriere_plan;

    Annelide* mon_ver;
    Image* image_tete_ver;
    Image* image_queue_ver;
    Image* image_dos_ver;
    Image* image_ventre_ver;

    dksMoteur3d* scene;
    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;
    dksConteneur* conteneur_tunnel;
    dksVolume* transition_caducee;
    dksVolume* transition_caducee_corps_2;
    VolumeFacesSeparees* transition_caducee_particules;

    double echelle_caducee;

    dksSourceLumineuse* sources[10];
    dksOeil* yeux[10];
    dksVolume* volumes[10];
    dksConteneur* conteneurs[10];

    Tunnel* le_tunnel;
    dksTrajectoire trajectoire_caducee;

    dksTextMap* texte_GP;
    dksTextMap* texte_Presents;
    dksTextMap* texte_Passages;
    dksPlan* logo2_GP;
    Image* logo2_bitmap;


    dksConteneur* conteneur_texte_1;
    dksConteneur* conteneur_texte_2;
    dksConteneur* conteneur_texte_3;

    //Piositions des objets dans le tunnel:
    float position_conteneur_tunnel;
    float position_ver[3];
    float position_repere_texte_1;
    float position_repere_texte_2;
    float position_repere_texte_3;

    dksTrajectoire trajectoire_camera_caducee_01;


    //================= Animations ======================

    //Drapeau d'initialisation des phases
    enum {
            GENERIQUE,
            VER,
            FACE_VER,
            TRANSITION_VUE1,
            TRANSITION_VUE2
            };
    bool etapes[5];
    //Interpolations:
    enum {
            POS_GPLOGO,
            POS_PRESENTS,
            POS_PASSAGES,
            POS_VER,
            POSX_OEIL_2,
            POSZ_OEIL_2,
            POSITION_ANIMATION_FACES_CADUCEE,
            ALPHA_SCENE,
            ALPHA_AURAS,
            VITESSE_TUNNEL,
            CAMERA_POS_TRAJECTOIRE_01,
            TRANSITION_TEXTURES_ARRIERE_PLAN
        };
    Interpolation_Parametre* interpolations_parametres[12];
    float points_enveloppe_camera_pos_trajectoire_01[3*2];
    enum
        {
            REPERE_CUBE_ARRIERE_PLAN
        };

    Interpolation_Repere* interpolations_reperes[1];

    bool drapeau_depart_faces;
    bool drapeau_vise_faces;

    float position_depart_faces;
    uint32_t index_depart_faces_animees;
    float point_vise[3];
    float position[3];
    float tangente[3];

    //==== Paramètres de l'éditeur:
    double pas_deplacement;
    double pas_rotation;
    uint8_t mode_deplacement_tunnel;

    //========================================================
    //          Constructeur
    //========================================================

    public:
    Scene_Tunnel(char* p_nom,dksEcran* p_ecran,Musique* p_la_musique,Scene_LogoGP* p_scene_logoGp):Scene(p_nom,p_ecran,p_la_musique)
    {
        uint32_t i;
        trajectoire_caducee_lwo2=NULL;
        trajectoire_caducee_convertie=NULL;
        transition_caducee_particules=NULL;
        image_transition_arriere_plan=NULL;
        cube_arriere_plan=NULL;
        oeil_1=NULL;
        scene=NULL;
        texte_GP=NULL;
        texte_Presents=NULL;
        texte_Passages=NULL;
        logo2_GP=NULL;
        logo2_bitmap=NULL;
        conteneur_texte_1=NULL;
        conteneur_texte_2=NULL;
        conteneur_texte_3=NULL;
        echelle_caducee=150.;
        image_dos_ver=NULL;
        image_ventre_ver=NULL;
        image_queue_ver=NULL;
        image_tete_ver=NULL;

        for (i=0;i<sizeof(etapes)/sizeof(bool);i++) etapes[i]=false;
        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres[i]=NULL;
        for (i=0;i<sizeof(interpolations_reperes)/sizeof(Interpolation_Repere*);i++) interpolations_reperes[i]=NULL;
        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) textures_arriere_plan[i]=NULL;
        for (i=0;i<sizeof(textures_arriere_plan_transition)/sizeof(Image*);i++) textures_arriere_plan_transition[i]=NULL;

        scene_logoGp=p_scene_logoGp;

        //======Création des volumes LWO2:

        //Sections du tunnel:
        /*
        sections_tunnel_lwo2=new dksChargeLWO2("LWO2/cercles_tunnel.lwo","textures/tunnel/");
        if (sections_tunnel_lwo2==NULL || sections_tunnel_lwo2->erreur!=DKSCHARGELWO2_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }

        volumes_convertis=new dksConvertionLWO2(sections_tunnel_lwo2);
        if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }
        */

        //Coordonnées de la trajectoire du caducée et des volumes de transition:
        trajectoire_caducee_lwo2=new dksChargeLWO2("LWO2/trajectoire_caducee.lwo");
        if (trajectoire_caducee_lwo2==NULL || trajectoire_caducee_lwo2->erreur!=DKSCHARGELWO2_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }

        trajectoire_caducee_convertie=new dksConvertionLWO2(trajectoire_caducee_lwo2);
        if (trajectoire_caducee_convertie==NULL || trajectoire_caducee_convertie->erreur!=DKSCONVERTIONLWO2_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }
        //========= Trajectoire de la caméra:

        char fichier_trajectoire[64]={"trajectoires/camera_caducee_01.traj"};
        if (!trajectoire_camera_caducee_01.charge_trajectoire(fichier_trajectoire))
        {
            cout<<"ERREUR - Trajectoire "<<fichier_trajectoire<<" impossible à charger"<<endl;
        }
        trajectoire_camera_caducee_01.determine_echelle_trajectoire(echelle_caducee);
        trajectoire_camera_caducee_01.determine_drapeau_longueurs(true);

                                    cout<<"LONGUEUR TRAJECTOIRE ------------- > "<<trajectoire_camera_caducee_01.renvoie_longueur_totale()<<endl;

        //======== Initialise de l'arrière plan
        textures_arriere_plan[0]=new Image("textures/tunnel/ocean_1024.tif");
        textures_arriere_plan[0]->determine_luminosite(-0.5);
        cube_arriere_plan=new dksCube("Cube_arriere_plan_tunnel",0.,0.,0.,10.,10.,10.,0xffffffff);
        cube_arriere_plan->ajoute_textures_arriere_plan(textures_arriere_plan[0],textures_arriere_plan[0],textures_arriere_plan[0],
                                                           textures_arriere_plan[0],textures_arriere_plan[0],textures_arriere_plan[0]);


        textures_arriere_plan_transition[0]=new Image("textures/arriere_plans/caducee_ciel_FR.jpg",false,true);
        textures_arriere_plan_transition[1]=new Image("textures/arriere_plans/caducee_ciel_BK.jpg",false,true);
        textures_arriere_plan_transition[2]=new Image("textures/arriere_plans/caducee_ciel_RT.jpg",false,true);
        textures_arriere_plan_transition[3]=new Image("textures/arriere_plans/caducee_ciel_LF.jpg",false,true);
        textures_arriere_plan_transition[4]=new Image("textures/arriere_plans/caducee_ciel_UP.jpg",false,true);
        textures_arriere_plan_transition[5]=new Image("textures/arriere_plans/caducee_ciel_DN.jpg",false,true);


        cube_arriere_plan->ajoute_textures_arriere_plan(textures_arriere_plan_transition[0],textures_arriere_plan_transition[1],textures_arriere_plan_transition[2],
                                                           textures_arriere_plan_transition[3],textures_arriere_plan_transition[4],textures_arriere_plan_transition[5]);


        if (!Bruit::genere_bruit_de_Perlin_2D_8bits(4,4,TAILLE_TEXTURE_TRANSITION,TAILLE_TEXTURE_TRANSITION,12,0.75,texture_transition_arriere_plan))
        {
            cout<<"ERREUR dans Scene_Tunnel() - Impossible de créer la texture de transition"<<endl;
            erreur=SCENE_ERREUR_INIT;
            return;
        }

        image_transition_arriere_plan=new Image("Texture_transition",texture_transition_arriere_plan,
                                                TAILLE_TEXTURE_TRANSITION,TAILLE_TEXTURE_TRANSITION,
                                                GL_LUMINANCE,GL_UNSIGNED_BYTE,8,1);


        //image_transition_arriere_plan->renversement_horizontal();
        //image_transition_arriere_plan->renversement_vertical();

        cube_arriere_plan->ajoute_textures_arriere_plan(image_transition_arriere_plan,image_transition_arriere_plan,image_transition_arriere_plan,
                                                           image_transition_arriere_plan,image_transition_arriere_plan,image_transition_arriere_plan);


        cube_arriere_plan->determine_rendu_transition_surfaces(true);
        cube_arriere_plan->determine_indices_textures_transition(0,1,2);

        cube_arriere_plan->inverse_sens_polygones();

        cube_arriere_plan->determine_affecte_tempon_profondeur(false);

        cube_arriere_plan->determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);

        //======== Initialise les éléments de la scène:

        source_1=new dksSourceLumineuse("Source_1",100.,100.,10.);
        source_2=new dksSourceLumineuse("Source_2",-100.,-100.,10.);
        source_1->determine_attenuation(1.,0.001,0.);
        source_2->determine_attenuation(.8,0.001,0.);
        source_2->determine_couleur_diffusion(0xffa0a0ff);

        //======== Le ver:
        mon_ver=new Annelide("mon_ver",0.,1.,0.,
                             32,12,
                             .25,1.,1.5,
                             20.,
                             0xff806000,true);
        mon_ver->elasticite=4.;
        image_tete_ver=new Image("textures/annelide/tete.tif",true,false);
        image_queue_ver=new Image("textures/annelide/queue.tif",true,false);
        image_dos_ver=new Image("textures/annelide/peau_dos_01.tif");
        image_ventre_ver=new Image("textures/annelide/peau_ventre_01.tif");
        if (image_queue_ver==NULL || image_queue_ver->erreur!=IMAGE_OK
            || image_tete_ver==NULL || image_tete_ver->erreur!=IMAGE_OK
            || image_dos_ver==NULL || image_dos_ver->erreur!=IMAGE_OK
            || image_ventre_ver==NULL || image_ventre_ver->erreur!=IMAGE_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

        mon_ver->ajoute_texture_couleur_sections(image_tete_ver,ANNELIDE_SURFACE_TETE);
        mon_ver->ajoute_texture_couleur_sections(image_dos_ver,ANNELIDE_SURFACE_DOS);
        mon_ver->ajoute_texture_couleur_sections(image_ventre_ver,ANNELIDE_SURFACE_VENTRE);
        mon_ver->ajoute_texture_couleur_sections(image_queue_ver,ANNELIDE_SURFACE_QUEUE);
        if (!mon_ver->initialise_textures())
        {
           erreur=SCENE_ERREUR_INIT;
            return;
        }
        mon_ver->initialise_modes_rendus_surfaces();

        //=========== Les Yeux:
        oeil_1=new dksOeil("oeil_1");
        oeil_2=new dksOeil("oeil_2");
        oeil_courant=oeil_1;
        oeil_1->determine_limites_profondeur(1.,10000.);
        oeil_2->determine_limites_profondeur(1.,10000.);

        //oeil_1->determine_mode_visee(DKSOEIL_VISEE_POINT);
        //oeil_1->determine_visee_inertielle(false);

        conteneur_tunnel=new dksConteneur("Repere_dans_le_tunnel",0,0,0);
        conteneur_tunnel->ajoute_enfant(oeil_1);
        conteneur_tunnel->ajoute_enfant(oeil_2);
        conteneur_tunnel->ajoute_enfant(mon_ver);
        conteneur_tunnel->ajoute_enfant(source_1);
        conteneur_tunnel->ajoute_enfant(source_2);


        //========== initialisation du texte:
        conteneur_texte_1=new dksConteneur("texte_1",0.,0.,0.);
        conteneur_texte_2=new dksConteneur("texte_2",0.,0.,0.);
        conteneur_texte_3=new dksConteneur("texte_3",0.,0.,0.);
        texte_GP=new dksTextMap("polices/Groupe.tif",ecran,"Texte_GP",0.14,4.,0.15,0.5);    //Base,echelle,espace lettres, espace
        texte_GP->determine_position_curseur(0.1,0.1);
        texte_GP->determine_texte("GENESIS PROJECT");
        texte_Presents=new dksTextMap("polices/Groupe.tif",ecran,"Texte_GP",0.14,4.,0.15,0.5);
        texte_Presents->determine_position_curseur(0.1,0.1);
        texte_Presents->determine_texte("Presents");
        texte_Passages=new dksTextMap("polices/Groupe.tif",ecran,"Texte_GP",0.14,8.,0.15,0.5);
        texte_Passages->determine_texte("PASSAGES");
        texte_Passages->determine_position_curseur(0.1,0.1);

        conteneur_texte_1->ajoute_enfant(texte_GP);
        conteneur_texte_2->ajoute_enfant(texte_Presents);
        conteneur_texte_3->ajoute_enfant(texte_Passages);

        logo2_GP=new dksPlan("Logo_2",0.,0.,0.,
                             40.,10.,
                             1,1);

        logo2_bitmap=new Image("textures/tunnel/GP_logo_by_Razorback_001.tif");

            if(logo2_bitmap==NULL || logo2_bitmap->erreur!=IMAGE_OK)
            {
                erreur=SCENE_ERREUR_INIT;
                return;
            }

        logo2_GP->determine_texture_recto(logo2_bitmap);
        //logo2_GP->determine_mode_remplissage(DKSVOLUME_REMPLISSAGE_ARETES);
        logo2_GP->surface_plan_recto->mode_rendu=DKSSURFACE_RENDU_TEXTURE_COULEUR;
        logo2_GP->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
        logo2_GP->surface_plan_recto->determine_transparence(0.01);
        logo2_GP->initialise_faces_transparentes();
        logo2_GP->determine_rendu_faces_transparentes(true);
        logo2_GP->rotation(-90/180.*M_PI,1.,0.,0.);
        logo2_GP->rotation(M_PI,0.,0.,1.);
        logo2_GP->determine_affecte_tempon_profondeur(false);
        conteneur_texte_1->ajoute_enfant(logo2_GP);

        //========= initialisation du tunnel:

        dksVolume* traj=trajectoire_caducee_convertie->renvoie_volume_index(0);

        traj->determine_echelle_geometrique(echelle_caducee);
        trajectoire_caducee.determine_drapeau_longueurs(false);    //Pour éviter des calculs inutiles
        trajectoire_caducee.determine_points(traj->renvoie_sommets(),traj->nbrSommets);
        trajectoire_caducee.tri_points(DKSTRAJECTOIRE_AXE_Y,true);
        trajectoire_caducee.determine_drapeau_longueurs(true);

        le_tunnel=scene_logoGp->le_tunnel;
        le_tunnel->determine_trajectoire_vecteur_generateur(&trajectoire_caducee);

        /*le_tunnel=new Tunnel(volumes_convertis->renvoie_volumes_convertis(),volumes_convertis->renvoie_nbrVolumes(),
                                50,25.,
                                &trajectoire_caducee);
                                */

        //========== initialisation de la transition avec le caducée:
        transition_caducee=trajectoire_caducee_convertie->renvoie_volume_index(1);
        transition_caducee->determine_echelle_geometrique(echelle_caducee);
        transition_caducee_particules=new VolumeFacesSeparees("transition_volume",transition_caducee);
        transition_caducee_particules->determine_affecte_par_brouillard(false);
        transition_caducee_particules->determine_liste_luminescences(false);

        transition_caducee_corps_2=trajectoire_caducee_convertie->renvoie_volume_index(2);
        transition_caducee_corps_2->determine_echelle_geometrique(echelle_caducee);
        transition_caducee_corps_2->determine_affecte_par_brouillard(false);
        transition_caducee_corps_2->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);


        //========== Création de la scène:
        conteneurs[0]=conteneur_tunnel;
        conteneurs[1]=conteneur_texte_1;
        conteneurs[2]=conteneur_texte_2;
        conteneurs[3]=conteneur_texte_3;
        conteneurs[4]=texte_GP;
        conteneurs[5]=texte_Presents;
        conteneurs[6]=texte_Passages;

        volumes[0]=mon_ver;
        volumes[1]=transition_caducee_particules;
        //volumes[2]=transition_caducee_corps_2;
        volumes[2]=logo2_GP;

        sources[0]=source_1;
        sources[1]=source_2;

        yeux[0]=oeil_1;
        yeux[1]=oeil_2;

        scene=new dksMoteur3d("Scene_tunnel",ecran,conteneurs,7,volumes,2,yeux,2,sources,2,oeil_1);
        if (scene==NULL || scene->erreur!=DKSM3D_OK)
        {
            erreur=SCENE_ERREUR_INIT;
            return;
        }

        scene->determine_cube_arriere_plan(cube_arriere_plan);



        //======== Préparation des animations:
        //initialise_t0();
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
    ~Scene_Tunnel()
    {
        cout<<"Destruction de la scene Tunnel"<<endl;
        uint32_t i;

        if (scene!=NULL) delete scene;
        if (oeil_1!=NULL) delete oeil_1;
        //if (volumes_convertis!=NULL) delete volumes_convertis;
        if (transition_caducee_particules!=NULL) delete transition_caducee_particules;
        if (trajectoire_caducee_lwo2!=NULL) delete trajectoire_caducee_lwo2;
        if (trajectoire_caducee_convertie!=NULL) delete trajectoire_caducee_convertie;
        //if (sections_tunnel_lwo2!=NULL) delete sections_tunnel_lwo2;
        if (conteneur_tunnel!=NULL) delete conteneur_tunnel;
        if (source_1!=NULL) delete source_1;
        if (source_2!=NULL) delete source_2;
        if (conteneur_texte_1!=NULL) delete conteneur_texte_1;
        if (conteneur_texte_2!=NULL) delete conteneur_texte_2;
        if (conteneur_texte_3!=NULL) delete conteneur_texte_3;
        if (texte_GP!=NULL) delete texte_GP;
        if (texte_Presents!=NULL) delete texte_Presents;
        if (texte_Passages!=NULL) delete texte_Passages;
        if (mon_ver!=NULL) delete mon_ver;
        if (cube_arriere_plan!=NULL) delete cube_arriere_plan;
        for (i=0;i<sizeof(interpolations_parametres)/sizeof(Interpolation_Parametre*);i++) if (interpolations_parametres[i]!=NULL) delete interpolations_parametres[i];
        for (i=0;i<sizeof(interpolations_reperes)/sizeof(Interpolation_Repere*);i++) if (interpolations_reperes[i]!=NULL) delete interpolations_reperes[i];
        for (i=0;i<sizeof(textures_arriere_plan)/sizeof(Image*);i++) if (textures_arriere_plan[i]!=NULL) delete textures_arriere_plan[i];
        for (i=0;i<sizeof(textures_arriere_plan_transition)/sizeof(Image*);i++) if (textures_arriere_plan_transition[i]!=NULL) delete textures_arriere_plan_transition[i];
        if (image_transition_arriere_plan!=NULL) delete image_transition_arriere_plan;
        //if (le_tunnel!=NULL) delete le_tunnel;
        if (image_dos_ver!=NULL) delete image_dos_ver;
        if (image_ventre_ver!=NULL) delete image_ventre_ver;
        if (image_tete_ver!=NULL) delete image_tete_ver;
        if (image_queue_ver!=NULL) delete image_queue_ver;
        if(logo2_GP!=NULL)delete logo2_GP;
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

        le_tunnel->maj(position_temps_scene);
        scene->maj(position_temps_scene);

        post_rendu->affiche_flou_256(oeil_1);

        if (!etapes[GENERIQUE+1])affiche_texte();
        //affiche_texte();
    }

    //-----------------------------------------------------
    //  Affiche les textes par-dessus le rendu des auras
    //-----------------------------------------------------
    void affiche_texte()
    {
        //glClear(GL_DEPTH_BUFFER_BIT);
        //texte_GP->affiche(oeil_1);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glPushMatrix();
        logo2_GP->maj(position_temps);
        logo2_GP->affiche(oeil_1);
        glPopMatrix();

        texte_Presents->affiche(oeil_1);
        texte_Passages->affiche(oeil_1);
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
        uint32_t i;
        scene->determine_oeil_courant(oeil_1);
        scene->determine_couleur_brouillard(0x0005444c);
        //scene->determine_couleur_brouillard(0xff022226);
        scene->determine_limites_brouillard(10.,600.);

        le_tunnel->determine_intervalles_perturbations(6.,6.,6.);
        le_tunnel->vitesse_vecteur_generateur=30.;
        le_tunnel->determine_mode_deplacement_vecteur_generateur(TUNNEL_MODE_PERLIN);

        position_repere_texte_1=interpolations_parametres[POS_GPLOGO]->valeur_depart;
        position_repere_texte_2=interpolations_parametres[POS_PRESENTS]->valeur_depart;
        position_repere_texte_3=interpolations_parametres[POS_PASSAGES]->valeur_depart;
        position_conteneur_tunnel=0.5;
        position_ver[0]=-1.;
        position_ver[1]=.5;
        position_ver[2]=-30.;
        mon_ver->determine_position(position_ver[0],position_ver[1],position_ver[2]);
        mon_ver->rotation(-90./180.*M_PI,0.,0.,1.);

        texte_GP->rotation(M_PI,0.,1.,0.);
        texte_Presents->rotation(M_PI,0.,1.,0.);
        texte_Passages->rotation(M_PI,0.,1.,0.);

        oeil_courant=oeil_1;
        oeil_1->determine_position(2.,-2.,-25.);
        oeil_2->determine_position(2.,-2.,9.);
        oeil_2->determine_conteneur_vise(mon_ver);
        oeil_2->determine_mode_visee(DKSOEIL_VISEE_CONTENEUR);
        oeil_2->desactive_visee_inertielle();

        le_tunnel->positionne_conteneur(conteneur_tunnel,position_conteneur_tunnel);
        le_tunnel->positionne_conteneur(conteneur_texte_1,position_repere_texte_1);
        le_tunnel->positionne_conteneur(conteneur_texte_2,position_repere_texte_2);
        le_tunnel->positionne_conteneur(conteneur_texte_3,position_repere_texte_3);

        cube_arriere_plan->determine_niveau_transition_textures_couleur(0.);

        transition_caducee_particules->drapeau_affiche=false;
        transition_caducee_particules->random_deplacements_faces(0.,0.,100.,200.,0.,0.);
        float pos_fin[3]={0.,0.,0.};

        for(i=0;i<transition_caducee_particules->renvoie_nombre_de_points();i++)
        {
            Particule_Face_Separee* particule=(Particule_Face_Separee*)transition_caducee_particules->renvoie_particule(i);
            particule->interpolation_deplacement->determine_parametres(-1.,particule->deplacement_centre_gravite,pos_fin,float(rand())/32768.*1.+1.,INTERPOLATION_COSINUS);
            particule->interpolation_niveau_eclairage->determine_parametres(-1.,2.,0.,.5,INTERPOLATION_COSINUS);
        }
        transition_caducee_corps_2->drapeau_affiche=false;

        post_rendu->alpha_tempon_ecran=0.;
        post_rendu->alpha_tempon_256=1.;

        texte_GP->drapeau_affiche=true;
        texte_Presents->drapeau_affiche=true;
        texte_Passages->drapeau_affiche=true;

        drapeau_depart_faces=false;
        drapeau_vise_faces=false;
        le_tunnel->drapeau_affiche=true;
       // scene->t0=t0;
    }


    //----------------------------------------------------------------------------
    //                Initialise les animations
    //----------------------------------------------------------------------------
    private:
    void initialise_animations()
    {
        interpolations_parametres[POS_GPLOGO]=new Interpolation_Parametre(-1.,0.48,0.9,10.,INTERPOLATION_LINEAIRE);
        interpolations_parametres[POS_PRESENTS]=new Interpolation_Parametre(2.,0.45,0.9,10.,INTERPOLATION_LINEAIRE);
        interpolations_parametres[POS_PASSAGES]=new Interpolation_Parametre(-1.,0.45,0.9,15.,INTERPOLATION_LINEAIRE);
        Courbe_Bezier_Plan* courbe_ver=new Courbe_Bezier_Plan(0.,0.,0.5,.5,0.5,1.,1.,1.);
        interpolations_parametres[POS_VER]=new Interpolation_Parametre(-1.,-30.,0.,5.,INTERPOLATION_BEZIER,courbe_ver);
        interpolations_parametres[POSX_OEIL_2]=new Interpolation_Parametre(-1.,2.,-150.,5.,INTERPOLATION_COSINUS);
        interpolations_parametres[POSZ_OEIL_2]=new Interpolation_Parametre(-1.,9.,-100.,5.,INTERPOLATION_COSINUS);
        interpolations_parametres[POSITION_ANIMATION_FACES_CADUCEE]=new Interpolation_Parametre(-1.,0,1.,25.,INTERPOLATION_LINEAIRE);
        interpolations_parametres[ALPHA_SCENE]=new Interpolation_Parametre(-1.,0,1.,.5,INTERPOLATION_LINEAIRE);
        interpolations_parametres[ALPHA_AURAS]=new Interpolation_Parametre(-1.,0,1.,.5,INTERPOLATION_LINEAIRE);
        interpolations_parametres[VITESSE_TUNNEL]=new Interpolation_Parametre(-1.,0,1.,1.,INTERPOLATION_LINEAIRE);
        interpolations_reperes[REPERE_CUBE_ARRIERE_PLAN]=new Interpolation_Repere(-1.,cube_arriere_plan,transition_caducee_particules,10.,INTERPOLATION_COSINUS);
        interpolations_parametres[CAMERA_POS_TRAJECTOIRE_01]=new Interpolation_Parametre(-1.,0,1.,29.,INTERPOLATION_LINEAIRE);
        interpolations_parametres[TRANSITION_TEXTURES_ARRIERE_PLAN]=new Interpolation_Parametre(60.,0,1.,5.,INTERPOLATION_LINEAIRE);
        //interpolations_parametres[TRANSITION_TEXTURES_ARRIERE_PLAN]=new Interpolation_Parametre(2.,0,1.,5.,INTERPOLATION_LINEAIRE);
    }

    void initialise_listes_affichage()
    {
        post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
        post_rendu->ajoute_rendu(scene,DKSECRAN_TEMPON_256,DKSRENDU_COULEURS_LUMINESCENTES);

        post_rendu->ajoute_rendu(le_tunnel,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
        post_rendu->ajoute_rendu(le_tunnel,DKSECRAN_TEMPON_256,DKSRENDU_COULEURS_LUMINESCENTES);

        //post_rendu->ajoute_rendu(logo2_GP,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
        post_rendu->ajoute_rendu(texte_Presents,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);
        post_rendu->ajoute_rendu(texte_Passages,DKSECRAN_TEMPON_ECRAN_1,DKSRENDU_COULEURS);

    }

    //Pour le branchement avec la scène suivante:
    public:
    Interpolation_Parametre* renvoie_interpolation_camera_pos_trajectoire_01()
    {
        return interpolations_parametres[CAMERA_POS_TRAJECTOIRE_01];
    }

    //======================================================
    //               Attente du retour du faisceau
    //              Déclenchements des animations
    //=======================================================
    public:
    virtual void attente_retour_faisceau()
    {
        if (!drapeau_initialisee || drapeau_scene_terminee) return;

        //Générique:
        //Départ scène à 17.8

        if (position_temps<23.5)
        {
            if (!etapes[GENERIQUE])
            {
                le_tunnel->vitesse_vecteur_generateur=60;
                etapes[GENERIQUE]=true;
                //post_rendu->alpha_tempon_ecran=1.;
            }

            position_repere_texte_1=interpolations_parametres[POS_GPLOGO]->renvoie_valeur_parametre(position_temps_scene);
            position_repere_texte_2=interpolations_parametres[POS_PRESENTS]->renvoie_valeur_parametre(position_temps_scene);
            //mon_ver->determine_position(position_ver[0],position_ver[1],position_ver[2]);
            le_tunnel->positionne_conteneur(conteneur_tunnel,position_conteneur_tunnel);
            le_tunnel->positionne_conteneur(conteneur_texte_1,position_repere_texte_1);
            le_tunnel->positionne_conteneur(conteneur_texte_2,position_repere_texte_2);
            le_tunnel->positionne_conteneur(conteneur_texte_3,position_repere_texte_3);
        }

        //Arrivé du ver:
        else if (position_temps<45.5)
        {
            if (!etapes[VER])
            {
                interpolations_parametres[VITESSE_TUNNEL]->determine_parametres(-1.,le_tunnel->vitesse_vecteur_generateur,200.,1.,INTERPOLATION_COSINUS);
                //le_tunnel->determine_intervalles_perturbations(4.,4.,4.);

                etapes[VER]=true;
            }

            le_tunnel->vitesse_vecteur_generateur=interpolations_parametres[VITESSE_TUNNEL]->renvoie_valeur_parametre(position_temps_scene);
            post_rendu->alpha_tempon_ecran=interpolations_parametres[ALPHA_SCENE]->renvoie_valeur_parametre(position_temps_scene);
            le_tunnel->positionne_conteneur(conteneur_tunnel,position_conteneur_tunnel);
            if(position_temps>32.5)
            {
                position_ver[2]=interpolations_parametres[POS_VER]->renvoie_valeur_parametre(position_temps_scene);
                mon_ver->determine_position(position_ver[0],position_ver[1],position_ver[2]);
            }
            if(position_temps_scene<17.5)
            {
                position_repere_texte_1=interpolations_parametres[POS_GPLOGO]->renvoie_valeur_parametre(position_temps_scene);
                position_repere_texte_2=interpolations_parametres[POS_PRESENTS]->renvoie_valeur_parametre(position_temps_scene);
                position_repere_texte_3=interpolations_parametres[POS_PASSAGES]->renvoie_valeur_parametre(position_temps_scene);
                le_tunnel->positionne_conteneur(conteneur_texte_1,position_repere_texte_1);
                le_tunnel->positionne_conteneur(conteneur_texte_2,position_repere_texte_2);
                le_tunnel->positionne_conteneur(conteneur_texte_3,position_repere_texte_3);
            }
            else
            {
                texte_GP->drapeau_affiche=false;
                texte_Presents->drapeau_affiche=false;
                texte_Passages->drapeau_affiche=false;
            }


        }

        //Vue de face du ver:
        else if (position_temps_scene<40)
        {
            if (!etapes[FACE_VER])
            {
                scene->determine_oeil_courant(oeil_2);
                oeil_courant=oeil_2;
                position_ver[0]=0.;
                position_ver[1]=0.;
                etapes[FACE_VER]=true;
            }
            mon_ver->determine_position(position_ver[0],position_ver[1],position_ver[2]);
            le_tunnel->positionne_conteneur(conteneur_tunnel,position_conteneur_tunnel);

        }



        //Transition avec le caducée - Plan 1:

        else if (position_temps_scene<54) //14
        {
            if (!etapes[TRANSITION_VUE1])
            {
                    /// Asupprimer après le developpement
                    /*
                    scene->determine_oeil_courant(oeil_2);
                    oeil_courant=oeil_2;

                    post_rendu->alpha_couleurs=1.;
                    post_rendu->alpha_auras=1.;
                    etapes[GENERIQUE+1]=true;
                    position_ver=0.;        /// A supprimer après le developpement
                    */


                interpolations_parametres[VITESSE_TUNNEL]->determine_parametres(-1.,200.,100.,4.,INTERPOLATION_COSINUS);
                cube_arriere_plan->a_ete_deplace();
                le_tunnel->determine_mode_deplacement_vecteur_generateur(TUNNEL_MODE_TRAJECTOIRE);
                float pos[3];
                trajectoire_caducee.renvoie_point(0,pos);
                trajectoire_camera_caducee_01.rotation_trajectoire(le_tunnel->angle_alignement_trajectoire,le_tunnel->axe_rotation_alignement_trajectoire);
                trajectoire_camera_caducee_01.positionne_trajectoire(pos[0],pos[1],pos[2]);
                transition_caducee_particules->determine_position(double(pos[0]),double(pos[1]),double(pos[2]));
                transition_caducee_particules->rotationf(le_tunnel->angle_alignement_trajectoire,le_tunnel->axe_rotation_alignement_trajectoire);
                transition_caducee_particules->drapeau_affiche=true;
                transition_caducee_corps_2->determine_position(double(pos[0]),double(pos[1]),double(pos[2]));
                transition_caducee_corps_2->rotationf(le_tunnel->angle_alignement_trajectoire,le_tunnel->axe_rotation_alignement_trajectoire);
                //transition_caducee_corps_2->drapeau_affiche=true;

                interpolations_reperes[REPERE_CUBE_ARRIERE_PLAN]->determine_parametres(-1.,cube_arriere_plan,transition_caducee_particules,10.,INTERPOLATION_COSINUS);

                etapes[TRANSITION_VUE1]=true;

            }
            interpolations_reperes[REPERE_CUBE_ARRIERE_PLAN]->renvoie_orientation_repere(position_temps_scene,cube_arriere_plan);
            cube_arriere_plan->a_ete_deplace();
            oeil_2->determine_position(interpolations_parametres[POSX_OEIL_2]->renvoie_valeur_parametre(position_temps_scene),oeil_2->y,interpolations_parametres[POSZ_OEIL_2]->renvoie_valeur_parametre(position_temps_scene));
            if (mon_ver->drapeau_affiche) mon_ver->determine_position(position_ver[0],position_ver[1],position_ver[2]);
            le_tunnel->vitesse_vecteur_generateur=interpolations_parametres[VITESSE_TUNNEL]->renvoie_valeur_parametre(position_temps_scene);
            le_tunnel->positionne_conteneur(conteneur_tunnel,position_conteneur_tunnel);

            //Calcul la position du ver dans la trajectoire du caducée:
            float rapport_longueur_trajectoire_longeur_tunnel=le_tunnel->trajectoire_vecteur_generateur->renvoie_longueur_totale()/(float(le_tunnel->nbrSections-2)*le_tunnel->distance_sections);
            float position_ver_caducee=le_tunnel->position_vecteur_sur_trajectoire-(1.-position_conteneur_tunnel)/rapport_longueur_trajectoire_longeur_tunnel;
            //Attend que le ver entre dans le caducée:

            if (position_ver_caducee>0.)
            {
                //Le ver est entré, animation des faces:
                position_depart_faces=interpolations_parametres[POSITION_ANIMATION_FACES_CADUCEE]->renvoie_valeur_parametre(position_temps_scene);
                //Le point visé change dès que le mouvement des faces dépasse le ver:
                float position_faces_visee=position_depart_faces-0.05;
                if (position_faces_visee>position_ver_caducee)
                {
                    if (!drapeau_vise_faces)
                    {
                        oeil_2->determine_mode_visee(DKSOEIL_VISEE_POINT);
                        oeil_2->active_visee_inertielle();
                        oeil_2->determine_inertie_visee(2.);
                        interpolations_parametres[POSX_OEIL_2]->determine_parametres(-1.,oeil_2->x,-500.,20.,INTERPOLATION_COSINUS);
                        //interpolations_parametres[POSZ_OEIL_2]->determine_parametres(-1.,oeil_2->z,-500.,30.,INTERPOLATION_COSINUS);
                        drapeau_vise_faces=true;
                        mon_ver->drapeau_affiche=false;
                    }
                    trajectoire_caducee.renvoie_position(position_faces_visee,point_vise);
                    //oeil_2->determine_position(interpolations_parametres[POSX_OEIL_2]->renvoie_valeur_parametre(position_temps_scene),oeil_2->y,oeil_2->z);
                    oeil_2->determine_point_vise(double(point_vise[0]),double(point_vise[1]),double(point_vise[2]));

                    if (position_depart_faces>0.25) le_tunnel->drapeau_affiche=false;   //Effacement du tunnel
                }


                //Les faces s'animent:
                index_depart_faces_animees=(transition_caducee_particules->nbrFaces-1)-position_depart_faces*(transition_caducee_particules->nbrFaces-1);
                transition_caducee_particules->interpolation_positions_clignottement_fourchette_faces(position_temps_scene,index_depart_faces_animees,transition_caducee_particules->nbrFaces-1);
            }
        }

        // Transition avec le caducée - Plan 2
        else if (position_temps_scene<68.)    //28
        {
            if (!etapes[TRANSITION_VUE2])
            {
                oeil_2->determine_position(0.,0.,0.);
                conteneur_tunnel->raz();
                oeil_2->desactive_visee_inertielle();
                oeil_2->active_initialisation_visee_inertielle();
                oeil_2->determine_inertie_visee(4.);
                oeil_2->determine_mode_visee(DKSOEIL_VISEE_POINT);
                etapes[TRANSITION_VUE2]=true;
            }

            position_depart_faces=interpolations_parametres[POSITION_ANIMATION_FACES_CADUCEE]->renvoie_valeur_parametre(position_temps_scene);
            //Position de la caméra sur la trajectoire:
            float position_trajectoire_camera=interpolations_parametres[CAMERA_POS_TRAJECTOIRE_01]->renvoie_valeur_parametre(position_temps);
            //cout<<"T:"<<position_temps<<"position scene tunnel: "<<position_trajectoire_camera<<endl;
            trajectoire_camera_caducee_01.renvoie_position(position_trajectoire_camera,position);
            conteneur_tunnel->determine_position(double(position[0]),double(position[1]),double(position[2]));

            //Position du point de visée:
            float position_point_visee=position_depart_faces*0.5+0.25;
            trajectoire_caducee.renvoie_position(position_point_visee,position);
            oeil_2->determine_point_vise(double(position[0]),double(position[1]),double(position[2]));
            //Les faces s'animent:
            index_depart_faces_animees=(transition_caducee_particules->nbrFaces-1)-position_depart_faces*(transition_caducee_particules->nbrFaces-1);
            transition_caducee_particules->interpolation_positions_clignottement_fourchette_faces(position_temps_scene,index_depart_faces_animees,transition_caducee_particules->nbrFaces-1);


            float niveau_transition=interpolations_parametres[TRANSITION_TEXTURES_ARRIERE_PLAN]->renvoie_valeur_parametre(position_temps_scene);
            cube_arriere_plan->determine_niveau_transition_textures_couleur(niveau_transition);

        }

        else
        {
            drapeau_scene_terminee=true;
        }
    }

    //============================================================================
    //                     Initialisations de l'éditeur
    //============================================================================

    virtual void initialise_editeur()
    {
        pas_deplacement=0.1;
        pas_rotation=0.05;
        oeil_1->masse=10.;
        oeil_1->determine_coefficient_frottement(1.,100.);
        mode_deplacement_tunnel=TUNNEL_MODE_PERLIN;
    }

        //===================================================
        //          Affiche la trajectoire éditée
        //===================================================
        void affiche_trajectoire()
        {

            //glLoadIdentity();
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
            //oeil_1->positionne_scene(t);
            trajectoire_camera_caducee_01.affiche_courbe(20,0x40ffff00);
            trajectoire_camera_caducee_01.affiche_points(0x80ff0000,5);
            //trajectoire_camera_caducee_01.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            //glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }

    //=================================================
    //                     Gestion de la souris
    //=================================================
    virtual void mouvement_souris(int x,int y)
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
                oeil_1->deplace(AVANT,pas_deplacement);
                break;
            case 100:
                //flèche gauche
                oeil_1->deplace(GAUCHE,pas_deplacement);
                break;
            case 103:
                //flèche bas
                oeil_1->deplace(ARRIERE,pas_deplacement);
                break;
            case 102:
                //flèche droite
                oeil_1->deplace(DROITE,pas_deplacement);
                break;

            //Touches haut et bas
            case 104:
                oeil_1->deplace(HAUT,pas_deplacement);
                break;

            case 105:
                oeil_1->deplace(BAS,pas_deplacement);
                break;

            //Touches F..
            case 1:
                scene->determine_oeil_courant(oeil_1);
                break;

            case 2:
                scene->determine_oeil_courant(oeil_2);
            default:
                break;

        }


    }

    virtual void touche_speciale_relachee(int key, int x, int y)
    {

        oeil_1->stop_propulsion();
        oeil_1->stop_propulsion_angulaire();
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

    }

    virtual void touche_appuyee(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case 27 :
                //fin_du_programme();
                exit(0);
                break;

            case '9':
                le_tunnel->determine_lissage_trajectoire(le_tunnel->renvoie_lissage_trajectoire()+0.1);
                cout<<le_tunnel->renvoie_lissage_trajectoire()<<endl;
                break;
            case '3':
                le_tunnel->determine_lissage_trajectoire(le_tunnel->renvoie_lissage_trajectoire()-0.1);
                cout<<le_tunnel->renvoie_lissage_trajectoire()<<endl;
                break;

            case '4':
                oeil_1->tourne(HAUT,pas_rotation);
                break;
            case '6':
                oeil_1->tourne(BAS,pas_rotation);
                break;
            case '7':
                oeil_1->tourne(GAUCHE,pas_rotation);
                break;
            case '1':
                oeil_1->tourne(DROITE,pas_rotation);
                break;
            case '0':
                oeil_1->tourne(ARRIERE,pas_rotation);
                break;
            case '.':
                oeil_1->tourne(AVANT,pas_rotation);
                break;
            case ' ':

                break;
            case '+':
                le_tunnel->vitesse_vecteur_generateur+=1.;
                cout<<le_tunnel->vitesse_vecteur_generateur<<endl;
                break;
            case '-':
                le_tunnel->vitesse_vecteur_generateur-=1.;
                cout<<le_tunnel->vitesse_vecteur_generateur<<endl;
                break;

            case 'd':
                if (mode_deplacement_tunnel==TUNNEL_MODE_PERLIN) mode_deplacement_tunnel=TUNNEL_MODE_TRAJECTOIRE;
                else mode_deplacement_tunnel=TUNNEL_MODE_PERLIN;
                le_tunnel->determine_mode_deplacement_vecteur_generateur(mode_deplacement_tunnel);
                cout<<uint16_t(mode_deplacement_tunnel)<<endl;
                break;

            case '=':

                break;
            case ')':

                break;

            default:
                break;
        }
    }


    virtual void touche_relachee(unsigned char key, int x, int y)
    {
        oeil_1->stop_propulsion();
        oeil_1->stop_propulsion_angulaire();
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


    }

};

#endif // SCENE_TUNNEL_H_INCLUDED
