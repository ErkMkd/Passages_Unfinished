#ifndef DKSCONTENEUR_H_INCLUDED
#define DKSCONTENEUR_H_INCLUDED

#include <math.h>
#include <vector>

#define DKS_TAILLE_NOMS 128

#include "../Geometrie_Espace/dksRepere.h"
#include "../Geometrie_Espace/dksAxeRotation.h"
#include "../../Utiles/Matrice4x4.h"


#define DKSCONTENEUR_EPSILON 1e-8

#define DKSCONTENEUR_CLASSE_CONTENEUR "Conteneur"
#define DKSCONTENEUR_CLASSE_CONTENEUR_ID 0

#define AVANT 0
#define ARRIERE 1
#define GAUCHE 2
#define DROITE 3
#define HAUT 4
#define BAS 5
#define HAUT_ABSOLU 6
#define BAS_ABSOLU 7

//Codes d'erreurs à récupérer dans .erreur:

#define DKSCONTENEUR_OK 0
#define DKSCONTENEUR_ERREUR_MEMOIRE 1

//===========================================================================================
//              Classe de gestion des conteneur.
//===========================================================================================

class dksConteneur :public dksRepere
{
		//Noms des classes héritées:
    public:

        uint16_t erreur; //Code d'erreur.
        char nom[DKS_TAILLE_NOMS];
		char* classe;
		uint16_t classe_id;

        dksRepere* repere_absolu; //Le repère dans l'espace absolu
        Matrice4x4 matrice;    //La matrice correspondant au repère absolu.

		dksConteneur* parent; //Le parent du conteneur
		vector<dksConteneur*> enfants; //Tableau des enfants

        bool a_change;	//Si les coordonnées du conteneur ont été modifiées lors de la gestion des mouvement.
                        //Cette variable est remise à false après la propagation de la hiérarchie.
		bool a_change_par_propagation;	//Si les coordonnées du conteneur ont été modifiées suite à une propagation hiérarchique.
                                        //Cette variable n'a à être remise à false que par les objets qui s'en servent (volumes,sources lumineuses)


		// gestion de la contrainte à une trajectoire
        bool contrainte_trajectoire; // aligne l'axe_z à la tangente de la trajectoire
		dksVecteurUnitaire* axe;  //Sert pour le calcul de la contrainte de trajectoire

		//Gestion de l'inertie
		dksVecteur* vitesse;
		dksVecteur* vitesse_circulaire;
		dksVecteur* propulsion;
		double propulsion_verticale;
		static const double  difference_acceptable;
		dksVecteur* propulsion_angulaire;
		bool visee_activee;
		double angle_rotation_instantanne;
		double angle_restant;

        dksRepere repere_t0;   //Les données cinétiques à l'intant t0.
        dksRepere repere_parent_ti;   //Le repère du parent à un instant compris entre t0 et t1.
        dksVecteur propulsion_t0;
        dksVecteur propulsion_angulaire_t0;
        double angle_restant_t0;
        bool visee_activee_t0;

		dksVecteurUnitaire* axe_rotation;

        //Gestion des calculs temporels:
        //double position_temps;
		//double delta_t;         //Intervalle avec le temps précédent (envoyé par le moteur)
		//double t0;
		double interpolation;   //Le facteur d'interpolation linéaire de la cinétique dans l'intant t.

		double masse;
		double valeur_propulsion_horizontale;   //Valeurs absolues des propulsions
		double valeur_propulsion_verticale;
		double valeur_propulsion_angulaire;
		double coefficient_frottement;

		bool contrainte_direction_propulsion;   //True si la direction de la propulsion est fixe par rapport au conteneur.
        dksVecteurUnitaire* direction_propulsion; //La direction de la propulsion, relative au conteneur.
        double valeur_propulsion_lineaire_courante;      //La valeur de la propulsion en cours.

        double distance_poursuite_min;  //Gestion de la poursuite d'un point
        double inertie_poursuite;
        double inertie_contrainte_roulis_rotation;  //L'amplitude de la contrainte du roulis à la rotation de l'axe Z

    protected:
		// intermédiaires de calculs
		dksVecteurUnitaire* axe_vertical;
		dksVecteurUnitaire* vecteur;
		dksVecteurUnitaire* vecteur_bis;
		dksAxeRotation axe_rotation_matrice;    //Sert pour le calcul des matrices

		bool drapeau_contrainte_verticale;
		bool drapeau_contrainte_roulis_rotation;    //L'angle de roulis est calculé en fonction de l'angle de rotation de l'axe Z
                                                    //Attention ! Il faut d'abord activer la contrainte verticale pour que la contrainte de roulis fonctionne !
		dksRepere repere_reference; ///Repère fixe d'axes X(1,0,0) Y(0,1,0) Z(0,0,1)

    public:
        //****************************************
        //      Renvoie une erreur OpenGl
        //****************************************

        static bool erreur_openGl(char* message)
        {
            GLenum erreur_openGl=glGetError();
            if (erreur_openGl!=GL_NO_ERROR)
            {
                cout<<message<<" : "<<gluErrorString(erreur_openGl)<<endl;
                return true;
            }
             return false;
        }
		//======================================================== Constructeur: ===================================

