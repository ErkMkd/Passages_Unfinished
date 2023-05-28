#ifndef DKSPERSONNAGE_H_INCLUDED
#define DKSPERSONNAGE_H_INCLUDED

/*
    Squelette de base d'un personnage humanoide
    Le centre du personnage est par défaut situé au niveau des talons
*/

#include "../Elements_Fondamentaux/dksConteneur.h"
#include "../LWO/dksChargeLWO2.h"
#include "../LWO/dksConvertionLWO2.h"

//================== Codes d'erreurs
#define DKSPERSONNAGE_OK 0
#define DKSPERSONNAGE_ERREUR_CHARGEMENT_VOLUME 1
#define DKSPERSONNAGE_ERREUR_CONVERTION_VOLUME 1

//================== Indices des points:
#define PERSONNAGE_NBRPOINTS 24


#define PERSONNAGE_OEIL_DROIT 17
#define PERSONNAGE_OEIL_GAUCHE 16
#define PERSONNAGE_ARTICULATION_TETE 15
#define PERSONNAGE_ARTICULATION_COU 14
#define PERSONNAGE_ARTICULATION_THORAX 23
#define PERSONNAGE_ARTICULATION_ABDOMEN 13
#define PERSONNAGE_EPAULE_DROITE 9
#define PERSONNAGE_EPAULE_GAUCHE 4
#define PERSONNAGE_COUDE_DROIT 10
#define PERSONNAGE_COUDE_GAUCHE 5
#define PERSONNAGE_POIGNET_DROIT 11
#define PERSONNAGE_POIGNET_GAUCHE 6
#define PERSONNAGE_MAIN_DROITE 12
#define PERSONNAGE_MAIN_GAUCHE 7
#define PERSONNAGE_HANCHE_DROITE 8
#define PERSONNAGE_HANCHE_GAUCHE 3
#define PERSONNAGE_GENOU_DROIT 21
#define PERSONNAGE_GENOU_GAUCHE 2
#define PERSONNAGE_CHEVILLE_DROITE 22
#define PERSONNAGE_CHEVILLE_GAUCHE 18
#define PERSONNAGE_TALON_DROIT 19
#define PERSONNAGE_TALON_GAUCHE 0
#define PERSONNAGE_PLANTE_DROITE 20
#define PERSONNAGE_PLANTE_GAUCHE 1
//=================================================================================================================================
//                          Définition de la classe
//          Pour faciliter les animations, les angles sont en degrés.
//=================================================================================================================================
class dksPersonnage:public dksConteneur
{
    protected:
        dksChargeLWO2* volumes_lwo2;
        dksConvertionLWO2* volumes_convertis;
        double echelle_geometrique;
        double Px;
        double Py;
        double Pz;
        double* point;
        double Cx;
        double Cy;
        double Cz;
        float cos_a;
        float sin_a;
        float cos_e;
        float sin_e;
        float cos_r;
        float sin_r;


    public:
        //Coordonnées des articulations et points stratégiques
        double squelette_base[3*PERSONNAGE_NBRPOINTS];    //Les points à l'état de repos
        double squelette[3*PERSONNAGE_NBRPOINTS];         //Les points articulés

        float angle_Pda; //Angles de rotations des poignets
        float angle_Pde;
        float angle_Pdr;
        float angle_Pga;
        float angle_Pge;
        float angle_Pgr;


        float angle_Cd; //Angles de rotation des coudes
        float angle_Cg;

        float angle_Eda; //Angles de rotation des épaules (abduction,extension)
        float angle_Ede;
        float angle_Edr;
        float angle_Ega;
        float angle_Ege;
        float angle_Egr;

        float angle_Vd; //Angles chevilles
        float angle_Vg;

        float angle_Gd; //Angles genoux
        float angle_Gg;

        float angle_Hda; //Angles de rotation des hanches (abduction,extension)
        float angle_Hde;
        float angle_Hga;
        float angle_Hge;

        float angle_Ta; //Angles de rotation de la tête (abduction, extension, rotation)
        float angle_Te;
        float angle_Tr;

        float angle_Ca; //Angles de rotation du cou
        float angle_Ce;
        float angle_Cr;

        float angle_Ha; //Angles de rotation du thorax
        float angle_He;
        float angle_Hr;

