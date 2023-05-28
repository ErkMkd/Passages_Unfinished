#ifndef SCENE_TEST_H_INCLUDED
#define SCENE_TEST_H_INCLUDED

//==== Erreurs

#define ESSAIM_OK 0
#include "Scene.h"
#include "../dksMoteur3d/Particules/dksParticulesVolumes.h"

class Scene_Test :public Scene
{
    public:

    dksOeil* oeil_1;

    dksMoteur3d* scene;

    dksCube* mon_cube;
    dksCube* support;
    dksCube* cube_source;
    dksCube* cube_source_2;
    dksCube* cube_source_3;
    dksSourceLumineuse* source_1;
    dksSourceLumineuse* source_2;
    dksSourceLumineuse* source_3;

    dksVolume* volumes[6];
    dksOeil* yeux[1];
    dksSourceLumineuse* sources[3];

    dksParticulesVolumes* mes_particules;
    dksVolume* volumes_particules[50];

    float roulis;
    float tangage;
    float lacet;

    //Données éditeur:
    bool balance;
    double vitesse_deplacement;
    double vitesse_rotation;
    double pas_deplacement;
    double pas_rotation;
    vector<dksConteneur*> conteneurs_selectionnables;
    dksConteneur* conteneur_selectionne;



    //===========================================================
    //              Constructeur
    //===========================================================

    public:
    Scene_Test(dksEcran* p_ecran):Scene(p_ecran)
    {
        uint32_t i;
        mes_particules=NULL;
        for (i=0;i<sizeof(volumes)/sizeof(dksVolume*);i++) volumes[i]=NULL;
        for (i=0;i<sizeof(yeux)/sizeof(dksOeil*);i++) yeux[i]=NULL;
        for (i=0;i<sizeof(sources)/sizeof(dksSourceLumineuse*);i++) sources[i]=NULL;
        for (i=0;i<sizeof(volumes_particules)/sizeof(dksVolume*);i++) volumes_particules[i]=NULL;

        //Création des éléments:
        oeil_1=new dksOeil("Oeil_1",0.,2.,-10.);
        yeux[0]=oeil_1;
        //oeil_1->determine_contrainte_verticale(true);

        source_1=new dksSourceLumineuse("source_1",2.,6.,-7.);
        sources[0]=source_1;
        source_1->rotation(45./180.*M_PI,1.,0.,0.);
        source_1->determine_type(DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE);

        source_2=new dksSourceLumineuse("source_2",6.,2.,0.);
        sources[1]=source_2;
        source_2->determine_couleur_diffusion(0xff0000);

        source_3=new dksSourceLumineuse("source_3",-6.,2.,0.);
        sources[2]=source_3;
        source_3->determine_couleur_diffusion(0xff20ff20);

        cube_source=new dksCube("cube_source",0.,0.,.75,.2,.2,1.5,0xffffff);
        cube_source->determine_couleur_luminescence(0xffffff,1.);
        cube_source->determine_projete_ombres(false);

        cube_source_2=new dksCube("cube_source 2",0.,0.,.75,.2,.2,1.5,0xffffff);
        cube_source_2->determine_couleur_luminescence(source_2->renvoie_couleur_diffusion_ARVB(),1.);
        //cube_source_2->determine_projete_ombres(false);

        cube_source_3=new dksCube("cube_source 3",0.,0.,.75,.2,.2,1.5,0xffffff);
        cube_source_3->determine_couleur_luminescence(source_3->renvoie_couleur_diffusion_ARVB(),1.);
        //cube_source_3->determine_projete_ombres(false);

        mon_cube=new dksCube("mon_cube",0.,1.,0.,1.,1.,1.,0xffff00);
        support=new dksCube("support",0.,-.5,0.,5.,1.,5.,0xffffff);
        volumes[0]=mon_cube;
        volumes[1]=support;
        volumes[2]=cube_source;
        volumes[3]=cube_source_2;
        volumes[4]=cube_source_3;

        mon_cube->determine_couleur_specularite(0xffffff,1.);
        mon_cube->determine_mode_rendu(DKSVOLUME_MODE_IMMEDIAT);

        source_1->ajoute_enfant(cube_source);
        source_1->determine_ouverture(90.);

        source_2->ajoute_enfant(cube_source_2);
        source_3->ajoute_enfant(cube_source_3);

        //Création des particules:
        for (i=0;i<sizeof(volumes_particules)/sizeof(dksVolume*);i++)
        {
            volumes_particules[i]=new dksCube("cube_particule",0.,0.,0.,0.2,0.2,0.2,0xff8080);
            volumes_particules[i]->determine_mode_rendu(DKSVOLUME_MODE_LISTE);
        }

        mes_particules=new dksParticulesVolumes("Particules_volumes",0.,0.,0.,volumes_particules,sizeof(volumes_particules)/sizeof(dksVolume*));


        mes_particules->initialise_flux(-2,2,0,30,-2,2,HAUT);
        mes_particules->random_orientations();
        volumes[5]=mes_particules;

        //Création de la scène:
        scene=new dksMoteur3d("essaim",ecran,
                            NULL,0,
                            volumes,6,
                            yeux,1,
                            sources,3,
                            oeil_1);

        scene->ajoute_volumes(volumes_particules,sizeof(volumes_particules)/sizeof(dksVolume*),mes_particules);
        scene->determine_rendu_ombres(true,source_1,DKSECRAN_TEMPON_PROFONDEUR_1024);

        oeil_courant=oeil_1;
        initialise_editeur();

        roulis=0.;
        tangage=0.;
        lacet=0.;
    }