		dksConteneur(char* p_nom = "Conteneur",
                    double px = 0., double py = 0., double pz = 0.,
                    dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL)
                    :dksRepere(px,py,pz,axe1,axe2,axe3)
		{
		    erreur=DKSCONTENEUR_OK;

			copie_nom(p_nom);

			classe=DKSCONTENEUR_CLASSE_CONTENEUR;
			classe_id=DKSCONTENEUR_CLASSE_CONTENEUR_ID;
			repere_absolu=new dksRepere();
			a_change=true;
			a_change_par_propagation=false;
			contrainte_trajectoire=false;
			parent=NULL;

			axe=new dksVecteurUnitaire();
			vitesse=new dksVecteur(0.,0.,0.);
			vitesse_circulaire=new dksVecteur(0.,0.,0.);
            propulsion=new dksVecteur(0.,0.,0.);
            propulsion_verticale=0.;
            propulsion_angulaire=new dksVecteur(0.,0.,0.);
            visee_activee=false;
            angle_restant=0.;
            axe_rotation=new dksVecteurUnitaire(0.,0.,0.);
            masse=0.;
            valeur_propulsion_horizontale=1000.;
            valeur_propulsion_verticale=700.;
            valeur_propulsion_angulaire=500.;
            determine_coefficient_frottement(float(100),float(1000));
            direction_propulsion=NULL;
            contrainte_direction_propulsion=false;

            distance_poursuite_min=10.;
            inertie_poursuite=10.;

            delta_t=0.;

            vecteur=new dksVecteurUnitaire(1.,0.,0.);
            vecteur_bis=new dksVecteurUnitaire(1.,0.,0.);
            axe_vertical=new dksVecteurUnitaire(0.,1.,0.);

            drapeau_contrainte_verticale=false;
            drapeau_contrainte_roulis_rotation=false;
            inertie_contrainte_roulis_rotation=5.;
           // t0=0.;
           // delta_t=0.;
           // position_temps=0.;
		}

        //-------- Copie le nom:
        void copie_nom(char* p_nom)
        {
            uint32_t i,taille;
            taille=strlen(p_nom);
            if (taille>DKS_TAILLE_NOMS-1)taille=DKS_TAILLE_NOMS-1;
            for(i=0;i<taille;i++)
            {
                nom[i]=p_nom[i];
            }
            nom[i]=0;
        }


		~dksConteneur()
		{
            std::cout<<"Destruction du conteneur: "<<nom<<std::endl;
		    if (axe_vertical!=NULL) delete(axe_vertical);
		    if (vecteur_bis!=NULL) delete(vecteur_bis);
		    if (vecteur!=NULL) delete(vecteur);
		    if (axe_rotation != NULL) delete(axe_rotation);
		    if (propulsion_angulaire!=NULL) delete(propulsion_angulaire);
		    if (vitesse_circulaire!=NULL)delete(vitesse_circulaire);
		    if (vitesse!=NULL)delete(vitesse);
		    if (axe!=NULL)delete(axe);
		    if (repere_absolu!=NULL)delete(repere_absolu);
		}


        //===================================================================================================================
		//======================================== Fonctions de gestion de la hiérarchie : =================================
        //===================================================================================================================
        uint32_t renvoie_nbr_enfants()
        {
            return enfants.size();
        }

        void vide_enfants()
        {
            enfants.clear();
        }

		//Réception des enfants d'un autre conteneur:
		bool accueil_enfants(dksConteneur* contSource)
		{
		    uint32_t nbrEnfants=contSource->renvoie_nbr_enfants();
			if ( nbrEnfants > 0)
			{
				for ( uint32_t i = 0 ; i < nbrEnfants ; i++)
				{
					contSource->enfants[i]->parent = this;
					contSource->enfants[i]->a_ete_deplace();
					enfants.push_back(contSource->enfants[i]);
				}
				contSource->vide_enfants();
			}
			return true;
		}

		//Réception des enfants d'un autre conteneur:
		//Les coordonnées absolues sont conservées. Cela perme de changer de parent sans saut de repère apparant.
		bool accueil_enfants_absolus(dksConteneur* contSource)
		{
		    uint32_t nbrEnfants=contSource->renvoie_nbr_enfants();

			if (nbrEnfants > 0)
			{
				for (uint32_t i = 0; i < nbrEnfants; i++)
				{
					contSource->enfants[i]->determine_repere_relatif(this);
					contSource->enfants[i]->parent = this;
					enfants.push_back(contSource->enfants[i]);
				}
				contSource->vide_enfants();
            }
            return true;

		}

		//Ajoute un enfant.
		//Test si l'enfant appartient déjà à la liste des enfants.
		//L'enfant est supprimé de la liste de son ancien parent.
		bool ajoute_enfant(dksConteneur* enfant)
		{
			for (uint32_t i = 0; i < enfants.size(); i++)
			{
				if (enfant == enfants[i])  return false;
			}

			if (enfant->parent!=NULL)enfant->parent->supprime_enfant(enfant);

			enfants.push_back(enfant);
			enfant->parent = this;
			enfant->a_ete_deplace();
			return true;
		}

		//Supprime un enfant.
		//! Le parent de l'enfant est mis à null. !
		// La fonction qui appelle cette méthode doit se dépêcher d'attribuer un nouveau parent.
		bool supprime_enfant(dksConteneur* enfant)
		{
		    vector<dksConteneur*>::iterator iterateur_enfant=enfants.begin();

			for (uint32_t i = 0; i < enfants.size(); i++)
			{
				if (enfant == *iterateur_enfant)
                {
                    enfants.erase(iterateur_enfant);
                    enfant->parent = NULL;
                    break;
                }
                iterateur_enfant++;
			}
			return true;

		}

		//Supprime un enfant.
		//L'enfant garde l'échelle et le repère absolus hérité de son ancien parent.
		//!!!!! Le parent de l'enfant est mis à null. !!!!!!!!
		// La fonction qui appelle cette méthode doit se dépêcher d'attribuer un nouveau parent.
		//Renvoie false si l'enfant n'appartient pas à la liste des enfants,ou si une erreur s'est produite.
		bool supprime_enfant_absolu(dksConteneur* enfant)
		{
		    vector<dksConteneur*>::iterator iterateur_enfant=enfants.begin();

			for (uint32_t i = 0; i < enfants.size(); i++)
			{
				if (enfant == *iterateur_enfant)
				{
                    enfants.erase(iterateur_enfant);
					enfant->echelle_x *= repere_absolu->echelle_x;
					enfant->echelle_y *= repere_absolu->echelle_y;
					enfant->echelle_z *= repere_absolu->echelle_z;
					enfant->parent = NULL;
					return true;
				}
				iterateur_enfant++;
			}
			return false;

		}

