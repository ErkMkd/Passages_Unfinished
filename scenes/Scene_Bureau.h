#ifndef SCENE_BUREAU_H_INCLUDED
#define SCENE_BUREAU_H_INCLUDED

#define SCENE_BUREAU_OK 0
#define SCENE_BUREAU_ERREUR_INIT 1

#include "Scene.h"

class Scene_Bureau : public Scene
{

    public:
        dksChargeLWO2* volumes_lwo2;
        dksChargeLWO2* volumes_test_lwo2;
        dksConvertionLWO2* volumes_convertis;
        dksConvertionLWO2* volumes_convertis_test;

        dksSourceLumineuse* source_1;
        dksSourceLumineuse* source_2;
        dksOeil* oeil_1;

        dksMoteur3d* scene;
        dksVolume* volumes[10];
        dksSourceLumineuse* sources[10];
        dksOeil* yeux[10];

        dksVolume* lampe_neon;
        dksVolume* piece;
        dksVolume* fauteuil;
        dksVolume* table;
        dksVolume* verre;
        dksVolume* sphere;
        dksVolume* logo;

        uint32_t teinte_principale;
        float ouverture_camera; //L'ouverture en °

        //Données pour le rendu des auras:
        GLuint texture_scene_id;
        GLuint texture_filtre_id;
        uint32_t taille_texture_aura;

        //============= Données de l'éditeur:

        char* fichier_trajectoire;
        dksTrajectoire ma_trajectoire;
        uint32_t point_actuel;
        float position_trajectoire;
        float coordonnees_trajectoire[3];
        float pas_position_trajectoire;

        uint32_t numero_face;

        vector<dksConteneur*> volumes_selectionnables;
        dksConteneur* volume_selectionne;
        double vitesse_rotation;
        double vitesse_deplacement;

