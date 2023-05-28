#ifndef DKSINSECTE_H_INCLUDED
#define DKSINSECTE_H_INCLUDED

/*

        Squelette de base des insectes

*/

#include "../Elements_Fondamentaux/dksConteneur.h"
#include "../LWO/dksChargeLWO2.h"
#include "../LWO/dksConvertionLWO2.h"

//================== Codes d'erreurs
#define DKSINSECTE_OK 0
#define DKSINSECTE_ERREUR_CHARGEMENT_VOLUME 1
#define DKSINSECTE_ERREUR_CONVERTION_VOLUME 1

//================== Indices des points:
#define INSECTE_NBR_ARTICULATIONS 24
#define INSECTE_NBR_CONTACTS 6  //Les points de contact des pattes avec le sol
#define INSECTE_NBR_AXES 16 //Nombre de points qui, avec certaines articulations, constituent les axes de rotation
#define INSECTE_NBRPOINTS 64 //Il y a les points qui ne servent qu'à l'édition du squelette

//Articulations:
#define INSECTE_FEMUR_AVANT_DROIT 0
#define INSECTE_TIBIA_AVANT_DROIT 18
#define INSECTE_TARSE_AVANT_DROIT 36
#define INSECTE_FEMUR_AVANT_GAUCHE 9
#define INSECTE_TIBIA_AVANT_GAUCHE 27
#define INSECTE_TARSE_AVANT_GAUCHE 39

#define INSECTE_FEMUR_MEDIAN_DROIT 3
#define INSECTE_TIBIA_MEDIAN_DROIT 21
#define INSECTE_TARSE_MEDIAN_DROIT 37
#define INSECTE_FEMUR_MEDIAN_GAUCHE 12
#define INSECTE_TIBIA_MEDIAN_GAUCHE 30
#define INSECTE_TARSE_MEDIAN_GAUCHE 40

#define INSECTE_FEMUR_ARRIERE_DROIT 6
#define INSECTE_TIBIA_ARRIERE_DROIT 24
#define INSECTE_TARSE_ARRIERE_DROIT 38
#define INSECTE_FEMUR_ARRIERE_GAUCHE 15
#define INSECTE_TIBIA_ARRIERE_GAUCHE 33
#define INSECTE_TARSE_ARRIERE_GAUCHE 41

#define INSECTE_ANTENNE_DROITE 48
#define INSECTE_ANTENNE_GAUCHE 49
#define INSECTE_ANTENNE_FLAGELLE_DROITE 63
#define INSECTE_ANTENNE_FLAGELLE_GAUCHE 62

#define INSECTE_AILE_AVANT_DROITE 56
#define INSECTE_AILE_AVANT_GAUCHE 50
#define INSECTE_BOUT_AILE_AVANT_DROITE 57
#define INSECTE_BOUT_AILE_AVANT_GAUCHE 51

#define INSECTE_AILE_ARRIERE_DROITE 59
#define INSECTE_AILE_ARRIERE_GAUCHE 53
#define INSECTE_BOUT_AILE_ARRIERE_DROITE 60
#define INSECTE_BOUT_AILE_ARRIERE_GAUCHE 54

//Axes de rotation:
#define INSECTE_AXE_FEMUR_AVANT_DROIT 2
#define INSECTE_AXE_TIBIA_AVANT_DROIT 20
#define INSECTE_AXE_FEMUR_AVANT_GAUCHE 11
#define INSECTE_AXE_TIBIA_AVANT_GAUCHE 29

#define INSECTE_AXE_FEMUR_MEDIAN_DROIT 5
#define INSECTE_AXE_TIBIA_MEDIAN_DROIT 23
#define INSECTE_AXE_FEMUR_MEDIAN_GAUCHE 14
#define INSECTE_AXE_TIBIA_MEDIAN_GAUCHE 32

#define INSECTE_AXE_FEMUR_ARRIERE_DROIT 8
#define INSECTE_AXE_TIBIA_ARRIERE_DROIT 26
#define INSECTE_AXE_FEMUR_ARRIERE_GAUCHE 17
#define INSECTE_AXE_TIBIA_ARRIERE_GAUCHE 35

#define INSECTE_AXE_AILE_AVANT_DROIT 58
#define INSECTE_AXE_AILE_AVANT_GAUCHE 52
#define INSECTE_AXE_AILE_ARRIERE_DROITE 61
#define INSECTE_AXE_AILE_ARRIERE_GAUCHE 55