		//Ajoute un enfant sans changer son repère absolu.
		//Renvoie false si l'enfant appartient déjà à la liste des enfants ou si une erreur s'est produite.
		//IMPORTANT POUR LES CHANGEMENTS DYNAMIQUES DE HIERARCHIE:
		//Le conteneur n'a pas besoin d'être affecté à un parent car les calculs
		//S'effectuent avec le repère absolu. Il faut juste que le conteneur soit situé dans
		//l'espace absolu.
		bool ajoute_enfant_absolu(dksConteneur* enfant)
		{
			if (enfant->parent == this) return false;

			enfant->echelle_x /= repere_absolu->echelle_x;
			enfant->echelle_y /= repere_absolu->echelle_y;
			enfant->echelle_z /= repere_absolu->echelle_z;
			enfant->determine_repere_relatif(this);

			enfants.push_back(enfant);
			enfant->parent = this;
			return true;
		}



    private:
		//Détermine le repère relatif du conteneur par rapport à un autre conteneur, qui n'est pas encore son parent.
		//Cette méthode sert pour changer de parent sans changer de position absolue
		void determine_repere_relatif(dksConteneur* cont)
		{
			dksRepere::determine_repere_relatif(repere_absolu, cont->repere_absolu, this);
			a_ete_deplace();
		}
    public:
		//Calcul une position relative au même niveau hiérarchique que le conteneur par rapport à une position absolue.
		//Cette fonction sert notamment pour calculer une nouvelle position du conteneur avant d'effectivement changer
		//sa position.
		void calcul_position_relative(dksVecteur* position_absolue, dksVecteur* position_relative)
		{
			dksRepere::point_relatif(position_absolue, parent->repere_absolu, position_relative);
		}



        //Initialise le repère absolu avant la propagation de la hiérarchie:
        void initialisation_repere(dksRepere* referent=NULL)
		{
		    if (referent==NULL)
		    {
		        repere_absolu->x = x*echelle_x;
                repere_absolu->y = y*echelle_y;
                repere_absolu->z = z*echelle_z;
                repere_absolu->echelle_x = echelle_x;
                repere_absolu->echelle_y = echelle_y;
                repere_absolu->echelle_z = echelle_z;
		    }
            else
            {
                repere_absolu->x = x*referent->echelle_x;
                repere_absolu->y = y*referent->echelle_y;
                repere_absolu->z = z*referent->echelle_z;
                repere_absolu->echelle_x = echelle_x*referent->echelle_x;
                repere_absolu->echelle_y = echelle_y*referent->echelle_y;
                repere_absolu->echelle_z = echelle_z*referent->echelle_z;
            }


			dksVecteur* repAxe = repere_absolu->axe_x;
			repAxe->x = axe_x->x;	//*echelle_x;
			repAxe->y = axe_x->y;	//*echelle_x;
			repAxe->z = axe_x->z;	//*echelle_x;

			repAxe = repere_absolu->axe_y;
			repAxe->x = axe_y->x;	//*echelle_y;
			repAxe->y = axe_y->y;	//*echelle_y;
			repAxe->z = axe_y->z;	//*echelle_y;

			repAxe = repere_absolu->axe_z;
			repAxe->x = axe_z->x;	//*echelle_z;
			repAxe->y = axe_z->y;	//*echelle_z;
			repAxe->z = axe_z->z;	//*echelle_z;

            uint32_t nbrEnfants=enfants.size();
			for (uint32_t i = 0; i < nbrEnfants; i++)
			{ enfants[i]->initialisation_repere(repere_absolu); }
		}

        //-------------------- Transforme les repères des enfants:

		void propagation_hierarchie()
		{
			a_change = false;
            if (enfants.empty())return;

			dksConteneur* fiston;
			dksRepere* rep_fis;

			dksVecteur* fiston_axe_x;
			dksVecteur* fiston_axe_y;
			dksVecteur* fiston_axe_z;

			double fiston_abs_x;
			double fiston_abs_y;
			double fiston_abs_z;

			double rep_abs_axe_x_x = repere_absolu->axe_x->x;
			double rep_abs_axe_x_y = repere_absolu->axe_x->y;
			double rep_abs_axe_x_z = repere_absolu->axe_x->z;

			double rep_abs_axe_y_x = repere_absolu->axe_y->x;
			double rep_abs_axe_y_y = repere_absolu->axe_y->y;
			double rep_abs_axe_y_z = repere_absolu->axe_y->z;

			double rep_abs_axe_z_x = repere_absolu->axe_z->x;
			double rep_abs_axe_z_y = repere_absolu->axe_z->y;
			double rep_abs_axe_z_z = repere_absolu->axe_z->z;

			double rep_abs_x = repere_absolu->x;
			double rep_abs_y = repere_absolu->y;
			double rep_abs_z = repere_absolu->z;

			double intermediaire_x;
			double intermediaire_y;

            uint32_t nbrEnfants=enfants.size();

			for (uint32_t i = 0; i < nbrEnfants; i++)
			{
				fiston = enfants[i];
				rep_fis = fiston->repere_absolu;

				fiston->a_change_par_propagation = true;

				//rep_fis.echelle_x += echelle_x;
				//rep_fis.echelle_y += echelle_y;
				//rep_fis.echelle_z += echelle_z;

				fiston_abs_x = rep_fis->x;
				fiston_abs_y = rep_fis->y;
				fiston_abs_z = rep_fis->z;

				fiston_axe_x = rep_fis->axe_x;
				fiston_axe_y = rep_fis->axe_y;
				fiston_axe_z = rep_fis->axe_z;

				rep_fis->x = rep_abs_x + fiston_abs_x * rep_abs_axe_x_x +  fiston_abs_y * rep_abs_axe_y_x + fiston_abs_z * rep_abs_axe_z_x;
				rep_fis->y = rep_abs_y + fiston_abs_x * rep_abs_axe_x_y +  fiston_abs_y * rep_abs_axe_y_y + fiston_abs_z * rep_abs_axe_z_y;
				rep_fis->z = rep_abs_z + fiston_abs_x * rep_abs_axe_x_z +  fiston_abs_y * rep_abs_axe_y_z + fiston_abs_z * rep_abs_axe_z_z;

				intermediaire_x = fiston_axe_x->x * rep_abs_axe_x_x + fiston_axe_x->y * rep_abs_axe_y_x + fiston_axe_x->z * rep_abs_axe_z_x;
				intermediaire_y = fiston_axe_x->x * rep_abs_axe_x_y + fiston_axe_x->y * rep_abs_axe_y_y + fiston_axe_x->z * rep_abs_axe_z_y;
				rep_fis->axe_x->z = fiston_axe_x->x * rep_abs_axe_x_z + fiston_axe_x->y * rep_abs_axe_y_z + fiston_axe_x->z * rep_abs_axe_z_z;
				rep_fis->axe_x->x = intermediaire_x;
				rep_fis->axe_x->y = intermediaire_y;

				intermediaire_x = fiston_axe_y->x * rep_abs_axe_x_x + fiston_axe_y->y * rep_abs_axe_y_x + fiston_axe_y->z * rep_abs_axe_z_x;
				intermediaire_y = fiston_axe_y->x * rep_abs_axe_x_y + fiston_axe_y->y * rep_abs_axe_y_y + fiston_axe_y->z * rep_abs_axe_z_y;
				rep_fis->axe_y->z = fiston_axe_y->x * rep_abs_axe_x_z + fiston_axe_y->y * rep_abs_axe_y_z + fiston_axe_y->z * rep_abs_axe_z_z;
				rep_fis->axe_y->x = intermediaire_x;
				rep_fis->axe_y->y	= intermediaire_y;

				intermediaire_x = fiston_axe_z->x * rep_abs_axe_x_x + fiston_axe_z->y * rep_abs_axe_y_x + fiston_axe_z->z * rep_abs_axe_z_x;
				intermediaire_y = fiston_axe_z->x * rep_abs_axe_x_y + fiston_axe_z->y * rep_abs_axe_y_y + fiston_axe_z->z * rep_abs_axe_z_y;
				rep_fis->axe_z->z = fiston_axe_z->x * rep_abs_axe_x_z + fiston_axe_z->y * rep_abs_axe_y_z + fiston_axe_z->z * rep_abs_axe_z_z;
				rep_fis->axe_z->x = intermediaire_x;
				rep_fis->axe_z->y = intermediaire_y;

                fiston->repere_absolu->maj_axes_echelles();

				fiston->propagation_hierarchie();
			}

		}

