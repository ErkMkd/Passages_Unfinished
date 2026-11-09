/*
*
*
*   Environnement de base
*
*
*/

#define EDITEUR_ACTIF true
#define EDITEUR_TRAJECTOIRE true
#define EDITEUR_PERSONNAGE false

#define JOUE_MUSIQUE false
#define PLEIN_ECRAN false
#define AFFICHE_FPS true

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

using namespace std;

#include "Utiles/Tableaux.h"
#include "Utiles/Rapport_Assertions.h"
#include "Utiles/Horloge.h"
#include "Utiles/Panneau_Developpement.h"
#include "dksMoteur2d/dksEcran.h"
#include "dksMoteur3d/LWO/dksChargeLWO2.h"
#include "dksMoteur3d/LWO/dksConvertionLWO2.h"
#include "dksMoteur3d/dksMoteur3d.h"
#include "dksMoteur3d/Rendu/dksShaders.h"
#include "dksMoteur3d/Volumes/dksCube.h"
#include "dksMoteur3d/Geometrie_Espace/dksTrajectoire.h"
#include "scenes/Scene_LogoGP.h"
#include "scenes/Scene_Tunnel.h"
#include "scenes/Scene_Caduce.h"
#include "scenes/Scene_Starfield.h"
#include "scenes/Scene_Arbre.h"
#include "scenes/Scene_Greetings.h"
#include "scenes/Scene_Jungler_Credits.h"
#include "scenes/Scene_Fin.h"

#include "Utiles/Musique.h"

#include <stdlib.h>


//============ Portables à deux cartes graphiques: demande la carte dédiée (NVIDIA Optimus, AMD PowerXpress)
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement=1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance=1;
}

//============ Console en UTF-8, et répertoire courant sur les données de la démo (textures, modèles, shaders...).
//             Doit précéder les variables statiques ci-dessous, dont certaines chargent des fichiers.
static bool environnement_initialise=[]()
{
    SetConsoleOutputCP(CP_UTF8);
    Rapport_Assertions::installe();
    #ifdef PASSAGES_DONNEES
    if (GetFileAttributesA("sources_shaders")==INVALID_FILE_ATTRIBUTES) SetCurrentDirectoryA(PASSAGES_DONNEES);
    #endif
    return true;
}();

//============ Initialise les variables statiques:
uint32_t dksSourceLumineuse::allocation_lumieres_openGl=0x0;
dksInitialiseShaders* dksShaders::bibliotheque=NULL;
double dksRendu::delta_t=0.;
double dksRendu::position_temps=0.;
bool dksSprite3d::openGl_mem[6];
bool Musique::drapeau_son_initialise=false;
GLint dksRendu::nbr_unites_textures_max=0;
Image* dksTexture::image_transparente=new Image("textures/transparente.tif");

//============= Musique
Musique* la_musique=NULL;
Musique* musique_fin=NULL;

//============ Gestion du temps:

Horloge horloge;

//============ Fenêtre:
GLFWwindow* fenetre=NULL;
Panneau_Developpement* panneau=NULL;

//============ Objets:
dksEcran* mon_ecran=NULL;

//============= scenes:
Scene* scene_actuelle=NULL;

dksOeil* oeil_courant;
Scene_LogoGP* scene_logoGp=NULL;
Scene_Tunnel* scene_tunnel=NULL;
Scene_Caduce* scene_caduce=NULL;
Scene_Starfield* scene_starfield=NULL;
Scene_Arbre* scene_arbre=NULL;
Scene_Greetings* scene_greets=NULL;
Scene_Jungler_Credits* scene_credits=NULL;
Scene_Fin* scene_fin=NULL;

//============ Debugage:
bool drapeau_erreur=false;
bool drapeau_debug=false;

uint32_t cptr_images=0;
uint32_t beat=0;
double temps_calcul_image; //temps mis pour le rendu de l'image (en s)
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

//****************************************************
//              Redimensionnement de la fenêtre
//****************************************************


