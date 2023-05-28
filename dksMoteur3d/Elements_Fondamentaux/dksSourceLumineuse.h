#ifndef DKSSOURCELUMINEUSE_H_INCLUDED
#define DKSSOURCELUMINEUSE_H_INCLUDED

#include "dksConteneur.h"
#include "dksOeil.h"
#include <math.h>
#include <vector>

#define DKSCONTENEUR_CLASSE_SOURCE_LUMINEUSE "Source_lumineuse"
#define DKSCONTENEUR_CLASSE_SOURCE_LUMINEUSE_ID 3

//Types de source:

#define DKSSOURCELUMINEUSE_INFINIE 1
#define DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE 2
#define DKSSOURCELUMINEUSE_DIRECTIONNELLE 3

//Erreurs
#define DKSSOURCELUMINEUSE_OK 0
#define DKSSOURCELUMINEUSE_ERREUR_ACTIVE 1

//===========================================================================================
//                          DEFINITION DE LA CLASSE SOURCE_LUMINEUSE
//
//  Note sur les activations:
//
//          active()/desactive() : active/désactive la vision depuis la source (dans le cas de l'utilisation de la source comme oeil)
//          active_lumiere()/desactive_lumiere: active/désactive l'éclairage de la source
//          lumiere_est_active() : renvoie l'état d'éclairage
//          est_actif() :renvoie l'état de la source en tant qu'oeil
//
//===========================================================================================

class dksSourceLumineuse :public dksOeil
{
    private:
        uint16_t   id; //Identifiant pour openGl
        GLuint   id_openGl; //Identifiant pour openGl



        uint8_t type;
        float ouverture_spot;
        float intensite;
        float attenuation_constante;
        float attenuation_lineaire;
        float attenuation_carree;

        float couleur_ambience_base[4];      //Couleurs de base
        float couleur_diffusion_base[4];
        float couleur_speculaire_base[4];

        float couleur_ambience[4];      //Couleurs après application de l'intensité
        float couleur_diffusion[4];
        float couleur_speculaire[4];

        float openGl_position[4];

        bool drapeau_lumiere_active;            //True lorsque la source est activée.
        Matrice4x4 matrice_uv;      //La matrice de projection de la texture de la scène sur les volumes. Sert surtout pour les ombres portées
        Matrice4x4 matrice_correction_UV;       //Matrice de corection des coordonnées UV (l'écran est en -1,-1 1,1, alors que les UV sont en 0,0 1,1)
        Matrice4x4 matrice_temp;

    public:
        bool drapeau_debug;

        GLfloat position[4];
        GLfloat direction[4];
        static uint32_t allocation_lumieres_openGl; //Les drapeaux d'allocation des sources


    //============================ CONSTRUCTEUR ==========================

        dksSourceLumineuse( char* p_nom = "Source",
                            double px = 0., double py = 0., double pz = 0.,
                            uint32_t p_couleur_diffusion=0xffffffff,
                            uint32_t p_couleur_ambiance=0xff000000,
                            uint32_t p_couleur_speculaire=0xffffffff,
                            dksVecteurUnitaire* pdirection_regard = NULL,
                            uint8_t p_type = DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE,
                            float p_ouverture_spot=45.,
                            float p_intensite=1.0
                            )
                            :dksOeil(p_nom,px,py,pz,pdirection_regard,NULL)
        {
            drapeau_lumiere_active=false;
            classe=DKSCONTENEUR_CLASSE_SOURCE_LUMINEUSE;
            classe_id=DKSCONTENEUR_CLASSE_SOURCE_LUMINEUSE_ID;
            determine_type(p_type);
            determine_ouverture_spot(p_ouverture_spot);
            determine_intensite(p_intensite);
            determine_attenuation(1.,0.,0.);
            erreur=DKSSOURCELUMINEUSE_OK;

            determine_couleur_ambiance(p_couleur_ambiance);
            determine_couleur_diffusion(p_couleur_diffusion);
            determine_couleur_speculaire(p_couleur_speculaire);

            //Initialise la matrice de correction pour la projection des ombres:
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(.5,.5,.5); //...des coordonnées déplacées vers la droite et vers le haut
            glScalef(0.5,0.5,.5);   //Division par deux...
            glGetDoublev(GL_MODELVIEW_MATRIX,matrice_correction_UV.matrice);
            glPopMatrix();
        }

