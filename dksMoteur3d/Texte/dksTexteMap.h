#ifndef DKSTEXTEMAP_H_INCLUDED
#define DKSTEXTEMAP_H_INCLUDED


#include "../../chargeurs_images/Image.h"
#include "../../dksMoteur2d/dksEcran.h"
#include "../../dksMoteur3d/Elements_Fondamentaux/dksConteneur.h"
#include "../../gestion_fichiers/gestion_fichiers.h"

#define DKSTEXTE_OK 0
#define DKSTEXTE_ERREUR_IMAGE 1
#define DKSTEXTE_ERREUR_MEMOIRE 2
#define DKSTEXTE_ERREUR_FICHIER 3

//=================================================================================
//                      Affichage de texte via une texture
//      L'image des police doit être un carré d'une puissance de 2 de cotés.
//      Les lettres tiennent dans des carré de 1/16 de côté, et sont rangées
//      dans l'ordre des codes ASCII, de gauche à droite et de bas en haut.
//
//      Les quadrilatères des lettres sont définis dans le sens direct.
//=================================================================================

#define DKSTEXTMAP_RENDU_2D 1
#define DKSTEXTMAP_RENDU_3D 2

class dksTextMap : public dksConteneur
{
    public:
        uint16_t erreur;
        float curseur_x;
        float curseur_y;
        uint8_t mode_rendu;
        uint32_t couleur;
        float alpha;
        float couleur_fond[3];

        GLint type_grossissement; //GL_LINEAR ou GL_NEAREST
        GLint type_diminution;

        //Données utiles pour le scroll-texte vertical:
        uint16_t pointeur_ligne;    //Numéro de la première ligne à afficher
        uint16_t nbr_lignes_max;    //Le nombre de lignes maximum à afficher

        float scroll_ymin;  //Entre 0 et 1
        float scroll_ymax;
        float vitesse_verticale;    //Le temps que met une ligne à traverser l'écran



    private:
        char* texte;
        Image* police;
        uint32_t largeur_caractere; //Largeur en pixels des carrés contenants les caractères.
        float base;
        float coordonnees_lettres[256*4*2];
        float normale[3];
        float espaces_lettres[256];
        float rectangle[4*3];
        float echelle;  //Taille des caractère par rapport à la largeur de l'écran=1
        float espace_lettres;
        float espace_lignes;
        float espace;

        uint32_t taille_texte;  //Nombre de caractères
        uint16_t nbr_lignes;    //Le nombre de lignes du texte (chaque ligne est séparée par "\n", automatiquement transcrite en code 10 lors de la compilation)
        uint32_t* indices_lignes;    //La tableau contenant les indices de départ des lignes dans le texte
        uint16_t* tailles_lignes;    //La tableau contenant les tailles des lignes

        dksEcran* ecran;

    //========================================================
    //              Constructeur
    //========================================================
    public:
    dksTextMap(char* fichier_image,dksEcran* p_ecran,char* p_nom="Texte",float p_base=0.,float p_echelle=0.1,float p_espace_lettres=0.15,float p_espace=0.5,float p_espace_lignes=0.15)
                :dksConteneur(p_nom)
    {
        erreur=DKSTEXTE_OK;
        ecran=p_ecran;
        mode_rendu=DKSTEXTMAP_RENDU_3D;
        texte=NULL;
        couleur=0xffffff;
        alpha=1.;
        couleur_fond={0.,0.,0.};
        type_grossissement=GL_LINEAR;
        type_diminution=GL_LINEAR;
        nbr_lignes=0;
        pointeur_ligne=0;
        nbr_lignes_max=1;
        taille_texte=0;
        tailles_lignes=NULL;
        indices_lignes=NULL;

        vitesse_verticale=5.;
        scroll_ymin=0.;
        scroll_ymax=1.;

        police=new Image(fichier_image);
        if (police==NULL || police->erreur!=IMAGE_OK) { erreur=DKSTEXTE_ERREUR_IMAGE; return; }
        if (!police->genere_texture_openGL(true)){ erreur=DKSTEXTE_ERREUR_IMAGE; return; };

        largeur_caractere=police->largeur/16;
        echelle=1.;

        initialise_coordonnees_lettres();
        initialise_espaces();
        curseur_x=0.;
        curseur_y=0.;

        determine_echelle(p_echelle);
        determine_espace_lettres(p_espace_lettres);
        determine_espace_lignes(p_espace_lignes);
        determine_espace(p_espace);
        base=p_base;
    }