//Points de contact:
#define INSECTE_CONTACT_AVANT_DROIT 45
#define INSECTE_CONTACT_AVANT_GAUCHE 42
#define INSECTE_CONTACT_MEDIAN_DROIT 46
#define INSECTE_CONTACT_MEDIAN_GAUCHE 43
#define INSECTE_CONTACT_ARRIERE_DROIT 47
#define INSECTE_CONTACT_ARRIERE_GAUCHE 44


//--------------------------------------------------------
//      Structure de définition des angles des pattes
//--------------------------------------------------------

#define INSECTE_PAVD 0    //Indices dans le tableau
#define INSECTE_PAVG 1
#define INSECTE_PMD 2
#define INSECTE_PMG 3
#define INSECTE_PARD 4
#define INSECTE_PARG 5

#ifndef RADIAN
#define RADIANS M_PI/180.
#endif

class Patte_Insecte
{
    public:
        float angle_Fa; //angle de rotation du fémur autour de son axe
        float angle_Fr; //angle de rotation du fémur (donc axe vertical)
        float angle_Ta; //Angle de rotation du tibia autour de son axe
        uint32_t id_femur;  //Les indices des points dans la table
        uint32_t id_tibia;
        uint32_t id_tarse;
        uint32_t id_contact;
        uint32_t id_axe_femur;
        uint32_t id_axe_tibia;

    Patte_Insecte()
    {
       raz();
    }

    void init_ids(uint32_t femur, uint32_t tibia, uint32_t tarse, uint32_t contact, uint32_t axe_femur,uint32_t axe_tibia)
    {
        id_femur=femur;
        id_tibia=tibia;
        id_tarse=tarse;
        id_contact=contact;
        id_axe_femur=axe_femur;
        id_axe_tibia=axe_tibia;
    }

    void determine_angles(float fa,float fr,float ta)
    {
        angle_Fa=fa;
        angle_Fr=fr;
        angle_Ta=ta;
    }

    void raz()
    {
        angle_Fa=0.;
        angle_Fr=0.;
        angle_Ta=0.;
    }
};


//=================================================================================================================================
//                          Définition de la classe
//          Pour faciliter les animations, les angles sont en degrés.
//=================================================================================================================================
class dksInsecte:public dksConteneur
{
    protected:
        dksChargeLWO2* volumes_lwo2;
        dksConvertionLWO2* volumes_convertis;
        double echelle_geometrique;
        double Px;
        double Py;
        double Pz;
        double Rx;
        double Ry;
        double Rz;
        double* point;
        double Cx;
        double Cy;
        double Cz;
        float cos_a;    //Axe quelconque ou abduction (axe horizontal longitudinal)
        float sin_a;
        float cos_r;    //Rotation (axe vertical)
        float sin_r;
        float cos_e;    //extension (axe horizontal transversal)
        float sin_e;


    public:
        //Coordonnées des articulations et points stratégiques
        double squelette_base[3*INSECTE_NBRPOINTS];    //Les points à l'état de repos
        double squelette[3*INSECTE_NBRPOINTS];         //Les points articulés

        //Angles des antennes:
        float angle_AntAd;    //Angles d'abduction (axe horizontal longitudinal)
        float angle_AntAg;
        float angle_AntEd;    //Angles d'extension (axe horizontal transversal)
        float angle_AntEg;
        float angle_AntRd;    //Angles de rotation (axe vertical)
        float angle_AntRg;

        //Angle des ailes:
        float angle_Aavd;   //Avant
        float angle_Aavg;
        float angle_Aard;   //Arriere
        float angle_Aarg;

        Patte_Insecte pattes[6];

