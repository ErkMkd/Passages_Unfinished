#ifndef DKSPOSTRENDU_H_INCLUDED
#define DKSPOSTRENDU_H_INCLUDED

/*

                    Rendus effectués après le rendu de scènes 3d ou 2d. (flou, profondeur de champ, fondus, halos....

*/

#include "dksEcran.h"
#include "dksRendu.h"
#include <vector>

//==========================================================================================================
//          Elements contenus dans les listes d'affichage
//  Ces objets sont renvoyés par la fonction "ajoute_rendu3d()"
//==========================================================================================================

//Les modes d'affichage:

class Element_PostRendu
{
    public:
    int32_t indice_liste_affichage; //L'indice dans la liste d'affichage
    bool drapeau_affiche;
    dksRendu* rendu;    //L'objet à afficher
    uint8_t mode_affichage; //Voir les modes de dksRendu3d
    uint8_t tempon_id;      //L'identifiant du tempon de sortie
    vector<Element_PostRendu*>* liste_affichage;

    Element_PostRendu(dksRendu* p_rendu, uint8_t p_tempon_id, uint8_t p_mode_affichage,vector<Element_PostRendu*>* p_liste)
    {
        rendu=p_rendu;
        tempon_id=p_tempon_id;
        mode_affichage=p_mode_affichage;
        liste_affichage=p_liste;
        indice_liste_affichage=liste_affichage->size();
        drapeau_affiche=true;
    }


};
//==========================================================================================================
//                      Classe de post-rendu
//==========================================================================================================

class dksPostRendu
{
    public:
    dksEcran* ecran;

    //Les listes d'affichage:
    vector<Element_PostRendu*> liste_affichage_ecran_1;
    vector<Element_PostRendu*> liste_affichage_ecran_2;
    vector<Element_PostRendu*> liste_affichage_quart;
    vector<Element_PostRendu*> liste_affichage_256;

    vector<Element_PostRendu*> liste_affichage_profondeur_512;
    vector<Element_PostRendu*> liste_affichage_profondeur_1024;
    vector<Element_PostRendu*> liste_affichage_profondeur_2048;

    /*
    vector<Element_PostRendu*> liste_affichage_ecran_couleur;
    vector<Element_PostRendu*> liste_affichage_ecran_surfaces_luminescentes;
    vector<Element_PostRendu*> liste_affichage_ecran_profondeur;

    vector<Element_PostRendu*> liste_affichage_quart_couleur;
    vector<Element_PostRendu*> liste_affichage_quart_surfaces_luminescentes;
    vector<Element_PostRendu*> liste_affichage_quart_profondeur;

    vector<Element_PostRendu*> liste_affichage_256_couleur;
    vector<Element_PostRendu*> liste_affichage_256_surfaces_luminescentes;
    vector<Element_PostRendu*> liste_affichage_256_profondeur;

    vector<Element_PostRendu*> liste_affichage_profondeur_512;
    vector<Element_PostRendu*> liste_affichage_profondeur_1024;
    vector<Element_PostRendu*> liste_affichage_profondeur_2048;
    */

    public:
    //--------- Paramètres du rendu des tempons:
    float alpha_tempon_ecran;
    float alpha_tempon_quart;
    float alpha_tempon_256;

    //---------- Paramètres du floutage:
    float largeur_flou;
    float hauteur_flou;
    float saturation_horizontale_flou;
    float saturation_verticale_flou;

    //---------- Paramètres du masquage de 2 textures:
    float couleur_masque[3];

    //======================================
    //          Constructeur
    //======================================
    dksPostRendu(dksEcran* p_ecran)
    {
        ecran=p_ecran;

        alpha_tempon_ecran=1.;
        alpha_tempon_quart=1.;
        alpha_tempon_256=1.;

        largeur_flou=10.;
        hauteur_flou=10.;
        saturation_horizontale_flou=2.;
        saturation_verticale_flou=2.;

        couleur_masque={0.,0.,0.};
    }

