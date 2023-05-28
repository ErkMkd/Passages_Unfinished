#ifndef REPERE_H_INCLUDED
#define REPERE_H_INCLUDED

#include "dksVecteur.h"
#include "dksVecteurUnitaire.h"
#include "vec3d.h"
#include "dksMatrice4.h"

//===================================================================================
//                  La classe chargée de gérer les repères tridimentionnels
//                  Le repère peut être transformé directecement sur ses axes, ou via
//                  sa matrice de transformation.
//
///     ATTENTION !!! A propos des angles Roulis-Tangage-Lacet:
//          Ces angles sont absolus par rapport au repère [(1,0,0)(0,1,0)(0,0,1)]
//          Tout appel à la fonction determine_RTL() anule les effets des éventuelles
//          précédentes rotations selon un axe quelconque.
//===================================================================================

class dksRepere : public dksVecteur
{
	public:
		dksVecteurUnitaire* axe_x;
		dksVecteurUnitaire* axe_y;
		dksVecteurUnitaire* axe_z;
    protected:
        dksVecteurUnitaire* axe_horizontal; //(1.,0.,0.);
        dksVecteurUnitaire* axe_vertical; //(0.,1.,0.);
        dksVecteurUnitaire* axe_profondeur; //(0.,0.,1.);

        float angle_tangage;
        float angle_lacet;
        float angle_roulis;

    public:

        dksVecteur* test_tableau_objets;

		double echelle_x; //Facteurs multiplicateurs des axes du repère
		double echelle_y;
		double echelle_z;

		//Pour alléger les calculs:
		//Ce sont les axes mis à l'échelle du repère
        double ax_x ;
        double ax_y ;
        double ax_z ;

        double ay_x ;
        double ay_y ;
        double ay_z ;

        double az_x ;
        double az_y ;
        double az_z ;

		//============================ Fonctions de translations de repères:

		//Calcul l'orientation d'un axe relativement à un repère:
		static void axe_relatif(dksVecteurUnitaire* axeAbs,dksRepere* parent_rep_abs,dksVecteurUnitaire* axeRel)
		{
			//Projection de l'axe:
			double xTemp = axeAbs->x ;
			double yTemp = axeAbs->y ;
			double zTemp = axeAbs->z ;

			//Projection de la nouvelle position sur les axes du nouveau repère par produits scalaires:
			axeRel->x = parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_x->y * yTemp + parent_rep_abs->axe_x->z * zTemp ;
			axeRel->y = parent_rep_abs->axe_y->x * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_y->z * zTemp ;
			axeRel->z = parent_rep_abs->axe_z->x * xTemp + parent_rep_abs->axe_z->y * yTemp + parent_rep_abs->axe_z->z * zTemp ;
		}

		//Calcul l'orientation d'une table d'axes relativement à un repère:
		//Les tables source et destination peuvent être la même table.
		static void axes_relatifs(double* axesAbs,dksRepere* parent_rep_abs,double* axesRel,uint32_t nbrAxes)
		{
		    for(uint32_t i=0;i<nbrAxes;i++)
			{
			    //Projection de l'axe:
                double xTemp = axesAbs[3*i];
                double yTemp = axesAbs[3*i+1] ;
                double zTemp = axesAbs[3*i+2] ;

                //Projection de la nouvelle position sur les axes du nouveau repère par produits scalaires:
                axesRel[3*i] = parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_x->y * yTemp + parent_rep_abs->axe_x->z * zTemp ;
                axesRel[3*i+1] = parent_rep_abs->axe_y->x * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_y->z * zTemp ;
                axesRel[3*i+2] = parent_rep_abs->axe_z->x * xTemp + parent_rep_abs->axe_z->y * yTemp + parent_rep_abs->axe_z->z * zTemp ;
			}

		}

		//Calcul les coordonnées d'un point relatives à un repère:
		//pointAbs et pointRel peuvent pointer sur le même objet Vecteur.
		static void point_relatif(dksVecteur* pointAbs, dksRepere* parent_rep_abs, dksVecteur* pointRel)
		{
			double xTemp = (pointAbs->x - parent_rep_abs->x) ;
			double yTemp = (pointAbs->y - parent_rep_abs->y) ;
			double zTemp = (pointAbs->z - parent_rep_abs->z) ;

			pointRel->x = (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_x->y * yTemp + parent_rep_abs->axe_x->z * zTemp)/parent_rep_abs->echelle_x;
			pointRel->y = (parent_rep_abs->axe_y->x * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_y->z * zTemp)/parent_rep_abs->echelle_y;
			pointRel->z = (parent_rep_abs->axe_z->x * xTemp + parent_rep_abs->axe_z->y * yTemp + parent_rep_abs->axe_z->z * zTemp)/parent_rep_abs->echelle_z;
		}

