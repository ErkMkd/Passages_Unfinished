#ifndef DKSMOTEUR3D_H_INCLUDED
#define DKSMOTEUR3D_H_INCLUDED

//**********************************************************************************************************
//
//      Rendu d'une sc�ne en perspective anim�e -
//      Code d'origine ActionScript 3.0 : Eric Kernin & S�bastien Saouter - 2009/2010
//      Portage C++ openGl - Eric Kernin 2010 - 2011
//      Version 2 - Mars 2011
//
//      R�vision: Les volumes sont affich�s individuellements. Leurs faces ne sont plus stock�es dans une table commune,
//                  Cela posait trop de probl�mes et obligeait � un exc�s de calculs.
//
//      Version 2.:
//                  Optimisation du rendu avec notamment l'utilisation des tableaux d'OpenGl.
//                  Gestion des "shaders" pour divers rendus (rugosit�, r�flexion...)
//
//      Le cube d'arri�re plan peut servir � plusieurs sc�nes.
//
//**********************************************************************************************************

#define EPSILON 1e-8

#include "Rendu/dksShaders.h"
#include "Geometrie_Espace/dksVecteur.h"
#include "Geometrie_Espace/dksVecteurUnitaire.h"
#include "Geometrie_Espace/dksRepere.h"
#include "Elements_Fondamentaux/dksConteneur.h"
#include "Elements_Fondamentaux/dksVolume.h"
#include "Volumes/dksCube.h"
#include "Elements_Fondamentaux/dksOeil.h"
#include "Elements_Fondamentaux/dksSourceLumineuse.h"


#include <GL/glut.h>

//================= Donn�es globales:
#define DKS_TAILLE_NOMS 128 //La taille des tempons contenants les cha�nes de caract�res pour les noms des �l�ments.

//================= Debugage:
#define DKSM3D_AFFICHE_BOITES_COLLISIONS false

#define DKSM3D_DEBUG_INITIALISATIONS false
#define DKSM3D_DEBUG_FACES_TRANSPARENTES false

//================= Codes d'erreurs � r�cup�rer dans .erreur:

#define DKSM3D_OK 0
#define DKSM3D_ERREUR_MEMOIRE 1
#define DKSM3D_ERREUR_TEXTURES 2


//================= Donnees de rendu:

#define DKSM3D_TEXTURES_TAILLES_DIFFERENTES false //Mipmap

//Structure utilis�e pour le tri des volumes (pour �viter les bugs dans l'affichage des faces transparentes)
class tri_volume
{   public:
    double distance;        //Distance du volume � l'oeil
    uint32_t indice_volume;   //L'indice du volume
};

//================================================================================
//                                   La classe:
//================================================================================

class dksMoteur3d : public dksConteneur
{
    public:

        uint16_t erreur; //Le code d'erreur si un probl�me est survenu.
        bool drapeau_active; //True lorsque la sc�ne est active et a la main sur openGl

        //Elements de la sc�ne:
        vector<dksConteneur*> elements;     //Table regroupant TOUT les �l�ments de la sc�ne.
                                            //Sert par exemple pour les calculs de mouvements cin�tiques.

        uint32_t nbrConteneurs;
        uint16_t nbrYeux;
        uint32_t nbrVolumes;
        uint16_t nbrSources;

        vector<dksConteneur*> conteneurs;
        vector<dksOeil*> yeux;
        vector<dksSourceLumineuse*> sources_lumineuses;
        vector<dksVolume*> volumes;
        vector<tri_volume> indices_tri_volumes; //Obsol�te pour le moment

        uint32_t nbr_faces_transparentes;
        vector<tri_face> faces_transparentes_triables; //Les faces triables sont celles qui appartiennent aux volumes n'utilisants pas les listes
                                                         // d'OpenGl pour leur rendu.
                                                         ///!!! ATTENTION EN MODIFIANT LE MODE DE RENDU D'UN VOLUME AVEC DES FACES TRANSPARENTES EN COURS D'ANIMATION !!!

        //Donn�es globales:
        //dksConteneur  conteneur_principal ; //Le premier parent de la hi�rarchie globale

        //Arri�re plan:
        dksCube* cube_arriere_plan;

        //Donn�es d'animation:
        bool drapeau_mouvements;    //True si le moteur se charge de rafraichir les mouvements

        //Couleur du fond:
        GLfloat couleur_fond[4];

        //Couleur de la lumi�re ambiante:
        GLfloat lumiere_ambiante[4];

        //Couleur du brouillard:
        bool drapeau_brouillard;
        bool drapeau_brouillard_surfaces_lumineuses; //True(d�faut): Pas de changement d'�tat du brouillard lors de l'affichage des surfaces transparentes
                                                    //False: D�sactivation du brouillard lors de l'affichage des surfaces transparentes
        float brouillard_min;
        float brouillard_max;
        GLfloat couleur_brouillard[4];

        //Dur�es:
        double duree_rendu;
        double duree_mouvements;
        double duree_hierarchie;
        double duree_rotations;
        double duree_sommets_absolus;
        double duree_volumes;
        double duree_collisions;

        bool drapeau_debug;
        bool drapeau_maj;
        bool drapeau_activation_rendu;  //True si la sc�ne est activ�e � chaque d�but d'affichage et d�sactiv�e � la fin.

    private:
        dksEcran* ecran;    //L'�cran dans lequel la sc�ne s'affiche
        dksOeil* oeil; /// L'oeil courant est en private, car lors d'un changement de point de vue, il faut r�initialiser l'�cran (taille, ouverture)

        uint32_t    indice_face_transparente;    //Indice utilis� lors de l'affichage des faces transparentes
        dksSurface* surface_courante;
        dksVolume* volume_courant;

        //Gestion des ombres port�es
        bool drapeau_rendu_ombres;
        GLuint texture_ombres_id;
        GLuint frameBuffer_ombres_id;
        GLint largeur_texture_ombres;
        GLint hauteur_texture_ombres;
        dksSourceLumineuse* source_des_ombres;
        GLfloat intensite_ombres;   //Facteur compris entre 0 et 1

        //Etat d'openGl restitu� � chaque rendu:
        bool drapeau_eclairage; //M�morise l'activation de l'�clairage



