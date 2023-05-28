#ifndef SPRITES_3D_H_INCLUDED
#define SPRITES_3D_H_INCLUDED

/*
        Les sprites 3d peuvent être transformés par rotation ou homothétie. Leur transparence est également paramétrable.
*/

#include <vector>
#include "../chargeurs_images/Image_TIFF.h"

#define SPR3D_OK 0
#define SPR3D_ERREUR_MEMOIRE 1
#define SPR3D_ERREUR_TIFF 2

typedef struct
{
    Image_TIFF* image;
    float x;    //Position de l'image à l'écran
    float y;
    float cx;   //Centre du sprite
    float cy;
    float zoom;

} sprite_3d;

class Sprites_3d
{
    private:
        GLuint* textures_openGl_id;
        uint32_t nbrImages;
        vector<sprite_3d> sprites;
        float echelle;
        float resolution_x;
        float resolution_y;

    public:
        uint32_t erreur;

    //============== Constructeur

    Sprites_3d (uint32_t p_nbrImages,char** noms_fichiers)
    {
        erreur=SPR3D_OK;
        nbrImages=p_nbrImages;
        resolution_x=1024.;
        resolution_y=768.;
        echelle=1.;
        if (nbrImages==0) textures_openGl_id=NULL;

        // Allocation de la table des identifiants OpenGl:
        textures_openGl_id=(GLuint*)calloc(sizeof( GLuint ),nbrImages);
        if (textures_openGl_id==NULL )
        {
            nbrImages=0;
            erreur=SPR3D_ERREUR_MEMOIRE;
            cout<<"ERREUR dans Sprites_3d.Sprites_3d() - Impossible d'allouer la table des identifiants OpenGl !"<<endl;
            return;
        }
        // Charge les images:
        for (uint32_t i=0;i<nbrImages;i++)
        {
            sprite_3d nouveau_sprite;   //La création dans la fonction ne plante pas...Peut-être parcequ'on est dans une boucle...
            nouveau_sprite.x=0.;
            nouveau_sprite.y=0.;
            nouveau_sprite.cx=0.;
            nouveau_sprite.cy=0.;
            nouveau_sprite.zoom=1.;
            nouveau_sprite.image=new Image_TIFF(noms_fichiers[i],ORIENTATION_TOPLEFT);
            if (nouveau_sprite.image==0 || nouveau_sprite.image->erreur!=IMAGE_TIFF_OK)
            {
                erreur=SPR3D_ERREUR_TIFF;
                return;
            }
            sprites.push_back(nouveau_sprite);
        }

        // Génère les identifiants:
        glGenTextures(nbrImages,textures_openGl_id); //Allocation des identifiants de textures

        genere_textures_openGL();
    }

    //================= Destructeur:
    ~Sprites_3d()
    {
        cout<<"DESTRUCTION SPRITES3D"<<endl;
        if ( nbrImages>0 && textures_openGl_id!=NULL )
            {
                glDeleteTextures(nbrImages,textures_openGl_id);
                free(textures_openGl_id);
                textures_openGl_id=NULL;
            }

        uint32_t taille=sprites.size();
        for (uint32_t i=0;i<taille;i++)
        {
            delete(sprites[i].image);
        }
        sprites.clear();
    }
    //=============== Génère les textures openGl à partir des images TIFF:
private:
    void genere_textures_openGL()
    {
        for (uint32_t i=0;i<nbrImages;i++)
        {
            Image_TIFF* image=sprites[i].image;
            image->texture_id=textures_openGl_id[i]; //Initialise les identifiants textures pour les images.

            glBindTexture(GL_TEXTURE_2D,textures_openGl_id[i]); //Sélection de la texture courante

            glTexParameterf(GL_TEXTURE_2D,      //Cible: La texture à paramétrer
                            GL_TEXTURE_WRAP_S,  //nomparam: Le paramètre à initialiser (ici:mode débordement horizontal de la texture 2d)
                            GL_REPEAT);          //valeur: la valeur du paramètre.
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); //Mode de débordement vertical de la texture 2d

            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //Mode d'étirement de la texture
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //Mode de rétrécissement de la texture

            glTexImage2D(GL_TEXTURE_2D, //Type de  texture
                        0,      //level: 0 pour une texture à 1 niveau (???)
                        4,      //component: 1:R, 2:RA, 3:RVB, 4:RVBA
                        image->largeur,image->hauteur, //largeur et hauteur de la texture en pixels
                        0,      //Largeur du bord. l et h sont incrémentées de 2x cette valeur
                        image->format_pixels, //Format des pixels
                        GL_UNSIGNED_BYTE, //Type des données
                        image->image);
        }
    }
