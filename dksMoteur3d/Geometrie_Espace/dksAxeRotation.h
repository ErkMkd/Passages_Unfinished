#ifndef DKSAXEROTATION_H_INCLUDED
#define DKSAXEROTATION_H_INCLUDED

#include <math.h>
#include "dksVecteurUnitaire.h"
#define DKSAXEROTATION_EPSILON 1e-6


class  dksAxeRotation :public dksVecteurUnitaire
{
    public:
		double angle;
		dksVecteur position;

		dksAxeRotation(double dir_x = 1.,double dir_y = 0., double dir_z = 0.,double a = 0.,
                    double px = 0., double py = 0., double pz = 0.):dksVecteurUnitaire(dir_x,dir_y,dir_z)
		{
			angle = a;
			position.x = px;
			position.y = py;
			position.z = pz;
		}


        //Calcul l'angle et l'axe de rotation entre deux vecteurs de même origine
        void calcul_rotation_entre_deux_vecteurs(double* axe1, double* axe2)
        {
            double a1x,a1y,a1z;
            double a2x,a2y,a2z;
            a1x=axe1[0];a1y=axe1[1];a1z=axe1[2];
            a2x=axe2[0];a2y=axe2[1];a2z=axe2[2];
            double n1=sqrt(a1x*a1x+a1y*a1y+a1z*a1z);
            double n2=sqrt(a2x*a2x+a2y*a2y+a2z*a2z);
            a1x/=n1;a1y/=n1;a1z/=n1;
            a2x/=n2;a2y/=n2;a2z/=n2;
            x=a1y*a2z-a1z*a2y;
            y=a1z*a2x-a1x*a2z;
            z=a1x*a2y-a1y*a2x;
            double n=sqrt(x*x+y*y+z*z);
            if(n<DKSAXEROTATION_EPSILON)
            {
                angle=0.;
                x=1.;y=0.;z=0.;
            }
            else
            {
                normalise();
                double cosA=a1x*a2x+a1y*a2y+a1z*a2z;
                angle=acos(cosA);
            }
        }
		//Calcul l'axe de rotation résultant entre deux repères.
		//Simple addition des produits vectoriels de deux des axes des repères.
		void calcul_rotation_entre_deux_reperes(dksRepere* rep1, dksRepere* rep2)
		{
		    //Calcul l'axe de rotation de l'axe X:
			dksVecteur* vect1 = rep1->axe_x;
			dksVecteur* vect2 = rep2->axe_x;

			double axe1_x = vect1->y * vect2->z - vect1->z * vect2->y;
			double axe1_y = vect1->z * vect2->x - vect1->x * vect2->z;
			double axe1_z = vect1->x * vect2->y - vect1->y * vect2->x;

            double cosX = vect1->x*vect2->x+vect1->y*vect2->y+vect1->z*vect2->z;
			double sinX = sqrt(axe1_x * axe1_x + axe1_y * axe1_y + axe1_z * axe1_z);

			//Calcul l'axe de rotation de l'axe Y:
			vect1 = rep1->axe_y;
			vect2 = rep2->axe_y;

			double axe2_x = vect1->y * vect2->z - vect1->z * vect2->y;
			double axe2_y = vect1->z * vect2->x - vect1->x * vect2->z;
			double axe2_z = vect1->x * vect2->y - vect1->y * vect2->x;

            double cosY = vect1->x*vect2->x+vect1->y*vect2->y+vect1->z*vect2->z;
			double sinY = sqrt(axe2_x * axe2_x + axe2_y * axe2_y + axe2_z * axe2_z);


			if (sinX<DKSAXEROTATION_EPSILON && sinY<DKSAXEROTATION_EPSILON)
			{
			    if (cosX>0. && cosY>0.) {angle=0.; return;}
			    if (cosX<0. && cosY>0.) {angle=M_PI; x=0.;y=1.;z=0.; return;}
			    if (cosX>0. && cosY<0.) {angle=M_PI; x=1.;y=0.;z=0.; return;}
			    if (cosX<0. && cosY<0.) {angle=M_PI; x=0.;y=0.;z=1.; return;}
			}

            //Calcul l'axe de rotation de l'axe Z
			vect1 = rep1->axe_z;
			vect2 = rep2->axe_z;

			double axe3_x = vect1->y * vect2->z - vect1->z * vect2->y;
			double axe3_y = vect1->z * vect2->x - vect1->x * vect2->z;
			double axe3_z = vect1->x * vect2->y - vect1->y * vect2->x;

			//double cosZ = vect1->x*vect2->x+vect1->y*vect2->y+vect1->z*vect2->z;
			double sinZ = sqrt(axe3_x * axe3_x + axe3_y * axe3_y + axe3_z * axe3_z);

            //Calcul l'axe rotation du repère:
			double dx = axe1_x + axe2_x + axe3_x;
			double dy = axe1_y + axe2_y + axe3_y;
			double dz = axe1_z + axe2_z + axe3_z;

			double n = sqrt(dx * dx + dy * dy + dz * dz);

            if (n <= DKSAXEROTATION_EPSILON)
			{

				angle = M_PI;

				if (sinZ < DKSAXEROTATION_EPSILON)
				{
					x = rep1->axe_x->x + rep2->axe_x->x;
					y = rep1->axe_x->y + rep2->axe_x->y;
					z = rep1->axe_x->z + rep2->axe_x->z;
				}
				else if (sinY < DKSAXEROTATION_EPSILON)
				{
					x = rep1->axe_x->x + rep2->axe_x->x;
					y = rep1->axe_x->y + rep2->axe_x->y;
					z = rep1->axe_x->z + rep2->axe_x->z;
				}
				else
				{
					x = rep1->axe_y->x + rep2->axe_y->x;
					y = rep1->axe_y->y + rep2->axe_y->y;
					z = rep1->axe_y->z + rep2->axe_y->z;
				}
				normalise();
				return;
			}

			x = dx / n;
			y = dy / n;
			z = dz / n;         /// Quand n devient <1, il peut y avoir une imprécision dans l'axe.

			//Calcul de l'angle:

			if (sinX > sinY) { vect1 = rep1->axe_x; vect2 = rep2->axe_x; } //choix de l'axe qui a subi la plus forte rotation;
			else { vect1 = rep1->axe_y; vect2 = rep2->axe_y; }

			//Projection du vecteur sur l'axe de rotation
			double prod_scal = vect1->x * x + vect1->y * y + vect1->z * z;

			double proj_x = x * prod_scal;
			double proj_y = y * prod_scal;
			double proj_z = z * prod_scal;

            //Les deux vecteurs sont projetés sur le plan de normale x,y,z:

			axe1_x = vect1->x - proj_x;
			axe1_y = vect1->y - proj_y;
			axe1_z = vect1->z - proj_z;

			axe2_x = vect2->x - proj_x;
			axe2_y = vect2->y - proj_y;
			axe2_z = vect2->z - proj_z;

			//Calcul de l'angle:
			double norme1=sqrt(axe1_x*axe1_x+axe1_y*axe1_y+axe1_z*axe1_z);
			double norme2=sqrt(axe2_x*axe2_x+axe2_y*axe2_y+axe2_z*axe2_z);
			axe1_x/=norme1;
			axe1_y/=norme1;
			axe1_z/=norme1;
			axe2_x/=norme2;
			axe2_y/=norme2;
			axe2_z/=norme2;

			double cosAngle = axe1_x * axe2_x + axe1_y * axe2_y + axe1_z * axe2_z;
			angle = acos(cosAngle);

		}