		static void point_relatif(dksVecteur* pointAbs, dksRepere* parent_rep_abs, double* point_rel)
		{
			double xTemp = (pointAbs->x - parent_rep_abs->x) ;
			double yTemp = (pointAbs->y - parent_rep_abs->y) ;
			double zTemp = (pointAbs->z - parent_rep_abs->z) ;

			point_rel[0] = (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_x->y * yTemp + parent_rep_abs->axe_x->z * zTemp)/parent_rep_abs->echelle_x;
			point_rel[1] = (parent_rep_abs->axe_y->x * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_y->z * zTemp)/parent_rep_abs->echelle_y;
			point_rel[2] = (parent_rep_abs->axe_z->x * xTemp + parent_rep_abs->axe_z->y * yTemp + parent_rep_abs->axe_z->z * zTemp)/parent_rep_abs->echelle_z;
		}

		//Détermine les coordonnées d'une table de points, par rapport à un repère.
		//C'est,en gros, l'inverse d'une translation d'un point par un repère.
		static void points_relatifs(double* points, dksRepere* parent_rep_abs, double* points_rel,uint32_t nbrPoints)
		{
		    for(uint32_t i=0;i<nbrPoints;i++)
		    {
		        double xTemp = (points[3*i] - parent_rep_abs->x) ;
                double yTemp = (points[3*i+1] - parent_rep_abs->y) ;
                double zTemp = (points[3*i+2] - parent_rep_abs->z) ;

                points_rel[3*i] = (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_x->y * yTemp + parent_rep_abs->axe_x->z * zTemp)/parent_rep_abs->echelle_x;
                points_rel[3*i+1] = (parent_rep_abs->axe_y->x * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_y->z * zTemp)/parent_rep_abs->echelle_y;
                points_rel[3*i+2] = (parent_rep_abs->axe_z->x * xTemp + parent_rep_abs->axe_z->y * yTemp + parent_rep_abs->axe_z->z * zTemp)/parent_rep_abs->echelle_z;
		    }

		}

		//Translate un point par un repère:
		static void translation_point(dksVecteur* pointRel, dksRepere* parent_rep_abs, dksVecteur* pointAbs)
		{
			double xTemp = pointRel->x * parent_rep_abs->echelle_x;
			double yTemp = pointRel->y * parent_rep_abs->echelle_y;
			double zTemp = pointRel->z * parent_rep_abs->echelle_z;

			pointAbs->x = parent_rep_abs->x + (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp);
			pointAbs->y = parent_rep_abs->y + (parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp);
			pointAbs->z = parent_rep_abs->z + (parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp);
		}

		//Translate un point par un repère:
		static void translation_point(vec3d* pointRel, dksRepere* parent_rep_abs, vec3d* pointAbs)
		{
			double xTemp = pointRel->x * parent_rep_abs->echelle_x;
			double yTemp = pointRel->y * parent_rep_abs->echelle_y;
			double zTemp = pointRel->z * parent_rep_abs->echelle_z;

			pointAbs->x = parent_rep_abs->x + (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp);
			pointAbs->y = parent_rep_abs->y + (parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp);
			pointAbs->z = parent_rep_abs->z + (parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp);
		}

        //Translate un point par un repère:
		static void translation_point(double* pointRel, dksRepere* parent_rep_abs, double* pointAbs)
		{
			double xTemp = pointRel[0] * parent_rep_abs->echelle_x;
			double yTemp = pointRel[1] * parent_rep_abs->echelle_y;
			double zTemp = pointRel[2] * parent_rep_abs->echelle_z;

			pointAbs[0] = parent_rep_abs->x + (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp);
			pointAbs[1] = parent_rep_abs->y + (parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp);
			pointAbs[2] = parent_rep_abs->z + (parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp);
		}

		//Translate l'orientation d'un axe par un repère:
		static void translation_axe(vec3d* axeRel, dksRepere* parent_rep_abs, vec3d* axeAbs)
		{
			double xTemp = axeRel->x * parent_rep_abs->echelle_x;
			double yTemp = axeRel->y * parent_rep_abs->echelle_y;
			double zTemp = axeRel->z * parent_rep_abs->echelle_z;

			axeAbs->x = parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp;
			axeAbs->y = parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp;
			axeAbs->z = parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp;
		}

