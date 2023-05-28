#ifndef DKSCHARGELWO2_H_INCLUDED
#define DKSCHARGELWO2_H_INCLUDED

/*
=====================================================================================================================

                    Chargeur de fichier LWO2 - V2 - Mars 2011

=====================================================================================================================
*/

/*
*   Chargement des textures:
*   ------------------------
*
*   Les répertoires des images utilisées pour les textures sont redirigés vers "chemin_images"
*   Si ce chemin n'est pas défini (NULL), le chemin figurant dans le fichier LWO2 est utilisé.
*
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "../../gestion_fichiers/gestion_fichiers.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "..\..\chargeurs_images\Image.h"
#include "GL/glut.h"

using namespace std;

//======== Codes des erreurs:

#define DKSCHARGELWO2_OK 0
#define DKSCHARGELWO2_ERREUR_LWO2 1
#define DKSCHARGELWO2_ERREUR_CALQUES 2
#define DKSCHARGELWO2_ERREUR_SURFACES 3
#define DKSCHARGELWO2_ERREUR_VOLUMES 4
#define DKSCHARGELWO2_ERREUR_UV 5
#define DKSCHARGELWO2_ERREUR_CLIPS 6
#define DKSCHARGELWO2_ERREUR_BLOCS 7
#define DKSCHARGELWO2_ERREUR_FICHIER 8
#define DKSCHARGELWO2_ERREUR_MEMOIRE 9

//+++++ Nouvelle version
#define DKSCHARGELWO2_DEBUG false
#define DKSCHARGELWO2_DEBUG_UV false
#define DKSCHARGELWO2_DEBUG_DUREES false
#define DKSCHARGELWO2_DEBUG_SURFACES false
#define DKSCHARGELWO2_DEBUG_TEXTURES false


/* chunk and subchunk IDs */

//======================== Codes ASCII des majuscules

// Ca peut servir dans de cas de bloc inconnus
//A=0x41    //N=0x4E    0=0x30
//B=0x42    //O=0x4F    1=0x31
//C=0x43    //P=0x50    2=0x32
//D=0x44    //Q=0x51    3=0x33
//E=0x45    //R=0x52    4=0x34
//F=0x46    //S=0x53    5=0x35
//G=0x47    //T=0x54
//H=0x48    //U=0x55
//I=0x49    //V=0x56
//J=0x4A    //W=0x57
//K=0x4B    //X=0x58
//L=0x4C    //Y=0x59
//M=0x4D    //Z=0x5A

#define LWID_(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))

#define ID_FORM  LWID_('F','O','R','M')
#define ID_LWO2  LWID_('L','W','O','2')
#define ID_LWOB  LWID_('L','W','O','B')

/* Identifiants récents: */
#define ID_VERS  LWID_('V','E','R','S')
#define ID_NODS  LWID_('N','O','D','S')

/* top-level chunks */
#define ID_LAYR  LWID_('L','A','Y','R')
#define ID_TAGS  LWID_('T','A','G','S')
#define ID_PNTS  LWID_('P','N','T','S')
#define ID_BBOX  LWID_('B','B','O','X')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_VMAD  LWID_('V','M','A','D')
#define ID_POLS  LWID_('P','O','L','S')
#define ID_PTAG  LWID_('P','T','A','G')
#define ID_ENVL  LWID_('E','N','V','L')
#define ID_CLIP  LWID_('C','L','I','P')
#define ID_SURF  LWID_('S','U','R','F')
#define ID_DESC  LWID_('D','E','S','C')
#define ID_TEXT  LWID_('T','E','X','T')
#define ID_ICON  LWID_('I','C','O','N')

/* types VMAP */
#define ID_PICK  LWID_('P','I','C','K')
#define ID_WGHT  LWID_('W','G','H','T')
#define ID_MNVW  LWID_('M','N','V','W')
#define ID_TXUV  LWID_('T','X','U','V')
#define ID_RGB   LWID_('R','G','B',' ')
#define ID_RGBA  LWID_('R','G','B','A')
#define ID_MORF  LWID_('M','O','R','F')
#define ID_SPOT  LWID_('S','P','O','T')

/* polygon types */
#define ID_FACE  LWID_('F','A','C','E')
#define ID_CURV  LWID_('C','U','R','V')
#define ID_PTCH  LWID_('P','T','C','H')
#define ID_MBAL  LWID_('M','B','A','L')
#define ID_BONE  LWID_('B','O','N','E')

/* polygon tags */
#define ID_SURF  LWID_('S','U','R','F')
#define ID_PART  LWID_('P','A','R','T')
#define ID_SMGP  LWID_('S','M','G','P')

/* envelopes */
#define ID_PRE   LWID_('P','R','E',' ')
#define ID_POST  LWID_('P','O','S','T')
#define ID_KEY   LWID_('K','E','Y',' ')
#define ID_SPAN  LWID_('S','P','A','N')
#define ID_TCB   LWID_('T','C','B',' ')
#define ID_HERM  LWID_('H','E','R','M')
#define ID_BEZI  LWID_('B','E','Z','I')
#define ID_BEZ2  LWID_('B','E','Z','2')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_STEP  LWID_('S','T','E','P')

/* clips */
#define ID_STIL  LWID_('S','T','I','L')
#define ID_ISEQ  LWID_('I','S','E','Q')
#define ID_ANIM  LWID_('A','N','I','M')
#define ID_XREF  LWID_('X','R','E','F')
#define ID_STCC  LWID_('S','T','C','C')
#define ID_TIME  LWID_('T','I','M','E')
#define ID_CONT  LWID_('C','O','N','T')
#define ID_BRIT  LWID_('B','R','I','T')
#define ID_SATR  LWID_('S','A','T','R')
#define ID_HUE   LWID_('H','U','E',' ')
#define ID_GAMM  LWID_('G','A','M','M')
#define ID_NEGA  LWID_('N','E','G','A')
#define ID_IFLT  LWID_('I','F','L','T')
#define ID_PFLT  LWID_('P','F','L','T')

/* surfaces */
#define ID_COLR  LWID_('C','O','L','R')
#define ID_LUMI  LWID_('L','U','M','I')
#define ID_DIFF  LWID_('D','I','F','F')
#define ID_SPEC  LWID_('S','P','E','C')
#define ID_GLOS  LWID_('G','L','O','S')
#define ID_REFL  LWID_('R','E','F','L')
#define ID_RFOP  LWID_('R','F','O','P')
#define ID_RIMG  LWID_('R','I','M','G')
#define ID_RSAN  LWID_('R','S','A','N')
#define ID_RBLR  LWID_('R','B','L','R')
#define ID_TRAN  LWID_('T','R','A','N')
#define ID_TROP  LWID_('T','R','O','P')
#define ID_TIMG  LWID_('T','I','M','G')
#define ID_TBLR  LWID_('T','B','L','R')
#define ID_RIND  LWID_('R','I','N','D')
#define ID_TRNL  LWID_('T','R','N','L')
#define ID_BUMP  LWID_('B','U','M','P')
#define ID_SMAN  LWID_('S','M','A','N')
#define ID_SIDE  LWID_('S','I','D','E')
#define ID_CLRH  LWID_('C','L','R','H')
#define ID_CLRF  LWID_('C','L','R','F')
#define ID_GLOW  LWID_('G','L','O','W')
#define ID_ADTR  LWID_('A','D','T','R')
#define ID_SHRP  LWID_('S','H','R','P')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_LSIZ  LWID_('L','S','I','Z')
#define ID_ALPH  LWID_('A','L','P','H')
#define ID_VCOL  LWID_('V','C','O','L')
#define ID_AVAL  LWID_('A','V','A','L')
#define ID_GVAL  LWID_('G','V','A','L')
#define ID_BLOK  LWID_('B','L','O','K')

/* texture layer */
#define ID_TYPE  LWID_('T','Y','P','E')
#define ID_CHAN  LWID_('C','H','A','N')
#define ID_NAME  LWID_('N','A','M','E')
#define ID_ENAB  LWID_('E','N','A','B')
#define ID_OPAC  LWID_('O','P','A','C')
#define ID_FLAG  LWID_('F','L','A','G')
#define ID_PROJ  LWID_('P','R','O','J')
#define ID_STCK  LWID_('S','T','C','K')
#define ID_TAMP  LWID_('T','A','M','P')

