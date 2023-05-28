#ifndef DKSVOLUME_H_INCLUDED
#define DKSVOLUME_H_INCLUDED


//*****************************************************************************************************************************************
//
//                                              Gestion des volumes DKS
//
//  Un volume peut être rendu de 3 manières:
//      1-En mode immédiat,
//      2-Avec les tableaux,
//      3-Par les listes d'affichage.
//          Dans ce dernier cas, il y a deux possibilités:
//                  1-Ne stocker en liste que les surfaces opaques,
//                  2-Stocker les surfacess opaques et les surfaces transparentes, auquel cas il peut y avoir des bugs d'afficache dus à l'abscence de tri.
//
//
//*****************************************************************************************************************************************

#include "dksConteneur.h"
#include "../Volumes/dksBoiteCollision.h"
#include "../../chargeurs_images/Image.h"
#include "dksOeil.h"
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

//Drapeaux pour la libération de la mémoire lors de la destruction:
#define DKSVOLUME_LIBERE_ARETES_FACES    0x00000001
#define DKSVOLUME_LIBERE_FACES           0x00000002
#define DKSVOLUME_LIBERE_NOM             0x00000004
#define DKSVOLUME_LIBERE_BOITE_COLLISION 0x00000008

//Modes pour le stockage en liste openGL:
#define DKSVOLUME_LISTE_INUTILISEE 0
#define DKSVOLUME_LISTE_SURFACES_OPAQUES 1
#define DKSVOLUME_LISTE_SURFACES_OPAQUES_ET_TRANSPARENTES 2

//Drapeau de débogage:

#define DKSVOLUME_DEBUG_DUREES false

//----------------------------
//  Structure des faces:
//----------------------------
class dksSurface;
class dksVolume;
class dksTexture;

class dksFace
{
    public:
        //Données fournies à la création du volume:
        uint16_t nbrSommets;
        uint32_t* aretes;   //Les aretes doivent boucler (dernier sommets lié au premier)
        dksSurface* surface;   //La surface à laquelle est affectée la face
        //Données initialisées automatiquement:
        uint32_t    indice; //L'indice de la face dans le volume
        dksVolume*  volume; //Le volume auquel appartient la face

        GLdouble normale_relative[3];
        GLdouble normale_absolue[3];

        double centre_gravite_relatif[3]; //Le centre de gravité de la face
        double centre_gravite_absolu[3];  //Le centre dans l'espace absolu

        uint32_t* liens_aretes; //Indices des arêtes de la face dans la table des arêtes du volume.

        GLdouble* normales_relatives_sommets; //Ces normales ne servent que si la surface est non-uniformément adoucie.
        GLdouble* normales_absolues_sommets;

        dksFace()
        {
            volume=NULL;
            aretes=NULL;
            surface=NULL;
            liens_aretes=NULL;
            nbrSommets=0;
            normales_relatives_sommets=NULL;
            normales_absolues_sommets=NULL;
        }

        ~dksFace()
        {
            if (aretes!=NULL) free (aretes);
            if (liens_aretes!=NULL) free(liens_aretes);
            if (normales_relatives_sommets!=NULL) free(normales_relatives_sommets);
            if (normales_absolues_sommets!=NULL) free(normales_absolues_sommets);
        }
};


//----------------------------------
//  Structure des surfaces:
//
//  Si la surface est opaque, il n'y a pas de tri des faces.
//  Si la surface est transparente, ses faces sont stockées dans la table des faces transparentes du volume.
//     Une surface est considérée comme transparente si:
//                  -son niveau de transparence > 0,
//                  -la texture de couleur a une couche alpha non uniforme ou <> 0xff
//                  -il y a une texture de transparence
//
//          a priori, la transparence d'un fragment est calculé comme suit:
//                  Tf=transparence+Af+Tt avec une limitation à 1. Af=Alpha, Tt=transparence de la texture de transparence
//
//----------------------------------
class dksSurface
{
    public:
        char* nom;
        dksVolume* volume;  //Le volume auquel appartient la surface
        bool double_face;   //Affiche les deux côtés des faces
        bool drapeau_adoucissement; //True si les faces sont adoucies
        bool drapeau_transparence;  //True si la surface est transparente.
        bool drpaeau_partiellement_adoucie; //True si certaines arêtes ne sont pas adoucies (angle>angle_limite_adoucissement)
        float angle_limite_adoucissement;

        float couleur_diffusion[3];
        float couleur_speculaire[3];
        float couleur_ambiance[3]; //Réflexion de la lumière ambiante
        float couleur_luminescence[3]; //Luminescence de la surface

        float luminescence;
        float diffusion;
        float ambiance;
        float specularite;
        float brillance;    //=Shininess
        float transparence;
        float translucidite; //Comment le gérer?
        float reflexion;// Gérée par les shaders
        float rugosite; //"Bump", géré par les shaders

        //Textures:
        vector<dksTexture*> textures;

        //faces liées à la surface:
        uint32_t nbr_faces;
        vector<dksFace*> faces;

        dksSurface()
        {
            nom=NULL;
        }

        ~dksSurface()
        {
            if (nom!=NULL) free(nom);
        }
};

//----------------------------------
//  Structure des textures:
//----------------------------------

//Types de texture:
#define DKSTEXTURE_INVALIDE 0
#define DKSTEXTURE_COULEUR 1
#define DKSTEXTURE_DIFFUSION 2
#define DKSTEXTURE_AMBIANCE 3
#define DKSTEXTURE_LUMINESCENCE 4
#define DKSTEXTURE_TRANSPARENCE 5
#define DKSTEXTURE_REFLEXION 6
#define DKSTEXTURE_RUGOSITE 7

class dksTexture
{
    public:
    uint8_t type;   //Couleur, diffusion, réflexion...
    bool drapeau_active; //True si la texture est active
    GLfloat* coordonnees_UV;    //Attention à la gestion des tables de coordonnées UV!
    Image* image;

    dksTexture()
    {
        type=DKSTEXTURE_INVALIDE;
    }
};

//Structure d'une liaison de faces:
//La liaison peut servir pour les sommets ou les arêtes.

class liaison_faces
{
    public:
        vector<uint32_t> faces;
};

//Structure utilisée pour le tri des faces transparentes:
class tri_face
{   public:
    double distance;        //Distance de la face à l'oeil
    uint32_t indice_face;   //L'indice de la face
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
        uint16_t erreur; //Code d'erreur en cas de problème
        //Sommets relatifs au volume:
        GLdouble* sommets_relatifs;
        //Sommets dans l'espace absolu. Ces sommets ne sont pas systématiquement calculés, car pour le rendu, ce sont les matrices d'OpenGl qui servent.
        //Les sommets absolus servent pour des traitements spécifiques (collisions, debuggages...).
		GLdouble* sommets_absolus;

		GLdouble* sommets;      //Pointe sur la table de sommets utilisée pour le rendu.
                                //Si le moteur est en mode TRANSFORMATIONS_MATRICES, ce sont les sommets
                                //relatifs qui sont utilisés. Si le moteur est en mode TRANSFORMATIONS_REPERES, les
                                //sommets absolus sont utilisés.
                                //Par défaut, ce sont les sommets relatifs qui sont utilisés.