        float angle_Aa; //Angles de rotation de l'abdomen (vertèbres lombaires)
        float angle_Ae;
        float angle_Ar;



    //=====================================
    //      Constructeur
    //=====================================
    public:
        dksPersonnage(char* p_nom,double px=0.,double py=0.,double pz=0.):dksConteneur(p_nom,px,py,pz)
        {
            uint32_t i;
            volumes_convertis=NULL;
            volumes_lwo2=NULL;

            echelle_geometrique=1.;
            angle_Pga=0.;
            angle_Pda=0.;

            //------------- Chargement du volume de définition des points:

            //char nom_fichier[]={"LWO2/snake.lwo"};

            char nom_fichier[]={"LWO2/articules/personnage.lwo"};   ///Fichier contenant le model de base du squelette

            volumes_lwo2=new dksChargeLWO2(nom_fichier);
            if (volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                cout<<"ERREUR dans dksPersonnage::dksPersonnage() - Impossible de charger le volume de définition des points - "<<nom_fichier<<endl;
                erreur=DKSPERSONNAGE_ERREUR_CHARGEMENT_VOLUME;
                return;
            }

            volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
            if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
            {
                cout<<"ERREUR dans dksPersonnage::dksPersonnage() - Impossible de convertir le volume de définition des points - "<<nom_fichier<<endl;
                erreur=DKSPERSONNAGE_ERREUR_CONVERTION_VOLUME;
                delete volumes_lwo2;
                return;
            }

            //------------ Mémorisation des points:
            double* points=volumes_convertis->renvoie_volume_index(0)->renvoie_sommets();

            for(i=0;i<PERSONNAGE_NBRPOINTS;i++)
            {
                squelette_base[3*i]=points[3*i];//-points[PERSONNAGE_CENTRE_GRAVITE*3];
                squelette_base[3*i+1]=points[3*i+1];//-points[PERSONNAGE_CENTRE_GRAVITE*3+1];
                squelette_base[3*i+2]=points[3*i+2];//-points[PERSONNAGE_CENTRE_GRAVITE*3+2];
            }

            raz_squelette();

            //------- Supprime les données LWO2:
            delete volumes_convertis;
            delete volumes_lwo2;
        }


    //=================================
    //      Destructeur
    //=================================
        ~dksPersonnage()
        {

        }

    //================================================
    //      Paramétrage
    //================================================
    virtual void determine_echelle_geometrique(double p_echelle)
    {
        double facteur=p_echelle/echelle_geometrique;
        uint16_t i;
        for(i=0;i<PERSONNAGE_NBRPOINTS;i++)
        {
            squelette_base[3*i]*=facteur;
            squelette_base[3*i+1]*=facteur;
            squelette_base[3*i+2]*=facteur;
        }
        echelle_geometrique=p_echelle;
    }