/* texture coordinates */
#define ID_TMAP  LWID_('T','M','A','P')
#define ID_AXIS  LWID_('A','X','I','S')
#define ID_CNTR  LWID_('C','N','T','R')
#define ID_SIZE  LWID_('S','I','Z','E')
#define ID_ROTA  LWID_('R','O','T','A')
#define ID_OREF  LWID_('O','R','E','F')
#define ID_FALL  LWID_('F','A','L','L')
#define ID_CSYS  LWID_('C','S','Y','S')

/* image map */
#define ID_IMAP  LWID_('I','M','A','P')
#define ID_IMAG  LWID_('I','M','A','G')
#define ID_WRAP  LWID_('W','R','A','P')
#define ID_WRPW  LWID_('W','R','P','W')
#define ID_WRPH  LWID_('W','R','P','H')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_AAST  LWID_('A','A','S','T')
#define ID_PIXB  LWID_('P','I','X','B')

/* procedural */
#define ID_PROC  LWID_('P','R','O','C')
#define ID_COLR  LWID_('C','O','L','R')
#define ID_VALU  LWID_('V','A','L','U')
#define ID_FUNC  LWID_('F','U','N','C')
#define ID_FTPS  LWID_('F','T','P','S')
#define ID_ITPS  LWID_('I','T','P','S')
#define ID_ETPS  LWID_('E','T','P','S')

/* gradient */
#define ID_GRAD  LWID_('G','R','A','D')
#define ID_GRST  LWID_('G','R','S','T')
#define ID_GREN  LWID_('G','R','E','N')
#define ID_PNAM  LWID_('P','N','A','M')
#define ID_INAM  LWID_('I','N','A','M')
#define ID_GRPT  LWID_('G','R','P','T')
#define ID_FKEY  LWID_('F','K','E','Y')
#define ID_IKEY  LWID_('I','K','E','Y')

/* shader */
#define ID_SHDR  LWID_('S','H','D','R')
#define ID_DATA  LWID_('D','A','T','A')

//======================== Structures des définitions de blocs(pour la définition des textures):

//Types d'opacité (définis par le sous-chunk OPAC):
#define LWO2_OPAC_Normale 0
#define LWO2_OPAC_Soustraction 1
#define LWO2_OPAC_Difference 2
#define LWO2_OPAC_Multiplication 3
#define LWO2_OPAC_Division 4
#define LWO2_OPAC_Alpha 5
#define LWO2_OPAC_Deplacement_texture 6
#define LWO2_OPAC_Addition 7

//Axes de déplacement dans le cas d'un type OPAC_Deplacement_texture (sous-chunk AXIS)
#define LWO2_AXIS_X 0
#define LWO2_AXIS_Y 1
#define LWO2_AXIS_Z 2

//Systèmes de coordonnées (bloc CSYS)
#define LWO2_REPERE_RELATIF 0
#define LWO2_REPERE_ABSOLU 1

//Modes de projections des textures. Pour le moment seul le mode UV est implémenté:
#define LWO2_TYPE_PROJECTION_PLAN 0
#define LWO2_TYPE_PROJECTION_CYLINDRIQUE 1
#define LWO2_TYPE_PROJECTION_SPHERIQUE 2
#define LWO2_TYPE_PROJECTION_CUBIQUE 3
#define LWO2_TYPE_PROJECTION_FRONTALE 4
#define LWO2_TYPE_PROJECTION_UV 5

//Modes de débordement des textures:
#define LWO2_MODE_DEBORDEMENT_DECOUPAGE 0
#define LWO2_MODE_DEBORDEMENT_REPETITION 1
#define LWO2_MODE_DEBORDEMENT_REFLEXION 2
#define LWO2_MODE_DEBORDEMENT_BORD 3

//---------------------------------------------------------------
//      Définition des bitmaps (blocs CLIP)
//      Les plugins et les filtres ne sont pas pris en compte
//      Les animations ne sont pas prises en compte.
//      Seulement les images fixes au format TIFF (CLIP STIL)
//---------------------------------------------------------------
class LWO2_Bitmap
{
    public:

        uint32_t identite;
        char*   fichier;
        Image* image;
        float   contraste;          //Défaut=0
        float   luminosite;         //Défaut=0
        float   saturation;         //Défaut=0
        float   decallage_teinte;   //defaut=0
        float   gamma;              //Défaut=1
        bool    negatif;            //true=invertion RGB

        LWO2_Bitmap()
        {
            fichier=NULL;
            image=NULL;
            contraste=0.;
            luminosite=0.;
            saturation=0.;
            decallage_teinte=0.;
            gamma=1.;
            negatif=false;

        }

        ~LWO2_Bitmap()
        {
            if (DKSCHARGELWO2_DEBUG) cout<<"DESTRUCTION DU BITMAP ' "<<fichier<<" '"<<endl;
            if (fichier!=NULL)free(fichier);
            if (image!=NULL) delete image;
        }

};

//---------------------------------------------------------------
//      Définition des polygones
//      Seuls les polygones de type "FACE" sont pris en compte.
//---------------------------------------------------------------

class LWO2_Polygone
{
    public:
                //Ces trois indices pointent dans la table des étiquettes.
    uint16_t    indice_etiquette_surface; //Surface appliquée au polygone
    uint16_t    indice_etiquette_part;    //Donnée d'édition, c'est la couleur donnée aux arêtes
    uint16_t    indice_etiquette_groupe_filtre;   //Le groupe de filtrage (shading) auquel appartient le polygone

    uint32_t    nbrSommets;
    uint32_t*   indices_sommets; //"Vertexs" - Indices dans la table des sommets du calque

    LWO2_Polygone()
    {
        indices_sommets=NULL;
    }
    ~LWO2_Polygone()
    {
        if (indices_sommets!=NULL) free(indices_sommets);
    }
};

//---------------------------------------------------------------
//Bloc VMAP= Vertex Mapping, en français: paramètres de sommet
//  Seuls les types TXUV sont pris en compte
//---------------------------------------------------------------
typedef struct
{
    uint32_t indice_sommet;     //Le sommet à corriger
    uint32_t indice_polygone;   //Le polygone concerné par la correction
    float U;            //Les coordonnées UV de substitution
    float V;
}LWO2_Sommet_UV_couture;

typedef struct
{
    uint32_t indice_sommet;
    float U;
    float V;
}LWO2_Sommet_UV;


class LWO2_ParametresSommets
{
    public:
    char*       nom;    //Utilisé pour faire le lien avec les blocs définis dans les surfaces (voir plus haut)
    uint32_t    dimensions;
    vector<LWO2_Sommet_UV>   sommets_UV;
    //Ces deux champs servent pour corriger les polygones situés à la couture des textures UV:
    //Ce genre de paramètres de sommet est couplé avec des paramètres portants le même nom.
    //Les vecteurs de couture se substituent aux autres vecteurs lorsque les polygones spécifiés sont calculés.
    bool                couture;    //true=couture
    vector<LWO2_Sommet_UV_couture>    sommets_UV_couture;  //Polygones concernés par la couture

    LWO2_ParametresSommets()
    {
        nom=NULL;
    }

    ~LWO2_ParametresSommets()
    {
       if (nom!=NULL)free(nom);
    }
};

//---------------------------------------------------------------
//      Définition des textures de surface. (Surface bloc
//      Seules les textures UV et les projections planes sont implémentées pour le moment.
//      Le fondu d'effet de texture (FALL) est ignoré.
//---------------------------------------------------------------