    ~dksPostRendu()
    {
        uint32_t i;

        //Vidange des listes d'affichage:
        for(i=0;i<liste_affichage_ecran_1.size();i++) delete liste_affichage_ecran_1[i];
        for(i=0;i<liste_affichage_ecran_2.size();i++) delete liste_affichage_ecran_2[i];

        for(i=0;i<liste_affichage_quart.size();i++) delete liste_affichage_quart[i];

        for(i=0;i<liste_affichage_256.size();i++) delete liste_affichage_256[i];

        for(i=0;i<liste_affichage_profondeur_512.size();i++) delete liste_affichage_profondeur_512[i];
        for(i=0;i<liste_affichage_profondeur_1024.size();i++) delete liste_affichage_profondeur_1024[i];
        for(i=0;i<liste_affichage_profondeur_2048.size();i++) delete liste_affichage_profondeur_2048[i];
    }

    //=================================================================
    //              Gestion des listes d'affichage:
    //=================================================================

    //--------------------------------------------------
    //  Renvoie la liste correspondant aux paramètres
    //--------------------------------------------------
    vector<Element_PostRendu*>* renvoie_liste_affichage(uint8_t tempon_id)
    {
        switch (tempon_id)
        {
            case DKSECRAN_TEMPON_ECRAN_1:
                return &liste_affichage_ecran_1;
            break;
            case DKSECRAN_TEMPON_ECRAN_2:
                return &liste_affichage_ecran_2;
            break;
            case DKSECRAN_TEMPON_QUART:
                return &liste_affichage_quart;
            break;
            case DKSECRAN_TEMPON_256:
                return &liste_affichage_256;
            break;
            case DKSECRAN_TEMPON_PROFONDEUR_512:
                return &liste_affichage_profondeur_512;
            break;
            case DKSECRAN_TEMPON_PROFONDEUR_1024:
                return &liste_affichage_profondeur_1024;
            break;
            case DKSECRAN_TEMPON_PROFONDEUR_2048:
                return &liste_affichage_profondeur_2048;
            break;
            default:
                return NULL;
            break;
        }
    }

    //--------------------------------------------------------------
    //  Ajoute/supprime un élément de rendu dans la liste correspondante
    //--------------------------------------------------------------
    Element_PostRendu* ajoute_rendu(dksRendu* p_rendu,uint8_t tempon_id, uint8_t mode_affichage)
    {
        vector<Element_PostRendu*>* liste_affichage=renvoie_liste_affichage(tempon_id);
        Element_PostRendu* element=new Element_PostRendu(p_rendu,tempon_id,mode_affichage,liste_affichage);
        liste_affichage->push_back(element);
        return element;
    }

    void supprime_rendu(Element_PostRendu* p_rendu)
    {
        vector<Element_PostRendu*>* liste_affichage=p_rendu->liste_affichage;
        liste_affichage->erase(liste_affichage->begin()+p_rendu->indice_liste_affichage);
        delete p_rendu;
    }

    //=================================================================
    //=================================================================
    //          Rendus
    //=================================================================
    //=================================================================


    //------- Rendu des éléménts avec leurs couleurs de base:
    void rendu_elements(dksOeil* oeil, vector<Element_PostRendu*>* liste_affichage)
    {
        uint32_t i;
        for (i=0;i<liste_affichage->size();i++)
        {
            Element_PostRendu* element=(*liste_affichage)[i];
                //cout<<element->rendu3d->nom<<" - mode: "<<uint32_t(element->mode_affichage)<<endl;
            if (element->drapeau_affiche && element->rendu->drapeau_affiche)
            {
                element->rendu->init_affichage();
                switch (element->mode_affichage)
                {
                    case DKSRENDU_COULEURS: element->rendu->affiche(oeil); break;
                    case DKSRENDU_COULEURS_LUMINESCENTES: element->rendu->affiche_couleurs_luminescentes(oeil);break;
                    case DKSRENDU_PROFONDEUR: element->rendu->affiche_profondeur(oeil); break;
                    default: break;
                }
                element->rendu->fin_affichage();
            }
        }
    }

    //======================================================================================================
    //          Gestion des tempons de rendu
    //======================================================================================================