        //=====================================================================
        //                          Constructeur
        /// Les tableaux d'�l�ments d'entr�e ne sont pas conserv�s. Ils peuvent
        /// reservir au programme utilisateur.
        //=====================================================================
    public:
        dksMoteur3d(char* p_nom,dksEcran* p_ecran,
                    dksConteneur** p_conteneurs=NULL, uint32_t p_nbrConteneurs=0,
                    dksVolume** p_volumes=NULL, uint32_t p_nbrVolumes=0,
                    dksOeil** p_yeux=NULL, uint16_t p_nbrYeux=0,
                    dksSourceLumineuse** p_sources=NULL,uint16_t p_nbrSources=0.,
                    dksOeil* p_oeil=NULL
                ):dksConteneur(p_nom)
        {
            erreur=DKSM3D_OK;
            drapeau_active=false;
            ecran=p_ecran;
            uint32_t i;
            //t0=-1.; t0 initialis� par active()
            drapeau_eclairage=false;
            drapeau_maj=false;
            cube_arriere_plan=NULL;

            //Ombres port�es:
            drapeau_rendu_ombres=false;
            texture_ombres_id=0;
            source_des_ombres=NULL;

            //nbrImages=0;
            nbrConteneurs=p_nbrConteneurs;
            nbrYeux=p_nbrYeux;
            nbrVolumes=p_nbrVolumes;
            nbrSources=p_nbrSources;

            creation_tables_elements(p_conteneurs,p_volumes,p_yeux,p_sources);
            initialise_table_faces_transparentes();

            oeil=p_oeil;

            drapeau_mouvements=true;

            //Param�tres par d�faut:
            couleur_brouillard[0]=0.;
            couleur_brouillard[1]=0.;
            couleur_brouillard[2]=0.;
            couleur_brouillard[3]=1.;
            brouillard_min=40.;
            brouillard_max=500.;
            drapeau_brouillard=false;
            drapeau_brouillard_surfaces_lumineuses=true;

            couleur_fond[0]=0.;
            couleur_fond[1]=0.;
            couleur_fond[2]=0.;
            couleur_fond[3]=1.;

            lumiere_ambiante[0]=0.2;
            lumiere_ambiante[1]=0.2;
            lumiere_ambiante[2]=0.2;
            lumiere_ambiante[3]=1.;

            //Initialise la hi�rarchie:
            if (DKSM3D_DEBUG_INITIALISATIONS) cout<<"///////////// Moteur 3d - Initialisation de la sc�ne  \\\\\\\\\\"<<endl;

            for (i=0;i<nbrVolumes;i++)
            {
                if (volumes[i]->parent==NULL)
                {
                    if (DKSM3D_DEBUG_INITIALISATIONS) cout<<volumes[i]->nom<<endl;
                     ajoute_enfant(volumes[i]);
                }
            }

            for (i=0;i<nbrConteneurs;i++)
            {
                if (conteneurs[i]->parent==NULL)
                {
                     ajoute_enfant(conteneurs[i]);
                }
            }

            for (i=0;i<nbrYeux;i++)
            {
                if (yeux[i]->parent==NULL)
                {
                     ajoute_enfant(yeux[i]);
                }
            }

            for (i=0;i<nbrSources;i++)
            {
                if (sources_lumineuses[i]->parent==NULL)
                {
                     ajoute_enfant(sources_lumineuses[i]);
                }
            }

            drapeau_debug=false;
            drapeau_activation_rendu=false;

            maj_resolutions_yeux();

        }

        //==========================================================================
        //                               Destructeur
        //==========================================================================

        ~dksMoteur3d()
        {
            cout<<"D�but de la destruction de l'objet dksMoteur3d: "<<nom<<endl;
            desactive();
            faces_transparentes_triables.clear();
            indices_tri_volumes.clear();
            cout<<"Fin de la destruction de l'objet dksMoteur3d"<<endl;
        }

        //====================================================================================================
        //                  Gestion des tables des �l�ments
        //====================================================================================================

    private:
        //------ Fonction appel�e par le constructeur
        void creation_tables_elements(dksConteneur** p_conteneurs, dksVolume** p_volumes,
                                        dksOeil** p_yeux, dksSourceLumineuse** p_sources)
        {
            uint32_t i;

            for (i=0;i<nbrConteneurs;i++) { conteneurs.push_back(p_conteneurs[i]); elements.push_back(p_conteneurs[i]);}
            for (i=0;i<nbrVolumes;i++)
                {
                    volumes.push_back(p_volumes[i]);
                    elements.push_back(p_volumes[i]);
                }
            for (i=0;i<nbrYeux;i++) { yeux.push_back(p_yeux[i]); elements.push_back(p_yeux[i]);}
            for (i=0;i<nbrSources;i++) { sources_lumineuses.push_back(p_sources[i]); elements.push_back(p_sources[i]);}
            creation_table_tri_volumes();
        }


        //------- Creation ou mise � jour de la table de tri des volumes:
        void creation_table_tri_volumes()
        {
            indices_tri_volumes.clear();
            for (uint32_t i = 0 ; i<nbrVolumes;i++)
            {
                ajoute_tri_volume();
                indices_tri_volumes[i].indice_volume=i;
                indices_tri_volumes[i].distance=0.;
            }
        }

        void ajoute_tri_volume()
        {
            tri_volume nouveau_tri_volume;
            indices_tri_volumes.push_back(nouveau_tri_volume);
        }

        //-----------------------------------------------------
        //      Renvoie true si l'�l�ment est dans la sc�ne
        //-----------------------------------------------------
    public:

        bool element_est_dans_la_scene(dksConteneur* element)
        {
           if (conteneur_est_dans_la_scene(element)) return true;
           if (volume_est_dans_la_scene(element)) return true;
           if (oeil_est_dans_la_scene(element)) return true;
           if (source_lumineuse_est_dans_la_scene(element)) return true;
           return false;
        }

        bool conteneur_est_dans_la_scene(dksConteneur* cont)
        {
            for (uint32_t i=0;i<nbrConteneurs;i++)
            {
                if (cont == conteneurs[i]) return true;
            }
            return false;
        }

        bool volume_est_dans_la_scene(dksConteneur* vol)
        {
            for (uint32_t i=0;i<nbrVolumes;i++)
            {
                if (vol == volumes[i]) return true;
            }
            return false;
        }

        bool source_lumineuse_est_dans_la_scene(dksConteneur* source)
        {
            for (uint32_t i=0;i<nbrSources;i++)
            {
                if (source == sources_lumineuses[i]) return true;
            }
            return false;
        }

        bool oeil_est_dans_la_scene(dksConteneur* noeil)
        {
            for (uint32_t i=0;i<nbrYeux;i++)
            {
                if (noeil == yeux[i]) return true;
            }
            return false;
        }

        //--------------------------------------------------------------
        //          Fonctions d'ajouts et de suppression d'�l�ments.
        //--------------------------------------------------------------

        bool ajoute_conteneur(dksConteneur* cont, dksConteneur* parent=NULL)
        {
            if (conteneur_est_dans_la_scene(cont))return false;
            conteneurs.push_back(cont);
            if (parent==NULL && cont->parent==NULL) ajoute_enfant(cont);
            else if (parent!=NULL) parent->ajoute_enfant(cont);
            nbrConteneurs++;
            elements.push_back(cont);
            return true;
        }