class LWO2_Texture
{
    public:
    uint32_t type;  //IMAP (ou PROC ou GRAD ou SHDR, non pris en compte pour le moment)
    char* chaine_tri;   //La chaine utilisée pour le tri des calques de textures
    uint32_t canal; //COLR,DIFF,LUMI,SPEC,GLOS,REFL,TRAN,RIND,TRNL,BUMP
    char nom_canal[5];  //Le nom du canal en chaine de caractères. C'est plus simple à débuguer.
    uint16_t active;    //true ou false
    bool fondu_pixels;  //True: lorsque les pixels de la texture sont plus gros que les pixels de l'écran, ils sont fondus.
    uint16_t type_opacite;  //0:Normale, 1:Soustraction, 2:Différence, 3:Multiplication,4:Division, 5:Alpha, 6:Déplacement de texture, 7: Addition
    float valeur_opacite;
    float amplitude_rugosite;   //Sert pour les textures de rugosité(Bump). La valeur des pixels est multipliée par cette amplitude.
    char* objet_de_reference;   //Le nom de l'objet de référence pour le repère des données précédentes.
    uint16_t systeme_coordonnees; //Pour les textures de projection ,spécifie le repère de référence (absolu ("world coordinates") ou relatif ("object coordinates") )
    float centre[3];        //Paramètres des textures projetées
    float echelle[3];
    float rotation[3];      //"head","pitch","bank" roulis,tangage,lacet en radians
    //Données propres au type IMAP:
    uint16_t    type_projection; //0:Plan, 1: Cylindrique, 2:Sphérique, 3:Cubique, 4: Frontale, 5:UV
    uint16_t    axe_projection; //0:X, 1:Y, 2:Z (Ne sert pas pour les UV)
    uint32_t identite_clip;  //L'identité du clip utilisé (image bitmap)
    uint16_t mode_debordement_largeur;  //0:découpe (=clamp), 1:répétition, 2:miroir, 3:couleur des bords
    uint16_t mode_debordement_hauteur;
    float nbr_debordements_largeur; //Nombre de répétition pour les projection sphériques et cylindriques.
    float nbr_debordements_hauteur;
    LWO2_ParametresSommets* Texture_UV;   //objet UV (structure définie ci-dessus)

    LWO2_Texture()
    {
        chaine_tri=NULL;
        objet_de_reference=NULL;
        active=true;
        fondu_pixels=true;
        systeme_coordonnees=LWO2_REPERE_RELATIF;
        centre[0]=0.;
        centre[1]=0.;
        centre[2]=0.;
        echelle[0]=1.;
        echelle[1]=1.;
        echelle[2]=1.;
        rotation[0]=0.;
        rotation[1]=0.;
        rotation[2]=0.;
    }
    ~LWO2_Texture()
    {
        if (chaine_tri!=NULL) free(chaine_tri);
        if (objet_de_reference!=NULL) free(objet_de_reference);
    }
};

//-------------------------------
//  Surfaces
//  Les enveloppes ne sont pas récupérées.
//--------------------------------

class LWO2_Surface
{
    public:
    char* nom; //Nom de la surface
    char* nom_source; //Le nom de la surface de laquelle est dérivée celle-ci. Le traitement des liens entre les surfaces et leurs sources est effectué après avoir distribué les données LWO2.
    uint32_t taille_chunk; //Taille du chunk SURF
    bool  double_face;    //false si le devant des faces est affiché, true si les deux cotés des faces sont visibles
    float couleur[3];      //COLR Couleur {R,V,B} en nombres flottants
    float luminescence;       //LUMI
    float diffusion;        //DIFF
    float specularite;      //SPEC
    float brillance;        //GLOS
    float reflexion;        //REFL
    float transparence;     //TRAN
    float translucidite;    //TRNL
    float teinte_lumiere;   //CLRH  taux de renvoie de la couleur de la source lumineuse. 0=renvoie toute la couleur de la source, 1.=ne renvoie que la couleur de la surface
    float terminateur;      //SHRP rapidité du fondu du terminateur des ombres (pris en compte par openGL?)
    float rugosite;         //BUMP  //A voir avec les shaders...
    float intensite_halo;             //GLOW ou GVAL - L'épaisseur du halo
    bool  halo;             //true si la surface émet un halo
    bool  adoucissement;    //True si les faces sont adoucies.

    float angle_limite_adoucissement; //l'angle entre les faces adjacentes au-delà duquel la couleur des sommets ne sont plus interpolées.
    vector<LWO2_Texture*> textures;    //Blocs de définition des textures ou des filtres (shaders) seules les définitions de textures UV sont récupérées.

    LWO2_Surface()
    {
        nom=NULL;
        //Valeurs par défaut:
        diffusion=1.0;
        luminescence=0.;
        specularite=0.;
        reflexion=0.;
        transparence=0.;
        teinte_lumiere=0.;
        terminateur=0.5;
        rugosite=1.0;
        brillance=0.4;  //Valeur par défaut donnée par la documentation du format LWO2
        double_face=false;
        adoucissement=false;
        halo=false;

    }
    ~LWO2_Surface()
    {
        if (nom!=NULL)free(nom);
        for(uint32_t i=0;i<textures.size();i++)
        {
            delete textures[i];
        }
    }
};

//----------------------------------------
//          Calques
//----------------------------------------
class LWO2_Calque
{
    public:
        char*       nom;
        uint16_t    index;
        int16_t     parent; //L'index du calque parent. -1 si aucun parent.
        uint16_t    drapeaux;   //bit0=1 : le calque est masqué.
        float       pivot[3];
        float       boite[6];
        uint32_t    nbrSommets;
        double*      sommets;
        vector<LWO2_ParametresSommets*>   parametres_sommets;    //Blocs VMAP
        vector<LWO2_Polygone*> polygones;

    LWO2_Calque()
    {
        nom=NULL;
        sommets=NULL;
        parent=-1;
    }

    ~LWO2_Calque()
    {
        if (nom!=NULL)free(nom);

        if (sommets!=NULL)free(sommets);

        uint32_t i;
        for(i=0 ;i<parametres_sommets.size();i++)
        {
            delete parametres_sommets[i];
        }

        for(i=0 ;i<polygones.size();i++)
        {
            delete polygones[i];
        }
    }

    uint32_t renvoie_nbr_polygones()
    {
        return (uint32_t)polygones.size();
    }

    LWO2_Polygone* renvoie_polygone(uint32_t indice)
    {
        if (indice>=polygones.size()) return NULL;
        return polygones[indice];
    }
};




//------------Définition des blocs, avec respect des hiérarchies: (C'est asser complexe, voir la doc du format LWO2)


//************************************************************************************************
//************************************************************************************************
//                              DEFINITION DE LA CLASSE
//************************************************************************************************
//************************************************************************************************


class dksChargeLWO2: public Fichiers
{
    public:
        int erreur;
    private:
        char* chemin_fichier;
        char*   chemin_images;
        uint32_t nbrCalques;

        uint32_t nbrSurfaces;

        float duree_chargement;



    //+++++++++++++++++++++++ Nouvelle version:
        ifstream fichier_flux;
        uint32_t taille_fichier;
        char* donnees_LWO2;  //Le fichier chargé en mémoire - Le traitement est plus rapide.
        vector<LWO2_Bitmap*> Bitmaps;  //La bibliothèque d'images du fichier LWO2
        vector<LWO2_Calque*> calques_lwo2;  //Les calques contenants les volumes
        vector<LWO2_Surface*> surfaces_lwo2;

        uint32_t*   ptr_lwo2;   //Tête de lecture des données au format LWO2
        uint32_t    taille_bloc,taille_sous_bloc;    //Sert lors de la recherche des sous-blocs
        uint32_t    position,position_bloc,position_sous_bloc;
        vector<char*> table_etiquettes;
        vector<LWO2_ParametresSommets*> table_parametres_sommets; //Les définitions de coordonnées UV utilisées par les textures
        //Arborescence basique du fichier
        char uint32_converti[5];   //Sert pour le debuguage, pour afficher les noms des blocs.


    //=================================================================
    //                  Constructeur
    //=================================================================

    public:
    //Entrées:  nom_fichier: le nom du fichier
    //          chemin_images: le répertoire où se trouvent les images utilisées pour les textures.
    dksChargeLWO2(char* nom_fichier,char* p_chemin_images=NULL)
    {
        taille_fichier=0;
        nbrCalques=0;
        nbrSurfaces=0;
        duree_chargement=0.;

        chemin_fichier=nom_fichier;
        chemin_images=p_chemin_images;

        erreur=DKSCHARGELWO2_OK;

        duree_chargement=glutGet(GLUT_ELAPSED_TIME);

        analyse(); //Traduction du verbe anglais "to parse"

        duree_chargement=glutGet(GLUT_ELAPSED_TIME)-duree_chargement;

        if (DKSCHARGELWO2_DEBUG && DKSCHARGELWO2_DEBUG_DUREES) cout<<"=======>Durée du chargement de l'objet "<<chemin_fichier<<" : "<<((float)duree_chargement)/1000.<<" s"<<endl;
    }

