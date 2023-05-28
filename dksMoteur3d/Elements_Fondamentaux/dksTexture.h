#ifndef DKSTEXTURE_H_INCLUDED
#define DKSTEXTURE_H_INCLUDED

//----------------------------------
// Classe de gestion des textures
//
//  Une même texture peut servir à plusieurs surfaces.
///  Le tableau des coordonnées UV en entrée est recopié.
///  Le programme appelant peut donc le supprimer après construction de la texture.
//
//
//----------------------------------

//Types de texture:
#define DKSTEXTURE_CANAL_INVALIDE 0
#define DKSTEXTURE_CANAL_COULEUR 1
#define DKSTEXTURE_CANAL_AMBIANCE 2
#define DKSTEXTURE_CANAL_DIFFUSION 3
#define DKSTEXTURE_CANAL_LUMINESCENCE 4
#define DKSTEXTURE_CANAL_TRANSPARENCE 5
#define DKSTEXTURE_CANAL_REFLEXION 6
#define DKSTEXTURE_CANAL_RUGOSITE 7

//Mode de projection:
#define DKSTEXTURE_PROJECTION_UV 1
#define DKSTEXTURE_PROJECTION_PLAN 2
#define DKSTEXTURE_PROJECTION_ENVIRONNEMENT 3

//Erreurs:
#define DKSTEXTURE_OK 0
#define DKSTEXTURE_ERREUR_MEMOIRE 1

//La structure de définition des sommets UV, lors de la création du volume
class dksSommetUV
{
    public:
        uint32_t indice_sommet;
        float   U;
        float   V;
};

//------ Classe des textures:

class dksTexture
{
    public:
    static Image* image_transparente;   //Pour faire disparaitre les textures.

    char nom[DKS_TAILLE_NOMS];
    uint16_t erreur;
    uint8_t canal;   //Couleur, diffusion, réflexion...
    uint8_t type_projection; //UV, plan ou réflection de l'environnement
    float luminescence; //Sert dans les cas de transition d'une texture à une autre.

    //Paramètres de rendu:
    GLint type_enveloppe;   //GL_MODULATE, GL_REPLACE, GL_DECAL, GL_BLEND, GL_ADD, GL_COMBINE
    GLint recouvrement_s;   //GL_CLAMP, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT
    GLint recouvrement_t;
    GLint type_grossissement;    //GL_LINEAR, GL_NEAREST
    GLint type_diminution;

    //Paramètres pour les coordonnées générées (autres que UV)
    GLenum mode_projection; //GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_REFLECTION_MAP ou GL_NORMAL_MAP
    GLenum reference_projection; //GL_OBJECT_PLANE ou GL_EYE_PLANE
    GLfloat axe_projection[4];

    //Paramètres pour les coordonnées UV:
    uint32_t nbrSommets;
    vector<dksSommetUV> coordonnees_UV;    //Coordonnées définies par l'utilisateur
    GLfloat* coordonnees_UV_tableau;    //Tableau initialisé par le constructeur du volume. !!! Non dupliqué par le constructeur de copie.

    Image* image;

    //Tables pour les textures de rugosité:
    //Ces tables sont créées par le volume.
    GLdouble* tangentes_faces;
    GLdouble* binormales_faces;
    GLdouble* tangentes_sommets;
    GLdouble* binormales_sommets;
    uint32_t* faces; //Les indexes des faces recouvertes par la texture.
    uint32_t nbrFaces;

    bool drapeau_mipmap;

    //Constructeur:
    dksTexture(char* p_nom=NULL,
               Image* p_image=NULL,
                uint8_t p_canal=DKSTEXTURE_CANAL_INVALIDE,
                float* p_uvs=NULL, uint32_t* p_indices_sommets=NULL,
                uint16_t p_nbrSommets=0,
                bool p_mipmap=false,
                GLint p_recouvrement_s=GL_CLAMP,
                GLint p_recouvrement_t=GL_CLAMP,
                GLint p_type_grossissement=GL_LINEAR,
                GLint p_type_diminution=GL_LINEAR
                )
    {

        erreur=DKSTEXTURE_OK;
        drapeau_mipmap=p_mipmap;
        canal=p_canal;
        nbrSommets=0;
        coordonnees_UV_tableau=NULL;
        image=p_image;
        tangentes_sommets=NULL;
        binormales_sommets=NULL;
        tangentes_faces=NULL;
        binormales_faces=NULL;
        faces=NULL;
        nbrFaces=0;
        luminescence=0.;

        //Gestion du nom:
        determine_nom(p_nom);
        //Par défaut, la texture est définie par des coordonnées UV:
        type_enveloppe=GL_MODULATE;
        type_projection=DKSTEXTURE_PROJECTION_UV;
        mode_projection=GL_OBJECT_LINEAR;
        reference_projection=GL_OBJECT_PLANE;
        axe_projection[0]=0.;
        axe_projection[1]=0.;
        axe_projection[2]=1.;
        axe_projection[3]=0.;

        recouvrement_s=p_recouvrement_s;
        recouvrement_t=p_recouvrement_t;
        type_grossissement=p_type_grossissement;
        type_diminution=p_type_diminution;

        if (image!=NULL)
        {
            initialise_image();
            image->genere_texture_openGL(drapeau_mipmap,recouvrement_s,recouvrement_t,type_grossissement,type_diminution);
        }
        if (p_nbrSommets>0 && p_indices_sommets!=NULL && p_uvs!=NULL) determine_UV(p_uvs,p_indices_sommets,p_nbrSommets);
    }

