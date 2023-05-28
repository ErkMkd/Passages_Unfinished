#ifndef CANAL_ANIMATION_CONTENEUR_H_INCLUDED
#define CANAL_ANIMATION_CONTENEUR_H_INCLUDED

/*
 *
 * 					Canal d'animation de conteneur
 * 					------------------------------
 *
 * Un canal de conteneur n'est pas forcément lié à un conteneur.
 * Le lien avec un conteneur peut se faire via le pointeur public "conteneur".
 *
 * La création des clés est dynamique:
 *
 * 1-Création d'une clé avec "creation_cle()" qui renvoie une cle sans l'ajouter au canal.
 *   Les clés crééent avec cette fonction sont supprimées par le destructeur du canal.
 * 2-On peut initialiser la clé soit en passant par "Canal_Animation.determine_valeur()" et autres...
 * 								soit en appelant "initialise_valeurs_cle()" auquel cas il faut que la canal soit lié à un conteneur
 * 3-On ajoute la clé au canal "via ajoute_cle()"
 *
 * Les raccords entre les clés (notamment pour les trajectoires courbes) sont effectués automatiquement.
 *
*/

#include "../Geometrie_Espace/AxeRotation.h"
#include "Canal_Animation.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                      Indices des paramètres:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef enum
{
    ZPOS=0,
    XPOS,
    YPOS,

    ECHELLE_Z,
    ECHELLE_X,
    ECHELLE_Y,

    ROT_X,	//Position de l'axe de rotation;
    ROT_Y,
    ROT_Z,

    ROT_DIRX, //Direction de l'axe de rotation
    ROT_DIRY,
    ROT_DIRZ,

    ROT_ANGLE,
	ROT_TOURS, //Nombre de tours à ajouter à la rotation de base
    ROT_SENS, //true=même sens que le calcul, false=sens inverse

    AXEZ_Z, //axes du repère
    AXEZ_X,
    AXEZ_Y,

    AXEX_Z,
    AXEX_X,
    AXEX_Y,

    AXEY_Z,
    AXEY_X,
    AXEY_Y,
    TRAJECTOIRE

}PARAMETRES_CANAL_CONTENEUR;

//=========================================================================================
//                                      La classe
//=========================================================================================

class Canal_Animation_Conteneur :public Canal_Animation
{
    public:
		//Paramètres propres au canal:
		Conteneur* conteneur;
    private:
		vec3d position_trajectoire; //Position sur les trajectoires courbes.
		AxeRotation axeRotation ; //L'axe utilisé pour les rotations du conteneur entre deux clés

        Repere repOrg; //Repères utilisés pour les calculs des axes de rotation
		Repere repDest;
    public:

        //========================================================================================
		//                                   Constructeur:
        //========================================================================================

        Canal_Animation_Conteneur(Conteneur* p_conteneur=NULL,vector<Cle_Animation*>* p_cles=NULL,bool p_actif=false)
                                :Canal_Animation(CANAL_ANIMATION_CONTENEUR,p_cles,p_actif)
		{
			if (p_conteneur != NULL) determine_conteneur(p_conteneur);
		}

        //--------- Détermine le conteneur qu'anime le canal:
		void determine_conteneur(Conteneur* cont)
		{
			conteneur = cont;
			//Clé de départ:
			if (cles.size() == 0)
			{
			    Cle_Animation* cle=creation_cle(0.,"Cle de départ");
			    cles.push_back(cle);
			    initialise_valeurs_cle(cle);
			}
		}

        //=================================================================================
		//                   Gestion des clés du canal de conteneur:
        //=================================================================================

        //---------------------------------------------
		//      Initialisation des clés du canal:
        //---------------------------------------------

