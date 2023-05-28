#ifndef DKSVOLUME_H_INCLUDED
#define DKSVOLUME_H_INCLUDED

//*****************************************************************************************************************************************************
//
//                                              Gestion des volumes DKS
//
//  Lors de la création d'un volume, TOUJOURS déterminer les faces PUIS les surfaces. Sinon certaines initialisations ne sont font pas correctement.
//                                  -------------------------------------------------
//
//  Un volume peut être rendu de 3 manières:
//      1-En mode immédiat,
//      2-Avec les tableaux,
//      3-Par les listes d'affichage.
//          Dans ce dernier cas, il y a deux possibilités:
//                  1-Ne stocker en liste que les surfaces opaques,
//                  2-Stocker les surfacess opaques et les surfaces transparentes, auquel cas il peut y avoir des bugs d'affichage dus à l'abscence de tri.
//
//      Le volume peut afficher en interne ses faces transparentes -> "determine_rendu_faces_transparentes(bool)"
///     Par défaut, c'est le moteur de la scène qui les affiche.
//
///     Note sur les tangentes et les binormales:
///         Elles sont approximatives sur les sommets(du fait des contraintes techniques).
///         L'adoucissement n'est pas toujours tout à fait juste, mais suffisant pour rendre la rugosité.
///         Plus les textures sont déformées sur les faces, moins la rugosité est juste.
//
//******************************************************************************************************************************************************

//#include "dksRendu3d.h"
#include "dksConteneur.h"
#include "dksOeil.h"
#include "dksSourceLumineuse.h"
#include "../Volumes/dksBoiteCollision.h"
#include "../../chargeurs_images/Image.h"
#include "../Rendu/dksShaders.h"
#include <math.h>
#include <vector>
#include <string>

#define DKSVOLUME_EPSILON 1e-8

//Informations pour la classe parent "dksConteneur"
#define DKSCONTENEUR_CLASSE_VOLUME "Volume"
#define DKSCONTENEUR_CLASSE_VOLUME_ID 1

//Codes d'erreurs, à récupérer dans dksVolume.erreur:
#define DKSVOLUME_OK 0
#define DKSVOLUME_ERREUR_VOLUME 1
#define DKSVOLUME_ERREUR_MEMOIRE 2
#define DKSVOLUME_ERREUR_LISTE_OPENGL 3
#define DKSVOLUME_ERREUR_SURFACE 4
#define DKSVOLUME_ERREUR_LIEN_FACE_SURFACE 5
#define DKSVOLUME_ERREUR_FACES 6

//Drapeaux pour la libération de la mémoire lors de la destruction:
#define DKSVOLUME_LIBERE_FACES           0x00000001
#define DKSVOLUME_LIBERE_BOITE_COLLISION 0x00000002
#define DKSVOLUME_LIBERE_SOMMETS         0x00000004
#define DKSVOLUME_LIBERE_SURFACES        0x00000008

//Modes de rendu:
#define DKSVOLUME_MODE_IMMEDIAT 0
#define DKSVOLUME_MODE_TABLEAUX 1
#define DKSVOLUME_MODE_LISTE    2   //Tout le volume est stoqué dans une liste d'affichage
#define DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES 3   //Surfaces opaques dans une liste, surfaces transparentes rendues en mode immédiat
#define DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES 4    //Surfaces opaques dans une liste, surfaces transparentes rendues en mode tableau

//Modes de remplissage des faces:
#define DKSVOLUME_REMPLISSAGE_SOMMETS 1     //N'affiche que les sommets du volume
#define DKSVOLUME_REMPLISSAGE_ARETES 2      //Affiche seulement les arètes
#define DKSVOLUME_REMPLISSAGE_COULEUR 3     //Affiche en aplats avec les couleurs de diffusion des surfaces
#define DKSVOLUME_REMPLISSAGE_SURFACES 4    //Affiche avec les modes de rendus définis dans les surfaces


//Sert pour les calculs de repères TBN des textures:
#define DKSVOLUME_FACE_INCLUSE 1
#define DKSVOLUME_FACE_FRONTALIERE 2

//Drapeau de débogage:

#define DKSVOLUME_DEBUG false
#define DKSVOLUME_DEBUG_NORMALES_SOMMETS false
#define DKSVOLUME_DEBUG_SURFACES false
#define DKSVOLUME_DEBUG_DUREES false
#define DKSVOLUME_DEBUG_FACES_TRANSPARENTES false
#define DKSVOLUME_DEBUG_TEXTURES false

#define DKSVOLUME_DEBUG_AFFICHE_NORMALES false
#define DKSVOLUME_DEBUG_AFFICHE_REPERES_TBN_FACES false
#define DKSVOLUME_DEBUG_AFFICHE_REPERES_TBN_SOMMETS false
#define DKSVOLUME_DEBUG_AFFICHE_NORMALES_SOMMETS_FACES false
#define DKSVOLUME_DEBUG_AFFICHE_REPERES_TBN_TEXTURES_RUGOSITE false

class dksSurface;
class dksVolume;

#include "dksFace.h"
#include "dksTexture.h"
#include "dksSurface.h"
#include "../Rendu/Effets_Speciaux/dksEffetSpecial.h"

//Structure d'une liaison de faces:
//La liaison peut servir pour les sommets ou les arêtes.

class liaison_faces
{
    public:
        vector<uint32_t> indices_faces;
};



//===========================================================================================
//                          DEFINITION DE LA CLASSE VOLUME
//
//  Prévoir un mode simple précision (float) pour optimiser.
//
//===========================================================================================

class dksVolume :public dksConteneur
{
     public:

        bool drapeau_debug;

        uint16_t erreur; //Code d'erreur en cas de problème

        float taille_points_sommets;    //Taille des sommets en mode de remplissage SOMMETS

        uint8_t mode_rendu;  //Mode de rendu du volume.
        uint16_t mode_remplissage;  //Initialisé à chaque nouvelle surface

        //Sommets relatifs au volume:
		GLdouble* sommets;      //Pointe sur la table de sommets utilisée pour le rendu.
                                //Si le moteur est en mode TRANSFORMATIONS_MATRICES, ce sont les sommets
                                //relatifs qui sont utilisés. Si le moteur est en mode TRANSFORMATIONS_REPERES, les
                                //sommets absolus sont utilisés.
                                //Par défaut, ce sont les sommets relatifs qui sont utilisés.

        uint32_t nbrSommets;
		uint32_t nbrFaces;
		uint32_t nbrAretes;
		dksFace** faces;
		vector<uint32_t> aretes;    //La table des aretes du volume. Chaque arête est définie par un binôme d'indexes de sommets.

        bool volume_a_change; //True lorsque le volume a bougé. Ses sommets sont alors rafraichis par la hiérarchie.

		double echelle_geometrique; //Facteur multiplicateur des sommets du volume
        float facteur_luminescence; //Facteur de luminescence général.

        //Tableau utilisé pour le tri des faces transparentes
        uint32_t nbrFaces_transparentes;
		vector<tri_face> indices_faces_transparentes;

        //Tables de liaisons. Ces informations peuvent servir.
		//vector<uint32_t> aretes; //Tableau qui peut être généré à partir des faces avec "initialisation_aretes()"
        //vector<liaison_faces> liaisons_aretes_faces; //Vecteur de vecteurs d'indices de faces
        vector<liaison_faces> liaisons_sommets_faces; //Vecteur de vecteurs d'indices de faces
        GLdouble* normales_sommets; //Normales relatives au volume des sommets

        //Tables pour la rugosité des faces:
        GLdouble* tangentes_sommets;
        GLdouble* binormales_sommets;

        //La table des surfaces.
		uint32_t nbrSurfaces;
		dksSurface** surfaces;
		dksSurface* surface_courante; //Surface en cours de rendu

        //La boite déterminant les limites de collision du volume:
        dksBoiteCollision* boite_collision;

        //Données géométriques:
        double centre_geometrique_relatif[3];
        double centre_geometrique_absolu[3];

        //Couleur des sommets en mode de remplissage SOMMETS
        float couleur_sommets[3];
        float couleur_aretes[3];

        //Couleurs du matériau courant:
        //(Réinitialisées à chaque changement de surface)
        GLfloat couleur_ambiance[4];
        GLfloat couleur_diffusion[4];
        GLfloat couleur_speculaire[4];
        GLfloat couleur_luminescence[4];

    protected:
        dksOeil* oeil_courant;  //L'observateur du volume


        double epsilon; //Sert pour le calcul de planitude des faces. Il est différent de la constante globale EPSILON
        //Gestion mémoire:
        uint32_t drapeaux_liberation_memoire;

        GLfloat noir[4];

        //Gestion des listes OpenGl:
        //Les liste d'openGl ne peuvent être utilisées que si les matrices d'OpenGl sont activées (voir dksMoteur3d.h)

        GLuint  liste_id;                           //L'identifiant de la liste OpenGl pour ce volume.
        GLuint  liste_surfaces_luminescentes_id;    //L'identifiant de la liste OpenGl pour le rendu des surfaces luminecentes de ce volume.
        GLuint  liste_profondeur_id;                //L'identifiant de la liste OpenGl pour le rendu des profondeur (sert pour les ombres portées)

        uint32_t duree_rendu;

        GLint sens_polygones;   //Sens dans lequel sont définies les faces avant des polygones (GL_CW ou GL_CCW)

        GLint nbr_attributs_sommets_max; //Le nombre maximum d'attributs supplémentaires par sommet

        //Gestion des ombres:
        dksSourceLumineuse* source_des_ombres;  //La source lumineuse génératrice des ombres
        GLuint texture_ombres_id;   //L'identifiant de la texture contenant le z-buffer de la scène vue de la source générant les ombres
        Matrice4x4 matrice_uv_source_des_ombres;  //Matrice uv source * Matrice volume : pour la projection des coordonnées UV de la texture des profondeurs
        Matrice4x4 matrice_vision_source_des_ombres;
        Matrice4x4 matrice_projection_vision_source_des_ombres;
        float angle_max_ombres; //75° par défaut. L'angle maximum entre les faces et la direction de la source des ombres pour projeter la texture de profondeur. Ca permet d'éviter les bugs de razances.
        float composantes_convertion[2];    //Les composantes de la matrice de projection de la source des ombres, qui servent à projeter la profondeur.
        GLfloat intensite_ombres; //L'intensite des ombres projetées sur le volume

        bool drapeau_affecte_par_brouillard;    //True par défaut: le volume est affecté par le brouillard.
        bool drapeau_rendu_faces_transparentes; //True si les faces transparentes sont rendues au sein du volume, false par défaut
        bool drapeau_tempon_profondeur; //True si les faces opaques affectent le tempon de profondeur
        bool drapeau_projete_ombres;    //True si le volume peut projeter des ombres (rendu lors de la phase de génération de la texture d'ombrage)
        bool drapeau_recoit_ombres;     //True si le volume peut recevoir les ombres projetées.
        bool drapeau_liste_luminescences;    //True si les surfaces luminescentes sont affichées via une liste OpenGL
        bool drapeau_liste_profondeurs;      //True si les profondeurs sont affichées via une liste openGl

