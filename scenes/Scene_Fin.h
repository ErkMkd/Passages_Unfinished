#ifndef SCENE_FIN_H_INCLUDED
#define SCENE_FIN_H_INCLUDED

/*

            Scène du générique de fin

*/


#include "Scene.h"
#include "../dksMoteur2d/dksAfficheImage.h"
#include "../dksMoteur3d/Texte/dksTexteMap.h"

class Scene_Fin : public Scene
{
    public:
    Image* bitmap_fin;
    dksAfficheImage* image_fin;

    dksCube* mon_cube;

    dksOeil* oeil_1;

    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;

    dksMoteur3d* scene;

    dksVolume* volumes[10];
    dksOeil* yeux[10];
    dksSourceLumineuse* sources[10];
    float ouverture_camera;

    dksTextMap* texte_fin;

    //Drapeau d'initialisation des phases
    enum {
            ZOOM_ARRIERE,
            SCROLL_TEXTE
            };
    bool etapes[2];

    //Interpolations:
    enum {
            IMAGE_ZOOM
        };
    Interpolation_Parametre* interpolations_parametres_1f[3];

    enum {
            IMAGE_POS
        };
    Interpolation_Parametre_2f* interpolations_parametres_2f[3];

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
    Scene_Fin(char* p_nom,dksEcran* p_ecran,Musique* p_musique):Scene(p_nom,p_ecran,p_musique)
    {
        mon_cube=NULL;
        oeil_1=NULL;
        source_1=NULL;
        source_2=NULL;
        scene=NULL;
        bitmap_fin=NULL;
        image_fin=NULL;
        texte_fin=NULL;

        uint32_t i;

        for (i=0;i<sizeof(etapes)/sizeof(bool);i++) etapes[i]=false;
        for (i=0;i<sizeof(interpolations_parametres_1f)/sizeof(Interpolation_Parametre*);i++) interpolations_parametres_1f[i]=NULL;
        for (i=0;i<sizeof(interpolations_parametres_2f)/sizeof(Interpolation_Parametre_2f*);i++) interpolations_parametres_2f[i]=NULL;

        //------------------- Image:
        bitmap_fin=new Image("textures/pics/queen_of_spades.jpg",true);
        if(bitmap_fin->erreur!=IMAGE_OK)
        {
            cout<<"ERREUR lors de la création de l'image de fin"<<endl;
            erreur=SCENE_ERREUR_INIT;
            return ;
        }

        image_fin=new dksAfficheImage(bitmap_fin,ecran);
        if(image_fin->erreur!=DKSAFFICHEIMAGE_OK)
        {
            cout<<"ERREUR lors de la création de l'afficheur de l'image de fin"<<endl;
            erreur=SCENE_ERREUR_INIT;
            return ;
        }
        //--------------- Texte:

            texte_fin=new dksTextMap("polices/end_font.tif",ecran,"Texte_fin",0.,0.025,.075,0.5,0.1); //Base,echelle,espace_lettres,espace,espace lignes
            texte_fin->determine_position_curseur(0.5,0.5);
            texte_fin->determine_texte_fichier("end_text.txt");
            texte_fin->mode_rendu=DKSTEXTMAP_RENDU_2D;
            texte_fin->pointeur_ligne=0;
            texte_fin->nbr_lignes_max=22;
            texte_fin->vitesse_verticale=15.;

        //----------------- Création des éléments de la scène:
        //Yeux:
            oeil_1=new dksOeil("Oeil_1",0.,0.,-10.);
            oeil_courant=oeil_1;

        //Création des sources:
            source_1=new dksSourceLumineuse("Source_1",100.,100.,-10.);
            source_1->determine_attenuation(1.,0.,0.);

        //Cubes:
            mon_cube=new dksCube("Cube_test",0.,0.,0.,1.,1.,1.,0xff00ff00);

        //Assignations:
            volumes[0]=mon_cube;
            sources[0]=source_1;
            yeux[0]=oeil_1;

        //Création de la scene:
            scene=new dksMoteur3d("Fin",ecran,
                            NULL,0,
                            NULL,0,
                            yeux,1,
                            sources,1,
                            oeil_1);


        initialise_animations();
        initialise_listes_affichage();
        #if EDITEUR_ACTIF
        initialise_editeur();
        #endif
    }