    //===============================================================================================
    //      Gestion des paramètres d'openGl
    //      Ces fonctions sont appelées uniquement lorsque la source est active au sein d'OpenGl
    //===============================================================================================
    private:
        //Fonction appelée lors de l'activation de la source au sein d'OpenGl:
        void initialise_openGl()
        {
            maj_openGl_intensite();
            maj_openGl_attenuation();
            //maj_openGl_couleur();
            maj();
        }


        void maj_openGl_couleur()
        {
            glLightfv(id_openGl, GL_AMBIENT,  couleur_ambience);
            glLightfv(id_openGl, GL_DIFFUSE,  couleur_diffusion);
            glLightfv(id_openGl, GL_SPECULAR, couleur_speculaire);
        }

        void maj_openGl_intensite()
        {
            couleur_ambience={
                                couleur_ambience_base[0]*intensite,
                                couleur_ambience_base[1]*intensite,
                                couleur_ambience_base[2]*intensite,
                                couleur_ambience_base[3]
                            };
            couleur_diffusion={
                                couleur_diffusion_base[0]*intensite,
                                couleur_diffusion_base[1]*intensite,
                                couleur_diffusion_base[2]*intensite,
                                couleur_diffusion_base[3]
                            };
            couleur_speculaire={
                                couleur_speculaire_base[0]*intensite,
                                couleur_speculaire_base[1]*intensite,
                                couleur_speculaire_base[2]*intensite,
                                couleur_speculaire_base[3]
                            };
            maj_openGl_couleur();
        }

        void maj_openGl_attenuation()
        {
            glLightf(id_openGl,GL_CONSTANT_ATTENUATION,attenuation_constante);
            glLightf(id_openGl,GL_LINEAR_ATTENUATION,attenuation_lineaire);
            glLightf(id_openGl,GL_QUADRATIC_ATTENUATION,attenuation_carree);
        }

        //Source directionnelle ou située à l'infini:
        void maj_openGl_orientation()
        {
            GLfloat direction[3];

            direction[0]=(float)(direction_regard->x);
            direction[1]=(float)(direction_regard->y);
            direction[2]=(float)(direction_regard->z);

            //cout<<"Direction regard:"<<direction[0]<<","<<direction[1]<<","<<direction[2]<<endl;

            if (type==DKSSOURCELUMINEUSE_DIRECTIONNELLE)
            {
                direction[3]=1.f;
                glLightfv(id_openGl, GL_SPOT_DIRECTION, direction);
            }
            else if (type==DKSSOURCELUMINEUSE_INFINIE)
            {
                direction[3]=0.f;
                glLightfv(id_openGl, GL_POSITION, direction);
            }
        }

    //======================================
    //      Gestion des couleurs
    //  Format d'entrée: 0xAARRVVBB
    //======================================
public:
        void determine_couleur_ambiance(uint32_t p_couleur)
        {
            couleur_ambience_base[0]=(float)((p_couleur>>16)&0xff)/255.f;
            couleur_ambience_base[1]=(float)((p_couleur>>8)&0xff)/255.f;
            couleur_ambience_base[2]=(float)(p_couleur&0xff)/255.f;
            couleur_ambience_base[3]=(float)((p_couleur>>24)&0xff)/255.f;
            if (drapeau_lumiere_active) maj_openGl_intensite();//glLightfv(id_openGl, GL_AMBIENT,  couleur_ambience);
        }

        void determine_couleur_diffusion(uint32_t p_couleur)
        {
            couleur_diffusion_base[0]=(float)((p_couleur>>16)&0xff)/255.f;
            couleur_diffusion_base[1]=(float)((p_couleur>>8)&0xff)/255.f;
            couleur_diffusion_base[2]=(float)(p_couleur&0xff)/255.f;
            couleur_diffusion_base[3]=(float)((p_couleur>>24)&0xff)/255.f;
            if (drapeau_lumiere_active) maj_openGl_intensite();//glLightfv(id_openGl, GL_DIFFUSE,  couleur_diffusion);
        }

        void determine_couleur_speculaire(uint32_t p_couleur)
        {
            couleur_speculaire_base[0]=(float)((p_couleur>>16)&0xff)/255.f;
            couleur_speculaire_base[1]=(float)((p_couleur>>8)&0xff)/255.f;
            couleur_speculaire_base[2]=(float)(p_couleur&0xff)/255.f;
            couleur_speculaire_base[3]=(float)((p_couleur>>24)&0xff)/255.f;
            if (drapeau_lumiere_active) maj_openGl_intensite();//glLightfv(id_openGl, GL_SPECULAR,  couleur_speculaire);
        }