static void redimensionne_fenetre(int width, int height)
{
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    mon_ecran->determine_dimensions(width,height);
    scene_actuelle->oeil_courant->determine_resolution(width,height);
    scene_actuelle->maj_dimensions_fenetre();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

//-------------------------------------------------------
//  Ralentissement pour simuler les cartes plus lentes
//-------------------------------------------------------
void ralenti()
    {
        for(uint32_t i=0;i<1000000;i++)
        {
            double ral=double(rand());
            ral=sqrt(ral);
        }
    }


//=================================================================================================================================================
//------------------------------------------------------------------------------
//                  Affichage principal
//          Gestion de l'enchainement des scènes
//------------------------------------------------------------------------------
//=================================================================================================================================================
static void affichage_principal(void)
{
    if (drapeau_debug) return;

    double debut_image=Horloge::temps_reel();
    double t=horloge.avance(); //t en secondes, 0 à la première image
    static bool premiere_image=true;
    if (premiere_image)
    {
        premiere_image=false;
         #if JOUE_MUSIQUE
        la_musique->joue(true);
        #endif
    }

    dksRendu::delta_t=t-dksRendu::position_temps;
    dksRendu::position_temps=t;


    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Pour sauter des scènes:

    scene_logoGp->drapeau_scene_terminee=true;
    scene_tunnel->drapeau_scene_terminee=true;
    scene_caduce->drapeau_scene_terminee=true;
    //scene_starfield->drapeau_scene_terminee=true;
    //scene_arbre->drapeau_scene_terminee=true;
    //scene_greets->drapeau_scene_terminee=true;
    //scene_credits->drapeau_scene_terminee=true;
    //scene_fin->drapeau_scene_terminee=true;



    //--------------------------------------------------------
    //                      Logo
    //--------------------------------------------------------

    if (!scene_logoGp->drapeau_scene_terminee)
    {
        if (!scene_logoGp->drapeau_initialisee)
        {
            scene_logoGp->initialise_t0(1./60.);
            scene_logoGp->active();
            scene_actuelle=scene_logoGp;
            scene_logoGp->drapeau_initialisee=true;
        }
        scene_logoGp->affiche(t);
    }

    //--------------------------------------------------------
    //     Tunnel
    //--------------------------------------------------------
    else if (!scene_tunnel->drapeau_scene_terminee)
    {
        if (!scene_tunnel->drapeau_initialisee)
        {
            scene_logoGp->desactive();
            scene_tunnel->initialise_t0(scene_logoGp->delta_t);
            scene_tunnel->active();
            //scene_tunnel->active(0);
            scene_actuelle=scene_tunnel;
            scene_tunnel->drapeau_initialisee=true;
        }
        scene_tunnel->affiche(t);
    }

    //--------------------------------------------------------
    //     Caducée
    //--------------------------------------------------------

    else if (!scene_caduce->drapeau_scene_terminee)
    {
        if (!scene_caduce->drapeau_initialisee)
        {
            scene_tunnel->desactive();
            scene_caduce->initialise_t0(scene_tunnel->delta_t);
            scene_caduce->active();
            scene_actuelle=scene_caduce;
            scene_caduce->drapeau_initialisee=true;
        }

        scene_caduce->affiche(t);
    }

    //--------------------------------------------------------
    //     Starfield
    //--------------------------------------------------------
    else if (!scene_starfield->drapeau_scene_terminee)
    {

        if (!scene_starfield->drapeau_initialisee)
        {

            scene_caduce->desactive();
            scene_starfield->initialise_t0(scene_caduce->delta_t);//-0.035
            scene_starfield->active();
            scene_actuelle=scene_starfield;
            scene_starfield->drapeau_initialisee=true;
        }
        scene_starfield->affiche(t);
    }

    //--------------------------------------------------------
    //     Arbre
    //--------------------------------------------------------
    else if (!scene_arbre->drapeau_scene_terminee)
    {
        if (!scene_arbre->drapeau_initialisee)
        {
            scene_starfield->desactive();
            scene_arbre->initialise_t0(1./60.);
            scene_arbre->active();
            scene_actuelle=scene_arbre;
            scene_arbre->drapeau_initialisee=true;
        }
        scene_arbre->affiche(t);
    }

    //--------------------------------------------------------
    //     Greetings
    //--------------------------------------------------------
    else if (!scene_greets->drapeau_scene_terminee)
    {

        if (!scene_greets->drapeau_initialisee)
        {
            scene_arbre->desactive();
            scene_greets->initialise_t0(scene_arbre->delta_t);
            scene_greets->active();
            scene_actuelle=scene_greets;
            scene_greets->drapeau_initialisee=true;
        }

        scene_greets->affiche(t);
    }

    //--------------------------------------------------------
    //     Jongleur et crédits
    //--------------------------------------------------------
    else if (!scene_credits->drapeau_scene_terminee)
    {

        if (!scene_credits->drapeau_initialisee)
        {
            scene_greets->desactive();
            scene_credits->initialise_t0(1./60.);
            scene_credits->active();
            scene_actuelle=scene_credits;
            scene_credits->drapeau_initialisee=true;
        }

        scene_credits->affiche(t);
    }

    //--------------------------------------------------------
    //    Générique de fin
    //--------------------------------------------------------
    else if (!scene_fin->drapeau_scene_terminee)
    {

        if (!scene_fin->drapeau_initialisee)
        {
            #if JOUE_MUSIQUE
            la_musique->stop();
            musique_fin->joue(true);
            musique_fin->determine_volume(0.75);
            #endif
            scene_credits->desactive();
            scene_fin->initialise_t0(1./60.);
            scene_fin->active();
            scene_actuelle=scene_fin;
            scene_fin->drapeau_initialisee=true;
        }

        scene_fin->affiche(t);
    }

    //ralenti();
    #if AFFICHE_FPS
        dksRendu::desactive_textures();
        glUseProgram(0);
        //glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
        temps_calcul_image=Horloge::temps_reel()-debut_image;
        if (panneau!=NULL) panneau->affiche(&horloge,scene_actuelle->nom,temps_calcul_image);
    #endif

    glfwSwapBuffers(fenetre);

    //if (cptr_images++==1)drapeau_debug=true;
    //ralenti();
}



//=================================================
//                      Fin du programme
//  Appelée à la fermeture de la fenêtre, ou par exit() (erreur, touche Échap)
//=================================================

static void fin_du_programme()
{
    static bool drapeau_fin=false;
    if (drapeau_fin) return;
    drapeau_fin=true;

    Scene* scenes[]={scene_logoGp,scene_tunnel,scene_caduce,scene_starfield,scene_arbre,scene_greets,scene_credits,scene_fin};
    for (Scene* scene:scenes) if (scene!=NULL) scene->drapeau_scene_terminee=true;

    if (panneau!=NULL)
    {
        delete panneau;
        panneau=NULL;
    }

    dksRendu::desactive_textures();
    glUseProgram(0);

    if(la_musique!=NULL) delete la_musique;
    if (scene_fin!=NULL) delete scene_fin;
    if (scene_credits!=NULL) delete scene_credits;
    if (scene_greets!=NULL) delete scene_greets;
    if (scene_arbre!=NULL) delete scene_arbre;
    if (scene_starfield!=NULL) delete scene_starfield;
    if (scene_caduce!=NULL) delete scene_caduce;
    if (scene_tunnel!=NULL) delete scene_tunnel;
    if (scene_logoGp!=NULL) delete scene_logoGp;
    if (dksShaders::bibliotheque!= NULL) delete dksShaders::bibliotheque;
    if (mon_ecran!= NULL) delete mon_ecran;

    if(dksTexture::image_transparente!=NULL) delete dksTexture::image_transparente;

    if (drapeau_erreur)
    {
        cout<<"OOPS - ERROR - Debugging needed!"<<endl;
        MessageBox(0,"ERROR !!- GURU MEDITATION\nPlease send me a screen shot of the console or copy/paste console messages for debugging...","Genesis ¤ Project - Passages",MB_OK | MB_ICONEXCLAMATION);
    }

    else cout<<"Demo correctly destructed - Bye bye sweet darling!..."<<endl;

}




//=========================================================================
//           Ce qui se passe lors de l'attente de retour faisceau
//=========================================================================
static void attente_retour_faisceau(void)
{
    scene_actuelle->attente_retour_faisceau();
}

//============================================================================
//                     Initialisations
//============================================================================
static void initialisations()
{
    #if JOUE_MUSIQUE
        la_musique=new Musique("Musique/music.mp3");
        musique_fin=new Musique("Musique/end_music.mp3");

        if(la_musique->erreur!=MUSIQUE_OK || musique_fin->erreur!=MUSIQUE_OK)
        {
            drapeau_erreur=true;exit(EXIT_FAILURE);
        }
    #endif

    if (!mon_ecran->creation_tempons_rendus())
    {
        drapeau_erreur=true;exit(EXIT_FAILURE);
    }

    scene_logoGp=new Scene_LogoGP("Scene_LogoGP",mon_ecran,la_musique);
    if (scene_logoGp==NULL || scene_logoGp->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;exit(EXIT_FAILURE);
    }

    scene_tunnel=new Scene_Tunnel("Scene_Tunnel",mon_ecran,la_musique,scene_logoGp);
    if (scene_tunnel==NULL || scene_tunnel->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }

    scene_caduce=new Scene_Caduce("Scene_Caducee",mon_ecran,la_musique,scene_tunnel);
    if (scene_caduce==NULL || scene_caduce->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }

    scene_starfield=new Scene_Starfield("Scene_Starfield",mon_ecran,la_musique,scene_caduce);
    if (scene_starfield==NULL || scene_starfield->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }


    scene_arbre=new Scene_Arbre("Scene_Arbre",mon_ecran,la_musique,scene_starfield);
    if (scene_arbre==NULL || scene_arbre->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }

    scene_greets=new Scene_Greetings("Scene_Greetings",mon_ecran,la_musique,scene_arbre);
    if (scene_greets==NULL || scene_greets->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }

    scene_credits=new Scene_Jungler_Credits("Scene_Jungler",mon_ecran,la_musique,scene_greets);
    if (scene_credits==NULL || scene_credits->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }

    scene_fin=new Scene_Fin("Scene_Fin",mon_ecran,la_musique);
    if (scene_fin==NULL || scene_fin->erreur!=SCENE_OK)
    {
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }


    scene_actuelle=scene_logoGp;

    dksMoteur3d::initialise_opengl();

}
//============================================================================
//                  Gestion clavier
//============================================================================
static void touche_speciale_appuyee(int key, int x, int y)
{

    scene_actuelle->touche_speciale_appuyee(key,x,y);
}

void touche_speciale_relachee(int key, int x, int y)
{
    scene_actuelle->touche_speciale_relachee(key,x,y);
}
void touche_appuyee(unsigned char key, int x, int y)
{
    scene_actuelle->touche_appuyee(key,x,y);
}

void touche_relachee(unsigned char key, int x, int y)
{
    scene_actuelle->touche_relachee(key,x,y);
}

void mouvement_souris(int x, int y)
{
    scene_actuelle->mouvement_souris(x,y);
}

//--------- Commandes hors éditeur:
void quitte_demo(unsigned char key, int x, int y)
{
    switch(key)
    {
       case 27 :
                //fin_du_programme();
                exit(0);
                break;
    }
}

//============================================================================
//      Traduction des événements GLFW pour les scènes, qui reçoivent les codes
//      de GLUT: caractères Latin-1, et codes GLUT_KEY_xxx pour les touches spéciales.
//============================================================================
static int code_glut_special(int touche)
{
    if (touche>=GLFW_KEY_F1 && touche<=GLFW_KEY_F12) return 1+touche-GLFW_KEY_F1; //GLUT_KEY_F1 à GLUT_KEY_F12
    switch (touche)
    {
        case GLFW_KEY_LEFT: return 100;         //GLUT_KEY_LEFT
        case GLFW_KEY_UP: return 101;           //GLUT_KEY_UP
        case GLFW_KEY_RIGHT: return 102;        //GLUT_KEY_RIGHT
        case GLFW_KEY_DOWN: return 103;         //GLUT_KEY_DOWN
        case GLFW_KEY_PAGE_UP: return 104;      //GLUT_KEY_PAGE_UP
        case GLFW_KEY_PAGE_DOWN: return 105;    //GLUT_KEY_PAGE_DOWN
        case GLFW_KEY_HOME: return 106;         //GLUT_KEY_HOME
        case GLFW_KEY_END: return 107;          //GLUT_KEY_END
        case GLFW_KEY_INSERT: return 108;       //GLUT_KEY_INSERT
        default: return -1;
    }
}

//Touches de contrôle que GLUT transmet comme des caractères:
static int code_glut_controle(int touche)
{
    switch (touche)
    {
        case GLFW_KEY_ESCAPE: return 27;
        case GLFW_KEY_ENTER: case GLFW_KEY_KP_ENTER: return 13;
        case GLFW_KEY_BACKSPACE: return 8;
        case GLFW_KEY_TAB: return 9;
        case GLFW_KEY_DELETE: return 127;
        default: return -1;
    }
}

static int touche_du_caractere=GLFW_KEY_UNKNOWN;   //Touche dont GLFW va transmettre le caractère
static map<int,unsigned char> caracteres_appuyes;  //Caractère transmis à l'appui de chaque touche, pour son relâchement

static void clavier(GLFWwindow* f,int touche,int code_physique,int action,int modificateurs)
{
    if (Panneau_Developpement::utilise_clavier()) return;
    double x,y;
    glfwGetCursorPos(f,&x,&y);

    #if EDITEUR_ACTIF
    int code=code_glut_special(touche);
    if (code>=0)
    {
        if (action==GLFW_RELEASE) touche_speciale_relachee(code,int(x),int(y));
        else touche_speciale_appuyee(code,int(x),int(y));     //Appui ou répétition
        return;
    }
    code=code_glut_controle(touche);
    if (action==GLFW_RELEASE)
    {
        if (code>=0) touche_relachee(code,int(x),int(y));
        else
        {
            map<int,unsigned char>::iterator c=caracteres_appuyes.find(touche);
            if (c!=caracteres_appuyes.end())
            {
                touche_relachee(c->second,int(x),int(y));
                caracteres_appuyes.erase(c);
            }
        }
    }
    else if (code>=0) touche_appuyee(code,int(x),int(y));
    else touche_du_caractere=touche;    //Le caractère arrive ensuite dans caractere()
    #else
    if (touche==GLFW_KEY_ESCAPE && action==GLFW_PRESS) quitte_demo(27,int(x),int(y));
    #endif
}

static void caractere(GLFWwindow* f,unsigned int point_de_code)
{
    #if EDITEUR_ACTIF
    if (Panneau_Developpement::utilise_clavier() || point_de_code>255) return;
    double x,y;
    glfwGetCursorPos(f,&x,&y);
    caracteres_appuyes[touche_du_caractere]=(unsigned char)point_de_code;
    touche_appuyee((unsigned char)point_de_code,int(x),int(y));
    #endif
}

//La souris n'est transmise que bouton enfoncé, comme avec glutMotionFunc().
static void souris(GLFWwindow* f,double x,double y)
{
    #if EDITEUR_ACTIF
    if (Panneau_Developpement::utilise_souris()) return;
    if (glfwGetMouseButton(f,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS
        || glfwGetMouseButton(f,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS
        || glfwGetMouseButton(f,GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) mouvement_souris(int(x),int(y));
    #endif
}

static void taille_fenetre(GLFWwindow* f,int largeur,int hauteur)
{
    if (largeur>0 && hauteur>0) redimensionne_fenetre(largeur,hauteur);   //0 quand la fenêtre est réduite
}

//============================================================================
//                     Programme principal
//============================================================================

int main(int argc, char *argv[])
{
    la_musique=NULL;
    musique_fin=NULL;
    atexit(fin_du_programme);
    //--------------- Initialise l'écran:
    mon_ecran=new dksEcran(640,384,PLEIN_ECRAN);
    //mon_ecran=new dksEcran(800,600,true);
    //--------------- Initialise GLFW et crée la fenêtre:
    if (!glfwInit())
    {
        cout<<"ERREUR dans glfwInit()"<<endl;
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }
    fenetre=glfwCreateWindow(mon_ecran->resolution_x,mon_ecran->resolution_y,"Genesis ¤ Project - Passages",NULL,NULL);
    if (fenetre==NULL)
    {
        cout<<"ERREUR dans glfwCreateWindow() - Impossible de créer la fenêtre OpenGL"<<endl;
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(fenetre);
    glfwSwapInterval(1);    //Synchronisation verticale

    //------------------ Initialise Glew (gestionnaire d'extensions OpenGl):
    GLenum erreur_glew=glewInit();
    if (erreur_glew!=GLEW_OK)
    {
        cout<<"ERREUR dans glewInit() - "<< glewGetErrorString(erreur_glew)<<endl;
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }
    cout<<"Version GLEW: "<<glewGetString(GLEW_VERSION)<<endl;

    //------------------ Carte graphique:
    cout<<"Carte graphique: "<<glGetString(GL_RENDERER)<<" - "<<glGetString(GL_VENDOR)<<endl;
    cout<<"Version OpenGL: "<<glGetString(GL_VERSION)<<endl;
    //Le jeu de shaders NVIDIA est le seul complet: il est utilisé quelle que soit la carte.
    uint8_t id_fabriquant=DKSSHADERS_FABRIQUANT_NVIDIA;
    //------------ Vérifiacation de l'implémentation d'OpenGL:
    glGetIntegerv(GL_MAX_TEXTURE_UNITS,&dksRendu::nbr_unites_textures_max);
    cout<<"Nombre max d'unités de texture: "<<dksRendu::nbr_unites_textures_max<<endl;

    //------------ Création des variables statiques qui nécessitent les initialisation précédentes:
     dksShaders::bibliotheque= new dksInitialiseShaders(id_fabriquant);
     if (dksShaders::bibliotheque->erreur!=DKSSHADERS_OK)
     {
         drapeau_erreur=true;
         exit(EXIT_FAILURE);
     }


    //------------ Initialise les données:
    initialisations();

    //------------ Clavier, souris et fenêtre:
    glfwSetKeyCallback(fenetre,clavier);
    glfwSetCharCallback(fenetre,caractere);
    glfwSetCursorPosCallback(fenetre,souris);
    glfwSetFramebufferSizeCallback(fenetre,taille_fenetre);

    //------------ Panneau de développement (après les fonctions ci-dessus, qu'il appelle en chaîne):
    #if AFFICHE_FPS
        panneau=new Panneau_Developpement(fenetre);
    #endif

    //------------ Lancement:
    if (mon_ecran->est_en_plein_ecran())
    {
        //dksEcran a déjà réglé la résolution de l'écran: la fenêtre le recouvre.
        GLFWmonitor* moniteur=glfwGetPrimaryMonitor();
        const GLFWvidmode* mode=glfwGetVideoMode(moniteur);
        glfwSetWindowMonitor(fenetre,moniteur,0,0,mode->width,mode->height,mode->refreshRate);
    }
    int largeur,hauteur;
    glfwGetFramebufferSize(fenetre,&largeur,&hauteur);
    redimensionne_fenetre(largeur,hauteur);

    while (!glfwWindowShouldClose(fenetre))
    {
        glfwPollEvents();
        attente_retour_faisceau();
        affichage_principal();
    }

    fin_du_programme();
    glfwTerminate();
    return 0;
}

