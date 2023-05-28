#ifndef INTERPOLATIONS_H_INCLUDED
#define INTERPOLATIONS_H_INCLUDED
//===================================================================
//             Outils d'interpolations
//===================================================================

#define INTERPOLATION_LINEAIRE 0
#define INTERPOLATION_COSINUS 1
#define INTERPOLATION_BEZIER 2
#define INTERPOLATION_INCONNUE 255


#include "Courbe_Bezier_Plan.h"
#include "../dksMoteur3d/Geometrie_Espace/dksRepere.h"
#include "../dksMoteur3d/Geometrie_Espace/dksAxeRotation.h"


class Interpolations
{

    public:
    //-----------------------------------------------------
    //  Interpolation linéaire entre deux valeurs a et b
    //  t varie entre 0 et 1 (0=a, 1=b)
    // renvoie la valeur intermédiaire.
    //-----------------------------------------------------
    static float interpolation_lineaire_f(float a,float b,float t)
    {
        return a*(1-t)+b*t;
    }
    static double interpolation_lineaire_d(double a,double b,double t)
    {
        return a*(1-t)+b*t;
    }

    static void interpolation_lineaire_2f(float* a,float* b,float t,float* res)
    {
        res[0]= a[0]*(1-t)+b[0]*t;
        res[1]= a[1]*(1-t)+b[1]*t;
    }

    static void interpolation_lineaire_2d(double* a,double* b,double t,double* res)
    {
        res[0]= a[0]*(1-t)+b[0]*t;
        res[1]= a[1]*(1-t)+b[1]*t;
    }


    static void interpolation_lineaire_3f(float* a,float* b,float t,float* res)
    {
        res[0]= a[0]*(1-t)+b[0]*t;
        res[1]= a[1]*(1-t)+b[1]*t;
        res[2]= a[2]*(1-t)+b[2]*t;
    }

    static void interpolation_lineaire_3d(double* a,double* b,double t,double* res)
    {
        res[0]= a[0]*(1-t)+b[0]*t;
        res[1]= a[1]*(1-t)+b[1]*t;
        res[2]= a[2]*(1-t)+b[2]*t;
    }



    //-----------------------------------------------------
    //  Interpolation sinusoïdale entre deux valeurs a et b
    //  x varie entre 0 et 1 (0=a, 1=b)
    // renvoie la valeur intermédiaire.
    //-----------------------------------------------------
    static float interpolation_cosinusoidale_f(float a,float b,float t)
    {
        return interpolation_lineaire_f(a,b,(-cosf(M_PI*t)+1.)/2.);
    }
    static double interpolation_cosinusoidale_d(double a,double b,double t)
    {
        return interpolation_lineaire_d(a,b,(-cos(M_PI*t)+1.)/2.);
    }

    static void interpolation_cosinusoidale_2f(float* a,float* b,float t,float* res)
    {
        interpolation_lineaire_2f(a,b,(-cosf(M_PI*t)+1.)/2.,res);
    }

    static void interpolation_cosinusoidale_2d(double* a,double* b,double t,double* res)
    {
        interpolation_lineaire_2d(a,b,(-cos(M_PI*t)+1.)/2.,res);
    }


    static void interpolation_cosinusoidale_3f(float* a,float* b,float t,float* res)
    {
        interpolation_lineaire_3f(a,b,(-cosf(M_PI*t)+1.)/2.,res);
    }

    static void interpolation_cosinusoidale_3d(double* a,double* b,double t,double* res)
    {
        interpolation_lineaire_3d(a,b,(-cos(M_PI*t)+1.)/2.,res);
    }



};


//-------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================
//      Enveloppe d'interpolation
//      x(temps) et y(position) varient de 0 à 1. Les points de l'enveloppe doivent être compris entre ces valeurs
//      Type d'interpolation possible entre les points: LINEAIRE et COSINUS
///     ATTENTION à ce que les points se suivent dans un ordre croissant sur l'axe x (temps) !!!
///     Les points situés à x>1. sont ignorés.
//===============================================================================================================
//-------------------------------------------------------------------------------------------------------------------------

