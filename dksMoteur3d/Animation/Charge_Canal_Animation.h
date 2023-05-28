#ifndef CHARGE_CANAL_ANIMATION_H_INCLUDED
#define CHARGE_CANAL_ANIMATION_H_INCLUDED

//======================================================================================================
//
//                          Charge un canal à partir d'un script XML
//
//      ATTENTION: Il ne faut pas détruire l'objet une fois le canal chargé, car le destructeur
//                  se charge de détruire les courbes et les canaux.
//
//======================================================================================================

#include "../../XML/tinyxml/tinyxml.h"
#include "Canal_Animation_Conteneur.h"
#include <vector>
#include <string.h>

#define DEBUG_CHARGEMENT_CANAL false

//=========== Codes d'erreurs:

#define CHARGE_CANAL_ANIMATION_OK 0
#define CHARGE_CANAL_ANIMATION_FICHIER 1
#define CHARGE_CANAL_ANIMATION_FORMAT 2
#define CHARGE_CANAL_ANIMATION_MEMOIRE 3

class Charge_Canal_Animation
{
    public:
        uint16_t erreur;
        string nom_fichier;
        TiXmlDocument canauxXML;

    private:
        vector<Canal_Animation*> canaux;
        vector<CourbeBezier_Plan*> courbes_interpolations_creees; //Pour libérer proprement la mémoire.
        vector<CourbeBezier_Espace*> courbes_trajectoires_creees;

        //==================================================================
        //                          CONSTRUCTEUR
        //==================================================================
    public:
        Charge_Canal_Animation(string p_nom_fichier)
        {
            nom_fichier=p_nom_fichier;
            //Charge le fichier:
            erreur=CHARGE_CANAL_ANIMATION_OK;
            if ( ! canauxXML.LoadFile((char*)&nom_fichier[0]))
            {
                erreur=CHARGE_CANAL_ANIMATION_FICHIER;
                return;
            }

            //Test le format du script XML:
            TiXmlHandle canauxXml_handle(&canauxXML);

            TiXmlElement* en_tete = canauxXml_handle.FirstChildElement("Canaux").ToElement();
            if (en_tete==NULL)
            {
                if (DEBUG_CHARGEMENT_CANAL) cout <<"! ERREUR dans Charge_Canal_Animation() - Le format du fichier n'est pas bon "<<nom_fichier<<endl;
                erreur=CHARGE_CANAL_ANIMATION_FORMAT;
                return;
            }

            //Charge les canaux:

            TiXmlElement* canalXml=canauxXml_handle.FirstChildElement("Canaux").FirstChildElement("Canal").ToElement();
            creation_canaux(canalXml);
        }

        //================== DESTRUCTEUR:

        virtual ~Charge_Canal_Animation()
        {
            uint32_t i;
            //Efface les canaux chargés:
            for (i=0;i<canaux.size();i++)
            {
                if (canaux[i]!=NULL)
                {
                    delete canaux[i];
                    canaux[i]=NULL;
                }
            }
            //Efface les courbes plan:
            for (i=0;i< courbes_interpolations_creees.size();i++)
            {
                if (courbes_interpolations_creees[i]!=NULL) delete courbes_interpolations_creees[i];
            }

            //Efface les courbes spatiales:
            for (i=0;i< courbes_trajectoires_creees.size();i++)
            {
                if (courbes_trajectoires_creees[i]!=NULL) delete courbes_trajectoires_creees[i];
            }
        }

        //==================================================================================
        //              Création des canaux
        //==================================================================================
        bool creation_canaux(TiXmlElement* canalXml)
        {
             while (canalXml!=NULL)
            {

                string type_canal=canalXml->Attribute("type");
                string nom_canal=canalXml->Attribute("nom");

                //Création du canal:
                Canal_Animation* nouveau_canal;

                if (type_canal=="CONTENEUR" || type_canal=="VOLUME" || type_canal=="OEIL" || type_canal=="SOURCE_LUMINEUSE")
                {
                    nouveau_canal=creation_Canal_Conteneur(canalXml);
                    if (nouveau_canal==NULL) return false;
                    nouveau_canal->nom=nom_canal;
                    canaux.push_back(nouveau_canal);
                }
                /*
                else if (type_canal=="SOURCE_LUMINEUSE")
                {
                }
                */
                else if (type_canal=="SCENE")
                {
                }

                else if (type_canal=="HIERARCHIE")
                {
                }

                canalXml=canalXml->NextSiblingElement("Canal");
            }

            return true;
        }

