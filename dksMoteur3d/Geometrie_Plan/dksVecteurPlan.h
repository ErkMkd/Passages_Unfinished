#ifndef DKSVECTEUR_H_INCLUDED
#define DKSVECTEUR_H_INCLUDED

#include "math.h"

class dksVecteurPlan
{
    public:
        double x;
        double y;

		dksVecteurPlan(double px=0., double py=0.)
		{
			x = px;
			y = py;
		}

		double norme() { return sqrt(x * x + y * y); }

		void unitaire()
		{
			double n = norme();
			x /= n;
			y /= n;
		}

		static double produit_scalaire( Vecteur_Plan* v1, Vecteur_Plan* v2 )
		{
			return v1->x * v2->x + v1->y * v2->y;
		}

		bool collineaire(Vecteur_Plan* vect)
		{
			if (fabs(x*vect->y - y*vect->x)<PRECISION) { return true; }
			else { return false; }
		}

		double distance(Vecteur_Plan* vect)
		{
				return (sqrt((vect->x-x)*(vect.x-x)+(vect.y-y)*(vect.y-y)));
		}

		/*
		 * Détermine si le point M appartient au même demi plan (défini par la droite AB) que le point C
		 */
		static bool meme_demi_plan(double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Mx, double My)
		{
			return ( (((Mx - Ax) * (Ay - By) + (My - Ay) * (Bx - Ax)) * ((Cx - Ax) * (Ay - By) + (Cy - Ay) * (Bx - Ax))) > EPSILON )
		}

		/*
		On compte le nombre d'intersection avec la demi droite positive des X
		Test dans le plan
		*/
		static bool inclus_dans_polygone(uint32_t* tabAretes, double* tabSommets,double x, double y, uint32_t nbr_sommets=-1)
		{
			if (nbr_sommets == -1) nbr_sommets = sizeof(tabAretes);
			uint32_t nbrInter = 0;

			double xpA = tabSommets[tabAretes[0]*2] - x;
			double ypA = tabSommets[tabAretes[0]*2+1] - y;

			if (xpA == 0 && ypA == 0) {return true;} //Si le point est sur un sommet, inclusion positive.
			else
			{
				if (xpA == 0) { xpA = 1e-4; }
				if (ypA == 0) { ypA = 1e-4; }
			}


			for (uint32_t i = 1; i < nbr_sommets;i++)
			{
				//Origine du polygone sur le point testé
				double xpB = tabSommets[tabAretes[i]*2] - x;
				double ypB = tabSommets[tabAretes[i]*2+1] - y;
				//Si Y des deux sommets de même signes, pas d'intersection:
				if (xpB == 0 && ypB == 0)
				{
					xpB = 1e-4;
					ypB = 1e-4;
				}
				else
				{
					if (xpB == 0) { xpB = 1e-4; }
					if (ypB == 0) { ypB = 1e-4; }
				}


				if (((ypA < 0) && (ypB > 0)) || ((ypA > 0) && (ypB < 0)))
				{
					if ((xpA > 0) && (xpB > 0)) { nbrInter++; }
					else if (xpA - ypA / (ypB - ypA) * (xpB - xpA) > 0) { nbrInter++;}
				}

				xpA = xpB;
				ypA = ypB;
			}

			if (nbrInter & 0x1) { return true; }
			else return false;

		}

		/*
		On compte le nombre d'intersection avec la demi droite positive des X
		Le polygone en entrée est défini par ses points qui se succèdent respectivement au tracé de son contour.
		*/
		static bool inclus_dans_polygone_sans_indexes(double* tabSommets,uint32_t nbrSommets, double x, double y)
		{

			uint32_t nbrInter = 0;

			double xpA = tabSommets[0] - x;
			double ypA = tabSommets[1] - y;

			if (xpA == 0 && ypA == 0) {return true;} //Si le point est sur un sommet, inclusion positive.
			else
			{
				if (xpA == 0) { xpA = 1e-4; }
				if (ypA == 0) { ypA = 1e-4; }
			}


			for (uint32_t i = 1; i < nbrSommets;i++)
			{
				//Origine du polygone sur le point testé
				double xpB = tabSommets[2*i] - x;
				double ypB = tabSommets[2 * i + 1] - y;
				//Si Y des deux sommets de même signes, pas d'intersection:
				if (xpB == 0 && ypB == 0)
				{
					xpB = 1e-4;
					ypB = 1e-4;
				}
				else
				{
					if (xpB == 0) { xpB = 1e-4; }
					if (ypB == 0) { ypB = 1e-4; }
				}


				if (((ypA < 0) && (ypB > 0)) || ((ypA > 0) && (ypB < 0)))
				{
					if ((xpA > 0) && (xpB > 0)) { nbrInter++; }
					else if (xpA - ypA / (ypB - ypA) * (xpB - xpA) > 0) { nbrInter++;}
				}

				xpA = xpB;
				ypA = ypB;
			}

			if (nbrInter & 0x1) { return true; }
			else return false;

		}


		static double angle (Vecteur_Plan* v1 ,Vecteur_Plan* v2)
		{
			double norme1 = v1->norme();
			double norme2 = v2->norme();

			if (norme1 > -EPSILON && norme1 < EPSILON) return 0.;
			if (norme2 > -EPSILON && norme2 < EPSILON) return 0.;

			double n1n2 = norme1 * norme2;

			//var v1u:Vecteur_Plan = new Vecteur_Plan(v1.x, v1.y);
			//var v2u:Vecteur_Plan = new Vecteur_Plan(v2.x, v2.y);
			//
			//v1u.unitaire();
			//v2u.unitaire();


			double cos_x = produit_scalaire(v1, v2)/n1n2;
			double sin_x = (v1->x * v2->y - v1->y * v2->x)/n1n2;

			//trace(cos_x, sin_x);

			return angle_360(cos_x, sin_x);
		}

		static double angle_360(double cos_x, double sin_x)
		{
			if (sin_x >= 0)
			{ return acos(cos_x); }
			else
			{ return -acos(cos_x) + 2 * M_PI; }
		}

		void toString()
		{
			std::cout<< "[Vecteur_Plan: x= "<<x<<" , y= "<<y<< " ]" <<std::endl;
		}
	}

};
#endif
