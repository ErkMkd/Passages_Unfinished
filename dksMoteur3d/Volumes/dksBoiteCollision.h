#ifndef BOITE_COLLISION_H_INCLUDED
#define BOITE_COLLISION_H_INCLUDED


#include "../dksAffichagesDebogue.h"
#include "../Geometrie_Espace/dksVecteur.h"
#include "../Geometrie_Espace/dksVecteurUnitaire.h"
#include "../Geometrie_Espace/dksRepere.h"

#include <vector>

    using namespace std;

//Structure des faces des boites de collision:
typedef struct
    {
        uint16_t nbrSommets;
        vector<uint32_t> aretes;   //Les aretes doivent boucler (dernier sommets lié au premier)
        //Données initialisées automatiquement:
        dksVecteurUnitaire normale_relative;
        dksVecteurUnitaire normale;
    }Face_collision;



//=============================================================================================
//                          Définition de la classe
//=============================================================================================
class dksBoiteCollision
{
    public:
        double xmin;
        double xmax;
        double ymin;
        double ymax;
        double zmin;
        double zmax;
        vector<double> sommets_relatifs;
        vector<double> sommets_absolus;
        vector<Face_collision> faces;
        vector<uint32_t> aretes;
        uint32_t nbrSommets;
        uint32_t nbrFaces;
        uint32_t nbrAretes;

    public:

        //=========================================================================================
        //                          CONSTRUCTEUR
        //=========================================================================================

        dksBoiteCollision(double p_xmin,double p_xmax, double p_ymin, double p_ymax, double p_zmin, double p_zmax)
        {
            xmin=p_xmin;
            xmax=p_xmax;
            ymin=p_ymin;
            ymax=p_ymax;
            zmin=p_zmin;
            zmax=p_zmax;
            initialise_sommets();
            initialise_faces();
            initialisation_aretes_depuis_faces();
        }

        //=========================================================================================
        //                          Gestion des sommets
        //=========================================================================================

        void determine_limites(double p_xmin,double p_xmax,double p_ymin,double p_ymax,double p_zmin,double p_zmax)
        {
            xmin=p_xmin;
            xmax=p_xmax;
            ymin=p_ymin;
            ymax=p_ymax;
            zmin=p_zmin;
            zmax=p_zmax;
            sommets_relatifs[0]=xmin;sommets_relatifs[1]=ymin;sommets_relatifs[2]=zmin;
            sommets_relatifs[3]=xmin;sommets_relatifs[4]=ymax;sommets_relatifs[5]=zmin;
            sommets_relatifs[6]=xmax;sommets_relatifs[7]=ymax;sommets_relatifs[8]=zmin;
            sommets_relatifs[9]=xmax;sommets_relatifs[10]=ymin;sommets_relatifs[11]=zmin;

            sommets_relatifs[12]=xmin;sommets_relatifs[13]=ymin;sommets_relatifs[14]=zmax;
            sommets_relatifs[15]=xmin;sommets_relatifs[16]=ymax;sommets_relatifs[17]=zmax;
            sommets_relatifs[18]=xmax;sommets_relatifs[19]=ymax;sommets_relatifs[20]=zmax;
            sommets_relatifs[21]=xmax;sommets_relatifs[22]=ymin;sommets_relatifs[23]=zmax;
        }

        //----------Génère les sommets:
        void initialise_sommets()
        {
            //cout<<"-------Limites: "<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<" "<<zmin<<" "<<zmax<<endl;
            sommets_relatifs.push_back(xmin);sommets_relatifs.push_back(ymin);sommets_relatifs.push_back(zmin);
            sommets_relatifs.push_back(xmin);sommets_relatifs.push_back(ymax);sommets_relatifs.push_back(zmin);
            sommets_relatifs.push_back(xmax);sommets_relatifs.push_back(ymax);sommets_relatifs.push_back(zmin);
            sommets_relatifs.push_back(xmax);sommets_relatifs.push_back(ymin);sommets_relatifs.push_back(zmin);

            sommets_relatifs.push_back(xmin);sommets_relatifs.push_back(ymin);sommets_relatifs.push_back(zmax);
            sommets_relatifs.push_back(xmin);sommets_relatifs.push_back(ymax);sommets_relatifs.push_back(zmax);
            sommets_relatifs.push_back(xmax);sommets_relatifs.push_back(ymax);sommets_relatifs.push_back(zmax);
            sommets_relatifs.push_back(xmax);sommets_relatifs.push_back(ymin);sommets_relatifs.push_back(zmax);
            nbrSommets=sommets_relatifs.size()/3;

            sommets_absolus.reserve(nbrSommets*3);
        }