        //Pointeurs sur les fonctions de rendu.
        void (dksVolume::*init_rendus[DKSSURFACE_NBR_RENDUS])();
        void (dksVolume::*rendus_immediats_opaques[DKSSURFACE_NBR_RENDUS])();
        void (dksVolume::*rendus_immediats_transparentes[DKSSURFACE_NBR_RENDUS])(dksFace*);

//========================================================================================================
//==================================== Constructeur du volume:
//========================================================================================================
    public:
        dksVolume( char* p_nom,
                uint32_t p_drapeaux_liberation_memoire=0,
                double px = 0., double py = 0., double pz = 0.,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL,
                GLdouble* pSommets = NULL,
                uint32_t p_nbrSommets=0,
                dksFace** pFaces = NULL,
                uint32_t p_nbrFaces=0,
                dksSurface** pSurfaces = NULL,
                uint32_t p_nbrSurfaces=0)
                :dksConteneur(p_nom,px,py,pz,axe1,axe2,axe3)
		{
		    drapeau_debug=false;
		    taille_points_sommets=3.;
		    //std::cout<<"------- Création du volume: "<<p_nom<<std::endl;
            noir={0.,0.,0.,1.};
            couleur_sommets={1.,1.,1.};
            couleur_aretes={1.,1.,1.};

            //--------- Initialise les variables par défaut:
            classe = DKSCONTENEUR_CLASSE_VOLUME;
            classe_id=DKSCONTENEUR_CLASSE_VOLUME_ID;

		    erreur=0;
		    drapeau_affiche=true;
		    sommets=NULL;
		    boite_collision=NULL;
		    faces=NULL;
		    surfaces=NULL;

		    normales_sommets=NULL;
		    tangentes_sommets=NULL;
		    binormales_sommets=NULL;
		    normales_sommets=NULL;

            nbrSommets=0;
		    nbrFaces=0;
		    nbrAretes=0;
		    nbrSurfaces=0;

            drapeau_rendu_faces_transparentes=false;
            drapeau_affecte_par_brouillard=true;
            drapeau_tempon_profondeur=true;
            drapeau_projete_ombres=true;
            drapeau_recoit_ombres=true;

            texture_ombres_id=0;
            source_des_ombres=NULL;
            angle_max_ombres=89.;
            intensite_ombres=1.;

            drapeaux_liberation_memoire=p_drapeaux_liberation_memoire;

            boite_collision=NULL;

		    echelle_geometrique=1.;
		    facteur_luminescence=1.;
		    epsilon=1e-7;

		    mode_rendu=DKSVOLUME_MODE_TABLEAUX;
		    liste_id=0;
		    liste_surfaces_luminescentes_id=0;
		    liste_profondeur_id=0;

            sens_polygones=GL_CCW;

		    volume_a_change=true;

            //Paramètres d'OpenGl:
            ///Voir à tester ces valeurs

		    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&nbr_attributs_sommets_max);


            //========================Création du volume:
            if (DKSVOLUME_DEBUG) cout<<"------------------ Création du volume: "<<nom<<endl;


		    //--------Initialise les sommets:

            if (!determine_sommets(pSommets,p_nbrSommets)) return;

            //---------Initialise les faces:

		    if (!determine_faces(pFaces,nbrFaces)) return;

            //--------- Initialise les surfaces:

            if (!determine_surfaces(pSurfaces,nbrSurfaces)) return;

            initialise_pointeurs_init_rendus();
            initialise_pointeurs_rendus_immediat();

            determine_mode_remplissage(DKSVOLUME_REMPLISSAGE_SURFACES);