        bool ajoute_volume(dksVolume* vol, dksConteneur* parent=NULL)
        {
            if (volume_est_dans_la_scene(vol))return false;
            volumes.push_back(vol);

            tri_volume nouveau_tri_volume;
            nouveau_tri_volume.distance=0.;
            nouveau_tri_volume.indice_volume=nbrVolumes;
            indices_tri_volumes.push_back(nouveau_tri_volume);

            if (parent==NULL && vol->parent==NULL) ajoute_enfant(vol);
            else if (parent!=NULL) parent->ajoute_enfant(vol);
            nbrVolumes++;
            elements.push_back(vol);
            initialise_table_faces_transparentes();
            return true;
        }

        //------ ATTENTION: La fonction ne test pas si les volumes sont d�j� dans la sc�ne.
        bool ajoute_volumes(dksVolume** table_volumes,uint32_t nombre,dksConteneur* parent=NULL)
        {
            uint32_t i;
            for (i=0;i<nombre;i++)
            {
                dksVolume* vol=table_volumes[i];
                volumes.push_back(vol);
                elements.push_back(vol);
                if (parent==NULL && vol->parent==NULL) ajoute_enfant(vol);
                else if (parent!=NULL && vol->parent==NULL) parent->ajoute_enfant(vol);
            }
            nbrVolumes+=nombre;
            creation_table_tri_volumes();
            initialise_table_faces_transparentes();
        }

        bool ajoute_source_lumineuse(dksSourceLumineuse* source, dksConteneur* parent=NULL)
        {
            if (source_lumineuse_est_dans_la_scene(source))return false;
            sources_lumineuses.push_back(source);
            if (parent==NULL && source->parent==NULL) ajoute_enfant(source);
            else if (parent!=NULL) parent->ajoute_enfant(source);
            nbrSources++;
            elements.push_back(source);
            return true;
        }

        bool ajoute_oeil(dksOeil* nouvel_oeil, dksConteneur* parent=NULL)
        {
            if (oeil_est_dans_la_scene(nouvel_oeil))return false;
            yeux.push_back(nouvel_oeil);
            if (parent==NULL && nouvel_oeil->parent==NULL) ajoute_enfant(nouvel_oeil);
            else if (parent!=NULL) parent->ajoute_enfant(nouvel_oeil);
            nbrYeux++;
            elements.push_back(nouvel_oeil);
            return true;
        }

        void maj_resolutions_yeux()
        {
            for (uint32_t i=0;i<nbrYeux;i++)
            {
                yeux[i]->determine_resolution(ecran->resolution_x,ecran->resolution_y);
            }
        }

        //-------------------------------------------
        //Gestion du volume d'arri�re plan:
        //-------------------------------------------
        void determine_cube_arriere_plan(dksCube* p_cube)
        {
            cube_arriere_plan=p_cube;
        }

        //---------------------------------------------------------------------------------------------------------------
        //  Initialise l'arri�re plan d'openGl pour le texturage d'environnement. Sert pour les shaders de "r�flexion cube"
        /// Un seul arri�re plan peut �tre activ� � la fois.
        ///------------------- PROVISOIRE !! -------------------------
        //---------------------------------------------------------------------------------------------------------------
        void active_arriere_plan()
        {
            if (cube_arriere_plan==NULL) return;
            //---------- Cr�ation des textures CUBE_MAP:
            Image* image;
            dksSurface* surface;
            glActiveTexture(GL_TEXTURE0);

            surface=cube_arriere_plan->renvoie_surface_avant();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            surface=cube_arriere_plan->renvoie_surface_arriere();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            surface=cube_arriere_plan->renvoie_surface_droite();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            surface=cube_arriere_plan->renvoie_surface_gauche();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            surface=cube_arriere_plan->renvoie_surface_haut();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            surface=cube_arriere_plan->renvoie_surface_bas();
            if(surface->textures_couleur.size()!=0)
            {
                image=surface->textures_couleur[0]->image;
                if (image!=NULL) glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,GL_RGBA,image->largeur,image->hauteur,0,image->format_pixels,image->type_pixels,image->image);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        }

        //==============================================================================
        //                  Gestion des faces transparentes
        //  Les faces transparentes sont stock�s dans une table globale, pour que le tri soit
        //  propre.
        //  Les faces des volume qui se servent enti�rement des liste openGl ou qui ont
        //  leur rendu de faces transparente activ� ne sont pas inclus dans cette table.
        //==============================================================================
        //-------- Rafraichi la table des faces transparentes ----
        void initialise_table_faces_transparentes()
        {
            uint32_t i,j,k;
            faces_transparentes_triables.clear();
            for (i=0;i<nbrVolumes;i++)
            {
                dksVolume* volume=volumes[i];
                if (volume->renvoie_mode_rendu()==DKSVOLUME_MODE_LISTE || volume->renvoie_drapeau_rendu_faces_transparentes()) continue; //Les faces des volumes affich�s par une liste openGl
                for (j=0; j< volume->nbrSurfaces;j++)
                {
                    dksSurface* surface = volume->surfaces[j];
                    if (surface->drapeau_transparence)
                    {
                        for(k=0;k<surface->nbr_faces;k++)
                        {
                            tri_face* nouveau_tri_face=ajoute_tri_face();
                            nouveau_tri_face->face=surface->faces[k];
                        }
                    }
                }
            }
            nbr_faces_transparentes=faces_transparentes_triables.size();
            #if DKSM3D_DEBUG_FACES_TRANSPARENTES
                cout<<"Nombre de faces transparentes dans la sc�ne"<<nom<<" : "<<nbr_faces_transparentes<<endl;
            #endif
        }

        private:
        tri_face* ajoute_tri_face()
        {
            tri_face nouvelle_face;
            faces_transparentes_triables.push_back(nouvelle_face);
            return &faces_transparentes_triables[faces_transparentes_triables.size()-1];
        }