    ~dksTextMap()
    {
        if (police!=NULL) delete police;
        if (texte!=NULL) free(texte);
        if (tailles_lignes!=NULL) free(tailles_lignes);
        if (indices_lignes!=NULL) free(indices_lignes);
    }

    //====================================================================
    //              Initialisations
    //====================================================================
    void initialise_coordonnees_lettres()
    {
        uint16_t x,y;

        //Coordonnées du quadrilatère:
        rectangle={0,0.,0., 1.,0.,0., 1.,1.,0., 0.,1.,0.};

        normale={0.,0.,1.};

        //Coordonnées de texture:
        for(y=0;y<16;y++)
        {
            for (x=0;x<16;x++)
            {
                coordonnees_lettres[(x+y*16)*8]=x/16.;
                coordonnees_lettres[(x+y*16)*8+1]=y/16.;
                coordonnees_lettres[(x+y*16)*8+2]=(x+1)/16.;
                coordonnees_lettres[(x+y*16)*8+3]=y/16.;
                coordonnees_lettres[(x+y*16)*8+4]=(x+1)/16.;
                coordonnees_lettres[(x+y*16)*8+5]=(y+1)/16.;
                coordonnees_lettres[(x+y*16)*8+6]=x/16.;
                coordonnees_lettres[(x+y*16)*8+7]=(y+1)/16.;
            }
        }
    }

    void initialise_espaces()
    {
        uint8_t* pixels=police->image;
        uint32_t x,y,px,py,pos_image_x,pos_image_y;
        uint32_t couleur;

        for(y=0;y<16;y++)
        {
            for (x=0;x<16;x++)
            {
                pos_image_x=x*largeur_caractere;
                pos_image_y=y*largeur_caractere;
                espaces_lettres[x+y*16]=1.;
                for (px=pos_image_x+largeur_caractere-1;px>pos_image_x;px--)
                {
                    bool colonne_vide=true;
                    for (py=pos_image_y;py<pos_image_y+largeur_caractere;py++)
                    {

                        couleur=*((uint32_t*)(pixels+(px+py*largeur_caractere*16)*4));
                        //cout<<hex<<couleur<<"  ";
                        if (couleur!=0x00000000) {colonne_vide=false; break;}
                    }
                    //cout<<endl;
                    if (colonne_vide)espaces_lettres[x+y*16]-=1./largeur_caractere;

                }
            }
        }
    }

    //-------------------------------
    //Détermine le texte à afficher:
    //-------------------------------

    //--------- Détermine le texte à partir d'un fichier:
    bool determine_texte_fichier(char* p_nom_fichier)
    {
        if (texte!=NULL)free(texte);
        if(tailles_lignes!=NULL) free(tailles_lignes);
        if(indices_lignes!=NULL) free(indices_lignes);

        taille_texte=Fichiers::renvoie_taille_fichier(p_nom_fichier);;
        if (taille_texte==0)
        {
            cout<<"ERREUR dans dksTextMap::determine_texte_fichier() - Impossible de déterminer la taille du fichier "<<p_nom_fichier<<endl;
            erreur=DKSTEXTE_ERREUR_FICHIER;
            return false;
        }

        texte=Fichiers::charge_donnees_binaires_allocation(p_nom_fichier);
        if (texte==NULL)
        {
            cout<<"ERREUR dans dksTextMap::determine_texte_fichier() - Impossible de charger le fichier: "<<p_nom_fichier<<endl;
            erreur=DKSTEXTE_ERREUR_FICHIER;
            return false;
        }

        texte[taille_texte]=0; //Au cas où...
        return initialise_texte();
    }

    //------------- Détermine le texte à partir d'une chaine:
    bool determine_texte(char* p_texte)
    {
        if (texte!=NULL)free(texte);
        if(tailles_lignes!=NULL) free(tailles_lignes);
        if(indices_lignes!=NULL) free(indices_lignes);
        taille_texte=strlen(p_texte);
        texte=(char*)malloc(taille_texte+1);
        if (texte==NULL)
        {
            cout<<"ERREUR dans dksTextMap::determine_texte() - Impossible d'allouer la chaîne de caractères: "<<p_texte<<endl;
            erreur=DKSTEXTE_ERREUR_MEMOIRE;
            return false;
        }
        memcpy(texte,p_texte,taille_texte);
        texte[taille_texte]=0;

        /*
        cout<<"Codes texte:"<<endl;
        for(uint32_t i=0;i<taille_texte;i++)
        {
            cout<<uint16_t(texte[i])<<" ";
        }
        cout<<endl;
        */
        //------- Compte le nombre de lignes:
        return initialise_texte();
    }