            drapeau_liste_luminescences=false;
            drapeau_liste_profondeurs=false;
            determine_liste_luminescences(true);
            determine_liste_profondeurs(true);
		}

        //===========================================================
		//                      Constructeur de copie:
		//===========================================================
        dksVolume(dksVolume& vol_org)
        {
            drapeau_debug=vol_org.drapeau_debug;
            uint32_t i;
            drapeaux_liberation_memoire=0;
            erreur=DKSVOLUME_OK;
            sommets=NULL;
            normales_sommets=NULL;
            tangentes_sommets=NULL;
            binormales_sommets=NULL;
            nbrSommets=0;
            faces=NULL;
            nbrFaces=0;
            surfaces=NULL;
            nbrSurfaces=0;
            liste_id=0;
		    liste_surfaces_luminescentes_id=0;
		    liste_profondeur_id=0;

		    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&nbr_attributs_sommets_max);

            copie_nom(vol_org.nom);

            echelle_geometrique=vol_org.echelle_geometrique;
            sens_polygones=vol_org.sens_polygones;
            drapeau_rendu_faces_transparentes=vol_org.drapeau_rendu_faces_transparentes;
            facteur_luminescence=vol_org.facteur_luminescence;

            //------ Création des sommets:

            uint32_t p_nbrSommets=vol_org.nbrSommets;
            double* sommets_org=vol_org.renvoie_sommets();
            double* p_sommets=NULL;

            if (p_nbrSommets>0)
            {
                p_sommets=(double*)malloc(p_nbrSommets*3*sizeof(double));

                if (p_sommets==NULL)
                {
                    cout<<"ERREUR dans dksVolume::dksVolume(dksVolume&) - impossible d'allouer la table des sommets"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return;
                }
            }

            for(i=0;i<p_nbrSommets*3;i++)p_sommets[i]=sommets_org[i];
            determine_sommets(p_sommets,p_nbrSommets);
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SOMMETS;

            //------------- Création des faces

            uint32_t p_nbrFaces=vol_org.nbrFaces;
            dksFace** p_faces=NULL;
            if (p_nbrFaces>0)
            {
                p_faces=(dksFace**)calloc(p_nbrFaces,sizeof(dksFace*));
                if (p_faces==NULL)
                {
                    cout<<"ERREUR dans dksVolume::dksVolume(dksVolume&) - impossible d'allouer la table des faces"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return;
                }
            }
            for(i=0;i<p_nbrFaces;i++)
            {
                p_faces[i]=new dksFace(*vol_org.renvoie_face(i));
            }
            determine_faces(p_faces,p_nbrFaces);
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;

            //-------Création des surfaces:


            uint32_t p_nbrSurfaces=vol_org.nbrSurfaces;
            dksSurface** p_surfaces=NULL;
            if (p_nbrSurfaces>0)
            {
                p_surfaces=(dksSurface**)calloc(p_nbrSurfaces,sizeof(dksSurface*));
                if (p_surfaces==NULL)
                {
                    cout<<"ERREUR dans dksVolume::dksVolume(dksVolume&) - impossible d'allouer la table des surfaces"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return;
                }
            }
            for(i=0;i<p_nbrSurfaces;i++)
            {
                p_surfaces[i]=new dksSurface(*vol_org.renvoie_surface(i));
            }
            determine_surfaces(p_surfaces,p_nbrSurfaces);
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SURFACES;

            //------ Pointeurs de rendus:

            initialise_pointeurs_init_rendus();
            initialise_pointeurs_rendus_immediat();

            //------ Paramètres ombres:
            angle_max_ombres=vol_org.angle_max_ombres;
            intensite_ombres=vol_org.intensite_ombres;

            //------ Drapeaux:
            drapeau_tempon_profondeur=vol_org.drapeau_tempon_profondeur;
            drapeau_affecte_par_brouillard=vol_org.drapeau_affecte_par_brouillard;
            drapeau_affiche=vol_org.drapeau_affiche;
            drapeau_projete_ombres=vol_org.drapeau_projete_ombres;        /// Les surfaces copient leurs propres drapeaux,
            drapeau_recoit_ombres=vol_org.drapeau_projete_ombres;  /// => pas besoin de passer par "determine_drapeau_ombres"
            drapeau_contrainte_verticale=false;

            //-------- Mode de rendu:

            determine_mode_remplissage(vol_org.mode_remplissage);
            determine_mode_rendu(vol_org.renvoie_mode_rendu());
            drapeau_liste_luminescences=false;
            drapeau_liste_profondeurs=false;
            determine_liste_luminescences(vol_org.renvoie_drapeau_liste_luminescences());
            determine_liste_profondeurs(vol_org.renvoie_drapeau_liste_profondeurs());

        }

		//===========================================================
		//                      Destructeur:
		//===========================================================

        virtual ~dksVolume()
        {
            if (DKSVOLUME_DEBUG) cout<<"Destruction du volume: "<<nom<<endl;
            uint32_t i;
            supprime_listes();
            supprime_boite_collision();
            supprime_surfaces();
            //if(strcmp("baballe",nom)==0) cout<<"0"<<endl;
            supprime_faces();
            //if(strcmp("baballe",nom)==0) cout<<"1"<<endl;
            supprime_sommets();
            //if(strcmp("baballe",nom)==0) cout<<"2"<<endl;
            supprime_aretes();
        }



        //------- Supprime la boîte de collision:
        void supprime_boite_collision()
        {
            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_BOITE_COLLISION)!=0)
            {
                if ( boite_collision!=NULL) delete boite_collision;
            }
            boite_collision=NULL;
        }

        //------- Supprime les surfaces:
        /// La table des surfaces est également supprimée !
        void supprime_surfaces()
        {
            uint32_t i;
            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_SURFACES)!=0 && surfaces!=NULL)
            {
                for(i=0; i<nbrSurfaces;i++)
                {

                    if (surfaces[i]!=NULL) delete surfaces[i];
                }
                free(surfaces);
            }
            surfaces=NULL;
            nbrSurfaces=0;
        }


        //------- Supprime les faces:
        void supprime_faces()
        {
            uint32_t i;
            if (faces!=NULL)
            {
                if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_FACES)!=0)
                {
                    for(i=0;i<nbrFaces;i++)
                    {
                        if (faces[i]!=NULL) delete faces[i];
                    }
                    free(faces);

                }
                faces=NULL;
            }
            nbrFaces=0;
        }

        //------- Supprime les sommets:
        void supprime_sommets()
        {
            if ( sommets != NULL )
            {
                if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_SOMMETS)!=0) free (sommets);
                sommets=NULL;
            }

            if ( normales_sommets != NULL )
            {
                free(normales_sommets);
                normales_sommets=NULL;
            }

            if ( tangentes_sommets != NULL )
            {
                free(tangentes_sommets);
                tangentes_sommets=NULL;
            }

            if ( binormales_sommets != NULL )
            {
                free(binormales_sommets);
                binormales_sommets=NULL;
            }


            nbrSommets=0;
        }

        //---------- Supprime la table des arêtes:
		void supprime_aretes()
		{
		    aretes.clear();
		    nbrAretes=0;
		}


        //=============================================================================================================================
        //                      Fonctions de définition de la forme du volume
        //=============================================================================================================================
    public:

        //------------------------------------------
        //  Détermine les sommets du volume.
        //  La table en entrée est utilisée telle quelle, sans être copiée !!
        /// Il faut veiller à ce que les liens avec les faces restent cohérents.
        //------------------------------------------

        bool determine_sommets(GLdouble* pSommets,uint32_t p_nbrSommets)
        {
            supprime_sommets();

            nbrSommets=p_nbrSommets;
            sommets=pSommets;
            if (nbrSommets>0)
            {
                normales_sommets=(double*)malloc(sizeof(double)*nbrSommets*3);
                tangentes_sommets=(double*)malloc(sizeof(double)*nbrSommets*3);
                binormales_sommets=(double*)malloc(sizeof(double)*nbrSommets*3);
                if (normales_sommets==NULL || tangentes_sommets==NULL || binormales_sommets==NULL)
                {
                    cout<<"Erreur dans dksVolume::determine_sommets() - Mémoire insuffisante"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return false;
                }
            }
            calcul_centre_geometrique_relatif();
            return initialise_boite_collisions();
        }

        //------------------------------------------------
        //      Détermine l'échelle géométrique du volume
        //---------------------taille_points_sommets---------------------------
        virtual void determine_echelle_geometrique(double e)
        {
            double ne=e/echelle_geometrique;
            for(uint32_t i=0;i<nbrSommets;i++)
            {
                sommets[3*i]*=ne;
                sommets[3*i+1]*=ne;
                sommets[3*i+2]*=ne;
            }
            boite_collision->applique_facteur_echelle(ne);
            echelle_geometrique=e;

            /*
            if(mode_rendu==DKSVOLUME_MODE_LISTE || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
            {
                supprime_listes();
                genere_listes();
            }
            */
            maj_listes();
        }

        //------------------------------------------
        //  Détermine les faces du volume.
        /// Il faut veiller à ce que les liens avec les surfaces restent cohérents.
        //------------------------------------------

        bool determine_faces(dksFace** pFaces,uint32_t p_nbrFaces)
        {
            supprime_faces();

            faces=pFaces;
            nbrFaces=p_nbrFaces;

            return initialise_faces();

        }

        //------------------------------------------
        //  Détermine les surfaces du volume.
        ///Il faut d'abord initialiser les faces, sinon
        ///les tables des faces des textures ne peuvent pas être créées.
        //------------------------------------------
        bool determine_surfaces(dksSurface** pSurfaces, uint32_t p_nbrSurfaces)
        {
            supprime_surfaces();

            surfaces=pSurfaces;
            nbrSurfaces=p_nbrSurfaces;

            if (DKSVOLUME_DEBUG) cout<<"Détermine les surfaces du volume "<<nom<<" Nombre: "<<nbrSurfaces<<endl;

            if (!initialise_surfaces()) return false;

            return initialise_textures();

        }


        //=============================================================================================================================
        //                      Initialisation des données géométriques relatives du volume
        //=============================================================================================================================

        //--------------------------------------------------------------------------------------
        //             Calcul les normales des sommets du volume
        //--------------------------------------------------------------------------------------
        void calcul_normales_sommets(uint32_t indice_depart=0,uint32_t nbr=0)
        {
            uint32_t i,j,k;
            if (nbr==0 && indice_depart==0) nbr=nbrSommets;
            else nbr+=indice_depart;

            for(i=indice_depart;i<nbr;i++)
            {
                GLdouble* nml_s=&normales_sommets[3*i];
                nml_s[0]=0.;
                nml_s[1]=0.;
                nml_s[2]=0.;
                liaison_faces* sommet_faces=&liaisons_sommets_faces[i];
                for (j=0;j<sommet_faces->indices_faces.size();j++)
                {
                    dksFace* face=faces[sommet_faces->indices_faces[j]];
                    nml_s[0]+=face->normale[0];
                    nml_s[1]+=face->normale[1];
                    nml_s[2]+=face->normale[2];
                }
                double nrm= sqrt( nml_s[0]*nml_s[0] + nml_s[1]*nml_s[1] + nml_s[2]*nml_s[2]);
                nml_s[0]/=nrm;
                nml_s[1]/=nrm;
                nml_s[2]/=nrm;
                #if DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_NORMALES_SOMMETS
                    cout<<"Sommet:"<<i<<"N:"<<nml_s[0]<<","<<nml_s[1]<<","<<nml_s[2]<<endl;
                #endif
            }
        }

        //------------------------------------------------------------------------------------------------------
        //             Calcul des tangentes et des binormales aux sommets du volume.
        //             Ces repères ne tiennent pas compte de l'orientation UV des textures,
        //              mais ils peuvent servir pour des shaders appliqués aux faces non texturées.
        //------------------------------------------------------------------------------------------------------
        void calcul_tangentes_binormales_sommets()
        {
            uint32_t i,j,k;
            for(i=0;i<nbrSommets;i++)
            {
                GLdouble* tgt_s=&tangentes_sommets[3*i];
                GLdouble* nml_s=&normales_sommets[3*i];

                //Tangente par produit vectoriel:
                //  ->  ->  ->
                //  Y ^ N = T
                tgt_s[1]=0;
                if (nml_s[1]==1.)
                {
                    tgt_s[0]=1.;
                    tgt_s[2]=0.;
                }
                else if (nml_s[1]==-1.)
                {
                    tgt_s[0]=-1.;
                    tgt_s[2]=0.;
                }
                else
                {
                    tgt_s[0]=nml_s[2];  //Simplification d'un produit vectoriel entre la normale et l'axe vertical (0,1,0)
                    tgt_s[2]=-nml_s[0];
                    GLdouble norme=sqrt(tgt_s[0]*tgt_s[0]+tgt_s[2]*tgt_s[2]);
                    tgt_s[0]/=norme;
                    tgt_s[2]/=norme;
                }

                binormales_sommets[3*i]=nml_s[1]*tgt_s[2]-nml_s[2]*tgt_s[1];
                binormales_sommets[3*i+1]=nml_s[2]*tgt_s[0]-nml_s[0]*tgt_s[2];
                binormales_sommets[3*i+2]=nml_s[0]*tgt_s[1]-nml_s[1]*tgt_s[0];
            }
        }

        //--------------------------------------------------------------------------------------
        //              Calcul du centre géométrique relatif du volume
        //--------------------------------------------------------------------------------------
        void calcul_centre_geometrique_relatif()
        {
            uint32_t i;
            double cx=0;
            double cy=0;
            double cz=0;
            for (i=0;i<nbrSommets*3;)
            {
                cx+= sommets[i++];
                cy+= sommets[i++];
                cz+= sommets[i++];
            }

            centre_geometrique_relatif[0] = cx/(double)nbrSommets;
            centre_geometrique_relatif[1] = cy/(double)nbrSommets;
            centre_geometrique_relatif[2] = cz/(double)nbrSommets;
        }

        //=============================================================================================================================
        //                      Initialisation des faces
        /// Les surfaces doivent être déterminées après les faces.
        //=============================================================================================================================
        bool initialise_faces()
        {

            uint32_t i;
            for (i=0;i<nbrFaces;i++)
            {

                faces[i]->indice=i;
                faces[i]->volume=this;
                calcul_centre_gravite_relatif_face(faces[i]);
                calcul_normale_face(faces[i]);
                calcul_tangente_binormale_face(faces[i]);
            }

            initialise_liaisons_sommets_faces();
            calcul_normales_sommets();
            calcul_tangentes_binormales_sommets();
            ///Les liens avec les surfaces sont créés lors de la détermination des surfaces
            //if (!initialise_liens_faces_surfaces()) return false;
            initialise_faces_transparentes();

            return creation_aretes();
        }

        //----------------------------------------------------------------
        //  A appeler lorsque les sommets subissent des transformations
        //----------------------------------------------------------------
        void maj_faces()
        {
            for (uint32_t i=0;i<nbrFaces;i++)
            {
                calcul_centre_gravite_relatif_face(faces[i]);
                calcul_normale_face(faces[i]);
                calcul_tangente_binormale_face(faces[i]);
            }
            calcul_normales_sommets();
            calcul_tangentes_binormales_sommets();
        }

        //---------------------------------------------------------
        //Initialise le tableau de tri des faces transparentes
        //Les faces transparentes étant triées, elle ne peuvent pas
        //être affichées par surface. Il faut une table spécifique.
        //---------------------------------------------------------

        void initialise_faces_transparentes()
        {
            uint32_t i;
            indices_faces_transparentes.clear();
            nbrFaces_transparentes=0;

            for (i=0;i<nbrFaces;i++)
            {
                if (faces[i]->surface!=NULL && faces[i]->surface->drapeau_transparence)
                {
                    tri_face tri;
                    indices_faces_transparentes.push_back(tri);
                    indices_faces_transparentes[nbrFaces_transparentes].face=faces[i];
                    indices_faces_transparentes[nbrFaces_transparentes].distance=0.;
                    nbrFaces_transparentes++;
                }

            }

            if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_FACES_TRANSPARENTES)
            {
                cout<<"Nombre de faces transparentes: "<<nbrFaces_transparentes<<endl;
            }
        }

        //---------------------------------------------------
        //      Calcule les normales relatives des faces
        //---------------------------------------------------

        void calcul_normale_face(dksFace* face)
        {
            GLdouble* nml=face->normale;

            uint32_t at0 = face->aretes[0] * 3;
            uint32_t at1 = face->aretes[1] * 3;
            uint32_t at2 = face->aretes[2] * 3;

            GLdouble v1x = sommets[at1] - sommets[at0];
            GLdouble v1y = sommets[at1 + 1] - sommets[at0 + 1];
            GLdouble v1z = sommets[at1 + 2] - sommets[at0 + 2];

            GLdouble v2x = sommets[at2] - sommets[at1];
            GLdouble v2y = sommets[at2 + 1] - sommets[at1 + 1];
            GLdouble v2z = sommets[at2 + 2] - sommets[at1 + 2];

            if (sens_polygones==GL_CCW)
            {
                nml[0]=v1y*v2z-v1z*v2y;
                nml[1]=v1z*v2x-v1x*v2z;
                nml[2]=v1x*v2y-v1y*v2x;
            }
            else
            {
                nml[0]=-(v1y*v2z-v1z*v2y);
                nml[1]=-(v1z*v2x-v1x*v2z);
                nml[2]=-(v1x*v2y-v1y*v2x);
            }

            GLdouble norme_nml=sqrt(nml[0]*nml[0]+nml[1]*nml[1]+nml[2]*nml[2]);
            nml[0]/=norme_nml;
            nml[1]/=norme_nml;
            nml[2]/=norme_nml;
        }

        //--------------------------------------------------------
        //  Versions optimisées, pour les calculs lors des rendus:
        //--------------------------------------------------------
        void calcul_normales_faces_CCW(uint32_t indice_depart=0, uint32_t nbr=0)
        {
            uint32_t i,at0,at1,at2;
            GLdouble v1x,v1y,v1z;
            GLdouble v2x,v2y,v2z;
            GLdouble nx,ny,nz;
            GLdouble norme_nml;
            if(nbr==0 && indice_depart==0)nbr=nbrFaces;
            else nbr+=indice_depart;

            for (i=indice_depart;i<nbr;i++)
            {
                dksFace* face=faces[i];
                GLdouble* nml=face->normale;

                at0 = face->aretes[0] * 3;
                at1 = face->aretes[1] * 3;
                at2 = face->aretes[2] * 3;

                v1x = sommets[at1] - sommets[at0];
                v1y = sommets[at1 + 1] - sommets[at0 + 1];
                v1z = sommets[at1 + 2] - sommets[at0 + 2];

                v2x = sommets[at2] - sommets[at1];
                v2y = sommets[at2 + 1] - sommets[at1 + 1];
                v2z = sommets[at2 + 2] - sommets[at1 + 2];

                nx=v1y*v2z-v1z*v2y;
                ny=v1z*v2x-v1x*v2z;
                nz=v1x*v2y-v1y*v2x;

                norme_nml=sqrt(nx*nx+ny*ny+nz*nz);
                nml[0]=nx/norme_nml;
                nml[1]=ny/norme_nml;
                nml[2]=nz/norme_nml;
            }

        }

        void calcul_normales_faces_CW(uint32_t indice_depart=0, uint32_t nbr=0)
        {
            uint32_t i,at0,at1,at2;
            GLdouble v1x,v1y,v1z;
            GLdouble v2x,v2y,v2z;
            GLdouble nx,ny,nz;
            GLdouble norme_nml;
            if(nbr==0 && indice_depart==0)nbr=nbrFaces;
            else nbr+=indice_depart;

            for (i=indice_depart;i<nbr;i++)
            {
                dksFace* face=faces[i];
                GLdouble* nml=face->normale;

                at0 = face->aretes[0] * 3;
                at1 = face->aretes[1] * 3;
                at2 = face->aretes[2] * 3;

                v1x = sommets[at1] - sommets[at0];
                v1y = sommets[at1 + 1] - sommets[at0 + 1];
                v1z = sommets[at1 + 2] - sommets[at0 + 2];

                v2x = sommets[at2] - sommets[at1];
                v2y = sommets[at2 + 1] - sommets[at1 + 1];
                v2z = sommets[at2 + 2] - sommets[at1 + 2];

                nx=v1y*v2z-v1z*v2y;
                ny=v1z*v2x-v1x*v2z;
                nz=v1x*v2y-v1y*v2x;

                norme_nml=sqrt(nx*nx+ny*ny+nz*nz);
                nml[0]=-(v1y*v2z-v1z*v2y);
                nml[1]=-(v1z*v2x-v1x*v2z);
                nml[2]=-(v1x*v2y-v1y*v2x);
            }

        }


        //------------------------------------------------------------------------
        // Calcul les normales relatives des sommets des faces
        // Ces normales ne servent que si l'adoucissement de la surface
        // à laquelle est liée la face est activé.
        /// Cette fonction est appelée par l'initialisateur des surfaces,
        /// car les normales des sommets des faces sont calculées
        /// en fonction de l'angle limite d'adoucissement des surfaces.
        //------------------------------------------------------------------------
        void calcule_normales_sommets_faces()
        {
            uint32_t i,j,k,indice_sommet;
            double cos_angle,angle;
            liaison_faces* lien;
            GLdouble* nrm;
            double norme;

            for (i=0;i<nbrFaces;i++)
            {
                dksFace* face=faces[i];
                dksSurface* surface=face->surface;
                //if (!surface->drapeau_adoucissement) continue;
                for (j=0;j< face->nbrSommets;j++)
                {
                    indice_sommet=face->aretes[j];
                    lien=&liaisons_sommets_faces[indice_sommet];
                    nrm=&face->normales_sommets[j*3];
                    nrm[0]=face->normale[0];
                    nrm[1]=face->normale[1];
                    nrm[2]=face->normale[2];

                    for (k=0;k<lien->indices_faces.size();k++)
                    {
                        dksFace* face_B=faces[lien->indices_faces[k]];
                        if (face_B != face)
                        {
                            //Calcul l'angle avec une face adjacente:
                            cos_angle=
                                        face->normale[0]*face_B->normale[0]+
                                        face->normale[1]*face_B->normale[1]+
                                        face->normale[2]*face_B->normale[2];
                            angle=acosf(cos_angle);


                            //cout<<"Angle entre la face "<<face->indice<<" et la face "<<face_B->indice<<" : "<<angle<<" / "<<face->surface->angle_limite_adoucissement<<endl;

                            //Si l'angle limite n'est pas atteint, calcul la normale des sommets de l'arête:
                            if (surface->angle_limite_adoucissement > angle)
                            {
                                nrm[0]+=face_B->normale[0];
                                nrm[1]+=face_B->normale[1];
                                nrm[2]+=face_B->normale[2];
                            }
                            else face->drapeau_partiellement_adoucie=true;
                        }
                    }
                    norme=sqrt(nrm[0]*nrm[0]+nrm[1]*nrm[1]+nrm[2]*nrm[2]);
                    nrm[0]/=norme;
                    nrm[1]/=norme;
                    nrm[2]/=norme;

                }

                if (face->drapeau_partiellement_adoucie)
                {
                    /*
                    if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_SURFACES)
                    {
                        cout<<"                Surface: "<<surface->nom<<" - Face "<<face->indice<<" partiellement adoucie"<<endl;
                    }
                    */
                    surface->ajoute_face_partiellement_adoucie(face);
                }
                else surface->ajoute_face_adoucie(face);
            }
        }

        //---------------------------------------------------
        //      Calcule les tangentes et binormales de la face
        //---------------------------------------------------

        void calcul_tangente_binormale_face(dksFace* face)
        {
            //Calcul de la tangente:
            // ->  ->  ->
            // Y ^ N = T
            GLdouble* tgt=face->tangente;
            GLdouble* bnm=face->binormale;
            tgt[1]=0.;
            GLdouble* nml=face->normale;
            if (nml[1] == 1.)
            {
                tgt[0]=1.;
                tgt[2]=0.;
            }
            else if (nml[1] == -1.)
            {
                tgt[0]=-1.;
                tgt[2]=0.;
            }
            else
            {
                tgt[0]=nml[2];  //Simplification d'un produit vectoriel entre la normale et l'axe vertical (0,1,0)
                tgt[2]=-nml[0];
                GLdouble norme=sqrt(tgt[0]*tgt[0]+tgt[2]*tgt[2]);
                tgt[0]/=norme;
                tgt[2]/=norme;
            }

            bnm[0]=nml[1]*tgt[2]-nml[2]*tgt[1];
            bnm[1]=nml[2]*tgt[0]-nml[0]*tgt[2];
            bnm[2]=nml[0]*tgt[1]-nml[1]*tgt[0];

        }

        //---------------------------------------------------------
        // Calcul le centre de gravité relatif d'une face
        //---------------------------------------------------------
        void calcul_centre_gravite_relatif_face(dksFace* face)
        {
            uint32_t indice;
            uint16_t j;
            double nbr,cx,cy,cz;

                cx=0.;
                cy=0.;
                cz=0.;


            for (j=0;j<face->nbrSommets;j++)
            {
                indice=face->aretes[j]*3;
                cx+=sommets[indice];
                cy+=sommets[indice+1];
                cz+=sommets[indice+2];
            }
            nbr=face->nbrSommets;
            face->centre_gravite_relatif[0]=cx/nbr;
            face->centre_gravite_relatif[1]=cy/nbr;
            face->centre_gravite_relatif[2]=cz/nbr;
        }

        //--------------------------------------------------------------------------------------
        //             Création de la table de correspondance entre les sommets et les faces
        //--------------------------------------------------------------------------------------
        void initialise_liaisons_sommets_faces()
        {
            uint32_t i,j,k;

            for (i=0;i<nbrSommets;i++)
            {
                ajoute_liaison_sommet();
            }

            for (j=0;j<nbrFaces;j++)
            {
                uint32_t* at=faces[j]->aretes;
                for (k=0;k<faces[j]->nbrSommets;k++)
                {
                        liaisons_sommets_faces[at[k]].indices_faces.push_back(j);
                }
            }
        }

        void ajoute_liaison_sommet()
        {
            liaison_faces liaison;
            liaisons_sommets_faces.push_back(liaison);
        }

        //=========================================================================================
        //                          Gestion des arêtes
        //=========================================================================================

        //--------------------------------------------------------------------------------------
        //              Création de la table des arêtes à partir des faces:
        //--------------------------------------------------------------------------------------

        bool creation_aretes()
		{
            uint32_t n,i,j,k,sommetA,sommetB;
            supprime_aretes();

			for (i = 0; i < nbrFaces; i++)
			{
			    n=faces[i]->nbrSommets;
				for (j = 0; j < n; j++ )
				{
					sommetA = faces[i]->aretes[j];
					sommetB = faces[i]->aretes[(j + 1) % n ];

					for (k = 0; k < nbrAretes*2; )
					{
						if (((aretes[k] == sommetA) && (aretes[k + 1] == sommetB)) || ((aretes[k] == sommetB) && (aretes[k + 1] == sommetA)))
						{break; }
						k += 2;
					}
					if (k == nbrAretes*2)
					{
						aretes.push_back(sommetA);
						aretes.push_back(sommetB);
						nbrAretes++;
					}
				}
			}
			if (DKSVOLUME_DEBUG) cout<<"---------------------------------------------------------------Nombre d'arêtes:"<<nom<<" - "<<nbrAretes<<endl;

			return true;
		}



        //=============================================================================================================================
        //                      Initialisation des surfaces
        //  Initialise les pointeur de surface des faces, à partir du nom des surfaces spécifiés dans les faces,
        //  les table de faces des surfaces.
        ///  Les faces doivent avoir été déterminées en amont.
        //=============================================================================================================================
        bool initialise_surfaces()
        {
            uint32_t i,j;
            dksFace* face;
            dksSurface* surface;

            //Initialise les paramètres des surfaces dépendants du volume:
            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                surface->volume=this;
                if (surface->transparence>0.) surface->drapeau_transparence=true;
                if(!surface->alloue_tableau_normales_sommets(nbrSommets))
                {
                    cout<<"ERREUR dans dksVolume::initialise_surfaces() - Mémoire insuffisante pour allouer le tableau des sommets"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return false;
                }
            }
            if (!initialise_liens_faces_surfaces()) return false;
            initialise_indices_sommets_surfaces();
            calcule_normales_sommets_faces();
            calcule_normales_sommets_surfaces();
            initialise_faces_transparentes();
            return true;
        }

        //---------- Initialise les pointeurs de surfaces des faces.
        bool initialise_liens_faces_surfaces()
        {
            uint32_t i,j;
            dksSurface* surface;
            dksFace* face;

            //Lie les faces aux surfaces:
            for (i=0;i<nbrFaces;i++)
            {
                face=faces[i];
                surface=renvoie_surface_nom(face->nom_surface);
                if (surface==NULL)
                {
                    cout<<"Erreur dans dksVolume.initialise_surfaces() - Volume: "<<nom<<" - La face "<<i<<" est liée à une surface inconnue: "<<face->nom_surface<<endl;
                    if(DKSVOLUME_DEBUG)
                    {
                        cout<<"Nombre de faces:"<<nbrFaces<<endl;
                        cout<<"Surfaces présentes dans le volume:"<<endl;
                        for (i=0;i<nbrSurfaces;i++) cout<<surfaces[i]->nom<<endl;
                    }
                    erreur=DKSVOLUME_ERREUR_LIEN_FACE_SURFACE;
                    return false;
                }
                face->surface=surface;
                surface->faces.push_back(face);
                surface->nbr_faces=surface->faces.size();
            }

            //Affiche les tables de faces des surfaces:
            if(DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_SURFACES)
            {
                for (i=0;i<nbrSurfaces;i++)
                {
                    surface=surfaces[i];
                    cout<<"Surface: "<<surface->nom<<endl;
                    for (j=0;j<surface->faces.size();j++)
                    {
                        cout<<"     Face: "<<surface->faces[j]->indice<<endl;
                    }
                }
            }

            return true;
        }

        //-------Création des tables d'indices de sommets des surfaces:
        void initialise_indices_sommets_surfaces()
        {
            uint32_t i,j;
            liaison_faces* lien;
            dksSurface* surface;
            for (i=0;i<nbrSommets;i++)
            {
                lien=&liaisons_sommets_faces[i];
                for(j=0;j<lien->indices_faces.size();j++)
                {
                    surface=faces[lien->indices_faces[j]]->surface;
                    if (surface->indices_sommets.size()==0 || surface->indices_sommets[surface->indices_sommets.size()-1]!=i) surface->indices_sommets.push_back(i);
                }
            }
        }


        //-----------------------------------------------
        //  Calcul les normales des sommets des surfaces.
        //  Ces normales servent pour le rendu par l'utilisation des
        //  tableaux de pointeurs.
        //  En effet, dans ce mode, les angles-seuils d'adoucissement
        //  ne fonctionnent qu'aux sommetslimitrophes des surfaces. Un seuil par
        //  face serait trop couteux en initialisations et rendrait les
        //  tableaux inutiles.
        //-----------------------------------------------
        void calcule_normales_sommets_surfaces()
        {
            uint32_t i,j,k,l;
            liaison_faces* lien;
            dksFace* face;

            bool sommets_limitrophes[nbrSommets];

            for(i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                //Copiage des normales des sommets du volume
                for (j=0;j<nbrSommets;j++)
                {
                    surface->normales_sommets[3*j]=normales_sommets[3*j];
                    surface->normales_sommets[3*j+1]=normales_sommets[3*j+1];
                    surface->normales_sommets[3*j+2]=normales_sommets[3*j+2];
                    sommets_limitrophes[j]=false;
                }

                //Recherche les sommets limitrophes

                for(j=0;j<surface->faces_partiellement_adoucies.size();j++)
                {
                    face=surface->faces_partiellement_adoucies[j];
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        lien=&liaisons_sommets_faces[face->aretes[k]];

                        for(l=0;l<lien->indices_faces.size();l++)
                        {
                            dksFace* face_b=faces[lien->indices_faces[l]];
                            if (face!=face_b && face_b->surface!=surface)
                            {
                                sommets_limitrophes[face->aretes[k]]=true;
                                break;
                            }
                        }
                    }
                }

                //-------Calcul les normales aux sommets limitrophes:
                vector<dksFace*> faces_exterieures;
                double nrm[3],nrm_ext[3];
                double cptr,cos_angle,angle;


                for (j=0;j<nbrSommets;j++)
                {
                    if (sommets_limitrophes[j])
                    {
                        //D'abord on calcul la normale moyenne des faces DE LA SURFACE adjentes au sommet
                        lien=&liaisons_sommets_faces[j];
                        nrm={0.,0.,0.};
                        faces_exterieures.clear();
                        cptr=0.;
                        for (k=0;k<lien->indices_faces.size();k++)
                        {
                            face=faces[lien->indices_faces[k]];
                            //Si la face est dans la surface, on l'ajoute pour la moyenne des normales, sans tenir compte du seuil d'adoucissement
                            if(face->surface==surface)
                            {
                                nrm[0]+=face->normale[0];
                                nrm[1]+=face->normale[1];
                                nrm[2]+=face->normale[2];
                                cptr++;
                            }
                            //Si la face n'est pas dans la surface, on la met de côté, pour vérifier son seuil
                            //avec la normale moyenne des faces incluses dans la surface à ce sommet.
                            else
                            {
                                faces_exterieures.push_back(face);
                            }
                        }
                        //Normale des faces incluses dans la surface à ce sommet:
                        nrm[0]/=cptr;
                        nrm[1]/=cptr;
                        nrm[2]/=cptr;
                        //Test le seuil d'adoucissement avec les faces adjacentes au sommet, extérieures à la surface:
                        cptr=1.;
                        nrm_ext={nrm[0],nrm[1],nrm[2]}; //Pour que le seuil d'adoucissement soit toujours comparé à la normale de la surface.
                        for(k=0;k<faces_exterieures.size();k++)
                        {
                            face=faces_exterieures[k];
                            //Calcul l'angle avec la normale moyenne:
                            cos_angle=
                                        face->normale[0]*nrm[0]+
                                        face->normale[1]*nrm[1]+
                                        face->normale[2]*nrm[2];
                            angle=acosf(cos_angle);

                            //Si l'angle limite n'est pas atteint, calcul la normale des sommets de l'arête:
                            if (surface->angle_limite_adoucissement > angle)
                            {
                                nrm_ext[0]+=face->normale[0];
                                nrm_ext[1]+=face->normale[1];
                                nrm_ext[2]+=face->normale[2];
                                cptr++;
                            }
                        }
                        nrm_ext[0]/=cptr;
                        nrm_ext[1]/=cptr;
                        nrm_ext[2]/=cptr;
                        //Stockage de la normale:
                        surface->normales_sommets[j*3]=nrm_ext[0];
                        surface->normales_sommets[j*3+1]=nrm_ext[1];
                        surface->normales_sommets[j*3+2]=nrm_ext[2];
                    }
                }
            }
        }

        //-----------------------------------------------------
        //      Initialise les modes de rendu des surfaces
        //-----------------------------------------------------
        void initialise_modes_rendus_surfaces()
        {
            for (uint32_t i=0 ;i<nbrSurfaces;i++) surfaces[i]->initialise_mode_rendu();
        }

        //=============================================================================================================================
        //                      Initialisation des textures
        //          Les liens entre les faces et les sommets ont été créés.
        //          Allocation et initialisation des tableaux de coordonnées UV, si il y en a.
        //=============================================================================================================================
        bool initialise_textures()
        {
            uint32_t i,j;
            dksSurface* surface;
            dksTexture* texture;
            vector<uint8_t> faces_utilisees; //Sert à mémoriser le status des faces incluses,adjacentes ou hors de la texture, pour les calculs de repères TBN

            if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_TEXTURES) cout<<endl<<"**** Initialisation des textures du volume: "<<nom<<endl<<endl;

            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_TEXTURES) cout<<" "<<surface->nom<<" ";

                //Ambiance
                for (j=0;j<surface->textures_ambiance.size();j++)
                {
                    texture=surface->textures_ambiance[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_ambiance_inactives.size();j++)
                {
                    texture=surface->textures_ambiance_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                //Couleur
                for (j=0;j<surface->textures_couleur.size();j++)
                {
                    texture=surface->textures_couleur[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_couleur_inactives.size();j++)
                {
                    texture=surface->textures_couleur_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                //Diffusion
                for (j=0;j<surface->textures_diffusion.size();j++)
                {
                    texture=surface->textures_diffusion[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_diffusion_inactives.size();j++)
                {
                    texture=surface->textures_diffusion_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                //Luminescence
                for (j=0;j<surface->textures_luminescence.size();j++)
                {
                    texture=surface->textures_luminescence[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_luminescence_inactives.size();j++)
                {
                    texture=surface->textures_luminescence_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                //Réflexion
                for (j=0;j<surface->textures_reflexion.size();j++)
                {
                    texture=surface->textures_reflexion[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_reflexion_inactives.size();j++)
                {
                    texture=surface->textures_reflexion_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                //Rugosite
                for (j=0;j<surface->textures_rugosite.size();j++)
                {
                    texture=surface->textures_rugosite[j];
                    if (!creation_table_faces_texture(texture,faces_utilisees)) return false;
                    if (!creation_tableau_UV(texture)) return false;
                    if (!creation_tangentes_binormales(texture,faces_utilisees)) return false;
                }
                for (j=0;j<surface->textures_rugosite_inactives.size();j++)
                {
                    texture=surface->textures_rugosite_inactives[j];
                    if (!creation_table_faces_texture(texture,faces_utilisees)) return false;
                    if (!creation_tableau_UV(texture)) return false;
                    if (!creation_tangentes_binormales(texture,faces_utilisees)) return false;
                }

                //Transparence:
                for (j=0;j<surface->textures_transparence.size();j++)
                {
                    texture=surface->textures_transparence[j];
                    if (!creation_tableau_UV(texture)) return false;
                }
                for (j=0;j<surface->textures_transparence_inactives.size();j++)
                {
                    texture=surface->textures_transparence_inactives[j];
                    if (!creation_tableau_UV(texture)) return false;
                }

                #if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_TEXTURES)

                    cout<<" *** Surface:"<<surface->nom<<endl;
                    cout<<"            Nombre de textures:"<<endl;
                    if (surface->textures_ambiance.size()>0)    cout<<"                  Ambiance:"<<surface->textures_ambiance.size()<<endl;
                    if (surface->textures_ambiance_inactives.size()>0)    cout<<"                  -inactives:"<<surface->textures_ambiance.size()<<endl;
                    if (surface->textures_couleur.size()>0)     cout<<"                   Couleur:"<<surface->textures_couleur.size()<<endl;
                    if (surface->textures_couleur_inactives.size()>0)     cout<<"                   -inactives:"<<surface->textures_couleur.size()<<endl;
                    if (surface->textures_diffusion.size()>0)   cout<<"                 Diffusion:"<<surface->textures_diffusion.size()<<endl;
                    if (surface->textures_diffusion_inactives.size()>0)   cout<<"                 -inactives:"<<surface->textures_diffusion.size()<<endl;
                    if (surface->textures_luminescence.size()>0) cout<<"              Luminescence:"<<surface->textures_luminescence.size()<<endl;
                    if (surface->textures_luminescence_inactives.size()>0) cout<<"              -inactives:"<<surface->textures_luminescence.size()<<endl;
                    if (surface->textures_reflexion.size()>0)   cout<<"                 Réflexion:"<<surface->textures_reflexion.size()<<endl;
                    if (surface->textures_reflexion_inactives.size()>0)   cout<<"                 -inactives:"<<surface->textures_reflexion.size()<<endl;
                    if (surface->textures_rugosite.size()>0)    cout<<"                  Rugosité:"<<surface->textures_rugosite.size()<<endl;
                    if (surface->textures_rugosite_inactives.size()>0)    cout<<"                  -inactives:"<<surface->textures_rugosite.size()<<endl;
                    if (surface->textures_transparence.size()>0)cout<<"              Transparence:"<<surface->textures_transparence.size()<<endl;
                    if (surface->textures_transparence_inactives.size()>0)cout<<"              -inactives:"<<surface->textures_transparence.size()<<endl;
                #endif
            }

            return true;
        }

        //------------------- Création de la table des faces affectées par la texture
        ///Pour le moment, cette table n'est créée que pour les textures de rugosité

        bool creation_table_faces_texture(dksTexture* texture, vector<uint8_t> &faces_utilisees)
        {
            uint32_t i,j,k,l,indice_sommet;
            dksFace* face;
            faces_utilisees.clear();
            //Libère la mémoire de l'ancienne table:
            if (texture->faces!=NULL) free(texture->faces);
            texture->nbrFaces=0;
            //Comptage des faces:
            for(i=0;i<nbrFaces;i++) faces_utilisees.push_back(0);

            for (i=0;i<texture->coordonnees_UV.size();i++)
            {
                indice_sommet=texture->coordonnees_UV[i].indice_sommet;
                liaison_faces* lien=&liaisons_sommets_faces[indice_sommet];
                for(j=0;j<lien->indices_faces.size();j++)
                {

                    if(faces_utilisees[lien->indices_faces[j]]==0)
                    {
                        //détermination des faces frontalières adjacentes (c'est à dire celles dont au moins un sommet n'a pas de coordonnées UV):
                        dksFace* face=faces[lien->indices_faces[j]];
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            for (l=0;l<texture->coordonnees_UV.size();l++)
                            {
                                if (face->aretes[k]==texture->coordonnees_UV[l].indice_sommet) break;
                            }
                            if(l==texture->coordonnees_UV.size())
                            {
                                faces_utilisees[lien->indices_faces[j]]=DKSVOLUME_FACE_FRONTALIERE; //Sont considérées comme frontalière TOUTES les faces ayant AU MOINS 1 sommet
                                                                                                    //non-inclus dans la table UV.
                                break;
                            }
                        }
                        if (k==face->nbrSommets) faces_utilisees[lien->indices_faces[j]]=DKSVOLUME_FACE_INCLUSE;
                    }
                }
            }

            for(i=0;i<nbrFaces;i++) if (faces_utilisees[i]==DKSVOLUME_FACE_INCLUSE) texture->nbrFaces++;
            // Allocation de la table:
            texture->faces=(uint32_t*)malloc(texture->nbrFaces*sizeof(uint32_t));
            if (texture->faces==NULL)
            {
                cout<<"ERREUR dans dksVolume.creation_table_faces_texture() - Mémoire insuffisante"<<endl;
                erreur=DKSVOLUME_ERREUR_MEMOIRE;
                return false;
            }
            //Stockage des indexes des faces recouvertes par la texture:
            j=0;
            for(i=0;i<nbrFaces;i++) if (faces_utilisees[i]==DKSVOLUME_FACE_INCLUSE) texture->faces[j++]=i;

            return true;
        }

        //---------------- Crée une table de coordonnées correspondantes à la texture
        bool creation_tableau_UV(dksTexture* texture)
        {
            uint32_t i,indice;
            GLfloat* tableau_UV;

            if (texture->type_projection==DKSTEXTURE_PROJECTION_UV)
            {
                //Libère l'ancien tableau, le cas échéant:
                if (texture->coordonnees_UV_tableau!=NULL) free(texture->coordonnees_UV_tableau);

                //Alloue le nouveau tableau:
                texture->coordonnees_UV_tableau=(GLfloat*)malloc(nbrSommets*sizeof(GLfloat)*2);
                if (texture->coordonnees_UV_tableau==NULL)
                {
                    cout<<"Erreur dans dksVolume::allocation_tableau_UV() - Mémoire insuffisante"<<endl;
                    erreur=DKSVOLUME_ERREUR_MEMOIRE;
                    return false;
                }
                //Initialise le tableau:
                tableau_UV=texture->coordonnees_UV_tableau;

                for (i=0;i<nbrSommets;i++)
                {
                    tableau_UV[2*i]=0.;
                    tableau_UV[2*i+1]=0.;
                }

                for (i=0;i<texture->nbrSommets;i++)
                {
                    indice=texture->coordonnees_UV[i].indice_sommet;
                    if (DKSVOLUME_DEBUG && DKSVOLUME_DEBUG_TEXTURES) cout<<indice<<endl;
                    tableau_UV[indice*2]=texture->coordonnees_UV[i].U;
                    tableau_UV[indice*2+1]=texture->coordonnees_UV[i].V;
                }

                return true;
            }
            return true;
        }


        //-------------- Calcul les tangentes et les binormales des faces et des sommets
        /// Les textures sont considérées comme n'étant pas ou peu déformées sur la face !!!

        //Seules les textures UV sont concernées.
        bool creation_tangentes_binormales(dksTexture* texture,vector<uint8_t>& faces_utilisees)
        {
            uint32_t i,j,indice_sommet,indice_face;
            GLfloat* tableau_UV;
            dksFace* face;
            dksVecteur tempon_tangente;
            liaison_faces* lien;

            if (!texture->type_projection==DKSTEXTURE_PROJECTION_UV)
            {
                cout<<"ERREUR dans dksVolume::creation_tengantes_binormales() - La texture n'est pas en projection UV!"<<endl;
                return false;
            }

            //Libère les anciens tableaux, le cas échéant:
            if (texture->tangentes_faces!=NULL) free(texture->tangentes_faces);
            if (texture->binormales_faces!=NULL) free(texture->binormales_faces);
            if (texture->tangentes_sommets!=NULL) free(texture->tangentes_sommets);
            if (texture->binormales_sommets!=NULL) free(texture->binormales_sommets);

            //Alloue les nouveaux tableaux:
            texture->tangentes_faces=(GLdouble*)malloc(nbrFaces*sizeof(GLdouble)*3);
            texture->binormales_faces=(GLdouble*)malloc(nbrFaces*sizeof(GLdouble)*3);
            texture->tangentes_sommets=(GLdouble*)malloc(nbrSommets*sizeof(GLdouble)*3);
            texture->binormales_sommets=(GLdouble*)malloc(nbrSommets*sizeof(GLdouble)*3);

            if (texture->tangentes_faces==NULL || texture->binormales_faces==NULL || texture->tangentes_sommets==NULL || texture->binormales_sommets==NULL)
            {
                cout<<"ERREUR dans dksVolume::creation_tengantes_binormales() - Mémoire insuffisante"<<endl;
                erreur=DKSVOLUME_ERREUR_MEMOIRE;
                return false;
            }

            //Initialise les tableaux:
            //Les repères TBN par défaut du volume sont utilisés:
            //  C'est plus propre dans le cas ou la texture ne couvre pas toute la surface
            tableau_UV=texture->coordonnees_UV_tableau;

            for (i=0;i<nbrFaces;i++)
            {
               texture->tangentes_faces[3*i]=faces[i]->tangente[0];
               texture->tangentes_faces[3*i+1]=faces[i]->tangente[1];
               texture->tangentes_faces[3*i+2]=faces[i]->tangente[2];
               texture->binormales_faces[3*i]=faces[i]->binormale[0];
               texture->binormales_faces[3*i+1]=faces[i]->binormale[1];
               texture->binormales_faces[3*i+2]=faces[i]->binormale[2];
            }
            for (i=0;i<nbrSommets;i++)
            {
               texture->tangentes_sommets[3*i]=tangentes_sommets[3*i];
               texture->tangentes_sommets[3*i+1]=tangentes_sommets[3*i+1];
               texture->tangentes_sommets[3*i+2]=tangentes_sommets[3*i+2];
               texture->binormales_sommets[3*i]=binormales_sommets[3*i];
               texture->binormales_sommets[3*i+1]=binormales_sommets[3*i+1];
               texture->binormales_sommets[3*i+2]=binormales_sommets[3*i+2];
            }


            //Calcul des repères TBN des faces, alignés à la texture:
            for (i=0 ; i<texture->nbrFaces ;i++)
            {
                indice_face=texture->faces[i];
                face=faces[indice_face];
                //Vecteur UV de la première arête de la face:
                GLdouble Au=texture->coordonnees_UV_tableau[face->aretes[0]*2];
                GLdouble Av=texture->coordonnees_UV_tableau[face->aretes[0]*2+1];
                GLdouble Bu=texture->coordonnees_UV_tableau[face->aretes[1]*2];
                GLdouble Bv=texture->coordonnees_UV_tableau[face->aretes[1]*2+1];
                GLdouble Cu=Bu-Au;
                GLdouble Cv=Bv-Av;
                //Normalisation:
                GLdouble norme=sqrt(Cu*Cu+Cv*Cv);
                Cu/=norme;
                Cv/=norme;
                //Angle d'inclinaison de l'arête par rapport à l'axe des U:
                GLdouble angle=acos(Cu);
                if (Cv<0.)angle=-angle;
                //Vecteur spatial de l'arête:
                GLdouble Apx=sommets[face->aretes[0]*3];
                GLdouble Apy=sommets[face->aretes[0]*3+1];
                GLdouble Apz=sommets[face->aretes[0]*3+2];
                GLdouble Bpx=sommets[face->aretes[1]*3];
                GLdouble Bpy=sommets[face->aretes[1]*3+1];
                GLdouble Bpz=sommets[face->aretes[1]*3+2];
                GLdouble Cpx=Bpx-Apx;
                GLdouble Cpy=Bpy-Apy;
                GLdouble Cpz=Bpz-Apz;
                //Normalisation:
                norme=sqrt(Cpx*Cpx+Cpy*Cpy+Cpz*Cpz);
                Cpx/=norme;
                Cpy/=norme;
                Cpz/=norme;
                //Rotation:
                tempon_tangente.x=Cpx;
                tempon_tangente.y=Cpy;
                tempon_tangente.z=Cpz;
                tempon_tangente.rotation(-angle,face->normale[0],face->normale[1],face->normale[2]);
                //Calcul de la binormale:
                texture->tangentes_faces[indice_face*3]=tempon_tangente.x;
                texture->tangentes_faces[indice_face*3+1]=tempon_tangente.y;
                texture->tangentes_faces[indice_face*3+2]=tempon_tangente.z;
                texture->binormales_faces[indice_face*3]=face->normale[1]*tempon_tangente.z - face->normale[2]*tempon_tangente.y;
                texture->binormales_faces[indice_face*3+1]=face->normale[2]*tempon_tangente.x - face->normale[0]*tempon_tangente.z;
                texture->binormales_faces[indice_face*3+2]=face->normale[0]*tempon_tangente.y - face->normale[1]*tempon_tangente.x;

            }

            //Calcul des repères TBN des sommets, par moyenne des faces incluses à la texture:
            ///Les seuils d'adoucissement des tangente et des binormales ne sont pas pris en compte pour les faces recouvertes par
            ///la texture. De plus les moyennes pour le calcul des composantes T,B sur les sommets ne tiennent pas compte des
            /// faces hors-texture. On obtient une tangente et une binormale approximative. Par contre la normale suit bien
            /// les adoucissements.
            for (i=0;i<texture->coordonnees_UV.size();i++)
            {
                indice_sommet=texture->coordonnees_UV[i].indice_sommet;
                GLdouble* tgt=&texture->tangentes_sommets[3*indice_sommet];
                GLdouble* bnm=&texture->binormales_sommets[3*indice_sommet];
                GLdouble* nml=&normales_sommets[3*indice_sommet];
                tgt[0]=0.;
                tgt[1]=0.;
                tgt[2]=0.;
                lien=&liaisons_sommets_faces[indice_sommet];
                GLdouble compteur_moyenne=0.;
                for(j=0;j<lien->indices_faces.size();j++)
                {
                    indice_face=lien->indices_faces[j];
                    if(faces_utilisees[indice_face]==DKSVOLUME_FACE_INCLUSE)
                    {
                        tgt[0]+=texture->tangentes_faces[3*indice_face];
                        tgt[1]+=texture->tangentes_faces[3*indice_face+1];
                        tgt[2]+=texture->tangentes_faces[3*indice_face+2];
                        compteur_moyenne++;
                    }
                }
                tgt[0]/=compteur_moyenne;
                tgt[1]/=compteur_moyenne;
                tgt[2]/=compteur_moyenne;
                bnm[0]=nml[1]*tgt[2] - nml[2]*tgt[1];
                bnm[1]=nml[2]*tgt[0] - nml[0]*tgt[2];
                bnm[2]=nml[0]*tgt[1] - nml[1]*tgt[0];
            }
            return true;
        }

        //=============================================================================================================================
        //                      Fonctions utiles
        //=============================================================================================================================
    public:
        //Renvoie une surface d'après son nom:
        //NULL si la surface n'existe pas.
        dksSurface* renvoie_surface_nom(char* nom_surface)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                if(strcmp(surfaces[i]->nom,nom_surface)==0) return surfaces[i];
            }
            return NULL;
        }

        dksSurface* renvoie_surface(uint32_t indice)
        {
            if (indice>nbrSurfaces) return NULL;
            return surfaces[indice];
        }

        //Renvoie la table des sommets relatifs du volume:
        double* renvoie_sommets()
        {
            return sommets;
        }

        //Renvoie la Face correspondant à l'indice:
        dksFace* renvoie_face(uint32_t indice)
        {
            if (indice>=nbrFaces) return NULL;
            return faces[indice];
        }

        //Renvoie la normale du sommet:
        double* renvoie_normale_relative_sommet(uint32_t indice)
        {
            if (indice>nbrSommets)return NULL;
            return &normales_sommets[indice*3];
        }


        //====================================================================================================================
        //                                              Gestion des couleurs
        //====================================================================================================================
        //Détermine les couleurs des surfaces
        void determine_couleur_ambiance(uint32_t couleur, float intensite=1.)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->determine_couleur_ambiance(couleur,intensite);
            }
        }

        void determine_couleur_diffusion(uint32_t couleur, float intensite=1.)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->determine_couleur_diffusion(couleur,intensite);
            }
        }

        void determine_couleur_specularite(uint32_t couleur, float intensite=1.)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->determine_couleur_speculaire(couleur,intensite);
            }
        }

        void determine_couleur_luminescence(uint32_t couleur, float intensite=1.)
        {
            for (uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->determine_couleur_luminescence(couleur,intensite);
            }
        }


        //Teinte les surfaces:
        void applique_teinte_diffusion(uint32_t teinte)
        {
            for(uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->applique_teinte_diffusion(teinte);
            }
        }

        void applique_teinte_specularite(uint32_t teinte)
        {
            for(uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->applique_teinte_specularite(teinte);
            }
        }

        void applique_teinte_luminescence(uint32_t teinte)
        {
            for(uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->applique_teinte_luminescence(teinte);
            }
        }

        void applique_teinte_ambiance(uint32_t teinte)
        {
            for(uint32_t i=0;i<nbrSurfaces;i++)
            {
                dksSurface* surface=surfaces[i];
                surface->applique_teinte_ambiance(teinte);
            }
        }



        //-----------------------------------------------
        //  Détermine le mode de rendu du volume.
        // Selon les performances souhaitées, on peut afficher le volume
        // dans les différents modes proposés par OpenGl.
        ///ATTENTION!!! Les listes de profondeur et de luminescence peuvent être rendue deux fois à l'init du volume
        ///(cf constructeur de copie....)
        //-----------------------------------------------

        virtual void determine_mode_rendu(uint8_t p_mode)
        {
            if (p_mode==mode_rendu) return;
            switch (p_mode)
            {
               case DKSVOLUME_MODE_IMMEDIAT:
                    //supprime_listes();
                    mode_rendu=p_mode;
                    break;

               case DKSVOLUME_MODE_TABLEAUX:
                    //supprime_listes();
                    mode_rendu=p_mode;
                    break;

               case DKSVOLUME_MODE_LISTE:
               case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
               case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                    mode_rendu=p_mode;
                    maj_liste_affichage();
                    break;
                default:
                    break;
            }
        }

        uint8_t renvoie_mode_rendu()
        {
            return mode_rendu;
        }

        //-----------------------------------------------------------------------------------
        //      Détermine si le volume doit afficher ses faces transparentes en interne.
        //      Lorsque le volume est inclus dans une scène, cette dernière prend le relais
        //      pour afficher correctement les faces transprantes.
        //-----------------------------------------------------------------------------------
        void determine_rendu_faces_transparentes(bool p_drapeau)
        {
            drapeau_rendu_faces_transparentes=p_drapeau;
        }

        bool renvoie_drapeau_rendu_faces_transparentes()
        {
            return drapeau_rendu_faces_transparentes;
        }

        //-----------------------------------------------------------------------------------
        //      Détermine si le volume est affecté par le brouillard
        //-----------------------------------------------------------------------------------
        void determine_affecte_par_brouillard(bool p_drapeau)
        {
            drapeau_affecte_par_brouillard=p_drapeau;
        }

        bool est_affecte_par_brouillard()
        {
            return drapeau_affecte_par_brouillard;
        }

        //-----------------------------------------------------------------------------------
        //      Détermine si les faces opaques affectent le tempon de profondeur
        //-----------------------------------------------------------------------------------
        void determine_affecte_tempon_profondeur(bool p_drapeau)
        {
            drapeau_tempon_profondeur=p_drapeau;
        }

        bool affecte_tempon_profondeur()
        {
            return drapeau_tempon_profondeur;
        }

        //-----------------------------------------------------------------------------------
        //      Détermine si le volume projete des ombres.
        //      Si le drapeau est false, le volume ne sera pas rendu lors de la passe de rendu
        //      de la texture d'ombrage.
        //-----------------------------------------------------------------------------------
        void determine_projete_ombres(bool p_drapeau)
        {
            drapeau_projete_ombres=p_drapeau;
            for(uint32_t i=0;i<nbrSurfaces;i++)surfaces[i]->determine_projete_ombres(p_drapeau);
        }

        bool projete_ombre()
        {
            return drapeau_projete_ombres;
        }

        //Gestion de l'affichage des ombres:
        ///En mode LISTE, le volume ne peut pas recevoir d'ombres.
        void determine_recoit_ombres(bool p_drapeau,GLuint p_texture=0,dksSourceLumineuse* p_source_des_ombres=NULL,GLfloat p_intensite_ombres=1.)
        {
            texture_ombres_id=p_texture;
            source_des_ombres=p_source_des_ombres;
            drapeau_recoit_ombres=p_drapeau;
            intensite_ombres=p_intensite_ombres;
            for(uint32_t i=0;i<nbrSurfaces;i++)surfaces[i]->determine_recoit_ombres(p_drapeau);
        }

        bool recoit_ombres()
        {
            return drapeau_recoit_ombres;
        }

        //L'angle maximum pour lequel on considère la face comme éclairée. Cela permet d'éviter les trop grandes distortion de la texture de profondeur sur les rasances.
        //L'angle est en degrés
        void determine_angle_seuil_ombres(float angle)
        {
            angle_max_ombres=angle;
        }

        float renvoie_angle_seuil_ombres()
        {
            return angle_max_ombres;
        }

        //------------------------------------------------------------------
        //  Détermine le sens dans lequel les polygones sont définis
        //  GL_CW: sens des aiguilles
        //  GL_CCW: sens inverse des aiguilles
        //-----------------------------------------------
