#ifndef CLE_CANAL_CONTENEUR_H_INCLUDED
#define CLE_CANAL_CONTENEUR_H_INCLUDED

#include "Cle_Animation.h"

//Indices des paramètres:
typedef enum
{
    XPOS=0,
    YPOS,
    ZPOS,

    ECHELLE_X,
    ECHELLE_Y,
    ECHELLE_Z,

    ROT_X,	//Position de l'axe de rotation;
    ROT_Y,
    ROT_Z,

    ROT_DIRX, //Direction de l'axe de rotation
    ROT_DIRY,
    ROT_DIRZ,

    ROT_ANGLE,
		//Pas besoin de conserver le sens et le nombre de tours. Seul l'angle de rotation est nécessaire.
		//Pour la compatibilité avec Perspective_DKS, les identifiants sont tout de même créés.
	ROT_TOURS, //Nombre de tours à ajouter à la rotation
    ROT_SENS, //true=même sens que le calcul, false=sens inverse

    AXEX_X, //axes du repère
    AXEX_Y,
    AXEX_Z,

    AXEY_X,
    AXEY_Y,
    AXEY_Z,

    AXEZ_X,
    AXEZ_Y,
    AXEZ_Z,
    TRAJECTOIRE

}PARAMETRES_CANAL_CONTENEUR;

//===============================================================================
//                  Définition des clés d'animation d'un canal de conteneur
//===============================================================================
class Cle_Canal_Conteneur :public Cle_Animation
{
    public:

        Parametre_Animation_FLOTTANT xpos;
        Parametre_Animation_FLOTTANT ypos;
        Parametre_Animation_FLOTTANT zpos;

        Parametre_Animation_FLOTTANT echelle_x;
        Parametre_Animation_FLOTTANT echelle_y;
        Parametre_Animation_FLOTTANT echelle_z;
        Parametre_Animation_FLOTTANT rot_x;
        Parametre_Animation_FLOTTANT rot_y;
        Parametre_Animation_FLOTTANT rot_z;
        Parametre_Animation_FLOTTANT rot_dir_x;
        Parametre_Animation_FLOTTANT rot_dir_y;
        Parametre_Animation_FLOTTANT rot_dir_z;
        Parametre_Animation_FLOTTANT rot_angle;

        Parametre_Animation_FLOTTANT axeX_x;
        Parametre_Animation_FLOTTANT axeX_y;
        Parametre_Animation_FLOTTANT axeX_z;

        Parametre_Animation_FLOTTANT axeY_x;
        Parametre_Animation_FLOTTANT axeY_y;
        Parametre_Animation_FLOTTANT axeY_z;

        Parametre_Animation_FLOTTANT axeZ_x;
        Parametre_Animation_FLOTTANT axeZ_y;
        Parametre_Animation_FLOTTANT axeZ_z;

        Parametre_Animation_TRAJECTOIRE trajectoire;

        //============================================================
        //                          CONSTRUCTEUR
        //============================================================
        Cle_Canal_Conteneur(double p_t=0.,string p_nom=""):Cle_Animation(p_t,p_nom)
        {
            initialise_parametres();
        }

        //---------------DESTRUCTEUR:
        virtual ~Cle_Canal_Conteneur()
        {
        }

        //--------------------Initialise les paramètres
        virtual void initialise_parametres()
        {
            xpos.initialise_identite("xpos",XPOS);
            ypos.initialise_identite("ypos",YPOS);
            zpos.initialise_identite("zpos",ZPOS);

            echelle_x.initialise_identite("echelle_x",ECHELLE_X);
            echelle_y.initialise_identite("echelle_y",ECHELLE_Y);
            echelle_z.initialise_identite("echelle_z",ECHELLE_Z);

            rot_x.initialise_identite("rot_x",ROT_X);
            rot_y.initialise_identite("rot_y",ROT_Y);
            rot_z.initialise_identite("rot_z",ROT_Z);
            rot_dir_x.initialise_identite("rot_dir_x",ROT_DIRX);
            rot_dir_y.initialise_identite("rot_dir_y",ROT_DIRY);
            rot_dir_z.initialise_identite("rot_dir_z",ROT_DIRZ);
            rot_angle.initialise_identite("rot_angle",ROT_ANGLE);

            axeX_x.initialise_identite("axeX_x",AXEX_X);
            axeX_y.initialise_identite("axeX_y",AXEX_Y);
            axeX_z.initialise_identite("axeX_z",AXEX_Z);

            axeY_x.initialise_identite("axeY_x",AXEY_X);
            axeY_y.initialise_identite("axeY_y",AXEY_Y);
            axeY_z.initialise_identite("axeY_z",AXEY_Z);

            axeZ_x.initialise_identite("axeZ_x",AXEZ_X);
            axeZ_y.initialise_identite("axeZ_y",AXEZ_Y);
            axeZ_z.initialise_identite("axeZ_z",AXEZ_Z);

            trajectoire.initialise_identite("trajectoire",TRAJECTOIRE);
        }

        //==============================================================
        //      Affiche le contenu de la clé, pour les débugages:
        //==============================================================
        virtual void toString()
        {
            Cle_Animation::toString();

            xpos.toString();
            ypos.toString();
            zpos.toString();

            echelle_x.toString();
            echelle_y.toString();
            echelle_z.toString();
            rot_x.toString();
            rot_y.toString();
            rot_z.toString();
            rot_dir_x.toString();
            rot_dir_y.toString();
            rot_dir_z.toString();
            rot_angle.toString();

            axeX_x.toString();
            axeX_y.toString();
            axeX_z.toString();

            axeY_x.toString();
            axeY_y.toString();
            axeY_z.toString();

            axeZ_x.toString();
            axeZ_y.toString();
            axeZ_z.toString();

            trajectoire.toString();
        }
};

#endif // CLE_CANAL_CONTENEUR_H_INCLUDED