        //--------- Facteur d'échelle:
        void applique_facteur_echelle(double f)
        {
            for(uint16_t i=0;i<8;i++)
            {
                sommets_relatifs[3*i]*=f;
                sommets_relatifs[3*i+1]*=f;
                sommets_relatifs[3*i+2]*=f;
            }
            xmin*=f;
            xmax*=f;
            ymin*=f;
            ymax*=f;
            zmin*=f;
            zmax*=f;
        }

        //---------Calcul les sommets absolus:
        void calcul_sommets_absolus(dksRepere* repere)
        {
            double ax_x = repere->axe_x->x * repere->echelle_x;
            double ax_y = repere->axe_x->y * repere->echelle_x;
            double ax_z = repere->axe_x->z * repere->echelle_x;

            double ay_x = repere->axe_y->x * repere->echelle_y;
            double ay_y = repere->axe_y->y * repere->echelle_y;
            double ay_z = repere->axe_y->z * repere->echelle_y;

            double az_x = repere->axe_z->x * repere->echelle_z;
            double az_y = repere->axe_z->y * repere->echelle_z;
            double az_z = repere->axe_z->z * repere->echelle_z;

            double rx=repere->x;
            double ry=repere->y;
            double rz=repere->z;

            for (unsigned int i = 0; i < nbrSommets*3;)
            {
                double xt = sommets_relatifs[i];
                double yt = sommets_relatifs[i+1];
                double zt = sommets_relatifs[i+2];
                // coordonnées de l'objet enfant dans le repère de l'objet parent
                sommets_absolus[i++] =  rx + xt * ax_x + yt * ay_x + zt * az_x;
                sommets_absolus[i++] =  ry + xt * ax_y + yt * ay_y + zt * az_y;
                sommets_absolus[i++] =  rz + xt * ax_z + yt * ay_z + zt * az_z;
            }
            calcul_normales(repere);
        }

        //=========================================================================================
        //                          Gestion des arêtes
        //=========================================================================================

        //--------------------------------------------------------------------------------------
        //              Création de la table des arêtes à partir des faces:
        //--------------------------------------------------------------------------------------
    private:
        void initialisation_aretes_depuis_faces()
		{
			nbrAretes = 0;
            uint32_t i,j,k,sommetA,sommetB;

			for (i = 0; i < nbrFaces; i++)
			{
				for (j = 0; j < faces[i].nbrSommets; j++ )
				{
					sommetA = faces[i].aretes[j];
					sommetB = faces[i].aretes[j + 1];

					for (k = 0; k < nbrAretes*2; )
					{
						if (((aretes[k] == sommetA) && (aretes[k + 1] == sommetB)) || ((aretes[k] == sommetB) && (aretes[k + 1] == sommetA)))
						{break; }
						k += 2;
					}
					if (k == nbrAretes*2)
					{
						aretes.push_back(sommetA);
						aretes.push_back(sommetB);
						nbrAretes++;
					}
				}
			}
		}

		//=========================================================================================
        //                          Gestion des faces
        //=========================================================================================

        //---------------------------------------------------------
        //Génère les faces:
        //---------------------------------------------------------
    private:
        void initialise_faces()
        {
            Face_collision face1;
            face1.aretes.push_back(0);
            face1.aretes.push_back(1);
            face1.aretes.push_back(2);
            face1.aretes.push_back(3);
            face1.aretes.push_back(0);
            face1.nbrSommets=face1.aretes.size()-1;
            faces.push_back(face1);

            Face_collision face2;
            face2.aretes.push_back(2);
            face2.aretes.push_back(6);
            face2.aretes.push_back(7);
            face2.aretes.push_back(3);
            face2.aretes.push_back(2);
            face2.nbrSommets=face2.aretes.size()-1;
            faces.push_back(face2);

            Face_collision face3;
            face3.aretes.push_back(6);
            face3.aretes.push_back(5);
            face3.aretes.push_back(4);
            face3.aretes.push_back(7);
            face3.aretes.push_back(6);
            face3.nbrSommets=face3.aretes.size()-1;
            faces.push_back(face3);

            Face_collision face4;
            face4.aretes.push_back(5);
            face4.aretes.push_back(1);
            face4.aretes.push_back(0);
            face4.aretes.push_back(4);
            face4.aretes.push_back(5);
            face4.nbrSommets=face4.aretes.size()-1;
            faces.push_back(face4);

            Face_collision face5;
            face5.aretes.push_back(0);
            face5.aretes.push_back(3);
            face5.aretes.push_back(7);
            face5.aretes.push_back(4);
            face5.aretes.push_back(0);
            face5.nbrSommets=face5.aretes.size()-1;
            faces.push_back(face5);

            Face_collision face6;
            face6.aretes.push_back(5);
            face6.aretes.push_back(6);
            face6.aretes.push_back(2);
            face6.aretes.push_back(1);
            face6.aretes.push_back(5);
            face6.nbrSommets=face6.aretes.size()-1;
            faces.push_back(face6);

            nbrFaces=faces.size();
            normales_relatives_faces();
        }