public:
        void determine_sens_polygones(GLint p_sens)
        {
            sens_polygones=p_sens;
        }

        GLint renvoie_sens_polygones()
        {
            return sens_polygones;
        }

        //-----------------------------------------------------------------------
        //      Inversion du sens des polygones.
        //  Les normales du volume sont également inversées
        ///  Les tangentes et binormales des repères TNB ne sont pas inversées.
        //-----------------------------------------------------------------------
        void inverse_sens_polygones()
        {
            uint32_t i;
            if (sens_polygones==GL_CCW) sens_polygones=GL_CW;
            else sens_polygones==GL_CCW;
            //Inverse les normales des sommets:
            for (i=0;i<nbrSommets;i++)
            {
                normales_sommets[3*i]=-normales_sommets[3*i];
                normales_sommets[3*i+1]=-normales_sommets[3*i+1];
                normales_sommets[3*i+2]=-normales_sommets[3*i+2];
            }
            //Inverse les normales des faces:
            for (i=0;i<nbrFaces;i++) { faces[i]->inverse_normales(); }
            //Inverse les normales des surfaces:
            for (i=0;i<nbrSurfaces;i++) { surfaces[i]->inverse_normales(); }
        }

        //------------------------------------------------------------------
        //  Détermine le mode de remplissage: couleurs,aretes, sommets ou surfaces
        //------------------------------------------------------------------
        void determine_mode_remplissage(uint8_t p_mode)
        {
            mode_remplissage=p_mode;
        }

        //========================================================================================================================================
        //========================================================================================================================================
        //                                              Gestion des mouvements
        //========================================================================================================================================
        //========================================================================================================================================