    //=====================================
    //      Constructeur
    //=====================================
    public:
        dksInsecte(char* p_nom,double px=0.,double py=0.,double pz=0.):dksConteneur(p_nom,px,py,pz)
        {
            uint32_t i;
            volumes_convertis=NULL;
            volumes_lwo2=NULL;

            echelle_geometrique=1.;

            //------------- Chargement du volume de définition des points:

            //char nom_fichier[]={"LWO2/snake.lwo"};

            char nom_fichier[]={"LWO2/articules/insecte.lwo"};   ///Fichier contenant le model de base du squelette

            volumes_lwo2=new dksChargeLWO2(nom_fichier);
            if (volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                cout<<"ERREUR dans dksInsecte::dksInsecte() - Impossible de charger le volume de définition des points - "<<nom_fichier<<endl;
                erreur=DKSINSECTE_ERREUR_CHARGEMENT_VOLUME;
                return;
            }

            volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
            if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
            {
                cout<<"ERREUR dans dksInsecte::dksInsecte() - Impossible de convertir le volume de définition des points - "<<nom_fichier<<endl;
                erreur=DKSINSECTE_ERREUR_CONVERTION_VOLUME;
                delete volumes_lwo2;
                return;
            }

            //------------ Mémorisation des points:
            double* points=volumes_convertis->renvoie_volume_index(0)->renvoie_sommets();

            for(i=0;i<INSECTE_NBRPOINTS;i++)
            {
                squelette_base[3*i]=points[3*i];//-points[PERSONNAGE_CENTRE_GRAVITE*3];
                squelette_base[3*i+1]=points[3*i+1];//-points[PERSONNAGE_CENTRE_GRAVITE*3+1];
                squelette_base[3*i+2]=points[3*i+2];//-points[PERSONNAGE_CENTRE_GRAVITE*3+2];
            }

            //Initialise les indices des articulations des pattes:
            pattes[INSECTE_PAVD].init_ids(INSECTE_FEMUR_AVANT_DROIT,INSECTE_TIBIA_AVANT_DROIT,INSECTE_TARSE_AVANT_DROIT,INSECTE_CONTACT_AVANT_DROIT,
                                            INSECTE_AXE_FEMUR_AVANT_DROIT,INSECTE_AXE_TIBIA_AVANT_DROIT);
            pattes[INSECTE_PAVG].init_ids(INSECTE_FEMUR_AVANT_GAUCHE,INSECTE_TIBIA_AVANT_GAUCHE,INSECTE_TARSE_AVANT_GAUCHE,INSECTE_CONTACT_AVANT_GAUCHE,
                                          INSECTE_AXE_FEMUR_AVANT_GAUCHE,INSECTE_AXE_TIBIA_AVANT_GAUCHE);
            pattes[INSECTE_PMD].init_ids(INSECTE_FEMUR_MEDIAN_DROIT,INSECTE_TIBIA_MEDIAN_DROIT,INSECTE_TARSE_MEDIAN_DROIT,INSECTE_CONTACT_MEDIAN_DROIT,
                                         INSECTE_AXE_FEMUR_MEDIAN_DROIT,INSECTE_AXE_TIBIA_MEDIAN_DROIT);
            pattes[INSECTE_PMG].init_ids(INSECTE_FEMUR_MEDIAN_GAUCHE,INSECTE_TIBIA_MEDIAN_GAUCHE,INSECTE_TARSE_MEDIAN_GAUCHE,INSECTE_CONTACT_MEDIAN_GAUCHE,
                                         INSECTE_AXE_FEMUR_MEDIAN_GAUCHE,INSECTE_AXE_TIBIA_MEDIAN_GAUCHE);
            pattes[INSECTE_PARD].init_ids(INSECTE_FEMUR_ARRIERE_DROIT,INSECTE_TIBIA_ARRIERE_DROIT,INSECTE_TARSE_ARRIERE_DROIT,INSECTE_CONTACT_ARRIERE_DROIT,
                                          INSECTE_AXE_FEMUR_ARRIERE_DROIT,INSECTE_AXE_TIBIA_ARRIERE_DROIT);
            pattes[INSECTE_PARG].init_ids(INSECTE_FEMUR_ARRIERE_GAUCHE,INSECTE_TIBIA_ARRIERE_GAUCHE,INSECTE_TARSE_ARRIERE_GAUCHE,INSECTE_CONTACT_ARRIERE_GAUCHE,
                                          INSECTE_AXE_FEMUR_ARRIERE_GAUCHE,INSECTE_AXE_TIBIA_ARRIERE_GAUCHE);

            raz_squelette();

            //------- Supprime les données LWO2:
            delete volumes_convertis;
            delete volumes_lwo2;
        }


    //=================================
    //      Destructeur
    //=================================
        ~dksInsecte()
        {

        }

    //================================================
    //      Paramétrage
    //================================================
    virtual void determine_echelle_geometrique(double p_echelle)
    {
        double facteur=p_echelle/echelle_geometrique;
        uint16_t i;
        for(i=0;i<INSECTE_NBRPOINTS;i++)
        {
            squelette_base[3*i]*=facteur;
            squelette_base[3*i+1]*=facteur;
            squelette_base[3*i+2]*=facteur;
        }
        echelle_geometrique=p_echelle;
    }