        //----------------------------------------------------
        //      Calcul le repère absolu d'un conteneur, et propage
        //      la hiérarchie aux enfants
        //----------------------------------------------------
        void calcul_repere_absolu()
        {
            if (parent!= NULL)
            {
                initialisation_repere(parent->repere_absolu);
                dksRepere::translation_repere(parent->repere_absolu,repere_absolu);
            }
            else initialisation_repere(NULL);

            propagation_hierarchie();
        }

        //--------- Mise à jour de la fonction de mise à l'échelle du repère:

		virtual void determine_echelle_repere(double ex, double ey, double ez)
		{
			echelle_x = ex; echelle_y = ey; echelle_z = ez;
			a_change = true;
		}

		//-------------------------------------------------------------------------------------------------
		//      Calcul la matrice de transformation correspondant au repère absolu
		//      Si le repère de référence est NULL, le repère (1,0,0)(0,1,0)(0,0,1) est utilisé
		/// La matrice courante d'OpenGl doit être la vue du modèle (MODELVIEW). La fonction affecte cette matrice.
		//-------------------------------------------------------------------------------------------------
        void calcul_matrice(dksRepere* repere_espace=NULL)
        {
            if(repere_espace==NULL)repere_espace=&repere_reference;
            glLoadIdentity();
            axe_rotation_matrice.calcul_rotation_entre_deux_reperes(repere_espace,repere_absolu);
            axe_rotation_matrice.calcul_rotation_entre_deux_reperes(repere_espace,repere_absolu);
            glTranslated(repere_absolu->x,repere_absolu->y,repere_absolu->z);
            glRotated(axe_rotation_matrice.angle*180./M_PI,axe_rotation_matrice.x,axe_rotation_matrice.y,axe_rotation_matrice.z);
            glScaled(repere_absolu->echelle_x,repere_absolu->echelle_y,repere_absolu->echelle_z);
            glGetDoublev(GL_MODELVIEW_MATRIX,matrice.matrice);

        }

        //-------------------------------------------------------------------------------------------------
		//      Calcul la matrice de transformation correspondant au repère relatif
		//      Cette fonction sert pour les volumes qui ne sont pas répertoriés dans la table de la scène (caractères des textes 3d)...
		/// La matrice courante d'OpenGl doit être la vue du modèle (MODELVIEW). La fonction affecte cette matrice.
		//-------------------------------------------------------------------------------------------------
        void calcul_matrice_relative(dksRepere* repere_parent=NULL)
        {
            if(repere_parent==NULL)repere_parent=&repere_reference;
            glLoadIdentity();
            axe_rotation_matrice.calcul_rotation_entre_deux_reperes(repere_parent,this);
            axe_rotation_matrice.calcul_rotation_entre_deux_reperes(repere_parent,this);
            glTranslated(x,y,z);
            glRotated(axe_rotation_matrice.angle*180./M_PI,axe_rotation_matrice.x,axe_rotation_matrice.y,axe_rotation_matrice.z);
            glScaled(echelle_x,echelle_y,echelle_z);
            glGetDoublev(GL_MODELVIEW_MATRIX,matrice.matrice);

        }

        //===================================================================================================================
		//============================== Fonctions de gestion des mouvements simples =================================
		//===================================================================================================================
        void determine_axe_vertical(double ax,double ay,double az)
        {
            axe_vertical->x=ax;
            axe_vertical->y=ay;
            axe_vertical->z=az;
            axe_vertical->normalise();
        }

		virtual void a_ete_deplace()
		{
			a_change = true;
		}

        void determine_contrainte_verticale(bool d)
        {
            drapeau_contrainte_verticale=d;
        }

        void determine_contrainte_roulis_rotation(bool d)
        {
            drapeau_contrainte_roulis_rotation=d;
        }

        //Rotation brute, d'un angle alpha autour d'un axe:
		virtual void rotation(double angle,double rx,double ry,double rz)
		{
		    if (drapeau_contrainte_verticale) rotation_contrainte_verticale(angle,rx,ry,rz);
		    else this->dksRepere::rotation(angle,rx,ry,rz);
			a_ete_deplace();
		}

