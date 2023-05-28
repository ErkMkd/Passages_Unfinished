#ifndef CONVERTION_LWO2_VOLUME_H_INCLUDED
#define CONVERTION_LWO2_VOLUME_H_INCLUDED

//===================================================================================================
//        Convertion des données récupérées du format LWO2 pour créer un volume utilisable par le moteur
//      La couleur d'ambiance ré-émise par les matériaux n'étant pas définie dans les données LWO,
//      elle est fixée au maximum (1.0).
//===================================================================================================

#include "Charge_LWO2.h"
#include "../Elements_Fondamentaux/Volume.h"
#include "../../gestion_fichiers/gestion_fichiers.h"

    using namespace std;

#define DEBUG_CONVERTION_LWO2 true
#define DEBUG_DUREES_CONVERTION_LWO2 false

//---------------------------------------------
// La structure renvoyée par le convertisseur
//---------------------------------------------

class Volumes_convertis
{
    public:
    uint32_t nbrVolumes;
    uint32_t nbrSurfaces;
    Volume** volumes;
    Surface** surfaces;

    Volumes_convertis()
    {
        nbrVolumes=0;
        volumes=NULL;
        surfaces=NULL;
    }

    ~Volumes_convertis()
    {
        uint32_t i;
        if (volumes!=NULL)
        {
            for (i=0;i<nbrVolumes;i++)
            {
                if (volumes[i]!=NULL) delete volumes[i];
            }
            free(volumes);
        }

        if (surfaces!=NULL)
        {
            for (i=0;i<nbrSurfaces;i++)
            {
                if (surfaces[i]!=NULL) delete surfaces[i];
            }
            free(surfaces);
        }


    }

    Surface* renvoie_surface(char* nom)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                if (strcmp(nom,surfaces[i]->nom)==0) return surfaces[i];
            }
            return NULL;
        }

};

//===================================================================================================
//                          Classe de convertion
//===================================================================================================

class Convertion_LWO2_Volume: public Fichiers   //Hérite de la classe Fichiers pour les traitements de chaînes de caratères.
{
    public:

