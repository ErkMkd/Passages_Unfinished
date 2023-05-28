#ifndef DKSPOLICE3D_H_INCLUDED
#define DKSPOLICE3D_H_INCLUDED

/*

        Classe utilisée par dksTexte3d - stockage et gestion des caractères matérialisés par des volumes
        La table des caractère doit respecter l'ordre des codes ascii, moins 33 (premier caractère "!"=33)

*/

//================================================================================================================
//                          Classe
//================================================================================================================

#define DKSPOLICE3D_OK 0

class dksPolice3d
{
    private:

    dksVolume** caracteres;     //Mettre le pointeur à NULL où les caractères ne sont pas définis.
    uint16_t    nbrCaracteres;

    float facteur_taille;       //Ces facteurs agissent sur les sommets des caractères
    float facteur_epaisseur;

    public:
    float espace_lettres;   //L'espace à ajouter entre chaque lettre
    double centrage_lettres[3*128]; //Les coordonnées de décallage pour le positionnement correct des lettres
    uint16_t erreur;
    //=====================================================
    //              Constructeur
    //=====================================================
    dksPolice3d(dksVolume** p_caracteres, uint16_t p_nbrCaracteres, float p_taille=1., float p_epaisseur=1.)
    {
        erreur=DKSPOLICE3D_OK;
        facteur_epaisseur=1.;
        facteur_taille=1.;
        espace_lettres=0.25;
        caracteres=p_caracteres;
        nbrCaracteres=p_nbrCaracteres;
        initialise_centrages_lettres();
        determine_facteurs_dimensions(p_taille,p_epaisseur);
    }

    ~dksPolice3d()
    {

    }

    //==========================================
    //          Initialisations
    //==========================================
    void initialise_centrages_lettres()
    {
        uint16_t i;
        for(i=0;i<nbrCaracteres;i++)
        {
            centrage_lettres[i*3]=-caracteres[i]->boite_collision->xmin;
            centrage_lettres[i*3+1]=caracteres[i]->y;
            centrage_lettres[i*3+2]=caracteres[i]->z;
        }
    }

    void determine_facteurs_dimensions(float p_taille, float p_epaisseur)
    {
        double t=double(p_taille/facteur_taille);
        double e=double(p_epaisseur/facteur_epaisseur);
        uint16_t i;
        uint32_t j;
        for (i=0;i<nbrCaracteres;i++)
        {
            dksVolume* caractere=caracteres[i];
            if (caractere!=NULL)
            {
                caractere->determine_position(caractere->x*t,caractere->y*t,0.);
                double* sommets=caractere->sommets;
                for(j=0;j<caractere->nbrSommets;j++)
                {
                    sommets[3*j]*=t;
                    sommets[3*j+1]*=t;
                    sommets[3*j+2]*=e;
                }
                caractere->maj_boite_collision();
                centrage_lettres[3*i]*=t;
                centrage_lettres[3*i+1]*=t;
                centrage_lettres[3*i+2]*=e;
            }
        }
        facteur_taille=p_taille;
        facteur_epaisseur=p_epaisseur;
        espace_lettres*=t;
    }

    //La valeur en entrée est en proportion d'une échelle 1.
    void determine_espace_lettres(float p_espace)
    {
        espace_lettres=p_espace*facteur_taille;
    }

    float renvoie_facteur_taille()
    {
        return facteur_taille;
    }

    void determine_mode_rendu(uint8_t mode)
    {
        for (uint16_t i=0;i<nbrCaracteres;i++) if (caracteres[i]!=NULL) caracteres[i]->determine_mode_rendu(mode);
    }

    //======================================
    //      Gestion des caractères
    //======================================
    uint16_t renvoie_id_caractere(char code_ascii)
    {
        uint16_t code16=uint16_t(code_ascii);
        if (code16<33 || (code16-33)>nbrCaracteres) return 0xffff;
        return code16-33;
    }

    dksVolume* renvoie_caractere(uint16_t id_caractere)
    {
        return caracteres[id_caractere];
    }

    double* renvoie_centrage_caractere(uint16_t id_caractere)
    {
        return &centrage_lettres[id_caractere*3];
    }

};

#endif // DKSPOLICE3D_H_INCLUDED
