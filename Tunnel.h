#ifndef TUNNEL_H_INCLUDED
#define TUNNEL_H_INCLUDED

#define NBRSECTIONSMAX 1000

//Les mode de déplacement du vecteur générateur:
#define TUNNEL_MODE_PERLIN      1   //La direction est générée par un bruit de Perlin
#define TUNNEL_MODE_TRAJECTOIRE 2   //Le déplacement se fait le long d'une trajectoire.
#define TUNNEL_MODE_LINEAIRE 3   //Le déplacement se fait en ligne droite.

#include "dksMoteur3d/Elements_Fondamentaux/dksConteneur.h"

class Tunnel : public dksConteneur
{
  public:

    uint32_t nbrSections;
    float vitesse_vecteur_generateur;  //en unités d'espace par seconde
    Bruit bruit_vecteur_generateur_x;
    Bruit bruit_vecteur_generateur_y;
    Bruit bruit_vecteur_generateur_z;

    double valeurs_bruit_x[20]; //Valeurs aléatoires pour la trajectoire du tunnel, suite fixe
    double valeurs_bruit_y[20];
    double valeurs_bruit_z[20];

    float perturbation_x;       //L'amplitude des perturbations
    float perturbation_y;       //Plus elle est grande, moins les virages seront serrés
    float perturbation_z;
    float distance_sections;    //Distance entre deux sections.
    float distance_vecteur_generateur;  //distance entre la dernière section et le vecteur générateur
    float position_intervalle;  //Position du vecteur générateur dans l'intervalle entre deux sections
    float position_vecteur_generateur[3];
    float orientation_vecteur_generateur[3];
    dksTrajectoire* trajectoire_vecteur_generateur;  //La trajectoire suivie par le vecteur générateur en mode TRAJECTOIRE
    float position_vecteur_sur_trajectoire; //La position du vecteur générateur sur la trajectoire ci-dessus (varie entre 0 et 1)
    uint8_t mode_deplacement_vecteur_generateur;
    float axe_rotation_alignement_trajectoire[3];   //Sert si il y a besoin d'aligner des volumes avec la trajectoire.
    float angle_alignement_trajectoire;

    float nbrVolumesSection;    //Le nombre d'éléments dans la bibliothèque
    dksVolume** bibliotheque_volumes_sections;    //Bibliothèque de sections

    float positions_sections[3*NBRSECTIONSMAX];
    float rotations_sections[4*NBRSECTIONSMAX];
    dksVolume* volumes_sections[NBRSECTIONSMAX];
    dksTrajectoire trajectoire_sections;

    double t_prec;
    bool drapeau_maj;

    //====================================================
    //          Constructeur
    //====================================================
    Tunnel(dksVolume** p_bibliotheque_volumes, uint32_t p_nbrVolumesSection,
            uint32_t p_nbrSections=25,float p_distance_sections=15.,dksTrajectoire* p_trajectoire=NULL,char* p_nom="Tunnel")
            :dksConteneur(p_nom)
    {
        nbrVolumesSection=p_nbrVolumesSection;
        bibliotheque_volumes_sections=p_bibliotheque_volumes;
        position_vecteur_generateur={0.,0.,0.};
        orientation_vecteur_generateur={0.,0.,1.};
        vitesse_vecteur_generateur=40;
        mode_deplacement_vecteur_generateur=TUNNEL_MODE_PERLIN;
        determine_trajectoire_vecteur_generateur(p_trajectoire);
        position_vecteur_sur_trajectoire=0.;

        determine_intervalles_perturbations(5.,5.,5.);

        nbrSections=p_nbrSections;
        distance_sections=p_distance_sections;
        trajectoire_sections.determine_facteur_lissage(2./3.);
        trajectoire_sections.determine_drapeau_longueurs(false);

        distance_vecteur_generateur=0.;
        t_prec=-1.;
        drapeau_maj=false;

        initialise_suites_aleatoires_fixes();

        bruit_vecteur_generateur_x.determine_nom("vecteur_generateur_x");
        bruit_vecteur_generateur_y.determine_nom("vecteur_generateur_y");
        bruit_vecteur_generateur_z.determine_nom("vecteur_generateur_z");

        cout<<"ORIENTATION - ------------- "<<orientation_vecteur_generateur[0]<<" , "<<orientation_vecteur_generateur[1]<<" , "<<orientation_vecteur_generateur[2]<<endl;

        initialise_sections();
    }


