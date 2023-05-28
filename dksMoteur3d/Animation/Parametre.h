#ifndef PARAMETRE_H_INCLUDED
#define PARAMETRE_H_INCLUDED

#include <string.h>
#include <vector>
#include "../Geometrie_Plan/Courbe_Bezier_Plan.h"
#include "../Geometrie_Espace/Courbe_Bezier_Espace.h"
    using namespace std;

//================================================
//          Types d'interpolations
//================================================
//Interpolations à une dimension:
#define INTERPOLATION_BASCULE 1  //La valeur du paramètre ne passera pas par des valeurs intermédiaires lors de l'interpolation
#define INTERPOLATION_LINEAIRE 2 //Variation linéaire de la valeur
#define INTERPOLATION_BEZIER 3 //Variation de la valeur suivant une courbe de bézier

//Interpolations multidimentionnelles:
#define TRAJECTOIRE_RECTILIGNE 1 //Les valeurs sont interpolées suivant une ligne droite
#define TRAJECTOIRE_COURBE 2 //Interpolation suivant une courbe
#define TRAJECTOIRE_CIRCULAIRE 3 //Interpolation suivant un arc de cercle

//===================================================
//          Types de donnée gérée par le paramètre
//===================================================
//Types de donnée pour les paramètres gérés par les clés:
#define PARAMETRE_BASE 0   //Pas de valeur pour le paramètre
#define PARAMETRE_ENTIER 1 //uint32
#define PARAMETRE_FLOTTANT 2   //double
#define PARAMETRE_BOOLEEN 3    //bool
#define PARAMETRE_TRAJECTOIRE 5 //Détermine la courbe d'une trajectoire spatiale.
#define PARAMETRE_HIERARCHIE 6 //Pointe sur un tableau de définition de la hiérarchie.
#define PARAMETRE_ELEMENT 7 //Pointe sur un élément fondamentale de la scène (oeil,lumière, volume ou conteneur)

//==================================================
//      Classe de base des paramètres d'animation
//==================================================
class Parametre_Animation
{
    public:
        string nom;
        uint32_t indice;
        bool interpolation;
        uint16_t methodeInterpolation;
        CourbeBezier_Plan* courbe;
        uint16_t type;

        Parametre_Animation(string p_nom="",uint32_t p_indice=0,uint16_t p_type=PARAMETRE_BASE,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL)
        {
            nom=p_nom;
            indice=p_indice;
            interpolation=p_interpolation;
            methodeInterpolation=p_methodeInterpolation;
            type=p_type;
            courbe=p_courbe;
        }


        virtual ~Parametre_Animation()
        {

        }

        void initialise_identite(string p_nom="",uint32_t p_indice=0)
        {
            nom=p_nom;
            indice=p_indice;
        }

        string renvoie_nom_type_interpolation(uint16_t p_type)
        {
            switch (p_type)
            {
                case INTERPOLATION_BASCULE:
                    return "BASCULE";break;
                case INTERPOLATION_BEZIER:
                    return "BEZIER";break;
                case INTERPOLATION_LINEAIRE:
                    return "LINEAIRE";break;
                default: return "indéfinie";break;
            }
        }

        string renvoie_nom_type_donnee(uint16_t p_type)
        {
            switch (p_type)
            {
                case PARAMETRE_BASE:
                    return "BASE";break;
                case PARAMETRE_BOOLEEN:
                    return "BOOLEEN";break;
                case PARAMETRE_ENTIER:
                    return "ENTIER";break;
                case PARAMETRE_FLOTTANT:
                    return "FLOTTANT";break;
                case PARAMETRE_TRAJECTOIRE:
                    return "TRAJECTOIRE";break;
                case PARAMETRE_HIERARCHIE:
                    return "HIERARCHIE";break;
                case PARAMETRE_ELEMENT:
                    return "ELEMENT";break;
                default: return "indéfini";break;
            }
        }

        virtual void toString()
        {
            cout<<nom<<" - Interpolation: "<<renvoie_nom_type_interpolation(methodeInterpolation)<<" - "<<interpolation<< " - type: " <<renvoie_nom_type_donnee(type);
        }

        virtual void determine_valeur(void* valeur)=0;
        virtual void* renvoie_valeur()=0;

};

//==================================================
//      Paramètre de type ENTIER
//==================================================
class Parametre_Animation_ENTIER: public Parametre_Animation
{
    public:
       int32_t valeur;

        Parametre_Animation_ENTIER(string p_nom="",uint32_t p_indice=0,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL,int32_t p_valeur=0)
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_ENTIER,p_interpolation,p_methodeInterpolation,p_courbe)
        {
            valeur=p_valeur;
        }

        virtual void determine_valeur(void* p_valeur)
        {
            valeur=(*(int32_t*)p_valeur);
        }

        virtual int32_t* renvoie_valeur()
        {
            return &valeur;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<" - valeur = "<<valeur<<endl;
        }

};

//==================================================
//      Paramètre de type FLOTTANT
//==================================================
class Parametre_Animation_FLOTTANT: public Parametre_Animation
{
    public:
       double valeur;