public:
        virtual void a_ete_deplace()
		{
			a_change = true;
			volume_a_change=true;
		}


		virtual void determine_position(double px, double py, double pz)
		{
			if (contrainte_trajectoire) alignement_tangente_trajectoire(px,py,pz);
			x = px; y = py; z = pz;

			a_change = true;
			volume_a_change = true;
		}

		virtual void determine_position(double* position)
		{
            if (contrainte_trajectoire) alignement_tangente_trajectoire(position[0],position[1],position[2]);
			x =position[0]; y = position[1]; z = position[2];

			a_change = true;
			volume_a_change = true;
        }

        virtual void determine_position(float* position)
		{
            determine_position(double(position[0]),double(position[1]),double(position[2]));
        }

        virtual void determine_RTL(double p_roulis, double p_tangage, double p_lacet)
		{
            angle_roulis=p_roulis;
            angle_tangage=p_tangage;
            angle_lacet=p_lacet;
            maj_orientation_RTL();
            a_change = true;
			volume_a_change = true;
		}


		virtual void determine_echelle_repere(double ex, double ey, double ez)
		{
			echelle_x = ex; echelle_y = ey; echelle_z = ez;
			a_change = true;
			volume_a_change = true;
		}

		//=========================================================================================
        //                          Gestion des collisions
        //=========================================================================================