    //===========================================
    //          Destructeur
    //===========================================
    ~Tunnel()
    {

    }

    //==============================================
    //          Initialise la trajectoire générée
    //==============================================
    void initialise_suites_aleatoires_fixes()
    {
        //valeurs_bruit_x={0.350281,0.895935,0.378082,0.509888,0.0624084,0.909851,0.852417,0.0788879,0.765991,0.221344};
        //valeurs_bruit_y={0.822815 , 0.746582 , 0.210388,0.415009,0.762939,0.639282,0.569672,0.595978,0.847198,0.439423};
        //valeurs_bruit_z={0.174103 , 0.858917 , 0.940063, 0.00418091,0.670929,0.750952,0.883575,0.17041,0.479645,0.385223};
        valeurs_bruit_x={0.5,0.895935,0.378082,0.509888,0.0624084,0.909851,0.852417,0.0788879,0.765991,0.221344};
        valeurs_bruit_y={0.5 , 0.746582 , 0.210388,0.415009,0.762939,0.639282,0.569672,0.595978,0.847198,0.439423};
        valeurs_bruit_z={1. , 0.858917 , 0.940063, 0.00418091,0.670929,0.750952,0.883575,0.17041,0.479645,0.385223};
    }

    //Oriente le vecteur générateur selon la première valeur de la suite aléatoire fixe
    void initialise_orientation_vecteur_generateur_suites_aleatoires_fixes()
    {
        orientation_vecteur_generateur[0]=valeurs_bruit_x[0]*2.-1.;
        orientation_vecteur_generateur[1]=valeurs_bruit_y[0]*2.-1.;
        orientation_vecteur_generateur[2]=valeurs_bruit_z[0]*2.-1.;
    }

    //==============================================
    //          Initialise le tunnel
    //==============================================

    void initialise_sections()
    {
        double t=0;
        uint32_t i;
        //Initialise les modes de rendus des sections:
        for(i=0;i<nbrVolumesSection;i++)
        {
            bibliotheque_volumes_sections[i]->determine_mode_rendu(DKSVOLUME_MODE_LISTE);
        }

        //Initialise les sections du tunnel
            //maj_vecteur_generateur(0.,1/60.);
            //initialise_orientation_vecteur_generateur_suites_aleatoires_fixes();
        ajoute_sections(nbrSections);
    }

    //==============================================
    //          Gestion du tunnel
    //==============================================
    void maj(double t)
    {
         uint32_t i;
        //Premier rendu: initialisation du tunnel:
        if (t_prec<0.)
        {
            t_prec=t; return;
        }

        float delta_t=t-t_prec;

        //Déplacement du vecteur générateur:
        maj_vecteur_generateur(t,delta_t);

        //Génère les sections:
        maj_sections(t);

        t_prec=t;
        drapeau_maj=true;

    }

    //=====================================================
    //              Affichage du tunnel
    //=====================================================
    virtual void affiche(dksOeil* oeil)
    {
        if (!drapeau_maj) return;
        /*
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glVertex3fv(position_vecteur_generateur);
            glVertex3f(position_vecteur_generateur[0]+orientation_vecteur_generateur[0],
                       position_vecteur_generateur[1]+orientation_vecteur_generateur[1],
                       position_vecteur_generateur[2]+orientation_vecteur_generateur[2]
                       );
        glEnd();
        glPointSize(6);
        glColor3ub(0xff,0xff,0x00);
        glBegin(GL_POINTS);
            glVertex3fv(position_vecteur_generateur);
            glVertex3f(position_vecteur_generateur[0]+orientation_vecteur_generateur[0],
                       position_vecteur_generateur[1]+orientation_vecteur_generateur[1],
                       position_vecteur_generateur[2]+orientation_vecteur_generateur[2]
                       );
        glEnd();


        trajectoire_sections.affiche_courbe(10,0xffffffff);
        trajectoire_sections.affiche_points(0xffff0000,3);

        glEnable(GL_LIGHTING);
        */



        //Affiche les sections:
        glMatrixMode(GL_MODELVIEW);
        for (uint32_t i=0;i<nbrSections;i++)
        {
            glPushMatrix();
                glTranslatef(positions_sections[3*i],positions_sections[3*i+1],positions_sections[3*i+2]);
                glRotatef(rotations_sections[i*4],rotations_sections[i*4+1],rotations_sections[i*4+2],rotations_sections[i*4+3]);
                volumes_sections[i]->affiche(oeil);
            glPopMatrix();
        }

        drapeau_maj=false;
    }