    //=======================================================================
    //          Animations
    //=======================================================================
    public:
        void maj_squelette()
        {
            double Px,Py,Pz;
            double* point;

            #define RADIANS M_PI/180.
            //Rotation poignet droit:
                cos_a=cosf(angle_Pda*RADIANS);
                sin_a=sinf(angle_Pda*RADIANS);
                cos_e=cosf(angle_Pde*RADIANS);
                sin_e=sinf(angle_Pde*RADIANS);
                cos_r=cosf(angle_Pdr*RADIANS);
                sin_r=sinf(angle_Pdr*RADIANS);

                Cx=squelette_base[PERSONNAGE_POIGNET_DROIT*3];
                Cy=squelette_base[PERSONNAGE_POIGNET_DROIT*3+1];
                Cz=squelette_base[PERSONNAGE_POIGNET_DROIT*3+2];

                rotation_abduction_extension_rotation_droite(PERSONNAGE_MAIN_DROITE,squelette_base);

            //Rotation poignet gauche:
                cos_a=cosf(angle_Pga*RADIANS);
                sin_a=sinf(angle_Pga*RADIANS);
                cos_e=cosf(angle_Pge*RADIANS);
                sin_e=sinf(angle_Pge*RADIANS);
                cos_r=cosf(angle_Pgr*RADIANS);
                sin_r=sinf(angle_Pgr*RADIANS);

                Cx=squelette_base[PERSONNAGE_POIGNET_GAUCHE*3];
                Cy=squelette_base[PERSONNAGE_POIGNET_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_POIGNET_GAUCHE*3+2];

                rotation_abduction_extension_rotation_gauche(PERSONNAGE_MAIN_GAUCHE,squelette_base);


            //Rotation coude droit:
                cos_e=cosf(angle_Cd*RADIANS);
                sin_e=sinf(angle_Cd*RADIANS);

                Cy=squelette_base[PERSONNAGE_COUDE_DROIT*3+1];
                Cz=squelette_base[PERSONNAGE_COUDE_DROIT*3+2];

                rotation_extension(PERSONNAGE_POIGNET_DROIT,squelette_base);
                rotation_extension(PERSONNAGE_MAIN_DROITE,squelette);


            //Rotation coude gauche:
                cos_e=cosf(angle_Cg*RADIANS);
                sin_e=sinf(angle_Cg*RADIANS);

                Cy=squelette_base[PERSONNAGE_COUDE_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_COUDE_GAUCHE*3+2];

                rotation_extension(PERSONNAGE_POIGNET_GAUCHE,squelette_base);
                rotation_extension(PERSONNAGE_MAIN_GAUCHE,squelette);

            //Rotation Epaule droite:
                cos_a=cosf(angle_Eda*RADIANS);
                sin_a=sinf(angle_Eda*RADIANS);
                cos_e=cosf(angle_Ede*RADIANS);
                sin_e=sinf(angle_Ede*RADIANS);
                cos_r=cosf(angle_Edr*RADIANS);
                sin_r=sinf(angle_Edr*RADIANS);

                Cx=squelette_base[PERSONNAGE_EPAULE_DROITE*3];
                Cy=squelette_base[PERSONNAGE_EPAULE_DROITE*3+1];
                Cz=squelette_base[PERSONNAGE_EPAULE_DROITE*3+2];

                rotation_abduction_extension_rotation_droite(PERSONNAGE_COUDE_DROIT,squelette_base);
                rotation_abduction_extension_rotation_droite(PERSONNAGE_POIGNET_DROIT,squelette);
                rotation_abduction_extension_rotation_droite(PERSONNAGE_MAIN_DROITE,squelette);

            //Rotation Epaule gauche:
                cos_a=cosf(angle_Ega*RADIANS);
                sin_a=sinf(angle_Ega*RADIANS);
                cos_e=cosf(angle_Ege*RADIANS);
                sin_e=sinf(angle_Ege*RADIANS);
                cos_r=cosf(angle_Egr*RADIANS);
                sin_r=sinf(angle_Egr*RADIANS);

                Cx=squelette_base[PERSONNAGE_EPAULE_GAUCHE*3];
                Cy=squelette_base[PERSONNAGE_EPAULE_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_EPAULE_GAUCHE*3+2];

                rotation_abduction_extension_rotation_gauche(PERSONNAGE_COUDE_GAUCHE,squelette_base);
                rotation_abduction_extension_rotation_gauche(PERSONNAGE_POIGNET_GAUCHE,squelette);
                rotation_abduction_extension_rotation_gauche(PERSONNAGE_MAIN_GAUCHE,squelette);

            //Rotation cheville droite:

                cos_e=cosf(angle_Vd*RADIANS);
                sin_e=sinf(angle_Vd*RADIANS);

                Cy=squelette_base[PERSONNAGE_CHEVILLE_DROITE*3+1];
                Cz=squelette_base[PERSONNAGE_CHEVILLE_DROITE*3+2];

                rotation_extension(PERSONNAGE_TALON_DROIT,squelette_base);
                rotation_extension(PERSONNAGE_PLANTE_DROITE,squelette_base);

            //Rotation cheville gauche:
                cos_e=cosf(angle_Vg*RADIANS);
                sin_e=sinf(angle_Vg*RADIANS);

                Cy=squelette_base[PERSONNAGE_CHEVILLE_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_CHEVILLE_GAUCHE*3+2];

                rotation_extension(PERSONNAGE_TALON_GAUCHE,squelette_base);
                rotation_extension(PERSONNAGE_PLANTE_GAUCHE,squelette_base);

            //Rotation genou droit:

                cos_e=cosf(angle_Gd*RADIANS);
                sin_e=sinf(angle_Gd*RADIANS);

                Cy=squelette_base[PERSONNAGE_GENOU_DROIT*3+1];
                Cz=squelette_base[PERSONNAGE_GENOU_DROIT*3+2];

                rotation_extension(PERSONNAGE_CHEVILLE_DROITE,squelette_base);
                rotation_extension(PERSONNAGE_TALON_DROIT,squelette);
                rotation_extension(PERSONNAGE_PLANTE_DROITE,squelette);

            //Rotation genou gauche:

                cos_e=cosf(angle_Gg*RADIANS);
                sin_e=sinf(angle_Gg*RADIANS);

                Cy=squelette_base[PERSONNAGE_GENOU_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_GENOU_GAUCHE*3+2];

                rotation_extension(PERSONNAGE_CHEVILLE_GAUCHE,squelette_base);
                rotation_extension(PERSONNAGE_TALON_GAUCHE,squelette);
                rotation_extension(PERSONNAGE_PLANTE_GAUCHE,squelette);

            //Rotation hanche droite:

                cos_a=cosf(angle_Hda*RADIANS);
                sin_a=sinf(angle_Hda*RADIANS);
                cos_e=cosf(angle_Hde*RADIANS);
                sin_e=sinf(angle_Hde*RADIANS);

                Cx=squelette_base[PERSONNAGE_HANCHE_DROITE*3];
                Cy=squelette_base[PERSONNAGE_HANCHE_DROITE*3+1];
                Cz=squelette_base[PERSONNAGE_HANCHE_DROITE*3+2];

                rotation_abduction_extension_droite(PERSONNAGE_GENOU_DROIT,squelette_base);
                rotation_abduction_extension_droite(PERSONNAGE_CHEVILLE_DROITE,squelette);
                rotation_abduction_extension_droite(PERSONNAGE_TALON_DROIT,squelette);
                rotation_abduction_extension_droite(PERSONNAGE_PLANTE_DROITE,squelette);

                //Rotation hanche gauche:

                cos_a=cosf(angle_Hga*RADIANS);
                sin_a=sinf(angle_Hga*RADIANS);
                cos_e=cosf(angle_Hge*RADIANS);
                sin_e=sinf(angle_Hge*RADIANS);

                Cx=squelette_base[PERSONNAGE_HANCHE_GAUCHE*3];
                Cy=squelette_base[PERSONNAGE_HANCHE_GAUCHE*3+1];
                Cz=squelette_base[PERSONNAGE_HANCHE_GAUCHE*3+2];

                rotation_abduction_extension_gauche(PERSONNAGE_GENOU_GAUCHE,squelette_base);
                rotation_abduction_extension_gauche(PERSONNAGE_CHEVILLE_GAUCHE,squelette);
                rotation_abduction_extension_gauche(PERSONNAGE_TALON_GAUCHE,squelette);
                rotation_abduction_extension_gauche(PERSONNAGE_PLANTE_GAUCHE,squelette);


                //Rotation tête:
                cos_a=cosf(angle_Ta*RADIANS);
                sin_a=sinf(angle_Ta*RADIANS);
                cos_e=cosf(angle_Te*RADIANS);
                sin_e=sinf(angle_Te*RADIANS);
                cos_r=cosf(angle_Tr*RADIANS);
                sin_r=sinf(angle_Tr*RADIANS);

                Cx=squelette_base[PERSONNAGE_ARTICULATION_TETE*3];
                Cy=squelette_base[PERSONNAGE_ARTICULATION_TETE*3+1];
                Cz=squelette_base[PERSONNAGE_ARTICULATION_TETE*3+2];

                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_DROIT,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_GAUCHE,squelette_base);