        //=============================================================================================
        //          Gestion des ombres port�es
        //  Voir la classe dksEcran pour les r�solutions possibles de la texture de profondeur
        //=============================================================================================
        public:
        void determine_rendu_ombres(bool p_drapeau,dksSourceLumineuse* p_source_des_ombres=NULL,uint8_t tempon_profondeur_id=DKSECRAN_TEMPON_PROFONDEUR_512,GLfloat p_intensite_ombres=1.)
        {
            intensite_ombres=p_intensite_ombres;
            if (tempon_profondeur_id==DKSECRAN_TEMPON_PROFONDEUR_512)
            {
                texture_ombres_id=ecran->textures[TX_OMBRES_512];
                frameBuffer_ombres_id=ecran->frame_buffers[FB_OMBRES_512];
            }
            else if (tempon_profondeur_id==DKSECRAN_TEMPON_PROFONDEUR_1024)
            {
                texture_ombres_id=ecran->textures[TX_OMBRES_1024];
                frameBuffer_ombres_id=ecran->frame_buffers[FB_OMBRES_1024];
            }
            else if (tempon_profondeur_id==DKSECRAN_TEMPON_PROFONDEUR_2048)
            {
                texture_ombres_id=ecran->textures[TX_OMBRES_2048];
                frameBuffer_ombres_id=ecran->frame_buffers[FB_OMBRES_2048];
            }

            source_des_ombres=p_source_des_ombres;
            if (texture_ombres_id==0 || source_des_ombres==NULL) drapeau_rendu_ombres=false;
            else drapeau_rendu_ombres=p_drapeau;

            //Dimensions de la texture de profondeur:
            if (drapeau_rendu_ombres)
            {
                glBindTexture(GL_TEXTURE_2D,texture_ombres_id);
                glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&largeur_texture_ombres);
                glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&hauteur_texture_ombres);
                source_des_ombres->determine_resolution(uint32_t(largeur_texture_ombres),uint32_t(hauteur_texture_ombres));
                //source_des_ombres->determine_resolution(largeur_texture_ombres,hauteur_texture_ombres);
            }

            //Initialise la source des ombres pour les volumes (sauf le volume d'arri�re plan):
            for (uint32_t i=0;i<nbrVolumes;i++)
            {
                volumes[i]->determine_recoit_ombres(drapeau_rendu_ombres&volumes[i]->recoit_ombres(),texture_ombres_id,source_des_ombres,intensite_ombres);
            }
        }

        //====================================================================================================
        //                            Fonctions de debogage
        //====================================================================================================

        void affiche_durees_calculs()
        {
            cout<<"--------- Dur�es de calcul:"<<endl;
            cout<<"mouvements"<<duree_mouvements<<endl;
            cout<<"hierarchie"<<duree_hierarchie<<endl;
            cout<<"sommets absolus"<<duree_sommets_absolus<<endl;
            cout<<"affichage des volumes"<<duree_volumes<<endl;
        }


        //=============================================================================
        //                          Initialisation g�n�rale d'openGl
        /// Normalement cette fonction n'est appel�e qu'une seule fois.
        //=============================================================================
    public:
        static void initialise_opengl()
        {
            //----Initialisation des param�tres de rendu des polygones:

            glPolygonMode(GL_FRONT,GL_FILL);//Mode de rendu des faces avant des polygones
            glPolygonMode(GL_BACK,GL_FILL);//Mode de rendu des faces arri�re des polygones
            glEnable(GL_CULL_FACE); //Active la distinction face avant/face arri�re des polygones pour le rendu.
            glCullFace(GL_BACK);    //D�termine quelle face des polygones n'est pas affich�e.
            glPointSize(1); //La taille des sommets en mode d'affichage de sommets

            //----Lissage:
            glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
            glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
            glDisable(GL_POLYGON_SMOOTH);
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POINT_SMOOTH);

            //----Initialisation du rendu des textures:

            glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

            dksRendu::desactive_textures();

            //----Initialisation de l'�clairage:

            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE); //Sert � d�finir que c'est la position de l'oeil qui sert pour le calcul de la sp�cularit�.
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR); //La lumi�re sp�culaire est ajout�e par dessus le texturage, pour �viter de l'alt�rer.

            glDisable(GL_NORMALIZE); //Convertion automatique des normales en vecteur unitaire

            //---- Mat�riaux de base :

            const GLfloat defaut_ambiance[]    = { 0.f, 0.f, 0.f, 0.f };
            const GLfloat defaut_diffusion[]    = { 0.8f, 0.8f, 0.8f, 1.f };
            const GLfloat defaut_speculaire[]   = { 1.f, 1.f, 1.f, 1.f };
            const GLfloat defaut_luminosite[]   = { 0.f, 0.f, 0.f, 1.f };
            const GLfloat defaut_brillance[] = { 64.0f };

            glDisable(GL_COLOR_MATERIAL);   //D�sactivation de l'usage de la commande glColorMaterial()

            glMaterialfv(GL_FRONT, GL_AMBIENT,   defaut_ambiance);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,   defaut_diffusion);
            glMaterialfv(GL_FRONT, GL_SPECULAR,  defaut_speculaire);
            glMaterialfv(GL_FRONT,GL_EMISSION, defaut_luminosite);
            glMaterialfv(GL_FRONT, GL_SHININESS, defaut_brillance);

        }

        //====================================================================================================
        //                            Initialisation de la sc�ne
        //====================================================================================================