        //==================================================================
        //                 Création d'un canal d'animation de conteneur
        //==================================================================

        Canal_Animation_Conteneur* creation_Canal_Conteneur(TiXmlElement* canalXml)
        {
            string booleen;

            string nom_canal=canalXml->Attribute("nom");
            bool canal_active=false;

            booleen=canalXml->Attribute("active");
            if (booleen=="true")canal_active=true;

            if ( DEBUG_CHARGEMENT_CANAL )
                cout<<"--- Création d'un canal de Conteneur: "<<nom_canal<<" - Activé: "<<booleen<<endl;


            //Canal_Animation_Conteneur(Conteneur* p_conteneur=NULL,vector<Cle_Animation*>* p_cles=NULL,bool p_actif=false)
            //                    :Canal_Animation(CANAL_ANIMATION_CONTENEUR,p_cles,p_actif)
            Canal_Animation_Conteneur* nouveau_canal=new Canal_Animation_Conteneur(NULL,NULL,canal_active);
            if (nouveau_canal==NULL)
            {
                cout<<"ERREUR dans Charge_Canal_Animation.creation_Canal_Conteneur() - Impossible de créé le canal"<<endl;
                erreur=CHARGE_CANAL_ANIMATION_MEMOIRE;
                return NULL;
            }

            //------Création des clés:
            TiXmlElement* cleXML=canalXml->FirstChildElement("Cle");
            while (cleXML!=NULL)
            {
                double cle_t;
                booleen=cleXML->Attribute("interpolation");
                cleXML->QueryDoubleAttribute("t",&cle_t);

                if ( DEBUG_CHARGEMENT_CANAL ) cout<<"  ->Cle - t:"<<cle_t<<" - interpolations:"<<booleen<<endl;
                Cle_Animation* nouvelle_cle=nouveau_canal->creation_cle(cle_t);
                //Drapeau principal des interpolations:
                if (booleen=="true")nouvelle_cle->active_interpolations();
                else nouvelle_cle->desactive_interpolations();
                //Initialise les valeurs de la clé:
                if (!lecture_parametres_cle(nouvelle_cle,cleXML))
                {
                    cout<<"ERREUR dans Charge_Canal_Animation.creation_Canal_Conteneur() - Impossible de lire les paramètres de la clé: "<<cle_t<<endl;
                    return NULL;
                }
                //Ajoute la clé au canal, au passage la rotation est calculée.
                nouveau_canal->ajoute_cle(nouvelle_cle);
                //Clé suivante:
                cleXML=cleXML->NextSiblingElement("Cle");

            }

            //---- Initialise les drapeaux d'interpolations:
            initialise_interpolations_canal_conteneur(nouveau_canal);

            return nouveau_canal;
        }