public:
        //Génère le pavé circonscrit au volume:
        //Renvoie true si la boîte a bien été créée.
        bool initialise_boite_collisions(double facteur_x=1.0,double facteur_y=1.0, double facteur_z=1.0)
        {
            if (nbrSommets==0) return true;
            double xmin=sommets[0];
            double xmax=xmin;
            double ymin=sommets[1];
            double ymax=ymin;
            double zmin=sommets[2];
            double zmax=zmin;
            for (uint32_t i=0;i<nbrSommets*3;)
            {
                double xt=sommets[i++];
                double yt=sommets[i++];
                double zt=sommets[i++];
                if (xt<xmin)xmin=xt; else if (xt>xmax)xmax=xt;
                if (yt<ymin)ymin=yt; else if (yt>ymax)ymax=yt;
                if (zt<zmin)zmin=zt; else if (zt>zmax)zmax=zt;
            }

            //Mise à l'échelle:
            xmin=(xmin-centre_geometrique_relatif[0])*facteur_x+centre_geometrique_relatif[0];
            xmax=(xmax-centre_geometrique_relatif[0])*facteur_x+centre_geometrique_relatif[0];

            ymin=(ymin-centre_geometrique_relatif[1])*facteur_y+centre_geometrique_relatif[1];
            ymax=(ymax-centre_geometrique_relatif[1])*facteur_y+centre_geometrique_relatif[1];

            zmin=(zmin-centre_geometrique_relatif[2])*facteur_z+centre_geometrique_relatif[2];
            zmax=(zmax-centre_geometrique_relatif[2])*facteur_z+centre_geometrique_relatif[2];

            //Création de la boite:

            boite_collision=new dksBoiteCollision(xmin,xmax,ymin,ymax,zmin,zmax);

            if (boite_collision==NULL)
            {
                cout<<"ERREUR dans Volume.initialise_boite_collision()- Impossible de créer la boîte de collision: "<<nom<<endl;
                return false;
            }
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_BOITE_COLLISION;
            return true;
        }

        //----Pour définir une boite de collision:
        void determine_boite_collision( dksBoiteCollision* p_boite)
        {
            if ( (drapeaux_liberation_memoire & DKSVOLUME_LIBERE_BOITE_COLLISION)!=0)
            {
                if (boite_collision!=NULL) delete boite_collision;
            }
            drapeaux_liberation_memoire&=-DKSVOLUME_LIBERE_BOITE_COLLISION-1;
            boite_collision=p_boite;
        }

        //-------- Rafraichi les limites de la boite de collision:
        void maj_boite_collision()
        {
            if (boite_collision==NULL || nbrSommets==NULL)return;
            double xmin=sommets[0];
            double xmax=xmin;
            double ymin=sommets[1];
            double ymax=ymin;
            double zmin=sommets[2];
            double zmax=zmin;
            for (uint32_t i=0;i<nbrSommets*3;)
            {
                double xt=sommets[i++];
                double yt=sommets[i++];
                double zt=sommets[i++];
                if (xt<xmin)xmin=xt; else if (xt>xmax)xmax=xt;
                if (yt<ymin)ymin=yt; else if (yt>ymax)ymax=yt;
                if (zt<zmin)zmin=zt; else if (zt>zmax)zmax=zt;
            }
            boite_collision->determine_limites(xmin,xmax,ymin,ymax,zmin,zmax);
        }


        //==========================================================================================================================
        //==========================================================================================================================
        //                         Gestion des coordonnées dans l'espace absolu
        //==========================================================================================================================
        //==========================================================================================================================