    //Lorsque l'on veut déterminer les mêmes angles pour toutes les pattes:
    void determine_angles_pattes(float fa,float fr, float ta)
    {
        for (uint16_t i=0;i<6;i++)
        {
            pattes[i].determine_angles(fa,fr,ta);
        }
    }

    //=======================================================================
    //          Animations
    //=======================================================================
    public:
        void maj_squelette()
        {
            maj_patte_droite(INSECTE_PAVD);
            maj_patte_droite(INSECTE_PMD);
            maj_patte_droite(INSECTE_PARD);
            maj_patte_gauche(INSECTE_PAVG);
            maj_patte_gauche(INSECTE_PMG);
            maj_patte_gauche(INSECTE_PARG);

            //Ailes:
            maj_aile(angle_Aavd*RADIANS,INSECTE_AILE_AVANT_DROITE,INSECTE_AXE_AILE_AVANT_DROIT,INSECTE_BOUT_AILE_AVANT_DROITE);
            maj_aile(-angle_Aavg*RADIANS,INSECTE_AILE_AVANT_GAUCHE,INSECTE_AXE_AILE_AVANT_GAUCHE,INSECTE_BOUT_AILE_AVANT_GAUCHE);

            maj_aile(angle_Aard*RADIANS,INSECTE_AILE_ARRIERE_DROITE,INSECTE_AXE_AILE_ARRIERE_DROITE,INSECTE_BOUT_AILE_ARRIERE_DROITE);
            maj_aile(-angle_Aarg*RADIANS,INSECTE_AILE_ARRIERE_GAUCHE,INSECTE_AXE_AILE_ARRIERE_GAUCHE,INSECTE_BOUT_AILE_ARRIERE_GAUCHE);

            //Antennes:
            cos_a=cosf(angle_AntAd*RADIANS);
            sin_a=sinf(angle_AntAd*RADIANS);
            cos_e=cosf(angle_AntEd*RADIANS);
            sin_e=sinf(angle_AntEd*RADIANS);
            cos_r=cosf(angle_AntRd*RADIANS);
            sin_r=sinf(angle_AntRd*RADIANS);
            Cx=squelette_base[INSECTE_ANTENNE_DROITE*3];
            Cy=squelette_base[INSECTE_ANTENNE_DROITE*3+1];
            Cz=squelette_base[INSECTE_ANTENNE_DROITE*3+2];
            abduction_extension_rotation_droite(INSECTE_ANTENNE_FLAGELLE_DROITE,squelette_base);

            cos_a=cosf(angle_AntAg*RADIANS);
            sin_a=sinf(angle_AntAg*RADIANS);
            cos_e=cosf(angle_AntEg*RADIANS);
            sin_e=sinf(angle_AntEg*RADIANS);
            cos_r=cosf(angle_AntRg*RADIANS);
            sin_r=sinf(angle_AntRg*RADIANS);
            Cx=squelette_base[INSECTE_ANTENNE_GAUCHE*3];
            Cy=squelette_base[INSECTE_ANTENNE_GAUCHE*3+1];
            Cz=squelette_base[INSECTE_ANTENNE_GAUCHE*3+2];
            abduction_extension_rotation_gauche(INSECTE_ANTENNE_FLAGELLE_GAUCHE,squelette_base);

        }

    private:
        void maj_patte_droite(uint8_t patte_id){maj_patte(patte_id,true);}
        void maj_patte_gauche(uint8_t patte_id){maj_patte(patte_id,false);}


