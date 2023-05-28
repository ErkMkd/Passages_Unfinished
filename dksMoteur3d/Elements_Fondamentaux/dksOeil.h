#ifndef DKSOEIL_H_INCLUDED
#define DKSOEIL_H_INCLUDED

#include <math.h>

#include "dksConteneur.h"

#define DKSCONTENEUR_CLASSE_OEIL "Oeil"
#define DKSCONTENEUR_CLASSE_OEIL_ID 2

#define DKSOEIL_VISEE_LIBRE 0       //Pas de visée particulière.
#define DKSOEIL_VISEE_POINT 1       //L'oeil vise un point
#define DKSOEIL_VISEE_CONTENEUR 2


class dksOeil :public dksConteneur
{
    public:
        dksVecteurUnitaire* direction_regard_relatif; //Z
		dksVecteurUnitaire* direction_haut_relatif;   //Y
		dksVecteurUnitaire* direction_gauche_relatif; //X

		dksVecteurUnitaire* direction_regard;
		dksVecteurUnitaire* direction_haut;
		dksVecteurUnitaire* direction_gauche;


    protected:
        Matrice4x4 matrice_projection;  //La matrice projetant les coordonnées sur la fenêtre de représentation
        Matrice4x4 matrice_vision;      //Matrice de positionnement de la scène
        Matrice4x4 matrice_projection_vision;   //Le produit matrice_projection*matrice_vision
        uint16_t mode_visee;
        double point_vise[3];
        dksConteneur* conteneur_vise;
        bool drapeau_visee_inertielle;
        bool drapeau_initialise_visee_inertielle;
        double inertie_visee;

        float ouverture;
        float rapport_xy;
        uint32_t resolution_x;
        uint32_t resolution_y;
        float zmin;
        float zmax;
        bool drapeau_actif; //True lorsque l'oeil est actif.


        //============================ CONSTRUCTEUR ==========================
    public:
        dksOeil(char* p_nom = "Oeil",
                double px = 0., double py = 0., double pz = 0.,
                dksVecteurUnitaire* pdirection_regard = NULL, dksVecteurUnitaire* pdirection_haut = NULL)
                :dksConteneur(p_nom,px,py,pz,NULL,pdirection_haut,pdirection_regard)
		{
		    mode_visee=DKSOEIL_VISEE_LIBRE;
		    conteneur_vise=NULL;

		    drapeau_actif=false;

		    point_vise[0]=0.;
		    point_vise[1]=0.;
		    point_vise[2]=0.;

		    drapeau_visee_inertielle=false;
		    drapeau_initialise_visee_inertielle=false;
		    inertie_visee=1.;

			direction_regard_relatif=axe_z;
			direction_gauche_relatif=axe_x;
			direction_haut_relatif=axe_y;

            classe = DKSCONTENEUR_CLASSE_OEIL;
            classe_id = DKSCONTENEUR_CLASSE_OEIL_ID;

			direction_regard = repere_absolu->axe_z;
			direction_haut = repere_absolu->axe_y;
			direction_gauche = repere_absolu->axe_x;

			determine_ouverture(45.);
		    determine_resolution(800,600);   //Mis à jour dès utilisation de l'oeil
		    determine_limites_profondeur(1.,1000.);
		}

		//DESTRUCTEUR:

		~dksOeil()
		{

		}

        //==================================================
        //          ON/OFF
        //==================================================
        virtual void active()
        {
            drapeau_actif=true;
            maj_projection();
        }

        virtual void desactive()
        {
            drapeau_actif=false;
        }

        virtual bool est_actif()
        {
            return drapeau_actif;
        }

		//=================================================================
		//              Positionne l'oeil dans la scène:
		//=================================================================

		//------------------------------------------------------------------------------
        //      Mise à jour du repère absolu et de la matrice.
        /// A n'utiliser que lorsque l'oeil n'est pas un élément d'une scène.
        /// La matrice courante devient la vue de l'oeil.
        //------------------------------------------------------------------------------
        virtual void maj(double t)
        {
            //position_temps=t;
            //delta_t=t-t0;

            applique_mouvements();
            calcul_repere_absolu();

            ///Pas besoin pour le moment:
            /*
            glPushMatrix();
            calcul_matrice();
            glPopMatrix();
            */

            positionne_scene();

            //t0=t;
        }