    //============= Destructeur:
    ~dksChargeLWO2()
    {
        uint32_t i;


        if (donnees_LWO2!=NULL) free(donnees_LWO2);
        //Destruction des bitmaps(CLIP):


        for (i=0;i< Bitmaps.size();i++)
        {
            delete Bitmaps[i];
        }

        for (i=0;i< calques_lwo2.size();i++)
        {
            delete calques_lwo2[i];
        }

        for (i=0;i<table_etiquettes.size();i++)
        {
            free(table_etiquettes[i]);
        }


        for (i=0;i<table_parametres_sommets.size();i++)
        {
            delete table_parametres_sommets[i];
        }



        for (i=0;i<surfaces_lwo2.size();i++)
        {
            delete(surfaces_lwo2[i]);
        }


    }


    //======================================================================
    //                Transcryptage
    /// !!! Passer par la classe gestion_fichiers, pour pouvoir basculer de la lecture normale vers la lecture d'une archive
    //======================================================================
private:
    bool analyse()
    {
        if (DKSCHARGELWO2_DEBUG) cout<<"=================  LECTURE DU FICHIER LWO2: ' "<<chemin_fichier<<" ' "<<"==================="<<endl<<endl;

        //------------ Test le format du fichier:

        //---Ouverture fichier:
        fichier_flux.open(chemin_fichier, ios::in | ios::binary | ios::ate);

        if (!fichier_flux.is_open())
        {
            std::cout<<"ERREUR - LWO2 - Impossible d'ouvrir le fichier"<<std::endl;
            erreur=DKSCHARGELWO2_ERREUR_FICHIER;
            return false;
        }
        //---Taille du fichier:
        taille_fichier=fichier_flux.tellg();
        if ( DKSCHARGELWO2_DEBUG ) cout<<"Taille du fichier: "<<taille_fichier<<endl;

        //---Test le format du fichier:
        uint32_t id4_FORM;
        uint32_t taille_lue;
        uint32_t id4_LWO2;

        fichier_flux.seekg(0,ios::beg);

        fichier_flux.read((char*)&id4_FORM,4);
        fichier_flux.read((char*)&taille_lue,4);
        fichier_flux.read((char*)&id4_LWO2,4);

        id4_FORM=retournement_uint32(id4_FORM);
        taille_lue=retournement_uint32(taille_lue);
        id4_LWO2=retournement_uint32(id4_LWO2);

        if ((id4_FORM!=ID_FORM) ||(id4_LWO2!=ID_LWO2))
        {
            cout<<"ERREUR - LWO2 - Le fichier n'est pas au format LWO2"<<endl;
            erreur=DKSCHARGELWO2_ERREUR_FICHIER;
            fichier_flux.close();
            return false;
        }

        if (taille_fichier!=(taille_lue+8))
        {
            cout<<"ERREUR - LWO2 - Le fichier est corrompu - taille fichier : "<<taille_fichier<<" - taille lue : "<<taille_lue<<endl;
            erreur=DKSCHARGELWO2_ERREUR_FICHIER;
            fichier_flux.close();
            return false;
        }

        //--------- copie le fichier en mémoire:
        fichier_flux.seekg(0,ios::beg);
        donnees_LWO2=(char*)malloc(taille_fichier);
        if (donnees_LWO2==NULL)
        {
            cout<<"ERREUR - LWO2 - Mémoire insuffisante pour charger le fichier"<<endl;
            erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
            fichier_flux.close();
            return false;

        }
        fichier_flux.read(donnees_LWO2,taille_fichier);
        fichier_flux.close();

        //-------- Création des structures:
        position=8;
        if (!creation_table_etiquettes() )  { free (donnees_LWO2); return false; }  //TAGS
        if (!creation_bitmaps() )           { free (donnees_LWO2); return false; }  //CLIP
        if (!creation_calques_lwo2())       { free (donnees_LWO2); return false; }  //LAYR
        if (!creation_parametres_sommets()) { free (donnees_LWO2); return false; }  //VMAP - VMAD
        if (!creation_surfaces_lwo2())      { free (donnees_LWO2); return false; }  //SURF

        //-------- Fin:
        if (DKSCHARGELWO2_DEBUG) cout<<endl<<"========== Fichier lu avec succès ! ============"<<endl<<endl<<endl;
        free(donnees_LWO2);
        donnees_LWO2=NULL;
        return true;
    }