        //--------------------------------------------------
        //  Crée un volume à partir d'un calque LWO2.
        //  Renvoie un tableau contenant les volumes.
        //  Renvoie 0 si il y a eut un problème.
        //--------------------------------------------------
        static Volumes_convertis* convertion(Charge_LWO2* donnees_lwo)
        {
            return NULL;

            uint32_t i,j;

            Volumes_convertis* volumes=new Volumes_convertis();

            //Convertion des surfaces:
            volumes->surfaces = convertion_surfaces(donnees_lwo);
            if (volumes->surfaces==NULL)
            {
                delete volumes;
                return NULL;
            }
            volumes->nbrSurfaces=donnees_lwo->renvoie_nbrSurfaces();

            //Convertion des volumes:
            volumes->nbrVolumes=donnees_lwo->renvoie_nbrCalques();
            volumes->volumes=(Volume**)calloc(volumes->nbrVolumes,4);
            if (volumes->volumes==NULL)
            {
                cout<<"ERREUR dans Convertion_LWO2_Volume::convertion - Mémoire insuffisante"<<endl;
                delete volumes;
                return NULL;
            }

            for (i=0;i<volumes->nbrVolumes;i++)
            {
                volumes->volumes[i]=convertion_volume(i,donnees_lwo,volumes);
                if (volumes->volumes[i]==NULL)
                {
                    delete volumes;
                    return NULL;
                }
            }
            return volumes;
        }

private:
        //===================================================================================
        //Converti les données d'un calque en volume
        //Renvoie un pointeur sur le nouveau volume,
        //NULL si une erreur s'est produite.
        //===================================================================================
        static Volume* convertion_volume(uint32_t numCalque, Charge_LWO2* donnees_lwo, Volumes_convertis* volumes)
        {
            uint32_t i,j;

            //Récupère le calque:
            LWO2_Calque* calque=donnees_lwo->renvoie_LWO2_calque(numCalque);

            if (calque==NULL)
            {
                if ( DEBUG_CONVERTION_LWO2 )
                cout<<" ERREUR dans Convertion_LWO2_Volume::creation_volume() - Calque inconnu : "<<numCalque<<endl;
                return NULL;
            }

            if ( DEBUG_CONVERTION_LWO2 )
            cout<<"========== Création du volume "<<calque->nom<<" à partir de données LWO2 ========="<<endl;


            //Création des faces:

            uint32_t nbrFaces=calque->renvoie_nbr_polygones();

            //Allocation des faces:
            Face** faces=(Face**)calloc(nbrFaces,4);
            if (faces==NULL)
            {
                if ( DEBUG_CONVERTION_LWO2 )
                cout<<" ERREUR dans Convertion_LWO2_Volume::creation_volume() - Impossible d'allouer la table des faces"<<endl;
                return NULL;
            }
            //Création des faces:
            for (i=0;i<nbrFaces;i++)
            {
                faces[i]=new Face();
            }

            //Initialisation des faces:

            for (i=0;i<nbrFaces;i++)
            {
                LWO2_Polygone* face_lwo2=calque->renvoie_polygone(i);
                faces[i]->indice=i; //! PROVISOIRE - Cette valeur est réinitialisée lors de la création du volume.
                ////Initialise la face:
                faces[i]->aretes=(uint32_t*)malloc(sizeof(uint32_t) * (uint32_t)face_lwo2->nbrSommets+1); //+1 pour le bouclage
                if(faces[i]->aretes==NULL)
                {
                    cout<<" ERREUR dans Convertion_LWO2_Volume.creation_volume() - Impossible d'allouer la table des arêtes"<<endl;
                    for (i=0;i<nbrFaces;i++){delete faces[i];}
                    free(faces);
                    return NULL;
                }

                for (j=0;j<face_lwo2->nbrSommets;j++)
                {
                    faces[i]->aretes[j]=face_lwo2->indices_sommets[j];
                }
                faces[i]->aretes[j]=face_lwo2->indices_sommets[0]; //Bouclage
                faces[i]->nbrSommets=face_lwo2->nbrSommets;
                faces[i]->surface=volumes->renvoie_surface(donnees_lwo->renvoie_etiquette(face_lwo2->indice_etiquette_surface));

                if (faces[i]->surface==NULL)
                {
                    cout<<"ERREUR dans Convertion_LWO2_Volume::creation_volume() - Surface introuvable pour la face "<<i<<endl;
                    for (i=0;i<nbrFaces;i++){delete faces[i];}
                    free(faces);
                    return NULL;
                }
            }

            //Création des textures UV:

            if ( DEBUG_CONVERTION_LWO2 )
            cout<<"............. Initialisation des UV des faces ................."<<endl;

            for (i=0;i< donnees_lwo->renvoie_nbrSurfaces() ;i++)
            {
                LWO2_Surface* surface_lwo=donnees_lwo->renvoie_LWO2_surface(i);
                for (j=0;j<nbrFaces;j++)
                {
                    //if (!faces[j]->affiche_texture) initialise_face_UV(faces[j],surface_lwo,donnees_lwo);
                }
            }


            //Création du volume:
            Volume* nouveau_volume = new Volume(calque->nom,
                                                LIBERE_FACES|LIBERE_ARETES_FACES,
                                                double(calque->pivot[0]),double(calque->pivot[1]),double(calque->pivot[2]),
                                                NULL,NULL,NULL,
                                                calque->sommets,calque->nbrSommets,
                                                faces,nbrFaces
                                                );
            return nouveau_volume;
        }




private:
        //===================================================================================
        //Converti les surfaces extraites d'un fichier LWO2:
        //Renvoie un tableau de surfaces, NULL si il y a eut un problème.
        //Ne pas oublier de restituer la mémoire une fois que les surfaces ne servent plus.
        //===================================================================================
        static Surface** convertion_surfaces(Charge_LWO2* donnees_lwo)
        {
            float duree_surfaces=glutGet(GLUT_ELAPSED_TIME);

            if ( DEBUG_CONVERTION_LWO2 )
            cout<<"+++++ Convertion des surfaces LWO2 +++++"<<endl;
            uint32_t i;
            uint32_t nbrSurfaces=donnees_lwo->renvoie_nbrSurfaces();
            if (nbrSurfaces==0)
            {
                cout<<"Pas de volume défini - "<<donnees_lwo->chemin_fichier<<endl;
                return NULL;
            }

            //---- Allocation de la table des pointeurs de surfaces:

            Surface** surfaces=(Surface**)calloc(nbrSurfaces,4);
            if (surfaces==NULL)
            {
                if ( DEBUG_CONVERTION_LWO2 ) cout<<"ERREUR dans Convertion_LWO2_Volume::creation_surfaces() - Mémoire insuffisante"<<std::endl;
                return NULL;
            }

            //Création des objets Surface:
            for (i=0;i<nbrSurfaces;i++)
            {
                surfaces[i]=new Surface();
            }
            //---- Création des surfaces:

            for (i=0;i<nbrSurfaces;i++)
            {
                LWO2_Surface* surface=donnees_lwo->renvoie_LWO2_surface(i);
                surfaces[i]->nom=copie_chaine(surface->nom);
                surfaces[i]->adoucissement=surface->adoucissement;
                surfaces[i]->double_face=surface->double_face;
                surfaces[i]->r=surface->couleur[0];
                surfaces[i]->v=surface->couleur[1];
                surfaces[i]->b=surface->couleur[2];
                surfaces[i]->luminosite=surface->luminosite;
                surfaces[i]->diffusion=surface->diffusion;
                surfaces[i]->specularite=surface->specularite;
                surfaces[i]->brillance=surface->brillance*100.;
                surfaces[i]->reflexion=surface->reflexion;
                surfaces[i]->transparence=surface->transparence;
                surfaces[i]->translucidite=surface->translucidite;
                surfaces[i]->ambiance=1.f;   //niveau de ré-émission de la couleur d'ambiance au maximum.
                surfaces[i]->angle_limite_adoucissement=surface->angle_limite_adoucissement;
                if (DEBUG_CONVERTION_LWO2)
                {
                    cout<<"  + SURFACE: "<<surfaces[i]->nom<<endl;
                    cout<<"      double face: "<<boolalpha<<surfaces[i]->double_face<<noboolalpha<<endl;
                }
            }

            duree_surfaces=glutGet(GLUT_ELAPSED_TIME)-duree_surfaces;
            if (DEBUG_DUREES_CONVERTION_LWO2)
            cout<<"Durée de création des surfaces : "<<duree_surfaces<<endl;
            return surfaces;
        }

};

#endif // CONVERTION_LWO2_VOLUME_H_INCLUDED
