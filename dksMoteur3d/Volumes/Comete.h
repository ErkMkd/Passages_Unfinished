#ifndef COMETE_H_INCLUDED
#define COMETE_H_INCLUDED

/*
*
*   Seul le volume situé au calque 0 du fichier lwo2 est pris en compte.
*
*   Les jets sont placés sur les faces appartenants à une surface nommée "jets"
*   Ils sont centrés et les rayons des surfaces de projection sont calculés selon les aires des faces.
*
*   Les paramètres ajustables pour les jets:
*           -Puissance
*           -Ouverture
*           -Tailles des particules
*           -Durées de vie des particules
*
*/

//===================================================================================================
//                                 Classe de base de la comète
//===================================================================================================

//Codes d'erreurs:

#define COMETE_OK 0
#define COMETE_NOYAU 10
#define COMETE_JETS 11


#include "../Elements_Fondamentaux/Conteneur.h"
#include "../Elements_Fondamentaux/Volume.h"
#include "../Particules/Jet_Particules.h"
#include "../LWO/Charge_LWO2.h"
#include "../LWO/Convertion_LWO2_Volume.h"

class Comete :public Conteneur
{

    private:
        Volume* noyau;
        vector<Jet_Particules*> jets;
        uint32_t nbrJets;

        Charge_LWO2*    noyau_lwo;
        Surface*   surfaces_noyau;

    //=================================================================
    //                      CONSTRUCTEUR
    //=================================================================

    public:
        Comete( char* p_nom="Comete",char* fichier_lwo_noyau=NULL,char* chemin_textures=NULL,
                double px=0., double py=0., double pz=0.,
                Vecteur_Unitaire* axe1 = NULL, Vecteur_Unitaire* axe2 = NULL, Vecteur_Unitaire* axe3 = NULL
                )
                :Conteneur(p_nom,px,py,pz,axe1,axe2,axe3)
        {

            noyau=NULL;
            noyau_lwo=NULL;
            nbrJets=0;

            //Chargement du noyau:
            if (!charge_noyau(fichier_lwo_noyau,chemin_textures))
            {
                erreur=COMETE_NOYAU;
                cout<<"ERREUR dans la comète "<<nom<<" - Impossible de charger le noyau"<<endl;
                return;
            }
            //Création des jets:
            if (!creation_jets())
            {
                erreur=COMETE_JETS;
                cout<<"ERREUR dans la comète "<<nom<<" - Impossible de créer les jeysers"<<endl;
                return;
            }


        }

        //============== DESTRUCTEUR:
        ~Comete()
        {
            for (uint32_t i=0;i<jets.size();i++)
            {
                if (jets[i]!=NULL) delete(jets[i]);
            }
            jets.clear();

            if (noyau!=NULL) delete noyau;
            if(noyau_lwo!=NULL) delete noyau_lwo;
            //!! Les surfaces ne sont pas effacées.
            //Elles doivent être purgées en aval par Convertion_LWO2_Volume::libere_memoire()
        }

        //===================================================================
        //              Initialisations de la comète
        //===================================================================

        //-------------------------
        //  Charge le noyau
        //-------------------------
        bool charge_noyau(char* fichier,char* chemin_textures)
        {
            noyau_lwo=new Charge_LWO2(fichier,chemin_textures);
            if (noyau_lwo==NULL) return false;
            if (noyau_lwo->erreur != LWO2_OK) return false;

            surfaces_noyau=Convertion_LWO2_Volume::creation_surfaces(noyau_lwo);
            if (surfaces_noyau==NULL) return false;
            noyau=Convertion_LWO2_Volume::creation_volume(noyau_lwo,surfaces_noyau,0);
            if (noyau==NULL) return false;
            if (noyau->erreur != VOLUME_OK) return false;

            ajoute_enfant(noyau);

            return true;
        }

        //-------------------------
        //  Génère les jets
        //-------------------------
        bool creation_jets()
        {
            uint32_t i;
            uint32_t nbr_particules_jeyser=200; //Le nombre max de particules par jeyser

            for (i=0;i<noyau->nbrFaces;i++)
            {
                Face* face=&noyau->faces[i];
                string nom_surface = string(face->surface->nom);
                if (nom_surface=="jets")
                {
                    double jx=face->centre_gravite_relatif.x;
                    double jy=face->centre_gravite_relatif.y;
                    double jz=face->centre_gravite_relatif.z;

                    float ouverture_jet=20;
                    float rayon_jet=0.2;

                    Jet_Particules* jeyser=new Jet_Particules("Jeyser",jx,jy,jz,nbr_particules_jeyser,
                                                                NULL,NULL,&face->normale_relative);
                    if (jeyser==NULL) return false;

                    jeyser->determine_eclairees(true);
                    jeyser->determine_debit(80.);
                    jeyser->determine_vitesse_jet(2.);
                    jeyser->random_axes_rotations_particules();
                    jeyser->random_durees_particules(1.,5.);
                    jeyser->random_masses_particules(1.,10.);
                    jeyser->random_tailles_particules(0.05,0.3);
                    jeyser->random_vitesses_rotations_particules(10.,600.);
                    jeyser->determine_ouverture(ouverture_jet);
                    jeyser->determine_rayon_jet(rayon_jet);
                    jeyser->determine_calcul_espace_absolu(true);

                    jeyser->determine_gravite(-0.2,0.,0.);
                    jeyser->determine_frottement_particules(0.1);

                    jets.push_back(jeyser);
                    nbrJets++;
                }
            }

            cout<<"Nombre de jeysers: "<<nbrJets<<endl;

            return true;
        }

        //--------------------------------
        //  Ajoute la comète à une scène
        //--------------------------------
        void ajoute_dans_scene(Moteur3d* scene)
        {
            scene->ajoute_conteneur(this);
            scene->ajoute_volume(noyau);
            for (uint32_t i=0;i<jets.size();i++)
            {
                scene->ajoute_particules((Particules*)jets[i],noyau);
            }
        }


        //============================================================================
        //                              Gestion des éléments
        //============================================================================


        Volume* renvoie_noyau()
        {
            return noyau;
        }

        vector<Jet_Particules*>* renvoie_jets()
        {
            return &jets;
        }

};
#endif // COMETE_H_INCLUDED