		//Rotation brute, d'un angle alpha autour d'un axe:
		virtual void rotation(double angle,dksVecteurUnitaire* axe)
		{
		    rotation(angle,axe->x,axe->y,axe->z);
		}

		//Rotation avec maintien de la verticalité (axe_x toujours horizontal)
		virtual void rotation_contrainte_verticale(double angle,double rx,double ry,double rz)
		{
		    double axe_z_prec_x=axe_z->x;
		    double axe_z_prec_z=axe_z->z;

		    axe_z->rotation(angle,rx,ry,rz);

		    if (axe_z->y==1.)
		    {
		        axe_x->rotation(angle,rx,ry,rz);
		        axe_y->rotation(angle,rx,ry,rz);
		    }
		    else
		    {
                axe_x->x=axe_vertical->y*axe_z->z-axe_vertical->z*axe_z->y;
                axe_x->y=axe_vertical->z*axe_z->x-axe_vertical->x*axe_z->z;
                axe_x->z=axe_vertical->x*axe_z->y-axe_vertical->y*axe_z->x;
                axe_x->normalise();
                axe_y->x=axe_z->y*axe_x->z-axe_z->z*axe_x->y;
                axe_y->y=axe_z->z*axe_x->x-axe_z->x*axe_x->z;
                axe_y->z=axe_z->x*axe_x->y-axe_z->y*axe_x->x;
		    }


		    if(drapeau_contrainte_roulis_rotation && axe_z->y<1.)
		    {
		        //Ne tient compte que de la rotation de l'axe Z sur la plan (X,Z)
		        double n1=sqrt(axe_z_prec_x*axe_z_prec_x+axe_z_prec_z*axe_z_prec_z);
		        double n2=sqrt(axe_z->x*axe_z->x+axe_z->z*axe_z->z);
		        double axe_z_x=axe_z->x/n2;
		        double axe_z_z=axe_z->z/n2;
		        axe_z_prec_x/=n1;
		        axe_z_prec_z/=n1;
		        double sin_angle=axe_z_prec_z*axe_z_x-axe_z_prec_x*axe_z_z;
		        double cos_angle=axe_z_prec_x*axe_z_x+axe_z_prec_z*axe_z_z;
		        double angle=asin(sin_angle); //Angle en radians
		        if(cos_angle<0.)
		        {
		            if(angle<0.) angle=-M_PI-angle;
		            else angle=M_PI-angle;
		        }
		        angle*=-inertie_contrainte_roulis_rotation;
		        axe_x->rotation(angle,axe_z);
		        axe_y->rotation(angle,axe_z);

		    }
		    a_ete_deplace();
		}

        virtual void determine_RTL(double p_roulis, double p_tangage, double p_lacet)
		{
            angle_roulis=p_roulis;
            angle_tangage=p_tangage;
            angle_lacet=p_lacet;
            maj_orientation_RTL();
            a_ete_deplace();
		}

		virtual void determine_position(double px, double py, double pz)
		{
			//Alignement à la trajectoire:
			if (contrainte_trajectoire) alignement_tangente_trajectoire(px,py,pz);

			x = px; y = py; z = pz;
			a_ete_deplace();
		}

		virtual void determine_position(float* position)
		{
			determine_position(double(position[0]),double(position[1]),double(position[2]));
		}

		virtual void determine_position(double* position)
		{
			determine_position(position[0],position[1],position[2]);
		}

		virtual void copie_repere(dksRepere* repere_source)
		{
		    this->dksRepere::copie_repere(repere_source);
		    a_ete_deplace();
		}

		virtual void copie_axes(dksRepere* repere_source)
		{
		    this->dksRepere::copie_axes(repere_source);
		    a_ete_deplace();
		}

        //==========================================================================================================================
		//============================== Fonctions de gestion des mouvements un peu plus complexes =================================
		//==========================================================================================================================


		// les valeurs sont des angles par rapport à la direction actuelle.
		void vise(double angle, uint8_t axe)
		{

			if (masse != 0)
			{
				visee_activee = true;
				angle_restant = angle;
				determine_propulsion_angulaire(axe);
			}
			else
			{
				if (axe==AVANT || axe==ARRIERE)
                {
                    axe_y->rotation(angle, axe_x);
                    axe_z->rotation(angle, axe_x);
                }

				else if (axe==GAUCHE || axe==DROITE)
                {
                    axe_x->rotation(angle, axe_y);
                    axe_z->rotation(angle, axe_y);
                }

				else if (axe==HAUT || axe==BAS)
                {
                    axe_x->rotation(angle, axe_z);
                    axe_y->rotation(angle, axe_z);
                }
			}
			a_ete_deplace();
		}


        //Aligne l'axe Z au sens du déplacement
		virtual void alignement_tangente_trajectoire(double px,double py,double pz)
		{
			double tangente_x = px - x;
			double tangente_y = py - y;
			double tangente_z = pz - z;

			double norme = sqrt(tangente_x * tangente_x + tangente_y * tangente_y + tangente_z * tangente_z);

			if ( norme>0.05 )
			{
				tangente_x /= norme;
				tangente_y /= norme;
				tangente_z /= norme;

				axe->x = axe_z->y * tangente_z - tangente_y * axe_z->z;
				axe->y = axe_z->z * tangente_x - tangente_z * axe_z->x;
				axe->z = axe_z->x * tangente_y - tangente_x * axe_z->y;

				double sinus = axe->norme(); //

				double cosinus = axe_z->x * tangente_x + axe_z->y * tangente_y + axe_z->z * tangente_z;

				if ( !((norme<DKSCONTENEUR_EPSILON) && (norme>-DKSCONTENEUR_EPSILON)) )
				{
					double angle = asin(sinus);
					if (cosinus < 0) angle = M_PI - angle; //Tenir compte des angles > 90°

					axe->x /= sinus;
					axe->y /= sinus;
					axe->z /= sinus;

					rotation(angle, axe);
				}

				else if ((cosinus<(-1.+DKSCONTENEUR_EPSILON))&&(cosinus>(-1.-DKSCONTENEUR_EPSILON)))
				{
					rotation(M_PI, axe_x);
				}
			}
		}

