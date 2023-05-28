#ifndef PARTICULES_H_INCLUDED
#define PARTICULES_H_INCLUDED

#include "../Geometrie_Espace/dksVecteurUnitaire.h"
#include "../Elements_Fondamentaux/dksVolume.h"
#include "../Elements_Fondamentaux/dksOeil.h"
#include "../../Utiles/Bruit.h"
#include "../../gestion_fichiers/gestion_fichiers.h"

//Structure de base des éléments du champ de particules:
class Element_particule
{
    public:
    void* suivante;    //Null si c'est le dernier élément de la liste
    void* precedente;  //Null si c'est le premier élément
    uint32_t point; //L'indice du point
    double vitesse[3];
    double masse;
    double date_creation;   //La date d'activation de la particule.
    double duree_vie;       //La durée de vie en secondes après la date de création
    Bruit* perturbation_x;    //Perturbation des mouvements
    Bruit* perturbation_y;
    Bruit* perturbation_z;
    double amplitude_perlin;
    double intervalle_perlin;


    Element_particule(uint32_t index)
    {
        point=index;
        masse=1.;
        vitesse={0.,0.,0.};
        perturbation_x=NULL;
        perturbation_y=NULL;
        perturbation_z=NULL;
        perturbation_x=new Bruit(); //Prévoir les erreurs...
        perturbation_y=new Bruit();
        perturbation_z=new Bruit();
        amplitude_perlin=2.;
        intervalle_perlin=.1;
        duree_vie=5.;
    }

    ~Element_particule()
    {
        if (perturbation_x!=NULL)delete perturbation_x;
        if (perturbation_y!=NULL)delete perturbation_y;
        if (perturbation_z!=NULL)delete perturbation_z;
    }

};

//Structure utilisée pour le tri des points:
class tri_particules
{   public:
    double distance;        //Distance du point à l'oeil
    uint32_t indice_point;   //L'indice du point
};

//===================================================================================================
//                                 Classe de base des particules
//      Les champs de particules héritent des volumes. Comme ça, on peut les ajouter à la liste
//      d'affichage d'une scène.
//
//      Les points sont dans le repère du conteneur.
//
//===================================================================================================

//============ Erreurs:
#define DKSPARTICULES_OK 0

class dksParticules :public dksVolume
{
    public:
        float epaisseur_points;
        float epaisseur_points_luminescents; //La taille des points lors du rendu des surfaces luminescents
        uint32_t couleur_points;
        bool drapeau_vie_limitee;   //Utilisation des listes
        bool drapeau_absolu;    //True si les points sont défini dans l'espace absolu (false par défaut)

    protected:
        double* points;     //Les coordonnées des particules
        uint32_t nbrPoints;
        Element_particule** particules;   //Les données propres aux particules.

        vector<tri_particules> indices_points;  //Pour le tri des points. Ca peut servir selon l'utilisation des particules.

        Element_particule* particules_actives;  //Pointeur sur la première particule active
        Element_particule* particules_inactives;    //Pointeur sur la première particule inactive

        double temps_precedent; //Sert pour les calculs temporels
        double* vitesses_points;    //Vecteurs V correspondants aux points respectifs
        double* masses_points;    //Vecteurs V correspondants aux points respectifs

        Bruit* perturbation;    //Perturbation globale
        bool drapeau_maj_points_auto;   //True si les paramètres des particules sont appliqués aux points lors de l'appel à "applique_mouvements"

        //-------- Données de gestion du mouvement selon un flux:
        double flux_xmin;
        double flux_xmax;
        double flux_ymin;
        double flux_ymax;
        double flux_zmin;
        double flux_zmax;
        uint8_t flux_direction; //AVANT,ARRIERE,HAUT,BAS,GAUCHE,DROITE  - Les points se déplacent selon les axes du conteneur.

        //=========================================================
        //                  CONSTRUCTEUR
        //=========================================================
    public:
        dksParticules (char* p_nom="Particules",
                    double px=0.,double py=0.,double pz=0., //Centre du champ
                    uint32_t p_nbrPoints=0,
                    dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                    ):dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
        {
            erreur=DKSPARTICULES_OK;
            points=NULL;
            particules=NULL;
            nbrPoints=0;

            epaisseur_points=2.;
            epaisseur_points_luminescents=4.;
            couleur_points=0xffffff;

            perturbation=new Bruit();
            determine_rafraichissement_auto(false);

            determine_nombre_de_points(p_nbrPoints);

            determine_parametres_flux(-10,10,-10,10,-10,10,BAS);

            drapeau_maj_points_auto=false;
            drapeau_vie_limitee=false;
            drapeau_absolu=false;
            desactive_particules();
        }