public:
        //============== Active la sc�ne et prend la main sur OpenGl:
        // Toutes les sources sont activ�es (si leur nombre est inf�rieur au nombre max g�r� par OpenGl)
        //  Donc, pour une gestion dynamique des sources, faire �a en Aval.
        void active()
        {
            if (drapeau_active) return ;

            //----Initialisation du tempon de profondeur:
            glDepthMask(GL_TRUE);   //Accessibilit� en �criture du tempon de profondeur.
                                    //Cela peut servir pour afficher les surfaces transparentes.
            glDepthFunc(GL_LESS); //Test de comparaison avec la profondeur pr�c�dente


            //----Activation de la transparence:
            //Le calcul de transparence s'effectue comme suit:
            // Cs*FCs+Cd*FCd : C=composante(A,R,V ou B)
            //                  s=pixel "source" (le pixel � ajouter)
            //                  d=pixel "destination" (le pixel d�j� pr�sent dans le tempon)
            //                  F=facteur
            // Les facteurs ne peuvent pas �tre d�termin� arbitrairement, h�las.
            // Il faut utiliser la fonction glBlendFunc(facteur_source,facteur_destination) avec des constantes pr�d�termin�es.

            //glBlendFunc(GL_SRC_ALPHA, GL_ONE ); //facteur ARVB pixel � ajouter , facteur ARVB pixel d�j� pr�sent dans le tempon;
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); //facteur ARVB pixel � ajouter , facteur ARVB pixel d�j� pr�sent dans le tempon;

            //----Couleur de fond:
            glClearColor(couleur_fond[0],couleur_fond[1],couleur_fond[2],couleur_fond[3]);

            //----Initialise le brouillard:

            if(drapeau_brouillard)
            {
                glFogi(GL_FOG_MODE,GL_LINEAR);
                glFogfv(GL_FOG_COLOR,couleur_brouillard);
                glFogf(GL_FOG_START,brouillard_min);
                glFogf(GL_FOG_END,brouillard_max);
                glFogf(GL_FOG_DENSITY,1.);
                glHint(GL_FOG_HINT,GL_NICEST);
                glEnable(GL_FOG);
            }

            else glDisable(GL_FOG);

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lumiere_ambiante);  //Couleur de la lumi�re ambiente globale

            drapeau_active=true;

            active_sources_lumineuses();

            oeil->determine_resolution(ecran->resolution_x,ecran->resolution_y);
            oeil->active();
        }


        void desactive()
        {
            if (!drapeau_active) return;
            drapeau_active=false;
            desactive_sources_lumineuses();
            oeil->desactive();
        }

        //---------------------------------------------------------------
        //-------- Gestion du brouillard:
        //---------------------------------------------------------------

        void active_brouillard()
        {
            drapeau_brouillard=true;
            if (drapeau_active)
            {
                glFogi(GL_FOG_MODE,GL_LINEAR);
                glFogfv(GL_FOG_COLOR,couleur_brouillard);
                glFogf(GL_FOG_START,brouillard_min);
                glFogf(GL_FOG_END,brouillard_max);
                glFogf(GL_FOG_DENSITY,1.);
                glHint(GL_FOG_HINT,GL_NICEST);
                glEnable(GL_FOG);
            }
        }

        void desactive_brouillard()
        {
            if (drapeau_active) glDisable(GL_FOG);
            drapeau_brouillard=false;
        }

        void determine_limites_brouillard(float min,float max)
        {
            brouillard_min=min;
            brouillard_max=max;
            if (drapeau_active)
            {
                glFogf(GL_FOG_START,brouillard_min);
                glFogf(GL_FOG_END,brouillard_max);
            }
        }

        void determine_alpha_brouillard(GLfloat alpha)
        {
            couleur_brouillard[3]=alpha;
            if (drapeau_active) glFogfv(GL_FOG_COLOR,couleur_brouillard);

        }

        void determine_couleur_brouillard(GLfloat r,GLfloat v,GLfloat b, GLfloat a)
        {
            couleur_brouillard[0]=r;
            couleur_brouillard[1]=v;
            couleur_brouillard[2]=b;
            couleur_brouillard[3]=a;
            if (drapeau_active) glFogfv(GL_FOG_COLOR,couleur_brouillard);
        }

        void determine_couleur_brouillard(uint32_t cb)
        {
            couleur_brouillard[0]=GLfloat((cb>>16)&0xff)/255.;
            couleur_brouillard[1]=GLfloat((cb>>8)&0xff)/255.;
            couleur_brouillard[2]=GLfloat(cb&0xff)/255.;
            couleur_brouillard[3]=GLfloat((cb>>24)&0xff)/255.;
            if (drapeau_active) glFogfv(GL_FOG_COLOR,couleur_brouillard);
        }

        //---------------------------------------------------------------
        //------- Gestion de la couleur du fond et de la lumi�re ambiante
        //---------------------------------------------------------------

        void determine_couleur_fond(GLfloat r,GLfloat v,GLfloat b, GLfloat a)
        {
            couleur_fond[0]=r;
            couleur_fond[1]=v;
            couleur_fond[2]=b;
            couleur_fond[3]=a;
            if (drapeau_active) glClearColor(couleur_fond[0],couleur_fond[1],couleur_fond[2],couleur_fond[3]);
        }

        void determine_couleur_fond_ARVB (uint32_t couleur)
        {
            GLfloat a=float((couleur>>24)&0xff)/256.;
            GLfloat r=float((couleur>>16)&0xff)/256.;
            GLfloat v=float((couleur>>8)&0xff)/256.;
            GLfloat b=float(couleur&0xff)/256.;
            determine_couleur_fond(r,v,b,a);
        }

        void determine_lumiere_ambiante(GLfloat r,GLfloat v,GLfloat b, GLfloat a)
        {
            lumiere_ambiante[0]=r;
            lumiere_ambiante[1]=v;
            lumiere_ambiante[2]=b;
            lumiere_ambiante[3]=a;
            if (drapeau_active) glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lumiere_ambiante);
        }

        void determine_lumiere_ambiante_ARVB (uint32_t couleur)
        {
            GLfloat a=float((couleur>>24)&0xff)/256.;
            GLfloat r=float((couleur>>16)&0xff)/256.;
            GLfloat v=float((couleur>>8)&0xff)/256.;
            GLfloat b=float(couleur&0xff)/256.;
            determine_lumiere_ambiante(r,v,b,a);
        }

        //Applique une teinte aux couleurs de diffusion des surfaces.
        //Ca sert pour simuler un eclairage.
        void applique_teinte_diffusion(uint32_t teinte)
        {
            for (uint32_t i=0;i<nbrVolumes;i++) volumes[i]->applique_teinte_diffusion(teinte);
        }

        void applique_teinte_specularite(uint32_t teinte)
        {
            for (uint32_t i=0;i<nbrVolumes;i++) volumes[i]->applique_teinte_specularite(teinte);
        }

        void applique_teinte_luminescence(uint32_t teinte)
        {
            for (uint32_t i=0;i<nbrVolumes;i++) volumes[i]->applique_teinte_luminescence(teinte);
        }

        void applique_teinte_ambiance_ARVB(uint32_t teinte)
        {
            for (uint32_t i=0;i<nbrVolumes;i++) volumes[i]->applique_teinte_ambiance(teinte);
        }



        //====================================================================================================
        //                       Fonctions de rendu de la sc�ne
        //=====================================================================================================

        void determine_oeil_courant(dksOeil* p_oeil)
        {
            if (drapeau_active)
            {
                if (p_oeil==NULL) return;
                if (oeil!=NULL)oeil->desactive();
                p_oeil->active();
            }
            oeil=p_oeil;
        }

        dksOeil* renvoie_oeil_courant()
        {
            return oeil;
        }

        //-----------------------------------------------------------------------------------------------
		// Gestion du rendu des sources lumineuses.
		//  Comme OpenGL limite le nombre de sources � 8,
		//  si la sc�ne contient plus de 8 sources, il faut activer les huit sources les plus proches.
		/// G�rer avec les shader pour plus de huit sources.
        //-----------------------------------------------------------------------------------------------
        void active_sources_lumineuses()
        {
            uint16_t i;
            drapeau_eclairage=false;
            GLint nbr_sources_max;
            glGetIntegerv(GL_MAX_LIGHTS,&nbr_sources_max);
            if (nbrSources<=nbr_sources_max)
            {
                for (i=0;i<nbrSources;i++)
                {
                    sources_lumineuses[i]->active_lumiere();
                }
            }
            if(nbrSources>0)
            {
                drapeau_eclairage=true;
                glEnable(GL_LIGHTING);
            }
        }

        void desactive_sources_lumineuses()
        {
            uint16_t i;
            for (i=0;i<nbrSources;i++)
            {
                sources_lumineuses[i]->desactive_lumiere();
            }
            glDisable(GL_LIGHTING);
            drapeau_eclairage=false;
        }

        void active_source_lumineuse(dksSourceLumineuse* source)
        {
            source->active_lumiere();
            rafraichi_drapeau_eclairage();
        }

        void desactive_source_lumineuse(dksSourceLumineuse* source)
        {
            source->desactive_lumiere();
            rafraichi_drapeau_eclairage();
        }

        private:
        void rafraichi_drapeau_eclairage()
        {
            if (!drapeau_active) return;
            for (uint32_t i=0;i<nbrSources;i++)
            {
                if (sources_lumineuses[i]->lumiere_est_active())
                {
                    drapeau_eclairage=true;
                    return;
                }
            }
            drapeau_active=false;
        }

        //---------------------------------------------------------------
		// Parcours la hi�rarchie pour lancer la propagation si besoin
        //---------------------------------------------------------------
		public:
		void maj_hierarchie(dksConteneur* cont)
		{
			dksConteneur* ce;
			unsigned int i;

            uint32_t nbrEnfants=cont->renvoie_nbr_enfants();

			for (i = 0; i < nbrEnfants ; i++)
			{
				ce = cont->enfants[i];

				if (ce->a_change)
                {
					ce->initialisation_repere(cont->repere_absolu);
					ce->translation_repere(cont->repere_absolu,ce->repere_absolu);
					ce->repere_absolu->maj_axes_echelles();
					ce->propagation_hierarchie();
				}
				else { maj_hierarchie(ce); }
			}
		}

		//---------------------------------------------------------------
		// Calcul les matrices de transformation des conteneurs, � partir
		//  de leurs rep�res absolus.
		/// La hi�rarchie doit avoir �t� mise � jour!
        //---------------------------------------------------------------
        void calcul_matrices_conteneurs()
        {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            for (uint32_t i=0;i<elements.size();i++)
            {
                elements[i]->calcul_matrice(this);
            }
            glPopMatrix();
        }

        //---------------------------------------------------------------
		//Tri les volumes
		/// Obsol�te pour le moment, car les faces transparentes doivent
		/// �tre tri�es en globalit� au sein de la sc�ne.
		//---------------------------------------------------------------
        //Le test utilis� pour le tri des volumes:
		static bool comparaison_volumes(tri_volume t1, tri_volume t2) { return t1.distance > t2.distance; }

		void tri_volumes() ///A ADAPTER
		{
		    //if (nbrFaces_transparentes==0) return;
		    //Calcul les distances avec l'oeil:
		    for (uint32_t i=0;i<nbrVolumes;i++)
		    {
                dksVolume* vol=volumes[indices_tri_volumes[i].indice_volume];
                vol->calcul_centre_geometrique_absolu();
                double dx=oeil->repere_absolu->x-vol->centre_geometrique_absolu[0];
                double dy=oeil->repere_absolu->y-vol->centre_geometrique_absolu[1];
                double dz=oeil->repere_absolu->z-vol->centre_geometrique_absolu[2];
                indices_tri_volumes[i].distance=dx*dx+dy*dy+dz*dz;
		    }
		    std::sort(indices_tri_volumes.begin(),indices_tri_volumes.end(),dksMoteur3d::comparaison_volumes);
		}

		//---------------------------------------------------------------
		//Tri les faces transparentes:
		//---------------------------------------------------------------
        //Le test utilis� pour le tri des faces:
		static bool comparaison_faces(tri_face t1, tri_face t2) { return t1.distance > t2.distance; }

		void tri_faces_transparentes()
		{
		    uint32_t i;
		    double dx,dy,dz;
		    //cout<<"-----------------------------------------------"<<endl;
		    //if (nbrFaces_transparentes==0) return;
		    //Calcul les distances avec l'oeil:
		    for (i=0;i<nbr_faces_transparentes;i++)
		    {
                dksFace* face=faces_transparentes_triables[i].face;
                face->calcul_centre_gravite_absolu(face->volume->repere_absolu);
                //cout<<face->volume->nom<<endl;
                dx=oeil->repere_absolu->x-face->centre_gravite_absolu[0];
                dy=oeil->repere_absolu->y-face->centre_gravite_absolu[1];
                dz=oeil->repere_absolu->z-face->centre_gravite_absolu[2];
                faces_transparentes_triables[i].distance=dx*dx+dy*dy+dz*dz;
		    }
		    std::sort(faces_transparentes_triables.begin(),faces_transparentes_triables.end(),comparaison_faces);
		}


        //====================================================================================
        //====================================================================================
        //                  Affichages pour le d�bugage
        //====================================================================================
        //====================================================================================

