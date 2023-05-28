#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED


#define SCENE_OK 0
#define SCENE_ERREUR_INIT 1

#include "../dksMoteur2d/dksPostRendu.h"
#include "../Utiles/Musique.h"

class Scene
{
    public:

    uint16_t erreur;
    char nom[128];
    bool drapeau_scene_terminee;    //True lorsque l'on peut passer à la scène suivante
    bool drapeau_initialisee;          //True lorsque les initialisations des transitions ont été réalisées
    dksOeil* oeil_courant;          //Sert pour l'affichage principal

    float position_temps;           //Le temps global
    float position_temps_depart;    //La date de départ de la scène.
    float position_temps_scene;    //Le temps dans la scène
    float delta_t;                  //Intervalle

    Musique* la_musique;

    protected:

    dksEcran* ecran;
    dksPostRendu* post_rendu;


    public:

    //****************************************
    //      Renvoie une erreur OpenGl
    //****************************************

    static bool erreur_openGl(char* message)
    {
        GLenum erreur_openGl=glGetError();
        if (erreur_openGl!=GL_NO_ERROR)
        {
            cout<<message<<" : "<<gluErrorString(erreur_openGl)<<endl;
            return true;
        }
         return false;
    }

    //*******************************************
    //      Désactive les unités de texture:
    //*******************************************
    static void desactive_textures()
    {
        uint16_t i;
        GLint nbr_unites_textures_max;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS,&nbr_unites_textures_max);
        for (i=0;i<nbr_unites_textures_max;i++)
            {
                glActiveTexture(GL_TEXTURE0+i);
                glDisable(GL_TEXTURE_2D);
                glClientActiveTexture(GL_TEXTURE0+i);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glActiveTexture(GL_TEXTURE0);
    }
    //==========================================================================================
    //                                      Constructeur
    //==========================================================================================
    Scene(char* p_nom, dksEcran* p_ecran=NULL,Musique* p_la_musique=NULL)
    {
        erreur=SCENE_OK;
        ecran=p_ecran;
        post_rendu=new dksPostRendu(ecran);
        drapeau_scene_terminee=false;
        drapeau_initialisee=false;
        oeil_courant=NULL;
        position_temps_depart=-1.;
        position_temps_scene=0.;
        copie_nom(p_nom);
        la_musique=p_la_musique;
    }

    //===================== Destructeur:
    ~Scene()
    {
        if (post_rendu!=NULL) delete post_rendu;
    }

    //-------- Copie le nom:
    void copie_nom(char* p_nom)
    {
        uint32_t i,taille;
        taille=strlen(p_nom);
        if (taille>sizeof(nom)-1)taille=sizeof(nom)-1;
        for(i=0;i<taille;i++)
        {
            nom[i]=p_nom[i];
        }
        nom[i]=0;
    }
    //==========================================================================================
    //                              Fonctions de base:
    //==========================================================================================

    virtual void initialise_t0(double t0)
    {
        delta_t=t0;
    } //Etat initial de la scène. Certaines scènes peuvent avoir besoin de données provenants d'autres scènes, pour les transitions.
    virtual void active(){}   //Activations, initialisations OpenGL juste avant le départ de l'animation
    virtual void desactive(){}
    virtual void maj_dimensions_fenetre(){} //Redimensionne la fenêtre si on est pas en plein écran
    virtual void affiche(double t)  //Mieux vaut appeler cette fonction originelle.
    {
        if (position_temps_depart<0.) position_temps_depart=t;
        else delta_t=t-position_temps;
        position_temps=t;
        position_temps_scene=position_temps-position_temps_depart;

    }
    virtual void attente_retour_faisceau(){}

    virtual void initialise_editeur(){}
    virtual void mouvement_souris(int x, int y){}
    virtual void touche_speciale_appuyee(int key, int x, int y){}
    virtual void touche_speciale_relachee(int key, int x, int y){}
    virtual void touche_appuyee(unsigned char key, int x, int y){}
    virtual void touche_relachee(unsigned char key, int x, int y){}



};

#endif // SCENE_H_INCLUDED