		//---------------------------------------------------------
        // Calcul les normales relatives des faces
        //---------------------------------------------------------

		void normales_relatives_faces()
		{
		    uint32_t i;
		    dksVecteur v1,v2;
			for (i = 0; i < nbrFaces; i++)
			{
				dksVecteurUnitaire* nml_rel = &faces[i].normale_relative;
				vector<uint32_t>* at = &(faces[i].aretes);

				uint32_t at0 = (*at)[0] * 3;
				uint32_t at1 = (*at)[1] * 3;
				uint32_t at2 = (*at)[2] * 3;

				v1.x = sommets_relatifs[at1] - sommets_relatifs[at0];
				v1.y = sommets_relatifs[at1 + 1] - sommets_relatifs[at0 + 1];
				v1.z = sommets_relatifs[at1 + 2] - sommets_relatifs[at0 + 2];

				v2.x = sommets_relatifs[at2] - sommets_relatifs[at1];
				v2.y = sommets_relatifs[at2 + 1] - sommets_relatifs[at1 + 1];
				v2.z = sommets_relatifs[at2 + 2] - sommets_relatifs[at1 + 2];

				nml_rel->produit_vectoriel(&v1, &v2);
				nml_rel->normalise();
			}

		}

		//---------------------------------------------------------
        //  Calcul les normales absolues des faces:
        //---------------------------------------------------------

        void calcul_normales(dksRepere* repere)
        {

            double ax_x = repere->axe_x->x ;
            double ax_y = repere->axe_x->y ;
            double ax_z = repere->axe_x->z ;

            double ay_x = repere->axe_y->x ;
            double ay_y = repere->axe_y->y ;
            double ay_z = repere->axe_y->z ;

            double az_x = repere->axe_z->x ;
            double az_y = repere->axe_z->y ;
            double az_z = repere->axe_z->z ;

            uint32_t i;
            for (i=0;i<nbrFaces;i++)
            {
                double xt=faces[i].normale_relative.x;
                double yt=faces[i].normale_relative.y;
                double zt=faces[i].normale_relative.z;
                faces[i].normale.x = xt * ax_x + yt * ay_x + zt * az_x;
                faces[i].normale.y = xt * ax_y + yt * ay_y + zt * az_y;
                faces[i].normale.z = xt * ax_z + yt * ay_z + zt * az_z;
            }

        }


		//=========================================================================================
        //                          Debugage
        //=========================================================================================
    public:
        void affiche()
        {
            /*
            for (uint32_t i=0;i<nbrSommets;i++)
            {
                Affichages_debogue::affiche_point(sommets_absolus[3*i],sommets_absolus[3*i+1],sommets_absolus[3*i+2],0xffff00);
            }
            */


            for (uint32_t i=0;i< nbrAretes;i++)
            {
                uint32_t indice=aretes[2*i]*3;
                double Ax=sommets_absolus[indice];
                double Ay=sommets_absolus[indice+1];
                double Az=sommets_absolus[indice+2];
                indice=aretes[2*i+1]*3;
                double Bx=sommets_absolus[indice];
                double By=sommets_absolus[indice+1];
                double Bz=sommets_absolus[indice+2];
                dksAffichagesDebogue::affiche_segment(Ax,Ay,Az,Bx,By,Bz,0xffff00);
            }
        }

        void en_texte()
        {
            uint32_t i;
            cout<<"++ Boite de collision ++"<<endl;
            cout<<"   Nombre de sommets: "<<nbrSommets<<endl;
            cout<<"   Nombre de faces  : "<<nbrFaces<<endl;
            cout<<"   Sommets:"<<endl;
            for (i=0;i<nbrSommets;i++)
            {
                cout<<"        x:"<<sommets_relatifs[3*i]<<"  y:"<<sommets_relatifs[3*i+1]<<"  z:"<<sommets_relatifs[3*i+2]<<endl;
            }
        }



};
#endif // BOITE_COLLISION_H_INCLUDED