        bool volume_a_change; //True lorsque le volume a bougé. Ses sommets sont alors rafraichis par la hiérarchie.

    private:
		double echelle_geometrique; //Facteur multiplicateur des sommets du volume
		double epsilon; //Sert pour le calcul de planitude des faces. Il est différent de la constante globale EPSILON

    public:

		uint32_t nbrSommets;
		uint32_t nbrFaces;
		uint32_t nbrAretes;
		dksFace** faces;

        //Tableau utilisé pour le tri des faces transparentes
        uint32_t nbrFaces_transparentes;
        vector<dksFace*> faces_transparentes;
		vector<tri_face> indices_faces_transparentes;

        //Tables de liaisons. Ces informations peuvent servir.
		vector<uint32_t> aretes; //Tableau qui peut être généré à partir des faces avec "initialisation_aretes()"
        vector<liaison_faces> liaisons_aretes_faces; //Vecteur de vecteurs d'indices de faces
        vector<liaison_faces> liaisons_sommets_faces; //Vecteur de vecteurs d'indices de faces
        vector<dksVecteurUnitaire> normales_relatives_sommets; //Normales relatives au volume des sommets
        vector<dksVecteurUnitaire> normales_absolues_sommets; //Normales des sommets dans l'espace absolu

        //La table des surfaces.
		uint32_t nbrSurfaces;
		vector<dksSurface*> surfaces;

        //La boite déterminant les limites de collision du volume:
        dksBoiteCollision* boite_collision;

        //Données géométriques:
        dksVecteur centre_geometrique_relatif;
        dksVecteur centre_geometrique_absolu;


    private:
        //Gestion mémoire:
        uint32_t drapeaux_liberation_memoire;

        //Couleur des matériaux:
        //(Réinitialisées à chaque changement de surface)
        GLfloat couleur_ambiance[4];
        GLfloat couleur_diffusion[4];
        GLfloat couleur_specularite[4];
        GLfloat couleur_luminescence[4];

        //Gestion des listes OpenGl:
        //Les liste d'openGl ne peuvent être utilisées que si les matrices d'OpenGl sont activées (voir dksMoteur3d.h)
        //


        uint8_t mode_liste_OpenGl;  //Utilisation des listes d'OpenGL: LISTE_INUTILISEE, LISTE_SURFACES_OPAQUES, LISTE_SURFACES_OPAQUES_ET_TRANSPARENTES
        GLuint  liste_id;   //L'identifiant de la liste OpenGl pour ce volume.

//========================================================================================================
//==================================== Constructeur du volume:
//========================================================================================================
    public:
        dksVolume( char* p_nom,
                uint32_t p_drapeaux_liberation_memoire=0,
                double px = 0., double py = 0., double pz = 0.,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL,
                double* psommets = NULL,
                uint32_t p_nbrSommets=0,
                dksFace** pfaces = NULL,
                uint32_t p_nbrFaces=0)
                :dksConteneur(p_nom,px,py,pz,axe1,axe2,axe3)
		{
		    //std::cout<<"------- Création du volume: "<<p_nom<<std::endl;

            //--------- Initialise les variables par défaut:
            classe = DKSCONTENEUR_CLASSE_VOLUME;
            classe_id=DKSCONTENEUR_CLASSE_VOLUME_ID;
		    erreur=0;
		    sommets_absolus=NULL;
		    sommets_relatifs=NULL;
		    echelle_geometrique=1.;
		    epsilon=1e-7;
		    nbrSommets=0;
		    nbrFaces=0;
		    nbrAretes=0;
		    boite_collision=NULL;
		    nbrSurfaces=0;
		    mode_liste_OpenGl=DKSVOLUME_LISTE_INUTILISEE;
		    liste_id=0;

		    volume_a_change=true;

		    //--------Initialise les sommets:
		    drapeaux_liberation_memoire=p_drapeaux_liberation_memoire;
		    nbrSommets=p_nbrSommets;
		    sommets_relatifs=psommets;
		    sommets_absolus=(double*)malloc(sizeof(double)*nbrSommets*3);
		    if (sommets_absolus==0){std::cout<<"Mémoire insuffisante"<<std::endl;erreur=DKSVOLUME_ERREUR_MEMOIRE;return;}

            //---------Initialise les faces:

		    nbrFaces=p_nbrFaces;
		    faces=pfaces;

            if (DKSVOLUME_DEBUG_DUREES) cout<<"-------------- Initialisation des faces du volume: "<<nom<<" ----------------"<<endl;
            double duree_faces=glutGet(GLUT_ELAPSED_TIME);

		    if (nbrFaces > 0)
            {
                double duree;

                /*
                if (DKSVOLUME_CREATION_ARETES)
                {
                    duree = glutGet(GLUT_ELAPSED_TIME);
                    initialise_aretes();
                    duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                    if (DEBUG_DUREES_VOLUME) cout<<"Aretes : "<<duree<<endl;
                }

                if (DKSVOLUME_CREATION_LIENS_ARETES_FACES)
                {
                    duree = glutGet(GLUT_ELAPSED_TIME);
                    initialise_liaisons_aretes_faces();
                    duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                    if (DEBUG_DUREES_VOLUME) cout<<"Liaisons aretes-faces : "<<duree<<endl;
                }
                */


                duree = glutGet(GLUT_ELAPSED_TIME);
                initialise_liaisons_sommets_faces();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Liaisons sommets-faces : "<<duree<<endl;

                duree = glutGet(GLUT_ELAPSED_TIME);
                initialise_faces();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Paramètres des faces : "<<duree<<endl;

                duree = glutGet(GLUT_ELAPSED_TIME);
                calcul_normales_relatives_sommets();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Normales des sommets : "<<duree<<endl;

                duree = glutGet(GLUT_ELAPSED_TIME);
                initialise_liens_faces_aretes();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Liens faces-arêtes : "<<duree<<endl;

                duree = glutGet(GLUT_ELAPSED_TIME);
                calcul_normales_relatives_sommets_faces();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Normales des sommets des faces : "<<duree<<endl;

                duree = glutGet(GLUT_ELAPSED_TIME);
                initialise_faces_transparentes();
                duree = glutGet(GLUT_ELAPSED_TIME)-duree;
                if (DKSVOLUME_DEBUG_DUREES) cout<<"Faces transparentes : "<<duree<<endl;
            }

            duree_faces=glutGet(GLUT_ELAPSED_TIME)-duree_faces;
            if (DKSVOLUME_DEBUG_DUREES)
            cout<<"**** Durée d'initialisation des faces du volume "<<nom<<" : "<<duree_faces<<endl<<endl;;

            //--------- Initialise les surfaces:
            double duree_surfaces=glutGet(GLUT_ELAPSED_TIME);

            creation_surfaces();

            duree_surfaces=glutGet(GLUT_ELAPSED_TIME)-duree_surfaces;
            if (DKSVOLUME_DEBUG_DUREES)
            cout<<"Durée d'initialisation des surfaces du volume "<<nom<<" : "<<duree_surfaces<<endl;

            //----------- Initialise les données géométriques:
            calcul_centre_geometrique_relatif();
		}


		//================ Destructeur:

