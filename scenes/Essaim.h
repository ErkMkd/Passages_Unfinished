#ifndef ESSAIM_H_INCLUDED
#define ESSAIM_H_INCLUDED

//==== Erreurs

#define ESSAIM_OK 0
#include "../dksMoteur3d/Particules/dksParticules.h"
#include "../dksMoteur3d/Texte/dksTexteMap.h"
#include "Scene.h"

class Essaim :public Scene
{
    public:

    dksOeil* oeil_1;

    private:

    dksConteneur* conteneur_1;
    dksParticules* mes_particules;
    dksMoteur3d* scene;

    dksVolume* volumes[10];
    dksOeil* yeux[10];
    dksConteneur* conteneurs[10];
    dksTextMap* mon_texte;

    double centre_gravite[3];
    double acceleration_gravite;
    double t_prec;
    double distance_min;
    double pas_deplacement;
    double pas_rotation;

    //===========================================================
    //              Constructeur
    //===========================================================

    public:
    Essaim(dksEcran* p_ecran):Scene(p_ecran)
    {
        oeil_1=NULL;
        mes_particules=NULL;
        scene=NULL;
        acceleration_gravite=9.8;
        t_prec=-1.;
        distance_min=1.;

        mes_particules=new dksParticules("Test_particules",0.,0.,0.,1000);
        //dispose_points();

        mes_particules->random_points(-5.,5.,-5.,5.,-5.,5.);

        //mes_particules->random_masses(1.,3.);
        mes_particules->determine_masses(1.);
        mes_particules->determine_rafraichissement_auto(true);


        centre_gravite={0.,0.,0.};

        oeil_1=new dksOeil("Oeil_1",0.,0.,-5.);

        conteneur_1=new dksConteneur("conteneur_1",0.,0.,0.);

        volumes[0]=mes_particules;
        yeux[0]=oeil_1;
        conteneurs[0]=conteneur_1;


        scene=new dksMoteur3d("essaim",ecran,
                            conteneurs,1,
                            volumes,1,
                            yeux,1,
                            NULL,0,
                            oeil_1);

        mon_texte=new dksTextMap("polices/Groupe.tif",ecran,"texte_test",0.14);
        mon_texte->determine_texte("INTENSE is back !");
        conteneur_1->ajoute_enfant(mon_texte);

        initialise_editeur();
    }

    ~Essaim()
    {
        cout<<"Destruction de la scene Esaim"<<endl;
        if(scene!=NULL) delete scene;
        if (mes_particules!=NULL) delete mes_particules;
        if(oeil_1!=NULL) delete oeil_1;
        if (conteneur_1!=NULL) delete conteneur_1;
        if (mon_texte!=NULL) delete mon_texte;
    }

    //===============================================================
    //          Initialisations
    //===============================================================

    virtual void active()
    {
        oeil_courant=oeil_1;
        scene->active();
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

    //====================================================
    //          Génère des coordonnées de départ
    //====================================================
    void dispose_points()
    {
        double px=2.;
        double py=1.;
        double pz=0.;
        double* coordonnees=mes_particules->renvoie_points();
        for (uint32_t i=0;i<mes_particules->renvoie_nombre_de_points();i++)
        {
            coordonnees[3*i]=px;
            coordonnees[3*i+1]=py;
            coordonnees[3*i+2]=pz;
            px+=1.;
        }
    }

    //=================================================================
    //                  Affichages
    //=================================================================
    public:


    virtual void affiche(double t)
    {
        if (t_prec<0.) { t_prec=t; return;}
        double dt=t-t_prec;
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glColor3ub(0xff,0xff,0xff);


        conteneur_1->rotation(.05,0.,1.,0.);
        glPointSize(6);
        scene->maj(t);
        scene->affiche(oeil_courant);

        mes_particules->maj_gravite_perturbation_Perlin(centre_gravite,3.,1.,distance_min, 2.,1.);
        mon_texte->affiche(oeil_courant);


        //mes_particules->gravite(centre_gravite,1.,0.5,distance_min);

    }

        //============================================================================
        //                     Initialisations de l'éditeur
        //============================================================================

        virtual void initialise_editeur()
        {
            pas_deplacement=0.1;
            pas_rotation=.1;
        }

        //=================================================
        //                     Gestion de la souris
        //=================================================
        virtual void mouvement_souris(int x,int y)
        {
                centre_gravite[0]=-((double)x-ecran->resolution_x/2)/100.;
                centre_gravite[1]=-((double)y-ecran->resolution_y/2)/100.;
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
                    oeil_1->tourne(HAUT,pas_rotation);
                    break;
                case 103:
                    //flèche bas
                    oeil_1->deplace(ARRIERE,pas_deplacement);
                    break;
                case 102:
                    //flèche droite
                    oeil_1->tourne(BAS,pas_rotation);
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

                    break;
                case '3':

                    break;

                case '4':

                    break;
                case '6':

                    break;
                case '7':

                    break;
                case '1':

                    break;
                case '0':

                    break;
                case '.':

                    break;
                case ' ':

                    break;
                case '+':
                    distance_min+=1.;
                    break;
                case '-':
                    distance_min-=1.;
                    break;


                default:
                    break;
            }
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


        }


};

#endif // ESSAIM_H_INCLUDED