    //======================================================================
    //                  Lit les blocs TAGS (étiquettes)
    //======================================================================
    bool creation_table_etiquettes()
    {
        if (DKSCHARGELWO2_DEBUG) cout<<endl<<"---- Création des étiquettes: ----"<<endl;

        uint32_t i;

        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            if(id==ID_TAGS)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                while(position_bloc<taille_bloc)
                {
                    char* etiquette=copie_chaine((char*)ptr_lwo2);
                    if (etiquette==NULL)
                    {
                        cout<<"Erreur dans dksChargeLWO2::creation_table_etiquettes()- Mémoire insuffisante pour créer les chaînes de caractères"<<endl;
                        erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                        return false;
                    }
                    table_etiquettes.push_back(etiquette);
                    position_bloc+=(strlen(etiquette)+2) & 0xfffffe;
                    ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                }

                for (i=0;i<table_etiquettes.size();i++)
                {
                    if (DKSCHARGELWO2_DEBUG) cout<<"        Etiquette: "<<table_etiquettes[i]<<endl;
                }

                position+=taille_bloc;
                return true;
            }
            else position+=2;
        }
        return true;
    }
    //======================================================================
    //                  Lit les blocs CLIP (bitmap)
    //======================================================================
    bool creation_bitmaps()
    {
        if (DKSCHARGELWO2_DEBUG) cout<<endl<<"---- Création des bitmaps ----"<<endl;

        uint32_t    identifiant;
        //---- Lecture des blocs:
        while(position<(taille_fichier-4))
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            if(retournement_uint32(*ptr_lwo2)==ID_CLIP)
            {
                position_bloc=0;
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute CLIP
                taille_bloc=retournement_uint32(*ptr_lwo2)+8; //+8 car je compte CLIP et sa taille
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute la taille du bloc
                identifiant=retournement_uint32(*ptr_lwo2);               //L'identifiant du clip
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                //Ne lit que les blocs de type STIL:
                if (retournement_uint32(*ptr_lwo2)==ID_STIL)
                {
                    position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                    taille_sous_bloc=retournement_uint16(*(uint16_t*)ptr_lwo2);
                    position_bloc+=2; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                    char* fichier_image=copie_chaine((char*)ptr_lwo2);
                    if (fichier_image==NULL)
                    {
                        cout<<"Erreur dans dksChargeLWO2::creation_bitmaps() 1 - Mémoire insuffisante pour créer les chaînes de caractères"<<endl;
                        erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                        return false;
                    }
                    position_bloc+= taille_sous_bloc; //On saute le bloc STIL

                    LWO2_Bitmap* bitmap=new LWO2_Bitmap();
                    Bitmaps.push_back(bitmap);
                    bitmap->identite=identifiant;

                    //Initialise le chemin du fichier:

                    if (chemin_images!=NULL)
                    {
                        recupere_nom_fichier(fichier_image);
                        bitmap->fichier=(char*)malloc(strlen(fichier_image)+strlen(chemin_images)+1);
                        if (bitmap->fichier==NULL)
                        {
                            cout<<"Erreur dans dksChargeLWO2::creation_bitmaps() 2 - Mémoire insuffisante pour créer les chaînes de caractères"<<endl;
                            erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                            return false;
                        }
                        strcpy(bitmap->fichier,chemin_images);
                        strcat(bitmap->fichier,fichier_image);
                        free(fichier_image);
                    }

                    else  bitmap->fichier=fichier_image;

                    //Chargement de l'image:
                    bitmap->image=new Image(bitmap->fichier);
                    bitmap->image->renversement_horizontal();

                    //recherche les sous-blocs de réglage de l'image:

                    while (position_bloc<taille_bloc-4)
                    {
                        ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                        uint16_t drapeau;
                        switch (retournement_uint32(*ptr_lwo2))
                        {
                            case ID_CONT:
                                bitmap->contraste=recupere_sous_bloc_FP4();
                                if (DKSCHARGELWO2_DEBUG) cout<<"Contraste:"<<bitmap->contraste<<endl;
                                break;
                            case ID_BRIT:
                                bitmap->luminosite=recupere_sous_bloc_FP4();
                                if (DKSCHARGELWO2_DEBUG) cout<<"Luminosite:"<<bitmap->luminosite<<endl;
                                break;
                            case ID_SATR:
                                bitmap->saturation=recupere_sous_bloc_FP4();
                                if (DKSCHARGELWO2_DEBUG) cout<<"Saturation:"<<bitmap->saturation<<endl;
                                break;
                            case ID_HUE:
                                bitmap->decallage_teinte=recupere_sous_bloc_FP4();
                                if (DKSCHARGELWO2_DEBUG) cout<<"Décallage de teinte:"<<bitmap->decallage_teinte<<endl;
                                break;
                            case ID_GAMM:
                                bitmap->gamma=recupere_sous_bloc_FP4();
                                if (DKSCHARGELWO2_DEBUG) cout<<"Gamma:"<<bitmap->gamma<<endl;
                                break;
                            case ID_NEGA:
                                drapeau=recupere_sous_bloc_U2();
                                if (drapeau!=0)
                                {
                                    bitmap->negatif=true;
                                    if (DKSCHARGELWO2_DEBUG) cout<<"Image en négatif"<<endl;
                                }
                                break;
                            default:
                                position_bloc+=2;
                                break;

                        }

                    }

                    //Fin, on cherche le bloc CLIP suivant:
                    position+=position_bloc;
                }

            }

            else position+=2;
        }

        return true;
    }

    //======================================================================
    //                  Lit les blocs LAYR (calque)
    //======================================================================
    static bool comparaison_tri_calques(LWO2_Calque* t1, LWO2_Calque* t2) { return t1->index < t2->index; }

    bool creation_calques_lwo2()
    {
        if (DKSCHARGELWO2_DEBUG) cout<<endl<<"---- Création des calques ----"<<endl;
        position=8;

        //---- Lecture des blocs:
        while(position<(taille_fichier-4))
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            if(retournement_uint32(*ptr_lwo2)==ID_LAYR)
            {
                position_bloc=0;
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute LAYR
                taille_bloc=retournement_uint32(*ptr_lwo2)+8; //+8 car je compte LAYR et sa taille
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute la taille du bloc

                LWO2_Calque* calque=new LWO2_Calque();
                calques_lwo2.push_back(calque);

                calque->index=retournement_uint16(*(uint16_t*)ptr_lwo2);
                position_bloc+=2; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                calque->drapeaux=retournement_uint16(*(uint16_t*)ptr_lwo2);
                position_bloc+=2; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                recupere_vecteur_3f(calque->pivot);
                calque->nom=copie_chaine((char*)ptr_lwo2);

                position_bloc+=(strlen(calque->nom)+2) & 0x7ffffffe; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                if ((taille_bloc-position_bloc)>=2)
                {
                    calque->parent=(int16_t)retournement_uint16(*(int16_t*)ptr_lwo2);
                }

                position+=taille_bloc;

                //---- Lecture des objets propres au calque:
                uint32_t    position_LAYR=position;
                if(!creation_points(calque)) return false;
                position=position_LAYR;
                if(!creation_polygones(calque)) return false;

                //---- Affiche les données:
                if(DKSCHARGELWO2_DEBUG)
                {
                    cout<<"** Calque : "<<calque->index<<endl;
                    cout<<"   Nom: "<<calque->nom<<endl;
                    if (calque->parent==-1)cout<<"   Parent: aucun"<<endl;
                    else cout<<"   Parent: "<<dec<<calque->parent<<endl;
                    cout<<"   Drapeaux:"<<hex<<calque->drapeaux<<endl;
                    cout<<dec;
                    cout<<"   Pivot - X: "<<calque->pivot[0]<<" - Y: "<<calque->pivot[1]<<" - Z: "<<calque->pivot[2]<<endl;
                    cout<<"   Polygones: "<<calque->polygones.size()<<endl;
                    cout<<"   Sommets: "<<calque->nbrSommets<<endl;

                }

            }
            else { position+=2;}
        }
        sort(calques_lwo2.begin(),calques_lwo2.end(),comparaison_tri_calques);
        return true;
    }

    //======================================================================
    //                  Lit les blocs PNTS (sommets)
    ///         !!! Convertis les nombre en double précision !!!
    //======================================================================
    bool creation_points(LWO2_Calque* calque)
    {
        uint32_t i;
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            if(id==ID_PNTS)
            {
                position_bloc=4;
                ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                calque->nbrSommets=retournement_uint32(*ptr_lwo2);
                calque->nbrSommets/=12;
                position_bloc+=4; ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                //Allocation des sommets:
                calque->sommets=(double*)malloc(calque->nbrSommets*8*3);

                if(calque->sommets==NULL)
                {
                    cout<<"Erreur dans dksChargeLWO2::creation_points() - Mémoire insuffisante"<<endl;
                    erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                    return false;
                }

                //Récupération des coordonnées:
                for(i=0;i<calque->nbrSommets;i++)
                {
                    recupere_vecteur_3f_convertion_3d(&calque->sommets[i*3]);
                }

                position+=calque->nbrSommets*12+8;
                return true;
            }

            //Si on sort du calque actuel:
            else if (id==ID_LAYR)
            {
                if (DKSCHARGELWO2_DEBUG) cout<<"Avertissement: Le calque "<<calque->index<<" ne contient pas de points"<<endl;
                return true;
            }

            else position+=2;
        }

        //Fin des données, aucun sommet trouvé:
        if (DKSCHARGELWO2_DEBUG) cout<<"Avertissement: Le calque "<<calque->index<<" ne contient pas de points"<<endl;
        return true;
    }

    //======================================================================
    //                  Lit les blocs POLS (polygones)
    //======================================================================
    bool creation_polygones(LWO2_Calque* calque)
    {
        uint32_t i,id;
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            id=retournement_uint32(*ptr_lwo2);
            if(id==ID_POLS)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                uint32_t type_polygones=retournement_uint32(*ptr_lwo2);
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                if (type_polygones==ID_FACE)
                {
                    vector<LWO2_Polygone*> table_polygones; //Table provisoire. Elle est insérée à la table des polygones du calque
                                                            //après la création des liens avec les étiquettes (surfaces, part, groupes de filtre)

                    while (position_bloc<taille_bloc)
                    {
                        LWO2_Polygone* polygone=new LWO2_Polygone();
                        table_polygones.push_back(polygone);
                        polygone->nbrSommets=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2)& 0x3ff; //Masquage des drapeaux
                        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                        //Allocation de la table des indices de sommets:
                        polygone->indices_sommets=(uint32_t*)malloc(polygone->nbrSommets*4);

                        if (polygone->indices_sommets==NULL)
                        {
                            cout<<"Erreur dans dksChargeLWO2::creation_polygones() - Mémoire insuffisante"<<endl;
                            erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                            return false;
                        }
                        //Récupération des indices de sommets:
                        for (i=0;i<polygone->nbrSommets;i++)
                        {
                                polygone->indices_sommets[i]=recupere_VX();
                        }
                    }

                    position+=taille_bloc;

                    //Lecture des liens avec les surfaces ou les groupements de polygones:
                    if ( !creation_liens_polygones(&table_polygones) )
                    {
                        for (i=0;i<table_polygones.size();i++)
                        {
                             delete table_polygones[i];
                        }
                        return false;
                    }

                    //Copie la nouvelle table de polygones dans le calque:
                    for (i=0;i<table_polygones.size();i++)
                    {
                         calque->polygones.push_back(table_polygones[i]);
                    }


                }
                else position+=taille_bloc;

            }
            //Test si on arrive au calque suivant:
            else if (id==ID_LAYR)
            {
                return true;
            }
            else
            {
                position+=2;
            }
        }
        return true;
    }
    //======================================================================
    //                  Lit les blocs PTAG (liens polygones)
    //======================================================================
    bool creation_liens_polygones(vector<LWO2_Polygone*>* table_polygones)
    {
        uint32_t indice_polygone;
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            if(id==ID_PTAG)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                uint32_t type_lien=retournement_uint32(*ptr_lwo2); //SURF, PART ou SMGP
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                if (type_lien==ID_SURF)
                {
                    while(position_bloc<taille_bloc)
                    {
                        indice_polygone=recupere_VX();
                        (*table_polygones)[indice_polygone]->indice_etiquette_surface=retournement_uint16(*(uint16_t*)ptr_lwo2);
                        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                    }
                }
                else if (type_lien==ID_PART)
                {
                    while(position_bloc<taille_bloc)
                    {
                        indice_polygone=recupere_VX();
                        (*table_polygones)[indice_polygone]->indice_etiquette_part=retournement_uint16(*(uint16_t*)ptr_lwo2);
                        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                    }
                }
                else if (type_lien==ID_SMGP)
                {
                    while(position_bloc<taille_bloc)
                    {
                        indice_polygone=recupere_VX();
                        (*table_polygones)[indice_polygone]->indice_etiquette_groupe_filtre=retournement_uint16(*(uint16_t*)ptr_lwo2);
                        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                    }
                }
                else if(type_lien==ID_COLR)
                {

                }
                else
                {
                    cout<<"Avertissement dans dksChargeLWO2::creation_liens_polygones() - Type de lien inconnu: "<<hex<<type_lien<<endl;
                    cout<<dec;
                }
                position+=taille_bloc;
            }
            //Test si on passe à une autre table de définition de polygones ouy carrément à un autre calque:
            else if (id==ID_POLS || id==ID_LAYR) return true;
            //Parcours les données:
            else position+=2;
        }
        return true;
    }

    //======================================================================
    //                  Création des paramètres de sommets (VMAP et VMAD)
    //======================================================================
    bool creation_parametres_sommets()
    {
        #if DKSCHARGELWO2_DEBUG
         cout<<endl<<"---- Création des coordonnées UV ----"<<endl;
        #endif
        uint32_t i;
        LWO2_ParametresSommets* parametres_UV;
        position=8;
        uint32_t type_texture; //Seul le type TXUV est pris en compte

        //Lecture des blocs VMAP:
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            if(id==ID_IMAP)    //On saute les blocs IMAP car ils peuvent contenir un identifiant VMAP, ce qui entrainerait des confusions
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2);
                position+=8+taille_bloc;
            }
            else if(id==ID_VMAP)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                type_texture=retournement_uint32(*ptr_lwo2);

                if (type_texture==ID_TXUV)
                {
                    position_bloc+=6;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute type+dimension
                    parametres_UV= new LWO2_ParametresSommets();
                    table_parametres_sommets.push_back(parametres_UV);
                    parametres_UV->nom=copie_chaine((char*)ptr_lwo2);
                    #if DKSCHARGELWO2_DEBUG_UV
                        cout<<parametres_UV->nom<<endl;
                    #endif
                    if (parametres_UV->nom==NULL)
                    {
                        cout<<"Erreur dans Charge_LWO2::creation_parametres_sommets() - Mémoire insuffisante"<<endl;
                        erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                        return false;
                    }
                    position_bloc+=(strlen(parametres_UV->nom)+2) & 0xfffffffe;
                    ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                    if (DKSCHARGELWO2_DEBUG) cout<<"*** Objet UV: "<<parametres_UV->nom<<endl;

                    while (taille_bloc>position_bloc)
                    {
                        LWO2_Sommet_UV* sommet_UV=ajoute_LWO2_sommet_UV(parametres_UV);
                        sommet_UV->indice_sommet=recupere_VX();
                        sommet_UV->U=retournement_float(*(float*)ptr_lwo2);
                        position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                        sommet_UV->V=retournement_float(*(float*)ptr_lwo2);
                        position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                        #if (DKSCHARGELWO2_DEBUG && DKSCHARGELWO2_DEBUG_UV)
                            cout<<"   "<<parametres_UV->sommets_UV.size()<<" : Sommet: "<<sommet_UV->indice_sommet<<" U="<<sommet_UV->U<<"  V="<<sommet_UV->V<<endl;
                        #endif
                    }
                }

                position+=taille_bloc;
            }
            else position+=2;
        }

        //------------- Lecture des blocs VMAD éventuels (création des coutures):
        position=8;
        //Lecture des blocs VMAP:
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            if(id==ID_VMAD)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                type_texture=retournement_uint32(*ptr_lwo2);

                if (type_texture==ID_TXUV)
                {
                    position_bloc+=6;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc); //On saute type+dimension

                    parametres_UV=NULL;
                    //Recherche l'objet LWO2_parametres_sommets correcpondant:
                    for (i=0;i<table_parametres_sommets.size();i++)
                    {
                        if (strcmp((char*)ptr_lwo2,table_parametres_sommets[i]->nom)==0)
                        {
                            parametres_UV=table_parametres_sommets[i];
                            break;
                        }
                    }

                    if (parametres_UV!=NULL)
                    {
                        parametres_UV->couture=true;
                        position_bloc+=(strlen((char*)ptr_lwo2)+2) & 0xfffffffe;
                        ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                        if (DKSCHARGELWO2_DEBUG) cout<<"*** Objet UV de couture: "<<parametres_UV->nom<<endl;

                        while (taille_bloc>position_bloc)
                        {
                            LWO2_Sommet_UV_couture* sommet_UV=ajoute_LWO2_sommet_UV_couture(parametres_UV);
                            sommet_UV->indice_sommet=recupere_VX();
                            sommet_UV->indice_polygone=recupere_VX();
                            sommet_UV->U=retournement_float(*(float*)ptr_lwo2);
                            position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                            sommet_UV->V=retournement_float(*(float*)ptr_lwo2);
                            position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                            if (DKSCHARGELWO2_DEBUG && DKSCHARGELWO2_DEBUG_UV) cout<<"   "<<parametres_UV->sommets_UV.size()<<" : Sommet: "<<sommet_UV->indice_sommet<<" : polygone: "<<sommet_UV->indice_polygone<<" U="<<sommet_UV->U<<"  V="<<sommet_UV->V<<endl;
                        }
                    }

                }

                position+=taille_bloc;
            }
            else position+=2;
        }
        return true;
    }

    //Ajoute un sommet à une table de coordonnées UV:
    LWO2_Sommet_UV* ajoute_LWO2_sommet_UV (LWO2_ParametresSommets* parametres_UV)
    {
        LWO2_Sommet_UV sommet;
        parametres_UV->sommets_UV.push_back(sommet);
        return &(parametres_UV->sommets_UV[parametres_UV->sommets_UV.size()-1]);
    }

    //Ajoute un sommet de couture à une table de coordonnées UV:
    LWO2_Sommet_UV_couture* ajoute_LWO2_sommet_UV_couture (LWO2_ParametresSommets* parametres_UV)
    {
        LWO2_Sommet_UV_couture sommet;
        parametres_UV->sommets_UV_couture.push_back(sommet);
        return &(parametres_UV->sommets_UV_couture[parametres_UV->sommets_UV_couture.size()-1]);
    }

    //======================================================================
    //                  Création des surfaces
    //======================================================================
    bool creation_surfaces_lwo2()
    {
        if (DKSCHARGELWO2_DEBUG) cout<<endl<<"---- Création des Surfaces ----"<<endl;
        position=8;
        //Lecture des blocs SURF:
        while(position<taille_fichier)
        {
            ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
            uint32_t id=retournement_uint32(*ptr_lwo2);
            //On saute les blocs PTAG qui peuvent contenir un identifiant SURF
            if(id==ID_PTAG)
            {
                position+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position);
                taille_bloc=retournement_uint32(*ptr_lwo2);
                position+=taille_bloc+4;
            }
            else if(id==ID_SURF)
            {
                position_bloc=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                taille_bloc=retournement_uint32(*ptr_lwo2)+8;
                position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                LWO2_Surface* surface=new LWO2_Surface();
                surfaces_lwo2.push_back(surface);

                //Récupère le nom de la surface:
                surface->nom=copie_chaine((char*)ptr_lwo2);
                if (surface->nom==NULL)
                {
                    cout<<"Erreur dans dksChargeLWO2::creation_surfaces_lwo2() - Mémoire insuffisante"<<endl;
                    erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                    return false;
                }

                if (DKSCHARGELWO2_DEBUG && DKSCHARGELWO2_DEBUG_SURFACES)
                {
                    cout<<"  ****** Surface: "<<surface->nom<<" - Taille chunk: "<<taille_bloc<<endl;
                }

                position_bloc+=(strlen((char*)ptr_lwo2)+2) & 0xfffffffe;
                ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                //Récupère le nom de la source:
                surface->nom_source=copie_chaine((char*)ptr_lwo2);
                if (surface->nom_source==NULL)
                {
                    cout<<"Erreur dans dksChargeLWO2::creation_surfaces_lwo2() - Mémoire insuffisante"<<endl;
                    erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
                    return false;
                }
                position_bloc+=(strlen((char*)ptr_lwo2)+2) & 0xfffffffe;
                ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);

                //Lecture des sous-blocs de la surface:

                while(position_bloc<taille_bloc)
                {
                    ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                   id=retournement_uint32(*ptr_lwo2);
                   position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                   taille_sous_bloc=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2);
                   position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
                   switch (id)
                   {
                        case ID_COLR:
                            surface->couleur[0]=retournement_float(*((float*)ptr_lwo2));
                            surface->couleur[1]=retournement_float(*((float*)ptr_lwo2+1));
                            surface->couleur[2]=retournement_float(*((float*)ptr_lwo2+2));
                        break;
                        case ID_DIFF:
                            surface->diffusion=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_LUMI:
                            surface->luminescence=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_SPEC:
                            surface->specularite=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_REFL:
                            surface->reflexion=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_TRAN:
                            surface->transparence=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_TRNL:
                            surface->translucidite=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_GLOS:
                            surface->brillance=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_SHRP:
                            surface->terminateur=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_BUMP:
                            surface->rugosite=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_SIDE:
                            if(retournement_uint16(*((uint16_t*)ptr_lwo2))==3) surface->double_face=true;
                        break;
                        case ID_SMAN:
                            surface->angle_limite_adoucissement=retournement_float(*((float*)ptr_lwo2));    //En radians
                            if (surface->angle_limite_adoucissement>0.) surface->adoucissement=true;
                        break;
                        case ID_RFOP:
                            //cout<<"RFOP"<<endl;
                        break;
                        case ID_RIMG:
                            //cout<<"RIMG"<<endl;
                        break;
                        case ID_RSAN:
                            //cout<<"RSAN"<<endl;
                        break;
                        case ID_RBLR:
                            //cout<<"RBLR"<<endl;
                        break;
                        case ID_RIND:
                            //cout<<"RIND"<<endl;
                        break;
                        case ID_TROP:
                            //cout<<"TROP"<<endl;
                        break;
                        case ID_TIMG:
                            //cout<<"TIMG"<<endl;
                        break;
                        case ID_TBLR:
                            //cout<<"TBLR"<<endl;
                        break;
                        case ID_CLRH:
                            surface->teinte_lumiere=retournement_float(*((float*)ptr_lwo2));
                        break;
                        case ID_CLRF:
                            //cout<<"CLRF"<<endl;
                        break;
                        case ID_ADTR:
                            //cout<<"ADTR"<<endl;
                        break;
                        case ID_GLOW:
                            //cout<<"GLOW"<<endl;
                        break;
                        case ID_GVAL:
                            surface->intensite_halo=retournement_float(*((float*)ptr_lwo2));
                            if (surface->intensite_halo>0.)surface->halo=true;
                        break;
                        case ID_LINE:
                            //cout<<"LINE"<<endl;
                        break;
                        case ID_ALPH:
                            //cout<<"ALPH"<<endl;
                        break;
                        case ID_VCOL:
                            //cout<<"VCOL"<<endl;
                        break;
                        case ID_VERS:
                            //cout<<"VERS"<<endl;
                        break;
                        case ID_NODS:
                            //cout<<"NODS"<<endl;
                        break;
                        //Création des textures (BLOK):
                        case ID_BLOK:
                            if(!creation_textures_lwo2(surface)) return false;
                        break;
                        //Si aucun identifiant reconnu:
                        default:
                            cout<<"Bloc de surface inconnu : "<<hex<<id<<endl;
                            cout<<dec;
                        break;
                   }
                   position_bloc+=taille_sous_bloc;
                }
                position+=taille_bloc;
            }
            else position+=2;
        }
        return true;
    }

    //======================================================================
    //                  Création des textures (BLOK)
    //  Seuls les bloc de type IMAP avec les projections UV (TXUV) sont pris en compte.
    //======================================================================
    bool creation_textures_lwo2(LWO2_Surface* surface)
    {
        uint32_t temp32;
        #if DKSCHARGELWO2_DEBUG
            cout<<"         .Texture - "<<endl;
        #endif
        position_sous_bloc=0;
        uint32_t id=retournement_uint32(*ptr_lwo2);
        uint32_t taille_sous_bloc_BLOK;
        LWO2_Texture* texture=new LWO2_Texture();
        surface->textures.push_back(texture);
        texture->type=id;
        position_sous_bloc+=6;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc+position_sous_bloc);
        texture->chaine_tri=copie_chaine((char*)ptr_lwo2);
        #if (DKSCHARGELWO2_DEBUG && DKSCHARGELWO2_DEBUG_TEXTURES)

            cout<<"               Chaîne de tri: "<<hex;
            uint32_t taille_chaine=strlen(texture->chaine_tri);
            for (uint16_t i=0 ; i<taille_chaine;i++)
            {
                cout<<(((uint32_t)texture->chaine_tri[i]) & 0xff);
            }
            cout<<dec<<endl;
        #endif
        if(texture->chaine_tri==NULL)
        {
            cout<<"Erreur dans dksChargeLWO2::creation_textures_lwo2() - Mémoire insuffisiante"<<endl;
            erreur=DKSCHARGELWO2_ERREUR_MEMOIRE;
            return false;
        }
        position_sous_bloc+=(strlen(texture->chaine_tri)+2) & 0xfffffffe;
        ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc+position_sous_bloc);

        //Lecture des paramètres de la texture:
        if (texture->type==ID_IMAP)
        {
            while(position_sous_bloc<taille_sous_bloc)
            {
                ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc+position_sous_bloc);
                id=retournement_uint32(*ptr_lwo2);
                position_sous_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc+position_sous_bloc);
                taille_sous_bloc_BLOK=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2);
                position_sous_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc+position_sous_bloc);
                switch(id)
                {
                    case ID_CHAN:
                        texture->canal=retournement_uint32(*ptr_lwo2);
                        texture->nom_canal[4]=0;
                        texture->nom_canal[0]=(char)(texture->canal>>24);
                        texture->nom_canal[1]=(char)(texture->canal>>16);
                        texture->nom_canal[2]=(char)(texture->canal>>8);
                        texture->nom_canal[3]=(char)(texture->canal);
                        break;
                    case ID_ENAB:
                        if(retournement_uint16(*(uint16_t*)ptr_lwo2)==0) texture->active=false;
                        break;
                    case ID_OPAC:
                        //cout<<"OPAC"<<endl;
                        break;
                    case ID_AXIS:
                        texture->axe_projection=retournement_uint16(*(uint16_t*)ptr_lwo2);
                        break;
                    case ID_PROJ:
                        texture->type_projection=retournement_uint16(*(uint16_t*)ptr_lwo2);//Pour le moment seul le type UV (5) est pris en compte.
                        break;
                    break;
                    case ID_IMAG:
                        texture->identite_clip=recupere_VX_sans_decallage_ptr();
                        break;
                    case ID_WRAP:
                        temp32=retournement_uint32(*ptr_lwo2);
                        texture->mode_debordement_largeur=temp32>>16;
                        texture->mode_debordement_hauteur=temp32 & 0xffff;
                        break;
                    case ID_WRPW:
                        texture->nbr_debordements_largeur=retournement_float(*(float*)ptr_lwo2);
                        break;
                    case ID_WRPH:
                        texture->nbr_debordements_hauteur=retournement_float(*(float*)ptr_lwo2);
                        break;
                    case ID_VMAP:
                        texture->Texture_UV=renvoie_ParametresSommets((char*)ptr_lwo2);
                        break;

                    case ID_AAST:
                        //cout<<"AAST"<<endl;
                        break;

                    case ID_PIXB:
                        if(retournement_uint16(*(uint16_t*)ptr_lwo2)==0)texture->fondu_pixels=false;
                        break;

                    case ID_STCK:
                        //cout<<"STCK"<<endl;
                        break;

                    case ID_TAMP:
                        texture->amplitude_rugosite=retournement_float(*(float*)ptr_lwo2);
                        break;

                    case ID_NEGA:
                        //cout<<"NEGA"<<endl;
                        break;

                    //Les sous-blocs suivants TMAP sont inclus dans la taille du bloc TMAP...A surveiller.
                    case ID_TMAP:
                        //cout<<"TMAP"<<endl;
                        break;
                            case ID_CNTR:
                                recupere_vecteur_3f_sans_decallage_ptr(texture->centre);
                                break;
                            case ID_SIZE:
                                recupere_vecteur_3f_sans_decallage_ptr(texture->echelle);
                                break;
                            case ID_ROTA:
                                recupere_vecteur_3f_sans_decallage_ptr(texture->rotation);
                                break;
                            case ID_OREF:
                                //cout<<"OREF"<<endl;
                                break;
                            case ID_FALL:
                                //cout<<"FALL"<<endl;
                                break;
                            case ID_CSYS:
                                //cout<<"CSYS"<<endl;
                                texture->systeme_coordonnees=retournement_uint16(*(uint16*)ptr_lwo2); //Repère de référence pour les textures projetées
                                break;

                    default:
                        cout<<"Bloc de texture inconnu : "<<hex<<id<<endl;
                        cout<<dec;
                    break;
                }
                position_sous_bloc+=taille_sous_bloc_BLOK;
            }
        }
        //Tri les textures:

        //Débuguage:
        #if DKSCHARGELWO2_DEBUG
            cout<<"               Canal: "<<converti_uint32_chaine(texture->canal);
            cout<<endl;
        #endif
        return true;
    }

    //------------------------------------------------------------------------------------
    //      Récupère un indice de type 'VX' :
    //      Si le premier octet est à 0xFF, l'indice s'étend sur les 3 octets suivants (24 bits)
    //      Sinon l'indice s'étend sur l'octet testé plus le suivant (16 bits)
    //------------------------------------------------------------------------------------

    uint32_t recupere_VX()
    {
        uint32_t donnee;
        if( *(uint8_t*)ptr_lwo2==0xff ) {donnee=retournement_uint32(*ptr_lwo2) & 0xffffff; position_bloc+=4;}
        else { donnee=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2) ; position_bloc+=2;}
        ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        return donnee;
    }

     uint32_t recupere_VX_sans_decallage_ptr()
    {
        uint32_t donnee;
        if( *(uint8_t*)ptr_lwo2==0xff ) {donnee=retournement_uint32(*ptr_lwo2) & 0xffffff;}
        else { donnee=(uint32_t)retournement_uint16(*(uint16_t*)ptr_lwo2) ;}
        return donnee;
    }


    //------------------------------------------------------------------------------------
    //      Récupère le nombre flottant dans un sous-bloc ne comportant que cette donnée
    //      Positionne le pointeur sur la fin du bloc.
    //------------------------------------------------------------------------------------
    float recupere_sous_bloc_FP4()
    {
        position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        uint16_t taille=retournement_uint16(*(uint16_t*)ptr_lwo2);
        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        float donnee=retournement_float(*(float*)ptr_lwo2);
        position_bloc+=taille;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        return donnee;
    }

    //------------------------------------------------------------------------------------
    //      Récupère le nombre 16bits dans un sous-bloc ne comportant que cette donnée
    //      Positionne le pointeur sur la fin du bloc.
    //------------------------------------------------------------------------------------
    uint16_t recupere_sous_bloc_U2()
    {
        position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        uint16_t taille=retournement_uint16(*(uint16_t*)ptr_lwo2);
        position_bloc+=2;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        uint16_t donnee=retournement_uint16(*(uint16_t*)ptr_lwo2);
        position_bloc+=taille;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        return donnee;
    }
    //------------------------------------------------------------------------------------
    //      Récupère des coordonnées tridimensionnelles
    //      Positionne le pointeur sur la fin du vecteur.
    //------------------------------------------------------------------------------------
    void recupere_vecteur_3f(float* coordonnees)
    {
        for(uint16_t i=0;i<3;i++)
        {
            coordonnees[i]=retournement_float(*(float*)ptr_lwo2);
            position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        }
    }

    void recupere_vecteur_3f_sans_decallage_ptr(float* coordonnees)
    {
        uint32_t position_vecteur=0;
        for(uint16_t i=0;i<3;i++)
        {
            coordonnees[i]=retournement_float(*(float*)(ptr_lwo2+position_vecteur));
            position_vecteur+=4;
        }
    }

    //Le vecteur est convertis en double précision:
    void recupere_vecteur_3f_convertion_3d(double* coordonnees)
    {
        for(uint16_t i=0;i<3;i++)
        {
            coordonnees[i]=(double)retournement_float(*(float*)ptr_lwo2);
            position_bloc+=4;ptr_lwo2=(uint32_t*)(donnees_LWO2+position+position_bloc);
        }

    }

    //=================================================================
    //                 Fonctions utiles
    //=================================================================
