#ifndef DKSVECTEURUNITAIRE_H_INCLUDED
#define DKSVECTEURUNITAIRE_H_INCLUDED

#include "dksVecteur.h"

class dksVecteurUnitaire : public dksVecteur
{
    public:


        dksVecteurUnitaire( double px = 1., double py = 0., double pz = 0. )
            :dksVecteur(px,py,pz)
		{
			double norme = sqrt(x * x + y * y + z * z);
			x/=norme;
			y/=norme;
			z/=norme;
		}

		void normalise()
		{
			double n = norme();

            x /= n;
			y /= n;
			z /= n;
		}

		void produit_vectoriel_unitaire(dksVecteurUnitaire* v1, dksVecteurUnitaire* v2)
		{
		    x = v1->y * v2->z - v2->y * v1->z;
			y = v1->z * v2->x - v2->z * v1->x;
			z = v1->x * v2->y - v2->x * v1->y;

			normalise();
		}


		double theta()
        {
			double signe=1.;
			double n = norme();
			if (y< 0.) { signe = -1.; }
			return signe*acos(x/n);
		}

		double phi()
		{ return -M_PI_2 + acos(z); }


};


#endif // DKSVECTEURUNITAIRE_H_INCLUDED