    //-----------------------------------------------------------------
    //      Affiche les éléments dans le tempon
    //-----------------------------------------------------------------
    void affiche_tempon_ecran_1(dksOeil* oeil, bool drapeau_efface_sortie=true)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_ECRAN_1]);
        glViewport(0,0,ecran->resolution_x,ecran->resolution_y);
        glDepthMask(GL_TRUE);
        if(drapeau_efface_sortie)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(0);
        rendu_elements(oeil,&liste_affichage_ecran_1);
    }

    void affiche_tempon_ecran_2(dksOeil* oeil, bool drapeau_efface_sortie=true)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_ECRAN_2]);
        glViewport(0,0,ecran->resolution_x,ecran->resolution_y);
        glDepthMask(GL_TRUE);
        if(drapeau_efface_sortie)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(0);
        rendu_elements(oeil,&liste_affichage_ecran_2);
    }

    void affiche_tempon_quart(dksOeil* oeil, bool drapeau_efface_sortie=true)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_QUART_1]);
        glViewport(0,0,ecran->largeur_texture_quart,ecran->hauteur_texture_quart);
        glDepthMask(GL_TRUE);
        if(drapeau_efface_sortie)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(0);
        rendu_elements(oeil,&liste_affichage_quart);
    }

    void affiche_tempon_256(dksOeil* oeil, bool drapeau_efface_sortie=true)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_256_1]);
        glViewport(0,0,256,256);
        glDepthMask(GL_TRUE);
        if(drapeau_efface_sortie)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(0);
        rendu_elements(oeil,&liste_affichage_256);
    }


    //------------------------------------------------------------------
    //      Floutage des tempons
    //      Le rendu à flouter doit avoir été fait dans le tempon 1
    //------------------------------------------------------------------
    void floutage_tempon_256()
    {
        //-------------- Floutage X:
        dksRendu::desactive_textures();
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_256_2]);
        glViewport(0,0,256,256);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-0.5,0.5,-0.5,0.5);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_256_1]);
        glUseProgram(dksShaders::bibliotheque->floutage_x->programme_id);
        glUniform1i(dksShaders::bibliotheque->floutage_x->taille,largeur_flou);
        glUniform1f(dksShaders::bibliotheque->floutage_x->saturation,saturation_horizontale_flou);
        glUniform1i(dksShaders::bibliotheque->floutage_x->texture,0);

        affiche_texture();

        //--------------- floutage Y:

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_256_1]);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_256_2]);
        glViewport(0,0,256,256);

        glUseProgram(dksShaders::bibliotheque->floutage_y->programme_id);
        glUniform1i(dksShaders::bibliotheque->floutage_y->taille,hauteur_flou);
        glUniform1f(dksShaders::bibliotheque->floutage_y->saturation,saturation_verticale_flou);
        glUniform1i(dksShaders::bibliotheque->floutage_y->texture,0);

        affiche_texture();

        glUseProgram(0);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }


    void floutage_tempon_quart()
    {
        //-------------- Floutage X:
        dksRendu::desactive_textures();
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_QUART_2]);
        glViewport(0,0,ecran->largeur_texture_quart,ecran->hauteur_texture_quart);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-0.5,0.5,-0.5,0.5);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_QUART_1]);
        glUseProgram(dksShaders::bibliotheque->floutage_x->programme_id);
        glUniform1i(dksShaders::bibliotheque->floutage_x->taille,largeur_flou);
        glUniform1f(dksShaders::bibliotheque->floutage_x->saturation,saturation_horizontale_flou);
        glUniform1i(dksShaders::bibliotheque->floutage_x->texture,0);

        affiche_texture();

        //--------------- floutage Y:

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->frame_buffers[FB_QUART_1]);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_QUART_2]);
        glViewport(0,0,ecran->largeur_texture_quart,ecran->hauteur_texture_quart);

        glUseProgram(dksShaders::bibliotheque->floutage_y->programme_id);
        glUniform1i(dksShaders::bibliotheque->floutage_y->taille,hauteur_flou);
        glUniform1f(dksShaders::bibliotheque->floutage_y->saturation,saturation_verticale_flou);
        glUniform1i(dksShaders::bibliotheque->floutage_y->texture,0);

        affiche_texture();

        glUseProgram(0);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    //-----------------------------------------------------
    //      Superposition de deux tempons avec masquage
    //-----------------------------------------------------
    void masque_2_tempons(uint8_t texture_1,uint8_t texture_2, uint8_t frameBuffer_resultat=DKSECRAN_TEMPON_DEFAUT)
    {
        glUseProgram(0);
        depart_superposition_tempons(frameBuffer_resultat);

        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[texture_1]);
        //glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,ecran->textures[texture_2]);
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        glUseProgram(dksShaders::bibliotheque->masque_2_textures->programme_id);
        glUniform1i(dksShaders::bibliotheque->masque_2_textures->texture_1,0);
        glUniform1i(dksShaders::bibliotheque->masque_2_textures->texture_2,1);
        glUniform3f(dksShaders::bibliotheque->masque_2_textures->couleur_masque,couleur_masque[0],couleur_masque[1],couleur_masque[2]);

        affiche_2_textures();

        glUseProgram(0);

        fin_superposition_tempons();

    }


    //==================================================================================================
    //          Gestion de l'affichage des tempons
    //  Une fois les rendus effectués dans les tempons, on peut les afficher dans le bitmap visible.
    //==================================================================================================

    //------------------------------------------------------
    //      Initialise la superposition des tempons:
    /// L'identifiant du tempon de sortie est de type DKSECRAN_TEMPON_xxxx
    //------------------------------------------------------
    void depart_superposition_tempons(uint8_t id_tempon_sortie=DKSECRAN_TEMPON_DEFAUT,bool drapeau_efface_sortie=true)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,ecran->renvoie_tempon_id_openGl(id_tempon_sortie));

        glViewport(0,0,ecran->renvoie_largeur_tempon(id_tempon_sortie),ecran->renvoie_hauteur_tempon(id_tempon_sortie));
        dksRendu::desactive_textures();
        if(drapeau_efface_sortie) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-0.5,0.5,-0.5,0.5);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
    }

    //-----------------------------------------------------
    //      Superpose un tempon à l'écran par défaut:
    //-----------------------------------------------------
    void superpose_tempon(uint8_t tempon_id)
    {
        switch (tempon_id)
        {
            case DKSECRAN_TEMPON_ECRAN_1:
                glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_ECRAN_1]);
                affiche_texture(alpha_tempon_ecran);
            break;
            case DKSECRAN_TEMPON_ECRAN_2:
                glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_ECRAN_2]);
                affiche_texture(alpha_tempon_ecran);
            break;
            case DKSECRAN_TEMPON_QUART:
                glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_QUART_1]);
                affiche_texture(alpha_tempon_quart);
            break;
            case DKSECRAN_TEMPON_256:
                glBindTexture(GL_TEXTURE_2D,ecran->textures[TX_256_1]);
                affiche_texture(alpha_tempon_256);
            break;
            default:
            break;
        }

    }

    //-----------------------------------------------------------------------------
    //      Fin de la superposition - restitution des états sensibles d'OpenGL
    //-----------------------------------------------------------------------------
    void fin_superposition_tempons()
    {
        glDisable(GL_BLEND);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    //------------------------------------------------------
    //  Affiche une texture qui occupe tout le tempon actuel
    //------------------------------------------------------
    //Le view port doit avoir été correctement initialisé
    void affiche_texture(float alpha=1.)
    {
        GLboolean color_material_mem=glIsEnabled(GL_COLOR_MATERIAL);
        glDisable(GL_COLOR_MATERIAL);
        glColor4f(1.,1.,1.,alpha);
        glBegin(GL_QUADS);
            glTexCoord2f(0.,0.);
            glVertex3f(-.5,-.5,-1.);
            glTexCoord2f(1.,0.);
            glVertex3f(.5,-.5,-1.);
            glTexCoord2f(1.,1.);
            glVertex3f(.5,.5,-1.);
            glTexCoord2f(0.,1.);
            glVertex3f(-.5,.5,-1.);
        glEnd();
        if (color_material_mem) glEnable(GL_COLOR_MATERIAL);
    }

    //------------------------------------------------------
    //  Affiche deux textures qui occupent tout le tempon actuel
    ///Le view port doit avoir été correctement initialisé
    ///Le shader éventuel utilisé doit avoir été initialisé
    //------------------------------------------------------
    void affiche_2_textures(float alpha=1.)
    {
        GLboolean color_material_mem=glIsEnabled(GL_COLOR_MATERIAL);
        glDisable(GL_COLOR_MATERIAL);
        glColor4f(1.,1.,1.,alpha);
        glBegin(GL_QUADS);
            glMultiTexCoord2f(GL_TEXTURE0,0.,0.);
            glMultiTexCoord2f(GL_TEXTURE1,0.,0.);
            glVertex3d(-.5,-.5,-1.);
            glMultiTexCoord2f(GL_TEXTURE0,1.,0.);
            glMultiTexCoord2f(GL_TEXTURE1,1.,0.);
            glVertex3d(.5,-.5,-1.);
            glMultiTexCoord2f(GL_TEXTURE0,1.,1.);
            glMultiTexCoord2f(GL_TEXTURE1,1.,1.);
            glVertex3d(.5,.5,-1.);
            glMultiTexCoord2f(GL_TEXTURE0,0.,1.);
            glMultiTexCoord2f(GL_TEXTURE1,0.,1.);
            glVertex3d(-.5,.5,-1.);
        glEnd();
        if (color_material_mem) glEnable(GL_COLOR_MATERIAL);
    }

    //====================================================================================================
    //          Affichage de la liste des objet rendu couleurs, dans le tempon aux dimensions de l'écran
    //====================================================================================================
    void affiche_couleur_tempon_ecran_1(dksOeil* oeil)
    {
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);

        //-------- Rendu des éléments dans une texture:

        affiche_tempon_ecran_1(oeil);

        //--------------- Affiche la texture:

        depart_superposition_tempons();

        //Affiche la texture contenant la scène:

        superpose_tempon(DKSECRAN_TEMPON_ECRAN_1);

        //Restitution des états sensibles...

        fin_superposition_tempons();
    }


    void affiche_couleur_tempon_ecran_2(dksOeil* oeil)
    {
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);

        //-------- Rendu des éléments dans une texture:

        affiche_tempon_ecran_2(oeil);

        //--------------- Affiche la texture:

        depart_superposition_tempons();

        //Affiche la texture contenant la scène:

        superpose_tempon(DKSECRAN_TEMPON_ECRAN_2);

        //Restitution des états sensibles...

        fin_superposition_tempons();
    }

    //====================================================================================================
    //              Rendu avec une aura autour des surfaces luminescentes
    //              La texture de rendu du floutage doit être un carré.
    ///             La scène doit avoir été mise à jour!
    //====================================================================================================
    void affiche_flou_256(dksOeil* oeil,uint8_t id_tempon_a_flouter=DKSECRAN_TEMPON_ECRAN_1, uint8_t id_tempon_sortie=DKSECRAN_TEMPON_DEFAUT)
    {
        //Réglages d'OpenGL:
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);

        //---------------- Premier rendu: Les surfaces à flouter:
        affiche_tempon_256(oeil);

        //---------------- Floutage:
        floutage_tempon_256();

        //---------------- Rendu de la scène:
        if(id_tempon_a_flouter==DKSECRAN_TEMPON_ECRAN_1) affiche_tempon_ecran_1(oeil);
        else if(id_tempon_a_flouter==DKSECRAN_TEMPON_ECRAN_2) affiche_tempon_ecran_2(oeil);

        //--------------- Superposition des tempons:
        depart_superposition_tempons(id_tempon_sortie);
        superpose_tempon(id_tempon_a_flouter);
        superpose_tempon(DKSECRAN_TEMPON_256);
        fin_superposition_tempons();
    }

    void affiche_flou_quart(dksOeil* oeil,uint8_t id_tempon_a_flouter=DKSECRAN_TEMPON_ECRAN_1, uint8_t id_tempon_sortie=DKSECRAN_TEMPON_DEFAUT)
    {
        //Réglages d'OpenGL:
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);

        //---------------- Premier rendu: Les surfaces à flouter:
        affiche_tempon_quart(oeil);

        //---------------- Floutage:
        floutage_tempon_quart();

        //---------------- Rendu de la scène:
        if(id_tempon_a_flouter==DKSECRAN_TEMPON_ECRAN_1) affiche_tempon_ecran_1(oeil);
        else if(id_tempon_a_flouter==DKSECRAN_TEMPON_ECRAN_2) affiche_tempon_ecran_2(oeil);

        //--------------- Superposition des tempons:
        depart_superposition_tempons(id_tempon_sortie);
        superpose_tempon(id_tempon_a_flouter);
        superpose_tempon(DKSECRAN_TEMPON_QUART);
        fin_superposition_tempons();
    }



};

#endif // DKSPOSTRENDU_H_INCLUDED