		//Translate l'orientation d'un axe par un repère:
		static void translation_axe(double* axeRel, dksRepere* parent_rep_abs, double* axeAbs)
		{
			double xTemp = axeRel[0] * parent_rep_abs->echelle_x;
			double yTemp = axeRel[1] * parent_rep_abs->echelle_y;
			double zTemp = axeRel[2] * parent_rep_abs->echelle_z;

			axeAbs[0] = parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp;
			axeAbs[1] = parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp;
			axeAbs[2] = parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp;
		}

		//Translate l'orientation d'une table d'axes par un repère:
		//Les tables source et destination peuvent être la même table.
		//La différence avec les points, c'est que la position de l'origine du repère n'est pas prise en compte.
		//Sert pour la translation des normales,tangentes ou binormales.
		static void translation_axes(DOUBLE* axesRel, dksRepere* parent_rep_abs, double* axesAbs,uint32_t nbrAxes)
		{
		    for(uint32_t i=0;i<nbrAxes;i++)
		    {
		        double xTemp = axesRel[3*i] * parent_rep_abs->echelle_x;
                double yTemp = axesRel[3*i+1] * parent_rep_abs->echelle_y;
                double zTemp = axesRel[3*i+2] * parent_rep_abs->echelle_z;

                axesAbs[3*i] = parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp;
                axesAbs[3*i+1] = parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp;
                axesAbs[3*i+2] = parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp;
		    }
		}

		//Translate une table de points par un repère:
		//La table de résultat peut être la même que la table de points relatifs.
		static void translation_points(double* pointsRel, dksRepere* parent_rep_abs, double* pointsAbs, uint32_t nbrSommets)
		{
			for (uint32_t i = 0; i < nbrSommets; i++)
			{
				double xTemp = pointsRel[3*i] * parent_rep_abs->echelle_x;
				double yTemp = pointsRel[3*i+1] * parent_rep_abs->echelle_y;
				double zTemp = pointsRel[3*i+2] * parent_rep_abs->echelle_z;

				pointsAbs[3*i] = parent_rep_abs->x + (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp);
				pointsAbs[3*i+1] = parent_rep_abs->y + (parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp);
				pointsAbs[3 * i + 2] = parent_rep_abs->z + (parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp);
			}
		}

		static void translation_points(vector<double>* pointsRel, dksRepere* parent_rep_abs, vector<double>* pointsAbs, uint32_t nbrSommets)
		{
			for (uint32_t i = 0; i < nbrSommets; i++)
			{
				double xTemp = (*pointsRel)[3*i] * parent_rep_abs->echelle_x;
				double yTemp = (*pointsRel)[3*i+1] * parent_rep_abs->echelle_y;
				double zTemp = (*pointsRel)[3*i+2] * parent_rep_abs->echelle_z;

				(*pointsAbs)[3*i] = parent_rep_abs->x + (parent_rep_abs->axe_x->x * xTemp + parent_rep_abs->axe_y->x * yTemp + parent_rep_abs->axe_z->x * zTemp);
				(*pointsAbs)[3*i+1] = parent_rep_abs->y + (parent_rep_abs->axe_x->y * xTemp + parent_rep_abs->axe_y->y * yTemp + parent_rep_abs->axe_z->y * zTemp);
				(*pointsAbs)[3 * i + 2] = parent_rep_abs->z + (parent_rep_abs->axe_x->z * xTemp + parent_rep_abs->axe_y->z * yTemp + parent_rep_abs->axe_z->z * zTemp);
			}
		}