		//Déplacement selon les axes du repère:
		virtual void deplace(uint8_t dir,double quantite=0.2)
		{
			//Dans le cas d'un déplacement sans inertie:

            a_ete_deplace();

            //Deplacement sans inertie:
            if (masse==0)
            {
                switch (dir)
                {
                    case AVANT:
                        x += quantite * axe_z->x;
                        y += quantite * axe_z->y;
                        z += quantite * axe_z->z;
                    break;

                    case ARRIERE:
                        x += -quantite * axe_z->x;
                        y += -quantite * axe_z->y;
                        z += -quantite * axe_z->z;
                    break;

                    case GAUCHE:
                        x += quantite * axe_x->x;
                        y += quantite * axe_x->y;
                        z += quantite * axe_x->z;
                    break;

                    case DROITE:
                        x += -quantite * axe_x->x;
                        y += -quantite * axe_x->y;
                        z += -quantite * axe_x->z;
                    break;

                    case HAUT:
                        x += quantite * axe_y->x;
                        y += quantite * axe_y->y;
                        z += quantite * axe_y->z;
                    break;

                    case BAS:
                        x += -quantite * axe_y->x;
                        y += -quantite * axe_y->y;
                        z += -quantite * axe_y->z;
                    break;
                }
            }
            //Déplacement avec inertie:
            else
            {
                determine_propulsion(dir);
            }


		}


		virtual void tourne(uint8_t axe, double quantite=M_PI/180.)
		{
			if (masse != 0)
			{
				determine_propulsion_angulaire(axe);
			}
			else
			{
                switch (axe)
				{
					case (AVANT):
						//axe_y->rotation(quantite, axe_z);
						//axe_x->rotation(quantite, axe_z);
						rotation(quantite,axe_z);
					break;

					case (GAUCHE ):
						//axe_y->rotation(quantite, axe_x);
						//axe_z->rotation(quantite, axe_x);
						rotation(quantite,axe_x);
					break;

					case (HAUT ):
						//axe_x->rotation(quantite, axe_y);
						//axe_z->rotation(quantite, axe_y);
						rotation(quantite,axe_y);
					break;

					case (ARRIERE):
						//axe_y->rotation(-quantite, axe_z);
						//axe_x->rotation(-quantite, axe_z);
						rotation(-quantite,axe_z);
					break;

					case (DROITE ):
						//axe_y->rotation(-quantite, axe_x);
						//axe_z->rotation(-quantite, axe_x);
						rotation(-quantite,axe_x);
					break;

					case (BAS ):
						//axe_x->rotation(-quantite, axe_y);
						//axe_z->rotation(-quantite, axe_y);
						rotation(-quantite,axe_y);
					break;

					case BAS_ABSOLU:
						rotation(-quantite, axe_vertical);
					break;

					case HAUT_ABSOLU:
						rotation(quantite, axe_vertical);
					break;
				}

			}
			a_ete_deplace();
		}

        //----------------------------------------------------
		//Mouvements propres du conteneur, à overrider:
        //----------------------------------------------------
		virtual void applique_mouvements()
		{
		    if (delta_t<1e-6) {return;}
			if (masse != 0.)
			{
				rafraichi_cinetique();
			}
		}


        //--------------------------------------------------------------------------------
        //          Poursuite d'un point
        //
        //--------------------------------------------------------------------------------
        void maj_poursuite_point(double* point_cible)
        {
           double vx=point_cible[0]-x;
           double vy=point_cible[1]-y;
           double vz=point_cible[2]-z;
           double distance=vx*vx+vy*vy+vz*vz;
           if(distance<0.1) return;
           distance=sqrt(distance);
           if(distance==distance_poursuite_min) return;

           double vdx=(vx-(vx/distance*distance_poursuite_min))/inertie_poursuite;
           double vdy=(vy-(vy/distance*distance_poursuite_min))/inertie_poursuite;
           double vdz=(vz-(vz/distance*distance_poursuite_min))/inertie_poursuite;

            x+=vdx;
            y+=vdy;
            z+=vdz;

            a_ete_deplace();
        }


        //------------------------------------------------------------------------------
        //      Mise à jour du repère absolu et de la matrice.
        /// A n'utiliser que lorsque le conteneur n'est pas un élément d'une scène.
        //------------------------------------------------------------------------------
        virtual void maj(double t)
        {
           // position_temps=t;
           // delta_t=t-t0;

            applique_mouvements();
            calcul_repere_absolu();

            glPushMatrix();
            calcul_matrice();
            glPopMatrix();

            //t0=t;
        }

        //===============================================================================
		//              Gestion de la cinétique
        //===============================================================================

        //-------- Calcul des mouvements instantanés:
		void rafraichi_cinetique()
		{
            memorise_cinetique_t0();
            interpolation=1.0;
			rafraichi_vitesse();
			rafraichi_position();
			rafraichi_vitesse_circulaire();
			rafraichi_rotation();
		}

        //-------- Détermine le coefficient de frottements:
		void determine_coefficient_frottement(double vlim,double valeur_propulsion)
		{
			coefficient_frottement = valeur_propulsion / vlim;
		}

        //-------- Détermine la propulsion, selon les axes du repère:
		void determine_propulsion(uint8_t dir)
		{
		    contrainte_direction_propulsion=true;

			switch (dir)
			{
				case AVANT:
                    valeur_propulsion_lineaire_courante=valeur_propulsion_horizontale;
					direction_propulsion=axe_z;
				break;

				case ARRIERE:
                    valeur_propulsion_lineaire_courante=-valeur_propulsion_horizontale;
					direction_propulsion=axe_z;
				break;

				case HAUT:
					valeur_propulsion_lineaire_courante=valeur_propulsion_horizontale;
					direction_propulsion=axe_y;
				break;

				case BAS:
					valeur_propulsion_lineaire_courante=-valeur_propulsion_horizontale;
					direction_propulsion=axe_y;
				break;

				case GAUCHE:
					valeur_propulsion_lineaire_courante=valeur_propulsion_horizontale;
					direction_propulsion=axe_x;
				break;

				case DROITE:
					valeur_propulsion_lineaire_courante=-valeur_propulsion_horizontale;
					direction_propulsion=axe_x;
				break;
			}
			propulsion->produit_vecteur_reel(direction_propulsion,valeur_propulsion_lineaire_courante);
		}