        //-------------------------------------------------------------------------------------------------
        // Lorsque l'oeil est inactif dans une scène, mais que l'on souhaite tout de même mettre à jour sa visée
        //-------------------------------------------------------------------------------------------------
        void maj_visee()
        {
            switch (mode_visee)
            {
                case DKSOEIL_VISEE_POINT:
                        visee_point();
                        break;
                case DKSOEIL_VISEE_CONTENEUR:
                    if (conteneur_vise!=NULL)
                    {
                        point_vise[0]=conteneur_vise->repere_absolu->x;
                        point_vise[1]=conteneur_vise->repere_absolu->y;
                        point_vise[2]=conteneur_vise->repere_absolu->z;
                        visee_point();
                    }
                    break;
                default: break;
            }
        }

        //-------------------------------------------------------
        // Lorsque l'oeil est actif dans une scène:
        //-------------------------------------------------------

        virtual void positionne_scene()
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            switch (mode_visee)
            {
                case DKSOEIL_VISEE_LIBRE: visee(); break;
                case DKSOEIL_VISEE_POINT:
                        visee_point();
                        visee();
                        break;
                case DKSOEIL_VISEE_CONTENEUR:
                    if (conteneur_vise==NULL) visee();
                    else
                    {
                        point_vise[0]=conteneur_vise->repere_absolu->x;
                        point_vise[1]=conteneur_vise->repere_absolu->y;
                        point_vise[2]=conteneur_vise->repere_absolu->z;
                        visee_point();
                        visee();
                    }
                    break;
                default: visee();break;
            }
            //Sauvegarde la matrice de la fenêtre de représentation:

            /*
            glGetDoublev(GL_MODELVIEW_MATRIX,projection.matrice);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glMultMatrixd(projection.matrice);
            glGetDoublev(GL_PROJECTION_MATRIX,projection.matrice);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            */