#define ENVELOPPE_INTERPOLATION_OK 0

class Enveloppe_Interpolation: public Interpolations
{
    public:
    uint16_t erreur;
    uint8_t type_interpolation;

    private:
    float* points;
    uint32_t nbrPoints;

    public:

    Enveloppe_Interpolation(float* p_points,uint32_t p_nbrPoints,uint8_t p_type=INTERPOLATION_LINEAIRE)
    {
        erreur=ENVELOPPE_INTERPOLATION_OK;
        type_interpolation=p_type;
        nbrPoints=p_nbrPoints;
        points=p_points;
    }

    ~Enveloppe_Interpolation()
    {
    }


    float renvoie_valeur(float t)
    {
        if (t<0.) t=0.;
        if (t>1.) t=1.;
        //Extrémités:
        if (t==0.) return points[1];
        if (t==1.) return points[nbrPoints*2+1];
        //cherche le numéro de l'intervalle:
        uint32_t i;
        for(i=0;i<nbrPoints;i++)
        {
            if (points[(i+1)*2]>=t) break;
        }
        //Position dans l'intervalle:
        float ti=(t-points[i*2])/(points[(i+1)*2]-points[i*2]);

        //Calcul la valeur:
        if (type_interpolation==INTERPOLATION_COSINUS) return interpolation_cosinusoidale_f(points[2*i+1],points[2*i+3],ti);
        else return interpolation_lineaire_f(points[2*i+1],points[2*i+3],ti);
    }



};

//-------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================
//      Interpolation entre deux valeurs
//      Si le temps de départ est inférieur à 0, l'interpolation débute dès le premier appel à renvoie_valeur_parametre()
//      La courbe en entrée ne sert que pour définir les paramètre de la courbe interne.
//===============================================================================================================
//-------------------------------------------------------------------------------------------------------------------------

#define INTERPOLATION_PARAMETRE_OK 0
#define INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE 1

class Interpolation_Parametre: public Interpolations
{
    public:
    uint16_t erreur;
    float t0;
    float valeur_depart;
    float valeur_fin;
    float duree;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;
    Enveloppe_Interpolation* enveloppe;

    bool drapeau_terminee; //True lorsque l'interpolation est terminée.

    public:

    Interpolation_Parametre(float p_t0,float p_depart,float p_fin,float p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL,Enveloppe_Interpolation* p_enveloppe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre::Interpolation_Parametre() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
        enveloppe=p_enveloppe;
    }

    ~Interpolation_Parametre()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(float p_t0,float p_depart,float p_fin,float p_duree,uint8_t p_type=INTERPOLATION_INCONNUE,Courbe_Bezier_Plan* p_courbe=NULL,Enveloppe_Interpolation* p_enveloppe=NULL)
    {
        t0=p_t0;
        valeur_depart=p_depart;
        valeur_fin=p_fin;
        duree=p_duree;
        if(p_type!=INTERPOLATION_INCONNUE)type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
        enveloppe=p_enveloppe;
    }

    //Réinitialise l'interpolation.
    ///La valeur de t0 est mise à -1 !!!
    void reset()
    {
        drapeau_terminee=false;
        t0=-1.;
    }