public:

    //====================================================
    //  Positionnement des sprites
    //====================================================
    void determine_position_sprite(uint32_t indice, float x, float y)
    {
        sprites[indice].x=x;
        sprites[indice].y=y;
    }

    void determine_centre_sprite(uint32_t indice, float x, float y)
    {
        sprites[indice].cx=x;
        sprites[indice].cy=y;
    }

    void centre_sprite(uint32_t indice)
    {
        sprites[indice].cx= float(sprites[indice].image->largeur)/2.;
        sprites[indice].cy= float(sprites[indice].image->hauteur)/2.;
    }

    uint32_t renvoie_largeur_sprite(uint32_t indice)
    {
        return sprites[indice].image->largeur;
    }

    uint32_t renvoie_hauteur_sprite(uint32_t indice)
    {
        return sprites[indice].image->hauteur;
    }
    //====================================================
    //  Affichage des sprites via une texture OpenGl
    //====================================================

    void affiche_sprite_3d(uint32_t indice,float alpha=1.0)
    {
        glPushMatrix();

        bool lighting_mem=glIsEnabled(GL_LIGHTING);
        bool blend_mem=glIsEnabled(GL_BLEND);
        bool texture_mem=glIsEnabled(GL_TEXTURE_2D);
        bool depth_test_mem=glIsEnabled(GL_DEPTH_TEST);
        bool cull_mem=glIsEnabled(GL_CULL_FACE);
        bool colorMat_mem=glIsEnabled(GL_COLOR_MATERIAL);

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")
        glDisable(GL_CULL_FACE);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);

        glLoadIdentity();
        float xp=sprites[indice].x-sprites[indice].cx*sprites[indice].zoom;
        float yp=sprites[indice].y-sprites[indice].cy*sprites[indice].zoom;
        glTranslatef(xp-resolution_x/2,-yp+resolution_y/2,-echelle);

        Image_TIFF* image=sprites[indice].image;
        glBindTexture(GL_TEXTURE_2D,image->texture_id);

        float largeur=float(image->largeur)*sprites[indice].zoom;
        float hauteur=float(image->hauteur)*sprites[indice].zoom;

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(1.0,1.0,1.0,alpha);
            glTexCoord2f(0,0); glVertex2f(0,0);
            glTexCoord2f(0,1); glVertex2f(0,-hauteur);
            glTexCoord2f(1,1); glVertex2f(largeur,-hauteur);
            glTexCoord2f(1,0); glVertex2f(largeur,0);
        glEnd();

        if (!blend_mem) glDisable(GL_BLEND);
        if (lighting_mem) glEnable(GL_LIGHTING);
        if (depth_test_mem) glEnable(GL_DEPTH_TEST);
        if (!texture_mem) glDisable(GL_TEXTURE_2D);
        if (cull_mem) glEnable(GL_CULL_FACE);
        if (!colorMat_mem) glDisable(GL_COLOR_MATERIAL);

        glPopMatrix();
    }

    //Résolution de l'écran pour une mise à l'échelle de l'image.
    void determine_echelle(float resolution_l,float resolution_h,float distance_ecran,float hauteur_ecran)
    {
        resolution_x=resolution_l;
        resolution_y=resolution_h;
        echelle=distance_ecran/(hauteur_ecran/resolution_h);
    }



    void determine_zoom(uint32_t indice, float zoom)
    {
        sprites[indice].zoom=zoom;
    }

};


#endif // SPRITES_3D_H_INCLUDED
