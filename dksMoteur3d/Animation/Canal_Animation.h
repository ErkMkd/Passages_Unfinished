#ifndef CANAL_ANIMATION_H_INCLUDED
#define CANAL_ANIMATION_H_INCLUDED

#include "Cle_Animation.h"

//=================================================================================
//                      Types de canaux
//=================================================================================

#define CANAL_ANIMATION_BASE 0
#define CANAL_ANIMATION_SCENE 1
#define CANAL_ANIMATION_HIERARCHIE 2
#define CANAL_ANIMATION_CONTENEUR 3
#define CANAL_ANIMATION_VOLUME 4
#define CANAL_ANIMATION_SOURCE_LUMINEUSE 5

//=================================================================================
//                      Définition de la classe de base des canaux d'animation
//==================================================================================

class  Canal_Animation
{
    public:
        string nom;
        void* scenario; //L'animation à laquelle appartient le canal.
        uint16_t type;  //Type du canal
    protected:
        vector<Cle_Animation*> cles; //Les clés du canal

        double position_depart; //Temps 0 du canal
        double duree; //Durée totale du canal.
        Vecteur position_rot;

        //Pour la gestion mémoire:
        vector<Cle_Animation*> cles_crees; //A libérer par le destructeur

    protected:
        bool actif;


        //=================================================================================
        //=================             CONSTRUCTEUR                    ===================
        //=================================================================================
    public:
        Canal_Animation (uint16_t p_type, vector<Cle_Animation*>* p_cles=NULL, bool p_actif=false)
        {
            type = p_type;
            actif = p_actif;
            position_depart=0.;
            nom="";

            if (p_cles!=NULL)
            {
                cles = *p_cles;   //Voir comment fonctionne la copie.
                vector<Cle_Animation*>::iterator i;
                uint32_t j=0;
                for (i = cles.begin(); i != cles.end(); i++)
                {
                    (*i)->canal = this;
                    (*i)->index = j++;
                }
            }
        }


        //======================== DESTRUCTEUR ==================
        virtual ~Canal_Animation()
        {
            uint32_t i;
            for (i=0;i<cles_crees.size();i++)
            {
                if (cles_crees[i]!=NULL)
                {
                    delete cles_crees[i];
                    cles_crees[i]=NULL;
                }
            }
        }

        //=================================================================================
        //                          Gestion de la lecture:
        //=================================================================================

        bool est_active()
        {
            return actif;
        }

        void active(double t = -1)
        {
            actif = true;
            if (t >= 0) position_depart = t;

            duree = cles[cles.size()-1]->t;
            //trace("duree", duree);
        }

        void desactive()
        {
            actif=false;
        }

        void determine_position_depart(double t_depart)
        {
            position_depart=t_depart;
        }

        double renvoie_position_depart()
        {
            return position_depart;
        }

        //=================================================================================
        //                       Fonctions de recherche des clés
        //=================================================================================

        //Renvoie la clé située à l'indice:
        //Si l'indice est hors du tableau, renvoie NULL.
        Cle_Animation* renvoie_cle(uint32_t indice)
        {
            if (indice>=cles.size())return NULL;
            else return cles[indice];
        }

        //Renvoie la clé suivante à la clé spécifiée en entrée
        //Renvoie null si il n'y a pas de clé suivante.
        Cle_Animation* renvoie_cle_suivante(Cle_Animation* cle)
        {
            if (cle->index >= cles.size() - 1) return NULL;
            else return cles[cle->index + 1];
        }

        //Renvoie la clé précédente à la clé spécifiée en entrée
        //Renvoie null si il n'y a pas de clé précédente.
        Cle_Animation* renvoie_cle_precedente(Cle_Animation* cle)
        {
            if (cle->index == 0) return NULL;
            else return cles[cle->index - 1];
        }


        //Renvoie la clé précédente la plus proche de la position indiquée
        //Si il y a une clé située au temps indiqué, elle est renvoyée par la fonction.
        Cle_Animation* renvoie_cle_precedente_proche(double t)
        {
            if (cles.size() == 0) return NULL;

            for ( uint32_t i = 1 ; i < cles.size(); i++ )
            {
                if (cles[i]->t > t) return cles[i - 1];
            }
            return cles[cles.size() - 1];
        }