		virtual void initialise_cle(Cle_Animation* cle)
		{
			//Position du conteneur:
			cle->ajoute_parametre("xpos",XPOS,double(0.f));
			cle->ajoute_parametre("ypos",YPOS,double(0.f));
			cle->ajoute_parametre("zpos",ZPOS,double(0.f));
			cle->ajoute_parametre("trajectoire",TRAJECTOIRE,(CourbeBezier_Espace*)NULL);

            cle->ajoute_parametre("echelle_x",ECHELLE_X,double(1.f));
            cle->ajoute_parametre("echelle_y",ECHELLE_Y,double(1.f));
            cle->ajoute_parametre("echelle_z",ECHELLE_Z,double(1.f));

			//Axe de rotation propre du conteneur.
			cle->ajoute_parametre("rot_x",ROT_X,double(0.f));
			cle->ajoute_parametre("rot_y",ROT_Y,double(0.f));
			cle->ajoute_parametre("rot_z",ROT_Z,double(0.f));

			cle->ajoute_parametre("rot_dir_x",ROT_DIRX,double(0.f));
			cle->ajoute_parametre("rot_dir_y",ROT_DIRY,double(0.f));
			cle->ajoute_parametre("rot_dir_z",ROT_DIRZ,double(0.f));

			cle->ajoute_parametre("rot_tours",ROT_TOURS,int32_t(0));
			cle->ajoute_parametre("rot_sens",ROT_SENS,bool(true));
			cle->ajoute_parametre("rot_angle",ROT_ANGLE,double(0.f));

			//Le repère momentané, qui sert à calculer le mouvement de rotation:
			cle->ajoute_parametre("axeX_x",AXEX_X,double(0.f));
			cle->ajoute_parametre("axeX_y",AXEX_Y,double(0.f));
			cle->ajoute_parametre("axeX_z",AXEX_Z,double(0.f));

			cle->ajoute_parametre("axeY_x",AXEY_X,double(0.f));
			cle->ajoute_parametre("axeY_y",AXEY_Y,double(0.f));
			cle->ajoute_parametre("axeY_z",AXEY_Z,double(0.f));

			cle->ajoute_parametre("axeZ_x",AXEZ_X,double(0.f));
			cle->ajoute_parametre("axeZ_y",AXEZ_Y,double(0.f));
			cle->ajoute_parametre("axeZ_z",AXEZ_Z,double(0.f));
        }

