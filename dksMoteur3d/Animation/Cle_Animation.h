#ifndef CLE_ANIMATION_H_INCLUDED
#define CLE_ANIMATION_H_INCLUDED

#include "Parametre.h"

//==================================================================================================================
//  Classe de base des clés d'animation
//!!!DEVELOPPER UNE TABLE DE CORRESPONDANCE ENTRE LES IDENTIFIANTS DES PARAMETRES ET LEUR INDICE DANS LA TABLE
//==================================================================================================================

class Cle_Animation
{
    //Données:
    public:
        string nom;
        double t; //temps en secondes où est située la clé
        uint32_t index; //L'index de la clé dans le tableau du canal
        void* canal;    //Le canal de la clé.


    protected:
        vector<Parametre_Animation*> parametres;    //Paramètres de la clef
        vector<Parametre_Animation*> parametres_crees;  //Paramètres à supprimer par le constructeur
        bool  drapeau_interpolations;    //Le drapeau globale d'activation des interpolations des paramètres de la clé.
                                        //Si ce drapeau est à false, aucun paramètre n'est interpolé.

        //=======================================
        //  Constructeur:
        //=======================================
    public:
        Cle_Animation(double t_p,string p_nom="")
        {
            nom=p_nom;
            t = t_p;
            index=0;
            drapeau_interpolations=true; //Par défaut, le drapeau global des interpolations est activé.
        }

        //----------------Destructeur:
        virtual ~Cle_Animation()
        {
            //Suppression des paramètres créés:
            for (uint32_t i=0;i<parametres_crees.size();i++)
            {
                if (parametres_crees[i]!=NULL)
                {
                    delete parametres_crees[i];
                    //parametres_crees[i]=NULL;
                }
            }
        }


        //====================================================================
        //                  Création de paramètres
        //             Les paramètres créés avec ces fonctions sont
        //             automatiquement supprimés par le destructeur
        //====================================================================

        //---------------Ajoute un paramètre de type ENTIER à la clé:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,int32_t p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_ENTIER* nouveau_parametre= new Parametre_Animation_ENTIER
                                                                (p_nom,p_indice,p_interpolation,p_methodeInterpolation,
                                                                p_courbe,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme ça on peut aussi ajouter des paramètres qui
                                                        //ne sont pas créés par la clef.
            return nouveau_parametre;
        }

        //----------------Ajoute un paramètre de type FLOTTANT à la clé:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,double p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_FLOTTANT* nouveau_parametre= new Parametre_Animation_FLOTTANT
                                                                (p_nom,p_indice,p_interpolation,p_methodeInterpolation,
                                                                p_courbe,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme ça on peut aussi ajouter des paramètres qui
                                                        //ne sont pas créés par la clef.
            return nouveau_parametre;
        }

