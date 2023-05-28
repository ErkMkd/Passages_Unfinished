#ifndef ORGANISATIONCOULEURS_H_INCLUDED
#define ORGANISATIONCOULEURS_H_INCLUDED

#include <vector>

//=========================================================
//              Classe OrganisationCouleurs
//=========================================================

class OrganisationCouleurs
{
    public:
        uint32_t tableau_couleurs[256];


    //---------------------------------------------
    //      Fonctions de convertion des valeurs:
    //---------------------------------------------
    public:
        static void convertion_2D_f_GLuint(uint32_t largeur, uint32_t hauteur,float *tempon, GLuint *tempon_uint32)
        {
            uint32_t x,y;
            for (y=0;y<hauteur;y++)
            {
                for (x=0;x<largeur;x++)
                {
                    GLuint valeur=tempon[x+y*largeur]*255.;
                    tempon_uint32[x+y*largeur]=(valeur<<16)|(valeur<<8)|valeur|0xff000000;
                }
            }
        }

        static void convertion_2D_d_GLuint(uint32_t largeur, uint32_t hauteur,double *tempon, GLuint *tempon_uint32)
        {
            uint32_t x,y;
            for (y=0;y<hauteur;y++)
            {
                for (x=0;x<largeur;x++)
                {
                    GLuint valeur=tempon[x+y*largeur]*255.;
                    tempon_uint32[x+y*largeur]=(valeur<<16)|(valeur<<8)|valeur|0xff000000;
                }
            }
        }

        static void convertion_2D_f_GLuint_reorganisation_couleurs(uint32_t largeur, uint32_t hauteur, OrganisationCouleurs* couleurs, float* tempon, GLuint *tempon_uint32)
        {
            uint32_t x,y;
            for (y=0;y<hauteur;y++)
            {
                for (x=0;x<largeur;x++)
                {
                    uint32_t indice=tempon[x+y*largeur]*255.;
                    tempon_uint32[x+y*largeur]=(GLuint)couleurs->tableau_couleurs[indice];
                }
            }
        }

        //---------------------------------------------
        //      Fonctions de transtypages
        //---------------------------------------------
        static uint32_t convertion_3f_uint32(float c[3])
        {
            return (uint32_t(c[0]*255.)<<16)|(uint32_t(c[1]*255.)<<8)|(uint32_t(c[2]*255.));
        }

        //---------------------------------------------
        //      Fonctions de dégradés
        //---------------------------------------------

        static uint32_t renvoie_couleur_interpolee_3f_uint32(float etapes[2][3],float t)
        {
            float r=etapes[0][0]+(etapes[1][0]-etapes[0][0])*t;
            float v=etapes[0][1]+(etapes[1][1]-etapes[0][1])*t;
            float b=etapes[0][2]+(etapes[1][2]-etapes[0][2])*t;
            return (uint32_t(r*255.)<<16)|(uint32_t(v*255.)<<8)|(uint32_t(b*255.));
        }

        //================================================================
        //                          CONSTRUCTEUR
        //================================================================

        OrganisationCouleurs()
        {
            rempli_couleurs(0);
        }

        void rempli_couleurs(uint32_t couleur)
        {
            for (uint16_t i=0; i<256;i++)
            {
                tableau_couleurs[i]=couleur;
            }
        }

        void determine_couleur(uint8_t indice, uint32_t couleur)
        {
            tableau_couleurs[indice]=couleur;
        }

        //-----------------------------------
        //      Génère un dégradé
        //-----------------------------------
        void genere_degrade(uint8_t indice_depart, uint32_t couleur_depart, uint8_t indice_fin, uint32_t couleur_fin)
        {
            if (indice_depart>indice_fin)
            {
                uint8_t temp=indice_fin;
                indice_fin=indice_depart;
                indice_depart=temp;
                temp=couleur_depart;
                couleur_depart=couleur_fin;
                couleur_fin=temp;
            }

            uint8_t nbr_etapes=indice_fin-indice_depart;

            float r0=couleur_depart&0xff;
            float v0=(couleur_depart>>8)&0xff;
            float b0=(couleur_depart>>16)&0xff;
            float a0=(couleur_depart>>24)&0xff;

            float r1=couleur_fin&0xff;
            float v1=(couleur_fin>>8)&0xff;
            float b1=(couleur_fin>>16)&0xff;
            float a1=(couleur_fin>>24)&0xff;

            float rp=(r1-r0)/(float)nbr_etapes;
            float vp=(v1-v0)/(float)nbr_etapes;
            float bp=(b1-b0)/(float)nbr_etapes;
            float ap=(a1-a0)/(float)nbr_etapes;

            for (uint16_t i=indice_depart;i<indice_fin;i++)
            {
                tableau_couleurs[i]=uint32_t(r0)|(uint32_t(v0)<<8)|(uint32_t(b0)<<16)|(uint32_t(a0)<<24);
                r0+=rp;v0+=vp;b0+=bp;a0+=ap;
            }

            tableau_couleurs[indice_fin]=couleur_fin;

        }

        void affiche_debogue()
        {
            for (uint16_t i=0 ;i<256;i++)
            {
                cout<<"indice:"<<i<<" - "<<hex<<tableau_couleurs[i]<<endl;
            }
        }

};




#endif // ORGANISATIONCOULEURS_H_INCLUDED