    float renvoie_valeur_parametre(float t)
    {
        if (drapeau_terminee) return valeur_fin;
        if (t0<0.) t0=t;
        else if (t<t0) return valeur_depart;
        else if (t>t0+duree) { drapeau_terminee=true; return valeur_fin;}

        if (type_interpolation==INTERPOLATION_LINEAIRE)
        {
            if (enveloppe==NULL) return interpolation_lineaire_f(valeur_depart,valeur_fin,(t-t0)/duree);
            else return interpolation_lineaire_f(valeur_depart,valeur_fin,enveloppe->renvoie_valeur((t-t0)/duree));
        }
        else if (type_interpolation==INTERPOLATION_COSINUS)
        {
            if (enveloppe==NULL) return interpolation_cosinusoidale_f(valeur_depart,valeur_fin,(t-t0)/duree);
            else interpolation_cosinusoidale_f(valeur_depart,valeur_fin,enveloppe->renvoie_valeur((t-t0)/duree));
        }

        else if (type_interpolation==INTERPOLATION_BEZIER)
        {
            if (enveloppe==NULL) return interpolation_lineaire_f(valeur_depart,valeur_fin,courbe_bezier->renvoie_ordonnee_via_abscisse((t-t0)/duree));
            else return interpolation_lineaire_f(valeur_depart,valeur_fin,courbe_bezier->renvoie_ordonnee_via_abscisse(enveloppe->renvoie_valeur((t-t0)/duree)));
        }
    }



};

//========================================================================================================
//              Suite d'interpolations
//  Les valeurs se présentent comme suit: valeur,duree,...
//  La dernière durée sert dans le cas d'un bouclage.
//  Si t0 en entrée est inférieur à 0, l'interpolation commence dès le premier appel à renvoie_valeur()
//========================================================================================================

class Suite_Interpolations :public Interpolations
{
    public:
        float t0;
        float duree_totale;
        float* tableau_valeurs;
        uint8_t* tableau_types;
        Courbe_Bezier_Plan** tableau_courbes;
        uint16_t nbrEtapes;
        bool drapeau_terminee;
        bool drapeau_bouclage;



    //-----------------------------------------------
    //      Constructeur
    //-----------------------------------------------
    public:
    Suite_Interpolations(float p_t0,float* p_valeurs, uint8_t* p_types,uint16_t p_nbrEtapes,bool p_bouclage=false,Courbe_Bezier_Plan** p_courbes=NULL)
    {
        t0=p_t0;
        tableau_valeurs=p_valeurs;
        tableau_types=p_types;
        tableau_courbes=p_courbes;
        nbrEtapes=p_nbrEtapes;
        drapeau_terminee=false;
        drapeau_bouclage=p_bouclage;
        maj_duree_totale();
    }

    //-----------------------------------------------
    //      Destructeur
    //-----------------------------------------------
    ~Suite_Interpolations()
    {

    }

    //--------------------------------------
    //      Contrôls de lecture
    //--------------------------------------
    ///Provisoire
    void start()
    {
        drapeau_terminee=false;
        t0=-1.;
    }

    ///Provisoire
    void stop()
    {
        drapeau_terminee=true;
        t0=-1.;
    }

    //Inverse les valeurs=inversion du mouvement
    void inverse()
    {
        uint16_t i,j;
        j=nbrEtapes-1;
        for(i=0;i<nbrEtapes/2;i++)
        {
            float val_mem=tableau_valeurs[2*i];
            float t_mem=tableau_valeurs[2*i+1];
            uint8_t type_mem=tableau_types[i];
            tableau_valeurs[2*i]=tableau_valeurs[2*j];
            tableau_valeurs[2*i+1]=tableau_valeurs[2*j+1];
            tableau_types[i]=tableau_types[j];
            tableau_valeurs[2*j]=val_mem;
            tableau_valeurs[2*j+1]=t_mem;
            tableau_types[j]=type_mem;
            if(tableau_courbes!=NULL)
            {
                Courbe_Bezier_Plan* courbe_mem=tableau_courbes[i];
                tableau_courbes[i]=tableau_courbes[j];
                tableau_courbes[j]=courbe_mem;
            }
            j--;
        }
    }
    //-----------------------------------------------
    //      Initialisations
    //-----------------------------------------------
    void maj_duree_totale()
    {
        uint16_t i;
        duree_totale=0.;
        for(i=0;i<nbrEtapes;i++)
        {
            duree_totale+=tableau_valeurs[i*2+1];
        }
    }

    void determine_t0(float p_t0)
    {
        t0=p_t0;    //Valeur négative si l'on veut relancer immédiatement le mouvement
    }

