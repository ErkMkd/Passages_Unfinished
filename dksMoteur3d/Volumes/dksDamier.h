#ifndef DKSDAMIER_H_INCLUDED
#define DKSDAMIER_H_INCLUDED

/*
            Génère un damier composé d'un nombre paramétrable de mailles.
            Plan défini sur O,X,Z
*/


#include "../Elements_Fondamentaux/dksVolume.h"

#define DKSDAMIER_DEBUG true

//------------- Erreurs:

#define DKSDAMIER_OK 0
#define DKSDAMIER_ERREUR_MEMOIRE 1



//===========================================================================================================
//             Classe
//===========================================================================================================

class dksDamier : public dksVolume
{
    public:
    uint32_t nbr_mailles_x;
    uint32_t nbr_mailles_z;

    float taille_x;  //En unités d'espace
    float taille_z;

    double* sommets_plan;
    dksFace** faces_plan;
    uint32_t* aretes_plan;
    dksSurface* surface_plan_recto_1;
    dksSurface* surface_plan_recto_2;
    dksSurface* surface_plan_verso_1;
    dksSurface* surface_plan_verso_2;
    dksSurface* surfaces_plan[4];
    bool drapeau_recto_verso;   //True si le plan est à double face (2 fois plus de polygones, 2 surfaces au lieu d'une.
    bool drapeau_triangles; //True(défaut) si les mailles sont divisées en deux triangles, false si les mailles sont des polygones quadrangulaires

    uint32_t couleur_recto_1;
    uint32_t couleur_recto_2;
    uint32_t couleur_verso_1;
    uint32_t couleur_verso_2;

    //Données pour les ondulations le long de l'axe Z:
    double amplitude_ondes;
    double longueur_ondes; //Taille d'une période en unités d'espace
    double vitesse_ondes; //Vitesse de déplacement des ondes en degres/s

    //===========================================================================================================
    //             CONSTRUCTEUR
    //===========================================================================================================

    dksDamier(char* p_nom="Plan",
            double px=0., double py=0., double pz=0.,
            float p_taille_x=1., float p_taille_z=1.,
            uint32_t p_nbr_mailles_x=10, uint32_t p_nbr_mailles_z=10,
            bool p_drapeau_triangles=true,
            bool p_drapeau_recto_verso=true,
            uint32_t p_couleur_recto_1=0xffffff,uint32_t p_couleur_recto_2=0xffd0d0ff,
            dksVecteurUnitaire* axe1=NULL, dksVecteurUnitaire* axe2=NULL, dksVecteurUnitaire* axe3=NULL
            ):dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
    {

        sens_polygones=GL_CW;

        surface_plan_recto_1=NULL;
        surface_plan_recto_2=NULL;
        surface_plan_verso_1=NULL;
        surface_plan_verso_2=NULL;

        sommets_plan=NULL;
        faces_plan=NULL;
        aretes_plan=NULL;

        taille_x=p_taille_x;
        taille_z=p_taille_z;

        nbr_mailles_x=p_nbr_mailles_x;
        nbr_mailles_z=p_nbr_mailles_z;

        drapeau_recto_verso=p_drapeau_recto_verso;
        drapeau_triangles=p_drapeau_triangles;

        couleur_recto_1=p_couleur_recto_1;
        couleur_recto_2=p_couleur_recto_2;

        if (!creation_sommets()) return;
        if (!creation_faces()) return;
        if (!creation_surfaces()) return;

        mode_remplissage=DKSVOLUME_REMPLISSAGE_SURFACES;

        longueur_ondes=25.;
        vitesse_ondes=120.;
        amplitude_ondes=2.;
    }
    //===================================================
    //              DESTRUCTEUR
    //===================================================
    ~dksDamier()
    {
        if (aretes_plan!=NULL) delete aretes_plan;  ///Attention, les faces ne doivent pas libérer leurs arètes.

        ///Libération par le destructeur du volume:
        //if (faces_plan!=NULL) delete faces_plan;
        if (surface_plan_recto_1!=NULL) delete surface_plan_recto_1;
        if (surface_plan_recto_2!=NULL) delete surface_plan_recto_2;
        //if (surface_plan_verso!=NULL) delete surface_plan_verso;
        //if (sommets_plan!=NULL) delete sommets_plan;
    }

    //====================================================================================================
    //  Création du maillage
    //====================================================================================================
    private:
    bool creation_sommets()
    {
        uint32_t i,mx,mz;
        double px,pz;
        //------- Allocation:
        uint32_t p_nbrSommets=(nbr_mailles_x+1)*(nbr_mailles_z+1);
        sommets_plan=(double*)malloc(p_nbrSommets*sizeof(double)*3);
        if(sommets_plan==NULL)
        {
            cout<<"ERREUR dans dksDamier::creation_sommets() - Impossible d'allouer les sommets"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }
        //--------- Coordonnées:
        px=taille_x/double(nbr_mailles_x);
        pz=taille_z/double(nbr_mailles_z);
        for (mz=0;mz<(nbr_mailles_z+1);mz++)
        {
            for(mx=0;mx<(nbr_mailles_x+1);mx++)
            {
                i=mx+mz*(nbr_mailles_x+1);
                sommets_plan[i*3]=double(mx)*px-taille_x/2.;
                sommets_plan[i*3+1]=0.;
                sommets_plan[i*3+2]=double(mz)*pz-taille_z/2.;
            }
        }

        //----------- Initialise les sommets du volume:
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SOMMETS;
        return determine_sommets(sommets_plan,p_nbrSommets);
    }

