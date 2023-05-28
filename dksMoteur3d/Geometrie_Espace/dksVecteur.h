#ifndef DKSVECTEUR_H_INCLUDED
#define DKSVECTEUR_H_INCLUDED

#include "math.h"
#include "../../dksMoteur2d/dksRendu.h"
#include <vector>

#define DKSVECTEUR_EPSILON 1e-8

    using namespace std;

class dksVecteur : public dksRendu
{

    public:

        double x;
        double y;
        double z;

        // L'axe est normalisé à l'unité
        //L'angle est en radians
        void rotation(double* pt, double angle,double* axe, double* resultat)
		{
			/*
			formule d'Olinde Rodrigues :
			V = cos(phi)*U + (1-cos(phi))*((U.N)*N) + sin(phi)[N^U]
			La formule ci-dessus donne l'expression vectorielle du transformé V  d'un vecteur U  quelconque, dans la rotation phi,N d'angle phi, et d'axe N unitaire
			*/

			double prod_scal = axe[0]*pt[0]+axe[1]*pt[1]+axe[2]*pt[2];

			double cos_angle = cos(angle);
			double sin_angle = sin(angle);

			resultat[0] = cos_angle * pt[0] + sin_angle * (axe[1] * pt[2] - axe[2]* pt[1]) + (1. - cos_angle) * prod_scal * axe[0];
			resultat[1] = cos_angle * pt[1] + sin_angle * (axe[2] * pt[0] - axe[0] * pt[2]) + (1. - cos_angle) * prod_scal * axe[1];
			resultat[2] = cos_angle * pt[2] + sin_angle * (axe[0] * pt[1] - axe[1] * pt[0]) + (1. - cos_angle) * prod_scal * axe[2];
		}



		// Constructeur
		dksVecteur ( double px = 1., double py = 0., double pz = 0. ):dksRendu()
		{
			x = px;
			y = py;
			z = pz;
		}

        void inverse()
        {
            x=-x;y=-y;z=-z;
        }

        void determine(double px,double py,double pz)
        {
            x=px;y=py;z=pz;
        }

        bool est_nul()
        {
            if (x==0 && y==0 && z==0) return true;
            else return false;
        }

        virtual void copie(dksVecteur* source)
        {
            x=source->x;
            y=source->y;
            z=source->z;
        }

		double norme() { return sqrtf(x * x + y * y + z * z); }

		double norme_carre() { return x * x + y * y + z * z; }

        void unitaire()
        {
            double n=sqrtf(x * x + y * y + z * z);
            x/=n;
            y/=n;
            z/=n;
        }

		static double produit_scalaire( dksVecteur* v1, dksVecteur* v2 )
		{
			return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
		}

		void produit_vectoriel(dksVecteur* v1, dksVecteur* v2)
		{
			x = v1->y * v2->z - v2->y * v1->z;
			y = v1->z * v2->x - v2->z * v1->x;
			z = v1->x * v2->y - v2->x * v1->y;
		}

		void produit_reel(double reel)
		{
			x *= reel;
			y *= reel;
			z *= reel;
		}

		void produit_vecteur_reel(dksVecteur* vecteur,double reel)
		{
			x = vecteur->x*reel;
			y = vecteur->y*reel;
			z = vecteur->z*reel;
		}

		void vecteur_nul()
		{
			x = 0; y = 0; z = 0;
		}

		// resultat est le projeté orthogonale de px,py,pz sur le plan (ABC)
		static void projection_orthogonale(double px, double py, double pz, dksVecteur* A, dksVecteur* B, dksVecteur* C, dksVecteur* resultat)
		{
			// intermédiaires de calcul
			double theta = A->x - B->x;
			double phi = A->x - C->x;

			double alpha = B->y - A->y + theta;
			double beta = B->z - A->z + theta;
			double gamma = C->y - A->y + phi;
			double delta = C->z - A->z + phi;

			double temp = alpha * delta - gamma * beta;

			// n : vecteur normal au plan (ABC)
			double nz = ( alpha * phi - gamma * theta ) / temp;
			double ny = ( delta * theta - beta * phi) / temp;
			double nx = 1 - ny - nz;

			// equation du plan (ABC) : nx * x + ny * y + nz * z + d = 0 or A € (ABC) donc :
			double d = -nx * A->x - ny * A->y - nz * A->z;

			double k = ( -nx * px - ny * py - nz * pz - d) / (nx * nx + ny * ny + nz * nz);

			resultat->x = px + k * nx;
			resultat->y = py + k * ny;
			resultat->z = pz + k * nz;
		}