        uint32_t renvoie_couleur_ambiance_ARVB()
        {
            return  (uint32_t(couleur_ambience_base[3]*255.)<<24)
                    | (uint32_t(couleur_ambience_base[0]*255.)<<16)
                    |  (uint32_t(couleur_ambience_base[1]*255.)<<8)
                    |  (uint32_t(couleur_ambience_base[2]*255.));
        }

        uint32_t renvoie_couleur_diffusion_ARVB()
        {
            return  (uint32_t(couleur_diffusion_base[3]*255.)<<24)
                    | (uint32_t(couleur_diffusion_base[0]*255.)<<16)
                    |  (uint32_t(couleur_diffusion_base[1]*255.)<<8)
                    |  (uint32_t(couleur_diffusion_base[2]*255.));
        }

        uint32_t renvoie_couleur_speculaire_ARVB()
        {
            return  (uint32_t(couleur_speculaire_base[3]*255.)<<24)
                    | (uint32_t(couleur_speculaire_base[0]*255.)<<16)
                    |  (uint32_t(couleur_speculaire_base[1]*255.)<<8)
                    |  (uint32_t(couleur_speculaire_base[2]*255.));
        }


    //===================================================
    //      Rafraichissement de la source.
    //      Fonction à appeler à chaque rendu d'image
    //      Cette fonction est nécessaire car l'accès au repère de la source est public.
    //===================================================
        void maj()
        {
            if (!drapeau_lumiere_active) return;


            switch (type)
            {
                case DKSSOURCELUMINEUSE_DIRECTIONNELLE:
                    direction[0]=(float)(direction_regard->x);
                    direction[1]=(float)(direction_regard->y);
                    direction[2]=(float)(direction_regard->z);
                    position[0]=(float)repere_absolu->x;
                    position[1]=(float)repere_absolu->y;
                    position[2]=(float)repere_absolu->z;
                    position[3]=1.f;
                    glLightfv(id_openGl, GL_POSITION, position);
                    glLightfv(id_openGl, GL_SPOT_DIRECTION, direction);
                    break;

                case DKSSOURCELUMINEUSE_INFINIE:
                    direction[0]=(float)(-direction_regard->x);
                    direction[1]=(float)(-direction_regard->y);
                    direction[2]=(float)(-direction_regard->z);
                    direction[4]=0.f;
                    glLightfv(id_openGl, GL_POSITION, direction);
                    break;

                case DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE:
                    position[0]=float(repere_absolu->x);
                    position[1]=float(repere_absolu->y);
                    position[2]=float(repere_absolu->z);
                    position[3]=1.f;
                    glLightfv(id_openGl, GL_POSITION, position);
                    break;
            }
        }
        //======================================
        //      Gestion des paramètres
        /// Au sein d'une scène, appeler dskMoteur3d::active/desactive_source_lumineuse()
        //======================================

        //----Active/Désactive la source:
        //      Renvoie false si la source n'a pas pu être activée.
        bool active_lumiere()
        {
            GLuint table_openGl_id[8]={
                                    GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,
                                    GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7
                                };

            if (drapeau_lumiere_active) return true;
            for (id=0 ;id<8;id++)
            {
                if ((allocation_lumieres_openGl&(1<<id))==0)
                {
                    allocation_lumieres_openGl|=(1<<id);
                    drapeau_lumiere_active=true;
                    id_openGl=table_openGl_id[id];
                    glEnable(id_openGl); //L'activation se fait par maj_intensite()
                    initialise_openGl();
                    return true;
                }
            }
           erreur=DKSSOURCELUMINEUSE_ERREUR_ACTIVE;
           return false;
        }

        void desactive_lumiere()
        {
            if (!drapeau_lumiere_active) return;
            glDisable(id_openGl);
            allocation_lumieres_openGl&=-(1<<id)-1;
            drapeau_lumiere_active=false;
        }

        //Contrainte due au genre...
        bool lumiere_est_active()
        {
            return drapeau_lumiere_active;
        }