        //Renvoie la clé suivante la plus proche de la position indiquée
        //Null si il n'y a pas de clé après la position spécifiée.
        Cle_Animation* renvoie_cle_suivante_proche(double t)
        {
            for (uint32_t i = 0; i < cles.size(); i++ )
            {
                if (cles[i]->t > t) { return cles[i]; }    //VOIR SI ON PEUT RENVOYER I
            }
            return NULL;
        }

        //=================================================================================
        //==================== Fonctions de calculs d'interpolations:
        //=================================================================================
    protected:
        //renvoie la valeur interpolée d'un paramètre de type FLOTTANT:
        //t est normalement compris entre 0 et 1
        double calcul_interpolation_FLOTTANT(Parametre_Animation* p_org,Parametre_Animation* p_dst,double t)
        {
            if (!p_org->interpolation) return ((Parametre_Animation_FLOTTANT*)p_org)->valeur;

            uint16_t mi = p_org->methodeInterpolation;
            double valOrg = ((Parametre_Animation_FLOTTANT*)p_org)->valeur;

            if (mi == INTERPOLATION_LINEAIRE)
            {

                return valOrg + ( ((Parametre_Animation_FLOTTANT*)p_dst)->valeur - valOrg) * t;
            }
            else if (mi == INTERPOLATION_BEZIER)
            {return valOrg + ( ((Parametre_Animation_FLOTTANT*)p_dst)->valeur - valOrg) * p_org->courbe->renvoie_ordonnee_via_abscisse(t);}
            else{return valOrg;}
        }

        //renvoie l'angle interpolé d'une rotation:
        //t est normalement compris entre 0 et 1
        double calcul_interpolation_rotation(Parametre_Animation* p_org,double t)
        {
            if (!p_org->interpolation) return 0.;

            uint16_t mi = p_org->methodeInterpolation;

            if (mi == INTERPOLATION_LINEAIRE)
            {return ((Parametre_Animation_FLOTTANT*)p_org)->valeur * t;}
            else if (mi == INTERPOLATION_BEZIER)
            {return ((Parametre_Animation_FLOTTANT*)p_org)->valeur * p_org->courbe->renvoie_ordonnee_via_abscisse(t);}
            else{return 0.;}
        }

        //renvoie la valeur interpolée d'un paramètre au format int 0x00RRVVBB (généralement utilisé pour la définition d'une couleur)
        //t est normalement compris entre 0 et 1
        uint32_t calcul_interpolationRVB(Parametre_Animation* p_org,Parametre_Animation* p_dst,double t)
        {
            if (!p_org->interpolation) return (uint32_t)((Parametre_Animation_ENTIER*)p_org)->valeur;

            uint16_t mi = p_org->methodeInterpolation;
            if (mi == INTERPOLATION_BASCULE) return ((Parametre_Animation_ENTIER*)p_org)->valeur;

            uint32_t valeur_org=(uint32_t)((Parametre_Animation_ENTIER*)p_org)->valeur;
            uint32_t valeur_dst=(uint32_t)((Parametre_Animation_ENTIER*)p_dst)->valeur;
            double valOrg_R = double((valeur_org >> 16) & 0xff);
            double valOrg_V = double((valeur_org >> 8) & 0xff);
            double valOrg_B = double(valeur_org & 0xff);

            double valDst_R = double((valeur_dst >> 16) & 0xff);
            double valDst_V = double((valeur_dst >> 8) & 0xff);
            double valDst_B = double(valeur_dst & 0xff);

            if (mi == INTERPOLATION_BEZIER) t = p_org->courbe->renvoie_ordonnee_via_abscisse(t);

            uint32_t val_R = uint32_t(valOrg_R + ( valDst_R - valOrg_R) * t);
            uint32_t val_V = uint32_t(valOrg_V + ( valDst_V - valOrg_V) * t);
            uint32_t val_B = uint32_t(valOrg_B + ( valDst_B - valOrg_B) * t);

            return (val_R << 16) | (val_V<<8) | val_B;
        }