        //-------------- Initialise les drapeaux d'interpolation des parametres du canal de conteneur:
        void initialise_interpolations_canal_conteneur( Canal_Animation_Conteneur* canal)
        {
            Cle_Animation* cle_prec;
            Cle_Animation* cle_suiv;
            cle_prec= canal->renvoie_cle(0);
            if (cle_prec==NULL) return;
            cle_suiv=canal->renvoie_cle_suivante(cle_prec);
            if (cle_suiv==NULL) cle_prec->desactive_interpolations();
            while(cle_suiv!=NULL)
            {
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(XPOS),cle_prec,cle_suiv);
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(YPOS),cle_prec,cle_suiv);
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(ZPOS),cle_prec,cle_suiv);
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(ECHELLE_X),cle_prec,cle_suiv);
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(ECHELLE_Y),cle_prec,cle_suiv);
                determine_interpolation(PARAMETRES_CANAL_CONTENEUR(ECHELLE_Z),cle_prec,cle_suiv);
                determine_interpolation_rotation(PARAMETRES_CANAL_CONTENEUR(ROT_ANGLE),cle_prec);
                //Interpolation de la trajectoire:

                if (cle_prec->renvoie_type_trajectoire(PARAMETRES_CANAL_CONTENEUR(TRAJECTOIRE))==TRAJECTOIRE_COURBE)
                {
                    if (
                            cle_prec->interpolation_activee(PARAMETRES_CANAL_CONTENEUR(XPOS))
                            || cle_prec->interpolation_activee(PARAMETRES_CANAL_CONTENEUR(YPOS))
                            || cle_prec->interpolation_activee(PARAMETRES_CANAL_CONTENEUR(ZPOS))

                        )
                    {
                        cle_prec->active_interpolation(PARAMETRES_CANAL_CONTENEUR(TRAJECTOIRE));
                        cle_prec->determine_methode_interpolation(PARAMETRES_CANAL_CONTENEUR(TRAJECTOIRE),
                                cle_prec->renvoie_methode_interpolation(PARAMETRES_CANAL_CONTENEUR(XPOS)));
                        cle_prec->determine_courbe_interpolation(PARAMETRES_CANAL_CONTENEUR(TRAJECTOIRE),
                                cle_prec->renvoie_courbe_interpolation(PARAMETRES_CANAL_CONTENEUR(XPOS)));
                    }
                }


                cle_prec=cle_suiv;
                cle_suiv=canal->renvoie_cle_suivante(cle_prec);
            }
        }


        //=====================================================================================
        //      Décrypte les paramètres XML d'une clé
        //      La clé doit être correctement initialisée
        //      Les drapeaux d'interpolation seront initialisés par initialise_interpolations()
        //      une fois toutes les clés décryptées.
         //=====================================================================================

        bool lecture_parametres_cle(Cle_Animation* cle,TiXmlElement* cleXML)
        {
            int entier;
            double flottant;
            string booleen;
            TiXmlElement* courbeXML;

            TiXmlElement* parametreXML = cleXML->FirstChildElement("parametre");
            while (parametreXML!=NULL)
            {
                string nom_parametre=parametreXML->Attribute("nom");
                string type_parametre=parametreXML->Attribute("type");
                int id_parametre;
                parametreXML->QueryIntAttribute("index",&id_parametre);

                if ( DEBUG_CHARGEMENT_CANAL )
                cout<<"      - parametre "<<id_parametre<<" - "<<nom_parametre<<" - type: "<<type_parametre<<endl;

                //Lecture des types FLOTTANT et ENTIER:
                if (type_parametre=="FLOTTANT" || type_parametre=="ENTIER")
                {
                    string methode_interpolation=parametreXML->Attribute("methodeInterpolation");
                    if ( DEBUG_CHARGEMENT_CANAL ) cout <<"        >Methode d'interpolation: "<<methode_interpolation<<endl;
                    if (methode_interpolation=="LINEAIRE") cle->determine_methode_interpolation(id_parametre,INTERPOLATION_LINEAIRE);
                    else if (methode_interpolation=="BEZIER")
                    {
                        cle->determine_methode_interpolation(id_parametre,INTERPOLATION_BEZIER);
                        courbeXML=parametreXML->FirstChildElement("Bezier")->ToElement();
                        if(courbeXML!=NULL)
                        {
                            double xa0,ya0,xa1,ya1,xb0,yb0,xb1,yb1;
                            courbeXML->QueryDoubleAttribute("XA0",&xa0);
                            courbeXML->QueryDoubleAttribute("YA0",&ya0);
                            courbeXML->QueryDoubleAttribute("XA1",&xa1);
                            courbeXML->QueryDoubleAttribute("YA1",&ya1);
                            courbeXML->QueryDoubleAttribute("XB0",&xb0);
                            courbeXML->QueryDoubleAttribute("YB0",&yb0);
                            courbeXML->QueryDoubleAttribute("XB1",&xb1);
                            courbeXML->QueryDoubleAttribute("YB1",&yb1);
                            xa1+=xa0;
                            ya1+=ya0;
                            xb1+=xb0;
                            yb1+=yb0;
                            if ( DEBUG_CHARGEMENT_CANAL )
                            {
                                cout<<"        > Courbe d'interpolation: P0X:"<<xa0<<" P0Y:"<<ya0<<endl;
                                cout<<"                            "<<" P1X:"<<xa1<<" P1Y:"<<ya1<<endl;
                                cout<<"                            "<<" P2X:"<<xb1<<" P2Y:"<<yb1<<endl;
                                cout<<"                            "<<" P3X:"<<xb0<<" P3Y:"<<yb0<<endl;
                            }
                            CourbeBezier_Plan* courbe_plan=new CourbeBezier_Plan(xa0,ya0,xa1,ya1,xb1,yb1,xb0,yb0,0.5);
                            if (courbe_plan==NULL)
                            {
                                cout<<"ERREUR impossible de créer la courbe d'interpolation - "<<cle->t<<" - parametre: "<<nom_parametre<<endl;
                                cle->determine_methode_interpolation(id_parametre,INTERPOLATION_LINEAIRE);
                            }
                            else
                            {
                                cle->determine_courbe_interpolation(id_parametre,courbe_plan);
                                courbes_interpolations_creees.push_back(courbe_plan);
                            }
                        }
                        //Si la courbe n'est pas définie, on passe en mode linéaire:
                        else
                        {
                            cout<<"ERREUR impossible de créer la courbe d'interpolation - "<<cle->t<<" - parametre: "<<nom_parametre<<endl;
                            cle->determine_methode_interpolation(id_parametre,INTERPOLATION_LINEAIRE);
                        }

                    }
                    else cle->determine_methode_interpolation(id_parametre,INTERPOLATION_BASCULE);

                    //Lecture de la valeur du paramètre:
                    if (type_parametre=="ENTIER")
                    {
                        parametreXML->QueryIntAttribute("valeur",&entier);
                        cle->determine_valeur(id_parametre,&entier);
                    }
                    else if (type_parametre=="FLOTTANT")
                    {
                        parametreXML->QueryDoubleAttribute("valeur",&flottant);
                        cle->determine_valeur(id_parametre,&flottant);
                    }
                }

                //Lecture du type BOOLEEN:
                else if (type_parametre=="BOOLEEN")
                {
                    booleen=parametreXML->Attribute("valeur");
                    bool drapeau;
                    if (booleen=="true")drapeau=true; else drapeau=false;
                    cle->determine_valeur(id_parametre,&drapeau);
                }

                //Lecture du type TRAJECTOIRE:
                else if (type_parametre=="TRAJECTOIRE")
                {
                    string type_trajectoire=parametreXML->Attribute("methodeInterpolation");
                    if (type_trajectoire=="TRAJECTOIRE_COURBE")
                    {
                        courbeXML=parametreXML->FirstChildElement("Trajectoire")->ToElement();
                        cle->determine_type_trajectoire(id_parametre,TRAJECTOIRE_COURBE);
                        double p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z,p3x,p3y,p3z;
                        courbeXML->QueryDoubleAttribute("P0X",&p0z);
                        courbeXML->QueryDoubleAttribute("P0Y",&p0x);
                        courbeXML->QueryDoubleAttribute("P0Z",&p0y);
                        courbeXML->QueryDoubleAttribute("P1X",&p1z);
                        courbeXML->QueryDoubleAttribute("P1Y",&p1x);
                        courbeXML->QueryDoubleAttribute("P1Z",&p1y);
                        courbeXML->QueryDoubleAttribute("P2X",&p2z);
                        courbeXML->QueryDoubleAttribute("P2Y",&p2x);
                        courbeXML->QueryDoubleAttribute("P2Z",&p2y);
                        courbeXML->QueryDoubleAttribute("P3X",&p3z);
                        courbeXML->QueryDoubleAttribute("P3Y",&p3x);
                        courbeXML->QueryDoubleAttribute("P3Z",&p3y);

                        if ( DEBUG_CHARGEMENT_CANAL )
                        {
                            cout<<"        > Courbe de trajectoire: P0X:"<<p0x<<" P0Y:"<<p0y<<" P0Z:"<<p0z<<endl;
                            cout<<"                            "<<" P1X:"<<p1x<<" P1Y:"<<p1y<<" P1Z:"<<p1z<<endl;
                            cout<<"                            "<<" P2X:"<<p2x<<" P2Y:"<<p2y<<" P2Z:"<<p2z<<endl;
                            cout<<"                            "<<" P3X:"<<p3x<<" P3Y:"<<p3y<<" P3Z:"<<p3z<<endl;
                        }
                        CourbeBezier_Espace* courbe_espace=new CourbeBezier_Espace(p0x,p0y,p0z,p1x,p1y,p1z,p2x,p2y,p2z,p3x,p3y,p3z);
                        if (courbe_espace==NULL)
                        {
                            cout<<"ERREUR impossible de créer la courbe de trajectoire - "<<cle->t<<" - parametre: "<<nom_parametre<<endl;
                            cle->determine_type_trajectoire(id_parametre,TRAJECTOIRE_RECTILIGNE);
                        }
                        else
                        {
                            cle->determine_valeur(id_parametre,courbe_espace);
                            courbes_trajectoires_creees.push_back(courbe_espace);
                        }
                    }
                    else
                    {
                        cle->determine_type_trajectoire(id_parametre,TRAJECTOIRE_RECTILIGNE);
                    }
                }


                parametreXML=parametreXML->NextSiblingElement("parametre");
            }

            return true;
        }

        //Détermine si il y a interpolation entre deux parametres de clés d'animation:
		//Il y a interpolation:
		//     -si l'interpolation de la clé est activée
		//	ET -si il y a une clé suivante
		//	ET -si le paramètre de la clé suivante est différent,
		//	ET -si l'interpolation est LINEAIRE ou suivant une courbe de BEZIER.

		bool determine_interpolation(uint32_t p, Cle_Animation* cle, Cle_Animation* cle_suiv)
		{

			uint16_t methode = cle->renvoie_methode_interpolation(p);
			bool interpolation=false;

			if ( cle->interpolations_activees()
				&& (cle_suiv != NULL)
				&& ((methode==INTERPOLATION_LINEAIRE) || (methode==INTERPOLATION_BEZIER))
				)
				{
				    uint16_t type_parametre=cle->renvoie_type_parametre(p);
				    switch (type_parametre)
				    {
                        case PARAMETRE_ENTIER:
                            if (*((int*)cle->renvoie_valeur(p)) != *((int*)cle_suiv->renvoie_valeur(p))) interpolation=true;
                            break;
                        case PARAMETRE_FLOTTANT:
                            if (*((double*)cle->renvoie_valeur(p))!= *((double*)cle_suiv->renvoie_valeur(p))) interpolation=true;
                            break;
                        default: interpolation=false;
                            break;
				    }
                }
			else interpolation=false;

            if (interpolation)cle->active_interpolation(p);
            else cle->desactive_interpolation(p);
            return interpolation;
		}

		//Détermine si l'interpolation d'une rotation doit être activée:
		bool determine_interpolation_rotation(uint32_t p, Cle_Animation* cle)
		{

			uint16_t methode = cle->renvoie_methode_interpolation(p);
			bool interpolation=false;

			if ( cle->interpolations_activees()
				&& ((methode==INTERPOLATION_LINEAIRE) || (methode==INTERPOLATION_BEZIER))
				)
				{
				    uint16_t type_parametre=cle->renvoie_type_parametre(p);
				    switch (type_parametre)
				    {
                        case PARAMETRE_ENTIER:
                            if (*((int*)cle->renvoie_valeur(p)) !=0x0 ) interpolation=true;
                            break;
                        case PARAMETRE_FLOTTANT:
                            if (*((double*)cle->renvoie_valeur(p)) != 0.) interpolation=true;
                            break;
                        default: interpolation=false;
                            break;
				    }
                }
			else interpolation=false;

            if (interpolation)cle->active_interpolation(p);
            else cle->desactive_interpolation(p);
            return interpolation;
		}

		//========================================================================
		//          Fonctions de gestion des canaux chargés
		//========================================================================

        //Renvoie un canal à partir de son nom.
        //NULL si le canal n'existe pas.
		Canal_Animation* renvoi_canal(string p_nom)
		{
		    for (uint32_t i=0;i< canaux.size();i++)
		    {
		        if (canaux[i]->nom==p_nom) return canaux[i];
		    }
		    return NULL;
		}

};

#endif // CHARGE_CANAL_ANIMATION_H_INCLUDED