    //=======Destructeur:
    ~Scene_Test()
    {
        uint32_t i;
        if(scene!=NULL) delete scene;
        for (i=0;i<sizeof(volumes)/sizeof(dksVolume*);i++) if (volumes[i]!=NULL) delete(volumes[i]);
        for (i=0;i<sizeof(yeux)/sizeof(dksOeil*);i++) if (yeux[i]!=NULL) delete(yeux[i]);
        for (i=0;i<sizeof(sources)/sizeof(dksSourceLumineuse*);i++) if (sources[i]!=NULL) delete(sources[i]);
        for (i=0;i<sizeof(volumes_particules)/sizeof(dksCube*);i++) if (volumes_particules[i]!=NULL) delete volumes_particules[i];
    }

    //===============================================================
    //          Initialisations
    //===============================================================

    virtual void active()
    {
        scene->active();
    }

    virtual void maj_dimensions_fenetre()
    {
        scene->maj_resolutions_yeux();
    }

    private:
    void initialise_openGl()
    {
        glClearColor(0.,0.,0.,0.);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glShadeModel(GL_SMOOTH);

        //Activation des éclairages:
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
    }


    //=================================================================
    //                  Affichages
    //=================================================================
    public:


    virtual void affiche(double t)
    {
        this->Scene::affiche(t);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0,0,ecran->resolution_x,ecran->resolution_y);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mon_cube->determine_RTL(roulis,tangage,lacet);
        mon_cube->a_ete_deplace();
        roulis+=0.01;
        tangage+=0.013;
        lacet+=0.017;

        mes_particules->maj_flux(3.);
        mes_particules->maj_volumes();
        scene->maj(t);
        double centre[]={0.,3.,0.};
        //mes_particules->maj_gravite_perturbation_Perlin(centre,1.,0.1,0.5,1.,0.25);
        //mes_particules->maj_positions_points();
        scene->affiche(oeil_courant);
    }


        //============================================================================
        //                     Initialisations de l'éditeur
        //============================================================================

        virtual void initialise_editeur()
        {
            pas_deplacement=0.01;
            pas_rotation=.1;
            vitesse_deplacement=0.1;
            vitesse_rotation=0.1;
            oeil_1->masse=10.;
            oeil_1->determine_coefficient_frottement(1.,100.);

            source_1->masse=10.;
            source_1->determine_coefficient_frottement(1.,100.);

            mes_particules->masse=10.;
            mes_particules->determine_coefficient_frottement(1.,100.);

            //mon_cube->masse=10;
            mon_cube->determine_coefficient_frottement(1.,100.);

            conteneurs_selectionnables.push_back(oeil_1);
            conteneurs_selectionnables.push_back(source_1);
            conteneurs_selectionnables.push_back(source_2);
            conteneurs_selectionnables.push_back(source_3);
            conteneurs_selectionnables.push_back(mon_cube);
            conteneurs_selectionnables.push_back(mes_particules);
            conteneur_selectionne=oeil_1;

            balance=true;
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

                    break;

                case 105:

                    break;

                //Touches F..
                case 1:

                    break;

                case 2:

                default:
                    break;

            }

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
                    vitesse_deplacement+=pas_deplacement; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '-':
                    vitesse_deplacement-=pas_deplacement; cout<<"Deplacement: "<<vitesse_deplacement<<endl;
                    break;
                case '*':
                    vitesse_rotation+=pas_rotation; cout<<"Rotation: "<<vitesse_rotation<<endl;
                    break;
                case '/':
                    vitesse_rotation-=pas_rotation; cout<<"Rotation: "<<vitesse_rotation<<endl;
                    break;
                case 'o':
                    if (scene->renvoie_oeil_courant()==oeil_1) {scene->determine_oeil_courant(source_1); conteneur_selectionne=source_1;}
                    else {scene->determine_oeil_courant(oeil_1);conteneur_selectionne=oeil_1;}
                case 'b':
                    if (balance)balance=false; else balance=true;
                    scene->determine_rendu_ombres(balance,source_1);
                    break;

                case 'y':
                    source_1->determine_ouverture(source_1->renvoie_ouverture()+0.5);
                    cout<<source_1->renvoie_ouverture()<<endl;
                    break;
                case 't':
                    source_1->determine_ouverture(source_1->renvoie_ouverture()-0.5);
                    cout<<source_1->renvoie_ouverture()<<endl;
                    break;

                default:
                    break;
            }
        }


        virtual void touche_relachee(unsigned char key, int x, int y)
        {
            switch (key)
            {
                default:
                    break;
            }
            conteneur_selectionne->stop_propulsion();
            conteneur_selectionne->stop_propulsion_angulaire();
        }


};

#endif // SCENE_TEST_H_INCLUDED