    //----------------------------
    //  Création des faces
    //----------------------------
    private:
    bool creation_faces()
    {
        if(drapeau_triangles) return creation_faces_triangles();
        else return creation_faces_quadrangles();
    }

    //---------- 4 côtés:
    bool creation_faces_quadrangles()
    {
        uint32_t i,j,s;
        //------- Table des faces:
        uint32_t p_nbrFaces=nbr_mailles_x*nbr_mailles_z;
        faces_plan=(dksFace**)malloc(p_nbrFaces*sizeof(dksFace**));
        if(faces_plan==NULL)
        {
            cout<<"ERREUR dans dksDamier::creation_faces() - Impossible d'allouer la table des faces"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }
        //------- Table des arètes:
        aretes_plan=(uint32_t*)malloc(p_nbrFaces*4*sizeof(uint32_t));
        if(aretes_plan==NULL)
        {
            cout<<"ERREUR dans dksDamier::creation_faces() - Impossible d'allouer la table des arètes"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }

        //------- Initialise les arètes:
        for(i=0;i<p_nbrFaces;i++)
        {
           s=(i%nbr_mailles_x)+i/nbr_mailles_x*(nbr_mailles_x+1);
           aretes_plan[4*i]=s;
           aretes_plan[4*i+1]=s+1;
           aretes_plan[4*i+2]=s+(nbr_mailles_x+1)+1;
           aretes_plan[4*i+3]=s+(nbr_mailles_x+1);
        }
        //------ Création des faces:
        char surface_1[]={"surface_recto_1"};
        char surface_2[]={"surface_recto_2"};
        char* surface_nom;
        for (i=0;i<p_nbrFaces;i++)
        {
            if(nbr_mailles_x&1==0)
            {
                if( ((i/nbr_mailles_x)&1)==1 )
                {
                    if((i&1)==1) surface_nom=surface_1;
                    else surface_nom=surface_2;
                }
                else
                {
                    if((i&1)==1) surface_nom=surface_2;
                    else surface_nom=surface_1;
                }
            }

            else
            {
                if((i&1)==1) surface_nom=surface_2;
                else surface_nom=surface_1;
            }


            faces_plan[i]=new dksFace(4,&aretes_plan[4*i],surface_nom,DKSFACE_LIBERE_RIEN);
        }
        //------- Initialise les faces:
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;
        return determine_faces(faces_plan,p_nbrFaces);
    }