        //cote: true=droit, false=gauche
        void maj_patte(uint8_t patte_id,bool cote)
        {
            Patte_Insecte* patte=&pattes[patte_id];

            cos_a=cosf(patte->angle_Ta*RADIANS);
            sin_a=sinf(patte->angle_Ta*RADIANS);
            Cx=squelette_base[patte->id_tibia*3];
            Cy=squelette_base[patte->id_tibia*3+1];
            Cz=squelette_base[patte->id_tibia*3+2];
            Rx=squelette_base[patte->id_axe_tibia*3]-Cx;
            Ry=squelette_base[patte->id_axe_tibia*3+1]-Cy;
            Rz=squelette_base[patte->id_axe_tibia*3+2]-Cz;
            double n=sqrt(Rx*Rx+Ry*Ry+Rz*Rz);
            Rx/=n;
            Ry/=n;
            Rz/=n;

            if(!cote){Rx=-Rx;Ry=-Ry;Rz=-Rz;}

            rotation_axe(patte->id_tarse,squelette_base);
            rotation_axe(patte->id_contact,squelette_base);

            cos_r=cosf(patte->angle_Fr*RADIANS);
            sin_r=sinf(patte->angle_Fr*RADIANS);
            cos_a=cosf(patte->angle_Fa*RADIANS);
            sin_a=sinf(patte->angle_Fa*RADIANS);
            Cx=squelette_base[patte->id_femur*3];
            Cy=squelette_base[patte->id_femur*3+1];
            Cz=squelette_base[patte->id_femur*3+2];
            Rx=squelette_base[patte->id_axe_femur*3]-Cx;
            Ry=squelette_base[patte->id_axe_femur*3+1]-Cy;
            Rz=squelette_base[patte->id_axe_femur*3+2]-Cz;
            n=sqrt(Rx*Rx+Ry*Ry+Rz*Rz);
            Rx/=n;
            Ry/=n;
            Rz/=n;

            if(!cote){Rx=-Rx;Ry=-Ry;Rz=-Rz;}

            rotation_axe(patte->id_tibia,squelette_base);
            rotation_axe(patte->id_tarse,squelette);
            rotation_axe(patte->id_contact,squelette);

            if(cote)
            {
                rotation_droite(patte->id_tibia,squelette);
                rotation_droite(patte->id_tarse,squelette);
                rotation_droite(patte->id_contact,squelette);
            }
            else
            {
                rotation_gauche(patte->id_tibia,squelette);
                rotation_gauche(patte->id_tarse,squelette);
                rotation_gauche(patte->id_contact,squelette);
            }

        }


        void maj_aile(float angle, uint16_t id_c, uint16_t id_a, uint16_t id_p)
        {
            cos_a=cosf(angle);
            sin_a=sinf(angle);
            Cx=squelette_base[id_c*3];
            Cy=squelette_base[id_c*3+1];
            Cz=squelette_base[id_c*3+2];
            Rx=squelette_base[id_a*3]-Cx;
            Ry=squelette_base[id_a*3+1]-Cy;
            Rz=squelette_base[id_a*3+2]-Cz;
            double n=sqrt(Rx*Rx+Ry*Ry+Rz*Rz);
            Rx/=n;
            Ry/=n;
            Rz/=n;

            rotation_axe(id_p,squelette_base);
        }


    //-------------------------------------
    //      Fonctions de rotation:
    //-------------------------------------
    protected:

        void rotation_droite(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Pz=src[id*3+2]-Cz;
            point=&squelette[id*3];
            point[0]=Px*cos_r-Pz*sin_r+Cx;
            point[2]=Px*sin_r+Pz*cos_r+Cz;
            point[1]=src[id*3+1];
        }

        void rotation_gauche(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Pz=src[id*3+2]-Cz;
            point=&squelette[id*3];
            point[0]=Px*cos_r+Pz*sin_r+Cx;
            point[2]=-Px*sin_r+Pz*cos_r+Cz;
            point[1]=src[id*3+1];
        }

        void abduction_extension_rotation_droite(uint32_t id, double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*(cos_a*cos_r+sin_a*sin_e*sin_r)+Py*(sin_a*cos_r-cos_a*sin_e*sin_r)-Pz*(cos_e*sin_r)+Cx;
            point[1]=Py*cos_a*cos_e-Px*sin_a*cos_e-Pz*sin_e+Cy;
            point[2]=Px*(cos_a*sin_r-sin_a*sin_e*cos_r)+Py*(sin_a*sin_r+cos_a*sin_e*cos_r)+Pz*(cos_e*cos_r)+Cz;
        }

        void abduction_extension_rotation_gauche(uint32_t id, double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*(cos_a*cos_r+sin_a*sin_e*sin_r)+Py*(-sin_a*cos_r+cos_a*sin_e*sin_r)+Pz*(cos_e*sin_r)+Cx;
            point[1]=Py*cos_a*cos_e+Px*sin_a*cos_e-Pz*sin_e+Cy;
            point[2]=Px*(-cos_a*sin_r+sin_a*sin_e*cos_r)+Py*(sin_a*sin_r+cos_a*sin_e*cos_r)+Pz*(cos_e*cos_r)+Cz;
        }