    //N'affiche que les surfaces luminescentes:
    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {
        glMatrixMode(GL_MODELVIEW);
        for (uint32_t i=0;i<nbrSections;i++)
        {
            glPushMatrix();
                glTranslatef(positions_sections[3*i],positions_sections[3*i+1],positions_sections[3*i+2]);
                glRotatef(rotations_sections[i*4],rotations_sections[i*4+1],rotations_sections[i*4+2],rotations_sections[i*4+3]);
                volumes_sections[i]->affiche_couleurs_luminescentes(oeil);
            glPopMatrix();
        }
    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {

    }

    //----------------------------------------
    //  Gestion des sections
    //----------------------------------------
    void maj_sections(double t)
    {
        float distance_residuelle=distance_vecteur_generateur/distance_sections;
        uint32_t nbrSections_a_generer=uint32_t(distance_residuelle);
        distance_residuelle-=nbrSections_a_generer;
        if (nbrSections_a_generer>0)
        {
            //cout<<nbrSections_a_generer<<endl;
            ajoute_sections(nbrSections_a_generer);
            distance_vecteur_generateur=distance_residuelle*distance_sections;
        }
        position_intervalle=distance_vecteur_generateur/distance_sections;
        //cout<<position_intervalle<<endl;
    }


    //----------------------------------------
    //  Mise à jour du vecteur générateur
    //----------------------------------------

    void maj_vecteur_generateur(double t,double delta_t)
    {
        float n,d,dx,dy,dz;

        if (mode_deplacement_vecteur_generateur==TUNNEL_MODE_PERLIN)
        {
            /*
            orientation_vecteur_generateur[0]=bruit_vecteur_generateur_x.valeur_aleatoire_temporelle(t,perturbation_x)*2.-1.;
            orientation_vecteur_generateur[1]=bruit_vecteur_generateur_y.valeur_aleatoire_temporelle(t,perturbation_y)*2.-1.;
            orientation_vecteur_generateur[2]=bruit_vecteur_generateur_z.valeur_aleatoire_temporelle(t,perturbation_z)*2.-1.;
            */
            orientation_vecteur_generateur[0]=bruit_vecteur_generateur_x.valeur_suite_temporelle(t,perturbation_x,valeurs_bruit_x,10)*2.-1.;
            orientation_vecteur_generateur[1]=bruit_vecteur_generateur_y.valeur_suite_temporelle(t,perturbation_y,valeurs_bruit_y,10)*2.-1.;
            orientation_vecteur_generateur[2]=bruit_vecteur_generateur_z.valeur_suite_temporelle(t,perturbation_z,valeurs_bruit_z,10)*2.-1.;

            n=sqrtf(orientation_vecteur_generateur[0]*orientation_vecteur_generateur[0]
                    +orientation_vecteur_generateur[1]*orientation_vecteur_generateur[1]
                    +orientation_vecteur_generateur[2]*orientation_vecteur_generateur[2]);

            orientation_vecteur_generateur[0]/=n;   //Normalise l'orientation du vecteur générateur
            orientation_vecteur_generateur[1]/=n;
            orientation_vecteur_generateur[2]/=n;

            dx=orientation_vecteur_generateur[0]*vitesse_vecteur_generateur*delta_t;
            dy=orientation_vecteur_generateur[1]*vitesse_vecteur_generateur*delta_t;
            dz=orientation_vecteur_generateur[2]*vitesse_vecteur_generateur*delta_t;

            distance_vecteur_generateur+=sqrtf(dx*dx+dy*dy+dz*dz);

            position_vecteur_generateur[0]+=dx;
            position_vecteur_generateur[1]+=dy;
            position_vecteur_generateur[2]+=dz;
        }

        else if (mode_deplacement_vecteur_generateur==TUNNEL_MODE_TRAJECTOIRE)
        {

            trajectoire_vecteur_generateur->renvoie_position_tangente(position_vecteur_sur_trajectoire,position_vecteur_generateur,orientation_vecteur_generateur);
            d=vitesse_vecteur_generateur*delta_t;
            distance_vecteur_generateur+=d;
            position_vecteur_sur_trajectoire+=d/trajectoire_vecteur_generateur->renvoie_longueur_totale();
            //if (position_vecteur_sur_trajectoire>=1.) determine_mode_deplacement_vecteur_generateur(TUNNEL_MODE_PERLIN);
            if (position_vecteur_sur_trajectoire>=1.)
            {
                reset_trajectoire_vecteur_generateur();
            }
        }

        else if (mode_deplacement_vecteur_generateur==TUNNEL_MODE_LINEAIRE)
        {
            n=sqrtf(orientation_vecteur_generateur[0]*orientation_vecteur_generateur[0]
                    +orientation_vecteur_generateur[1]*orientation_vecteur_generateur[1]
                    +orientation_vecteur_generateur[2]*orientation_vecteur_generateur[2]);

            orientation_vecteur_generateur[0]/=n;   //Normalise l'orientation du vecteur générateur
            orientation_vecteur_generateur[1]/=n;
            orientation_vecteur_generateur[2]/=n;

            dx=orientation_vecteur_generateur[0]*vitesse_vecteur_generateur*delta_t;
            dy=orientation_vecteur_generateur[1]*vitesse_vecteur_generateur*delta_t;
            dz=orientation_vecteur_generateur[2]*vitesse_vecteur_generateur*delta_t;

            distance_vecteur_generateur+=sqrtf(dx*dx+dy*dy+dz*dz);

            position_vecteur_generateur[0]+=dx;
            position_vecteur_generateur[1]+=dy;
            position_vecteur_generateur[2]+=dz;
        }
    }

    //-------------------------------------------------------
    // Positionne un conteneur dans le tunnel:
    // La coordonnées logitudinale p varie entre 0 et 1
    //-------------------------------------------------------
    void positionne_conteneur(dksConteneur* cont,float p=0.5)
    {
        float position[3];
        float tangente[3];
        double tangented[3];
        p+=position_intervalle/float(nbrSections);
        float position_intervalle_section=float(nbrSections)*p;
        uint32_t indice_section=position_intervalle_section; //Indice de la section où se trouve le conteneur.
        position_intervalle_section-=indice_section;        //Position du conteneur dans l'intervalle
        float position_intervalle_conteneur=position_intervalle+(1.-position_intervalle_section);

        trajectoire_sections.renvoie_position_intervalle(indice_section,position_intervalle_section,position);

        cont->determine_position(position);
        //Calcul de la tangente à la trajectoire:
        uint32_t pv=indice_section+2; //Intervalle en avant qui sert à déterminer la tangente

        trajectoire_sections.renvoie_position_intervalle(pv,position_intervalle_section,tangente);

        tangente[0]-=position[0];
        tangente[1]-=position[1];
        tangente[2]-=position[2];

        float n=sqrtf(tangente[0]*tangente[0]+tangente[1]*tangente[1]+tangente[2]*tangente[2]);
        tangented[0]=double(tangente[0]/n);
        tangented[1]=double(tangente[1]/n);
        tangented[2]=double(tangente[2]/n);

        cont->initialisation_repere_orthonorme(NULL,NULL,tangented);

    }

    //---------------------------------------
    //      Ajoute des sections
    //  en entrée: le nombre de sections à ajouter
    //---------------------------------------

    void ajoute_sections(uint32_t nbr)
    {
        uint32_t i;
        if (nbr>nbrSections) nbr=nbrSections;

        for(i=nbrSections-1;i>nbr-1;i--)
        {
            positions_sections[3*i]=positions_sections[3*(i-nbr)];
            positions_sections[3*i+1]=positions_sections[3*(i-nbr)+1];
            positions_sections[3*i+2]=positions_sections[3*(i-nbr)+2];

            rotations_sections[4*i]=rotations_sections[4*(i-nbr)];
            rotations_sections[4*i+1]=rotations_sections[4*(i-nbr)+1];
            rotations_sections[4*i+2]=rotations_sections[4*(i-nbr)+2];
            rotations_sections[4*i+3]=rotations_sections[4*(i-nbr)+3];

            volumes_sections[i]=volumes_sections[i-nbr];
        }

        for (i=nbr-1;i<nbrSections;i--)
        {
            initialise_section(i);
        }

        trajectoire_sections.determine_points_inverses(positions_sections,nbrSections);
    }

    //--------------------------------------------------------------
    //      Initialise la position et l'orientation d'une section
    //      par rapport au vecteur générateur et à la position
    //      de la section dans le tunnel
    //--------------------------------------------------------------
    void initialise_section(uint32_t i)
    {
        float angle,n;

        if (i==0 || i==nbrSections-1)
        {
            if (i==nbrSections-1)
            {
                positions_sections[i*3]=position_vecteur_generateur[0]-orientation_vecteur_generateur[0]*distance_sections*(nbrSections-1);
                positions_sections[i*3+1]=position_vecteur_generateur[1]-orientation_vecteur_generateur[1]*distance_sections*(nbrSections-1);
                positions_sections[i*3+2]=position_vecteur_generateur[2]-orientation_vecteur_generateur[2]*distance_sections*(nbrSections-1);
            }
            else //if (i==0)
            {
                positions_sections[i*3]=position_vecteur_generateur[0];
                positions_sections[i*3+1]=position_vecteur_generateur[1];
                positions_sections[i*3+2]=position_vecteur_generateur[2];
            }

            calcul_orientation_section(i);
        }

        //Interpolation entre la section précédente et le vecteur générateur:
        else
        {
            float px=positions_sections[(i+1)*3];
            float py=positions_sections[(i+1)*3+1];
            float pz=positions_sections[(i+1)*3+2];

            float dx=position_vecteur_generateur[0]-px;
            float dy=position_vecteur_generateur[1]-py;
            float dz=position_vecteur_generateur[2]-pz;

            n=sqrtf(dx*dx+dy*dy+dz*dz);
            dx/=n;dy/=n;dz/=n;

            positions_sections[i*3]=px+dx*distance_sections;
            positions_sections[i*3+1]=py+dy*distance_sections;
            positions_sections[i*3+2]=pz+dz*distance_sections;

            //Orientation calquée sur la section précédente:
            ///Voir si une interpolation d'orientation est nécessaire!
            rotations_sections[i*4]=rotations_sections[(i+1)*4];
            rotations_sections[i*4+1]=rotations_sections[(i+1)*4+1];
            rotations_sections[i*4+2]=rotations_sections[(i+1)*4+2];
            rotations_sections[i*4+3]=rotations_sections[(i+1)*4+3];
        }

        volumes_sections[i]=bibliotheque_volumes_sections[rand()&0x3];
    }

    //=============================================================================
    //          Calcul l'angle de rotation, par rapport au vecteur générateur:
    //=============================================================================
    void calcul_orientation_section(uint16_t i)
    {
            float n;
            float angle=acosf(orientation_vecteur_generateur[2]);
            if (angle<1e-8 && angle>-1e-8)
            {
                rotations_sections[i*4]=0.;
                rotations_sections[i*4+1]=0.;
                rotations_sections[i*4+2]=0.;
                rotations_sections[i*4+3]=1.;
            }
            else
            {
                rotations_sections[i*4]=angle/M_PI*180.;
                rotations_sections[i*4+1]=-orientation_vecteur_generateur[1];
                rotations_sections[i*4+2]=orientation_vecteur_generateur[0];
                rotations_sections[i*4+3]=0.;

                n=sqrtf(rotations_sections[i*4+1]*rotations_sections[i*4+1]+rotations_sections[i*4+2]*rotations_sections[i*4+2]);
                if (n<1e-8)
                {
                    rotations_sections[i*4]=1.;
                    rotations_sections[i*4+1]=0.;
                }
                else
                {
                    rotations_sections[i*4+1]/=n;
                    rotations_sections[i*4+2]/=n;
                }
            }
    }
    //=============================================================================================================
    //                         Transistions entre les modes de génération du déplacement du vecteur générateur
    //=============================================================================================================

    //-------------------------------------------------------------------------------------------
    //  pour éviter un saut, le premier point de la trajectoire est positionné sur le vecteur,
    //  et tout les autres points sont repositionné relativements à ce premier point.
    //-------------------------------------------------------------------------------------------

    void basculement_perlin_trajectoire()
    {
        reset_trajectoire_vecteur_generateur();
        //L'orientation!
    }

    void basculement_trajectoire_perlin()
    {

    }

    //repositionne et aligne la trajectoire à la position et l'orientation actuelle du vecteur générateur:
    void reset_trajectoire_vecteur_generateur()
    {
        //Orientation de la trajectoire:
        float tangente_lien[3];
        trajectoire_vecteur_generateur->renvoie_tangente(0.,tangente_lien);
        //Calcul l'axe de rotation entre l'orientation du vecteur générateur et la tangente au point de départ de la trajectoire:
        axe_rotation_alignement_trajectoire[0]=orientation_vecteur_generateur[1]*tangente_lien[2]-orientation_vecteur_generateur[2]*tangente_lien[1];
        axe_rotation_alignement_trajectoire[1]=orientation_vecteur_generateur[2]*tangente_lien[0]-orientation_vecteur_generateur[0]*tangente_lien[2];
        axe_rotation_alignement_trajectoire[2]=orientation_vecteur_generateur[0]*tangente_lien[1]-orientation_vecteur_generateur[1]*tangente_lien[0];
        float norme=sqrtf(
                        axe_rotation_alignement_trajectoire[0]*axe_rotation_alignement_trajectoire[0]
                        +axe_rotation_alignement_trajectoire[1]*axe_rotation_alignement_trajectoire[1]
                        +axe_rotation_alignement_trajectoire[2]*axe_rotation_alignement_trajectoire[2]
                        );

        axe_rotation_alignement_trajectoire[0]/=norme;
        axe_rotation_alignement_trajectoire[1]/=norme;
        axe_rotation_alignement_trajectoire[2]/=norme;
        //Calcul l'angle:
        angle_alignement_trajectoire=-acosf(orientation_vecteur_generateur[0]*tangente_lien[0]+orientation_vecteur_generateur[1]*tangente_lien[1]+orientation_vecteur_generateur[2]*tangente_lien[2]);
        trajectoire_vecteur_generateur->rotation_trajectoire(angle_alignement_trajectoire,axe_rotation_alignement_trajectoire);
        //Enfin, positionnne le départ de la trajectoire sur la position actuelle du vecteur générateur.
        trajectoire_vecteur_generateur->positionne_trajectoire_point_0(position_vecteur_generateur[0],position_vecteur_generateur[1],position_vecteur_generateur[2]);
        position_vecteur_sur_trajectoire=0.;
    }



    //============================================================
    //                          Paramétrages
    //============================================================
    void determine_lissage_trajectoire(float lissage)
    {
        trajectoire_sections.determine_facteur_lissage(lissage);
    }

    float renvoie_lissage_trajectoire()
    {
        return trajectoire_sections.renvoie_facteur_lissage();
    }

    void determine_intervalles_perturbations(float px,float py, float pz)
    {
        perturbation_x=px;
        perturbation_y=py;
        perturbation_z=pz;
    }

    void determine_trajectoire_vecteur_generateur(dksTrajectoire* p_trajectoire)
    {
        trajectoire_vecteur_generateur=p_trajectoire;
    }

    void determine_mode_deplacement_vecteur_generateur(uint8_t p_mode)
    {
        if (p_mode==mode_deplacement_vecteur_generateur) return;

        else if (mode_deplacement_vecteur_generateur==TUNNEL_MODE_PERLIN && p_mode==TUNNEL_MODE_TRAJECTOIRE)
        {
            if (trajectoire_vecteur_generateur==NULL) return;
            basculement_perlin_trajectoire();
        }
        else if (mode_deplacement_vecteur_generateur==TUNNEL_MODE_TRAJECTOIRE && p_mode==TUNNEL_MODE_PERLIN)
        {
            basculement_trajectoire_perlin();
        }

        mode_deplacement_vecteur_generateur=p_mode;
    }

};
#endif // TUNNEL_H_INCLUDED
