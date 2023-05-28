#ifndef DKSJETPARTICULES_H_INCLUDED
#define DKSJETPARTICULES_H_INCLUDED


/*

            Génère un jet de particules

*/

#include "dksParticules.h"
//=============================================================
//      Classe des éléments gérés par le jet:
//=============================================================
#define JET_ETAPES_COULEURS_MAX 4 //nombre max d'étapes dans l'évolution des couleurs des particules au cours de leur vie

class Particule_Jet : public Element_particule
{
    public:
        float couleur[4];   //r,v,b,a
        uint16_t nbrEtapesCouleurs;
        float etapes_couleurs[JET_ETAPES_COULEURS_MAX][3];
        float taille;

    //----- Constructeur:
    Particule_Jet(uint32_t p_index):Element_particule(p_index)
    {
        taille=3.;
        couleur={1.,1.,1.,1.};
        nbrEtapesCouleurs=0;
    }

    //Détermine la couleur de la particule en fonction de son age.
    ///l'age est compris entre 0 et 1:
    void determine_couleur(float age)
    {
        if (age>=1. || age<0.)return;
        couleur[3]=1.-age;

        if(nbrEtapesCouleurs>1)
        {
            float etape_couleur=age*float(nbrEtapesCouleurs);
            uint16_t etape_depart=uint16_t(etape_couleur);
            etape_couleur-=etape_depart;
            couleur[0]=etapes_couleurs[etape_depart][0]+(etapes_couleurs[etape_depart+1][0]-etapes_couleurs[etape_depart][0])*etape_couleur;
            couleur[1]=etapes_couleurs[etape_depart][1]+(etapes_couleurs[etape_depart+1][1]-etapes_couleurs[etape_depart][1])*etape_couleur;
            couleur[2]=etapes_couleurs[etape_depart][2]+(etapes_couleurs[etape_depart+1][2]-etapes_couleurs[etape_depart][2])*etape_couleur;
        }



    }

};

//============================================================================
//              Classe
//============================================================================


class dksJetParticules : public dksParticules
{
    private:
    float compteur_creation_particules; //Sert pour le control du débit

    float direction_jet[3];
    float normale_jet[3];
    float binormale_jet[3];

    public:
    float position_jet[3];
    float ouverture; //Angle en ° d'ouverture du cône de projection
    float vitesse_jet; //Vitesse en m/s de projection
    float rayon_surface_propulsion; //Le rayon de la surface d'où sont projetées les particules
    float debit; //Nombre de particules générées par seconde



    //=====================================
    //      Constructeur
    //=====================================

    dksJetParticules(char* p_nom="Jet_Particules",
                    double px=0.,double py=0.,double pz=0.,
                    uint32_t p_nbrPoints=0
                    ):dksParticules(p_nom,px,py,pz,0)
    {
        determine_nombre_de_points(p_nbrPoints);
        drapeau_vie_limitee=true;
        ouverture=30.;
        vitesse_jet=5.;
        rayon_surface_propulsion=0.1;
        compteur_creation_particules=0.;
        debit=10.;
        determine_direction_jet(1.,1.,0.);
        position_jet={0.,0.,0.};

    }

    //---------- Destructeur:
    ~dksJetParticules()
    {

    }