    //------------------ Initialise les données pour l'affichage:
    private:
    bool initialise_texte()
    {

        uint32_t i,j;
        nbr_lignes=1;
        pointeur_ligne=0;
        for(i=0;i<taille_texte;i++) if(texte[i]==10)nbr_lignes++;
        nbr_lignes_max=nbr_lignes;          ///Valeur par défaut, à changer après si besoin...

        //---------- Allocation du tableau des tailles et indices de lignes:
        tailles_lignes=(uint16_t*)malloc(nbr_lignes*sizeof(uint16_t));
        indices_lignes=(uint32_t*)malloc(nbr_lignes*sizeof(uint32_t));
        if (tailles_lignes==NULL || indices_lignes==NULL)
        {
            cout<<"ERREUR dans dksTextMap::initialise_texte() - Impossible d'allouer les tables d'informations des lignes (tailles,indices)"<<endl;
            erreur=DKSTEXTE_ERREUR_MEMOIRE;
            return false;
        }
        //--------- Calcul les tailles des lignes:
        j=0;
        tailles_lignes[j]=0;
        indices_lignes[j]=0;
        for(i=0;i<taille_texte;i++)
        {
            tailles_lignes[j]++;    //!!!le retour est compté !!!
            if(texte[i]==10)
            {
                j++;
                tailles_lignes[j]=0;
                indices_lignes[j]=i+1;
            }

        }
        return true;
    }

    //-------------------------------
    //Détermine la position du curseur
    //-------------------------------
    public:
    void determine_position_curseur(float px,float py)
    {
        curseur_x=px;
        curseur_y=py;
    }

    //-----------------------------------------------------
    //      Espacement entre les lettres,
    //      1.=largeur du rectangle contenant les lettres.
    //-----------------------------------------------------
    void determine_espace_lettres(float p_espace)
    {
        espace_lettres=p_espace;
    }

    //-----------------------------------------------------
    //      Espacement entre les lignes,
    //      1.=hauteur du rectangle contenant les lettres.
    //-----------------------------------------------------
    void determine_espace_lignes(float p_espace)
    {
        espace_lignes=p_espace;
    }

    //-----------------------------------------------------
    //     Taille du caractère ESPACE
    //     1.=largeur du carré contenant les lettres.
    //-----------------------------------------------------
    void determine_espace(float p_espace)
    {
        espace=p_espace;
    }

    //=======================================================================
    //                          Determine/Renvoie
    //=======================================================================
    void determine_echelle(float e)
    {
        echelle=e;
    }
    //=======================================================================
    //                          Affichages
    //=======================================================================