        //------------------------------------------------
        //  Interpolation du conteneur à l'instant t:
        //------------------------------------------------
        // Il n'y a pas de test d'activation du canal, cela permet
        // d'initialiser le conteneur à n'importe quel moment de son animation.
		virtual void initialise_objet(double t)
		{
		    if ( (conteneur==NULL) )return;

			t -= position_depart;
			//if (t >= duree) desactive(); //??

			Cle_Animation* cle_prec = renvoie_cle_precedente_proche(t);
			Cle_Animation* cle_suiv = renvoie_cle_suivante_proche(t);

			/*
			if (cles.length > 1)
			{
				trace("Cléfs:");
				for each (var clef:Cle_Animation in cles)
				{trace("cle:", clef.t);}
			}
			if (cle_prec.t > 0)
			{
				trace("cle_prec:", cle_prec);
				trace("cle_suiv:", cle_suiv);
			}
			*/

			//var p_org:Array = cle_prec.parametres;

			conteneur->axe_x->x = *((double*)cle_prec->renvoie_valeur(AXEX_X));
			conteneur->axe_x->y = *((double*)cle_prec->renvoie_valeur(AXEX_Y));
			conteneur->axe_x->z = *((double*)cle_prec->renvoie_valeur(AXEX_Z));

			conteneur->axe_y->x = *((double*)cle_prec->renvoie_valeur(AXEY_X));
			conteneur->axe_y->y = *((double*)cle_prec->renvoie_valeur(AXEY_Y));
			conteneur->axe_y->z = *((double*)cle_prec->renvoie_valeur(AXEY_Z));

			conteneur->axe_z->x = *((double*)cle_prec->renvoie_valeur(AXEZ_X));
			conteneur->axe_z->y = *((double*)cle_prec->renvoie_valeur(AXEZ_Y));
			conteneur->axe_z->z = *((double*)cle_prec->renvoie_valeur(AXEZ_Z));


			if ((cle_prec->t == t) || (cle_suiv == NULL) || (!cle_prec->interpolations_activees()))
			{
				conteneur->determine_position( *((double*)cle_prec->renvoie_valeur(XPOS)), *((double*)cle_prec->renvoie_valeur(YPOS)), *((double*)cle_prec->renvoie_valeur(ZPOS)) );

				conteneur->echelle_x = *((double*)cle_prec->renvoie_valeur(ECHELLE_X));
				conteneur->echelle_y = *((double*)cle_prec->renvoie_valeur(ECHELLE_Y));
				conteneur->echelle_z = *((double*)cle_prec->renvoie_valeur(ECHELLE_Z));
			}

			else if (cle_suiv != NULL && cle_prec->interpolations_activees())
			{
				// !!!!!!! GERER DIFFERAMMENT LES DRAPEAUX "INTERPOLATION" DES PARAMETRES !!!!!!!!!!!!!!
				//p_org = cle_prec.parametres;
				//var p_dst:Array = cle_suiv.parametres;
				double tOrg = cle_prec->t;
				double tprop = (t - tOrg) / (cle_suiv->t - tOrg);

				//---------- Déplacement:
				uint16_t trajectoire_type = cle_prec->renvoie_type_trajectoire(TRAJECTOIRE); //p_org[TRAJECTOIRE].methodeInterpolation;
				if ( trajectoire_type == TRAJECTOIRE_RECTILIGNE)
				{
					conteneur->determine_position
                                (
                                    calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(XPOS),cle_suiv->renvoie_parametre(XPOS), tprop),
                                    calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(YPOS),cle_suiv->renvoie_parametre(YPOS), tprop),
                                    calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(ZPOS),cle_suiv->renvoie_parametre(ZPOS), tprop)
                                );
				}
				else if (trajectoire_type == TRAJECTOIRE_COURBE)
				{
					///////////////////// ATTENTION AU LIEN !!!!
					calcul_interpolation_trajectoire_courbe((Parametre_Animation_TRAJECTOIRE*)cle_prec->renvoie_parametre(TRAJECTOIRE),tprop,&position_trajectoire);
					//position_trajectoire.toString();
					conteneur->determine_position( position_trajectoire.x, position_trajectoire.y, position_trajectoire.z );
				}
				/*
				else if (trajectoire_type == Interpolation.TRAJECTOIRE_CIRCULAIRE)
				{
					calcul_interpolation_trajectoire_circulaire(cle_prec,p_org[TRAJECTOIRE].valeur,p_org[XPOS],tprop,position_trajectoire);
					conteneur.determine_position( position_trajectoire.x, position_trajectoire.y, position_trajectoire.z );
				}
				*/

				//------------Echelle:
				conteneur->determine_echelle_repere
                                    (
                                        calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(ECHELLE_X),cle_suiv->renvoie_parametre(ECHELLE_X), tprop),
                                        calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(ECHELLE_Y),cle_suiv->renvoie_parametre(ECHELLE_Y), tprop),
                                        calcul_interpolation_FLOTTANT(cle_prec->renvoie_parametre(ECHELLE_Z),cle_suiv->renvoie_parametre(ECHELLE_Z), tprop)
                                    );

				//------------Rotation:
                double angle_rotation=*((double*)cle_prec->renvoie_valeur(ROT_ANGLE));
				if (angle_rotation != 0)
				{

				    /*
					if (p_org[TRAJECTOIRE].methodeInterpolation == Interpolation.TRAJECTOIRE_CIRCULAIRE)
					{
						axeRotation.x = Vecteur_Unitaire(p_org[TRAJECTOIRE].valeur).x;
						axeRotation.y = Vecteur_Unitaire(p_org[TRAJECTOIRE].valeur).y;
						axeRotation.z = Vecteur_Unitaire(p_org[TRAJECTOIRE].valeur).z;
					}

					else
					{
					*/
						axeRotation.x = *((double*)cle_prec->renvoie_valeur(ROT_DIRX));
						axeRotation.y = *((double*)cle_prec->renvoie_valeur(ROT_DIRY));
						axeRotation.z = *((double*)cle_prec->renvoie_valeur(ROT_DIRZ));
						//cout<<"X: "<<axeRotation.x<<" Y: "<<axeRotation.y<<" Z: "<<axeRotation.z<<endl;
					//}
					angle_rotation = calcul_interpolation_rotation(cle_prec->renvoie_parametre(ROT_ANGLE), tprop);
					//cout<<"ANGLE DE ROTATION:"<<angle_rotation/M_PI*180.<<endl;
					conteneur->rotation(angle_rotation, &axeRotation);
				}
			}
            //cout<<"X: "<<conteneur->axe_x->x<<" Y: "<<conteneur->axe_x->y<<" Z: "<<conteneur->axe_x->z<<endl;
			conteneur->a_ete_deplace();
		}

        //---------------------------------------------------------------------------------------
		//Initialisation des valeurs de la clé en fonction des paramètres actuels du conteneur
		//  Les paramètres ROT_TOURS et ROT_SENS ne sont pas modifiés, mais ils sont pris en compte
		//  pour le calcul de l'angle.
        //---------------------------------------------------------------------------------------
		virtual void initialise_valeurs_cle(Cle_Animation* cle)
		{
		    if (conteneur==NULL)return;
		    //Enregistrement de la position et du repère:

			cle->determine_valeur(XPOS,&conteneur->x);
			cle->determine_valeur(YPOS,&conteneur->y);
			cle->determine_valeur(ZPOS,&conteneur->z);

			cle->determine_valeur(ECHELLE_X,&conteneur->echelle_x);
			cle->determine_valeur(ECHELLE_Y,&conteneur->echelle_y);
			cle->determine_valeur(ECHELLE_Z,&conteneur->echelle_z);

			cle->determine_valeur(AXEX_X,&conteneur->axe_x->x);
			cle->determine_valeur(AXEX_Y,&conteneur->axe_x->y);
			cle->determine_valeur(AXEX_Z,&conteneur->axe_x->z);

			cle->determine_valeur(AXEY_X,&conteneur->axe_y->x);
			cle->determine_valeur(AXEY_Y,&conteneur->axe_y->y);
			cle->determine_valeur(AXEY_Z,&conteneur->axe_y->z);

			cle->determine_valeur(AXEZ_X,&conteneur->axe_z->x);
			cle->determine_valeur(AXEZ_Y,&conteneur->axe_z->y);
			cle->determine_valeur(AXEZ_Z,&conteneur->axe_z->z);

			//cout<<"AXE_X x="<<conteneur->axe_x->x<<" y="<<conteneur->axe_x->y<<" z="<<conteneur->axe_x->z<<endl;

			//Mise à jour de la courbe de la trajectoire:
			CourbeBezier_Espace* trajectoire=cle->renvoie_courbe_trajectoire(TRAJECTOIRE);
			if (trajectoire!=NULL)
			{
				position_trajectoire.x = conteneur->x;
				position_trajectoire.y = conteneur->y;
				position_trajectoire.z = conteneur->z;
				trajectoire->determine_A0(&position_trajectoire);
				Cle_Animation* cle_prec = renvoie_cle_precedente(cle);
				if (cle_prec != NULL)
				{
					trajectoire = cle_prec->renvoie_courbe_trajectoire(TRAJECTOIRE);
					if (trajectoire != NULL) trajectoire->determine_B0(&position_trajectoire);
				}
			}

			//Raccord:
			raccorde_cles(cle,renvoie_cle_suivante(cle));
			raccorde_cles(renvoie_cle_precedente(cle),cle);

			//Calcul la rotation de base:

			//rafraichi_rotation(cle); //Rafraichissement de l'axe à chaque init d'objet.
		}

		// Calcul de la rotation si il y a une clé suivante.
		void rafraichi_rotation(Cle_Animation* cle)
		{
			Cle_Animation* cle_suiv = renvoie_cle_suivante(cle);
			if (cle_suiv != NULL) calcul_rotation(cle, cle_suiv);
		}

		//Détermine le nombre de tours et le sens dans une interpolation de rotation:
		void determine_parametres_rotation(Cle_Animation* cle,int32_t p_tours,bool p_sens)
		{
		    cle->determine_valeur(ROT_TOURS,&p_tours);
		    cle->determine_valeur(ROT_SENS,&p_sens);
		    rafraichi_rotation(cle);
		}

    protected:
		//Calcul l'axe de rotation entre deux clés.
		//Les paramètres de la clé d'origine sont réinitialisés.
		void calcul_rotation(Cle_Animation* cle_org, Cle_Animation* cle_dest)
		{
			Vecteur_Unitaire* vtemp = repOrg.axe_x;
			vtemp->x = *((double*)cle_org->renvoie_valeur(AXEX_X));
			vtemp->y = *((double*)cle_org->renvoie_valeur(AXEX_Y));
			vtemp->z = *((double*)cle_org->renvoie_valeur(AXEX_Z));

			vtemp = repOrg.axe_y;
			vtemp->x = *((double*)cle_org->renvoie_valeur(AXEY_X));
			vtemp->y = *((double*)cle_org->renvoie_valeur(AXEY_Y));
			vtemp->z = *((double*)cle_org->renvoie_valeur(AXEY_Z));

            vtemp = repOrg.axe_z;
			vtemp->x = *((double*)cle_org->renvoie_valeur(AXEZ_X));
			vtemp->y = *((double*)cle_org->renvoie_valeur(AXEZ_Y));
			vtemp->z = *((double*)cle_org->renvoie_valeur(AXEZ_Z));


			vtemp = repDest.axe_x;
			vtemp->x = *((double*)cle_dest->renvoie_valeur(AXEX_X));
			vtemp->y = *((double*)cle_dest->renvoie_valeur(AXEX_Y));
			vtemp->z = *((double*)cle_dest->renvoie_valeur(AXEX_Z));

			vtemp = repDest.axe_y;
			vtemp->x = *((double*)cle_dest->renvoie_valeur(AXEY_X));
			vtemp->y = *((double*)cle_dest->renvoie_valeur(AXEY_Y));
			vtemp->z = *((double*)cle_dest->renvoie_valeur(AXEY_Z));

			vtemp = repDest.axe_z;
			vtemp->x = *((double*)cle_dest->renvoie_valeur(AXEZ_X));
			vtemp->y = *((double*)cle_dest->renvoie_valeur(AXEZ_Y));
			vtemp->z = *((double*)cle_dest->renvoie_valeur(AXEZ_Z));

			axeRotation.calcul_rotation_entre_deux_reperes(&repOrg, &repDest);

			cle_org->determine_valeur(ROT_DIRX,&axeRotation.x);
			cle_org->determine_valeur(ROT_DIRY,&axeRotation.y);
			cle_org->determine_valeur(ROT_DIRZ,&axeRotation.z);

			double alpha= axeRotation.angle;

			if (!*((bool*)cle_org->renvoie_valeur(ROT_SENS)))
			{
				alpha -= M_PI*2.;
				alpha -= M_PI*2. * double(*((int32_t*)cle_org->renvoie_valeur(ROT_TOURS)));
			}
			else
			{
				alpha += M_PI*2. * double(*((int32_t*)cle_org->renvoie_valeur(ROT_TOURS)));
			}

			cle_org->determine_valeur(ROT_ANGLE,&alpha);

		}

    public:
		//Raccord entre deux clés, après suppression ou insertion d'autres clés:
		//La trajectoire et la rotation sont mises à jour.
		virtual void raccorde_cles(Cle_Animation* cle_prec, Cle_Animation* cle_suiv)
		{
			if ((cle_prec != NULL) && (cle_suiv != NULL))
			{
				CourbeBezier_Espace* trajectoire = cle_prec->renvoie_courbe_trajectoire(TRAJECTOIRE);
				if (trajectoire != NULL)
				{
					position_trajectoire.x = *((double*)cle_suiv->renvoie_valeur(XPOS));
					position_trajectoire.y = *((double*)cle_suiv->renvoie_valeur(YPOS));
					position_trajectoire.z = *((double*)cle_suiv->renvoie_valeur(ZPOS));
					trajectoire->determine_B0(&position_trajectoire);
					//trajectoire->toString();
				}
				calcul_rotation(cle_prec,cle_suiv);
			}
		}
};


#endif // CANAL_ANIMATION_CONTENEUR_H_INCLUDED
