#ifndef DKSSTARFIELD_H_INCLUDED
#define DKSSTARFIELD_H_INCLUDED

/*

    Rendus des champs d'étoiles
    L'affichage se fait par ordre de priorité:
        Si image==NULL et sprite==NULL, les étoiles sont affichés par de simples points
        Si image!=NULL et sprite==NULL, les étoiles sont affichées avec l'extension spritePoint
        Si sprite!=NULL, les étoiles sont affichées avec le sprite.

*/

#include "dksParticules.h"
#include "../../chargeurs_images/Image.h"
#include "dksSprite3d.h"

#define DKSSTARFIELD_OK 0
#define DKSSTARFIELD_ERREUR_IMAGE 1


class dksStarfield : public dksParticules
{
    public:
        double rayon; //le rayon du champs. Si un point sort de ce rayon en arrière du déplacement,il est replacé en avant. Attention aux points situés trop loin hors du rayon.
        double taille_points;    //Taille des points en unités d'espace
        uint32_t couleur;   //Couleur des points
        float alpha;
        bool drapeau_destruction_image; //True si l'image est détruite par le destructeur

    private:
        bool drapeau_initialisation;    //True lorsque le vecteur d'orientation peut être calculé (à la deuxième mise à jour...)
        double orientation_deplacement[3]; //le vecteur UNITAIRE indiquant la direction et le sens du déplacement
        double centre_prec[3]; //Sert pour le calcul de l'orientation
        Image* image;           //L'image à afficher à la place des points (la transparence ne marche pas....)
        dksSprite3d* sprite;    //Apparence des points


        //======================================================
        //          CONSTRUCTEUR
        //======================================================
    public:
        dksStarfield(char* p_nom="Starfield",
                    double px=0.,double py=0.,double pz=0., //Centre du champ
                    uint32_t p_nbrPoints=0,
                    double p_rayon=1000.,       //Rayon du champ d'étoiles
                    double p_taille_points=1.,  //taille des étoiles en unités d'espace
                    dksSprite3d* p_sprite=NULL,
                    char* nom_fichier_image=NULL
                    ):dksParticules(p_nom,px,py,pz,p_nbrPoints)
        {
                taille_points=p_taille_points;
                rayon=p_rayon;
                image=NULL;
                sprite=NULL;
                reset();
                couleur=0xffffffff;
                alpha=float(couleur>>24)/255.;
                if (nom_fichier_image!=NULL) determine_image(nom_fichier_image);
                determine_sprite(p_sprite);
                drapeau_destruction_image=true;
        }

        //================= DESTRUCTEUR
        ~dksStarfield()
        {
            if(drapeau_destruction_image && image!=NULL) delete image;
        }

        //-------------------------------------------------
        //  Enclenchement du starfield
        //  A appeler au départ du mouvement
        //-------------------------------------------------
        void reset()
        {
            drapeau_initialisation=false;
        }

        void desactive_reset()
        {
            drapeau_initialisation=true;
        }

        //-------------------------------------------------
        //  Détermine une texture pour les points:
        //-------------------------------------------------
        bool determine_image(char* nom_fichier=NULL)
        {
            if (image!=NULL) delete image;
            image=NULL;
            if (nom_fichier==NULL) return true;
            image=new Image(nom_fichier);
            if (image==NULL || image->erreur!=IMAGE_OK)
            {
                erreur=DKSSTARFIELD_ERREUR_IMAGE;
                cout<<"ERREUR dans dksStarfield::determine_image() - Impossible de charger l'image "<<nom_fichier<<endl;
                return false;
            }
            if (!image->genere_texture_openGL()) {cout<<"ERREUR dans dksStarfield::determine_image()"<<endl;erreur=DKSSTARFIELD_ERREUR_IMAGE; return false;}
            return true;
        }

        bool determine_image(Image* p_image=NULL)
        {
            if (image!=NULL) delete image;
            image=NULL;
            if (p_image==NULL) return true;
            image=p_image;
            if (!image->genere_texture_openGL()) {cout<<"ERREUR dans dksStarfield::determine_image()"<<endl;erreur=DKSSTARFIELD_ERREUR_IMAGE; return false;}
            return true;
        }

        //--------------------------------------------------
        //      Détermine un sprite pour l'image des étoiles
        //--------------------------------------------------
        void determine_sprite(dksSprite3d* p_sprite)
        {
            sprite=p_sprite;
            if (sprite!=NULL )
            {
                sprite->facteur_taille=taille_points/float(sprite->image->largeur);
            }
        }

        //-----------------------------------------------------------
        //          Inits divers
        //-----------------------------------------------------------
        void determine_centre_precedent(double *cp)
        {
            centre_prec={cp[0],cp[1],cp[2]};
        }

        void renvoie_centre_precedent(double* cp)
        {
            cp[0]=centre_prec[0];
            cp[1]=centre_prec[1];
            cp[2]=centre_prec[2];
        }