        //======================================================
        //                   DESTRUCTEUR:
        //======================================================
        //Le destructeur de la classe parent est appelé juste après.
        ~dksParticules()
        {
            indices_points.clear();
            destruction_particules();
            if (points!=NULL) free(points);
            if (particules!=NULL) free(particules);
            if (perturbation!=NULL) delete perturbation;
        }

        //====================================================================================================================
        //Fonctions à écraser par les classes héritières qui utilisent d'autres données pour les particules:
        //====================================================================================================================
        virtual void creation_particules()
        {
            for (uint32_t i=0;i<nbrPoints;i++) particules[i]=new Element_particule(i);

        }

        virtual void destruction_particules()
        {
            for (uint32_t i=0;i<nbrPoints;i++)
            {
                if (particules[i] != NULL) delete particules[i];
            }
        }

        //=============================================
        //         Gestion des points
        //=============================================
        bool determine_nombre_de_points(uint32_t nbr)
        {
            uint32_t i;
            if (points!=NULL) free(points);
            if (particules!=NULL)
            {
                destruction_particules();
                free(particules);
            }
            nbrPoints=nbr;
            if (nbrPoints==0) return true;
            points=(double*)malloc(nbrPoints*3*sizeof(double));


            particules=(Element_particule**)calloc(nbrPoints,sizeof(Element_particule*));

            if (points==NULL || particules==NULL)
            {
                erreur=DKSVOLUME_ERREUR_MEMOIRE;
                cout<<"Erreur dans dksParticule::determine_nombre_de_points() - Mémoire insuffisante"<<endl;
                return false;
            }

            creation_particules();

            //Table de tri:
            indices_points.clear();
            for (i=0;i<nbrPoints;i++)
            {
                ajoute_tri_point();
                indices_points[i].indice_point=i;
            }
            return true;
        }

        private:
        void ajoute_tri_point()
        {
            tri_particules indice_tri;
            indices_points.push_back(indice_tri);
        }


        //------------------------------------------------
        //      Détermine l'échelle géométrique des points
        //------------------------------------------------
        public:
        virtual void determine_echelle_geometrique(double e)
        {
            double ne=e/echelle_geometrique;
            for(uint32_t i=0;i<nbrPoints;i++)
            {
                points[3*i]*=ne;
                points[3*i+1]*=ne;
                points[3*i+2]*=ne;
            }
            echelle_geometrique=e;
        }

        //-------------------------------------------
        //Détermine les coordonnées des points.
        //-------------------------------------------
        public:
        void determine_points(double *p_points,uint32_t p_nbrPoints)
        {
            if ( nbrPoints<p_nbrPoints ) p_nbrPoints=nbrPoints;
            for (uint32_t i=0;i<p_nbrPoints;i++)
            {
                points[3*i]=p_points[3*i];
                points[3*i+1]=p_points[3*i+1];
                points[3*i+2]=p_points[3*i+2];
            }
            a_ete_deplace();
        }

        //N'affecte que les coordonnées X et Z:
        void determine_points_XZ(double p_points[],uint32_t p_nbrPoints)
        {
            if ( nbrPoints<p_nbrPoints ) p_nbrPoints=nbrPoints;
            for (uint32_t i=0;i<p_nbrPoints;i++)
            {
                points[3*i]=p_points[3*i];
                //points[3*i+1]=p_points[3*i+1];
                points[3*i+2]=p_points[3*i+2];
            }
            a_ete_deplace();
        }

        //Détermine les coordonnées d'un point.
        //Renvoie true si tout s'est bien passé.
        bool determine_point(uint32_t indice, double px,double py,double pz)
        {
            if (indice>=nbrPoints) return false;
            points[3*indice]=px;
            points[3*indice+1]=py;
            points[3*indice+2]=pz;
            a_ete_deplace();
            return true;
        }