public:

        //---------------------------------------------------------------
		//Affiche les normales relatives des sommets d'un volume
		//---------------------------------------------------------------
        void affiche_normales_relatives_sommets_volume(dksVolume* volume)
        {
            uint32_t i;
            double* sommets=volume->renvoie_sommets();
            double* normales=volume->normales_sommets;
            double norme_visible=1.;

            for (i=0;i<volume->nbrSommets;i++)
            {
                double Ax=sommets[3*i];
                double Ay=sommets[3*i+1];
                double Az=sommets[3*i+2];

                double Bx= normales[3*i] * norme_visible + Ax;
                double By= normales[3*i+1] * norme_visible + Ay;
                double Bz= normales[3*i+2] * norme_visible + Az;

                if (drapeau_debug)
                {
                    cout<<"Sommet:"<<i<<" X:"<<normales[3*i]<<" Y:"<<normales[3*i+1]<<" Z:"<<normales[3*i+2]<<endl;
                    drapeau_debug=false;
                }


                dksAffichagesDebogue::affiche_segment(Ax,Ay,Az,Bx,By,Bz,0xff0000);
            }

        }

        //---------------------------------------------------------------
		//Affiche la normale d'un sommet d'une face:
		//---------------------------------------------------------------


        void affiche_normales_relatives_sommets_face(dksVolume* volume, uint32_t indice_face)
        {
            dksFace* face=volume->faces[indice_face];
            for (uint16_t i=0;i<face->nbrSommets;i++)
            {
                affiche_normale_relative_sommet_face(volume,indice_face,i);
            }
        }

		void affiche_normale_relative_sommet_face(dksVolume* volume, uint32_t indice_face, uint16_t indice_sommet)
		{
		    if (!drapeau_active) return;
		    dksFace* face= volume->faces[indice_face];
		    uint32_t indice=3*face->aretes[indice_sommet];
		    double* sommets=volume->renvoie_sommets();
		    double Ax=sommets[indice];
		    double Ay=sommets[indice+1];
		    double Az=sommets[indice+2];

            //dksVecteur distance(oeil->repere_absolu->x-Ax,oeil->repere_absolu->y-Ay,oeil->repere_absolu->z-Az);

		    double norme_visible=1.;
            double* normale_sommet=volume->renvoie_normale_relative_sommet(face->aretes[indice_sommet]);
		    double Bx= normale_sommet[0] * norme_visible + Ax;
		    double By= normale_sommet[1] * norme_visible + Ay;
		    double Bz= normale_sommet[2] * norme_visible + Az;

		    dksAffichagesDebogue::affiche_segment(Ax,Ay,Az,Bx,By,Bz,0xff0000);
		}

        //---------------------------------------------------------------
		//Affiche la normale d'une face:
		//---------------------------------------------------------------

		void affiche_normale_relative_face(dksVolume* volume, uint32_t indice_face) ///A ADAPTER
		{
		    if (!drapeau_active) return;
		    dksFace* face= volume->renvoie_face(indice_face);

		    double Ax=face->centre_gravite_relatif[0];
		    double Ay=face->centre_gravite_relatif[1];
		    double Az=face->centre_gravite_relatif[2];

		    double norme_visible=1.;

		    double Bx= face->normale[0] * norme_visible + Ax;
		    double By= face->normale[1] * norme_visible + Ay;
		    double Bz= face->normale[2] * norme_visible + Az;

		    dksAffichagesDebogue::affiche_segment(Ax,Ay,Az,Bx,By,Bz,0xffff00);
		}

        //====================================================================================================
        //                         Calcul les positions instantann�es des objets
        //                          Le temps est exprim� en secondes.
        //====================================================================================================
