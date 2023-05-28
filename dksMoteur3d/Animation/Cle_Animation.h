#ifndef CLE_ANIMATION_H_INCLUDED
#define CLE_ANIMATION_H_INCLUDED

#include "Parametre.h"

//==================================================================================================================
//  Classe de base des cl�s d'animation
//!!!DEVELOPPER UNE TABLE DE CORRESPONDANCE ENTRE LES IDENTIFIANTS DES PARAMETRES ET LEUR INDICE DANS LA TABLE
//==================================================================================================================

class Cle_Animation
{
    //Donn�es:
    public:
        string nom;
        double t; //temps en secondes o� est situ�e la cl�
        uint32_t index; //L'index de la cl� dans le tableau du canal
        void* canal;    //Le canal de la cl�.


    protected:
        vector<Parametre_Animation*> parametres;    //Param�tres de la clef
        vector<Parametre_Animation*> parametres_crees;  //Param�tres � supprimer par le constructeur
        bool  drapeau_interpolations;    //Le drapeau globale d'activation des interpolations des param�tres de la cl�.
                                        //Si ce drapeau est � false, aucun param�tre n'est interpol�.

        //=======================================
        //  Constructeur:
        //=======================================
    public:
        Cle_Animation(double t_p,string p_nom="")
        {
            nom=p_nom;
            t = t_p;
            index=0;
            drapeau_interpolations=true; //Par d�faut, le drapeau global des interpolations est activ�.
        }

        //----------------Destructeur:
        virtual ~Cle_Animation()
        {
            //Suppression des param�tres cr��s:
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
        //                  Cr�ation de param�tres
        //             Les param�tres cr��s avec ces fonctions sont
        //             automatiquement supprim�s par le destructeur
        //====================================================================

        //---------------Ajoute un param�tre de type ENTIER � la cl�:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,int32_t p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_ENTIER* nouveau_parametre= new Parametre_Animation_ENTIER
                                                                (p_nom,p_indice,p_interpolation,p_methodeInterpolation,
                                                                p_courbe,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme �a on peut aussi ajouter des param�tres qui
                                                        //ne sont pas cr��s par la clef.
            return nouveau_parametre;
        }

        //----------------Ajoute un param�tre de type FLOTTANT � la cl�:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,double p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_FLOTTANT* nouveau_parametre= new Parametre_Animation_FLOTTANT
                                                                (p_nom,p_indice,p_interpolation,p_methodeInterpolation,
                                                                p_courbe,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme �a on peut aussi ajouter des param�tres qui
                                                        //ne sont pas cr��s par la clef.
            return nouveau_parametre;
        }

        //-----------------Ajoute un param�tre de type BOOLEEN � la cl�:
        Parametre_Animation* ajoute_parametre(
                            string p_nom,uint32_t p_indice,bool p_valeur,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            Parametre_Animation_BOOLEEN* nouveau_parametre= new Parametre_Animation_BOOLEEN
                                                                (p_nom,p_indice,p_valeur);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme �a on peut aussi ajouter des param�tres qui
                                                        //ne sont pas cr��s par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un param�tre de type TRAJECTOIRE � la cl�:
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
            parametres.push_back(nouveau_parametre);    //Comme �a on peut aussi ajouter des param�tres qui
                                                        //ne sont pas cr��s par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un param�tre de type HIERARCHIE � la cl�:
        Parametre_Animation* ajoute_parametre
                            (
                            string p_nom,uint32_t p_indice,vector<definition_hierarchie>* p_hierarchie
                            )
        {
            Parametre_Animation_HIERARCHIE* nouveau_parametre= new Parametre_Animation_HIERARCHIE
                                                                (p_nom,p_indice,p_hierarchie);
            parametres_crees.push_back(nouveau_parametre);
            parametres.push_back(nouveau_parametre);    //Comme �a on peut aussi ajouter des param�tres qui
                                                        //ne sont pas cr��s par la clef.
            return nouveau_parametre;
        }

        //------------------Ajoute un param�tre de type ELEMENT � la cl�:
        Parametre_Animation* ajoute_parametre
                            (
                            string p_nom,uint32_t p_indice,Conteneur* p_element
                            )
        {
            Parametre_Animation_ELEMENT* nouveau_parametre= new Parametre_Animation_ELEMENT
                                                                (p_nom,p_indice,p_element);
            parametres_crees.push_back(nouveau_parametre);  //Comme �a on peut aussi ajouter des param�tres qui
                                                            //ne sont pas cr��s par la clef.
            parametres.push_back(nouveau_parametre);

            return nouveau_parametre;
        }

        //=============================================================
        //              Gestion des param�tres globaux:
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
        //Renvoie l'indice du param�tre dans la table, � partir de son identifiant.
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
        //Renvoie le param�tre correspondant � l'identifiant:
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

        //Active l'interpolation d'un param�tre:
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

        //Renvoie la valeur du param�tre de la cl�:
        void* renvoie_valeur(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return parametres[indice]->renvoie_valeur();
        }

        //Renvoie la m�thode d'interpolation d'un param�tre:
        uint16_t renvoie_methode_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return 0xffff;
            return parametres[indice]->methodeInterpolation;
        }

        //Renvoie la courbe de b�zier d'interpolation d'un param�tre:
        //Null si pas de courbe d�finie.
        CourbeBezier_Plan* renvoie_courbe_interpolation(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return parametres[indice]->courbe;
        }


        //D�termine la m�thode d'interpolation d'un param�tre:
        void determine_methode_interpolation(uint32_t parametre_id, uint16_t methode)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->methodeInterpolation = methode;
        }

        //D�termine la m�thode d'interpolation d'un param�tre:
        void determine_courbe_interpolation(uint32_t parametre_id, CourbeBezier_Plan* courbe)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            parametres[indice]->courbe = courbe;
        }

        //Le parametre doit �tre de type TRAJECTOIRE:
        void determine_type_trajectoire(uint32_t parametre_id,uint16_t p_type)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return;
            ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->type_trajectoire = p_type;
        }

        //Le parametre doit �tre de type TRAJECTOIRE:
        uint16_t renvoie_type_trajectoire(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return 0xffff;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->type_trajectoire;
        }

        //Le parametre doit �tre de type TRAJECTOIRE:
        CourbeBezier_Espace* renvoie_courbe_trajectoire(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return NULL;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->courbe_trajectoire;
        }

        //Le parametre doit �tre de type TRAJECTOIRE:
        bool renvoie_asservissement_tangente(uint32_t parametre_id)
        {
            uint32_t indice=renvoie_indice_parametre(parametre_id);
            if(indice==0xffffffff) return false;
            return ((Parametre_Animation_TRAJECTOIRE*)parametres[indice])->asservissement_tangente;
        }

        //Affiche les param�tres de la cl�:
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