        void rotation_axe(uint32_t id, double* src)
		{
			/*
			formule d'Olinde Rodrigues :
			V = cos(phi)*U + (1-cos(phi))*((U.N)*N) + sin(phi)[N^U]
			La formule ci-dessus donne l'expression vectorielle du transformé V  d'un vecteur U  quelconque, dans la rotation phi,N d'angle phi, et d'axe N unitaire
			*/

            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

			double prod_scal = Rx*Px+Ry*Py+Rz*Pz;

			double xt = cos_a * Px + sin_a * (Ry * Pz - Rz * Py) + (1. - cos_a) * prod_scal * Rx;
			double yt = cos_a * Py + sin_a * (Rz * Px - Rx * Pz) + (1. - cos_a) * prod_scal * Ry;
			point[2]   = cos_a * Pz + sin_a * (Rx * Py - Ry * Px) + (1. - cos_a) * prod_scal * Rz + Cz;
			point[0]     = xt + Cx;
			point[1]   = yt + Cy;
		}

    //===============================================
    //      Réinitialisation du squelette
    //===============================================
    public:
        void raz_squelette()
        {
            uint16_t i;
            for(i=0;i<INSECTE_NBRPOINTS;i++)
            {
                squelette[3*i]=squelette_base[3*i];
                squelette[3*i+1]=squelette_base[3*i+1];
                squelette[3*i+2]=squelette_base[3*i+2];
            }

            for(i=0;i<6;i++) pattes[i].raz();

            angle_AntAd=0.;
            angle_AntAg=0.;
            angle_AntRd=0.;
            angle_AntRg=0.;
            angle_AntEd=0.;
            angle_AntEg=0.;

            angle_Aavd=0.;
            angle_Aavg=0.;
            angle_Aard=0.;
            angle_Aarg=0.;
        }
    //===============================================
    //          Affichages
    //===============================================
    public:
        virtual void affiche(dksOeil* oeil)
        {
            glPushMatrix();
            glMultMatrixd(matrice.matrice);

            desactive_textures();
            glUseProgram(0);

            uint16_t i;
            GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);

            //Lignes:
            glColor3f(1.,1.,1.);

            //patte avant droite
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_AVANT_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_AVANT_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_AVANT_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_AVANT_DROIT]);
            glEnd();
            //patte avant gauche
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_AVANT_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_AVANT_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_AVANT_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_AVANT_GAUCHE]);
            glEnd();
            //patte mediane droite
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_MEDIAN_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_MEDIAN_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_MEDIAN_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_MEDIAN_DROIT]);
            glEnd();
            //patte mediane gauche
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_MEDIAN_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_MEDIAN_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_MEDIAN_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_MEDIAN_GAUCHE]);
            glEnd();

            //patte arrière droite
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_ARRIERE_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_ARRIERE_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_ARRIERE_DROIT]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_ARRIERE_DROIT]);
            glEnd();

            //patte arrière gauche
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_FEMUR_ARRIERE_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TIBIA_ARRIERE_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_TARSE_ARRIERE_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_CONTACT_ARRIERE_GAUCHE]);
            glEnd();

            //Aile avant droite:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_AILE_AVANT_DROITE]);
            glVertex3dv(&squelette[3*INSECTE_BOUT_AILE_AVANT_DROITE]);
            glEnd();

            //Aile avant gauche:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_AILE_AVANT_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_BOUT_AILE_AVANT_GAUCHE]);
            glEnd();

            //Aile arrière droite:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_AILE_ARRIERE_DROITE]);
            glVertex3dv(&squelette[3*INSECTE_BOUT_AILE_ARRIERE_DROITE]);
            glEnd();

            //Aile arrière gauche:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_AILE_ARRIERE_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_BOUT_AILE_ARRIERE_GAUCHE]);
            glEnd();

            //Antenne droite:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_ANTENNE_DROITE]);
            glVertex3dv(&squelette[3*INSECTE_ANTENNE_FLAGELLE_DROITE]);
            glEnd();

            //Antenne gauche:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*INSECTE_ANTENNE_GAUCHE]);
            glVertex3dv(&squelette[3*INSECTE_ANTENNE_FLAGELLE_GAUCHE]);
            glEnd();

            //Points:
            glColor3f(1.,1.,0.);
            glPointSize(5.);
            glBegin(GL_POINTS);
            for(i=0;i<INSECTE_NBRPOINTS;i++) glVertex3dv(&squelette[i*3]);
            glEnd();

            if (lum_mem)glEnable(GL_LIGHTING);

            glPopMatrix();
        }
};
#endif // DKSINSECTE_H_INCLUDED
