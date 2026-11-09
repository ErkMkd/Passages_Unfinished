#ifndef DKSSPRITE3D_H_INCLUDED
#define DKSSPRITE3D_H_INCLUDED

/*
        Gestion de l'affichage d'un sprite situé dans l'espace.
        Le sprite est toujours face à l'oeil. Sa taille est calculée en fonction de la distance à l'oeil.
        C'est une classe de base, les coordonnées du sprite ne sont pas mémorisées ici.
*/

#include "../../chargeurs_images/Image.h"

#define DKSSPRITE3D_OK 0
#define DKSSPRITE3D_ERREUR_IMAGE 1

class dksSprite3d
{
    static bool openGl_mem[6];

    public:
    uint16_t erreur;
    char nom[64];
    Image* image;
    float facteur_taille;   //Taille d'un pixel en unités d'espace

    protected:
    bool drapeau_affecte_par_brouillard;    //True par défaut: le sprite est affecté par le brouillard.


    //====================================================
    //                  CONSTRUCTEUR
    //====================================================
    public:
    dksSprite3d(char* p_nom, char* p_fichier_image,float p_facteur_taille=1.)
    {
        erreur=DKSSPRITE3D_OK;
        image=NULL;
        facteur_taille=p_facteur_taille;
        uint32_t i,t;
        drapeau_affecte_par_brouillard=true;

        //Initialise le nom:
        t=strlen(p_nom);
        if (t>62)t=62;
        for(i=0;i<strlen(p_nom);i++)
        {
            nom[i]=p_nom[i];
        }
        nom[i]=0;

        //Charge l'image:
        image=new Image(p_fichier_image);
        if (image==NULL || image->erreur!=IMAGE_OK)
        {
            erreur=DKSSPRITE3D_ERREUR_IMAGE;
            cout<<"ERREUR - Impossible de charger l'image du sprite3d :"<<nom<<endl;
        }
        if (!image->genere_buffer_openGl()) {cout<<"ERREUR dans dksSprite3d::dksSprite3d()"<<endl; erreur=DKSSPRITE3D_ERREUR_IMAGE; return;}
        if (!image->genere_texture_openGL()) {cout<<"ERREUR dans dksSprite3d::dksSprite3d()"<<endl;erreur=DKSSPRITE3D_ERREUR_IMAGE; return;}
    }

    //==================================
    //      Destructeur
    //==================================
    ~dksSprite3d()
    {
        if (image!=NULL) delete image;
    }

    //-----------------------------------------------------------------------------------
    //      Détermine si le sprite est affecté par le brouillard
    //-----------------------------------------------------------------------------------
    void determine_affecte_par_brouillard(bool p_drapeau)
    {
        drapeau_affecte_par_brouillard=p_drapeau;
    }

    bool est_affecte_par_brouillard()
    {
        return drapeau_affecte_par_brouillard;
    }

    //============================================================
    //      Affichage
    //============================================================

    //----------------------------------------------------------------
    //      Affiche l'image en utilisant les déplacements de pixels.
    //      Sert pour les affichages à plat.
    //----------------------------------------------------------------