        Parametre_Animation_FLOTTANT(string p_nom="",uint32_t p_indice=0,
                            bool p_interpolation=false, uint16_t p_methodeInterpolation=INTERPOLATION_BASCULE,
                            CourbeBezier_Plan* p_courbe=NULL,double p_valeur=0.)
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_FLOTTANT,p_interpolation,p_methodeInterpolation,p_courbe)
        {
            valeur=p_valeur;
        }

        virtual void determine_valeur(void* p_valeur)
        {
            valeur=*(double*)p_valeur;
        }

        virtual double* renvoie_valeur()
        {
            return &valeur;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<" - valeur = "<<valeur<<endl;
        }

};

//==================================================
//      Paramètre de type BOOLEEN
//==================================================
class Parametre_Animation_BOOLEEN: public Parametre_Animation
{
    public:
       bool valeur;

        Parametre_Animation_BOOLEEN(string p_nom="",uint32_t p_indice=0,bool p_valeur=false)
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_BOOLEEN)
        {
            valeur=p_valeur;
        }

        virtual void determine_valeur(void* p_valeur)
        {
            valeur=*(bool*)p_valeur;
        }

        virtual bool* renvoie_valeur()
        {
            return &valeur;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<nom<<" - valeur = "<<valeur<<endl;
        }

};

//==================================================
//      Paramètre de type TRAJECTOIRE
//==================================================
class Parametre_Animation_TRAJECTOIRE: public Parametre_Animation
{
    public:
        uint16_t type_trajectoire;
        CourbeBezier_Espace* courbe_trajectoire;
        bool asservissement_tangente;


        Parametre_Animation_TRAJECTOIRE(
                            string p_nom="",
                            uint32_t p_indice=0,
                            bool p_interpolation=false,
                            uint16_t p_methodeInterpolation=INTERPOLATION_LINEAIRE,
                            CourbeBezier_Plan* p_courbe_interpolation=NULL,
                            uint16_t p_type_trajectoire=TRAJECTOIRE_RECTILIGNE,
                            CourbeBezier_Espace* p_courbe_trajectoire=NULL,
                            bool p_asservissement_tangente=false
                            )
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_TRAJECTOIRE,p_interpolation,p_methodeInterpolation,p_courbe_interpolation)
        {
            type_trajectoire=p_type_trajectoire;
            courbe_trajectoire=p_courbe_trajectoire;
            asservissement_tangente=p_asservissement_tangente;
        }

        string renvoie_nom_type_trajectoire(uint16_t p_type)
        {
            switch (p_type)
            {
                case TRAJECTOIRE_RECTILIGNE:
                    return "RECTILIGNE";break;
                case TRAJECTOIRE_COURBE:
                    return "COURBE";break;
                case TRAJECTOIRE_CIRCULAIRE:
                    return "CIRCULAIRE";break;
                default:
                return "indéfinie";break;
            }
        }

        virtual void determine_valeur(void* p_valeur)
        {
            courbe_trajectoire=(CourbeBezier_Espace*) p_valeur;
        }

        virtual CourbeBezier_Espace* renvoie_valeur()
        {
            return courbe_trajectoire;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<" - type trajectoire: "<<renvoie_nom_type_trajectoire(type_trajectoire);
            if (type_trajectoire==TRAJECTOIRE_COURBE)
            {
                cout << " - Courbe:";
                courbe_trajectoire->toString();
            }
            cout<<endl;
        }

};

//==================================================
//      Paramètre de type HIERARCHIE
//==================================================

class definition_hierarchie
{
    public:
        Conteneur* conteneur;
        Conteneur* parent;
};


class Parametre_Animation_HIERARCHIE: public Parametre_Animation
{
    public:
       vector<definition_hierarchie> hierarchie;

        Parametre_Animation_HIERARCHIE(string p_nom="",uint32_t p_indice=0,vector<definition_hierarchie>* p_hierarchie=NULL)
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_HIERARCHIE)
        {
            if (p_hierarchie!=NULL) hierarchie=*p_hierarchie; //Voir comment fonctionne la copie.
        }

        virtual void determine_valeur(void* p_valeur)
        {
            hierarchie=*(vector<definition_hierarchie>*)p_valeur;
        }

        virtual vector<definition_hierarchie>* renvoie_valeur()
        {
            return &hierarchie;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<" - Définition de la hiérarchie"<<endl;
        }

};

//==================================================
//      Paramètre de type ELEMENT
//==================================================

class Parametre_Animation_ELEMENT: public Parametre_Animation
{
    public:

        Conteneur* element;

        Parametre_Animation_ELEMENT(string p_nom="",uint32_t p_indice=0,Conteneur* p_element=NULL)
                            :Parametre_Animation(p_nom,p_indice,PARAMETRE_ELEMENT)
        {
            element=p_element;
        }

        virtual void determine_valeur(void* p_valeur)
        {
            element=(Conteneur*)p_valeur;
        }

        virtual Conteneur* renvoie_valeur()
        {
            return element;
        }

        virtual void toString()
        {
            Parametre_Animation::toString();
            cout<<" - Elément : ";
            if (element!=NULL)cout<<element->nom<<endl;
            else cout<<"NULL"<<endl;
        }

};


#endif // PARAMETRE_H_INCLUDED