		static double distance_point_plan(double px, double py, double pz,dksVecteur* A,dksVecteur* B,dksVecteur* C)
		{
			// intermédiaires de calcul
			double theta = A->x - B->x;
			double phi = A->x - C->x;

			double alpha = B->y - A->y + theta;
			double beta = B->z - A->z + theta;
			double gamma = C->y - A->y + phi;
			double delta = C->z - A->z + phi;

			double temp = alpha * delta - gamma * beta;

			// n : vecteur normal au plan (ABC)
			double nz = ( alpha * phi - gamma * theta ) / temp;
			double ny = ( delta * theta - beta * phi) / temp;
			double nx = 1 - ny - nz;

			// equation du plan (ABC) : nx * x + ny * y + nz * z + d = 0 or A € (ABC) donc :
			double d = -nx * A->x - ny * A->y - nz * A->z;

			return fabs( -nx * px - ny * py - nz * pz - d) / (nx * nx + ny * ny + nz * nz);
		}

		// calcul l'intersection des droites (AB) et (CD), sans vérification de la réalité de l'intersection trouvée
		//Fonction utilisée notamment par l'interface d'édition de scènes.
		static void intersection_certaine_de_droites(dksVecteur* A, dksVecteur* B, dksVecteur* C, dksVecteur* D, dksVecteur* res)
		{
			double dir_AB_x = B->x - A->x;
			double dir_AB_y = B->y - A->y;
			double dir_AB_z = B->z - A->z;

			double dir_CD_x = D->x - C->x;
			double dir_CD_y = D->y - C->y;

			double tempy = A->y - C->y;
			double tempx = A->x - C->x;

			if ((dir_AB_x == 0) && (dir_AB_y == 0))
			{
				double facteur2;

				if (dir_CD_x == 0) { facteur2 = tempy / dir_CD_y; }
				else { facteur2 = tempx / dir_CD_x; }

				res->x = C->x + facteur2 * dir_CD_x;
				res->y = C->y + facteur2 * dir_CD_y;
				res->z = C->z + facteur2 * (D->z - C->z);

				return;
			}

			double temp = dir_AB_x * dir_CD_y - dir_AB_y * dir_CD_x;
			double facteur = (tempy * dir_CD_x - tempx * dir_CD_y) / temp ;

			res->x = A->x +  dir_AB_x * facteur;
			res->y = A->y +  dir_AB_y * facteur;
			res->z = A->z +  dir_AB_z * facteur;
		}


        // L'axe doit être un vecteur unitaire.
        //L'angle est en radians
        void rotation(double angle, dksVecteur* axe)
		{
			/*
			formule d'Olinde Rodrigues :
			V = cos(phi)*U + (1-cos(phi))*((U.N)*N) + sin(phi)[N^U]
			La formule ci-dessus donne l'expression vectorielle du transformé V  d'un vecteur U  quelconque, dans la rotation phi,N d'angle phi, et d'axe N unitaire
			*/

			double prod_scal = produit_scalaire(axe,this);

			double cos_angle = cosf(angle);
			double sin_angle = sinf(angle);

			double xt = cos_angle * x + sin_angle * (axe->y * z - axe->z * y) + (1. - cos_angle) * prod_scal * axe->x;
			double yt = cos_angle * y + sin_angle * (axe->z * x - axe->x * z) + (1. - cos_angle) * prod_scal * axe->y;
			z = cos_angle * z + sin_angle * (axe->x * y - axe->y * x) + (1. - cos_angle) * prod_scal * axe->z;
			x = xt;
			y = yt;
		}

		// L'axe est normalisé à l'unité
        //L'angle est en radians
        void rotation(double angle,double rx,double ry,double rz)
		{
			/*
			formule d'Olinde Rodrigues :
			V = cos(phi)*U + (1-cos(phi))*((U.N)*N) + sin(phi)[N^U]
			La formule ci-dessus donne l'expression vectorielle du transformé V  d'un vecteur U  quelconque, dans la rotation phi,N d'angle phi, et d'axe N unitaire
			*/

			double prod_scal = rx*x+ry*y+rz*z;

			double cos_angle = cos(angle);
			double sin_angle = sin(angle);

			double xt = cos_angle * x + sin_angle * (ry * z - rz * y) + (1. - cos_angle) * prod_scal * rx;
			double yt = cos_angle * y + sin_angle * (rz * x - rx * z) + (1. - cos_angle) * prod_scal * ry;
			z = cos_angle * z + sin_angle * (rx * y - ry * x) + (1. - cos_angle) * prod_scal * rz;
			x = xt;
			y = yt;
		}

