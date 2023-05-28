#ifndef DKSECRAN_H_INCLUDED
#define DKSECRAN_H_INCLUDED

#include "windows.h"

#define DKSECRAN_OK 0
#define DKSECRAN_ERREUR_MODE 1
#define DKSECRAN_ERREUR_TEMPONS_RENDUS 2

//------------------------------
//          Formats
//------------------------------
#define DKSECRAN_FORMAT_4_3 3
#define DKSECRAN_FORMAT_16_9 2
#define DKSECRAN_FORMAT_16_10 3
#define DKSECRAN_FORMAT_5_4 4

//--------------------------------------------
//Identifiants des tempons de rendu:
//--------------------------------------------

#define DKSECRAN_TEMPON_DEFAUT 0    //Le rendu direct dans le framebuffer par défaut (le seul visible, en fait...)
#define DKSECRAN_TEMPON_ECRAN_1 1
#define DKSECRAN_TEMPON_ECRAN_2 2
#define DKSECRAN_TEMPON_QUART 3
#define DKSECRAN_TEMPON_256 4
#define DKSECRAN_TEMPON_PROFONDEUR_512 5
#define DKSECRAN_TEMPON_PROFONDEUR_1024 6
#define DKSECRAN_TEMPON_PROFONDEUR_2048 7


//Frame buffers ---------- Ces identifiants sont privés!!!!! Utiliser ceux ci-dessus comme identifiants.
enum
        {
          FB_ECRAN_1,                 //Rendu de la scène dans une texture de la taille de l'écran
          FB_ECRAN_2,                 //Rendu de la scène dans une texture de la taille de l'écran
          FB_256_1,                 //Rendu dans une texture de 256*256
          FB_256_2,
          FB_QUART_1,               //Rendu dans une texture du quart de la résolution de l'écran
          FB_QUART_2,
          FB_OMBRES_512,             //Rendu des profondeurs des fragments, dans une texture de 512*512, pour les ombres.
          FB_OMBRES_1024,             //Rendu des profondeurs des fragments, dans une texture de 1024*1024, pour les ombres.
          FB_OMBRES_2048             //Rendu des profondeurs des fragments, dans une texture de 2048*2048, pour les ombres.
        };

//Render buffers
enum
        {
            RB_PROFONDEUR_ECRAN_1,
            RB_PROFONDEUR_ECRAN_2,
            RB_PROFONDEUR_256_1,
            RB_PROFONDEUR_256_2,
            RB_PROFONDEUR_QUART_1,
            RB_PROFONDEUR_QUART_2
        };

//Textures de rendu:
enum
        {
            TX_ECRAN_1,
            TX_ECRAN_2,
            TX_256_1,
            TX_256_2,
            TX_QUART_1,
            TX_QUART_2,
            TX_OMBRES_512,
            TX_OMBRES_1024,
            TX_OMBRES_2048
        };

//==========================================================================================
//                          Classe
//==========================================================================================

class dksEcran
{
    private:
        DEVMODE ModeOrig; //Sauvegarde du mode écran
        bool drapeau_plein_ecran;
        bool drapeau_tempons_rendu;     //True lorsque les tempons de rendu ont été générés
        GLfloat couleur_bords[3];
    public:
        uint16_t    erreur;
        uint32_t resolution_x;
        uint32_t resolution_y;
        uint8_t format;

        GLuint largeur_texture_quart;
        GLuint hauteur_texture_quart;

        //Frame buffers:

        GLuint frame_buffers[9];

        //Render buffers:

        GLuint render_buffers[6];

        //Textures:

        GLuint textures[9];

        //----------------------------------------------
        //Renvoie true si une erreur s'est produite
        //----------------------------------------------
        protected:
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


        //*************************************************************************************************
        //   CONSTRUCTEUR
        //*************************************************************************************************
        public:
        dksEcran(uint32_t p_resolution_x, uint32_t p_resolution_y, bool p_plein_ecran=false,uint8_t p_format=DKSECRAN_FORMAT_16_9)
        {
            uint32_t i;
            format=p_format;
            for(i=0;i<sizeof(frame_buffers)/sizeof(GLuint);i++)frame_buffers[i]=0;
            for(i=0;i<sizeof(render_buffers)/sizeof(GLuint);i++)render_buffers[i]=0;
            for(i=0;i<sizeof(textures)/sizeof(GLuint);i++)textures[i]=0;

            drapeau_tempons_rendu=false;

            resolution_x=p_resolution_x;
            resolution_y=p_resolution_y;
            erreur=DKSECRAN_OK;
            if (p_plein_ecran)
            {
                if (!initialise_ecran()) erreur=DKSECRAN_ERREUR_MODE;
            }
            else drapeau_plein_ecran=false;
        }