    //===============================================
    //      Gestion des particules
    //===============================================
    virtual void creation_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Jet* particule=new Particule_Jet(i);
            particules[i]=particule;
        }
    }

    virtual void destruction_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++)
            {
                if (particules[i] != NULL) delete particules[i];
            }
    }

    void determine_etapes_couleurs(uint16_t nbrEtapes,float* p_couleurs)
    {
        uint32_t i;
        uint16_t j;
        if (nbrEtapes>JET_ETAPES_COULEURS_MAX)nbrEtapes=JET_ETAPES_COULEURS_MAX;
        for(i=0;i<nbrPoints;i++)
        {
            Particule_Jet* particule=(Particule_Jet*)particules[i];
            particule->nbrEtapesCouleurs=nbrEtapes;
            for (j=0;j<nbrEtapes;j++)
            {
                particule->etapes_couleurs[j]={p_couleurs[j*3],p_couleurs[j*3+1],p_couleurs[j*3+2]};
            }
        }
    }

    //Un repère est ascocié au jet, pour orienter correctement les perticules
    void determine_direction_jet(float dx,float dy, float dz)
    {
        float n=sqrtf(dx*dx+dy*dy+dz*dz);
        dx/=n;dy/=n;dz/=n;
        direction_jet={dx,dy,dz};
        float tx,tz;
        //Calcul binormale:
        tx=dz;
        //ty=0.;
        tz=-dx;
        n=sqrtf(tx*tx+tz*tz);

        if (n<1e-5)
        {
            binormale_jet={-1.,0.,0.};
            normale_jet={0.,1.,0.};
        }

        else
        {
            binormale_jet={tx/n,0.,tz/n};
            //Calcul Normale:
            normale_jet={
                            binormale_jet[2]*dy,
                            binormale_jet[0]*dz-binormale_jet[2]*dx,
                            -binormale_jet[0]*dy
                        };
        }

    }

    //================================================
    //          Gestion du jet
    /// Fonction à appeler après applique_mouvement()
    //================================================



    virtual void maj_ages()
    {
        Particule_Jet* particule=(Particule_Jet*)particules_actives;
        Particule_Jet* suivante;

        while (particule!=NULL)
        {
            double age_particule=position_temps-particule->date_creation;
            if(age_particule>=particule->duree_vie)
            {
                suivante=(Particule_Jet*)particule->suivante;
                detruit_particule(particule);
                particule=suivante;
            }
            else
            {
                age_particule/=particule->duree_vie;
                particule->determine_couleur(age_particule);
                particule=(Particule_Jet*) particule->suivante;
            }
        }
    }

    //---------------------------------
    //      Activation d'une particule
    /// Fonction à appeler après applique_mouvements()
    //---------------------------------

    //--------- Oriente un vecteur dans le repère du jet:
    void oriente_vecteur(float* vecteur, double* vecteur_dest)
    {
        vecteur_dest[0]=vecteur[0]*binormale_jet[0]+vecteur[1]*normale_jet[0]+vecteur[2]*direction_jet[0];
        vecteur_dest[1]=vecteur[0]*binormale_jet[1]+vecteur[1]*normale_jet[1]+vecteur[2]*direction_jet[1];
        vecteur_dest[2]=vecteur[0]*binormale_jet[2]+vecteur[1]*normale_jet[2]+vecteur[2]*direction_jet[2];
    }

    //--------- Provoque l'éjection d'un nombre déterminé de particules:
    void ejection_particules(uint32_t nbr)
    {
        for(uint32_t i=0;i<nbr;i++) active_particule();
    }

    //--------- Active une particule en lui attribuant sa direction de propulsion, inclue dans le cône du jet

     void active_particule()
        {
            if ( particules_inactives==NULL ) return;
            Particule_Jet* particule = (Particule_Jet*)particules_inactives;

            //Initialise les paramètres de départ de la particule:

            particule->date_creation=position_temps;
            particule->couleur[3]=1.;

            float inclinaison,px,py,theta,phi;
            inclinaison=float(rand())/32768.;
            theta=float(rand())/32768.*2.*M_PI;
            phi=ouverture/180.*M_PI*inclinaison;

            px=cosf(theta);
            py=sinf(theta);

            float vitesse_temp[3]={
                                    px*sinf(phi) * vitesse_jet ,
                                    py*sinf(phi) * vitesse_jet,
                                    cosf(phi) * vitesse_jet
                                    };
            float point_temp[3]={
                                    px*inclinaison*rayon_surface_propulsion,
                                    py*inclinaison*rayon_surface_propulsion,
                                    0.
                                };

            oriente_vecteur(vitesse_temp,particule->vitesse);
            oriente_vecteur(point_temp,&points[particule->point*3]);
            points[particule->point*3]+=position_jet[0];
            points[particule->point*3+1]+=position_jet[1];
            points[particule->point*3+2]+=position_jet[2];
            a_ete_deplace();


            //Particule dans l'espace relatif du conteneur:
            //if (!drapeau_absolu) determine_point(particule->point,double(px*inclinaison*rayon_surface_propulsion),double(py*inclinaison*rayon_surface_propulsion),0.);

            //Particule dans l'espace absolu.
            //L'orientation du jet reste dans l'espace du conteneur.
            /*
            else
            {

                uint32_t indice=particule->point;
                vec3d point;
                point.x=px*inclinaison*rayon_surface_propulsion;
                point.y=py*inclinaison*rayon_surface_propulsion;
                point.z=0.;
                Repere::translation_point(&point,repere_absolu,&point);
                points_absolus[3*indice]=point.x;
                points_absolus[3*indice+1]=point.y;
                points_absolus[3*indice+2]=point.z;

                point.x=particule->vitesse.x;
                point.y=particule->vitesse.y;
                point.z=particule->vitesse.z;
                Repere::translation_axe(&point,repere_absolu,&point);
                particule->vitesse.x=point.x;
                particule->vitesse.y=point.y;
                particule->vitesse.z=point.z;


            }
            */
            //Active la particule:
            deplace_noeud(particule,&particules_inactives,&particules_actives);
        }

    //---------------------------------
    //      Gestion du débit
    /// Fonction à appeler après applique_mouvement(), à cause du delta_t
    //---------------------------------
    void maj_debit()
    {
        uint32_t i,nbr;
        compteur_creation_particules+=debit*delta_t;

        if ( compteur_creation_particules>1. )
        {
            nbr=(uint32_t)compteur_creation_particules;
            for(i=0;i<nbr;i++)
            {
                active_particule();
            }
            compteur_creation_particules-=float(nbr);
        }
    }

    //---------------------------------------------------------------------
    //          Gestion des mouvements
    //          Il y a plusieurs type de jets:
    //          Un jet continu qui utilise le débit
    //          Ejection d'un nombre déterminé de particules
    //---------------------------------------------------------------------
    void maj_jet_continu()
    {
        maj_debit();
        maj_ages();
        maj_positions_points_perturbation_perlin();
    }

    void maj_jet_discontinu()
    {
        //maj_debit();
        maj_ages();
        maj_positions_points_perturbation_perlin();
    }


    virtual void applique_mouvements()
    {
        dksVolume::applique_mouvements();

        if (delta_t<1e-6) return;

        if (drapeau_maj_points_auto)
        {
            maj_jet_continu();
        }

    }

    //==============================================
    //          Affichage
    //==============================================

    //Le rendu propre aux classes dérivées.
    virtual void affiche(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
        glPointSize(epaisseur_points);
        GLboolean light_mem=glIsEnabled(GL_LIGHTING);
        GLboolean blend_mem=glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);
        Particule_Jet* particule=(Particule_Jet*)particules_actives;
        glUseProgram(0);
        desactive_textures();
        glBegin(GL_POINTS);
        while (particule!=NULL)
        {
            glColor4fv(particule->couleur);
            glVertex3dv(&points[3*particule->point]);
            particule=(Particule_Jet*)particule->suivante;
        }
        glEnd();
        if(light_mem)glEnable(GL_LIGHTING);
        if(!blend_mem)glDisable(GL_BLEND);
    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
        glPointSize(epaisseur_points_luminescents);
        GLboolean blend_mem=glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);
        Particule_Jet* particule=(Particule_Jet*)particules_actives;
        while (particule!=NULL)
        {
            glColor4fv(particule->couleur);
            glVertex3dv(&points[3*particule->point]);
            particule=(Particule_Jet*)particule->suivante;
        }
        glEnd();
        if(!blend_mem)glDisable(GL_BLEND);

    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
    }

};
#endif // DKSJETPARTICULES_H_INCLUDED