        //renvoie la valeur position interpolée sur une trajectoire:
        //t est normalement compris entre 0 et 1
        void calcul_interpolation_trajectoire_courbe(Parametre_Animation_TRAJECTOIRE* trajectoire,double t,vec3d* position)
        {
            if (trajectoire->type_trajectoire!=TRAJECTOIRE_COURBE) return;

            if (!trajectoire->interpolation)
            {
                trajectoire->courbe_trajectoire->renvoie_P0(position);
                return;
            }

            uint16_t mi = trajectoire->methodeInterpolation;

            if (mi == INTERPOLATION_LINEAIRE)
            {
                trajectoire->courbe_trajectoire->renvoie_position(t, position);
            }
            else if (mi == INTERPOLATION_BEZIER)
            {
                trajectoire->courbe_trajectoire->renvoie_position( trajectoire->courbe->renvoie_ordonnee_via_abscisse(t), position);
            }
            else
            {
                trajectoire->courbe_trajectoire->renvoie_P0(position);
            }
        }

        //Interpolation d'une trajectoire circulaire:
        /*
        void calcul_interpolation_trajectoire_circulaire(Cle_Animation* cle, AxeRotation* axe, parametre_interpolation:Object, t:Number, position:Object):void
        {
            var mi:int = parametre_interpolation.methodeInterpolation;

            if ((parametre_interpolation.interpolation == false) || ( mi == Interpolation.BASCULE))
            {
                position.x=cle.parametres[Canal_Animation_Conteneur.XPOS].valeur;
                position.y=cle.parametres[Canal_Animation_Conteneur.YPOS].valeur;
                position.z=cle.parametres[Canal_Animation_Conteneur.ZPOS].valeur;
                return;
            }

            if (mi == Interpolation.LINEAIRE)
            {
                var angle_t:Number = t * axe.angle;
            }
            else if (mi == Interpolation.BEZIER)
            {
                angle_t = axe.angle * parametre_interpolation.courbe.renvoie_ordonnee_via_abscisse(t);
            }

            position_rot.x = cle.parametres[Canal_Animation_Conteneur.XPOS].valeur - axe.position.x;
            position_rot.y = cle.parametres[Canal_Animation_Conteneur.YPOS].valeur - axe.position.y;
            position_rot.z = cle.parametres[Canal_Animation_Conteneur.ZPOS].valeur - axe.position.z;

            position_rot.rotation(angle_t, axe);

            position.x= position_rot.x + axe.position.x ;
            position.y= position_rot.y + axe.position.y ;
            position.z= position_rot.z + axe.position.z ;
        }
        */

        //=================================================================================
        //=================== Fonctions de gestion de la table des clés:
        //=================================================================================
    public:
        //Fonction qui crée une clé adaptée au canal.
        //LA clé n'est pas ajoutée au canal. Pour cela, appeler ajoute_cle()
        //! La clé n'est pas ajoutée à la table des clés du canal. Comme ça, il est possible de créer une clé
        //! vide que l'on peut ajouter dans un autre canal.
        Cle_Animation* creation_cle(double t=0.,string p_nom="")
        {
            Cle_Animation* nouvelle_clef=new Cle_Animation(t,p_nom);
            cles_crees.push_back(nouvelle_clef);
            initialise_cle(nouvelle_clef);
            return nouvelle_clef;
        }

        //Ajoute une cle au canal.
        void ajoute_cle(Cle_Animation* cle, double t=0.)
        {
            double temps = cle->t + t;
            Cle_Animation* cle_prec = renvoie_cle_precedente_proche(temps);
            cle->t = temps;
            cle->canal=this;

            uint32_t index = 0;
            if (cle_prec != NULL) index = cle_prec->index + 1;

            //Insertion de la clé au bon endroit. Ca évite un tri.
            cles.insert(cles.begin()+index,1,cle); // <=> cles.splice(index, 0, cle ) (en ActionScript 3)

            //Initialisation des indexs de la clé et des clé suivantes:
            uint32_t i;
            for (i = index; i < cles.size(); i++ )
            {
                cles[i]->index = i;
            }
            //Si besoin, on raccorde les clés entre elles.
            //Cela peut être les trajectoires courbes pour un canal de conteneur.
            raccorde_cles(cle_prec, cle);
            raccorde_cles(cle, renvoie_cle_suivante(cle));

            //trace(cle);
        }