        //---------------------------------------------------------------
        //Rotation en fonction des angles de Roulis, Tangage et Lacet:
        //  Les angles sont en radians.
        //---------------------------------------------------------------
		void rotation_RTL(float roulis,float tangage, float lacet)
        {
            double px=x;
            double py=y;
            double pz=z;
            double sin_aR=sin(roulis);
            double cos_aR=cos(roulis);
            double sin_aT=sin(tangage);
            double cos_aT=cos(tangage);
            double sin_aL=sin(lacet);
            double cos_aL=cos(lacet);

            x= sin_aL * ( pz*cos_aT - sin_aT * ( px*sin_aR + py*cos_aR ) ) +  cos_aL * ( px*cos_aR - py*sin_aR );
            y= pz * sin_aT + cos_aT * ( px*sin_aR + py*cos_aR );
            z= cos_aL * ( pz*cos_aT - sin_aT * ( px*sin_aR + py*cos_aR ) ) - sin_aL * ( px*cos_aR - py*sin_aR );
        }

        //Renvoie le paramètre correspondant à l'intersection entre la droite (A,B) et plan(O,U,V)
        //La normale N doit être unitaire.
        //Renvoie Nan si la droite est parallèle au plan.
        static double intersection_droite_plan(dksVecteur* segment_A, dksVecteur* segment_B, dksVecteur* plan_O, dksVecteur* plan_U, dksVecteur* plan_V, dksVecteur* N)
        {
            dksVecteur AB( segment_B->x - segment_A->x , segment_B->y - segment_A->y , segment_B->z - segment_A->z );
            dksVecteur OU( plan_U->x - plan_O->x , plan_U->y - plan_O->y , plan_U->z - plan_O->z );
            dksVecteur OV( plan_V->x - plan_O->x , plan_V->y - plan_O->y , plan_V->z - plan_O->z);

            //Calcul la distance:
            double D= - N->x*plan_O->x - N->y*plan_O->y - N->z*plan_O->z;

            //Test la parallèléité:
            double temp=AB.x*N->x+AB.y*N->y+AB.z*N->z;
            if (temp > -DKSVECTEUR_EPSILON && temp < DKSVECTEUR_EPSILON) return NAN;

            //Calcul le facteur:
            //if (N->x<0.) return -(N->x*segment_A->x+N->y*segment_A->y+N->z*segment_A->z+D)/temp;
            //else
            return -(N->x*segment_A->x+N->y*segment_A->y+N->z*segment_A->z+D)/temp;
        }

        /*
		On compte le nombre d'intersection avec la demi droite positive des U
		Test dans l'espace. Le point doit être dans le plan du polygone.
		*/
		static bool inclus_dans_polygone(vector<uint32_t>* tabAretes, vector<double>* tabSommets,uint32_t nbr_sommets,
                                        dksVecteur* normale,
                                        double x, double y, double z )
		{
			uint32_t nbrInter = 0;
            uint32_t U,V;
            double Up,Vp;

			//Détermine le plan de projection.
			//La composante la plus grande de la normale détermine la dimension ignorée:
			double nx=fabs(normale->x);
			double ny=fabs(normale->y);
			double nz=fabs(normale->z);
			if ((nx > ny) && (nx > nz)) { U=1;V=2;Up=y;Vp=z; }
			else if ((ny > nx) && (ny > nz)) { U=2;V=0;Up=z;Vp=x; }
			else { U=0;V=1;Up=x;Vp=y; }

			double xpA = (*tabSommets)[(*tabAretes)[0]*3+U] - Up;
			double ypA = (*tabSommets)[(*tabAretes)[0]*3+V] - Vp;

			if (xpA == 0 && ypA == 0) {return true;} //Si le point est sur un sommet, inclusion positive.
			else
			{
				if (xpA == 0) { xpA = 1e-4; }
				if (ypA == 0) { ypA = 1e-4; }
			}


			for (uint32_t i = 1; i <= nbr_sommets;i++)
			{
				//Origine du polygone sur le point testé
				double xpB = (*tabSommets)[(*tabAretes)[i]*3+U] - Up;
				double ypB = (*tabSommets)[(*tabAretes)[i]*3+V] - Vp;
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


		virtual void toString()
		{
			cout<<"Vecteur: "<<x<<","<<y<<","<<z<<endl;
		}

		//===============================================
		//          Affichage
		//===============================================
		virtual void affiche(dksOeil* oeil)
		{

		}
        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {

        }
        virtual void affiche_profondeur(dksOeil* oeil)
        {

        }
};




#endif // DKSVECTEUR_H_INCLUDED