        //=================================================
        //              Constructeur
        //=================================================
        Scene_Bureau(char* p_nom,dksEcran* p_ecran):Scene(p_nom,p_ecran)
        {
            volumes_test_lwo2=NULL;
            volumes_lwo2=NULL;
            volumes_convertis=NULL;
            volumes_convertis_test=NULL;
            source_1=NULL;
            source_2=NULL;
            oeil_1=NULL;
            scene=NULL;
            //volumes={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            //sources={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            //yeux={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            ouverture_camera=60.;
            teinte_principale=0xff80eeff;
            taille_texture_aura=512;

            volumes_lwo2=new dksChargeLWO2("LWO2/bureau.lwo","textures/bureau/");
            if (volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                erreur=SCENE_BUREAU_ERREUR_INIT;
                return;
            }

            volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
            if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
            {
                erreur=SCENE_BUREAU_ERREUR_INIT;
                return;
            }

            volumes_test_lwo2=new dksChargeLWO2("LWO2/intense_logo.lwo","textures/intense_logo/");
            if (volumes_test_lwo2==NULL || volumes_test_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                erreur=SCENE_BUREAU_ERREUR_INIT;
                return;
            }

            volumes_convertis_test=new dksConvertionLWO2(volumes_test_lwo2);
            if (volumes_convertis_test==NULL || volumes_convertis_test->erreur!=DKSCONVERTIONLWO2_OK)
            {
                erreur=SCENE_BUREAU_ERREUR_INIT;
                return;
            }

            //----------- Assignations:
            volumes[0]=volumes_convertis->renvoie_volume_index(0);
            volumes[1]=volumes_convertis->renvoie_volume_index(1);
            volumes[2]=volumes_convertis->renvoie_volume_index(2);
            volumes[3]=volumes_convertis->renvoie_volume_index(3);
            volumes[4]=volumes_convertis->renvoie_volume_index(4);
            volumes[5]=volumes_convertis_test->renvoie_volume_index(0);
            volumes[6]=volumes_convertis_test->renvoie_volume_index(1);
            piece=volumes_convertis->renvoie_volume_index(0);
            table=volumes_convertis->renvoie_volume_index(1);
            fauteuil=volumes_convertis->renvoie_volume_index(2);
            lampe_neon=volumes_convertis->renvoie_volume_index(3);
            verre=volumes_convertis->renvoie_volume_index(4);
            logo=volumes_convertis_test->renvoie_volume_index(0);
            sphere=volumes_convertis_test->renvoie_volume_index(1);
            sphere->determine_rendu_faces_transparentes(true);
            logo->determine_rendu_faces_transparentes(true);

            // Sources lumineuses:
            source_1=new dksSourceLumineuse("Source_1",0.,0.,0.);
            source_2=new dksSourceLumineuse("Source_2",0.,0.,0.);

            //zyeux:
            oeil_1=new dksOeil("Oeil_1");
            oeil_1->determine_mode_visee(DKSOEIL_VISEE_CONTENEUR);
            //oeil_1->determine_conteneur_vise(verre);
            oeil_1->determine_contrainte_verticale(false);
            oeil_1->masse=40.;
            oeil_1->determine_coefficient_frottement(.1,20.);
            oeil_1->determine_limites_profondeur(0.1,100.);
            oeil_1->determine_ouverture(ouverture_camera);


            //volumes[5]=&mon_cube;

            sources[0]=source_1;
            sources[1]=source_2;
            yeux[0]=oeil_1;

            //Initialise la scene:
            scene=new dksMoteur3d("Scene_Bureau",ecran,NULL,0,volumes,7,yeux,1,sources,2,oeil_1);
            if (scene==NULL || scene->erreur!=DKSM3D_OK)
            {
                erreur=SCENE_BUREAU_ERREUR_INIT;
                return;
            }

            scene->determine_couleur_fond(0.,0.,0.,0.);
            //-------- Initialise l'éclairage:
            source_1->determine_couleur_diffusion(0xffffffff);
            //source_1->determine_couleur_ambiance(0xff80eeff);
            source_1->determine_couleur_ambiance(0xff000000);
            source_2->determine_couleur_diffusion(0xffe0e0ff);
            source_2->determine_couleur_ambiance(0xff000000);
            source_1->determine_attenuation(1.25,.1,0.);
            source_2->determine_attenuation(1.25,.1,0.);
            source_2->determine_position(0.,-10.,0.);


            dksSurface* surface;
            //------- Initialise les murs du bureau:

            piece->applique_teinte_diffusion(teinte_principale);
            piece->determine_couleur_ambiance(source_1->renvoie_couleur_ambiance_ARVB(),0.);
            surface=piece->renvoie_surface_nom("plafond");
            //surface->determine_ambiance(.2);

            //------- Initialise la table

            table->applique_teinte_diffusion(teinte_principale);
            table->determine_couleur_ambiance(source_1->renvoie_couleur_ambiance_ARVB(),0.25);

            //------- Initialise le fauteuil:

            fauteuil->applique_teinte_diffusion(teinte_principale);
            fauteuil->determine_couleur_ambiance(source_1->renvoie_couleur_ambiance_ARVB(),.25);

            //------- Initialise la lampe à néon:

            lampe_neon->applique_teinte_diffusion(teinte_principale);
            lampe_neon->applique_teinte_luminescence(teinte_principale);
            lampe_neon->determine_couleur_ambiance(source_1->renvoie_couleur_ambiance_ARVB(),0.);
            lampe_neon->ajoute_enfant(source_1);
            surface=lampe_neon->renvoie_surface_nom("neon");
            surface->determine_couleur_luminescence(0xffffffff,1.);
            surface->determine_couleur_diffusion(0xffffffff,1.);
            //cout<<"R: "<<surface->couleur_luminescence[0]<<" V: "<<surface->couleur_luminescence[1]<<" B: "<<surface->couleur_luminescence[2]<<endl;

            //------- Initialise le verre:

            verre->determine_couleur_ambiance(source_1->renvoie_couleur_ambiance_ARVB(),0.25);
            verre->determine_couleur_specularite(0xffffffff,1.);

            //-------- Initialise les objets de test:
            logo->determine_position(0.,2.,2.);
            sphere->determine_position(.5,1.,2.);
            sphere->masse=10;
            logo->masse=10.;
            sphere->determine_coefficient_frottement(1.,100.);
            logo->determine_coefficient_frottement(1.,100.);

            //-------- Modes de rendus des objets:
            sphere->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);
            logo->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);

            initialise_auras();

            //scene->active();

            initialise_editeur();

        }

        //==========================================================
        //      Destructeur
        //==========================================================
        ~Scene_Bureau()
        {
            cout<<"Destruction de la scene Bureau"<<endl;

            if (scene!=NULL) delete scene;
            if ( oeil_1!=NULL) delete oeil_1;
            if (volumes_convertis!=NULL) delete volumes_convertis;
            if (volumes_convertis_test!=NULL) delete volumes_convertis_test;
            if (volumes_lwo2!=NULL) delete volumes_lwo2;
            if (volumes_test_lwo2!=NULL) delete volumes_test_lwo2;
            if (source_1!=NULL) delete source_1;
            if (source_2!=NULL) delete source_2;
            if (texture_filtre_id!=0) glDeleteTextures(1,&texture_filtre_id);
            if (texture_scene_id!=0) glDeleteTextures(1,&texture_scene_id);
        }

        //==========================================================
        //      Initialisations
        //==========================================================
        void initialise_auras()
        {
            //------------TEXTURE DE FILTRE:
            glGenTextures(1,&texture_filtre_id);
            glBindTexture(GL_TEXTURE_2D,texture_filtre_id);
            //Paramètres d'affichage de la texture:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    //Il semble que ce paramètre active la texture.....
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,taille_texture_aura,taille_texture_aura,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

            //------------TEXTURE DE RENDU DE SCENE:
            glGenTextures(1,&texture_scene_id);
            glBindTexture(GL_TEXTURE_2D,texture_scene_id);
            //Paramètres d'affichage de la texture:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //Il semble que ce paramètre active la texture.....
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,ecran->resolution_x,ecran->resolution_y,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        }

        //==========================================================
        //      Fonctions
        //==========================================================
        virtual void active(float p_t0=0.)
        {
            this->Scene::active(p_t0);
            scene->active(p_t0);
            oeil_courant=oeil_1;
        }

        virtual void affiche(double t)
        {
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene->maj(t);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            //scene->affiche();

            //scene->affiche_aura_surfaces_luminescentes(texture_filtre_id,ecran->resolution_x,ecran->resolution_y);
            scene->affiche(oeil_courant);
            affiche_trajectoire();
        }

        virtual void maj_dimensions_fenetre()
        {
            scene->maj_resolutions_yeux();
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
            position_trajectoire=0.;
            pas_position_trajectoire=0.005;
            fichier_trajectoire={"trajectoires/camera_01.traj"};
            volume_selectionne=NULL;
            vitesse_deplacement=0.1;
            vitesse_rotation=0.1;

            numero_face=0;

            volumes_selectionnables.push_back(oeil_1);
            volumes_selectionnables.push_back(table);
            volumes_selectionnables.push_back(fauteuil);
            volumes_selectionnables.push_back(verre);
            volumes_selectionnables.push_back(lampe_neon);
            volumes_selectionnables.push_back(source_1);
            volumes_selectionnables.push_back(source_2);
            volumes_selectionnables.push_back(logo);
            volumes_selectionnables.push_back(sphere);
            volume_selectionne=oeil_1;

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
            //oeil_1->positionne_scene(t);
            ma_trajectoire.affiche_courbe(20,0x40ffff00);
            ma_trajectoire.affiche_points(0x80ff0000,5);
            ma_trajectoire.affiche_point(point_actuel,0xffff6060,8);
            glEnable(GL_LIGHTING);
            glDisable(GL_BLEND);
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
                    volume_selectionne->deplace(AVANT,vitesse_deplacement);
                    volume_selectionne->affiche_position();
                    break;
                case 100:
                    //flèche gauche
                    volume_selectionne->deplace(GAUCHE,vitesse_deplacement);
                    volume_selectionne->affiche_position();
                    break;
                case 103:
                    //flèche bas
                    volume_selectionne->deplace(ARRIERE,vitesse_deplacement);
                    volume_selectionne->affiche_position();
                    break;
                case 102:
                    //flèche droite
                    volume_selectionne->deplace(DROITE,vitesse_deplacement);
                    volume_selectionne->affiche_position();
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
            volume_selectionne->stop_propulsion();
            volume_selectionne->stop_propulsion_angulaire();
            glutPostRedisplay();
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
                    volume_selectionne->deplace(HAUT,vitesse_deplacement);
                    volume_selectionne->affiche_position();
                    break;
                case '3':
                    volume_selectionne->deplace(BAS,vitesse_deplacement);
                    volume_selectionne->affiche_position();
                    break;

                case '4':
                    volume_selectionne->tourne(BAS,vitesse_rotation);
                    break;
                case '6':
                     volume_selectionne->tourne(HAUT,vitesse_rotation);
                    break;
                case '7':
                   volume_selectionne->tourne(GAUCHE,vitesse_rotation);
                    break;
                case '1':
                    volume_selectionne->tourne(DROITE,vitesse_rotation);
                    break;
                case '0':
                     volume_selectionne->tourne(ARRIERE,vitesse_rotation);
                    break;
                case '.':
                    volume_selectionne->tourne(AVANT,vitesse_rotation);
                    break;
                case ' ':
                    for (uint32_t i=0;i<volumes_selectionnables.size();i++)
                    {
                        if (volumes_selectionnables[i]==volume_selectionne)
                        {
                            if (i==volumes_selectionnables.size()-1) volume_selectionne=volumes_selectionnables[0];
                            else volume_selectionne=volumes_selectionnables[i+1];
                            cout<<"Volume sélectionné: "<<volume_selectionne->nom<<endl;
                            break;
                        }
                    }
                    break;
                case '+':
                    vitesse_deplacement+=0.0025; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '-':
                    vitesse_deplacement-=0.0025; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
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
                //------ Debuguage:
                case 'p':
                    numero_face++;
                    break;
                case 'o':
                    numero_face--;
                    break;


                //---------- Edition des trajectoire:
                case 13:
                    ma_trajectoire.ajoute_point(volume_selectionne->x,volume_selectionne->y,volume_selectionne->z);
                    point_actuel=ma_trajectoire.renvoie_nombre_points()-1;
                    break;
                case 8:
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
                    volume_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
                    break;
                case 233:
                    position_trajectoire+=pas_position_trajectoire;
                    cout<<"Position trajectoire t="<<position_trajectoire;
                    ma_trajectoire.renvoie_position(position_trajectoire,coordonnees_trajectoire);
                    volume_selectionne->determine_position(coordonnees_trajectoire);
                    cout<<" Coordonnées X:"<<coordonnees_trajectoire[0]<<" Y:"<<coordonnees_trajectoire[1]<<" Z:"<<coordonnees_trajectoire[2]<<endl;
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

            volume_selectionne->stop_propulsion();
            volume_selectionne->stop_propulsion_angulaire();
            glutPostRedisplay();
        }
};

#endif // SCENE_BUREAU_H_INCLUDED
