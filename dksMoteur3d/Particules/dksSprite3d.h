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
        bool fog_mem=glIsEnabled(GL_FOG);
        if (!drapeau_affecte_par_brouillard) glDisable(GL_FOG);
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
        glDisable(GL_BLEND);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,image->buffer_id);
        glDrawPixels(image->largeur,image->hauteur,image->format_pixels,image->type_pixels,(GLubyte*)NULL+(0));
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);

        if(fog_mem) glEnable(GL_FOG); else glDisable(GL_FOG);
    }

    //-------------------------------------------------------------------
    //  Affiche l'image en utilisant une texture.
    //  Sert pour les affchages dans l'espace, si il y a peu de sprites à afficher.
    //-------------------------------------------------------------------

    void affiche_texture(double px,double py,double pz, dksOeil* oeil,float alpha=1.)
    {
        bool lighting_mem=glIsEnabled(GL_LIGHTING);
        bool blend_mem=glIsEnabled(GL_BLEND);
        bool texture_mem=glIsEnabled(GL_TEXTURE_2D);
        bool depth_test_mem=glIsEnabled(GL_DEPTH_TEST);
        bool cull_mem=glIsEnabled(GL_CULL_FACE);
        bool fog_mem=glIsEnabled(GL_FOG);

        if (!drapeau_affecte_par_brouillard) glDisable(GL_FOG);

        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        glBindTexture(GL_TEXTURE_2D,image->texture_id);

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


        glBegin(GL_QUADS);
            glColor4f(1.0,1.0,1.0,alpha);
            glTexCoord2f(0,0); glVertex3f(x0,y0,z0);
            glTexCoord2f(1,0); glVertex3f(x1,y1,z1);
            glTexCoord2f(1,1); glVertex3f(x2,y2,z2);
            glTexCoord2f(0,1); glVertex3f(x3,y3,z3);
        glEnd();

        if (!blend_mem) glDisable(GL_BLEND);
        if (lighting_mem) glEnable(GL_LIGHTING);
        if (depth_test_mem) glEnable(GL_DEPTH_TEST);
        if (!texture_mem) glDisable(GL_TEXTURE_2D);
        if (!cull_mem) glDisable(GL_CULL_FACE);
        if(fog_mem) glEnable(GL_FOG); else glDisable(GL_FOG);

    }

    //-----------------------------------------------------------------
    //Si il y a beaucoup de sprites à afficher,
    //Se servir de ces fonction pour éviter les inits à répétition
    //------------------------------------------------------------------
    static void initialise_affichage()
    {
        openGl_mem[0]=glIsEnabled(GL_LIGHTING);
        openGl_mem[1]=glIsEnabled(GL_BLEND);
        openGl_mem[2]=glIsEnabled(GL_TEXTURE_2D);
        openGl_mem[3]=glIsEnabled(GL_DEPTH_TEST);
        openGl_mem[4]=glIsEnabled(GL_CULL_FACE);
        openGl_mem[5]=glIsEnabled(GL_FOG);

        /// Voir si il y a besoin de désactiver toutes les unités de texture.

        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
    }

    void affiche_texture_optimisee(double* position, dksOeil* oeil,float alpha=1.)
    {
        if (!drapeau_affecte_par_brouillard) glDisable(GL_FOG);
        else
        {
            if (openGl_mem[5]) glEnable(GL_FOG);
            else glDisable(GL_FOG);
        }

        glBindTexture(GL_TEXTURE_2D,image->texture_id);

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

        glBegin(GL_QUADS);
            glColor4f(1.0,1.0,1.0,alpha);
            glTexCoord2f(0,0); glVertex3f(x0,y0,z0);
            glTexCoord2f(1,0); glVertex3f(x1,y1,z1);
            glTexCoord2f(1,1); glVertex3f(x2,y2,z2);
            glTexCoord2f(0,1); glVertex3f(x3,y3,z3);
        glEnd();

    }

    static void fin_affichage()
    {
        if (openGl_mem[0]) glEnable(GL_LIGHTING);
        if (!openGl_mem[1]) glDisable(GL_BLEND);
        if (!openGl_mem[2]) glDisable(GL_TEXTURE_2D);
        if (openGl_mem[3]) glEnable(GL_DEPTH_TEST);
        if (!openGl_mem[4]) glDisable(GL_CULL_FACE);
        if(openGl_mem[5]) glEnable(GL_FOG); else glDisable(GL_FOG);
    }

};

#endif // DKSSPRITE3D_H_INCLUDED