    void affiche_pixels(double px, double py, double pz, dksOeil* oeil)
    {
        bool fog_mem=dksEtat::est_active(GL_FOG);
        if (!drapeau_affecte_par_brouillard) dksEtat::desactive(GL_FOG);
        //Calcul la taille du sprite par rapport à l'oeil:
        float dx=px-oeil->repere_absolu->x;
        float dy=py-oeil->repere_absolu->y;
        float dz=pz-oeil->repere_absolu->z;

        float dist=oeil->direction_regard->x*dx+oeil->direction_regard->y*dy+oeil->direction_regard->z*dz;

        if (dist<=0) return;
        GLint resolution[4];
        glGetIntegerv(GL_VIEWPORT,resolution);
        float facteur_resolution=resolution[3]/(2*tanf(oeil->renvoie_ouverture()/360.*M_PI));
        float zoom=facteur_taille*facteur_resolution/dist;
        //float taille=facteur_taille/dist;
        glPixelZoom(zoom,zoom);

        //centrage:
        float cx=facteur_taille*float(image->largeur)/2.;
        float cy=facteur_taille*float(image->hauteur)/2.;
        float Rx= px + oeil->repere_absolu->axe_x->x * cx - oeil->repere_absolu->axe_y->x * cy ;
        float Ry= py + oeil->repere_absolu->axe_x->y * cx - oeil->repere_absolu->axe_y->y * cy ;
        float Rz= pz + oeil->repere_absolu->axe_x->z * cx - oeil->repere_absolu->axe_y->z * cy ;

        //Position du raster:
        glRasterPos3f(Rx,Ry,Rz);

        //affichage:
        dksEtat::desactive(GL_BLEND);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,image->buffer_id);
        glDrawPixels(image->largeur,image->hauteur,image->format_pixels,image->type_pixels,(GLubyte*)NULL+(0));
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);

        if(fog_mem) dksEtat::active(GL_FOG); else dksEtat::desactive(GL_FOG);
    }

    //-------------------------------------------------------------------
    //  Affiche l'image en utilisant une texture.
    //  Sert pour les affchages dans l'espace, si il y a peu de sprites à afficher.
    //-------------------------------------------------------------------

    void affiche_texture(double px,double py,double pz, dksOeil* oeil,float alpha=1.)
    {
        bool lighting_mem=dksEtat::est_active(GL_LIGHTING);
        bool blend_mem=dksEtat::est_active(GL_BLEND);
        bool texture_mem=dksEtat::est_active(GL_TEXTURE_2D);
        bool depth_test_mem=dksEtat::est_active(GL_DEPTH_TEST);
        bool cull_mem=dksEtat::est_active(GL_CULL_FACE);
        bool fog_mem=dksEtat::est_active(GL_FOG);

        if (!drapeau_affecte_par_brouillard) dksEtat::desactive(GL_FOG);

        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::desactive(GL_LIGHTING);
        dksEtat::active(GL_BLEND);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::active(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")
        dksEtat::active(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        dksEtat::lie_texture(GL_TEXTURE_2D,image->texture_id);

        float cx=float(image->largeur)*facteur_taille/2.;
        float cy=float(image->hauteur)*facteur_taille/2.;

        //Calcul les coordonnées du rectangle:
        float x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3;
        dksVecteurUnitaire* repX=oeil->repere_absolu->axe_x;
        dksVecteurUnitaire* repY=oeil->repere_absolu->axe_y;

        x0=px+repX->x*cx-repY->x*cy;
        y0=py+repX->y*cx-repY->y*cy;
        z0=pz+repX->z*cx-repY->z*cy;

        x1=px-repX->x*cx-repY->x*cy;
        y1=py-repX->y*cx-repY->y*cy;
        z1=pz-repX->z*cx-repY->z*cy;

        x2=px-repX->x*cx+repY->x*cy;
        y2=py-repX->y*cx+repY->y*cy;
        z2=pz-repX->z*cx+repY->z*cy;

        x3=px+repX->x*cx+repY->x*cy;
        y3=py+repX->y*cx+repY->y*cy;
        z3=pz+repX->z*cx+repY->z*cy;


        dksImmediat::debut(GL_QUADS);
            dksImmediat::couleur4f(1.0,1.0,1.0,alpha);
            dksImmediat::texture2f(0,0); dksImmediat::sommet3f(x0,y0,z0);
            dksImmediat::texture2f(1,0); dksImmediat::sommet3f(x1,y1,z1);
            dksImmediat::texture2f(1,1); dksImmediat::sommet3f(x2,y2,z2);
            dksImmediat::texture2f(0,1); dksImmediat::sommet3f(x3,y3,z3);
        dksImmediat::fin();

        if (!blend_mem) dksEtat::desactive(GL_BLEND);
        if (lighting_mem) dksEtat::active(GL_LIGHTING);
        if (depth_test_mem) dksEtat::active(GL_DEPTH_TEST);
        if (!texture_mem) dksEtat::desactive(GL_TEXTURE_2D);
        if (!cull_mem) dksEtat::desactive(GL_CULL_FACE);
        if(fog_mem) dksEtat::active(GL_FOG); else dksEtat::desactive(GL_FOG);

    }

    //-----------------------------------------------------------------
    //Si il y a beaucoup de sprites à afficher,
    //Se servir de ces fonction pour éviter les inits à répétition
    //------------------------------------------------------------------
    static void initialise_affichage()
    {
        openGl_mem[0]=dksEtat::est_active(GL_LIGHTING);
        openGl_mem[1]=dksEtat::est_active(GL_BLEND);
        openGl_mem[2]=dksEtat::est_active(GL_TEXTURE_2D);
        openGl_mem[3]=dksEtat::est_active(GL_DEPTH_TEST);
        openGl_mem[4]=dksEtat::est_active(GL_CULL_FACE);
        openGl_mem[5]=dksEtat::est_active(GL_FOG);

        /// Voir si il y a besoin de désactiver toutes les unités de texture.

        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::desactive(GL_LIGHTING);
        dksEtat::active(GL_BLEND);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::active(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")
        dksEtat::active(GL_CULL_FACE);
        glFrontFace(GL_CCW);
    }

    void affiche_texture_optimisee(double* position, dksOeil* oeil,float alpha=1.)
    {
        if (!drapeau_affecte_par_brouillard) dksEtat::desactive(GL_FOG);
        else
        {
            if (openGl_mem[5]) dksEtat::active(GL_FOG);
            else dksEtat::desactive(GL_FOG);
        }

        dksEtat::lie_texture(GL_TEXTURE_2D,image->texture_id);

        float cx=float(image->largeur)*facteur_taille/2.;
        float cy=float(image->hauteur)*facteur_taille/2.;

        //Calcul les coordonnées du rectangle:
        float x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3;
        dksVecteurUnitaire* repX=oeil->repere_absolu->axe_x;
        dksVecteurUnitaire* repY=oeil->repere_absolu->axe_y;

        x0=position[0]+repX->x*cx-repY->x*cy;
        y0=position[1]+repX->y*cx-repY->y*cy;
        z0=position[2]+repX->z*cx-repY->z*cy;

        x1=position[0]-repX->x*cx-repY->x*cy;
        y1=position[1]-repX->y*cx-repY->y*cy;
        z1=position[2]-repX->z*cx-repY->z*cy;

        x2=position[0]-repX->x*cx+repY->x*cy;
        y2=position[1]-repX->y*cx+repY->y*cy;
        z2=position[2]-repX->z*cx+repY->z*cy;

        x3=position[0]+repX->x*cx+repY->x*cy;
        y3=position[1]+repX->y*cx+repY->y*cy;
        z3=position[2]+repX->z*cx+repY->z*cy;

        dksImmediat::debut(GL_QUADS);
            dksImmediat::couleur4f(1.0,1.0,1.0,alpha);
            dksImmediat::texture2f(0,0); dksImmediat::sommet3f(x0,y0,z0);
            dksImmediat::texture2f(1,0); dksImmediat::sommet3f(x1,y1,z1);
            dksImmediat::texture2f(1,1); dksImmediat::sommet3f(x2,y2,z2);
            dksImmediat::texture2f(0,1); dksImmediat::sommet3f(x3,y3,z3);
        dksImmediat::fin();

    }

    static void fin_affichage()
    {
        if (openGl_mem[0]) dksEtat::active(GL_LIGHTING);
        if (!openGl_mem[1]) dksEtat::desactive(GL_BLEND);
        if (!openGl_mem[2]) dksEtat::desactive(GL_TEXTURE_2D);
        if (openGl_mem[3]) dksEtat::active(GL_DEPTH_TEST);
        if (!openGl_mem[4]) dksEtat::desactive(GL_CULL_FACE);
        if(openGl_mem[5]) dksEtat::active(GL_FOG); else dksEtat::desactive(GL_FOG);
    }

};

#endif // DKSSPRITE3D_H_INCLUDED