    virtual void affiche(dksOeil* oeil)
    {
        float r=float((couleur>>16)&0xff)/255.;
        float v=float((couleur>>8)&0xff)/255.;
        float b=float(couleur&0xff)/255.;
        glColor4f(r,v,b,alpha);
        desactive_textures();
        glUseProgram(0);

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,police->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_fond);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,type_diminution);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        GLboolean color_material_mem=glIsEnabled(GL_COLOR_MATERIAL);
        glDisable(GL_COLOR_MATERIAL);

        if (mode_rendu==DKSTEXTMAP_RENDU_2D) affiche_2d();
        else if (mode_rendu==DKSTEXTMAP_RENDU_3D) affiche_3d();

        if (color_material_mem) glEnable(GL_COLOR_MATERIAL);
    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {

    }

    virtual void affiche_profondeur(dksOeil* oeil)
    {

    }

    private:

    //---------------------------------------------------
    //  Affiche les lettres de face, sans perspective
    //---------------------------------------------------

    void affiche_2d()
    {
        uint32_t i,c;
        float rapport_ecran=(float)ecran->resolution_y/ecran->resolution_x;
        glFrontFace(GL_CCW);

        GLboolean eclairage_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.,1.,0.,rapport_ecran);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(curseur_x,curseur_y*rapport_ecran,0.);

        affiche_texte(texte);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        if(eclairage_mem)glEnable(GL_LIGHTING);
    }

    //--------------------------------------------------------------------------
    //  Affiche les lettres dans l'espace.
    //  Le texte est géré comme un affichage transparent (Z-buffer désactivé)
    //  donc pour les recouvrements il faut faire attention à l'ordre d'affichage.
    //--------------------------------------------------------------------------
    void affiche_3d()
    {
        uint32_t i,c;
        float rapport_ecran=(float)ecran->resolution_y/ecran->resolution_x;
        glFrontFace(GL_CCW);
        GLboolean eclairages_mem=glIsEnabled(GL_LIGHTING);
        GLboolean mask_mem;
        glGetBooleanv(GL_DEPTH_WRITEMASK,&mask_mem);
        glDisable(GL_LIGHTING);

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixd(matrice.matrice);
        affiche_texte(texte);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDepthMask(mask_mem);
        if (eclairages_mem) glEnable(GL_LIGHTING);
    }


    //-------------------------------------
    //  Affichage de base:
    //-------------------------------------

    private:
    void affiche_texte(char* texte,bool centrage=true)
    {
        uint32_t t,i,c,j;
        float largeur_texte;
        float position_depart_ligne;

        glPushMatrix();
        glScalef(echelle,echelle,1.);
        uint16_t ligne_actuelle=pointeur_ligne;

        //Lecture des lignes:
        for(t=0;t<nbr_lignes_max;t++)
        {
            if(ligne_actuelle>=nbr_lignes) break;
            largeur_texte=0.;
            position_depart_ligne=0.;
            j=indices_lignes[ligne_actuelle];
            //Centrage de la ligne:
            if (centrage)
            {
                for (i=0;i<tailles_lignes[ligne_actuelle];i++)
                {
                    c=uint32_t(texte[j+i]);
                    if (c==' ') largeur_texte+=espace;
                    else largeur_texte+=espaces_lettres[c]+espace_lettres;
                }
                position_depart_ligne=-largeur_texte/2.;
            }

            //Affiche la phrase:
            glPushMatrix();
            glTranslatef(position_depart_ligne,-base,0.);
            for (i=0;i<tailles_lignes[ligne_actuelle];i++)
            {
                c=uint32_t(texte[j+i]);
                if (c==' ')
                {
                    glTranslatef(espace,0.,0.);
                }
                else
                {
                    glBegin(GL_QUADS);
                    glNormal3fv(normale);
                    //cout<<"Point "<<i<<" "<<rectangle[0]<<" "<<rectangle[1]<<" "<<rectangle[2]<<endl;
                    glTexCoord2fv(&coordonnees_lettres[c*8+6]);
                    glVertex3fv(&rectangle[0]);
                    glTexCoord2fv(&coordonnees_lettres[c*8+4]);
                    glVertex3fv(&rectangle[3]);
                    glTexCoord2fv(&coordonnees_lettres[c*8+2]);
                    glVertex3fv(&rectangle[6]);
                    glTexCoord2fv(&coordonnees_lettres[c*8+0]);
                    glVertex3fv(&rectangle[9]);
                    glEnd();
                    glTranslatef(espaces_lettres[c]+espace_lettres,0.,0.);
                }
            }
            glPopMatrix();
            glTranslatef(0.,-1.-espace_lignes,0.);   //Ligne suivante
            ligne_actuelle++;
        }

        glPopMatrix();
    }

    //-------------------------------------------------------
    //      Gestion du scrolling vertical
    //-------------------------------------------------------
    public:
    void maj_scrolling_vertial()
    {
        float rapport_ecran=(float)ecran->resolution_y/ecran->resolution_x;
        curseur_y+=1./vitesse_verticale*delta_t;
        if(vitesse_verticale>0.)
        {
            if(curseur_y>scroll_ymax)
            {
                pointeur_ligne++;
                curseur_y-=(echelle*(1.+espace_lignes))/rapport_ecran;
            }
            if (pointeur_ligne>=nbr_lignes)
            {
                curseur_y=scroll_ymin;
                pointeur_ligne=0;
            }
        }

        else
        {
            if(curseur_y<scroll_ymin)
            {
                pointeur_ligne--;
                curseur_y+=(echelle*(1.+espace_lignes))/rapport_ecran;
            }
        }


    }
};

#endif // DKSTEXTEMAP_H_INCLUDED