        ~dksVolume()
        {
            std::cout<<"Destruction du volume: "<<nom<<std::endl;

            detruit_liste_OpenGl();

            if(sommets_absolus!=0) free(sommets_absolus);

            unsigned int i;


            //Libère la boite de collision:
            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_BOITE_COLLISION)!=0)
            {
                if ( boite_collision!=NULL) delete boite_collision;
            }

            //Libère les tableaux d'arêtes des faces:

            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_ARETES_FACES)!=0)
            {
                for (i=0;i<nbrFaces;i++)
                {
                    if (faces[i]->aretes!=0) free(faces[i]->aretes);
                }
            }

            //Libère les faces:

            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_FACES)!=0) free(faces);

            //Libère la mémoire utilisée pour nom:

            if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_NOM)!=0) free(nom);

            //Libère la table des liaisons des arêtes et des faces:
            /*
            if ( liaisons_aretes_faces!=NULL)
            {
                for (i=0;i<nbrAretes;i++)
                {
                    delete liaisons_aretes_faces[i];
                }
            }
            */
        }

        //=========================================================================================
        //                          Gestion des données géométriques
        //=========================================================================================

        //--------------------------------------------------------------------------------------
        //              Calcul du centre géométrique du volume
        //--------------------------------------------------------------------------------------
        void calcul_centre_geometrique_relatif()
        {
            uint32_t i;
            double cx=0;
            double cy=0;
            double cz=0;
            for (i=0;i<nbrSommets*3;)
            {
                cx+= sommets_relatifs[i++];
                cy+= sommets_relatifs[i++];
                cz+= sommets_relatifs[i++];
            }

            cx/=(double)nbrSommets;
            cy/=(double)nbrSommets;
            cz/=(double)nbrSommets;
            centre_geometrique_relatif.x=cx;
            centre_geometrique_relatif.y=cy;
            centre_geometrique_relatif.z=cz;
        }

        //!Appeler cette fonction APRES avoir calculer les sommets absolus.
        void calcul_centre_geometrique_absolu()
        {
            double cx = centre_geometrique_relatif.x;
            double cy = centre_geometrique_relatif.y;
            double cz = centre_geometrique_relatif.z;

            // coordonnées de l'objet enfant dans le repère de l'objet parent
            centre_geometrique_absolu.x =  repere_absolu->x + cx * ax_x + cy * ay_x + cz * az_x;
            centre_geometrique_absolu.y =  repere_absolu->y + cx * ax_y + cy * ay_y + cz * az_y;
            centre_geometrique_absolu.z =  repere_absolu->z + cx * ax_z + cy * ay_z + cz * az_z;
        }

        //=========================================================================================
        //                          Gestion des arêtes
        //=========================================================================================

        //--------------------------------------------------------------------------------------
        //              Création de la table des arêtes à partir des faces:
        //--------------------------------------------------------------------------------------

        void initialise_aretes()
		{
			nbrAretes = 0;
            uint32_t i,j,k,sommetA,sommetB;

			for (i = 0; i < nbrFaces; i++)
			{
				for (j = 0; j < faces[i]->nbrSommets; j++ )
				{
					sommetA = faces[i]->aretes[j];
					sommetB = faces[i]->aretes[j + 1];

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
		}

		//--------------------------------------------------------------------------------------
        //             Création de la table de correspondance entre les arêtes et les faces
        //--------------------------------------------------------------------------------------
        void initialise_liaisons_aretes_faces()
        {
            uint32_t i,j,k;
            for (i=0;i<nbrAretes;i++)
            {
                ajoute_liaison();
                for (j=0;j<nbrFaces;j++)
                {
                    uint32_t* at=faces[j]->aretes;
                    for (k=0;k<faces[j]->nbrSommets;k++)
                    {
                        if ( ((aretes[2*i]==at[k]) && (aretes[2*i+1] == at[k+1]))
                            || ((aretes[2*i]==at[k+1]) && (aretes[2*i+1] == at[k])) )
                        {
                            liaisons_aretes_faces[i].faces.push_back(j);
                            break;
                        }
                    }
                }
            }
        }

        void ajoute_liaison()
        {
            liaison_faces liaison;
            liaisons_aretes_faces.push_back(liaison);
        }


        //Affiche la table des liaisons des arêtes avec les faces:
        void affiche_liaison_aretes_faces()
        {
            for (uint32_t i=0;i<liaisons_aretes_faces.size();i++)
            {
                std::cout<<"Arete: "<<i<<" faces: ";
                for (uint32_t j=0;j<liaisons_aretes_faces[i].faces.size();j++)
                {
                    std::cout<<liaisons_aretes_faces[i].faces[j]<<" ";
                }
                std::cout<<std::endl;
            }
        }

        //=========================================================================================
        //                          Gestion des faces
        //=========================================================================================

        //---------------------------------------------------------
        //Initialise le tableau de tri des faces transparentes
        //Les faces transparentes étant triées, elle ne peuvent pas
        //être affichées par surface. Il faut une table spécifique.
        //---------------------------------------------------------


        void initialise_faces_transparentes()
        {
            indices_faces_transparentes.clear();
            nbrFaces_transparentes=0;

            for (uint32_t i=0;i<nbrFaces;i++)
            {
                if (faces[i]->surface->transparence!=0.)
                {
                    tri_face tri;
                    indices_faces_transparentes.push_back(tri);
                    indices_faces_transparentes[nbrFaces_transparentes].indice_face=i;
                    indices_faces_transparentes[nbrFaces_transparentes].distance=0.;
                    nbrFaces_transparentes++;
                }

            }
        }

        //--------------------------------------------------------------------------------------
        //Réorganise l'ordre des sommets de manière à ne pas avoir 3 point alignés au départ.
        //--------------------------------------------------------------------------------------
		//Ces trois points servent pour le calcul de la normale. Il ne faut pas qu'ils soient alignés.
		//ATTENTION!!! IL FAUT QUE LES ARETES FACES AIENT ETE ALLOUEES DYNAMIQUEMENT.
        //Renvoie false si une erreur s'est produite.

		bool reorganisation_aretes_pour_calcul_normale()
		{
		    if ((drapeaux_liberation_memoire & DKSVOLUME_LIBERE_ARETES_FACES)==0)return false;

		    uint32_t i;
		    uint16_t j,k;
			for (i=0; i < nbrFaces; i++)
			{
				uint32_t* at = faces[i]->aretes;

				double xA = sommets_relatifs[at[0]*3];
				double yA = sommets_relatifs[at[0]*3+1];
				double zA = sommets_relatifs[at[0] * 3 + 2];

				double xB = sommets_relatifs[at[1] * 3];
				double yB = sommets_relatifs[at[1] * 3 + 1];
				double zB = sommets_relatifs[at[1] * 3 + 2];

				for (j=0; j < faces[i]->nbrSommets-1; j++)
				{
					double xC = sommets_relatifs[at[j + 2] * 3];
					double yC = sommets_relatifs[at[j + 2] * 3 + 1];
					double zC = sommets_relatifs[at[j + 2] * 3 + 2];

					double vect1_x = xB - xA;
					double vect1_y = yB - yA;
					double vect1_z = zB - zA;

					double vect2_x = xC - xB;
					double vect2_y = yC - yB;
					double vect2_z = zC - zB;

					double scal = vect1_x * vect2_x + vect1_y * vect2_y + vect1_z * vect2_z;
					double scal2 = scal * scal;
					double norme1_2 = vect1_x * vect1_x + vect1_y * vect1_y + vect1_z * vect1_z;
					double norme2_2 = vect2_x * vect2_x + vect2_y * vect2_y + vect2_z * vect2_z;

					double test = norme1_2 * norme2_2 - scal2;

					if (((test > DKSVOLUME_EPSILON) || (test < - DKSVOLUME_EPSILON))) break;

					xA = xB;
					yA = yB;
					zA = zB;

					xB = xC;
					yB = yC;
					zB = zC;
				}
				if (j != 0)
				{
					uint32_t* nouveau_at = (uint32_t*) malloc(sizeof(uint32_t)*(faces[i]->nbrSommets+1));
					if (nouveau_at==0)return false;
					for (k = 0; k < faces[i]->nbrSommets; k++)
					{
						nouveau_at[k] = at[j++];
						if (j == faces[i]->nbrSommets) j = 0;
					}
					nouveau_at[k] = nouveau_at[0];
					free(faces[i]->aretes);  //ATTENTION!!! IL FAUT QUE LES ARETES FACES AIENT ETE ALLOUEES DYNAMIQUEMENT.
					faces[i]->aretes = nouveau_at;
				}
			}
			return true;
		}


        //---------------------------------------------------------------------
        // Crée les liens avec les arêtes dans la table des arêtes du volume
        //---------------------------------------------------------------------

        void  initialise_liens_faces_aretes()
        {
            /*
            uint32_t i,j,k;
            for (i=0;i<nbrFaces;i++)
            {
                uint32_t* at=faces[i]->aretes;
                for (j=0;j<faces[i]->nbrSommets;j++)
                {
                    for (k=0;k<nbrAretes;k++)
                    {
                        if ( ((aretes[2*k]==at[j]) && (aretes[2*k+1] == at[j+1]))
                            || ((aretes[2*k]==at[j+1]) && (aretes[2*k+1] == at[j])) )
                        {
                            faces[i]->liens_aretes.push_back(k);
                            break;
                        }
                    }
                }
            }
            */
        }



		//---------------------------------------------------------
        // Calcul les normales relatives des faces
        // Fonction d'initialisation, à n'appeler que lors de la construction du volume.
        //---------------------------------------------------------

		void normales_relatives_faces()
		{
		    uint32_t i;
			for (i = 0; i < nbrFaces; i++)
			{
				GLdouble* nml_rel = faces[i]->normale_relative;
				uint32_t* at = faces[i]->aretes;

				uint32_t at0 = at[0] * 3;
				uint32_t at1 = at[1] * 3;
				uint32_t at2 = at[2] * 3;

				GLdouble v1x = sommets_relatifs[at1] - sommets_relatifs[at0];
				GLdouble v1y = sommets_relatifs[at1 + 1] - sommets_relatifs[at0 + 1];
				GLdouble v1z = sommets_relatifs[at1 + 2] - sommets_relatifs[at0 + 2];

				GLdouble v2x = sommets_relatifs[at2] - sommets_relatifs[at1];
				GLdouble v2y = sommets_relatifs[at2 + 1] - sommets_relatifs[at1 + 1];
				GLdouble v2z = sommets_relatifs[at2 + 2] - sommets_relatifs[at1 + 2];

				nml_rel[0]=v1y*v2z-v1z*v2y;
				nml_rel[1]=v1z*v2x-v1x*v2z;
				nml_rel[2]=v1x*v2y-v1y*v2x;

				GLdouble norme_nml=sqrt(nml_rel[0]*nml_rel[0]+nml_rel[1]*nml_rel[1]+nml_rel[2]*nml_rel[2]);
				nml_rel[0]/=norme_nml;
				nml_rel[1]/=norme_nml;
				nml_rel[2]/=norme_nml;

				 //Copie la normale relative dans la normale absolue.
                 //Sert pour l'utilisation des matrices d'OpenGl:
                 faces[i]->normale_absolue[0]=nml_rel[0];
                 faces[i]->normale_absolue[1]=nml_rel[1];
                 faces[i]->normale_absolue[2]=nml_rel[2];

				//Initialise les normales des sommets:


				for (uint16_t j=0;j<faces[i]->nbrSommets;j++)
				{
                    ajoute_normale(faces[i]);
                    faces[i]->normales_relatives_sommets[j].x=nml_rel->x;
                    faces[i]->normales_relatives_sommets[j].y=nml_rel->y;
                    faces[i]->normales_relatives_sommets[j].z=nml_rel->z;
				}
			}
		}

		//---------------------------------------------------------
        // Calcul les centres de gravité relatifs des faces
        //---------------------------------------------------------
        void calcul_centres_gravite_relatifs_faces()
        {
            for (uint32_t i=0;i<nbrFaces;i++)
            {
                double cx=0.;
                double cy=0.;
                double cz=0.;

                for (uint16_t j=0;j<faces[i]->nbrSommets;j++)
                {
                    uint32_t indice=faces[i]->aretes[j]*3;
                    cx+=sommets_relatifs[indice];
                    cy+=sommets_relatifs[indice+1];
                    cz+=sommets_relatifs[indice+2];
                }
                double nbr=faces[i]->nbrSommets;
                faces[i]->centre_gravite_relatif[0]=cx/nbr;
                faces[i]->centre_gravite_relatif[1]=cy/nbr;
                faces[i]->centre_gravite_relatif[2]=cz/nbr;
            }
        }


        //---------------------------------------------------------
        // Calcul les normales relatives des sommets des faces
        // Ces normales ne servent que si l'adoucissement de la surface
        // à laquelle est liée la face est activé.
        //---------------------------------------------------------
        void calcul_normales_relatives_sommets_faces()
        {
            for (uint32_t i=0;i<nbrFaces;i++)
            {
                for (uint16_t j=0;j< faces[i]->nbrSommets;j++)
                {
                    uint32_t indice_sommet=faces[i]->aretes[j];

                    for (uint32_t k=0;k<liaisons_sommets_faces[indice_sommet].faces.size();k++)
                    {
                        Face* face_B=faces[liaisons_sommets_faces[indice_sommet].faces[k]];
                        if (face_B != faces[i])
                        {
                            //Calcul l'angle avec une face adjacente:
                            double cos_angle=
                                        faces[i]->normale_relative.x*face_B->normale_relative.x+
                                        faces[i]->normale_relative.y*face_B->normale_relative.y+
                                        faces[i]->normale_relative.z*face_B->normale_relative.z;
                            double angle=acosf(cos_angle);

                            //std::cout<<"Angle entre la face "<<faces[i].indice<<" et la face "<<face_B->indice<<" : "<<angle<<" / "<<faces[i].surface->angle_limite_adoucissement<<std::endl;

                            //Si l'angle limite n'est pas atteint, calcul la normale des sommets de l'arête:
                            if (faces[i]->surface->angle_limite_adoucissement > angle)
                            {
                                faces[i]->normales_relatives_sommets[j].x+=face_B->normale_relative.x;
                                faces[i]->normales_relatives_sommets[j].y+=face_B->normale_relative.y;
                                faces[i]->normales_relatives_sommets[j].z+=face_B->normale_relative.z;
                                //Copie la normale relative dans la normale absolue.
                                //Sert pour l'utilisation des matrices d'OpenGl:
                                faces[i]->normales_sommets[j].x=faces[i]->normales_relatives_sommets[j].x;
                                faces[i]->normales_sommets[j].y=faces[i]->normales_relatives_sommets[j].y;
                                faces[i]->normales_sommets[j].z=faces[i]->normales_relatives_sommets[j].z;

                            }
                        }
                    }
                    faces[i]->normales_relatives_sommets[j].convertir();
                    faces[i]->normales_sommets[j].x=faces[i]->normales_relatives_sommets[j].x;
                    faces[i]->normales_sommets[j].y=faces[i]->normales_relatives_sommets[j].y;
                    faces[i]->normales_sommets[j].z=faces[i]->normales_relatives_sommets[j].z;
                }
            }
        }


        //---------------------------------------------------------
        //Initialise les données nécessaires au rendu des faces
        //---------------------------------------------------------
        void initialise_faces()
        {
            for (uint32_t i=0;i<nbrFaces;i++)
            {
                //Initialisations des données de travail:
                faces[i]->volume=this;
                faces[i]->indice=i;
            }
            reorganisation_aretes_pour_calcul_normale();
            normales_relatives_faces();
            calcul_centres_gravite_relatifs_faces();
        }

        //====================================================================================================
        //                            Gestion des surfaces
        //====================================================================================================

        //---------------------------------------------------------------
        //              Création de la table des liens faces<->surfaces
        //---------------------------------------------------------------
        bool creation_surfaces()
        {
            nbrSurfaces=0;
            surfaces.clear();
            //Création de la table des surfaces et des tables des faces dans les surfaces :)
            Face* face;
            uint32_t i,k;
            for (i=0;i<nbrFaces;i++)
            {
                face=faces[i];
                //Test si la surface est déjà répertoirée:
                for (k=0;k<nbrSurfaces;k++)
                {
                    if(surfaces[k].surface==face->surface)
                    {
                        //Ajoute la face:
                        surfaces[k].faces.push_back(face);
                        surfaces[k].nbrFaces++;
                        if (face->affiche_texture) surfaces[k].faces_texturees.push_back(face);
                        else surfaces[k].faces_planes.push_back(face);
                        break;
                    }
                }
                // Si la surface n'est pas répertoriée, on l'ajoute à la table:
                if (k==nbrSurfaces)
                {
                    Surface_faces nouvelle_surface;
                    surfaces.push_back(nouvelle_surface);
                    surfaces[nbrSurfaces].surface=face->surface;
                    surfaces[nbrSurfaces].nbrFaces=1;
                    surfaces[nbrSurfaces].faces.push_back(face);
                    if (face->affiche_texture) surfaces[nbrSurfaces].faces_texturees.push_back(face);
                    else surfaces[nbrSurfaces].faces_planes.push_back(face);
                    nbrSurfaces++;
                }
            }

            return true;
        }


        //=========================================================================================
        //                          Gestion des sommets
        //=========================================================================================


        //--------------------------------------------------------------------------------------
        //             Création de la table de correspondance entre les sommets et les faces
        //--------------------------------------------------------------------------------------
        void initialise_liaisons_sommets_faces()
        {
            uint32_t i,j,k;

            for (i=0;i<nbrSommets;i++)
            {
                ajoute_liaison_sommet();
                for (j=0;j<nbrFaces;j++)
                {
                    uint32_t* at=faces[j]->aretes;
                    for (k=0;k<faces[j]->nbrSommets;k++)
                    {
                        if(at[k]==i)
                        {
                            liaisons_sommets_faces[i].faces.push_back(j);
                            break;
                        }
                    }
                }
            }
            //affiche_liaison_sommets_faces();
        }

        void ajoute_liaison_sommet()
        {
            liaison_faces liaison;
            liaisons_sommets_faces.push_back(liaison);
        }

        //--------------------------------------------------------------------------------------
        //Calcul les normales relatives des sommets:
        //--------------------------------------------------------------------------------------

        //Appeler cette fonction après avoir créé les liaisons.
        void calcul_normales_relatives_sommets()
        {
            for(uint32_t i=0 ; i< nbrSommets;i++)
            {
            //calcul la normale au sommet:
                ajoute_normale_sommet();
                for (uint32_t j=0;j<liaisons_sommets_faces[i].faces.size();j++)
                {
                    //Calcul la somme des normales des faces liées au sommet:
                    Vecteur_Unitaire* normale_relative_face=&faces[liaisons_sommets_faces[i].faces[j]]->normale_relative;
                    normales_relatives_sommets[i].x+=normale_relative_face->x;
                    normales_relatives_sommets[i].y+=normale_relative_face->y;
                    normales_relatives_sommets[i].z+=normale_relative_face->z;
                }
                normales_relatives_sommets[i].convertir();
            }
        }

        void ajoute_normale_sommet()
        {
            Vecteur_Unitaire normale_relative_sommet(0.,0.,0.);
            Vecteur_Unitaire normale_sommet;
            normales_relatives_sommets.push_back(normale_relative_sommet);
            normales_sommets.push_back(normale_sommet);
        }

        //--------------------------------------------------------------------------------------
        //Affiche la table des liaisons des sommets avec les faces:
        //--------------------------------------------------------------------------------------
        void affiche_liaison_sommets_faces()
        {
            for (uint32_t i=0;i<liaisons_sommets_faces.size();i++)
            {
                std::cout<<"Sommet: "<<i<<" faces: ";
                for (uint32_t j=0;j<liaisons_sommets_faces[i].faces.size();j++)
                {
                    std::cout<<liaisons_sommets_faces[i].faces[j]<<" ";
                }
                std::cout<<std::endl;
            }
        }

         //=========================================================================================
        //                         Gestion des coordonnées dans l'espace absolu
        //=========================================================================================

        //---------------------------------------------------------
        //Calcul les sommets dans l'espace absolu:
        //---------------------------------------------------------

		void calcul_sommets_absolus()
		{
			double xt;
			double yt;
			double zt;

			if (a_change_par_propagation || a_change || volume_a_change)
			{
				for (unsigned int i = 0; i < nbrSommets*3;)
				{
					xt = sommets_relatifs[i];
					yt = sommets_relatifs[i+1];
					zt = sommets_relatifs[i+2];
					// coordonnées de l'objet enfant dans le repère de l'objet parent
					sommets_absolus[i++] =  repere_absolu->x + xt * ax_x + yt * ay_x + zt * az_x;
					sommets_absolus[i++] =  repere_absolu->y + xt * ax_y + yt * ay_y + zt * az_y;
					sommets_absolus[i++] =  repere_absolu->z + xt * ax_z + yt * ay_z + zt * az_z;

				}

				//Calculs des données des faces:

				calcul_donnees_faces();

                //Calculs de la boite de collision:
                if (boite_collision!=NULL) boite_collision->calcul_sommets_absolus(repere_absolu);

				//calcul_normales_sommets();


			}
		}

        //-------------------------------------------------------------------
        //  Calcul les données géométriques des faces dans l'espace absolus:
        //-------------------------------------------------------------------

        void calcul_donnees_faces()
        {
            uint32_t i;
            double xt,yt,zt;

            for (i=0;i<nbrFaces;i++)
            {
                //---- Calcul les normales des faces:
                if (!faces[i]->surface->adoucissement)
                {
                    xt=faces[i]->normale_relative.x;
                    yt=faces[i]->normale_relative.y;
                    zt=faces[i]->normale_relative.z;
                    faces[i]->normale.x = xt * ax_x + yt * ay_x + zt * az_x;
                    faces[i]->normale.y = xt * ax_y + yt * ay_y + zt * az_y;
                    faces[i]->normale.z = xt * ax_z + yt * ay_z + zt * az_z;
                }
                //---- Calcul les normales des sommets, si la surface est adoucie:
                else
                {
                    for (uint16_t j=0;j<faces[i]->nbrSommets;j++)
                    {
                        xt=faces[i]->normales_relatives_sommets[j].x;
                        yt=faces[i]->normales_relatives_sommets[j].y;
                        zt=faces[i]->normales_relatives_sommets[j].z;
                        faces[i]->normales_sommets[j].x = xt * ax_x + yt * ay_x + zt * az_x;
                        faces[i]->normales_sommets[j].y = xt * ax_y + yt * ay_y + zt * az_y;
                        faces[i]->normales_sommets[j].z = xt * ax_z + yt * ay_z + zt * az_z;
                    }
                }

            }
        }

        //---------------------------------------------------------
        //  Calcul les normales absolues des sommets:
        //---------------------------------------------------------

        void calcul_normales_sommets()
        {
            unsigned int i;

            for (i=0;i< nbrSommets ;i++)
            {
                double xt=normales_relatives_sommets[i].x;
                double yt=normales_relatives_sommets[i].y;
                double zt=normales_relatives_sommets[i].z;
                normales_sommets[i].x = xt * ax_x + yt * ay_x + zt * az_x;
                normales_sommets[i].y = xt * ax_y + yt * ay_y + zt * az_y;
                normales_sommets[i].z = xt * ax_z + yt * ay_z + zt * az_z;
            }
        }

        //---------------------------------------------------------
        //  Calcul le centre de gravite absolu d'une face:
        //---------------------------------------------------------

        void calcul_centre_gravite_absolu_face(uint32_t indice_face)
        {
            double xt=faces[indice_face]->centre_gravite_relatif.x;
            double yt=faces[indice_face]->centre_gravite_relatif.y;
            double zt=faces[indice_face]->centre_gravite_relatif.z;
            faces[indice_face]->centre_gravite.x = repere_absolu->x + xt * ax_x + yt * ay_x + zt * az_x;
            faces[indice_face]->centre_gravite.y = repere_absolu->y + xt * ax_y + yt * ay_y + zt * az_y;
            faces[indice_face]->centre_gravite.z = repere_absolu->z + xt * ax_z + yt * ay_z + zt * az_z;
        }

        void calcul_centre_gravite_absolu_face(dksFace* face)
        {
            double xt=face->centre_gravite_relatif.x;
            double yt=face->centre_gravite_relatif.y;
            double zt=face->centre_gravite_relatif.z;
            face->centre_gravite.x = repere_absolu->x + xt * ax_x + yt * ay_x + zt * az_x;
            face->centre_gravite.y = repere_absolu->y + xt * ax_y + yt * ay_y + zt * az_y;
            face->centre_gravite.z = repere_absolu->z + xt * ax_z + yt * ay_z + zt * az_z;
        }



        //=========================================================================================
        //                          Gestion des mouvements
        //=========================================================================================

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


        //=========================================================================================
        //                          Gestion des collisions
        //=========================================================================================

        //Génère le pavé circonscrit au volume:
        //Renvoie true si la boîte a bien été créée.
        bool initialise_boite_collisions(double facteur_x=1.0,double facteur_y=1.0, double facteur_z=1.0)
        {
            double xmin=sommets_relatifs[0];
            double xmax=xmin;
            double ymin=sommets_relatifs[1];
            double ymax=ymin;
            double zmin=sommets_relatifs[2];
            double zmax=zmin;

            for (uint32_t i=0;i<nbrSommets*3;)
            {
                double xt=sommets_relatifs[i++];
                double yt=sommets_relatifs[i++];
                double zt=sommets_relatifs[i++];
                if (xt<xmin)xmin=xt; else if (xt>xmax)xmax=xt;
                if (yt<ymin)ymin=yt; else if (yt>ymax)ymax=yt;
                if (zt<zmin)zmin=zt; else if (zt>zmax)zmax=zt;
            }

            //Mise à l'échelle:
            xmin=(xmin-centre_geometrique_relatif.x)*facteur_x+centre_geometrique_relatif.x;
            xmax=(xmax-centre_geometrique_relatif.x)*facteur_x+centre_geometrique_relatif.x;

            ymin=(ymin-centre_geometrique_relatif.y)*facteur_y+centre_geometrique_relatif.y;
            ymax=(ymax-centre_geometrique_relatif.y)*facteur_y+centre_geometrique_relatif.y;

            zmin=(zmin-centre_geometrique_relatif.z)*facteur_z+centre_geometrique_relatif.z;
            zmax=(zmax-centre_geometrique_relatif.z)*facteur_z+centre_geometrique_relatif.z;

            //Création de la boite:
            boite_collision=new Boite_Collision(xmin,xmax,ymin,ymax,zmin,zmax);

            if (boite_collision==NULL)
            {
                cout<<"ERREUR dans Volume.initialise_boite_collision()- Impossible de créer la boîte de collision: "<<nom<<endl;
                return false;
            }
            drapeaux_liberation_memoire|=LIBERE_BOITE_COLLISION;
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


        //=========================================================================================
        //                          Duplication:
        //=========================================================================================

        //---------- Duplique les faces du volume:
        //!!! Attention à libérer la mémoire utilisée pour les copies des faces !!!

        dksFace** duplique_faces()
        {
            uint32_t i;
            uint16_t j;
            unsigned int* atc;
            unsigned int* at;
            dksFace** copies_faces=(dksFace**)calloc(nbrFaces,4);
            if (copies_faces==NULL) return NULL;
            for(i=0;i<nbrFaces;i++)
            {
                copies_faces[i]=new Face();
                copies_faces[i]->nbrSommets=faces[i]->nbrSommets;
                copies_faces[i]->surface=faces[i]->surface;
                atc=(uint32_t*)malloc(sizeof(uint32_t)*(faces[i]->nbrSommets+1));
                if(atc==NULL)
                {
                    for (j=0;j<i;j++)
                    {
                        delete(copies_faces[j]);
                    }
                    free(copies_faces);
                    return NULL;
                }
                copies_faces[i]->aretes=atc;
                at=faces[i]->aretes;
                for(j=0;j<faces[i]->nbrSommets+1;j++)
                {
                    atc[j]=at[j];
                }
            }
            return copies_faces;
        }

        //------------------- Duplique le volume:
        //Pour le moment les sommets relatifs ne sont pas dupliqués.
        //Il faut faire attention, lors des modifications sur ces sommets,
        //Tout les duplicatats seront affectés.

        dksVolume* duplique()
        {
            dksFace** copies_faces=duplique_faces();
            if (copies_faces==NULL)return NULL;

            Volume* clone=new dksVolume(nom,LIBERE_FACES|LIBERE_ARETES_FACES,
                                        x,y,z,
                                        new Vecteur_Unitaire(axe_x->x,axe_x->y,axe_x->z),
                                        new Vecteur_Unitaire(axe_y->x,axe_y->y,axe_y->z),
                                        new Vecteur_Unitaire(axe_z->x,axe_z->y,axe_z->z),
                                        sommets_relatifs,nbrSommets,
                                        copies_faces,nbrFaces
                                        );

            return clone;
        }




         //=========================================================================================
        //                          Paramètres du volume
        //=========================================================================================

    public:

        //Renvoie la table des sommets relatifs du volume:
        double* renvoie_sommets_relatifs()
        {
            return sommets_relatifs;
        }

        //Renvoie la table des sommets absolus du volume:
        double* renvoie_sommets_absolus()
        {
            return sommets_absolus;
        }

        //Renvoie la Face correspondant à l'indice:
        dksFace* renvoie_face(uint32_t indice)
        {
            if (indice>=nbrFaces) return NULL;
            return faces[indice];
        }

        //****************************************************************************************************************
        //****************************************************************************************************************
        //                                              AFFICHAGE DU VOLUME
        //****************************************************************************************************************
        //****************************************************************************************************************



        //---------------------------------------------------------------
		//Initialise OpenGl pour le rendu d'un materiau:
		//---------------------------------------------------------------
private:
        void initialise_materiau(dksSurface* surface)
		{
		    couleur_ambiance[0]=surface->couleur_ambiance[0]*surface->ambiance;
		    couleur_ambiance[1]=surface->couleur_ambiance[1]*surface->ambiance;
		    couleur_ambiance[2]=surface->couleur_ambiance[2]*surface->ambiance;
		    couleur_ambiance[3]=1.;

            couleur_diffusion[0]=surface->couleur_diffuse[0]*surface->diffusion;
            couleur_diffusion[1]=surface->couleur_diffuse[1]*surface->diffusion;
            couleur_diffusion[2]=surface->couleur_diffuse[2]*surface->diffusion;
            couleur_diffusion[3]=1.-surface->transparence;

            couleur_speculaire[0]=surface->couleur_speculaire[0]*surface->specularite;
            couleur_speculaire[1]=surface->couleur_speculaire[1]*surface->specularite;
            couleur_speculaire[2]=surface->couleur_speculaire[2]*surface->specularite;
            couleur_speculaire[3]=1.;

            couleur_emission[0]=surface->couleur_emission[0]*surface->luminosite;
            couleur_emission[1]=surface->couleur_emission[1]*surface->luminosite;
            couleur_emission[2]=surface->couleur_emission[2]*surface->luminosite;
            couleur_emission[3]=1.;

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
            glMaterialfv(face_materiaux,GL_AMBIENT, couleur_ambiance);
            glMaterialfv(face_materiaux,GL_DIFFUSE, couleur_diffusion);
            glMaterialfv(face_materiaux,GL_EMISSION, couleur_emission);
            glMaterialfv(face_materiaux,GL_SPECULAR, couleur_speculaire);
            glMaterialf(face_materiaux, GL_SHININESS, surface->brillance);
		}



		//---------------------------------------------------------------
		//Tri les faces transparentes:
		//---------------------------------------------------------------
        //Le test utilisé pour le tri des faces:
		static bool comparaison(tri_face t1, tri_face t2) { return t1.distance < t2.distance; }

		void tri_faces_transparentes(dksOeil* oeil) ///A ADAPTER
		{
		    //if (nbrFaces_transparentes==0) return;
		    //Calcul les distances avec l'oeil:
		    for (uint32_t i=0;i<nbrFaces_transparentes;i++)
		    {
                Face* face=faces[indices_faces_transparentes[i].indice_face]; //Au besoin créer une table.
                calcul_centre_gravite_absolu_face(face);
                double dx=oeil->repere_absolu->x-face->centre_gravite.x;
                double dy=oeil->repere_absolu->y-face->centre_gravite.y;
                double dz=oeil->repere_absolu->z-face->centre_gravite.z;
                indices_faces_transparentes[i].distance=dx*dx+dy*dy+dz*dz;
		    }
		    std::sort(indices_faces_transparentes.begin(),indices_faces_transparentes.end(),Volume::comparaison);
		}

        //========================================================================
        //              Affichage
        //========================================================================
 public:
        //Sert dans le cas où le moteur caulcul les coordonnées absolues des sommets:
        void affiche_absolu(dksOeil* oeil)
        {
            sommets=sommets_absolus;
            affiche(oeil);
        }

        //Sert dans le cas où le moteur se sert des matrices de transformation d'OpenGl:
        //La liste openGl peut alors servir.
        void affiche_relatif(dksOeil* oeil)
        {
            if (drapeau_liste_OpenGl)
            {
                glCallList(liste_id);
            }
            else
            {
                sommets=sommets_relatifs;
                affiche(oeil);
            }
        }

        //----------- Affichage:
private:
    void affiche(dksOeil* oeil)
        {
            //-------- Tri les faces transparentes:
                if (nbrFaces_transparentes>1) tri_faces_transparentes(oeil);

            //-------- Affichage des faces:

                affiche_faces_opaques();
                if (nbrFaces_transparentes>0) affiche_faces_transparentes();

            //-----------------------
				a_change_par_propagation = false;
				a_change = false;
				volume_a_change = false;
        }

        //========================================================================
        //              Affichages des faces
        //========================================================================

private:
        //----------------- Affiche les faces transparentes:
        void affiche_faces_transparentes()
        {
            uint32_t i,k,indice;
            uint32_t* at;
            Face* face;

            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);

            //unsigned int surfaces_cptr=0;
            //unsigned int faces_cptr=0;

            Surface* surface_courante=NULL;

            for (i=nbrFaces_transparentes;i>0;i--)
            {

                face=faces[indices_faces_transparentes[i-1].indice_face];
                at = face->aretes;

                if (surface_courante!=face->surface)
                {
                    surface_courante=face->surface;
                    initialise_materiau(surface_courante);
                }

                //surfaces_cptr++;
                //Adoucie:
                if (surface_courante->adoucissement)
                {
                    if (face->affiche_texture)
                    {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                            glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                    }
                    else
                    {
                        glDisable(GL_TEXTURE_2D);
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                    }
                    glEnd();
                }
                //Non adoucie:
                else
                {

                    glNormal3d(face->normale.x,face->normale.y,face->normale.z);

                    if (face->affiche_texture)
                    {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                        glBegin(GL_POLYGON);
                        //glBegin(GL_TRIANGLE_FAN);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                    }
                    else
                    {
                        glDisable(GL_TEXTURE_2D);
                        glBegin(GL_POLYGON);
                        //glBegin(GL_TRIANGLE_FAN);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                    }
                    glEnd();
                }
            }
        }


        //----------------- Affiche les faces opaques:
        void affiche_faces_opaques()
        {
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            uint32_t i,j,k,indice;
            uint32_t* at;
            Face* face;

            for (i=0;i<nbrSurfaces;i++)
            {
                if (surfaces[i].surface->transparence!=0.) continue;
                //Pas besoin d'optim tant qu'il n'y a pas beaucoup de surfaces
                initialise_materiau(surfaces[i].surface);

                //surfaces_cptr++;

                //---Faces opaques simple-face
                //Adoucie:
                if (surfaces[i].surface->adoucissement)
                {
                    //Faces texturées:
                    glEnable(GL_TEXTURE_2D);
                    for (j=0;j<surfaces[i].faces_texturees.size();j++)
                    {
                        face=surfaces[i].faces_texturees[j];
                        at=face->aretes;
                        glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                            glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                        glEnd();
                    }

                    //Faces planes:
                    glDisable(GL_TEXTURE_2D);
                    for (j=0;j<surfaces[i].faces_planes.size();j++)
                    {

                        face=surfaces[i].faces_planes[j];
                        at = face->aretes;
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                        glEnd();
                    }
                }

                //Non adoucie:
                else
                {
                    //Faces texturées:
                    glEnable(GL_TEXTURE_2D);

                    for (j=0;j<surfaces[i].faces_texturees.size();j++)
                    {
                        face=surfaces[i].faces_texturees[j];
                        at = face->aretes;
                        glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        glNormal3d(face->normale.x,face->normale.y,face->normale.z);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                        glEnd();
                    }

                    //Faces planes:
                    glDisable(GL_TEXTURE_2D);
                    for (j=0;j<surfaces[i].faces_planes.size();j++)
                    {
                        face=surfaces[i].faces_planes[j];
                        at = face->aretes;
                        //glBegin(GL_TRIANGLE_FAN);
                        glBegin(GL_POLYGON);
                        glNormal3d(face->normale.x,face->normale.y,face->normale.z);
                        for (k=0;k<face->nbrSommets;k++)
                        {
                            indice=at[k]*3;
                            glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                        }
                        glEnd();
                    }

                }
            }
        }

        //======================================================================================
        //              Gestion de la liste d'affichage d'OpenGl
        //
        // !!!! ATTENTIION !!!!
        //  Le volume ne peut être affiché par sa liste que si le moteur utilise les matrices
        //  d'OpenGl (voir Moteur.h).
        //
        //======================================================================================

        //---------------------------------------------------------------------------
        //  Cette fonction effectue un rendu de l'objet pour le stocker dans une
        //  liste d'affichage d'OpenGL. Le rendu est plus rapide par la suite.
        //  Il y a des contraintes:
        //      -Les faces transparentes ne sont plus triées,
        //      -Le volume ne doit pas changer de forme ou de matériaux.
        //  Renvoie false si la liste n'a pas pu être créée.
        //---------------------------------------------------------------------------
public:
        bool enregistre_liste_OpenGl()
        {
            if (drapeau_liste_OpenGl)
            {
                glDeleteLists(liste_id,1);
                drapeau_liste_OpenGl=false;
            }
            liste_id=glGenLists(1);
            if (liste_id==0)
            {
                erreur=VOL_ERREUR_LISTE_OPENGL;
                return false;
            }
            glNewList(liste_id,GL_COMPILE);
            sommets=sommets_relatifs;
            rendu_volume();
            glEndList();
            drapeau_liste_OpenGl=true;
            return true;
        }

        void detruit_liste_OpenGl()
        {
            if (drapeau_liste_OpenGl)
            {
                glDeleteLists(liste_id,1);
                liste_id=0;
                drapeau_liste_OpenGl=false;
            }
        }

        //----------------------------------------------
        //          Génère la liste du volume
        //----------------------------------------------
private:
        void rendu_volume()
        {
            uint32_t i,j,k,indice;
            uint32_t* at;
            Face* face;

            for (i=0;i<nbrSurfaces;i++)
            {
                //Transparence:
                if (surfaces[i].surface->transparence!=0.)
                {
                    glEnable(GL_BLEND);
                    glDepthMask(GL_FALSE);
                }
                else
                {
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                }
                //Pas besoin d'optim tant qu'il n'y a pas beaucoup de surfaces
                initialise_materiau(surfaces[i].surface);

                //surfaces_cptr++;

                //---Faces opaques simple-face
                //Adoucie:
                if (surfaces[i].surface->adoucissement)
                {
                    //Faces texturées:
                    if (surfaces[i].faces_texturees.size()>0)
                    {
                        glEnable(GL_TEXTURE_2D);
                        for (j=0;j<surfaces[i].faces_texturees.size();j++)
                        {
                            face=surfaces[i].faces_texturees[j];
                            at=face->aretes;
                            glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                            //glBegin(GL_TRIANGLE_FAN);
                            glBegin(GL_POLYGON);
                            for (k=0;k<face->nbrSommets;k++)
                            {
                                indice=at[k]*3;
                                glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                                glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                                glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                            }
                            glEnd();
                        }
                    }

                    //Faces planes:
                    if (surfaces[i].faces_planes.size()>0)
                    {
                        glDisable(GL_TEXTURE_2D);
                        for (j=0;j<surfaces[i].faces_planes.size();j++)
                        {

                            face=surfaces[i].faces_planes[j];
                            at = face->aretes;
                            //glBegin(GL_TRIANGLE_FAN);
                            glBegin(GL_POLYGON);
                            for (k=0;k<face->nbrSommets;k++)
                            {
                                indice=at[k]*3;
                                glNormal3d(face->normales_sommets[k].x,face->normales_sommets[k].y,face->normales_sommets[k].z);
                                glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                            }
                            glEnd();
                        }
                    }

                }

                //Non adoucie:
                else
                {
                    //Faces texturées:
                    if (surfaces[i].faces_texturees.size()>0)
                    {
                        glEnable(GL_TEXTURE_2D);

                        for (j=0;j<surfaces[i].faces_texturees.size();j++)
                        {
                            face=surfaces[i].faces_texturees[j];
                            at = face->aretes;
                            glBindTexture(GL_TEXTURE_2D,face->image->texture_id);
                            //glBegin(GL_TRIANGLE_FAN);
                            glBegin(GL_POLYGON);
                            glNormal3d(face->normale.x,face->normale.y,face->normale.z);
                            for (k=0;k<face->nbrSommets;k++)
                            {
                                indice=at[k]*3;
                                glTexCoord2f(face->coordonnees_UV[2*k],face->coordonnees_UV[2*k+1]);
                                glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                            }
                            glEnd();
                        }
                    }

                    //Faces planes:
                    if (surfaces[i].faces_planes.size()>0)
                    {
                        glDisable(GL_TEXTURE_2D);

                        for (j=0;j<surfaces[i].faces_planes.size();j++)
                        {
                            face=surfaces[i].faces_planes[j];
                            at = face->aretes;
                            //glBegin(GL_TRIANGLE_FAN);
                            glBegin(GL_POLYGON);
                            glNormal3d(face->normale.x,face->normale.y,face->normale.z);
                            for (k=0;k<face->nbrSommets;k++)
                            {
                                indice=at[k]*3;
                                glVertex3d(sommets[indice],sommets[indice+1],sommets[indice+2]);
                            }
                            glEnd();
                        }
                    }

                }
            }
        }

};
#endif // DKSVOLUME_H_INCLUDED