		//Translate un repere par un autre repère:
		//Si le repère destination est NULL, le résultat est stocké dans le repère enfant
		static void translation_repere(dksRepere* parent,dksRepere* enfant,dksRepere* dest=NULL)
		{
			dksVecteur* fiston_axe_x=enfant->axe_x;
			dksVecteur* fiston_axe_y=enfant->axe_y;
			dksVecteur* fiston_axe_z=enfant->axe_z;

			double fiston_abs_x = enfant->x;
			double fiston_abs_y = enfant->y;
			double fiston_abs_z = enfant->z;

			double rep_abs_axe_x_x = parent->axe_x->x;
			double rep_abs_axe_x_y = parent->axe_x->y;
			double rep_abs_axe_x_z = parent->axe_x->z;

			double rep_abs_axe_y_x = parent->axe_y->x;
			double rep_abs_axe_y_y = parent->axe_y->y;
			double rep_abs_axe_y_z = parent->axe_y->z;

			double rep_abs_axe_z_x = parent->axe_z->x;
			double rep_abs_axe_z_y = parent->axe_z->y;
			double rep_abs_axe_z_z = parent->axe_z->z;

			double rep_abs_x = parent->x;
			double rep_abs_y = parent->y;
			double rep_abs_z = parent->z;

			double intermediaire_x;
			double intermediaire_y;

			if(dest==NULL) dest=enfant;
			else
			{
			    dest->echelle_x=enfant->echelle_x;
			    dest->echelle_y=enfant->echelle_y;
			    dest->echelle_z=enfant->echelle_z;
			}

			dest->x = rep_abs_x + fiston_abs_x * rep_abs_axe_x_x +  fiston_abs_y * rep_abs_axe_y_x + fiston_abs_z * rep_abs_axe_z_x;
			dest->y = rep_abs_y + fiston_abs_x * rep_abs_axe_x_y +  fiston_abs_y * rep_abs_axe_y_y + fiston_abs_z * rep_abs_axe_z_y;
			dest->z = rep_abs_z + fiston_abs_x * rep_abs_axe_x_z +  fiston_abs_y * rep_abs_axe_y_z + fiston_abs_z * rep_abs_axe_z_z;

			intermediaire_x = fiston_axe_x->x * rep_abs_axe_x_x + fiston_axe_x->y * rep_abs_axe_y_x + fiston_axe_x->z * rep_abs_axe_z_x;
			intermediaire_y = fiston_axe_x->x * rep_abs_axe_x_y + fiston_axe_x->y * rep_abs_axe_y_y + fiston_axe_x->z * rep_abs_axe_z_y;
			dest->axe_x->z = fiston_axe_x->x * rep_abs_axe_x_z + fiston_axe_x->y * rep_abs_axe_y_z + fiston_axe_x->z * rep_abs_axe_z_z;
			dest->axe_x->x = intermediaire_x;
			dest->axe_x->y = intermediaire_y;

			intermediaire_x = fiston_axe_y->x * rep_abs_axe_x_x + fiston_axe_y->y * rep_abs_axe_y_x + fiston_axe_y->z * rep_abs_axe_z_x;
			intermediaire_y = fiston_axe_y->x * rep_abs_axe_x_y + fiston_axe_y->y * rep_abs_axe_y_y + fiston_axe_y->z * rep_abs_axe_z_y;
			dest->axe_y->z = fiston_axe_y->x * rep_abs_axe_x_z + fiston_axe_y->y * rep_abs_axe_y_z + fiston_axe_y->z * rep_abs_axe_z_z;
			dest->axe_y->x = intermediaire_x;
			dest->axe_y->y = intermediaire_y;

			intermediaire_x = fiston_axe_z->x * rep_abs_axe_x_x + fiston_axe_z->y * rep_abs_axe_y_x + fiston_axe_z->z * rep_abs_axe_z_x;
			intermediaire_y = fiston_axe_z->x * rep_abs_axe_x_y + fiston_axe_z->y * rep_abs_axe_y_y + fiston_axe_z->z * rep_abs_axe_z_y;
			dest->axe_z->z = fiston_axe_z->x * rep_abs_axe_x_z + fiston_axe_z->y * rep_abs_axe_y_z + fiston_axe_z->z * rep_abs_axe_z_z;
			dest->axe_z->x = intermediaire_x;
			dest->axe_z->y = intermediaire_y;
			dest->maj_axes_echelles();
		}


		//Calcul les coordonnees relatives d'un repère par rapport à un autre repère:
		static void determine_repere_relatif(dksRepere* repAbs, dksRepere* repParent, dksRepere* repRel )
		{
			double xTemp = repAbs->x - repParent->x;
			double yTemp = repAbs->y - repParent->y;
			double zTemp = repAbs->z - repParent->z;

			//Projection de la nouvelle position sur les axes du nouveau repère par produits scalaires:
			repRel->x = repParent->axe_x->x * xTemp + repParent->axe_x->y * yTemp + repParent->axe_x->z * zTemp;
			repRel->y = repParent->axe_y->x * xTemp + repParent->axe_y->y * yTemp + repParent->axe_y->z * zTemp;
			repRel->z = repParent->axe_z->x * xTemp + repParent->axe_z->y * yTemp + repParent->axe_z->z * zTemp;

			axe_relatif(repAbs->axe_x, repParent, repRel->axe_x);
			axe_relatif(repAbs->axe_y, repParent, repRel->axe_y);
			axe_relatif(repAbs->axe_z, repParent, repRel->axe_z);

			repRel->x /= repParent->echelle_x;
			repRel->y /= repParent->echelle_y;
			repRel->z /= repParent->echelle_z;
		}