public:

        //--------------------------------------------------------------------------------------
        //              Calcul le centre géométrique du volume
        /// Sert pour le tri!
        //--------------------------------------------------------------------------------------
        //!Appeler cette fonction APRES avoir calculer les sommets absolus.
        void calcul_centre_geometrique_absolu()
        {
            double cx = centre_geometrique_relatif[0];
            double cy = centre_geometrique_relatif[1];
            double cz = centre_geometrique_relatif[2];

            // coordonnées de l'objet enfant dans le repère de l'objet parent
            centre_geometrique_absolu[0] =  repere_absolu->x + cx * repere_absolu->ax_x + cy * repere_absolu->ay_x + cz * repere_absolu->az_x;
            centre_geometrique_absolu[1] =  repere_absolu->y + cx * repere_absolu->ax_y + cy * repere_absolu->ay_y + cz * repere_absolu->az_y;
            centre_geometrique_absolu[2] =  repere_absolu->z + cx * repere_absolu->ax_z + cy * repere_absolu->ay_z + cz * repere_absolu->az_z;
        }

        //---------------------------------------------------------
        //  Calcul le centre de gravite absolu d'une face:
        /// Servent pour le tri des faces transparentes!
        //---------------------------------------------------------

        /*
        void calcul_centre_gravite_absolu_face(uint32_t indice_face)
        {
            double xt=faces[indice_face]->centre_gravite_relatif[0];
            double yt=faces[indice_face]->centre_gravite_relatif[1];
            double zt=faces[indice_face]->centre_gravite_relatif[2];
            faces[indice_face]->centre_gravite_absolu[0] = repere_absolu->x + xt * ax_x + yt * ay_x + zt * az_x;
            faces[indice_face]->centre_gravite_absolu[1] = repere_absolu->y + xt * ax_y + yt * ay_y + zt * az_y;
            faces[indice_face]->centre_gravite_absolu[2] = repere_absolu->z + xt * ax_z + yt * ay_z + zt * az_z;
        }

        void calcul_centre_gravite_absolu_face(dksFace* face)
        {
            double xt=face->centre_gravite_relatif[0];
            double yt=face->centre_gravite_relatif[1];
            double zt=face->centre_gravite_relatif[2];
            face->centre_gravite_absolu[0] = repere_absolu->x + xt * ax_x + yt * ay_x + zt * az_x;
            face->centre_gravite_absolu[1] = repere_absolu->y + xt * ax_y + yt * ay_y + zt * az_y;
            face->centre_gravite_absolu[2] = repere_absolu->z + xt * ax_z + yt * ay_z + zt * az_z;
        }
        */


        //****************************************************************************************************************
        //****************************************************************************************************************
        //                                              AFFICHAGE DU VOLUME
        //****************************************************************************************************************
        //****************************************************************************************************************

        //---------------------------------------------------------------
		//Initialise OpenGl pour le rendu d'un materiau:
		//---------------------------------------------------------------
public: ///Public, pour que le moteur puisse s'en servir lors de l'affichage des faces transparentes d'une scène
        void initialise_materiau(dksSurface* surface)
		{
            surface_courante=surface;
            couleur_ambiance[0]=surface->couleur_ambiance[0]*surface->ambiance;
		    couleur_ambiance[1]=surface->couleur_ambiance[1]*surface->ambiance;
		    couleur_ambiance[2]=surface->couleur_ambiance[2]*surface->ambiance;
		    couleur_ambiance[3]=1.;//1.-surface->transparence;

            couleur_diffusion[0]=surface->couleur_diffusion[0]*surface->diffusion;
            couleur_diffusion[1]=surface->couleur_diffusion[1]*surface->diffusion;
            couleur_diffusion[2]=surface->couleur_diffusion[2]*surface->diffusion;
            couleur_diffusion[3]=1.-surface->transparence;          //Seul l'alpha de la couleur de diffusion est pris en compte

            couleur_speculaire[0]=surface->couleur_speculaire[0]*surface->specularite;
            couleur_speculaire[1]=surface->couleur_speculaire[1]*surface->specularite;
            couleur_speculaire[2]=surface->couleur_speculaire[2]*surface->specularite;
            couleur_speculaire[3]=1.;//-surface->transparence_speculaire;

            float f_lum=surface->luminescence*facteur_luminescence;
            couleur_luminescence[0]=surface->couleur_luminescence[0]*f_lum;
            couleur_luminescence[1]=surface->couleur_luminescence[1]*f_lum;
            couleur_luminescence[2]=surface->couleur_luminescence[2]*f_lum;
            couleur_luminescence[3]=1.;//-surface->transparence;

            //cout<<"R:"<<ambiance_faces[0]<<" V:"<<ambiance_faces[1]<<" B:"<<ambiance_faces[2]<<endl;


            //Gestion de l'affichage double-face:
            //Attention, les normales ne sont pas recalculées selon l'orientation de la face par rapport à l'oeil.
            GLuint face_materiaux;


            if(surface->double_face)
            {
                glDisable(GL_CULL_FACE);
                face_materiaux=GL_FRONT_AND_BACK;
                glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
            }
            else
            {
                glEnable(GL_CULL_FACE); //Active la distinction face avant/face arrière des polygones pour le rendu.
                face_materiaux=GL_FRONT;
                glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
            }

            glColor3fv(couleur_diffusion);  //Au cas où les éclairages sont désactivés
            glMaterialfv(face_materiaux,GL_AMBIENT, couleur_ambiance);
            glMaterialfv(face_materiaux,GL_DIFFUSE, couleur_diffusion);
            glMaterialfv(face_materiaux,GL_EMISSION, couleur_luminescence);
            glMaterialfv(face_materiaux,GL_SPECULAR, couleur_speculaire);
            glMaterialf(face_materiaux, GL_SHININESS, surface->brillance);
            glShadeModel(GL_SMOOTH);


            //Gestion des tableaux:

            if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
            {
                if (surface->drapeau_adoucissement)
                {
                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_DOUBLE,0,surface->normales_sommets);
                }
                else glDisableClientState(GL_NORMAL_ARRAY);
            }

            //---- Initialise openGL selon le rendu de la surface:
            glUseProgram(0);    ///Il faut le laisser ici !!
            desactive_textures();
            desactive_pointeurs_attributs_sommets();
            (this->*init_rendus[surface->mode_rendu])();
		}

        //---------------------------------------------------------------
		// Rendu d'un materiau luminescent. Sert pour le rendu des auras
		//---------------------------------------------------------------