    //----- 3 côtés:
    bool creation_faces_triangles()
    {
        uint32_t i,j,s;
        //------- Table des faces:
        uint32_t p_nbrFaces=nbr_mailles_x*nbr_mailles_z*2;
        faces_plan=(dksFace**)malloc(p_nbrFaces*sizeof(dksFace**));
        if(faces_plan==NULL)
        {
            cout<<"ERREUR dans dksDamier::creation_faces() - Impossible d'allouer la table des faces"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }
        //------- Table des arètes:
        aretes_plan=(uint32_t*)malloc(p_nbrFaces*3*sizeof(uint32_t));
        if(aretes_plan==NULL)
        {
            cout<<"ERREUR dans dksDamier::creation_faces() - Impossible d'allouer la table des arètes"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }

        //------- Initialise les arètes:
        for(i=0;i<p_nbrFaces/2;i++)
        {
           s=(i%nbr_mailles_x)+i/nbr_mailles_x*(nbr_mailles_x+1);
           aretes_plan[6*i]=s;
           aretes_plan[6*i+1]=s+1;
           aretes_plan[6*i+2]=s+(nbr_mailles_x+1);

           aretes_plan[6*i+3]=s+1;
           aretes_plan[6*i+4]=s+(nbr_mailles_x+1)+1;
           aretes_plan[6*i+5]=s+(nbr_mailles_x+1);
        }
        //------ Création des faces:
        char surface_1[]={"surface_recto_1"};
        char surface_2[]={"surface_recto_2"};
        char* surface_nom;

        for (i=0;i<p_nbrFaces;i++)
        {
            if(nbr_mailles_x&1==0)
            {
                if( ((i/nbr_mailles_x/2)&1)==1 )
                {
                    if((i&2)==2) surface_nom=surface_1;
                    else surface_nom=surface_2;
                }
                else
                {
                    if((i&2)==2) surface_nom=surface_2;
                    else surface_nom=surface_1;
                }
            }

            else
            {
                if((i&2)==2) surface_nom=surface_2;
                else surface_nom=surface_1;
            }

            faces_plan[i]=new dksFace(3,&aretes_plan[3*i],surface_nom,DKSFACE_LIBERE_RIEN);
        }
        //------- Initialise les faces:
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;
        return determine_faces(faces_plan,p_nbrFaces);
    }
    //----------------------------
    //  Création des surfaces
    //----------------------------
    private:
    bool creation_surfaces()
    {
        surface_plan_recto_1=new dksSurface("surface_recto_1");
        surface_plan_recto_1->determine_couleur_diffusion(couleur_recto_1);
        surfaces_plan[0]=surface_plan_recto_1;

        surface_plan_recto_2=new dksSurface("surface_recto_2");
        surface_plan_recto_2->determine_couleur_diffusion(couleur_recto_2);
        surfaces_plan[1]=surface_plan_recto_2;

        return determine_surfaces(surfaces_plan,2);
    }
    //=================================================================================================
    //              Gestion des textures
    //=================================================================================================
    public:
    bool determine_texture_recto(Image* image,uint8_t p_canal=DKSTEXTURE_CANAL_COULEUR)
    {
        //-------- Allocation UV:
        float* uvs_tab=(float*)malloc(nbrSommets*2*sizeof(float));
        if (uvs_tab==NULL)
        {
            cout<<"ERREUR dans dksDamier::determine_texture_recto() - Impossible d'allouer la table des coordonnées UV"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }

        //---------- Allocation indices sommets:
        uint32_t* indices_uvs=(uint32_t*)malloc(nbrSommets*sizeof(uint32_t));
        if (indices_uvs==NULL)
        {
            cout<<"ERREUR dans dksDamier::determine_texture_recto() - Impossible d'allouer la table des indices UV"<<endl;
            erreur=DKSDAMIER_ERREUR_MEMOIRE;
            return false;
        }

        //---------- Initialisation des coordonnées UV:
        uint32_t i,px,pz;
        i=0;
        for(px=0;px<nbr_mailles_x+1;px++)
        {
            for(pz=0;pz<nbr_mailles_z+1;pz++)
            {
                uvs_tab[2*i]=float(px)/float(nbr_mailles_x+1);
                uvs_tab[2*i+1]=float(pz)/float(nbr_mailles_z+1);
                indices_uvs[i]=i;
                i++;
            }
        }
        //---------- Création de la texture:
        /// La texture est automatiquement détruite par la surface
        dksTexture* texture_recto_1=new dksTexture(NULL,image,p_canal,uvs_tab,indices_uvs,nbrSommets,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
        dksTexture* texture_recto_2=new dksTexture(NULL,image,p_canal,uvs_tab,indices_uvs,nbrSommets,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
        free(uvs_tab);
        free(indices_uvs);
        surface_plan_recto_1->ajoute_texture(texture_recto_1);
        surface_plan_recto_2->ajoute_texture(texture_recto_2);
        initialise_textures();
    }
    //=============================================================================================
    //                  Gestion des hauteurs des mailles
    //=============================================================================================

    //--------------------------------------------------------------------------
    //      Détermine le relief à partir d'une texture en niveaux de gris
    ///     L'image doit être en niveaux de gris sur 8 bits!
    //--------------------------------------------------------------------------
    void determine_relief(Image* bitmap_relief,double facteur,double ymin=0.)
    {
        if (bitmap_relief->bits_par_pixel!=8) return;

        uint32_t i,px,pz;
        double texel;
        i=0;
        for (pz=0;pz<(nbr_mailles_z+1);pz++)
        {
            for (px=0;px<(nbr_mailles_x+1);px++)
            {
                uint32_t indice=px*(bitmap_relief->largeur-1)/nbr_mailles_x+(pz*(bitmap_relief->hauteur-1)/nbr_mailles_z)*bitmap_relief->largeur;
                //cout<<indice<<endl;
                texel=double(bitmap_relief->image[bitmap_relief->largeur*bitmap_relief->hauteur-1-indice])/255.;
                sommets[i*3+1]=ymin+texel*facteur;
                //cout<<sommets_plan<<" - "<<sommets<<endl;
                i++;
            }
        }
    }

    //=============================================================================================
    //                  Gestion des animations
    //=============================================================================================

    //----------------------------------------------------------------------
    //      Mise à jour des ondulations le long de l'axe Z.
    //  Fonction à appeler après applique_mouvement(),
    //  pour avoir les bons repères temporels (position_temps et delta_t)
    //----------------------------------------------------------------------
    void maj_ondulations_z()
    {
        uint32_t i;
        double vo=vitesse_ondes/180.*M_PI*position_temps;
        double decal=M_PI*2./longueur_ondes;
        for(i=0;i<nbrSommets;i++)
        {
            sommets[3*i+1]=sin(vo+decal*sommets[3*i+2])*amplitude_ondes;
        }
        //Rafraichi les normales des faces:
        if(sens_polygones==GL_CW) calcul_normales_faces_CW();
        else calcul_normales_faces_CCW();
        calcul_normales_sommets();
    }

};

#endif // DKSDAMIER_H_INCLUDED