        //================ DESTRUCTEUR
        ~dksEcran()
        {
            if (drapeau_tempons_rendu)
            {
                glDeleteFramebuffers(sizeof(frame_buffers)/sizeof(GLuint),frame_buffers);
                glDeleteRenderbuffers(sizeof(render_buffers)/sizeof(GLuint),render_buffers);
                glDeleteTextures(sizeof(textures)/sizeof(GLuint),textures);
            }

            if (drapeau_plein_ecran) Restituer_ecran();
        }

        //*************************************************************************************************
        //   Gestion de la résolution
        //*************************************************************************************************
        bool est_en_plein_ecran()
        {
            return drapeau_plein_ecran;
        }

        bool determine_dimensions(uint32_t dx, uint32_t dy)
        {
            if ( (dx==resolution_x) && (dy==resolution_y) ) return true;
            resolution_x=dx;
            resolution_y=dy;
            if (drapeau_plein_ecran)
            {
                if (!change_resolution())
                {
                    erreur=DKSECRAN_ERREUR_MODE;
                    return false;
                }
            }
            if (drapeau_tempons_rendu) return maj_tempons_rendu();
            return true;
        }

        //Détermine la couleur des bords hors-champ des textures de rendu.
        void determine_couleur_bords(float r,float v,float b)
        {
            couleur_bords={r,v,b};
            for(uint32_t i=0;i<sizeof(textures)/sizeof(GLuint);i++)
            {
                glBindTexture(GL_TEXTURE_2D,textures[i]);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
            }
        }

