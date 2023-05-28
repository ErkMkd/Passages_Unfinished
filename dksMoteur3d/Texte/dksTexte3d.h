#ifndef DKSTEXTE3D_H_INCLUDED
#define DKSTEXTE3D_H_INCLUDED

/*

        Classe de gestion des textes en caractères 3d

*/

#include "../Particules/dksParticulesVolumes.h"
#include "dksPolice3d.h"

//===============================================================================================================================
//                                                      Classe
//===============================================================================================================================

#define DKSTEXTE3D_OK 0
#define DKSTEXTE3D_ERREUR_MEMOIRE 1

class dksTexte3d : public dksParticulesVolumes
{
    private:
        char* texte;    //Le texte à afficher, chaîne terminée par un 0
        dksPolice3d* police;
        double espace;
        double curseur[3];

    public:
        double vecteur_curseur[3];
        uint16_t erreur;
        double depart_curseur[3];
        bool drapeau_centrage;

    //==========================================================================
    //                          Constructeur
    //==========================================================================
    dksTexte3d(dksPolice3d* p_police, char* p_texte=NULL,char* p_nom="Texte3d",double px=0.,double py=0.,double pz=0.)
                :dksParticulesVolumes(p_nom,px,py,pz)
    {
        erreur=DKSTEXTE3D_OK;
        police=p_police;
        texte=NULL;
        if (!determine_texte(p_texte)) return;
        espace=double(police->renvoie_facteur_taille());
        vecteur_curseur={-1.,0.,0.};
        depart_curseur={0.,0.,0.};
        determine_centrage(true);

    }

    ~dksTexte3d()
    {
        if(texte!=NULL) free(texte);
    }

    //=========================================================================
    //          Gestion texte
    //=========================================================================

    //-------------------------------
    //Détermine le texte à afficher:
    //-------------------------------
    bool determine_texte(char* p_texte)
    {
        if (texte!=NULL)free(texte);
        uint32_t taille=strlen(p_texte);
        texte=(char*)malloc(taille+1);
        if (texte==NULL)
        {
            cout<<"ERREUR dans dksText3d::determine_texte() - Impossible d'allouer la chaîne de caractères: "<<p_texte<<endl;
            erreur=DKSTEXTE3D_ERREUR_MEMOIRE;
            return false;
        }
        memcpy(texte,p_texte,taille);
        texte[taille]=0;
        if (!determine_nombre_de_points(taille)) return false;
        return true;
    }

    char* renvoie_texte()
    {
        return texte;
    }

    void determine_centrage(bool d)
    {
        drapeau_centrage=d;
        if(d)calcul_centrage();
        else depart_curseur={0.,0.,0.};

    }

    private:
    void calcul_centrage()
    {
        curseur={0.,0.,0.};
        double pas;

        for (uint32_t i=0;i<nbrPoints;i++)
        {
            if(texte[i]==32)
            {
                //cout<<" ";
                pas=espace+double(police->espace_lettres);
                curseur[0]+=vecteur_curseur[0]*pas;
                curseur[1]+=vecteur_curseur[1]*pas;
                curseur[2]+=vecteur_curseur[2]*pas;
            }
            else
            {
                uint16_t lettre_id=police->renvoie_id_caractere(texte[i]);

                if(lettre_id!=0xffff)
                {
                    dksVolume* lettre=police->renvoie_caractere(lettre_id);
                    //double* centrage=police->renvoie_centrage_caractere(lettre_id);
                    double largeur_lettre=lettre->boite_collision->xmax-lettre->boite_collision->xmin;
                    pas=largeur_lettre+double(police->espace_lettres);
                    curseur[0]+=vecteur_curseur[0]*pas;
                    curseur[1]+=vecteur_curseur[1]*pas;
                    curseur[2]+=vecteur_curseur[2]*pas;
                }
            }
        }

        depart_curseur={-curseur[0]/2.,-curseur[1]/2.,-curseur[2]/2.};
    }

    //-------------------------------
    //Détermine la position du curseur
    //-------------------------------
    public:
    void determine_position_curseur(double px,double py, double pz)
    {
        depart_curseur={px,py,pz};
    }


    //----------------------------------------------------------------------------------------------
    //     Taille du caractère ESPACE, proportionnel à un facteur de taille des caractère=1.
    //----------------------------------------------------------------------------------------------
    void determine_espace(double p_espace)
    {
        espace=p_espace*double(police->renvoie_facteur_taille());
    }


    //--------------------------------------------
    //      Gestion des mouvements des lettres
    //--------------------------------------------

    void maj_texte()
    {
        uint32_t i;

        for(i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            particule->maj_RTL(float(delta_t));
            //particule->maj_echelle();
            //if (particule->volume!=NULL)particule->volume->determine_position(&points[i*3]);
        }
    }
    //=========================================================================
    //          Affichage
    //=========================================================================


    virtual void affiche(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
        uint32_t i;
        curseur={depart_curseur[0],depart_curseur[1],depart_curseur[2]};
        double pas;
        for (i=0;i<nbrPoints;i++)
        {
            Particule_Volume* particule=(Particule_Volume*) particules[i];
            if(texte[i]==32)
            {
                //cout<<" ";
                pas=espace+double(police->espace_lettres);
                curseur[0]+=vecteur_curseur[0]*pas;
                curseur[1]+=vecteur_curseur[1]*pas;
                curseur[2]+=vecteur_curseur[2]*pas;
            }
            else
            {
                uint16_t lettre_id=police->renvoie_id_caractere(texte[i]);

                if(lettre_id!=0xffff)
                {
                    dksVolume* lettre=police->renvoie_caractere(lettre_id);
                    double* centrage=police->renvoie_centrage_caractere(lettre_id);
                    //cout<<lettre->nom;
                    lettre->determine_position(curseur[0]-centrage[0],curseur[1]+centrage[1],curseur[2]+centrage[2]);
                    lettre->determine_RTL(particule->roulis,particule->tangage,particule->lacet);
                    glPushMatrix();

                    //dksRepere::translation_repere(repere_absolu,lettre,lettre->repere_absolu);
                    lettre->calcul_matrice_relative();

                    glPopMatrix();
                    glPushMatrix();
                    glMultMatrixd(lettre->matrice.matrice);
                    lettre->affiche(oeil);
                    glPopMatrix();
                    double largeur_lettre=lettre->boite_collision->xmax-lettre->boite_collision->xmin;
                    pas=largeur_lettre+double(police->espace_lettres);
                    curseur[0]+=vecteur_curseur[0]*pas;
                    curseur[1]+=vecteur_curseur[1]*pas;
                    curseur[2]+=vecteur_curseur[2]*pas;
                }
            }
        }
        //cout<<endl;
    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {
        if(!drapeau_affiche) return;
    }

};

#endif // DKSTEXTE3D_H_INCLUDED