		//Rotation d'une table de points autour de l'axe:
		//La table de résultats peut être la même que la table d'entrée.
		void rotation_points(vector<double>* points, vector<double>* points_resultat, uint32_t nbrPoints = 0xffffffff)
		{
			/*
			formule d'Olinde Rodrigues :
			V = cos(phi)*U + (1-cos(phi))*((U.N)*N) + sin(phi)[N^U]
			La formule ci-dessus donne l'expression vectorielle du transformé V  d'un vecteur U  quelconque, dans la rotation phi,N d'angle phi, et d'axe N unitaire
			*/

			uint32_t nb;
			if (nbrPoints == 0xffffffff) nb = points->size() / 3;
			else nb = nbrPoints;

			for (uint32_t i = 0; i < nb; i++)
			{
				double px = (*points)[3 * i] - position.x;
				double py = (*points)[3 * i + 1] - position.y;
				double pz = (*points)[3 * i + 2] - position.z;
				double prod_scal =  px * x + py * y + pz * z;
				double cos_angle = cos(angle);
				double sin_angle = sin(angle);

				(*points_resultat)[3 * i] = cos_angle * px + sin_angle * (y * pz - z * py) + (1. - cos_angle) * prod_scal * x + position.x;
				(*points_resultat)[3 * i + 1] = cos_angle * py + sin_angle * (z * px - x * pz) + (1. - cos_angle) * prod_scal * y + position.y;
				(*points_resultat)[3 * i + 2] = cos_angle * pz + sin_angle * (x * py - y * px) + (1. - cos_angle) * prod_scal * z + position.z;
			}
		}

		void rotation_points(double* points, double* points_resultat, uint32_t nbrPoints)
		{
			for (uint32_t i = 0; i < nbrPoints; i++)
			{
				double px = points[3 * i] - position.x;
				double py = points[3 * i + 1] - position.y;
				double pz = points[3 * i + 2] - position.z;
				double prod_scal =  px * x + py * y + pz * z;
				double cos_angle = cos(angle);
				double sin_angle = sin(angle);

				points_resultat[3 * i] = cos_angle * px + sin_angle * (y * pz - z * py) + (1. - cos_angle) * prod_scal * x + position.x;
				points_resultat[3 * i + 1] = cos_angle * py + sin_angle * (z * px - x * pz) + (1. - cos_angle) * prod_scal * y + position.y;
				points_resultat[3 * i + 2] = cos_angle * pz + sin_angle * (x * py - y * px) + (1. - cos_angle) * prod_scal * z + position.z;
			}
		}


		virtual void toString()
		{
			cout<< "dksAxeRotation -> direction: "<< x <<" "<< y<<" "<< z<< ", angle°: "<< angle*180/M_PI << ", position: ";
            position.toString();
		}

};

#endif // DKSAXEROTATION_H_INCLUDED