		//=============================== Constructeur:

        dksRepere(double px=0, double py=0, double pz=0, dksVecteurUnitaire* my_axe_x=NULL, dksVecteurUnitaire* my_axe_y=NULL, dksVecteurUnitaire* my_axe_z=NULL)
        :dksVecteur(px,py,pz)
		{
		    axe_x=NULL;
		    axe_y=NULL;
		    axe_z=NULL;

		    axe_x=new dksVecteurUnitaire(1.,0.,0.);
		    axe_y=new dksVecteurUnitaire(0.,1.,0.);
		    axe_z=new dksVecteurUnitaire(0.,0.,1.);

		    axe_horizontal=new dksVecteurUnitaire(1.,0.,0.);
		    axe_vertical=new dksVecteurUnitaire(0.,1.,0.);
		    axe_profondeur=new dksVecteurUnitaire(0.,0.,1.);

            initialisation_repere_orthonorme(my_axe_x,my_axe_y,my_axe_z);
            echelle_x=1.;
            echelle_y=1.;
            echelle_z=1.;

            angle_roulis=0.;
            angle_tangage=0.;
            angle_lacet=0.;

		}

        virtual ~dksRepere()
        {
            if (axe_x!=NULL) delete(axe_x);
            if (axe_y!=NULL) delete(axe_y);
            if (axe_z!=NULL) delete(axe_z);
            if (axe_horizontal!=NULL) delete(axe_horizontal);
            if (axe_vertical!=NULL) delete(axe_vertical);
            if (axe_profondeur!=NULL) delete(axe_profondeur);
        }

        void raz()
        {
            x=0;y=0;z=0;
            axe_x->x = 1.; axe_x->y = 0.; axe_x->z = 0.;
			axe_y->x = 0.; axe_y->y = 1.; axe_y->z = 0.;
			axe_z->x = 0.; axe_z->y = 0.; axe_z->z = 1.;
        }

        void raz_axes()
        {
            axe_x->x = 1.; axe_x->y = 0.; axe_x->z = 0.;
			axe_y->x = 0.; axe_y->y = 1.; axe_y->z = 0.;
			axe_z->x = 0.; axe_z->y = 0.; axe_z->z = 1.;
        }