        //Supprime une clé au temps indiqué.
        //Renvoie la clé supprimée ou null le cas échéant.
        Cle_Animation* supprime_cle_position(double pos)
        {
            if (pos==0.){return NULL;} //La clé au temps 0 est toujours présente
            for (uint32_t i=0;i<cles.size();i++)
            {
                if (cles[i]->t > pos){return NULL;}
                if (cles[i]->t==pos)
                {
                    return supprime_cle_index(i);
                }
            }
            return NULL;
        }

        //Supprime une clé à l'index indiqué.
        //!!! La clé 0 n'est pas protégée !!! Comme ça elle peut être écrasée
        //Renvoie la clé supprimée du tableau ou null si l'index est hors tableau.
        Cle_Animation* supprime_cle_index(uint32_t i)
        {
            if (i<cles.size())
            {
                Cle_Animation* clef = cles[i];
                raccorde_cles(renvoie_cle_precedente(clef), renvoie_cle_suivante(clef));
                cles.erase(cles.begin()+i); //<=> cles.splice(i, 1);
                //Initialisation des indexs des clé suivantes:
                for (i = clef->index; i < cles.size(); i++ )
                {
                    cles[i]->index = i;
                }
                return clef;
                //keysTab[keysTab.length - 1].cle.interpolation = false;
            }
            else return NULL;
        }


        // déplacement d'un groupe de cles -> les clefs doivent appartenir au canal !!!!
        //************** FONCTION OBSOLETE **************************
        /*
        public function deplace_cles(selection_cles:Array, t:Number=0):void
        {
            var temps_de_reference:Number = selection_cles[0].t;

            for each (var clef:Cle_Animation in selection_cles)
            {
                var temps:Number = clef.t - temps_de_reference + t;
                var cle_prec:Cle_Animation = renvoie_cle_precedente_proche(temps);
                clef.t = temps;
                cles.splice( cle_prec.index + 1, 0, clef );
                for (var i:int = cle_prec.index + 1; i < cles.length; i++ )
                {
                    cles[i].index = i;
                }
            }
        }
        */


        virtual void toString()
        {
            cout<<"------ Canal d'animation - type: "<< type <<endl;
            for (uint32_t i=0;i< cles.size();i++)
            {
                cles[i]->toString();
            }
        }

        //=================================================================================
        //=================== Prototypes des fonctions à adapter selon le type de canal:
        //=================================================================================

        //Crée les paramètres propre au canal d'une clé:
        virtual void initialise_cle(Cle_Animation* cle)=0;

        //Prototype de la fonction qui initialise l'objet affecté au canal avec les valeurs des paramètres d'un moment t :
        virtual void initialise_objet(double t) = 0;
        /*
        {

            t -= position_depart;
            if (t >= duree) desactive();
            Cle_Animation* cle_prec = renvoie_cle_precedente_proche(t);
            Cle_Animation* cle_suiv = renvoie_cle_suivante_proche(t);

        }
        */


        //Prototype de la fonction qui initialise les valeurs de la clé en fonction de son objet de référence:
        virtual void initialise_valeurs_cle(Cle_Animation* cle) = 0;
        /*
        {
            cout<<"Initialise les valeurs"<<endl;
        }
        */
        //Prototype de la fonction qui effectue le raccord entre deux clés, après suppression ou insertion d'autres clés.
        //!!!!!!!!!LES CLES DOIVENT APPARTENIR AU CANAL - Il n'y a pas de vérification !!!!!!!!!!
        virtual void raccorde_cles(Cle_Animation* cle_prec, Cle_Animation* cle_suiv) =0;
        /*
        {
            if ((cle_prec != NULL) && (cle_suiv != NULL))
            {
                cout<<"Raccord"<<endl;
            }
        }
        */

};

#endif // CANAL_ANIMATION_H_INCLUDED