        //----Détermination des paramètres:
        void determine_intensite(float p_intensite)
        {
            if (p_intensite<0.)p_intensite=0.;
            intensite=p_intensite;
            if (drapeau_lumiere_active) maj_openGl_intensite();
        }

        float renvoie_intensite()
        {
            return intensite;
        }

        void determine_attenuation(float p_constante,float p_lineaire,float p_carree)
        {
            attenuation_constante=p_constante;
            attenuation_lineaire=p_lineaire;
            attenuation_carree=p_carree;
            if (drapeau_lumiere_active) maj_openGl_attenuation();
        }
        float renvoie_attenuation_constante()
        {
            return attenuation_constante;
        }
        float renvoie_attenuation_lineaire()
        {
            return attenuation_lineaire;
        }
        float renvoie_attenuation_carree()
        {
            return attenuation_carree;
        }

        void determine_type(uint8_t p_type)
        {
            if ( (p_type!=DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE) && (type!=DKSSOURCELUMINEUSE_INFINIE) && (type!=DKSSOURCELUMINEUSE_DIRECTIONNELLE) ) return;
            type=p_type;
            if (!drapeau_lumiere_active) return;
            if ( (type==DKSSOURCELUMINEUSE_OMNIDIRECTIONNELLE) || (type==DKSSOURCELUMINEUSE_INFINIE) ) glLightf(id_openGl, GL_SPOT_CUTOFF,180.); //La spéciale valeur 180 indique à openGl que la source n'est plus directionelle.
            else glLightf(id_openGl, GL_SPOT_CUTOFF,ouverture_spot);
            maj();
        }

        uint8_t renvoie_type()
        {
            return type;
        }

        void determine_ouverture_spot(float p_ouverture_spot)
        {
            if (p_ouverture_spot>90.)p_ouverture_spot=90.;
            ouverture_spot=p_ouverture_spot;
            if ((type==DKSSOURCELUMINEUSE_DIRECTIONNELLE) && drapeau_lumiere_active) glLightf(id_openGl, GL_SPOT_CUTOFF,ouverture_spot);
        }

        float renvoie_ouverture_spot()
        {
            return ouverture_spot;
        }

        GLuint renvoie_id_openGl()
        {
            return id_openGl;
        }

        //---------------------------------------------
        //Copie les paramètres d'une source:
        //Pour copier le repère et la position, utiliser copie()
        void copie_parametres(dksSourceLumineuse* source_org)
        {
            determine_attenuation(source_org->renvoie_attenuation_constante(),source_org->renvoie_attenuation_lineaire(),source_org->renvoie_attenuation_carree());
            determine_couleur_ambiance(source_org->renvoie_couleur_ambiance_ARVB());
            determine_couleur_diffusion(source_org->renvoie_couleur_diffusion_ARVB());
            determine_couleur_speculaire(source_org->renvoie_couleur_speculaire_ARVB());
            determine_ouverture_spot(source_org->renvoie_ouverture_spot());
            determine_type(source_org->renvoie_type());
            determine_intensite(source_org->renvoie_intensite());
        }

        //=========================================================================
        //      Gestion des projections:
        //=========================================================================

        virtual void positionne_scene()
        {
            this->dksOeil::positionne_scene();
            //Calcul la matrice de projection de la texture du rendu pour le calcul des UV:
            matrice_correction_UV.multiplication_droite(matrice_projection,matrice_temp);
            matrice_temp.multiplication_droite(matrice_vision,matrice_uv);
        }

        //Sert pour les rendus en plusieurs passes (calcul des ombres portées...):
        //La différence avec maj_projection() de la classe héritée dksOeil est qu'il
        // n'y a pas de test du drapeau d'activation de la vision.
        void maj_projection_source(uint32_t resX,uint32_t resY)
        {
                glViewport(0,0,resX,resY);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(ouverture,rapport_xy,zmin,zmax);
                glMatrixMode(GL_MODELVIEW);
        }

        //Renvoie la matrice de projection de la scène vue de la source, pour une projection sur les volumes (uv)
        //Cette matrice sert surtout pour la projection de la texture de profondeur, afin de rendre les ombres portées.
        ///Il faut que la fonction positionne_scene() aie été appelée pour un calcul correcte de cette matrice.
        Matrice4x4* renvoie_matrice_uv()
        {
            return &matrice_uv;
        }


};
#endif // SOURCE_LUMINEUSE_H_INCLUDED