private:
    char* converti_uint32_chaine(uint32_t id)
    {
        uint32_converti[0]=(char)(id>>24)&0xff;
        uint32_converti[1]=(char)(id>>16)&0xff;
        uint32_converti[2]=(char)(id>>8)&0xff;
        uint32_converti[3]=(char)id&0xff;
        uint32_converti[4]=0;
        return uint32_converti;
    }
public:
    char* renvoie_nom_fichier()
    {
        return chemin_fichier;
    }

    LWO2_Calque* renvoie_LWO2_calque(uint32_t numCalque)
    {
        if (numCalque>=calques_lwo2.size()) return NULL;
        return calques_lwo2[numCalque];
    }

    LWO2_Calque* renvoie_LWO2_calque_nom(char* nom_calque)
    {
        uint32_t i;
        for(i=0;i<calques_lwo2.size();i++)
        {
            if(strcmp(calques_lwo2[i]->nom,nom_calque)==0) return calques_lwo2[i];
        }
        return NULL;
    }

    uint32_t renvoie_nbrBitmaps()
    {
        return (uint32_t)Bitmaps.size();
    }

    uint32_t renvoie_nbrSurfaces()
    {
        return (uint32_t)surfaces_lwo2.size();
    }

    uint32_t renvoie_nbrCalques()
    {
        return (uint32_t)calques_lwo2.size();
    }

    LWO2_Surface* renvoie_LWO2_surface(uint32_t indice)
    {
        if(indice>=surfaces_lwo2.size()) return NULL;
        return surfaces_lwo2[indice];
    }

    LWO2_Surface* renvoie_LWO2_surface(char* nom)
    {
        for (uint32_t i=0; i<surfaces_lwo2.size();i++)
        {
            if (strcmp(nom,surfaces_lwo2[i]->nom)==0) return surfaces_lwo2[i];
        }
        return NULL;
    }

    char* renvoie_etiquette(uint32_t indice)
    {
        if ( table_etiquettes.size()<= indice ) return NULL;
        return table_etiquettes[indice];
    }

    //Renvoie le bitmap correspondant à l'identifiant
    LWO2_Bitmap* renvoie_LWO2_bitMap_id(uint32_t id)
    {
        uint32_t i;
        LWO2_Bitmap* bitmap;
        for (i=0;i<Bitmaps.size();i++)
        {
            bitmap=Bitmaps[i];
            if (bitmap->identite == id) return bitmap;
        }
        return NULL;
    }

    //Renvoie le bitmap correspondant à l'index dans le tableau des bitmaps
    LWO2_Bitmap* renvoie_LWO2_bitmap_index(uint32_t indice)
    {
        if(indice>=Bitmaps.size()) return NULL;
        return Bitmaps[indice];
    }

    //Génère les objets textures d'openGl à partir des images:
    /*
    bool genere_textures_OpenGl(bool drapeau_mipmap=false)
    {
        uint32_t i;
        for (i=0;i<Bitmaps.size();i++)
        {
            if (!Bitmaps[i]->image->genere_texture_openGL(drapeau_mipmap)) return false;
        }
        return true;
    }
    */

    //------------------------------------------------------------------------------------
    //      Renvoie l'objet Parametres_Sommets en fonction du nom spécifié en entrée
    //      Renvoie NULL si aucun objet ne porte ce nom.
    //------------------------------------------------------------------------------------
    LWO2_ParametresSommets* renvoie_ParametresSommets(char* nom_cherche)
    {
        for(uint32_t i=0;i<table_parametres_sommets.size();i++)
        {
            if (strcmp(nom_cherche,table_parametres_sommets[i]->nom)==0) return table_parametres_sommets[i];
        }
        return NULL;
    }


};
#endif // CHARGE_LWO2_H_INCLUDED