protected:
        virtual void initialise_materiau_luminescent(dksSurface* surface)
		{
		    GLuint face_materiaux;
		    surface_courante=surface;
		    float f_lum=surface->luminescence*facteur_luminescence;
		    if (f_lum>0.)
		    {
                couleur_luminescence[0]=surface->couleur_luminescence[0]*f_lum;
                couleur_luminescence[1]=surface->couleur_luminescence[1]*f_lum;
                couleur_luminescence[2]=surface->couleur_luminescence[2]*f_lum;
                couleur_luminescence[3]=1.-surface->transparence;
                //glColor3f(couleur_luminescence[0],couleur_luminescence[1],couleur_luminescence[2]);
                glColor3fv(couleur_luminescence);
		    }

		    //Surface opaque (sert juste à masquer les surfaces lumineuses)
		    else glColor4fv(noir);

            //Gestion de l'affichage double-face:
		    if(surface->double_face) glDisable(GL_CULL_FACE);
            else glEnable(GL_CULL_FACE); //Active la distinction face avant/face arrière des polygones pour le rendu.
		}

        //-------------------------------------------------------------------------------
		// Rendu d'un materiau pour le z-buffer. Sert pour le rendu des ombres
		// Il est peut-être préférable que les masques de couleurs soient mis à false: glColorMask(false,false,false,false)
		//-------------------------------------------------------------------------------
protected:
        virtual void initialise_materiau_profondeur(dksSurface* surface)
		{
		    //Gestion de l'affichage double face:
            if(surface->double_face) glDisable(GL_CULL_FACE);
            else glEnable(GL_CULL_FACE); //Active la distinction face avant/face arrière des polygones pour le rendu.
		}



		//--------Désactive les pointeurs d'attributs des sommets:
		void desactive_pointeurs_attributs_sommets()
		{
		    for(uint16_t i=0;i<nbr_attributs_sommets_max;i++) glDisableVertexAttribArray(i);
		}


        //---------------------------------------------------------------
		//Tri les faces transparentes:
		//---------------------------------------------------------------
        //Le test utilisé pour le tri des faces:
		static bool comparaison_face(tri_face t1, tri_face t2) { return t1.distance > t2.distance; }

		void tri_faces_transparentes(dksOeil* oeil) ///A ADAPTER
		{
		    uint32_t i;
		    double dx,dy,dz;
		    //if (nbrFaces_transparentes==0) return;
		    //Calcul les distances avec l'oeil:
		    for (i=0;i<nbrFaces_transparentes;i++)
		    {
                dksFace* face=indices_faces_transparentes[i].face; //Au besoin créer une table.
                face->calcul_centre_gravite_absolu(repere_absolu);
                dx=oeil->repere_absolu->x-face->centre_gravite_absolu[0];
                dy=oeil->repere_absolu->y-face->centre_gravite_absolu[1];
                dz=oeil->repere_absolu->z-face->centre_gravite_absolu[2];
                indices_faces_transparentes[i].distance=dx*dx+dy*dy+dz*dz;
		    }
		    std::sort(indices_faces_transparentes.begin(),indices_faces_transparentes.end(),dksVolume::comparaison_face);
		}

        //============================================================================================================================================================
        //============================================================================================================================================================
        //                      Fonctions de rendu:
        //============================================================================================================================================================
        //============================================================================================================================================================
public:
        //------------------------------------------------------------------------------
        //      Mise à jour du repère absolu et de la matrice.
        /// A n'utiliser que lorsque le conteneur n'est pas un élément d'une scène.
        /// La matrice courante est multipliée à la matrice du volume. Il n'y a plus qu'à afficher.
        //------------------------------------------------------------------------------
        virtual void maj(double t)
        {
            //position_temps=t;
            //delta_t=t-t0;

            applique_mouvements();
            calcul_repere_absolu();

            glPushMatrix();
            calcul_matrice();
            glPopMatrix();

            glMultMatrixd(matrice.matrice);

            //t0=t;
        }

        //------------------------------------------------
        //              Affichage du volume
        //------------------------------------------------
        virtual void affiche(dksOeil* oeil)
        {
            uint32_t duree_test;
            uint32_t i,j;
            uint16_t k;
            oeil_courant=oeil;
            GLboolean brouillard_mem=glIsEnabled(GL_FOG);
            if (erreur!=DKSVOLUME_OK || (!drapeau_affiche)) return;
            if (!drapeau_affecte_par_brouillard) {glDisable(GL_FOG);};

            #include "debug_volume_affiche.h"
            glEnable(GL_DEPTH_TEST);

            //-------------------------- Affiche les sommets
            if (mode_remplissage==DKSVOLUME_REMPLISSAGE_SOMMETS)
            {
                GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
                glPointSize(taille_points_sommets);
                desactive_textures();
                glDisable(GL_LIGHTING);
                glColor3fv(couleur_sommets);
                for (i=0;i<nbrSommets;i++)
                {
                    glBegin(GL_POINTS);
                        glVertex3dv(&sommets[3*i]);
                    glEnd();
                }
                if (lum_mem)glEnable(GL_LIGHTING);
            }

            //-------------------------- Affiche les arètes
            else if (mode_remplissage==DKSVOLUME_REMPLISSAGE_ARETES)
            {
                desactive_textures();
                GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
                glDisable(GL_LIGHTING);
                glColor3fv(couleur_aretes);
                for (i=0;i<nbrAretes;i++)
                {
                    glBegin(GL_LINES);
                        glVertex3dv(&sommets[aretes[i*2]*3]);
                        glVertex3dv(&sommets[aretes[i*2+1]*3]);
                    glEnd();
                }
                if (lum_mem)glEnable(GL_LIGHTING);
            }

            //-------------------------- Affiche les faces en aplats colorés:
            else if (mode_remplissage==DKSVOLUME_REMPLISSAGE_COULEUR)
            {
                rendu_aplats_couleur();
            }

            //------------------------- Affiche les surfaces:
            else if (mode_remplissage==DKSVOLUME_REMPLISSAGE_SURFACES)
            {
                glColorMask(true,true,true,true);
                glFrontFace(sens_polygones); //Détermine quelle est la face "avant" des polygones, selon
                                             // le sens des sommets: CCW (counter-clockwise) ou CW (clockwise)
                switch (mode_rendu)
                {
                    case DKSVOLUME_MODE_IMMEDIAT:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);
                        rendu_opaques_mode_immediat();
                        if (drapeau_rendu_faces_transparentes)rendu_transparentes_mode_immediat(oeil);
                    break;

                    case DKSVOLUME_MODE_TABLEAUX:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,sommets);
                        rendu_opaques_mode_tableaux();
                        if (drapeau_rendu_faces_transparentes)rendu_transparentes_mode_tableaux(oeil);
                    break;

                    case DKSVOLUME_MODE_LISTE:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);
                        rendu_mode_liste();
                    break;

                    case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        glDisableClientState(GL_NORMAL_ARRAY);
                        rendu_mode_liste_opaques_immediat_transparentes(oeil);
                    break;

                    case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,sommets);
                        rendu_mode_liste_opaques_tableaux_transparentes(oeil);
                    break;
                }
            }
            glUseProgram(0);
            if (!drapeau_affecte_par_brouillard && brouillard_mem) glEnable(GL_FOG);
        }


        //---------------------------------------------------------
        //              Affichage des surfaces lumineuses
        //              Sert pour le rendu des auras
        //---------------------------------------------------------
        public:
        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {
            if (erreur!=DKSVOLUME_OK || (!drapeau_affiche)) return;
            GLboolean eclairage_mem=glIsEnabled(GL_LIGHTING);
            GLboolean brouillard_mem=glIsEnabled(GL_FOG);
            if (!drapeau_affecte_par_brouillard) glDisable(GL_FOG);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            desactive_textures();
            glUseProgram(0);
            glFrontFace(sens_polygones); //Détermine quelle est la face "avant" des polygones, selon
                                         // le sens des sommets: CCW (counter-clockwise) ou CW (clockwise)

            if(drapeau_liste_luminescences)
            {
                glDisableClientState(GL_VERTEX_ARRAY);
                rendu_surfaces_luminescentes_liste();
            }
            else
            {
                switch (mode_rendu)
                {
                    case DKSVOLUME_MODE_IMMEDIAT:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        rendu_surfaces_luminescentes_immediat();
                    break;

                    //Le volume en mode LISTE peut rendre ses surfaces luminescentes en mode Tableau.
                    //Ca permet d'avoir la possibilité de modifier sa luminescence en temps réel.
                    case DKSVOLUME_MODE_TABLEAUX:
                    case DKSVOLUME_MODE_LISTE:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,sommets);
                        rendu_surfaces_luminescentes_tableaux();
                    break;

                    break;
                }
            }

            if (eclairage_mem) glEnable(GL_LIGHTING);
            if (!drapeau_affecte_par_brouillard && brouillard_mem) glEnable(GL_FOG);

        }

        //---------------------------------------------------------
        //              Affichage de la profondeur.
        //              Sert pour le rendu des ombres
        //---------------------------------------------------------
        virtual void affiche_profondeur(dksOeil* oeil)
        {
            if ( erreur!=DKSVOLUME_OK || (!drapeau_affiche) ) return;

            if (drapeau_recoit_ombres && (source_des_ombres!=NULL))
            {
                //Sauvegarde la matrice de projection des uv de la texture de profondeurs:
                matrice.multiplication_gauche(*source_des_ombres->renvoie_matrice_uv(),matrice_uv_source_des_ombres);
                matrice.multiplication_gauche(*source_des_ombres->renvoie_matrice_vision(),matrice_vision_source_des_ombres);
                matrice.multiplication_gauche(*source_des_ombres->renvoie_matrice_projection_vision(),matrice_projection_vision_source_des_ombres);
                //Récupère les composantes de la matrice de projection de la source des ombres qui vont servir à convertir
                //les valeurs du Zbuffer en coordonnées spatiales:
                Matrice4x4* matrice_projection_source=source_des_ombres->renvoie_matrice_projection();
                composantes_convertion= { float(matrice_projection_source->matrice[14]) , float(matrice_projection_source->matrice[10]) }; ///Invertion de l'ordre...(little endian)
            }

            if ( (!drapeau_projete_ombres) || (!drapeau_tempon_profondeur) ) return;

            //Initialise l'état openGl nécessaire:
            /// Remonter tout ça....?
            GLboolean eclairage_mem=glIsEnabled(GL_LIGHTING);
            GLboolean brouillard_mem=glIsEnabled(GL_FOG);
            glDisable(GL_FOG);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDisable(GL_LIGHTING);
            glDisable(GL_BLEND);
            glShadeModel(GL_FLAT);

            glFrontFace(sens_polygones); //Détermine quelle est la face "avant" des polygones, selon
                                         // le sens des sommets: CCW (counter-clockwise) ou CW (clockwise)
            desactive_textures();
            glUseProgram(0);

            //Rendu:
            if(drapeau_liste_profondeurs)
            {
                glDisableClientState(GL_VERTEX_ARRAY);
                rendu_profondeur_liste();
            }
            else
            {
                switch (mode_rendu)
                {
                    case DKSVOLUME_MODE_IMMEDIAT:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        rendu_profondeur_immediat();
                    break;

                    case DKSVOLUME_MODE_TABLEAUX:
                        glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3,GL_DOUBLE,0,sommets);
                        rendu_profondeur_tableaux();
                    break;

                    case DKSVOLUME_MODE_LISTE:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES:
                    case DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        rendu_profondeur_liste();
                    break;
                }
            }

            if (eclairage_mem) glEnable(GL_LIGHTING);
            if (brouillard_mem) glEnable(GL_FOG);
            glUseProgram(0);
        }

protected:
        //---------------------------------------------------------
        //          Initialisation des rendus:
        //---------------------------------------------------------
        #include "dksVolume_init_mode_rendu.h"
        //---------------------------------------------------------
        //          Rendu des surfaces opaques en mode immédiat:
        //---------------------------------------------------------
        #include "dksVolume_mode_rendu_immediat.h"
        //----------------------------------------------------------------------------------------
        //          Rendu des surfaces opaques avec l'utilisation des tableaux:
        //----------------------------------------------------------------------------------------
        #include "dksVolume_mode_rendu_tableaux.h"
        //---------------------------------------------------------
        //          Rendus pour le stockage dans les listes OpenGl:
        //---------------------------------------------------------
        #include "dksVolume_mode_rendu_liste.h"


};

#endif // DKSVOLUME_H_INCLUDED