    //=====================================================
    //          DESTRUCTEUR
    //=====================================================
    ~Scene_Fin()
    {
        uint32_t i;
        if (scene!=NULL) delete scene;
        if (mon_cube!=NULL) delete mon_cube;
        if (oeil_1!=NULL) delete oeil_1;
        if (source_1!=NULL) delete source_1;
        if (source_2!=NULL) delete source_2;
        if(image_fin!=NULL) delete image_fin;
        if(bitmap_fin!=NULL)delete bitmap_fin;

        for (i=0;i<sizeof(interpolations_parametres_1f)/sizeof(Interpolation_Parametre*);i++)
            if (interpolations_parametres_1f[i]!=NULL) delete interpolations_parametres_1f[i];
        for (i=0;i<sizeof(interpolations_parametres_2f)/sizeof(Interpolation_Parametre_2f*);i++)
            if (interpolations_parametres_2f[i]!=NULL) delete interpolations_parametres_2f[i];

        if(texte_fin!=NULL) delete texte_fin;
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
        image_fin->affiche(oeil_courant);
        scene->affiche(oeil_courant);
        texte_fin->affiche(oeil_courant);
        //glClear(GL_DEPTH_BUFFER_BIT);

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
        image_fin->zoom=0.02;
        image_fin->xpos=-0.0375;
        image_fin->ypos=0.2775;

        interpolations_parametres_1f[IMAGE_ZOOM]=new Interpolation_Parametre(-1.,0.02,1.,10.,INTERPOLATION_COSINUS);
        float depart_2f[2]={-0.0375,0.2775};
        float dest_2f[2]={0.,0.};
        interpolations_parametres_2f[IMAGE_POS]=new Interpolation_Parametre_2f(-1.,depart_2f,dest_2f,10.,INTERPOLATION_COSINUS);
    }

    public:
    //----------------------------------------------------------------------------
    //                  Etat de la scène au départ de l'animation
    //----------------------------------------------------------------------------
    virtual void initialise_t0(double t0)
    {
        this->Scene::initialise_t0(t0);
        oeil_1->determine_ouverture(ouverture_camera);
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
        //mon_cube->rotation(0.05,.1,1.,-0.1);
        //image_fin->angle_rotation+=0.1;

        if (position_temps_scene<10.)
            {
                if (!etapes[ZOOM_ARRIERE])
                {
                    etapes[ZOOM_ARRIERE]=true;
                }
                image_fin->zoom=interpolations_parametres_1f[IMAGE_ZOOM]->renvoie_valeur_parametre(position_temps_scene);
                float valeurs_2f[2];
                interpolations_parametres_2f[IMAGE_POS]->renvoie_valeur_parametre(position_temps_scene,valeurs_2f);
                image_fin->determine_position(valeurs_2f);


            }

        else
            {
                texte_fin->maj_scrolling_vertial();
                //drapeau_scene_terminee=true;
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

            position_trajectoire=0.;
            pas_position_trajectoire=0.005;
            fichier_trajectoire={"trajectoires/camera_fin_01.traj"};

            conteneurs_selectionnables.push_back(oeil_1);

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
            float pas_texte=0.1;
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
                #endif EDITEUR_TRAJECTOIRE

                #if !EDITEUR_TRAJECTOIRE
                case 'a':
                    image_fin->ypos+=0.0025;
                    cout<<"X="<<image_fin->xpos<<" Y="<<image_fin->ypos<<endl;
                break;
                case 'w':
                    image_fin->ypos-=0.0025;
                    cout<<"X="<<image_fin->xpos<<" Y="<<image_fin->ypos<<endl;
                break;
                case 'q':
                    image_fin->xpos-=0.0025;
                    cout<<"X="<<image_fin->xpos<<" Y="<<image_fin->ypos<<endl;
                break;
                case 's':
                    image_fin->xpos+=0.0025;
                    cout<<"X="<<image_fin->xpos<<" Y="<<image_fin->ypos<<endl;
                break;
                case 'd':
                    image_fin->zoom_x-=0.01;
                    image_fin->zoom_y-=0.01;
                    cout<<"Zoom="<<image_fin->zoom_x<<endl;
                break;
                case 'f':
                    image_fin->zoom_x+=0.01;
                    image_fin->zoom_y+=0.01;
                    cout<<"Zoom="<<image_fin->zoom_x<<endl;
                break;
                case 'g':
                    image_fin->angle_rotation-=1.;
                    cout<<"angle="<<image_fin->angle_rotation<<endl;
                break;
                case 'h':
                    image_fin->angle_rotation+=1.;
                    cout<<"angle="<<image_fin->angle_rotation<<endl;
                break;


                case 't':
                    texte_fin->curseur_x-=pas_texte;
                    cout<<"curseur:"<<texte_fin->curseur_x<<" "<<texte_fin->curseur_y<<endl;
                break;

                case 'y':
                    texte_fin->curseur_x+=pas_texte;
                    cout<<"curseur:"<<texte_fin->curseur_x<<" "<<texte_fin->curseur_y<<endl;
                break;

                case 'u':
                    texte_fin->curseur_y-=pas_texte;
                    cout<<"curseur:"<<texte_fin->curseur_x<<" "<<texte_fin->curseur_y<<endl;
                break;

                case 'i':
                    texte_fin->curseur_y+=pas_texte;
                    cout<<"curseur:"<<texte_fin->curseur_x<<" "<<texte_fin->curseur_y<<endl;
                break;

                #endif

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

#endif // SCENE_FIN_H_INCLUDED
