#ifndef DKSAFFICHEIMAGE_H_INCLUDED
#define DKSAFFICHEIMAGE_H_INCLUDED

/*
    Gestion de l'affichage d'une image en arrière-plan, avec rotation, zoom, scroll...
*/

#include "../chargeurs_images/Image.h"
#include "dksRendu.h"
#include "dksEcran.h"
#include "../dksMoteur3d/Elements_Fondamentaux/dksOeil.h"

//================ Erreurs:
#define DKSAFFICHEIMAGE_OK 0
#define DKSAFFICHEIMAGE_ERREUR_IMAGE 1

//==================================================================
//                      Classe
//==================================================================

class dksAfficheImage :public dksRendu
{
    public:
        uint16_t erreur;
        Image *image;
        float alpha;
        float couleur_fond[3];
        float couleur_teinte[3];
        float angle_rotation;
        bool drapeau_proportions;   //True si le zoom commun aux deux dimension est utilisé (l'image garde alors ses proportions)
        float zoom;
        float zoom_x;
        float zoom_y;
        float xpos;
        float ypos;
    private:
        dksEcran* ecran;
        //Paramètres de rendu:
        GLint recouvrement_s;   //GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT
        GLint recouvrement_t;
        GLint type_grossissement;    //GL_LINEAR, GL_NEAREST
        GLint type_diminution;
        GLint type_rendu;   //GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND, GL_ADD, GL_COMBINE


    //===============================
    //      Constructeur
    //===============================
    public:
        dksAfficheImage(Image *p_image,dksEcran* p_ecran):dksRendu()
        {
            erreur=DKSAFFICHEIMAGE_OK;
            alpha=1.;
            image=p_image;
            ecran=p_ecran;
            if(image->texture_id==0)
                if(!image->genere_texture_openGL())
                {
                    erreur=DKSAFFICHEIMAGE_ERREUR_IMAGE;
                    return;
                }
            affecte(couleur_fond,0.,0.,0.);
            affecte(couleur_teinte,1.,1.,1.);
            recouvrement_s=GL_CLAMP_TO_BORDER;
            recouvrement_t=GL_CLAMP_TO_BORDER;
            type_grossissement=GL_LINEAR;
            type_diminution=GL_LINEAR;
            type_rendu=GL_REPLACE;
            angle_rotation=0.;
            zoom_x=1.;
            zoom_y=1.;
            zoom=1.;
            xpos=0.;
            ypos=0.;
            drapeau_proportions=true;

        }

    //===============================
    //      Destructeur
    //===============================
        ~dksAfficheImage()
        {

        }

    //===============================
    //      Paramétrage
    //===============================
    public:
        void determine_position(float* pos)
        {
            xpos=pos[0];
            ypos=pos[1];
        }
    //===============================
    //      Affichages
    //===============================
    public:
        virtual void affiche(dksOeil* oeil)
        {
            //glViewport(0,0,ecran->resolution_x,ecran->resolution_y);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GLboolean depth_mask_mem;
            glGetBooleanv(GL_DEPTH_WRITEMASK,&depth_mask_mem);
            GLboolean depth_mem=dksEtat::est_active(GL_DEPTH_TEST);

            desactive_textures();
            dksEtat::utilise_programme(0);

            dksMatrices::mode(GL_PROJECTION);
            dksMatrices::empile();
            dksMatrices::identite();
            float ecran_xdim=float(ecran->resolution_x)/2.;
            float ecran_ydim=float(ecran->resolution_y)/2.;

            dksMatrices::ortho2D(-ecran_xdim,ecran_xdim,-ecran_ydim,ecran_ydim);
            dksMatrices::mode(GL_MODELVIEW);
            dksMatrices::empile();
            dksMatrices::identite();


            glDepthMask(GL_FALSE);
            dksEtat::desactive(GL_CULL_FACE);
            dksEtat::desactive(GL_LIGHTING);
            dksEtat::desactive(GL_DEPTH_TEST);
            dksEtat::active(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);



            //---------Activation de la texture:

            dksEtat::unite_active(GL_TEXTURE0);
            dksEtat::active(GL_TEXTURE_2D);
            dksEtat::lie_texture(GL_TEXTURE_2D,image->texture_id);
            glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_fond);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,recouvrement_s);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,recouvrement_t);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,type_grossissement);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,type_diminution);
            dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,type_rendu);



            //------- Calcul les rapports:
            float re=ecran_xdim/ecran_ydim;
            float ri=float(image->largeur)/float(image->hauteur);

            float zix,ziy;
            if(ecran->resolution_y*ri>ecran->resolution_x)
            {
                zix=1./re;
                ziy=ri/re;
            }
            else
            {
                zix=1./ri;
                ziy=1.;
            }

            //------- Application des transformations:
                dksMatrices::mode(GL_TEXTURE);
                dksMatrices::empile();
                dksMatrices::identite();
                dksMatrices::translation(xpos+0.5,ypos+0.5,0.);
                if(drapeau_proportions) dksMatrices::echelle(zoom,zoom,1.); else dksMatrices::echelle(zoom_x,zoom_y,1.); //Zoom utilisateur
                dksMatrices::echelle(zix,ziy,1.);       //Mise à l'échelle
                dksMatrices::rotation(angle_rotation,0.,0.,1.);


            //Affiche la texture:
            GLboolean color_material_mem=dksEtat::est_active(GL_COLOR_MATERIAL);
            dksEtat::desactive(GL_COLOR_MATERIAL);
            dksImmediat::couleur4f(couleur_teinte[0],couleur_teinte[1],couleur_teinte[2],alpha);
            dksImmediat::debut(GL_QUADS);


            dksImmediat::texture2d(-re/2.,-0.5);
            dksImmediat::sommet3d(-ecran_xdim,-ecran_ydim,-1.);
            dksImmediat::texture2d(re/2.,-0.5);
            dksImmediat::sommet3d(ecran_xdim,-ecran_ydim,-1.);
            dksImmediat::texture2d(re/2.,.5);
            dksImmediat::sommet3d(ecran_xdim,ecran_ydim,-1.);
            dksImmediat::texture2d(-re/2.,0.5);
            dksImmediat::sommet3d(-ecran_xdim,ecran_ydim,-1.);


            dksImmediat::fin();
            if (color_material_mem) dksEtat::active(GL_COLOR_MATERIAL);



            //Restitue l'état d'openGl:
            dksMatrices::depile();  //Matrice de texture
            dksMatrices::mode(GL_MODELVIEW);
            dksEtat::desactive(GL_BLEND);
            dksMatrices::depile();
            dksMatrices::mode(GL_PROJECTION);
            dksMatrices::depile();
            dksMatrices::mode(GL_MODELVIEW);
            glDepthMask(depth_mask_mem);
            if(depth_mem)dksEtat::active(GL_DEPTH_TEST);
        }

        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {

        }

        virtual void affiche_profondeur(dksOeil* oeil)
        {

        }

};

#endif // DKSAFFICHEIMAGE_H_INCLUDED