        //-----------------------------------------------------------------------
        //       Simple champ d'étoiles:
        /// Appeler APRES la mise à jour de la scène
        //-----------------------------------------------------------------------
        void maj_champ_etoiles_base(double cx, double cy, double cz)
        {
            uint32_t i;
            double n,px,py,pz,distance;
            if (!drapeau_initialisation)
            {
                centre_prec={cx,cy,cz};
                drapeau_initialisation=true;
            }

            //Calcul le vecteur de déplacement:
            orientation_deplacement=  {
                                        cx-centre_prec[0],
                                        cy-centre_prec[1],
                                        cz-centre_prec[2]
                                    };

            n=sqrt( orientation_deplacement[0]*orientation_deplacement[0]
                    +orientation_deplacement[1]*orientation_deplacement[1]
                    +orientation_deplacement[2]*orientation_deplacement[2] );

            centre_prec={cx,cy,cz};

            //repositionne les points qui débordent si le déplacement n'est pas nul:
            if (n==0) return;

            orientation_deplacement[0]/=n;
            orientation_deplacement[1]/=n;
            orientation_deplacement[2]/=n;

            for(i=0;i<nbrPoints;i++)
            {
                px=points[3*i]-cx;
                py=points[3*i+1]-cy;
                pz=points[3*i+2]-cz;
                distance=px*orientation_deplacement[0]+py*orientation_deplacement[1]+pz*orientation_deplacement[2];
                if (distance<=-rayon)
                {
                    px=orientation_deplacement[0]*rayon*2.;
                    py=orientation_deplacement[1]*rayon*2.;
                    pz=orientation_deplacement[2]*rayon*2.;
                    points[3*i]+=px;
                    points[3*i+1]+=py;
                    points[3*i+2]+=pz;
                }
            }
        }

        //-----------------------------------------------------------
        //                      Affichage
        //-----------------------------------------------------------
        virtual void affiche(dksOeil* oeil)
        {
            //Sauvegarde états sensibles openGL:
            bool blend_mem=glIsEnabled(GL_BLEND);
            bool lighting_mem=glIsEnabled(GL_LIGHTING);
            GLboolean depth_mask_mem;
            glGetBooleanv(GL_DEPTH_WRITEMASK,&depth_mask_mem);

            //Initialise la transparence;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);//GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);

            //Affiche des points si pas de sprite
            if (sprite==NULL) affiche_points(oeil);
            //Affiche les étoiles avec le sprite:
            else
            {
                dksSprite3d::initialise_affichage();
                uint32_t i;

                for (i=0;i<nbrPoints;i++) sprite->affiche_texture_optimisee(&points[i*3],oeil,alpha);

                dksSprite3d::fin_affichage();
            }

            //Restitue les états sensibles d'openGL:
            glDepthMask(depth_mask_mem);
            if (!blend_mem) glDisable(GL_BLEND);
            if (lighting_mem) glEnable(GL_LIGHTING);
        }

        //-----------------------------------------------------------
        //      Affiche les étoiles avec des points
        /// Les points sont limités à une taille de 64*64 pixels....
        //-----------------------------------------------------------

        void affiche_points(dksOeil* oeil)
        {
            uint32_t i;

            //Initialise openGl:
            bool point_smooth_mem=glIsEnabled(GL_POINT_SMOOTH);

            glEnable(GL_POINT_SMOOTH);
            glColor4f(float(uint8_t(couleur>>16))/255.,float(uint8_t(couleur>>8))/255.,float(uint8_t(couleur)),float(uint8_t(couleur>>24))/255.);

            //Calcul la taille des points:
            GLint resolution[4];
            glGetIntegerv(GL_VIEWPORT,resolution);
            float taille=taille_points * resolution[3] / (2*tanf(oeil->renvoie_ouverture()/360*M_PI));

            glPointSize(taille);
            GLfloat attenuation[3]={0.,0.,1.};
            //glPointSize(10);
            //GLfloat attenuation[3]={1.,0.,0.};

            glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,attenuation);
            glPointParameterf(GL_POINT_SIZE_MAX,128.);
            glPointParameterf(GL_POINT_SIZE_MIN,1.);

            //Initialise la texture:
            desactive_textures();
            if (image!=NULL)
            {
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_POINT_SPRITE);
                glBindTexture(GL_TEXTURE_2D,image->texture_id);
                glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,GL_TRUE);
                glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN,GL_LOWER_LEFT);
                glEnable(GL_TEXTURE_2D);
            }

            //Affiche les points:
            glBegin(GL_POINTS);
                for (i=0;i<nbrPoints;i++) glVertex3dv(&points[3*i]);
            glEnd();

            //Restitue les données évcentuellement sensibles:

            if (!point_smooth_mem) glDisable(GL_POINT_SMOOTH);

            if (image!=NULL)
            {
                glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,GL_FALSE);
                glDisable(GL_POINT_SPRITE);
                glDisable(GL_TEXTURE_2D);
            }

        }

        //-----------------------------------------------------------
        //     Affichage pour le rendu des pixels luminescents
        /// Les points sont limités à une taille de 64*64 pixels....
        //-----------------------------------------------------------
        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {
            affiche(oeil);
        }

};

#endif // DKSSTARFIELD_H_INCLUDED