public:
        virtual void maj(double t)
        {
            //------------- Rafraichi l'intervalle de temps:
            //position_temps=t;
            //delta_t=t-t0;


            if ((erreur!=DKSM3D_OK) || (!drapeau_active)) return;

            uint32_t i;

            dksVecteur oeil_face;




            //------------- Mouvements cin�tiques (lorsque la masse des conteneurs n'est pas nulle)

               for (i=0;i<elements.size();i++)
               {
                   elements[i]->applique_mouvements();
               }


            //-------------Propagation de la hierarchie (calcul des rep�res absolus )

                maj_hierarchie(this);

            //------------- Calcul le rep�re absolu du volume d'arri�re-plan:

                if (cube_arriere_plan!=NULL)
                {

                    cube_arriere_plan->determine_position(oeil->repere_absolu->x,oeil->repere_absolu->y,oeil->repere_absolu->z);
                    cube_arriere_plan->calcul_repere_absolu();
                    cube_arriere_plan->calcul_matrice(this);

                }

            //-------- Mise � jour des param�tres de la source des ombres:
                if (drapeau_rendu_ombres)
                {
                    //Il faut que les limites en profondeur de la source des ombre soient les m�me que celles de l'oeil courant,
                    //pour que les tempons de profondeur de l'oeil et de la source correspondent.
                    source_des_ombres->determine_limites_profondeur(oeil->renvoie_zmin(),oeil->renvoie_zmax());
                }

            //-------- Positionne l'oeil:

                if (oeil!=NULL) oeil->positionne_scene();

            //------------- Calcul les matrices de transformations des conteneurs pour OpenGl:

               calcul_matrices_conteneurs();

            //-------- Gestion des collisions et des r�action physiques:

                //moteur_physique.maj_collisions();



            //-------- Calculs des �clairages:

                for (i=0;i<nbrSources;i++)
                {
                    sources_lumineuses[i]->maj();
                }

            //--------- Tri les volumes:
                //tri_volumes();
            //--------- Tri les faces transparentes:
                tri_faces_transparentes();

            //--------- Rafraichi la date du moteur physique:

                //t0=t;
                drapeau_maj=true;
        }

        //====================================================================================================
        //                                       Affiche la sc�ne
        //====================================================================================================