        //Initialisation du repère à tendance orthonormée.
		// Il reste des cas où le repère peut être aberrant. (ex: axe_x=null, axe_y=null axe_z=1,0,0 => génère un axe_x=axe_z...)
		void initialisation_repere_orthonorme(dksVecteurUnitaire* my_axe_x=NULL,dksVecteurUnitaire* my_axe_y=NULL, dksVecteurUnitaire* my_axe_z=NULL)
		{
		// le repère est toujours direct (ou presque... cf dernière ligne...)
			if ( (my_axe_x == NULL) && (my_axe_y == NULL) && (my_axe_z == NULL))
			{
			    return;
            }
			else if ( ( my_axe_x == NULL) &&  (my_axe_y == NULL) )
			{
			    axe_z->x=my_axe_z->x;
			    axe_z->y=my_axe_z->y;
			    axe_z->z=my_axe_z->z;
			    axe_x->produit_vectoriel_unitaire(axe_vertical, axe_z);
			    if (axe_x->norme()<0.9)
			    {
			        axe_x->x=1.;axe_x->y=0.;axe_x->z=0.;
			        axe_y->x=0.;axe_x->y=0.;axe_x->z=-1.;
			    }
			    else axe_y->produit_vectoriel_unitaire(axe_z,axe_x);
            }

			else if ( ( my_axe_y == NULL) &&  (my_axe_z == NULL) )
			{
			    axe_x->x=my_axe_x->x;
			    axe_x->y=my_axe_x->y;
			    axe_x->z=my_axe_x->z;
			    axe_y->produit_vectoriel_unitaire(axe_profondeur, axe_x);
			    if (axe_y->norme()<0.9)
			    {
			        axe_y->x=-1.;axe_y->y=0.;axe_y->z=0.;
			        axe_z->x=0.;axe_z->y=0.;axe_x->z=1.;
			    }
			    else axe_z->produit_vectoriel_unitaire(axe_x,axe_y);
            }
			else if ( ( my_axe_x == NULL) &&  (my_axe_z == NULL) )
			{
			    axe_y->x=my_axe_y->x;
			    axe_y->y=my_axe_y->y;
			    axe_y->z=my_axe_y->z;
			    axe_z->produit_vectoriel_unitaire(axe_horizontal, axe_y);
			    if (axe_z->norme()<0.9)
			    {
			        axe_z->x=0.;axe_z->y=0.;axe_z->z=1.;
			        axe_x->x=0.;axe_x->y=-1.;axe_x->z=0.;
			    }
			    else axe_x->produit_vectoriel_unitaire(axe_y,axe_z);
            }

			else if ( my_axe_x == NULL)
			{
			    axe_y->x=my_axe_y->x;
			    axe_y->y=my_axe_y->y;
			    axe_y->z=my_axe_y->z;

			    axe_z->x=my_axe_z->x;
			    axe_z->y=my_axe_z->y;
			    axe_z->z=my_axe_z->z;

			    axe_x->produit_vectoriel_unitaire(axe_y, axe_z);
            }
			else if ( my_axe_y == NULL)
			{
			    axe_x->x=my_axe_x->x;
			    axe_x->y=my_axe_x->y;
			    axe_x->z=my_axe_x->z;

			    axe_z->x=my_axe_z->x;
			    axe_z->y=my_axe_z->y;
			    axe_z->z=my_axe_z->z;

			    axe_y->produit_vectoriel_unitaire(axe_z, axe_x);
            }
			else if ( my_axe_z == NULL)
			{
			    axe_x->x=my_axe_x->x;
			    axe_x->y=my_axe_x->y;
			    axe_x->z=my_axe_x->z;

			    axe_y->x=my_axe_y->x;
			    axe_y->y=my_axe_y->y;
			    axe_y->z=my_axe_y->z;

			    axe_z->produit_vectoriel_unitaire(axe_x, axe_y);
            }
			else
			{
			    axe_x->x=my_axe_x->x;
			    axe_x->y=my_axe_x->y;
			    axe_x->z=my_axe_x->z;

			    axe_y->x=my_axe_y->x;
			    axe_y->y=my_axe_y->y;
			    axe_y->z=my_axe_y->z;

			    axe_z->x=my_axe_z->x;
			    axe_z->y=my_axe_z->y;
			    axe_z->z=my_axe_z->z;
            }
		}

		void initialisation_repere_orthonorme(double* my_axe_x=NULL,double* my_axe_y=NULL, double* my_axe_z=NULL)
		{
		// le repère est toujours direct (ou presque... cf dernière ligne...)
            double n;

			if ( (my_axe_x == NULL) && (my_axe_y == NULL) && (my_axe_z == NULL))
			{
			    return;
            }
			else if ( ( my_axe_x == NULL) &&  (my_axe_y == NULL) )
			{
			    axe_z->x=my_axe_z[0];
			    axe_z->y=my_axe_z[1];
			    axe_z->z=my_axe_z[2];

			    axe_x->produit_vectoriel_unitaire(axe_vertical, axe_z);

			    if (axe_x->norme()<0.9)
			    {
			        axe_x->x=1.;axe_x->y=0.;axe_x->z=0.;
			        axe_y->x=0.;axe_x->y=0.;axe_x->z=-1.;
			    }
			    else axe_y->produit_vectoriel_unitaire(axe_z,axe_x);
            }

			else if ( ( my_axe_y == NULL) &&  (my_axe_z == NULL) )
			{
			    axe_x->x=my_axe_x[0];
			    axe_x->y=my_axe_x[1];
			    axe_x->z=my_axe_x[2];

			    axe_y->produit_vectoriel_unitaire(axe_profondeur, axe_x);

			    if (axe_y->norme()<0.9)
			    {
			        axe_y->x=-1.;axe_y->y=0.;axe_y->z=0.;
			        axe_z->x=0.;axe_z->y=0.;axe_x->z=1.;
			    }
			    else axe_z->produit_vectoriel_unitaire(axe_x,axe_y);
            }
			else if ( ( my_axe_x == NULL) &&  (my_axe_z == NULL) )
			{
			    axe_y->x=my_axe_y[0];
			    axe_y->y=my_axe_y[1];
			    axe_y->z=my_axe_y[2];

			    axe_z->produit_vectoriel_unitaire(axe_horizontal, axe_y);

			    if (axe_z->norme()<0.9)
			    {
			        axe_z->x=0.;axe_z->y=0.;axe_z->z=1.;
			        axe_x->x=0.;axe_x->y=-1.;axe_x->z=0.;
			    }
			    else axe_x->produit_vectoriel_unitaire(axe_y,axe_z);
            }

			else if ( my_axe_x == NULL)
			{
			    axe_y->x=my_axe_y[0];
			    axe_y->y=my_axe_y[1];
			    axe_y->z=my_axe_y[2];

			    axe_z->x=my_axe_z[0];
			    axe_z->y=my_axe_z[1];
			    axe_z->z=my_axe_z[2];

			    axe_x->produit_vectoriel_unitaire(axe_y, axe_z);
            }
			else if ( my_axe_y == NULL)
			{
			    axe_x->x=my_axe_x[0];
			    axe_x->y=my_axe_x[1];
			    axe_x->z=my_axe_x[2];

			    axe_z->x=my_axe_z[0];
			    axe_z->y=my_axe_z[1];
			    axe_z->z=my_axe_z[2];

			    axe_y->produit_vectoriel_unitaire(axe_z, axe_x);
            }
			else if ( my_axe_z == NULL)
			{
			    axe_x->x=my_axe_x[0];
			    axe_x->y=my_axe_x[1];
			    axe_x->z=my_axe_x[2];

			    axe_y->x=my_axe_y[0];
			    axe_y->y=my_axe_y[1];
			    axe_y->z=my_axe_y[2];

			    axe_z->produit_vectoriel_unitaire(axe_x, axe_y);
            }
			else
			{
			    axe_x->x=my_axe_x[0];
			    axe_x->y=my_axe_x[1];
			    axe_x->z=my_axe_x[2];

			    axe_y->x=my_axe_y[0];
			    axe_y->y=my_axe_y[1];
			    axe_y->z=my_axe_y[2];

			    axe_z->x=my_axe_z[0];
			    axe_z->y=my_axe_z[1];
			    axe_z->z=my_axe_z[2];
            }
		}

