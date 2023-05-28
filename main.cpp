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
#include <gl/glew.h>
#include <gl/glut.h>

using namespace std;

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

#include "Utiles/Texte.h"
#include "Utiles/Musique.h"

#include <stdlib.h>



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

double t_depart=-1.;

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
float fps;
float fps_affiche;
float temps_calcul_image; //temps mis pour le rendu de l'image (en s)
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

//=================================================
//====================================================================
//                      Affichage
//====================================================================
//=================================================
static void affiche_fps()
{
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(1.,1.,0.);
    Texte::affiche_nombre(-0.9,0.8,fps_affiche,3);
    //Texte::affiche_nombre(-0.7,0.8,beat,3);
    //Texte::affiche_nombre(-0.7,0.8,temps_calcul_image,3);
}

static void affiche_contexte()
{
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(1.,1.,0.);
    Texte::affiche_texte(-0.9,0.6,(char*)glGetString(GL_VERSION));
    Texte::affiche_texte(-0.9,0.5,(char*)glGetString(GL_RENDERER));
    Texte::affiche_texte(-0.9,0.4,(char*)glGetString(GL_VENDOR));
}

static void affiche_t(double t)
{
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(1.,1.,0.);
    Texte::affiche_nombre(-0.9,0.7,t,2);
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

    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0; //t en secondes
    if (t_depart<0.)
    {
        t_depart=t;
         #if JOUE_MUSIQUE
        la_musique->joue(true);
        #endif
    }
    t-=t_depart;

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
        temps_calcul_image=glutGet(GLUT_ELAPSED_TIME)/1000.-t_depart-t;
        fps=1./temps_calcul_image;

        affiche_fps();
        //affiche_contexte();
        affiche_t(t);
    #endif

    glutSwapBuffers();

    //if (cptr_images++==1)drapeau_debug=true;
    //ralenti();
}



//---------------------------
//Pour éviter les oscillations du compteur:
//---------------------------

static void rafraichi_affiche_fps(int num)
{
        fps_affiche=fps;
        glutTimerFunc(100,&rafraichi_affiche_fps,0);
}

//=================================================
//                      Fin du programme
//=================================================

//Le temps des inits de fin, les fonctions de rafraîchissement sont désactivées:
static void fin_redimensionne_fenetre(int width, int height){}
static void fin_affichage_principal(void){}
static void fin_attente_retour_faisceau(void){glutPostRedisplay();}


static void fin_du_programme()
{
    scene_logoGp->drapeau_scene_terminee=true;
    scene_tunnel->drapeau_scene_terminee=true;
    scene_caduce->drapeau_scene_terminee=true;
    scene_starfield->drapeau_scene_terminee=true;
    scene_arbre->drapeau_scene_terminee=true;
    scene_greets->drapeau_scene_terminee=true;
    scene_credits->drapeau_scene_terminee=true;
    scene_fin->drapeau_scene_terminee=true;

    glutReshapeFunc(fin_redimensionne_fenetre);
    glutDisplayFunc(fin_affichage_principal);
    glutIdleFunc(fin_attente_retour_faisceau);

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
    glutPostRedisplay();
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
    glutPostRedisplay();
}

void touche_speciale_relachee(int key, int x, int y)
{
    scene_actuelle->touche_speciale_relachee(key,x,y);
    glutPostRedisplay();
}
void touche_appuyee(unsigned char key, int x, int y)
{
    scene_actuelle->touche_appuyee(key,x,y);
    glutPostRedisplay();
}

void touche_relachee(unsigned char key, int x, int y)
{
    scene_actuelle->touche_relachee(key,x,y);
    glutPostRedisplay();
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
    glutPostRedisplay();
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
    uint32_t le=640;
    mon_ecran=new dksEcran(640,384,PLEIN_ECRAN);
    //mon_ecran=new dksEcran(800,600,true);
    //--------------- Initialise GLUT:
    glutInit(&argc, argv);
    glutInitWindowSize(mon_ecran->resolution_x,mon_ecran->resolution_y);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Genesis ¤ Project - Passages");
    glutReshapeFunc(redimensionne_fenetre);
    glutDisplayFunc(affichage_principal);
    glutIdleFunc(attente_retour_faisceau);

#if EDITEUR_ACTIF
    glutKeyboardFunc(touche_appuyee);
    glutKeyboardUpFunc(touche_relachee);
    glutSpecialFunc(touche_speciale_appuyee);
    glutSpecialUpFunc(touche_speciale_relachee);
    glutMotionFunc(mouvement_souris);
#else
    glutKeyboardFunc(quitte_demo);
#endif

    //------------------ Initialise Glew (gestionnaire d'extensions OpenGl):
    GLenum erreur_glew=glewInit();
    if (erreur_glew!=GLEW_OK)
    {
        cout<<"ERREUR dans glewInit() - "<< glewGetErrorString(erreur_glew)<<endl;
        drapeau_erreur=true;
        exit(EXIT_FAILURE);
    }
    cout<<"Version GLEW: "<<glewGetString(GLEW_VERSION)<<endl;

    //------------------ Test la carte graphique:
    uint8_t id_fabriquant;
    if (strcmp((char*)glGetString(GL_VENDOR),"NVIDIA Corporation")==0) id_fabriquant=DKSSHADERS_FABRIQUANT_NVIDIA;
    else if (strcmp((char*)glGetString(GL_VENDOR),"ATI Technologies Inc.")==0) id_fabriquant=DKSSHADERS_FABRIQUANT_ATI;
    else
    {
        MessageBox(0,"Incompatible graphic card!\nFor the moment, only NVIDIA and ATI cards are 100% supported...","Genesis ¤ Project - Passages",MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }
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

    //------------ Initialise les timers:
    #if AFFICHE_FPS
        glutTimerFunc(500, &rafraichi_affiche_fps, 0);
    #endif
    //------------ Chargement de la musique:

    //------------ Lancement:
    if (mon_ecran->est_en_plein_ecran()) glutFullScreen();
    glutMainLoop();
}