		void stop_propulsion()
		{
			propulsion->x = 0.;
			propulsion->y = 0.;
			propulsion->z = 0.;
			contrainte_direction_propulsion=false;
		}

		void determine_propulsion_angulaire(uint8_t dir)
		{
			switch (dir)
			{
				case AVANT:
					propulsion_angulaire->produit_vecteur_reel(axe_z,valeur_propulsion_angulaire);
				break;

				case ARRIERE:
					propulsion_angulaire->produit_vecteur_reel(axe_z,-valeur_propulsion_angulaire);
				break;

				case HAUT:
					propulsion_angulaire->produit_vecteur_reel(axe_y,valeur_propulsion_angulaire);
				break;

				case BAS:
					propulsion_angulaire->produit_vecteur_reel(axe_y,-valeur_propulsion_angulaire);
				break;

				case GAUCHE:
					propulsion_angulaire->produit_vecteur_reel(axe_x,valeur_propulsion_angulaire);
				break;

				case DROITE:
					propulsion_angulaire->produit_vecteur_reel(axe_x,-valeur_propulsion_angulaire);
				break;

				case HAUT_ABSOLU:
					propulsion_angulaire->produit_vecteur_reel(axe_vertical,valeur_propulsion_angulaire);
				break;

				case BAS_ABSOLU:
					propulsion_angulaire->produit_vecteur_reel(axe_vertical,-valeur_propulsion_angulaire);
				break;
			}
		}

		void stop_propulsion_angulaire()
		{
			propulsion_angulaire->x = 0.;
			propulsion_angulaire->y = 0.;
			propulsion_angulaire->z = 0.;
		}

        //Mémorise le repère avant l'application du mouvement:
        //Sert surtout lors de la visée
		void memorise_cinetique_t0()
		{
		    repere_t0.x=x;
		    repere_t0.y=y;
		    repere_t0.z=z;

		    repere_t0.axe_x->x=axe_x->x;
		    repere_t0.axe_x->y=axe_x->y;
		    repere_t0.axe_x->z=axe_x->z;

		    repere_t0.axe_y->x=axe_y->x;
		    repere_t0.axe_y->y=axe_y->y;
		    repere_t0.axe_y->z=axe_y->z;

		    repere_t0.axe_z->x=axe_z->x;
		    repere_t0.axe_z->y=axe_z->y;
		    repere_t0.axe_z->z=axe_z->z;

		    repere_t0.echelle_x=echelle_x;
		    repere_t0.echelle_y=echelle_y;
		    repere_t0.echelle_z=echelle_z;

            angle_restant_t0=angle_restant;
            visee_activee_t0=visee_activee;

            propulsion_t0.x=propulsion->x;
            propulsion_t0.y=propulsion->y;
            propulsion_t0.z=propulsion->z;

            propulsion_angulaire_t0.x=propulsion_angulaire->x;
            propulsion_angulaire_t0.y=propulsion_angulaire->y;
            propulsion_angulaire_t0.z=propulsion_angulaire->z;

		}



		//Calcul la vitesse instantannée.
    protected:
		void rafraichi_vitesse()
		{
		    if (vitesse->est_nul() && propulsion->est_nul() && propulsion_verticale == 0) return;

            //Aligne la propulsion à la direction relative au conteneur:
			if (contrainte_direction_propulsion && !propulsion->est_nul() && direction_propulsion!=NULL)
			{
			    propulsion->produit_vecteur_reel(direction_propulsion,valeur_propulsion_lineaire_courante);
			}

            //Calcul la vitesse:
			vitesse->x += delta_t * ( -coefficient_frottement / masse * vitesse->x + propulsion->x / masse);
			vitesse->y += delta_t * ( -coefficient_frottement / masse * vitesse->y + (propulsion->y+ propulsion_verticale) / masse);
			vitesse->z += delta_t * ( -coefficient_frottement / masse * vitesse->z + propulsion->z / masse);

			if (vitesse->norme_carre() < DKSCONTENEUR_EPSILON) vitesse->vecteur_nul();
			else { a_ete_deplace(); }
		}


        //Calcul la position instantannée:
		void rafraichi_position()
		{
            x = repere_t0.x + vitesse->x * delta_t * interpolation;
            y = repere_t0.y + vitesse->y * delta_t * interpolation;
            z = repere_t0.z + vitesse->z * delta_t * interpolation;
		}

		void rafraichi_position(dksRepere* repere_temp,double p_interpolation)
		{
            repere_temp->x = repere_t0.x + vitesse->x * delta_t * p_interpolation;
            repere_temp->y = repere_t0.y + vitesse->y * delta_t * p_interpolation;
            repere_temp->z = repere_t0.z + vitesse->z * delta_t * p_interpolation;
		}

		//Calcul la vitesse instantannée.
		void rafraichi_vitesse_circulaire()
		{
		    if (vitesse_circulaire->est_nul() && propulsion_angulaire->est_nul()) return;

			vitesse_circulaire->x += delta_t * ( -coefficient_frottement / masse * vitesse_circulaire->x + propulsion_angulaire->x / masse);
			vitesse_circulaire->y += delta_t * ( -coefficient_frottement / masse * vitesse_circulaire->y + propulsion_angulaire->y / masse);
			vitesse_circulaire->z += delta_t * ( -coefficient_frottement / masse * vitesse_circulaire->z + propulsion_angulaire->z / masse);

			if (vitesse_circulaire->norme_carre() < DKSCONTENEUR_EPSILON) vitesse_circulaire->vecteur_nul();
			else { a_ete_deplace(); }

			//trace(vitesse_circulaire);
		}

        //Calcul la rotation à l'instant t0+(t1-t0)*interpolation:

		void rafraichi_rotation()
		{
		    /*
		    rafraichi_rotation((dksRepere*)this,interpolation);
		}

		void rafraichi_rotation(dksRepere* repere_temp,double p_interpolation)
		{
		    if (repere_temp==NULL)repere_temp=(dksRepere*)this;

		    repere_temp->axe_x->determine(repere_t0.axe_x->x,repere_t0.axe_x->y,repere_t0.axe_x->z);
			repere_temp->axe_y->determine(repere_t0.axe_y->x,repere_t0.axe_y->y,repere_t0.axe_y->z);
			repere_temp->axe_z->determine(repere_t0.axe_z->x,repere_t0.axe_z->y,repere_t0.axe_z->z);

			//Distance d'application de la force pour l'instant on considère que la force s'applique à 1 m du centre de gravité.
			double rayon = 1.;
			double norme=vitesse_circulaire->norme();

			if (norme < DKSCONTENEUR_EPSILON) return;

			repere_temp->axe_x->determine(repere_t0.axe_x->x,repere_t0.axe_x->y,repere_t0.axe_x->z);
			repere_temp->axe_y->determine(repere_t0.axe_y->x,repere_t0.axe_y->y,repere_t0.axe_y->z);
			repere_temp->axe_z->determine(repere_t0.axe_z->x,repere_t0.axe_z->y,repere_t0.axe_z->z);

			axe_rotation->x = vitesse_circulaire->x / norme;
			axe_rotation->y = vitesse_circulaire->y / norme;
			axe_rotation->z = vitesse_circulaire->z / norme;

			angle_rotation_instantanne = norme * delta_t * p_interpolation / rayon;

			repere_temp->axe_x->rotation(angle_rotation_instantanne, axe_rotation);
			repere_temp->axe_y->rotation(angle_rotation_instantanne, axe_rotation);
			repere_temp->axe_z->rotation(angle_rotation_instantanne, axe_rotation);
            */

            axe_x->determine(repere_t0.axe_x->x,repere_t0.axe_x->y,repere_t0.axe_x->z);
			axe_y->determine(repere_t0.axe_y->x,repere_t0.axe_y->y,repere_t0.axe_y->z);
			axe_z->determine(repere_t0.axe_z->x,repere_t0.axe_z->y,repere_t0.axe_z->z);

			//Distance d'application de la force pour l'instant on considère que la force s'applique à 1 m du centre de gravité.
			double rayon = 1.;
			double norme=vitesse_circulaire->norme();

			if (norme < DKSCONTENEUR_EPSILON) return;

			axe_x->determine(repere_t0.axe_x->x,repere_t0.axe_x->y,repere_t0.axe_x->z);
			axe_y->determine(repere_t0.axe_y->x,repere_t0.axe_y->y,repere_t0.axe_y->z);
			axe_z->determine(repere_t0.axe_z->x,repere_t0.axe_z->y,repere_t0.axe_z->z);

			axe_rotation->x = vitesse_circulaire->x / norme;
			axe_rotation->y = vitesse_circulaire->y / norme;
			axe_rotation->z = vitesse_circulaire->z / norme;

			angle_rotation_instantanne = norme * delta_t * interpolation / rayon;

			rotation(angle_rotation_instantanne, axe_rotation);

            /*

			angle_restant=angle_restant_t0;
			visee_activee=visee_activee_t0;

			propulsion_angulaire->x=propulsion_angulaire_t0.x;
			propulsion_angulaire->y=propulsion_angulaire_t0.y;
			propulsion_angulaire->z=propulsion_angulaire_t0.z;

			if (visee_activee)
			{
				double test = angle_restant * rayon - norme * masse / coefficient_frottement;

				angle_restant -= angle_rotation_instantanne;

				//trace(angle_restant, test);

				if (test <= 0)
				{
					stop_propulsion_angulaire();
					visee_activee = false;
				}
			}
            */
			//else
			//{
				//angle_restant -= angle_rotation_instantanne;
				//trace(angle_restant);
			//}

		}

		//***********************************************************************
		//              Calculs pour la gestion des collisions - EN TRAVAUX -
		//***********************************************************************

		//Renvoie le repere du conteneur à l'instant t0+(t1-t0)*p_interpolation:
		//Il faut appeler cette fonction après rafraichi_cinetique()
		//Cette fonction sert au moteur physique pour déterminer une date de collision.
public:
        /*
		void renvoie_repere_interpole(double p_interpolation, dksRepere* repere_interpole)
		{
		    determine_interpolation_instantannee(p_interpolation,repere_interpole);
		    repere_interpole->echelle_x=echelle_x;
		    repere_interpole->echelle_y=echelle_y;
		    repere_interpole->echelle_z=echelle_z;
            if (parent!=NULL)
            {
                parent->renvoie_repere_interpole(p_interpolation,&repere_parent_ti);
                dksRepere::translation_repere(repere_interpole,&repere_parent_ti);
            }
		}


        //Dans le cas d'une collision, permet de calculer le repère à un instant compris entre t0 et t1.
		//Rafraichi directement le repere du conteneur:
		void determine_interpolation_instantannee(double p_interpolation)
		{
            interpolation=p_interpolation;
            rafraichi_position();
            rafraichi_rotation();
		}

        //Rafraichi un repère tempon:
		void determine_interpolation_instantannee(double p_interpolation,dksRepere* repere_temp)
		{
            rafraichi_position(repere_temp,p_interpolation);
            rafraichi_rotation(repere_temp,p_interpolation);
		}
		*/

		//======================================================================================================
		//              Affichages pour l'édition et le debugage
		//======================================================================================================
		public:
		void affiche_position()
		{
		    cout<<"X: "<<x<<" Y: "<<y<<" Z: "<<z<<endl;
		}
		virtual void toString()
		{
		    cout<<"Conteneur: "<<nom<<endl;
		    this->dksRepere::toString();
		}
		virtual void affiche_liste_enfants()
		{
		    cout<<"Enfants du conteneur: "<<nom<<endl;
		    for(uint32_t i=0;i<enfants.size();i++)
		    {
		        cout<<enfants[i]->nom<<endl;
		    }
		}
};


#endif // DKSCONTENEUR_H_INCLUDED