		//Rotation forcée, sans tenir compte du torseur:
		virtual void rotation(double angle, dksVecteurUnitaire* axe)
		{
			axe_x->rotation(angle, axe);
			axe_y->rotation(angle, axe);
			axe_z->rotation(angle, axe);
		}

		virtual void rotation(double angle, double rx, double ry, double rz)
		{
			axe_x->rotation(angle, rx,ry,rz);
			axe_y->rotation(angle, rx,ry,rz);
			axe_z->rotation(angle, rx,ry,rz);
		}

		virtual void rotationf(float angle_f, float* r)
		{
		    double angle=double(angle_f);
		    double rx=double(r[0]);
		    double ry=double(r[1]);
		    double rz=double(r[2]);
			axe_x->rotation(angle, rx,ry,rz);
			axe_y->rotation(angle, rx,ry,rz);
			axe_z->rotation(angle, rx,ry,rz);
		}

		//Translation d'une table de points par le repère:
		void translation_sommets(double* sommets, double* tabRes, int nbrSommets)
		{
			maj_axes_echelles();

			int nbr = nbrSommets * 3;
			int j = 0;
			for (int i = 0; i < nbr;)
			{
				double xt = sommets[i++];
				double yt = sommets[i++];
				double zt = sommets[i++];
				// coordonnées de l'objet enfant dans le repère de l'objet parent
				tabRes[j++] =  x + xt * ax_x + yt * ay_x + zt * az_x;
				tabRes[j++] =  y + xt * ax_y + yt * ay_y + zt * az_y;
				tabRes[j++] =  z + xt * ax_z + yt * ay_z + zt * az_z;
			}
		}


		virtual void determine_position(double px, double py, double pz)
		{
			x = px; y = py; z = pz;
		}

        //=================================================================================================================
        //                  Gestion de l'orientation du repère en fonction des angles de tangage, lacet et roulis
        //=================================================================================================================
        virtual void determine_RTL(double p_roulis, double p_tangage, double p_lacet)
		{
            angle_roulis=p_roulis;
            angle_tangage=p_tangage;
            angle_lacet=p_lacet;
            maj_orientation_RTL();
		}

		void maj_orientation_RTL()
		{
		    raz_axes();

			axe_x->rotation_RTL(angle_roulis,angle_tangage,angle_lacet);
			axe_y->rotation_RTL(angle_roulis,angle_tangage,angle_lacet);
			axe_z->rotation_RTL(angle_roulis,angle_tangage,angle_lacet);
		}