    //-----------------------------------------------
    //      Renvoie la valeur en fonction du temps
    //-----------------------------------------------
    float renvoie_valeur(float t)
    {
        if (drapeau_terminee)
        {
            if(t0<0.) return tableau_valeurs[0];
            else return tableau_valeurs[(nbrEtapes-1)*2];
        }

        if (t0<0.) t0=t;
        else if (t<t0) return tableau_valeurs[0];
        else if((!drapeau_bouclage) && t>t0+duree_totale-tableau_valeurs[(nbrEtapes-1)*2+1])
        {
            drapeau_terminee=true;
            return tableau_valeurs[(nbrEtapes-1)*2];
        }
        else if (t>t0+duree_totale)
        {
            drapeau_terminee=true;
            return tableau_valeurs[0];
        }

        //Détermine l'étape actuelle:
        uint16_t etape_actuelle;
        float duree_actuelle;
        float t_relatif=0.;
        for(etape_actuelle=0;etape_actuelle<nbrEtapes;etape_actuelle++)
        {
            duree_actuelle=tableau_valeurs[etape_actuelle*2+1];
            if((duree_actuelle+t_relatif+t0)>t) break;
            t_relatif+=duree_actuelle;
        }

        float position=(t-t0-t_relatif)/duree_actuelle;
        uint8_t type_actuel=tableau_types[etape_actuelle];
        float valeur_depart=tableau_valeurs[etape_actuelle*2];
        float valeur_fin;

        if(etape_actuelle==nbrEtapes-1) valeur_fin=tableau_valeurs[0];
        else valeur_fin=tableau_valeurs[etape_actuelle*2+2];

        if (type_actuel==INTERPOLATION_LINEAIRE)
        {
            return interpolation_lineaire_f(valeur_depart,valeur_fin,position);
        }
        else if (type_actuel==INTERPOLATION_COSINUS)
        {
            return interpolation_cosinusoidale_f(valeur_depart,valeur_fin,position);
        }

        else if (type_actuel==INTERPOLATION_BEZIER)
        {
            return interpolation_lineaire_f(valeur_depart,valeur_fin,tableau_courbes[etape_actuelle]->renvoie_ordonnee_via_abscisse(position));
        }
    }
};



//-------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================
//      Interpolation entre deux coordonnées bidimentionnelles
//      Si le temps de départ est inférieur à 0, l'interpolation débute dès le premier appel à renvoie_valeur_parametre()
//      La courbe en entrée ne sert que pour définir les paramètre de la courbe interne.
//===============================================================================================================
//-------------------------------------------------------------------------------------------------------------------------

class Interpolation_Parametre_2f: public Interpolations
{
    public:
    uint16_t erreur;
    float t0;
    float valeur_depart[2];
    float valeur_fin[2];
    float duree;

    bool drapeau_terminee;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;

    public:

    Interpolation_Parametre_2f(float p_t0,float* p_depart,float* p_fin,float p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre_2f::Interpolation_Parametre_2f() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
    }

    ~Interpolation_Parametre_2f()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(float p_t0,float* p_depart,float* p_fin,float p_duree,uint8_t p_type=INTERPOLATION_INCONNUE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        t0=p_t0;
        valeur_depart={p_depart[0],p_depart[1]};
        valeur_fin={p_fin[0],p_fin[1]};
        duree=p_duree;
        if(p_type!=INTERPOLATION_INCONNUE) type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
    }