                //Rotation cou:
                cos_a=cosf(angle_Ca*RADIANS);
                sin_a=sinf(angle_Ca*RADIANS);
                cos_e=cosf(angle_Ce*RADIANS);
                sin_e=sinf(angle_Ce*RADIANS);
                cos_r=cosf(angle_Cr*RADIANS);
                sin_r=sinf(angle_Cr*RADIANS);

                Cx=squelette_base[PERSONNAGE_ARTICULATION_COU*3];
                Cy=squelette_base[PERSONNAGE_ARTICULATION_COU*3+1];
                Cz=squelette_base[PERSONNAGE_ARTICULATION_COU*3+2];

                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_TETE,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_GAUCHE,squelette);


                //Rotation thorax:
                cos_a=cosf(angle_Ha*RADIANS);
                sin_a=sinf(angle_Ha*RADIANS);
                cos_e=cosf(angle_He*RADIANS);
                sin_e=sinf(angle_He*RADIANS);
                cos_r=cosf(angle_Hr*RADIANS);
                sin_r=sinf(angle_Hr*RADIANS);

                Cx=squelette_base[PERSONNAGE_ARTICULATION_THORAX*3];
                Cy=squelette_base[PERSONNAGE_ARTICULATION_THORAX*3+1];
                Cz=squelette_base[PERSONNAGE_ARTICULATION_THORAX*3+2];

                rotation_abduction_extension_rotation_centrale(PERSONNAGE_MAIN_DROITE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_MAIN_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_POIGNET_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_POIGNET_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_COUDE_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_COUDE_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_EPAULE_DROITE,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_EPAULE_GAUCHE,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_COU,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_TETE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_GAUCHE,squelette);

                //Rotation Abdomen (vertèbres lombaires):
                cos_a=cosf(angle_Aa*RADIANS);
                sin_a=sinf(angle_Aa*RADIANS);
                cos_e=cosf(angle_Ae*RADIANS);
                sin_e=sinf(angle_Ae*RADIANS);
                cos_r=cosf(angle_Ar*RADIANS);
                sin_r=sinf(angle_Ar*RADIANS);

                Cx=squelette_base[PERSONNAGE_ARTICULATION_ABDOMEN*3];
                Cy=squelette_base[PERSONNAGE_ARTICULATION_ABDOMEN*3+1];
                Cz=squelette_base[PERSONNAGE_ARTICULATION_ABDOMEN*3+2];

                rotation_abduction_extension_rotation_centrale(PERSONNAGE_MAIN_DROITE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_MAIN_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_POIGNET_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_POIGNET_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_COUDE_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_COUDE_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_EPAULE_DROITE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_EPAULE_GAUCHE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_THORAX,squelette_base);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_COU,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_ARTICULATION_TETE,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_DROIT,squelette);
                rotation_abduction_extension_rotation_centrale(PERSONNAGE_OEIL_GAUCHE,squelette);

                //Copie de l'abdomen et des hanches dans le tempon:
                double* point_src=&squelette_base[PERSONNAGE_ARTICULATION_ABDOMEN*3];
                double* point_dst=&squelette[PERSONNAGE_ARTICULATION_ABDOMEN*3];
                point_dst[0]=point_src[0];
                point_dst[1]=point_src[1];
                point_dst[2]=point_src[2];
                point_src=&squelette_base[PERSONNAGE_HANCHE_DROITE*3];
                point_dst=&squelette[PERSONNAGE_HANCHE_DROITE*3];
                point_dst[0]=point_src[0];
                point_dst[1]=point_src[1];
                point_dst[2]=point_src[2];
                point_src=&squelette_base[PERSONNAGE_HANCHE_GAUCHE*3];
                point_dst=&squelette[PERSONNAGE_HANCHE_GAUCHE*3];
                point_dst[0]=point_src[0];
                point_dst[1]=point_src[1];
                point_dst[2]=point_src[2];


        }

        //-----------------------------------------------
        //          Fonctions de rotations
        //-----------------------------------------------
    private:

        void rotation_abduction_droite(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            point=&squelette[id*3];
            point[0]=Px*cos_a+Py*sin_a+Cx;
            point[1]=-Px*sin_a+Py*cos_a+Cy;
            point[2]=src[id*3+2];
        }

        void rotation_abduction_gauche(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            point=&squelette[id*3];
            point[0]=Px*cos_a-Py*sin_a+Cx;
            point[1]=Px*sin_a+Py*cos_a+Cy;
            point[2]=src[id*3+2];
        }

        void rotation_extension(uint32_t id,double* src)
        {
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;
            point=&squelette[id*3];
            point[2]=Pz*cos_e+Py*sin_e+Cz;
            point[1]=-Pz*sin_e+Py*cos_e+Cy;
            point[0]=src[id*3];
        }

        void rotation_abduction_extension_droite(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*cos_a+Py*sin_a+Cx;
            point[1]=-Pz*sin_e+(Py*cos_a*cos_e-Px*sin_a*cos_e)+Cy;
            point[2]=Pz*cos_e+(Py*cos_a*sin_e-Px*sin_a*sin_e)+Cz;
        }

        void rotation_abduction_extension_gauche(uint32_t id,double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*cos_a-Py*sin_a+Cx;
            point[1]=-Pz*sin_e+(Py*cos_a*cos_e+Px*sin_a*cos_e)+Cy;
            point[2]=Pz*cos_e+(Py*cos_a*sin_e+Px*sin_a*sin_e)+Cz;
        }

        void rotation_abduction_extension_rotation_centrale(uint32_t id, double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*(cos_a*cos_r+sin_a*sin_e*sin_r)+Py*(-sin_a*cos_r+cos_a*sin_e*sin_r)-Pz*(cos_e*sin_r)+Cx;
            point[1]=Py*cos_a*cos_e+Px*sin_a*cos_e+Pz*sin_e+Cy;
            point[2]=Px*(cos_a*sin_r-sin_a*sin_e*cos_r)+Py*(-sin_a*sin_r-cos_a*sin_e*cos_r)+Pz*(cos_e*cos_r)+Cz;
        }

        void rotation_abduction_extension_rotation_droite(uint32_t id, double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*(cos_a*cos_r+sin_a*sin_e*sin_r)+Py*(sin_a*cos_r-cos_a*sin_e*sin_r)-Pz*(cos_e*sin_r)+Cx;
            point[1]=Py*cos_a*cos_e-Px*sin_a*cos_e-Pz*sin_e+Cy;
            point[2]=Px*(cos_a*sin_r-sin_a*sin_e*cos_r)+Py*(sin_a*sin_r+cos_a*sin_e*cos_r)+Pz*(cos_e*cos_r)+Cz;
        }

        void rotation_abduction_extension_rotation_gauche(uint32_t id, double* src)
        {
            Px=src[id*3]-Cx;
            Py=src[id*3+1]-Cy;
            Pz=src[id*3+2]-Cz;

            point=&squelette[id*3];

            point[0]=Px*(cos_a*cos_r+sin_a*sin_e*sin_r)+Py*(-sin_a*cos_r+cos_a*sin_e*sin_r)+Pz*(cos_e*sin_r)+Cx;
            point[1]=Py*cos_a*cos_e+Px*sin_a*cos_e-Pz*sin_e+Cy;
            point[2]=Px*(-cos_a*sin_r+sin_a*sin_e*cos_r)+Py*(sin_a*sin_r+cos_a*sin_e*cos_r)+Pz*(cos_e*cos_r)+Cz;
        }

    //===============================================
    //      Réinitialisation du squelette
    //===============================================
    public:
        void raz_squelette()
        {
            uint16_t i;
            for(i=0;i<PERSONNAGE_NBRPOINTS;i++)
            {
                squelette[3*i]=squelette_base[3*i];
                squelette[3*i+1]=squelette_base[3*i+1];
                squelette[3*i+2]=squelette_base[3*i+2];
            }

            angle_Pda=0.; //Angles de rotations des poignets
            angle_Pde=0.;
            angle_Pdr=0.;
            angle_Pga=0.;
            angle_Pge=0.;
            angle_Pgr=0.;

            angle_Cd=0.; //Angles de rotation des coudes
            angle_Cg=0.;

            angle_Eda=0.; //Angles de rotation des épaules (abduction,extension)
            angle_Ede=0.;
            angle_Ega=0.;
            angle_Ege=0.;

            angle_Vd=0.; //Angles chevilles
            angle_Vg=0.;

            angle_Gd=0.; //Angles genoux
            angle_Gg=0.;

            angle_Hda=0.; //Angles de rotation des hanches (abduction,extension)
            angle_Hde=0.;
            angle_Hga=0.;
            angle_Hge=0.;

            angle_Ta=0.; //Angles de rotation de la tête (abduction, extension, rotation)
            angle_Te=0.;
            angle_Tr=0.;

            angle_Ca=0.; //Angles de rotation du cou
            angle_Ce=0.;
            angle_Cr=0.;

            angle_Ha=0.; //Angles de rotation du thorax
            angle_He=0.;
            angle_Hr=0.;

            angle_Aa=0.; //Angles de rotation de l'abdomen (vertèbres lombaires)
            angle_Ae=0.;
            angle_Ar=0.;
        }
    //===============================================
    //          Affichages
    //===============================================
    public:
        virtual void affiche(dksOeil* oeil)
        {

            /*
            repere_absolu->copie_repere(this);
            glPushMatrix();
            calcul_matrice();
            glPopMatrix();
            */

            glPushMatrix();
            glMultMatrixd(matrice.matrice);

            uint16_t i;
            GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);

            glColor3f(1.,1.,1.);
            //Colonne vertébrale:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*PERSONNAGE_ARTICULATION_TETE]);
            glVertex3dv(&squelette[3*PERSONNAGE_ARTICULATION_COU]);
            glVertex3dv(&squelette[3*PERSONNAGE_ARTICULATION_THORAX]);
            glVertex3dv(&squelette[3*PERSONNAGE_ARTICULATION_ABDOMEN]);
            glEnd();

            //Bras droit:

            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*PERSONNAGE_EPAULE_DROITE]);
            glVertex3dv(&squelette[3*PERSONNAGE_COUDE_DROIT]);
            glVertex3dv(&squelette[3*PERSONNAGE_POIGNET_DROIT]);
            glVertex3dv(&squelette[3*PERSONNAGE_MAIN_DROITE]);
            glEnd();

            //Bras gauche:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*PERSONNAGE_EPAULE_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_COUDE_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_POIGNET_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_MAIN_GAUCHE]);
            glEnd();

            //Jambe droite:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*PERSONNAGE_HANCHE_DROITE]);
            glVertex3dv(&squelette[3*PERSONNAGE_GENOU_DROIT]);
            glVertex3dv(&squelette[3*PERSONNAGE_CHEVILLE_DROITE]);
            glVertex3dv(&squelette[3*PERSONNAGE_TALON_DROIT]);
            glVertex3dv(&squelette[3*PERSONNAGE_PLANTE_DROITE]);
            glEnd();

            //Jambe gauche:
            glBegin(GL_LINE_STRIP);
            glVertex3dv(&squelette[3*PERSONNAGE_HANCHE_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_GENOU_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_CHEVILLE_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_TALON_GAUCHE]);
            glVertex3dv(&squelette[3*PERSONNAGE_PLANTE_GAUCHE]);
            glEnd();

            //Points:
            glColor3f(1.,0.,0.);
            glPointSize(5.);
            glBegin(GL_POINTS);
            for(i=0;i<PERSONNAGE_NBRPOINTS;i++)
            {
                glVertex3dv(&squelette[i*3]);
            }
            glEnd();

            if (lum_mem)glEnable(GL_LIGHTING);

            glPopMatrix();
        }


        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {

        }


        virtual void affiche_profondeur(dksOeil* oeil)
        {

        }

        //===============================================
        //      Debugage et editions
        //===============================================
    public:
        void angles_to_string()
        {
            cout<<"----------- Angles des articulations de "<<nom<<endl;
            cout<<"angle_Pda="<<angle_Pda<<" angle_Pde="<<angle_Pde<<" angle_Pdr="<<angle_Pdr<<endl;
            cout<<"angle_Pga="<<angle_Pga<<" angle_Pge="<<angle_Pge<<" angle_Pgr="<<angle_Pgr<<endl;
            cout<<"angle_Cd="<<angle_Cd<<endl;
            cout<<"angle_Cg="<<angle_Cg<<endl;
            cout<<"angle_Eda="<<angle_Eda<<" angle_Ede="<<angle_Ede<<" angle_Edr="<<angle_Edr<<endl;
            cout<<"angle_Ega="<<angle_Ega<<" angle_Ege="<<angle_Ege<<" angle_Egr="<<angle_Egr<<endl;
            cout<<"angle_Vd="<<angle_Vd<<endl;
            cout<<"angle_Vg="<<angle_Vg<<endl;
            cout<<"angle_Gd="<<angle_Gd<<endl;
            cout<<"angle_Gg="<<angle_Gg<<endl;
            cout<<"angle_Hda="<<angle_Hda<<" angle_Hde="<<angle_Hde<<endl;
            cout<<"angle_Hga="<<angle_Hga<<" angle_Hge="<<angle_Hge<<endl;
            cout<<"-----------------------"<<endl;

        }

};

#endif // DKSPERSONNAGE_H_INCLUDED