		float renvoie_angle_roulis() { return angle_roulis; }
		float renvoie_angle_tangage() { return angle_tangage; }
		float renvoie_angle_lacet() { return angle_lacet; }

        //============================================================================================
        //      Calcul les angles tangage,lacet et roulis en fonction de l'orientation du repère
        //============================================================================================

		double renvoie_tangage_relatif()
		{
			return asin(axe_z->y);
		}

		double renvoie_roulis_relatif()
		{
			double norme_vect_z = sqrt(axe_z->x * axe_z->x + axe_z->z * axe_z->z);
			double temp_roulis = -axe_z->x / norme_vect_z * axe_x->z + axe_z->z / norme_vect_z * axe_x->x;

			if (temp_roulis > 1.) temp_roulis = 1.;
			else if (temp_roulis < -1.) temp_roulis = -1.;

			double roulis = -acos(temp_roulis);
			if (axe_x->y < 0) roulis = -roulis;

			return roulis;
		}

		double renvoie_lacet_relatif()
		{
			double norme_vect_z = sqrt(axe_z->x * axe_z->x + axe_z->z * axe_z->z);

			double lacet_temp = axe_z->z / norme_vect_z;

			if (lacet_temp > 1.) lacet_temp = 1.;
			else if (lacet_temp < -1.) lacet_temp = -1.;

			double lacet = acos(lacet_temp);
			if (axe_x->z > 0.) lacet = -lacet;

			return lacet;
		}

        //Transforme le repère par une matrice 4*4:
		void transforme_matrice(dksMatrice4* matrice,dksRepere* rep_dest)
        {
            matrice->applique_vecteur(this,rep_dest);
            matrice->applique_axe(axe_x,rep_dest->axe_x);
            matrice->applique_axe(axe_y,rep_dest->axe_y);
            matrice->applique_axe(axe_z,rep_dest->axe_z);
        }

		virtual void copie_repere(dksRepere* repere_source)
		{
			x = repere_source->x;
			y = repere_source->y;
			z = repere_source->z;

			axe_x->x = repere_source->axe_x->x;
			axe_x->y = repere_source->axe_x->y;
			axe_x->z = repere_source->axe_x->z;

			axe_y->x = repere_source->axe_y->x;
			axe_y->y = repere_source->axe_y->y;
			axe_y->z = repere_source->axe_y->z;

			axe_z->x = repere_source->axe_z->x;
			axe_z->y = repere_source->axe_z->y;
			axe_z->z = repere_source->axe_z->z;

			echelle_x=repere_source->echelle_x;
			echelle_y=repere_source->echelle_y;
			echelle_z=repere_source->echelle_z;
		}

		//Ne copie que les axes.
		virtual void copie_axes(dksRepere* repere_source)
		{
			axe_x->x = repere_source->axe_x->x;
			axe_x->y = repere_source->axe_x->y;
			axe_x->z = repere_source->axe_x->z;

			axe_y->x = repere_source->axe_y->x;
			axe_y->y = repere_source->axe_y->y;
			axe_y->z = repere_source->axe_y->z;

			axe_z->x = repere_source->axe_z->x;
			axe_z->y = repere_source->axe_z->y;
			axe_z->z = repere_source->axe_z->z;
		}

		void determine_axes(double* coordonnees)
		{
		    axe_x->x=coordonnees[0];
		    axe_x->y=coordonnees[1];
		    axe_x->z=coordonnees[2];
		    axe_y->x=coordonnees[3];
		    axe_y->y=coordonnees[4];
		    axe_y->z=coordonnees[5];
		    axe_z->x=coordonnees[6];
		    axe_z->y=coordonnees[7];
		    axe_z->z=coordonnees[8];
		}

		void maj_axes_echelles()
		{
		    ax_x = axe_x->x * echelle_x;
            ax_y = axe_x->y * echelle_x;
            ax_z = axe_x->z * echelle_x;

            ay_x = axe_y->x * echelle_y;
            ay_y = axe_y->y * echelle_y;
            ay_z = axe_y->z * echelle_y;

            az_x = axe_z->x * echelle_z;
            az_y = axe_z->y * echelle_z;
            az_z = axe_z->z * echelle_z;
		}

		virtual void toString()
		{
		    std::cout<<"Repere: position: "<<x<<","<<y<<","<<z<<std::endl;
		    std::cout<<"axe_x:";axe_x->toString();
		    std::cout<<"axe_y:";axe_y->toString();
		    std::cout<<"axe_z:";axe_z->toString();
		}

};

#endif // DKSREPERE_H_INCLUDED
