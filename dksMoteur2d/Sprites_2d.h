#ifndef SPRITES_2D_H_INCLUDED
#define SPRITES_2D_H_INCLUDED

/*
        Les sprites 2d sont affichés par transferts de pixels. On peut donc effectuer des
        traitements graphiques au pixel dans ces tempons.
*/

#include <vector>
#include "../chargeurs_images/Image_TIFF.h"

#define SPR2D_OK 0
#define SPR2D_ERREUR_MEMOIRE 1
#define SPR2D_ERREUR_TIFF 2

typedef struct
{
    Image_TIFF* image;
    float x;    //Position de l'image à l'écran
    float y;
    float zoom;
    bool drapeau_stencil;   //Si "true", le sprite est un cache à afficher dans le tempon "stencil" d'OpenGl

} sprite_2d;

class Sprites_2d
{
    private:
        uint32_t nbrImages;
        vector<sprite_2d> sprites;
        uint32_t resolution_x;
        uint32_t resolution_y;

    public:
        uint32_t erreur;

    //============== Constructeur

    Sprites_2d (uint32_t p_nbrImages,char** noms_fichiers)
    {

        erreur=SPR2D_OK;
        nbrImages=p_nbrImages;
        resolution_x=1024;
        resolution_y=768;

        // Charge les images:
        for (uint32_t i=0;i<nbrImages;i++)
        {
            sprite_2d nouveau_sprite;
            nouveau_sprite.x=0.;
            nouveau_sprite.y=0.;
            nouveau_sprite.zoom=1.;
            nouveau_sprite.drapeau_stencil=false;
            nouveau_sprite.image=new Image_TIFF(noms_fichiers[i],ORIENTATION_BOTLEFT);
            if (nouveau_sprite.image==0 || nouveau_sprite.image->erreur!=IMAGE_TIFF_OK)
            {
                erreur=SPR2D_ERREUR_TIFF;
                return;
            }
            sprites.push_back(nouveau_sprite);
        }

    }

    //================= Destructeur:
    ~Sprites_2d()
    {
        cout<<"DESTRUCTION SPRITES2D"<<endl;

        uint32_t taille=sprites.size();
        for (uint32_t i=0;i<taille;i++)
        {
            delete(sprites[i].image);
        }
        sprites.clear();
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

    //====================================================
    //  PAramètres des sprites:
    //====================================================

    void determine_resolution(uint32_t r_l, uint32_t r_h)
    {
        resolution_x=r_l;
        resolution_y=r_h;
    }

    void determine_zoom(uint32_t indice, float zoom)
    {
        sprites[indice].zoom=zoom;
    }

    //Le drapeau stencil ne peut être activé que si l'image est en mode 8 bits par pixel
    void determine_stencil(uint32_t indice,bool p_drapeau)
    {
        if (sprites[indice].image->bits_par_pixel==8)
        {
            sprites[indice].image->format_pixels=GL_STENCIL_INDEX;
            sprites[indice].drapeau_stencil=p_drapeau;
        }
    }

    //====================================================
    //  Affichage des sprites via un tempon de pixels
    //====================================================

    void affiche_sprite_2d(uint32_t indice)
    {

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0., (float)resolution_x,0. ,(float)resolution_y,-1.,100.);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();


        bool lighting_mem=glIsEnabled(GL_LIGHTING);
        bool blend_mem=glIsEnabled(GL_BLEND);
        bool texture_mem=glIsEnabled(GL_TEXTURE_2D);
        bool depth_test_mem=glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST); //Désactive la prise en compte de la profondeur ("z-buffer")

        Image_TIFF* image=sprites[indice].image;
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glRasterPos2f (sprites[indice].x, sprites[indice].y);
        glDrawPixels(image->largeur,image->hauteur,image->format_pixels,GL_UNSIGNED_BYTE,image->image);


        if (!blend_mem) glDisable(GL_BLEND);
        if (lighting_mem) glEnable(GL_LIGHTING);
        if (depth_test_mem) glEnable(GL_DEPTH_TEST);
        if (!texture_mem) glDisable(GL_TEXTURE_2D);


        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

    }
};


#endif // SPRITES_2D_H_INCLUDED