public:
        virtual void affiche(dksOeil* oeil)
        {
            uint32_t i;

            /*
            drapeau_debug=false;
            if (strcmp("Scene_Caduce",nom)==0) drapeau_debug=true;
            */

            //----------------- G�n�re la texture de profondeur pour les ombres:
            if (drapeau_rendu_ombres) affiche_profondeur(source_des_ombres);

            if (drapeau_eclairage) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);

            //------------------Active les �tats n�cessaire au bon affichage:
            GLboolean depth_test_mem=glIsEnabled(GL_DEPTH_TEST);
            glEnable(GL_DEPTH_TEST);

            //if ((erreur!=DKSM3D_OK) || (!drapeau_active) || (!drapeau_maj) ) return;
            if ((erreur!=DKSM3D_OK) || (!drapeau_active) ) return;

            //------------- Volume d'arri�re plan:
            //Le volume d'arri�re plan n'est pas sens� avoir des faces transparentes
            glMatrixMode(GL_MODELVIEW);

            if (cube_arriere_plan!=NULL)
            {
                if (drapeau_brouillard) glDisable(GL_FOG);
                glDisable(GL_LIGHTING);
                glPushMatrix();
                glMultMatrixd(cube_arriere_plan->matrice.matrice);

                cube_arriere_plan->affiche(oeil);
                glPopMatrix();
                if (drapeau_brouillard) glEnable(GL_FOG);
                if (drapeau_eclairage) glEnable(GL_LIGHTING);

            }

            //----------------Volumes de la sc�ne:
            //if (drapeau_debug) cout<<"T0: "<<glutGet(GLUT_ELAPSED_TIME)<<" ";

            for ( i=0 ; i<nbrVolumes ; i++ )
            {
                //dksVolume* vol=volumes[indices_tri_volumes[i].indice_volume];
                dksVolume* vol=volumes[i];

                glPushMatrix();
                glMultMatrixd(vol->matrice.matrice);
                    //if(drapeau_debug)cout<<"Volume: "<<i<<" - "<<vol->nom<<endl;
                vol->affiche(oeil);
                glPopMatrix();
            }

            //if (drapeau_debug) cout<<"T1: "<<glutGet(GLUT_ELAPSED_TIME)<<endl;

            //------------------- Faces transparentes pr�sentes dans la table globale:

            if(nbr_faces_transparentes>0)
            {
                rendu_faces_transparentes_scene();
            }

            //---------------Affiche les bo�tes de collision :
            #if DKSM3D_AFFICHE_BOITES_COLLISIONS

            glDisable(GL_DEPTH_TEST);
            for (i=0;i< nbrVolumes;i++)
            {
                if (volumes[i]->boite_collision!=NULL)volumes[i]->boite_collision->affiche();
            }

            #endif

            //---------------Restitue les �tats sensibles:
            if (depth_test_mem)glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);



            //drapeau_maj=false;
        //cout<<duree_rendu<<endl;
        }


        //====================================================================================================
        //                       Affiche les surfaces luminescentes de la sc�ne
        //====================================================================================================


        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {
            if ((erreur!=DKSM3D_OK) || (!drapeau_active)) return;
            glMatrixMode(GL_MODELVIEW);
            glDisable(GL_LIGHTING);
            //glDisableClientState(GL_NORMAL_ARRAY);
            //desactive_textures();
            //glUseProgram(0);
            GLboolean brouillard_mem=glIsEnabled(GL_FOG);
            if (!drapeau_brouillard_surfaces_lumineuses) glDisable(GL_FOG);

            //--------- Affiche les volume:
            for ( uint32_t i=0 ; i<nbrVolumes ; i++ )
            {
                //dksVolume* vol=volumes[indices_tri_volumes[i-1].indice_volume];
                dksVolume* vol=volumes[i];
                glPushMatrix();
                glMultMatrixd(vol->matrice.matrice);
                vol->affiche_couleurs_luminescentes(oeil);
                glPopMatrix();
            }

            if (brouillard_mem) glEnable(GL_FOG);

        //cout<<duree_rendu<<endl;
        }

        //===============================================================================================================
        //                       Rendu de la sc�ne,vue de la source des ombres, pour obtenir la texture de profondeur
        //===============================================================================================================
        virtual void affiche_profondeur(dksOeil* oeil)
        {
            if ((erreur!=DKSM3D_OK) || (!drapeau_active) || (!drapeau_rendu_ombres)) return;

            GLboolean lumieres_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);
            glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
            glDisableClientState(GL_NORMAL_ARRAY);
            desactive_textures();
            glUseProgram(0);

            //--------- Calcul la matrice de transformation vue de la source:

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();

            //--------- M�morisation du frame buffer actuel:
            GLint frameBuffer_mem;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&frameBuffer_mem);
            GLint dimensions_mem[4];
            glGetIntegerv(GL_VIEWPORT,dimensions_mem);


            //------- Activation du tempon de rendu des distances:
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frameBuffer_ombres_id);
            source_des_ombres->maj_projection_source(largeur_texture_ombres,hauteur_texture_ombres);
            glViewport(0,0,largeur_texture_ombres,hauteur_texture_ombres);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            glClear(GL_DEPTH_BUFFER_BIT);

            source_des_ombres->positionne_scene();

            //--------- Affiche les volume:

            for ( uint32_t i=0 ; i<nbrVolumes ; i++ )
            {
                //dksVolume* vol=volumes[indices_tri_volumes[i-1].indice_volume];
                dksVolume* vol=volumes[i];
                glPushMatrix();
                glMultMatrixd(vol->matrice.matrice);
                vol->affiche_profondeur(oeil);
                glPopMatrix();
            }

            //---------- Restitue les matrices
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glMatrixMode(GL_MODELVIEW);

            //---------- Restitue les �tats d'openGl:
            if (lumieres_mem) glEnable(GL_LIGHTING);
            glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frameBuffer_mem);
            glViewport(dimensions_mem[0],dimensions_mem[1],dimensions_mem[2],dimensions_mem[3]);

        //cout<<duree_rendu<<endl;
        }

        //-----------------------------------------------
        //      D�sactive les unit�s de textures:
        //-----------------------------------------------
        /*
        private:
        void desactive_textures()
        {
            uint16_t i;
            GLint nbr_unites_textures_max;
            glGetIntegerv(GL_MAX_TEXTURE_UNITS,&nbr_unites_textures_max);
            for (i=0;i<nbr_unites_textures_max;i++)
                {
                    glActiveTexture(GL_TEXTURE0+i);
                    glDisable(GL_TEXTURE_2D);
                    glClientActiveTexture(GL_TEXTURE0+i);
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                }
		}
		*/

		//==================================================================================================================================================
		//==================================================================================================================================================
        //                                      Rendu des faces transparentes de la sc�ne
		//==================================================================================================================================================
		//==================================================================================================================================================

        void rendu_faces_transparentes_scene()
        {
            GLint blend_mem=glIsEnabled(GL_BLEND);
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            indice_face_transparente=0;
            dksRendu::desactive_textures();
            glUseProgram(0);

            for (;indice_face_transparente<nbr_faces_transparentes;indice_face_transparente++)
            {
                dksFace* face=faces_transparentes_triables[indice_face_transparente].face;
                surface_courante=face->surface;
                volume_courant=face->volume;
                initialise_mode_rendu_volume();
                glPopMatrix();
                glPushMatrix();
                glMultMatrixd(volume_courant->matrice.matrice);
                volume_courant->initialise_materiau(surface_courante);
                switch(volume_courant->mode_rendu)
                {
                    case DKSVOLUME_MODE_IMMEDIAT:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
                        rendu_faces_transparentes_immediat();
                        indice_face_transparente--;
                    break;
                    case DKSVOLUME_MODE_TABLEAUX:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                        rendu_faces_transparentes_tableaux();
                        indice_face_transparente--;
                    break;
                }
            }


            glUseProgram(0);
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            if(!blend_mem)glDisable(GL_BLEND);
        }

        //--------------------------------------------
        //      Initialise les pointeurs de base:
        //--------------------------------------------
        void initialise_mode_rendu_volume()
        {
            switch (volume_courant->mode_rendu)
                {
                    case DKSVOLUME_MODE_IMMEDIAT:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);
                    break;

                    case DKSVOLUME_MODE_TABLEAUX:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,volume_courant->sommets);
                    break;

                    case DKSVOLUME_MODE_LISTE:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);

                    break;

                    case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);

                    break;

                    case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,volume_courant->sommets);
                    break;
                }
        }

        //-------------------------------------------------------------------------
        //          Rendu des faces transparentes de la sc�ne en mode imm�diat
        //-------------------------------------------------------------------------

        #include "dksMoteur3d_rendu_transparentes_immediat.h"

        //-------------------------------------------------------------------------
        //          Rendu des faces transparentes de la sc�ne en mode tableaux
        //-------------------------------------------------------------------------

        #include "dksMoteur3d_rendu_transparentes_tableaux.h"

        //=====================================================================
        //      Activation / D�sactivation de la sc�ne entre chaque rendu
        //=====================================================================
        virtual void init_affichage()
        {
            if(drapeau_activation_rendu) active();
        }

        virtual void fin_affichage()
        {
            if(drapeau_activation_rendu) desactive();
        }
};

#endif // MOTEUR3D_H_INCLUDED