            glGetDoublev(GL_PROJECTION_MATRIX,matrice_projection.matrice);
            glGetDoublev(GL_MODELVIEW_MATRIX,matrice_vision.matrice);
            matrice_projection.multiplication_droite(matrice_vision,matrice_projection_vision);
        }

        //Renvoie les matrices de projection et de vision de l'oeil:
        Matrice4x4* renvoie_matrice_projection()
        {
            return &matrice_projection;
        }
        Matrice4x4* renvoie_matrice_vision()
        {
            return &matrice_vision;
        }

        Matrice4x4* renvoie_matrice_projection_vision()
        {
            return &matrice_projection_vision;
        }

        private:
        //------------------------------------------------------
        //          Calcul la matrice de vue
        //------------------------------------------------------
        void visee()
        {
            gluLookAt(repere_absolu->x,repere_absolu->y,repere_absolu->z,
                    repere_absolu->x+direction_regard->x,
                    repere_absolu->y+direction_regard->y,
                    repere_absolu->z+direction_regard->z,
                    direction_haut->x,
                    direction_haut->y,
                    direction_haut->z);
        }


        //------------------------------------------------------------------
        //                       Vise un point.
        //------------------------------------------------------------------
        void visee_point()
        {
            double vx,vy,vz,rx,ry,rz,angle;
            double prx,pry,prz;
            double n;
            double dx,dy,dz;
            dx=direction_regard_relatif->x;
            dy=direction_regard_relatif->y;
            dz=direction_regard_relatif->z;
            //Coordonnées du point visé dans le repère du parent de l'oeil:
            vx=point_vise[0]-parent->repere_absolu->x;
            vy=point_vise[1]-parent->repere_absolu->y;
            vz=point_vise[2]-parent->repere_absolu->z;

            prx= vx*parent->repere_absolu->axe_x->x + vy*parent->repere_absolu->axe_x->y + vz*parent->repere_absolu->axe_x->z;
            pry= vx*parent->repere_absolu->axe_y->x + vy*parent->repere_absolu->axe_y->y + vz*parent->repere_absolu->axe_y->z;
            prz= vx*parent->repere_absolu->axe_z->x + vy*parent->repere_absolu->axe_z->y + vz*parent->repere_absolu->axe_z->z;
            //Calcul le vecteur de visee:
            vx=prx-x;
            vy=pry-y;
            vz=prz-z;
            n=sqrt(vx*vx+vy*vy+vz*vz);
            vx/=n;
            vy/=n;
            vz/=n;

            //Calcul l'axe de rotation:
            rx=dy*vz-dz*vy;
            ry=dz*vx-dx*vz;
            rz=dx*vy-dy*vx;
            n=rx*rx+ry*ry+rz*rz;

            if(n==0)
            {
                if(rx*dx+ry*dy+rz*dz<0.)
                {
                    if(dy==1.)
                    {   //Si le regard est pointé vers le haut:
                        rx=axe_horizontal->x;
                        ry=axe_horizontal->y;
                        rz=axe_horizontal->z;
                    }
                    else
                    {
                        rx=axe_vertical->y*dz-axe_vertical->z*dy;
                        ry=axe_vertical->z*dx-axe_vertical->x*dz;
                        rz=axe_vertical->x*dy-axe_vertical->y*dx;
                    }
                }
                else
                {
                    visee();
                    return;
                }

            }
            else
            {
                n=sqrt(n);
                rx/=n;
                ry/=n;
                rz/=n;
            }


            //Calcul l'angle
            double cos_angle=direction_regard_relatif->x*vx+direction_regard_relatif->y*vy+direction_regard_relatif->z*vz;
            if (cos_angle>1.)cos_angle=1.;
            if (cos_angle<-1.)cos_angle=-1.;
            angle=acos(cos_angle);
            if (drapeau_visee_inertielle && (!drapeau_initialise_visee_inertielle) && angle>(1e-6))
            {
                angle*=delta_t*inertie_visee;
            }
            drapeau_initialise_visee_inertielle=false;
            //rotation avec contrainte de verticalité:
            //if (drapeau_contrainte_verticale) rotation_contrainte_verticale(angle,rx,ry,rz);
            //else
            rotation(angle,rx,ry,rz);

            //Mise à jour du repère absolu et propagation de la hiérarchie:
            calcul_repere_absolu();

        }

        //==========================================================
        //      Gestion des modes de visée
        //==========================================================
        public:
        void determine_mode_visee(uint16_t mv)
        {
            mode_visee=mv;
        }

        void determine_point_vise(double px,double py,double pz)
        {
            point_vise[0]=px;
            point_vise[1]=py;
            point_vise[2]=pz;
        }

        void determine_point_vise(double* pv)
        {
            point_vise[0]=pv[0];
            point_vise[1]=pv[1];
            point_vise[2]=pv[2];
        }

        void determine_conteneur_vise(dksConteneur* cont)
        {
            conteneur_vise=cont;
        }

        void active_visee_inertielle()
        {
            drapeau_visee_inertielle=true;
        }
        void desactive_visee_inertielle()
        {
            drapeau_visee_inertielle=false;
        }

        //Si ce drapeau est activé, le premier rafraichissement
        //de la visée suivant se fera sans inertie.
        void active_initialisation_visee_inertielle()
        {
            drapeau_initialise_visee_inertielle=true;
        }
        void desactive_initialisation_visee_inertielle()
        {
            drapeau_initialise_visee_inertielle=false;
        }

        void determine_inertie_visee(double p_inertie)
        {
            inertie_visee=p_inertie;
        }

        double renvoie_inertie_visee()
        {
            return inertie_visee;
        }

        //=========================================================
        //------- Détermine l'ouverture (en degrés) de l'oeil courant:
        //=========================================================
        void determine_ouverture(float p_ouverture)
        {
            ouverture=p_ouverture;
            maj_projection();
        }

        void determine_resolution(uint32_t largeur_ecran, uint32_t hauteur_ecran)
        {
            resolution_x=largeur_ecran;
            resolution_y=hauteur_ecran;
            rapport_xy=float(resolution_x)/float(resolution_y);
            maj_projection();
        }

        float renvoie_ouverture()
        {
            return ouverture;
        }
        float renvoie_rapport_xy()
        {
            return rapport_xy;
        }

        uint32_t renvoie_resolution_x()
        {
            return resolution_x;
        }

        uint32_t renvoie_resolution_y()
        {
            return resolution_y;
        }

        void determine_limites_profondeur(float p_zmin,float p_zmax)
        {
            zmin=p_zmin;
            zmax=p_zmax;
            maj_projection();
        }

        float renvoie_zmin() { return zmin; }

        float renvoie_zmax() { return zmax; }


        void maj_projection()
        {
            if (drapeau_actif)
            {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(ouverture,rapport_xy,zmin,zmax);
                glMatrixMode(GL_MODELVIEW);
            }
        }


};

#endif // DKSOEIL_H_INCLUDED