        //*************************************************************************************************
        //   Gestion des tempons de rendu
        /// Appeler cette fonction après l'initialisation de la librairie GLEW, pour avoir accès aux extensions
        //*************************************************************************************************
        bool creation_tempons_rendus()
        {
            glGenFramebuffers(sizeof(frame_buffers)/sizeof(GLuint),frame_buffers);
            glGenRenderbuffers(sizeof(render_buffers)/sizeof(GLuint),render_buffers);
            glGenTextures(sizeof(textures)/sizeof(GLuint),textures);

            couleur_bords={0.,0.,0.};

            //----------------- Création des tempons de rendu de la même résolution que l'écran:
            //------------------Ecran 1:
                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_1]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,resolution_x,resolution_y);

                glBindTexture(GL_TEXTURE_2D,textures[TX_ECRAN_1]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //Il semble que ce paramètre active la texture.....
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,resolution_x,resolution_y,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                //Relie les tempons au frame buffer:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_ECRAN_1]);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_1]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_ECRAN_1],0);

                if(!validation_fb(FB_ECRAN_1)) return false;

            //------------------Ecran 2:
                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_2]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,resolution_x,resolution_y);

                glBindTexture(GL_TEXTURE_2D,textures[TX_ECRAN_2]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //Il semble que ce paramètre active la texture.....
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,resolution_x,resolution_y,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                //Relie les tempons au frame buffer:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_ECRAN_2]);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_2]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_ECRAN_2],0);

                if(!validation_fb(FB_ECRAN_2)) return false;


            //----------------- Tempon de rendu 256*256 n°1:
                glBindTexture(GL_TEXTURE_2D,textures[TX_256_1]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //GL_LINEAR est important pour ne pas avoir de gros carrés...
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_256_1]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,256,256);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_256_1]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_256_1],0);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_256_1]);

                if(!validation_fb(FB_256_1)) return false;


            //----------------- Tempon de rendu 256*256 n°2:
                glBindTexture(GL_TEXTURE_2D,textures[TX_256_2]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_256_2]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,256,256);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_256_2]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_256_2],0);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_256_2]);

                if(!validation_fb(FB_256_2)) return false;

            //----------------- Tempon de rendu 1/4 de la résolution de l'écran - n°1:
                largeur_texture_quart=resolution_x/4;
                hauteur_texture_quart=resolution_y/4;
                glBindTexture(GL_TEXTURE_2D,textures[TX_QUART_1]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //GL_LINEAR est important pour ne pas avoir de gros carrés...
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,largeur_texture_quart,hauteur_texture_quart,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_1]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,largeur_texture_quart,hauteur_texture_quart);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_QUART_1]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_QUART_1],0);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_1]);

                if(!validation_fb(FB_QUART_1)) return false;

            //----------------- Tempon de rendu 1/4 de la résolution de l'écran - n°2:
                glBindTexture(GL_TEXTURE_2D,textures[TX_QUART_2]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,largeur_texture_quart,hauteur_texture_quart,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_2]);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,largeur_texture_quart,hauteur_texture_quart);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_QUART_2]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[TX_QUART_2],0);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_2]);

                if(!validation_fb(FB_QUART_2)) return false;

            //----------------- Tempon de rendu des profondeurs de 512*512:
                glBindTexture(GL_TEXTURE_2D,textures[TX_OMBRES_512]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); ///Pour la texture des ombres, il FAUT lisser le rendu pour éviter les bugs d'affichage.
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
                glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,512,512,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_OMBRES_512]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,textures[TX_OMBRES_512],0);

                if(!validation_fb(FB_OMBRES_512)) return false;

            //----------------- Tempon de rendu des profondeurs de 1024*1024:
                glBindTexture(GL_TEXTURE_2D,textures[TX_OMBRES_1024]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
                glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,1024,1024,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_OMBRES_1024]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,textures[TX_OMBRES_1024],0);

                if(!validation_fb(FB_OMBRES_1024)) return false;

            //----------------- Tempon de rendu des profondeurs de 2048*2048:
                glBindTexture(GL_TEXTURE_2D,textures[TX_OMBRES_2048]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_bords);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
                glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,2048,2048,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[FB_OMBRES_2048]);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,textures[TX_OMBRES_2048],0);
                if(!validation_fb(FB_OMBRES_2048)) return false;

                //Restitution des tempons par défaut:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
                glBindRenderbuffer(GL_RENDERBUFFER,0);
                glBindTexture(GL_TEXTURE_2D,0);

            drapeau_tempons_rendu=true;

            //Erreur?:
            if(erreur_openGl("ERREUR dans dksEcran::creation_tempons_rendus() : "))
            {
                erreur=DKSECRAN_ERREUR_TEMPONS_RENDUS;
                return false;
            }
            return true;

        }

        //-------------------------------------------
        //      Vérifie la validité du frame buffer
        //-------------------------------------------
    private:
        bool validation_fb(GLuint id)
        {
            GLenum fb_status;
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffers[id]);
            fb_status=glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
            if(fb_status!=GL_FRAMEBUFFER_COMPLETE)
            {
                cout<<GL_FRAMEBUFFER_COMPLETE<<endl;
                cout<<"!!!! ERREUR dans dksEcran::creation_tempons_rendu() - Impossible de créer le frameBuffer: "<<id<<" - erreur: "<<fb_status<<endl;
                return false;
            }
            return true;
        }

        //--------------------------------------------------------------
        //      Redimensionne les tempons de rendu qui en ont besoin
        //--------------------------------------------------------------
    private:
        bool maj_tempons_rendu()
        {
            // Texture de la taille de l'écran:
            glBindTexture(GL_TEXTURE_2D,textures[TX_ECRAN_1]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,resolution_x,resolution_y,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.

            glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_1]);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,resolution_x,resolution_y);

            glBindTexture(GL_TEXTURE_2D,textures[TX_ECRAN_2]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,resolution_x,resolution_y,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.
            glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_ECRAN_2]);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,resolution_x,resolution_y);


            largeur_texture_quart=resolution_x/4;
            hauteur_texture_quart=resolution_y/4;

            // Les textures au 1/4 de l'écran
            glBindTexture(GL_TEXTURE_2D,textures[TX_QUART_1]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,largeur_texture_quart,hauteur_texture_quart,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.
            glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_1]);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,largeur_texture_quart,hauteur_texture_quart);

            glBindTexture(GL_TEXTURE_2D,textures[TX_QUART_2]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,largeur_texture_quart,hauteur_texture_quart,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL); //Alloue la mémoire automatiquement.
            glBindRenderbuffer(GL_RENDERBUFFER,render_buffers[RB_PROFONDEUR_QUART_2]);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,largeur_texture_quart,hauteur_texture_quart);


            glBindRenderbuffer(GL_RENDERBUFFER,0);
            //Erreur?:
            if(erreur_openGl("ERREUR dans dksEcran::maj_tempons_rendu() : "))
            {
                erreur=DKSECRAN_ERREUR_TEMPONS_RENDUS;
                return false;
            }
            return true;
        }

    public:
        ///L'identifiant en entrée est du type: DKSECRAN_TEMPON_xxxx
        GLuint renvoie_tempon_id_openGl(uint8_t tempon_id)
        {
            GLuint id;
            switch(tempon_id)
            {
                case DKSECRAN_TEMPON_256:
                    id=frame_buffers[FB_256_1];
                break;
                case DKSECRAN_TEMPON_ECRAN_1:
                    id=frame_buffers[FB_ECRAN_1];
                break;
                case DKSECRAN_TEMPON_ECRAN_2:
                    id=frame_buffers[FB_ECRAN_2];
                break;
                case DKSECRAN_TEMPON_DEFAUT:
                    id=0;
                break;
                case DKSECRAN_TEMPON_QUART:
                    id=frame_buffers[FB_QUART_1];
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_512:
                    id=frame_buffers[FB_OMBRES_512];
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_1024:
                    id=frame_buffers[FB_OMBRES_1024];
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_2048:
                    id=frame_buffers[FB_OMBRES_2048];
                default:
                    id=0;
                    break;
            }
            return id;
        }

        uint32_t renvoie_largeur_tempon(uint8_t id_tempon)
        {
            GLuint l;
            switch(id_tempon)
            {
                case DKSECRAN_TEMPON_256:
                    l=256;
                break;
                case DKSECRAN_TEMPON_ECRAN_1:
                case DKSECRAN_TEMPON_ECRAN_2:
                case DKSECRAN_TEMPON_DEFAUT:
                    l=resolution_x;
                break;
                case DKSECRAN_TEMPON_QUART:
                    l=largeur_texture_quart;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_512:
                    l=512;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_1024:
                    l=1024;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_2048:
                    l=2048;
                default:
                    l=0;
                    break;
            }
            return l;
        }

        uint32_t renvoie_hauteur_tempon(uint8_t id_tempon)
        {
            GLuint h;
            switch(id_tempon)
            {
                case DKSECRAN_TEMPON_256:
                    h=256;
                break;
                case DKSECRAN_TEMPON_ECRAN_1:
                case DKSECRAN_TEMPON_ECRAN_2:
                case DKSECRAN_TEMPON_DEFAUT:
                    h=resolution_y;
                break;
                case DKSECRAN_TEMPON_QUART:
                    h=hauteur_texture_quart;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_512:
                    h=512;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_1024:
                    h=1024;
                break;
                case DKSECRAN_TEMPON_PROFONDEUR_2048:
                    h=2048;
                default:
                    h=0;
                    break;
            }
            return h;
        }

        //*************************************************************************************************
        //    Configure la résolution écran
        //*************************************************************************************************
    private:
        bool initialise_ecran()
        {
            //Récupère le mode du système:
            EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &ModeOrig);
            if (!change_resolution()) return false;
            //Cache le curseur
            ShowCursor(false);
            drapeau_plein_ecran=true;
            return true;
        }

        bool change_resolution()
        {
            DEVMODE Mode;
            memcpy(&Mode, &ModeOrig, sizeof(DEVMODE));
            //Change la résolution
            Mode.dmPelsWidth  = resolution_x;
            Mode.dmPelsHeight = resolution_y;

            uint32_t message_mode=ChangeDisplaySettings(&Mode, CDS_FULLSCREEN);

            switch (message_mode)
            {
                case DISP_CHANGE_SUCCESSFUL:
                    break;
                case DISP_CHANGE_RESTART:
                    affiche_message("Veuillez redémarrer pour valider le nouveau mode graphique");
                    return false;
                    break;
                case DISP_CHANGE_BADFLAGS:
                    affiche_message("Erreur - Données ecran invalides");
                    return false;
                    break;
                case DISP_CHANGE_FAILED:
                    affiche_message("Erreur - Mode graphique invalide");
                    return false;
                    break;
                case DISP_CHANGE_BADMODE:
                    affiche_message("Erreur - Mode graphique non supporté");
                    return false;
                    break;
                case DISP_CHANGE_NOTUPDATED:
                    affiche_message("Erreur - Impossible d'écrire les données dans le registre...");
                    return false;
                    break;
            }
            return true;
        }

        void Restituer_ecran()
        {
        //Réinitialise l'écran
            ShowCursor(true);
            ChangeDisplaySettings(&ModeOrig, 0);
            drapeau_plein_ecran=false;
            return;
        }

        void affiche_message(char* texte)
        {
            MessageBox(0,texte,"Classe Ecran",MB_OK | MB_ICONEXCLAMATION);
        }

};

#endif // DKSECRAN_H_INCLUDED