        //-----------------Ajoute un paramètre de type BOOLEEN à la clé:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,bool p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_BOOLEEN* nouveau_parametre= new Parametre_Animation_BOOLEEN
                                                                (p_nom,p_indice,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme ça on peut aussi ajouter des paramètres qui
                                                        //ne sont pas créés par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un paramètre de type TRAJECTOIRE à la clé:
        Parametre_Animation* ajoute_parametre
                            (
                            string p_nom,uint32_t p_indice,
                            CourbeBezier_Espace* p_courbe_trajectoire,
                            bool p_interpolation=false,
                            uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL,
                            uint16_t p_type_trajectoire=TRAJECTOIRE_RECTILIGNE,
                            bool p_asservissement_tangente=false
                            )

        {
            Parametre_Animation_TRAJECTOIRE* nouveau_parametre= new Parametre_Animation_TRAJECTOIRE
                                                                (p_nom,p_indice,p_interpolation,p_methodeInterpolation,
                                                                p_courbe,p_type_trajectoire,p_courbe_trajectoire,
                                                                p_asservissement_tangente);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme ça on peut aussi ajouter des paramètres qui
                                                        //ne sont pas créés par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un paramètre de type HIERARCHIE à la clé:
        Parametre_Animation* ajoute_parametre
                            (
                            string p_nom,uint32_t p_indice,vector<definition_hierarchie>* p_hierarchie
                            )
        {
            Parametre_Animation_HIERARCHIE* nouveau_parametre= new Parametre_Animation_HIERARCHIE
                                                                (p_nom,p_indice,p_hierarchie);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme ça on peut aussi ajouter des paramètres qui
                                                        //ne sont pas créés par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un paramètre de type ELEMENT à la clé:
        Parametre_Animation* ajoute_parametre
                            (
                            string p_nom,uint32_t p_indice,Conteneur* p_element
                            )
        {
            Parametre_Animation_ELEMENT* nouveau_parametre= new Parametre_Animation_ELEMENT
                                                                (p_nom,p_indice,p_element);
            parametres_crees.push_back(nouveau_parametre);  //Comme ça on peut aussi ajouter des paramètres qui
                                                            //ne sont pas créés par la clef.
            parametres.push_back(nouveau_parametre);

            return nouveau_parametre;
        }

        //=============================================================
        //              Gestion des paramètres globaux:
        //=============================================================

    public:
        void active_interpolations()
        {
            drapeau_interpolations=true;
        }

        void desactive_interpolations()
        {
            drapeau_interpolations=false;
        }

        bool interpolations_activees()
        {
            return drapeau_interpolations;
        }

    private:
        //Renvoie l'indice du paramètre dans la table, à partir de son identifiant.
        //Renvoie -1 si l'identifiant est inconnu.
        uint32_t renvoie_indice_parametre(uint32_t parametre)
        {
            for (uint32_t i=0;i<parametres.size();i++)
            {
                if (parametres[i]->indice==parametre) return i;
            }
            return 0xffffffff;
        }
    public:
        //Renvoie le paramètre correspondant à l'identifiant:
        Parametre_Animation* renvoie_parametre(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return parametres[indice];
        }

        //Renvoie le type du parametre:
        uint16_t renvoie_type_parametre(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return 0xffff;
            return parametres[indice]->type;
        }

        //Active l'interpolation d'un paramètre:
        void active_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->interpolation = true;
        }

        void desactive_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->interpolation = false;
        }

        bool interpolation_activee(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return false;
            return parametres[indice]->interpolation;
        }

        void determine_valeur(uint32_t parametre_id, void* valeur)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->determine_valeur(valeur);
        }

        //Renvoie la valeur du paramètre de la clé:
        void* renvoie_valeur(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return parametres[indice]->renvoie_valeur();
        }

        //Renvoie la méthode d'interpolation d'un paramètre:
        uint16_t renvoie_methode_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return 0xffff;
            return parametres[indice]->methodeInterpolation;
        }

        //Renvoie la courbe de bézier d'interpolation d'un paramètre:
        //Null si pas de courbe définie.
        CourbeBezier_Plan* renvoie_courbe_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return parametres[indice]->courbe;
        }


        //Détermine la méthode d'interpolation d'un paramètre:
        void determine_methode_interpolation(uint32_t parametre_id, uint16_t methode)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->methodeInterpolation = methode;
        }

        //Détermine la méthode d'interpolation d'un paramètre:
        void determine_courbe_interpolation(uint32_t parametre_id, CourbeBezier_Plan* courbe)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->courbe = courbe;
        }

        //Le parametre doit être de type TRAJECTOIRE:
        void determine_type_trajectoire(uint32_t parametre_id,uint16_t p_type)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->type_trajectoire = p_type;
        }

        //Le parametre doit être de type TRAJECTOIRE:
        uint16_t renvoie_type_trajectoire(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return 0xffff;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->type_trajectoire;
        }

        //Le parametre doit être de type TRAJECTOIRE:
        CourbeBezier_Espace* renvoie_courbe_trajectoire(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->courbe_trajectoire;
        }

        //Le parametre doit être de type TRAJECTOIRE:
        bool renvoie_asservissement_tangente(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return false;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->asservissement_tangente;
        }

        //Affiche les paramètres de la clé:
        virtual void toString()
        {
            cout << "--- Cle_Animation: " << " - t: " << t << " - Nom:"<< nom << endl;
            vector<Parametre_Animation*>::iterator i;
            for (i=parametres.begin(); i!=parametres.end(); i++)
            {
                (*i)->toString();
            }
        }
};
#endif // CLE_ANIMATION_H_INCLUDED