    void renvoie_valeur_parametre(float t, float* resultat)
    {
        if (t0<0.) t0=t;
        else if (t<t0) {resultat[0]=valeur_depart[0];resultat[1]=valeur_depart[1]; return;}
        else if (t>t0+duree)  {resultat[0]=valeur_fin[0];resultat[1]=valeur_fin[1]; drapeau_terminee=true; return;}

        if (type_interpolation==INTERPOLATION_LINEAIRE) interpolation_lineaire_2f(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
        else if (type_interpolation==INTERPOLATION_COSINUS) interpolation_cosinusoidale_2f(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
    }



};

//===============================================================================================================
//      Interpolation de deux valeurs - type double
//===============================================================================================================

class Interpolation_Parametre_2d: public Interpolations
{
    public:
    uint16_t erreur;
    double t0;
    double valeur_depart[2];
    double valeur_fin[2];
    double duree;

    bool drapeau_terminee;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;

    public:

    Interpolation_Parametre_2d(double p_t0,double* p_depart,double* p_fin,double p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre_2d::Interpolation_Parametre_2d() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
    }

    ~Interpolation_Parametre_2d()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(double p_t0,double* p_depart,double* p_fin,double p_duree,uint8_t p_type,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        t0=p_t0;
        valeur_depart={p_depart[0],p_depart[1]};
        valeur_fin={p_fin[0],p_fin[1]};
        duree=p_duree;
        type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
    }


    void renvoie_valeur_parametre(double t, double* resultat)
    {
        if (t0<0.) t0=t;
        else if (t<t0) {resultat[0]=valeur_depart[0];resultat[1]=valeur_depart[1]; return;}
        else if (t>t0+duree)  {resultat[0]=valeur_fin[0];resultat[1]=valeur_fin[1];drapeau_terminee=true; return;}


        if (type_interpolation==INTERPOLATION_LINEAIRE) return interpolation_lineaire_2d(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
        if (type_interpolation==INTERPOLATION_COSINUS) return interpolation_cosinusoidale_2d(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
    }

};


//-------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================
//      Interpolation entre deux coordonnées tridimensionnelle
//      Si le temps de départ est inférieur à 0, l'interpolation débute dès le premier appel à renvoie_valeur_parametre()
//      La courbe en entrée ne sert que pour définir les paramètre de la courbe interne.
//===============================================================================================================
//-------------------------------------------------------------------------------------------------------------------------

class Interpolation_Parametre_3f: public Interpolations
{
    public:
    uint16_t erreur;
    float t0;
    float valeur_depart[3];
    float valeur_fin[3];
    float duree;

    bool drapeau_terminee;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;

    public:

    Interpolation_Parametre_3f(float p_t0,float* p_depart,float* p_fin,float p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre_3f::Interpolation_Parametre_3f() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
    }

    ~Interpolation_Parametre_3f()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(float p_t0,float* p_depart,float* p_fin,float p_duree,uint8_t p_type,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        t0=p_t0;
        valeur_depart={p_depart[0],p_depart[1],p_depart[2]};
        valeur_fin={p_fin[0],p_fin[1],p_fin[2]};;
        duree=p_duree;
        type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
    }


    void renvoie_valeur_parametre(float t, float* resultat)
    {
        if (t0<0.) t0=t;
        else if (t<t0) {resultat[0]=valeur_depart[0];resultat[1]=valeur_depart[1];resultat[2]=valeur_depart[2]; return;}
        else if (t>t0+duree)  {resultat[0]=valeur_fin[0];resultat[1]=valeur_fin[1];resultat[2]=valeur_fin[2]; drapeau_terminee=true; return;}

        if (type_interpolation==INTERPOLATION_LINEAIRE) interpolation_lineaire_3f(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
        else if (type_interpolation==INTERPOLATION_COSINUS) interpolation_cosinusoidale_3f(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
    }



};

//===============================================================================================================
//      Interpolation de trois valeurs - type double
//===============================================================================================================

class Interpolation_Parametre_3d: public Interpolations
{
    public:
    uint16_t erreur;
    double t0;
    double valeur_depart[3];
    double valeur_fin[3];
    double duree;

    bool drapeau_terminee;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;

    public:

    Interpolation_Parametre_3d(double p_t0,double* p_depart,double* p_fin,double p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre_3d::Interpolation_Parametre_3d() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
    }

    ~Interpolation_Parametre_3d()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(double p_t0,double* p_depart,double* p_fin,double p_duree,uint8_t p_type,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        t0=p_t0;
        valeur_depart={p_depart[0],p_depart[1],p_depart[2]};
        valeur_fin={p_fin[0],p_fin[1],p_fin[2]};;
        duree=p_duree;
        type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
    }


    void renvoie_valeur_parametre(double t, double* resultat)
    {
        if (t0<0.) t0=t;
        else if (t<t0) {resultat[0]=valeur_depart[0];resultat[1]=valeur_depart[1];resultat[2]=valeur_depart[2]; return;}
        else if (t>t0+duree)  {resultat[0]=valeur_fin[0];resultat[1]=valeur_fin[1];resultat[2]=valeur_fin[2]; drapeau_terminee=true; return;}


        if (type_interpolation==INTERPOLATION_LINEAIRE) return interpolation_lineaire_3d(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
        if (type_interpolation==INTERPOLATION_COSINUS) return interpolation_cosinusoidale_3d(valeur_depart,valeur_fin,(t-t0)/duree,resultat);
    }

};
//-------------------------------------------------------------------------------------------------------------------------
//===============================================================================================================
//      Interpolation de l'orientation d'un repère
//==============================================================================================================
//-------------------------------------------------------------------------------------------------------------------------



class Interpolation_Repere: public Interpolations
{
    public:
    uint16_t erreur;
    double t0;

    dksRepere repere_depart;
    dksRepere repere_fin;
    dksAxeRotation axe_rotation;
    double duree;

    bool drapeau_terminee;

    uint8_t type_interpolation;
    Courbe_Bezier_Plan* courbe_bezier;

    public:

    Interpolation_Repere(double p_t0,dksRepere* p_depart,dksRepere* p_fin,double p_duree,uint8_t p_type=INTERPOLATION_LINEAIRE,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        erreur=INTERPOLATION_PARAMETRE_OK;
        courbe_bezier=new Courbe_Bezier_Plan();
        if(courbe_bezier==NULL)
        {
            cout<<"ERREUR dans Interpolation_Parametre::Interpolation_Parametre() - Impossible de créer la courbe d'interpolation"<<endl;
            erreur=INTERPOLATION_PARAMETRE_ERREUR_MEMOIRE;
            return;
        }
        determine_parametres(p_t0,p_depart,p_fin,p_duree,p_type,p_courbe);
    }

    ~Interpolation_Repere()
    {
        if (courbe_bezier!=NULL) delete courbe_bezier;
    }

    //Pour définir tout les paramètres d'un coup.
    void determine_parametres(double p_t0,dksRepere* p_depart,dksRepere* p_fin,double p_duree,uint8_t p_type,Courbe_Bezier_Plan* p_courbe=NULL)
    {
        t0=p_t0;
        repere_depart.copie_axes(p_depart);
        repere_fin.copie_axes(p_fin);
        duree=p_duree;
        type_interpolation=p_type;
        drapeau_terminee=false;
        if (p_courbe!=NULL) courbe_bezier->copie_parametres(p_courbe);
        //Calcul l'angle et l'axe de rotation entre l'orientation de départ et l'orientation de fin:
        axe_rotation.calcul_rotation_entre_deux_reperes(&repere_depart,&repere_fin);
    }


    void renvoie_orientation_repere(double t, dksRepere* resultat)
    {
        if (t0<0.) t0=t;
        else if (t<t0) {resultat->copie_axes(&repere_depart); return;}
        else if (t>t0+duree)  {resultat->copie_axes(&repere_fin); drapeau_terminee=true; return;}

        double angle_t;
        resultat->copie_axes(&repere_depart);
        if (type_interpolation==INTERPOLATION_LINEAIRE) angle_t=interpolation_lineaire_d(0.,axe_rotation.angle,(t-t0)/duree);
        if (type_interpolation==INTERPOLATION_COSINUS) angle_t=interpolation_cosinusoidale_d(0.,axe_rotation.angle,(t-t0)/duree);
        resultat->rotation(angle_t,&axe_rotation);
    }



};


#endif // INTERPOLATIONS_H_INCLUDED