        //Génère des coordonnées au hazard, dans les limite spécifiées en entrée:
        void random_points(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
        {
            double random_max= double(RAND_MAX);
            uint32_t rand_32;
            for (uint32_t i=0;i<nbrPoints;i++)
            {
                rand_32=(rand()<<15)|(rand());
                points[3*i]=(xmax-xmin)/random_max*double(rand_32)/random_max+xmin;
                rand_32=(rand()<<15)|(rand());
                points[3*i+1]=(ymax-ymin)/random_max*double(rand_32)/random_max+ymin;
                rand_32=(rand()<<15)|(rand());
                points[3*i+2]=(zmax-zmin)/random_max*double(rand_32)/random_max+zmin;

                //cout<<"x: "<<points_relatifs[3*i]<<" y: "<<points_relatifs[3*i+1]<<" z: "<<points_relatifs[3*i+2]<<endl;
            }
            a_ete_deplace();
        }

        void determine_masses(float m)
        {
            for (uint32_t i=0;i < nbrPoints;i++)
            {
                particules[i]->masse=m;
            }
        }

        //Détermine les masses des particules au hazard, dans la fourchette définie en entrée:
        void random_masses(float min,float max)
        {
            for (uint32_t i=0;i < nbrPoints;i++)
            {
                particules[i]->masse=float(rand())/32768.*(max-min)+min;
            }
        }

        //Perturbations avec bruit de Perlin:
        void random_amplitudes_perlin(double pmin,double pmax)
        {
            for (uint32_t i=0;i < nbrPoints;i++)
            {
                particules[i]->amplitude_perlin=float(rand())/32768.*(pmax-pmin)+pmin;
            }
        }

        void random_intervalles_perlin(double imin,double imax)
        {
            for (uint32_t i=0;i < nbrPoints;i++)
            {
                particules[i]->intervalle_perlin=float(rand())/32768.*(imax-imin)+imin;
            }
        }


        //=========================================================================================
        //                          Gestion des mouvements
        //=========================================================================================


		//Déplacement des points:
		void deplace_points(double dx,double dy, double dz, uint32_t premier=0, uint32_t p_nbrPoints=0)
		{
		    p_nbrPoints+=premier;
		    if ( (p_nbrPoints==0) || (p_nbrPoints>nbrPoints) ) p_nbrPoints=nbrPoints;

		   for (uint32_t i=premier ;i<p_nbrPoints;i++)
		   {
		       points[3*i]+=dx;
		       points[3*i+1]+=dy;
		       points[3*i+2]+=dz;
		   }
		   a_ete_deplace();
		}

        //---------------------------------------------------------------------------------
        //  Mis à jour des positions des points en fonction  des paramètres des particules
        //---------------------------------------------------------------------------------
        virtual void maj_positions_points()
        {
            for (uint32_t i=0;i<nbrPoints;i++)
                {
                    points[3*i]+=particules[i]->vitesse[0]*delta_t;
                    points[3*i+1]+=particules[i]->vitesse[1]*delta_t;
                    points[3*i+2]+=particules[i]->vitesse[2]*delta_t;
                }
        }

        virtual void maj_positions_points_perturbation_perlin()
        {
            double px,py,pz;
            for (uint32_t i=0;i<nbrPoints;i++)
                {
                    Element_particule* particule=particules[i];
                    px=particule->amplitude_perlin*(particule->perturbation_x->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    py=particule->amplitude_perlin*(particule->perturbation_y->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    pz=particule->amplitude_perlin*(particule->perturbation_z->valeur_aleatoire_temporelle(position_temps,particule->intervalle_perlin)*2.-1.);
                    points[3*i]+=(px+particule->vitesse[0])*delta_t;
                    points[3*i+1]+=(py+particule->vitesse[1])*delta_t;
                    points[3*i+2]+=(pz+particule->vitesse[2])*delta_t;
                }
        }


		double* renvoie_vitesses()
		{
		    return vitesses_points;
		}

        double* renvoie_points()
        {
            return points;
        }

        virtual Element_particule* renvoie_particule(uint32_t i)
        {
            return particules[i];
        }

		uint32_t renvoie_nombre_de_points()
		{
		    return nbrPoints;
		}

        void determine_rafraichissement_auto(bool drapeau)
        {
            drapeau_maj_points_auto=drapeau;
        }

		virtual void applique_mouvements()
		{
		    dksVolume::applique_mouvements();

		    if (delta_t<1e-6) return;

            if (drapeau_maj_points_auto) maj_positions_points();

		}

		//----------------------------------------------------------------------
		//                  Mouvement des points par un centre de gravité
		/// Appeler APRES la mise à jour de la scène
		//----------------------------------------------------------------------

		void maj_gravite(double centre[3],double acceleration,double frottements=1.,double distance_min=1.)
        {
            uint32_t i=0;
            double mo[3];
            double distance;
            for(i=0;i<nbrPoints;i++)
            {
                mo[0]=centre[0]-points[3*i];
                mo[1]=centre[1]-points[3*i+1];
                mo[2]=centre[2]-points[3*i+2];
                distance=sqrt(mo[0]*mo[0]+mo[1]*mo[1]+mo[2]*mo[2]);

                if (distance>distance_min)
                {
                    mo[0]/=distance;
                    mo[1]/=distance;
                    mo[2]/=distance;

                    particules[i]->vitesse[0]+=(mo[0]*acceleration - particules[i]->vitesse[0]*frottements)/particules[i]->masse*delta_t;
                    particules[i]->vitesse[1]+=(mo[1]*acceleration - particules[i]->vitesse[1]*frottements)/particules[i]->masse*delta_t;
                    particules[i]->vitesse[2]+=(mo[2]*acceleration - particules[i]->vitesse[2]*frottements)/particules[i]->masse*delta_t;
                }

            }
        }

        //-----------------------------------------------------------------------
        //       Gravite avec trajectoires perturbées par un bruit de Perlin
        /// Appeler APRES la mise à jour de la scène
        //-----------------------------------------------------------------------
        void maj_gravite_perturbation_Perlin(double centre[3],double acceleration,double frottements=1.,double distance_min=1., double amplitude=1.,double intervalle=1.)
        {
            uint32_t i=0;
            double mo[3];
            double distance;
            double px,py,pz;
            for(i=0;i<nbrPoints;i++)
            {
                mo[0]=centre[0]-points[3*i];
                mo[1]=centre[1]-points[3*i+1];
                mo[2]=centre[2]-points[3*i+2];
                distance=sqrt(mo[0]*mo[0]+mo[1]*mo[1]+mo[2]*mo[2]);

                px=amplitude*(particules[i]->perturbation_x->valeur_aleatoire_temporelle(position_temps,intervalle)*2.-1.);
                py=amplitude*(particules[i]->perturbation_y->valeur_aleatoire_temporelle(position_temps,intervalle)*2.-1.);
                pz=amplitude*(particules[i]->perturbation_z->valeur_aleatoire_temporelle(position_temps,intervalle)*2.-1.);

                if (distance>distance_min)
                {
                    mo[0]/=distance;
                    mo[1]/=distance;
                    mo[2]/=distance;

                    particules[i]->vitesse[0]+=(( (mo[0]*acceleration+px) - particules[i]->vitesse[0]*frottements) / particules[i]->masse )*delta_t;
                    particules[i]->vitesse[1]+=(( (mo[1]*acceleration+py) - particules[i]->vitesse[1]*frottements) / particules[i]->masse )*delta_t;
                    particules[i]->vitesse[2]+=(( (mo[2]*acceleration+pz) - particules[i]->vitesse[2]*frottements) / particules[i]->masse )*delta_t;
                }

                else
                {

                    particules[i]->vitesse[0]+=(( px - particules[i]->vitesse[0]*frottements) / particules[i]->masse )*delta_t;
                    particules[i]->vitesse[1]+=(( py - particules[i]->vitesse[1]*frottements) / particules[i]->masse )*delta_t;
                    particules[i]->vitesse[2]+=(( pz - particules[i]->vitesse[2]*frottements) / particules[i]->masse )*delta_t;
                }

            }
        }

        //-----------------------------------------------------------------------
        //       Gestion d'un flux de particules se déplaçant dans une direction
        //          dans un espace déterminé
        //-----------------------------------------------------------------------
        void initialise_flux(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, uint8_t p_direction )
        {
            determine_parametres_flux(xmin,xmax,ymin,ymax,zmin,zmax,p_direction);
            random_points(xmin,xmax,ymin,ymax,zmin,zmax);
        }

        void determine_parametres_flux(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, uint8_t p_direction)
        {
            flux_xmin=xmin;
            flux_ymin=ymin;
            flux_zmin=zmin;
            flux_xmax=xmax;
            flux_ymax=ymax;
            flux_zmax=zmax;
            flux_direction=p_direction;
        }

        //--------Mise à jour de la position des points

        void maj_flux(double vitesse=1.)
        {
            uint32_t i;
            double p;
            if (vitesse<0.) return; //Permet d'optimiser le calcul des coordonnées - Changer la direction pour un flux inversé
            if (delta_t<1e-6) return;
            vitesse*=delta_t;
            switch (flux_direction)
            {
                case HAUT:
                    for (i=0;i<nbrPoints;i++)
                    {
                        p=points[3*i+1]+vitesse;
                        if (p>flux_ymax) p=flux_ymin+(p-flux_ymax);
                        points[3*i+1]=p;
                    }
                    break;
                case BAS:
                    for (i=0;i<nbrPoints;i++)
                    {
                        p=points[3*i+1]-vitesse;
                        if (p<flux_ymin) p=flux_ymax-(flux_ymin-p);
                        points[3*i+1]=p;
                    }
                    break;
                case GAUCHE:
                    for (i=0;i<nbrPoints;i++)
                    {
                        points[3*i]+=vitesse;
                        if (p>flux_xmax) p=flux_xmin+(p-flux_xmax);
                        points[3*i]=p;
                    }
                    break;
                case DROITE:
                    for (i=0;i<nbrPoints;i++)
                    {
                        p=points[3*i]-vitesse;
                        if (p<flux_xmin) p=flux_xmax-(flux_xmin-p);
                        points[3*i]=p;
                    }
                    break;
                case AVANT:
                    for (i=0;i<nbrPoints;i++)
                    {
                        points[3*i+2]+=vitesse;
                        if (p>flux_zmax) p=flux_zmin+(p-flux_zmax);
                        points[3*i]=p;
                    }
                    break;
                case ARRIERE:
                    for (i=0;i<nbrPoints;i++)
                    {
                        p=points[3*i+2]-vitesse;
                        if (p<flux_zmin) p=flux_zmax-(flux_zmin-p);
                        points[3*i+2]=p;
                    }
                    break;
                default:break;
            }
        }

        //=========================================================
		//              Tri les points:
        //=========================================================
        //Le test utilisé pour le tri des points:
		static bool comparaison(tri_particules t1, tri_particules t2) { return t1.distance < t2.distance; }

		virtual void tri_points(dksOeil* oeil)
		{
		    // Calcul la position de l'oeil par rapport au repère du champ de particules:
            double position_oeil[3];
            dksRepere::point_relatif(oeil->repere_absolu,this,position_oeil);

		    //Calcul les distances avec l'oeil:
		    for (uint32_t i=0;i<nbrPoints;i++)
		    {
                double dx=position_oeil[0]-points[i*3];
                double dy=position_oeil[1]-points[i*3+1];
                double dz=position_oeil[2]-points[i*3+2];
                indices_points[i].distance=dx*dx+dy*dy+dz*dz;
                indices_points[i].indice_point=i;
		    }
		    std::sort(indices_points.begin(),indices_points.end(),dksParticules::comparaison);
		}

        //==========================================================================
        //         Gestion des particules actives et inactives
        //==========================================================================
        void random_durees_vie(double dmin,double dmax)
        {
            uint32_t i;
            for (i=0;i<nbrPoints;i++)
            {
                particules[i]->duree_vie=dmin+(dmax-dmin)*double(rand())/32768.;
            }
        }

        void determine_durees_vie(double duree)
        {
            uint32_t i;
            for (i=0;i<nbrPoints;i++)
            {
                particules[i]->duree_vie=duree;
            }
        }

        virtual void active_particule(double date)
        {
            if ( particules_inactives==NULL ) return;
            Element_particule* particule = (Element_particule*)particules_inactives;

            //Initialise les paramètres de départ de la particule:

            particule->date_creation=date;

            //Active la particule:
            deplace_noeud(particule,&particules_inactives,&particules_actives);
        }

        //----- Destruction d'une particule:
        void detruit_particule(Element_particule* particule)
        {
            deplace_noeud(particule,&particules_actives,&particules_inactives);
        }

        //Déplace un noeud d'une liste vers une autre liste.
        //Le noeud est ajouté à la tête de la liste de destination.
        void deplace_noeud(Element_particule* noeud, Element_particule** liste_source, Element_particule** liste_destination)
        {
            Element_particule* noeud_suivant=(Element_particule*)noeud->suivante;
            Element_particule* noeud_precedent=(Element_particule*)noeud->precedente;

            if (noeud_suivant!= NULL) noeud_suivant->precedente=noeud_precedent;
            if (noeud_precedent!=NULL) noeud_precedent->suivante=noeud_suivant;
            else *liste_source=noeud_suivant;

            if ((*liste_destination)!=NULL)
            {
                (*liste_destination)->precedente=noeud;
            }
            noeud->suivante=*liste_destination;
            noeud->precedente=NULL;
            *liste_destination=noeud;
        }

        //Désactive toutes les particules:
        void desactive_particules()
        {
            particules_actives=NULL;
            if (nbrPoints>0)
            {
                particules_inactives=particules[0];
                particules[0]->precedente=NULL;

                uint32_t i;
                    for (i=0;i< nbrPoints-1;i++)
                    {
                        particules[i]->suivante=particules[i+1];
                        particules[i+1]->precedente=particules[i];
                    }
                    particules[i]->suivante=NULL;
            }
        }

        //Active toutes les particules:
        void active_particules(double date)
        {
            particules_inactives=NULL;
            if (nbrPoints>0)
            {
                particules_actives=particules[0];
                particules[0]->precedente=NULL;

                uint32_t i;
                    for (i=0;i< nbrPoints-1;i++)
                    {
                        particules[i]->suivante=particules[i+1];
                        particules[i+1]->precedente=particules[i];
                    }
                    particules[i]->suivante=NULL;
            }
        }

        //------- Mise à jour des particules selon leur durée de vie:
        // Selon l'age de la particules, les classes héritières peuvent
        // avoir différentes réactions. Fonction à redéfinir selon les besoins.
        virtual void maj_ages(double date)
        {
            Element_particule* particule=particules_actives;
            Element_particule* suivante;
            while (particule!=NULL)
            {
                if(date-particule->date_creation>=particule->duree_vie)
                {
                    suivante=(Element_particule*)particule->suivante;
                    detruit_particule(particule);
                    particule=suivante;
                }
                else particule=(Element_particule*) particule->suivante;
            }
        }


		//==========================================================================
        //         Prototypes des fonctions spécifiques aux types de particules
        //==========================================================================
        virtual void affiche_vie_limitee()
        {
            glBegin(GL_POINTS);
            Element_particule* particule=particules_actives;
            while (particule!=NULL)
            {
                glVertex3dv(&points[3*particule->point]);
                particule=(Element_particule*)particule->suivante;
            }
            glEnd();
        }

        //Le rendu propre aux classes dérivées.
        virtual void affiche(dksOeil* oeil)
        {
                uint32_t i;
                glPointSize(epaisseur_points);
                float r=float(((couleur_points>>16)&0xff))/255.;
                float v=float(((couleur_points>>8)&0xff))/255.;
                float b=float((couleur_points&0xff))/255.;
                GLboolean light_mem=glIsEnabled(GL_LIGHTING);
                glDisable(GL_LIGHTING);
                glColor3f(r,v,b);
                if (drapeau_vie_limitee) affiche_vie_limitee();
                else
                {
                    glBegin(GL_POINTS);
                    for (i=0;i<nbrPoints;i++)
                    {
                        glVertex3dv(&points[3*i]);
                    }
                    glEnd();
                }
                if(light_mem)glEnable(GL_LIGHTING);
        }

        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {
        }

        virtual void affiche_profondeur(dksOeil* oeil)
        {
        }

        //===========================================================================================
        //      Gestion des fichiers
        //===========================================================================================
        bool sauve_points(char* nom_fichier)
        {
            return Fichiers::sauve_donnees_binaires((char*)points,nbrPoints*3*sizeof(double),nom_fichier);
        }

        ///Il faut que les points aient déjà été alloués!
        bool charge_points(char* nom_fichier)
        {
            int32_t taille=Fichiers::charge_donnees_binaires((char*)points,nbrPoints*3*sizeof(double),nom_fichier);
            if (taille==-1) return false;
            a_ete_deplace();
            return true;
        }


};

#endif // PARTICULES_H_INCLUDED