    //------------------Constructeur de copie:
    //Les données initialisées par le volume ne sont
    // pas dupliquées. Elles doivent être à nouveau initialisées par le
    // le volume utilisateur.

    dksTexture(dksTexture& texture_originale)
    {
        uint32_t i;
        erreur=texture_originale.erreur;
        canal=texture_originale.canal;
        nbrSommets=texture_originale.nbrSommets;
        coordonnees_UV_tableau=NULL;
        image=texture_originale.image;
        tangentes_sommets=NULL;
        binormales_sommets=NULL;
        tangentes_faces=NULL;
        binormales_faces=NULL;
        faces=NULL;
        nbrFaces=0;
        determine_nom(texture_originale.nom);      ///Le nom est dupliqué sans modification !!!

        type_enveloppe=texture_originale.type_enveloppe;
        type_projection=texture_originale.type_projection;
        mode_projection=texture_originale.mode_projection;
        reference_projection=texture_originale.reference_projection;
        axe_projection[0]=texture_originale.axe_projection[0];
        axe_projection[1]=texture_originale.axe_projection[1];
        axe_projection[2]=texture_originale.axe_projection[2];
        axe_projection[3]=texture_originale.axe_projection[3];
        recouvrement_s=texture_originale.recouvrement_s;
        recouvrement_t=texture_originale.recouvrement_t;
        type_diminution=texture_originale.type_diminution;
        type_grossissement=texture_originale.type_grossissement;

        for (i=0;i<nbrSommets;i++)
        {
            dksSommetUV* sommetUV=&texture_originale.coordonnees_UV[i];
            dksSommetUV* nouveau_sommet=ajoute_sommet_UV();
            nouveau_sommet->indice_sommet=sommetUV->indice_sommet;
            nouveau_sommet->U=sommetUV->U;
            nouveau_sommet->V=sommetUV->V;
        }

    }

    //==================================================
    //                  Destructeur
    ///       L'image n'est pas supprimée.
    //==================================================
    ~dksTexture()
    {
        if (coordonnees_UV_tableau!=NULL) free(coordonnees_UV_tableau);
        if(tangentes_faces!=NULL) free(tangentes_faces);
        if(binormales_faces!=NULL) free(binormales_faces);
        if(tangentes_sommets!=NULL) free(tangentes_sommets);
        if(binormales_sommets!=NULL) free(binormales_sommets);
        if (faces!=NULL) free(faces);
    }

    //============================================================================
    //                  Gestion de la texture
    //============================================================================
    void determine_nom(char* p_nom)
    {
        uint32_t i,taille;
        if (p_nom==NULL) nom[0]=0;
            else
            {
                taille=strlen(p_nom);
                if (taille>DKS_TAILLE_NOMS-1)taille=DKS_TAILLE_NOMS-1;
                for(i=0;i<taille;i++)
                {
                    nom[i]=p_nom[i];
                }
                nom[i]=0;
            }

    }

    void determine_image(Image* p_image)
    {
        image=p_image;
        if (image!=NULL)
        {
            initialise_image();
            image->genere_texture_openGL(drapeau_mipmap,recouvrement_s,recouvrement_t,type_grossissement,type_diminution);
        }
    }

    //Selon le canal de la texture, le mode OpenGl de l'image peut avoir
    //besoin d'une "mise aux normes"
    void initialise_image()
    {
        switch (canal)
        {
            case DKSTEXTURE_CANAL_COULEUR:
                if (image->bits_par_pixel==8) image->format_pixels=GL_LUMINANCE;
            break;
        }
    }

    //Détermine les coordonnées UV:
    void determine_UV(float* uvs, uint32_t* indices, uint16_t p_nbrSommets)
    {
        uint16_t i;
        coordonnees_UV.clear();
        for (i=0;i<p_nbrSommets;i++)
        {
            dksSommetUV* nouveau_UV=ajoute_sommet_UV();
            nouveau_UV->indice_sommet=indices[i];
            nouveau_UV->U=uvs[2*i];
            nouveau_UV->V=uvs[2*i+1];
        }
        nbrSommets=p_nbrSommets;
    }

    //Cette fonction sert uniquement au constructeur de copie
    private:
    dksSommetUV* ajoute_sommet_UV()
    {
        dksSommetUV nouveau_sommet;
        coordonnees_UV.push_back(nouveau_sommet);
        return &(coordonnees_UV[coordonnees_UV.size()-1]);
    }

};

#endif // DKSTEXTURE_H_INCLUDED
