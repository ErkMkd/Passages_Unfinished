//---------------------------------------------------------
//          Rendus pour le stockage dans la liste OpenGl:
//---------------------------------------------------------
        void rendu_mode_liste()
        {
            glCallList(liste_id);
        }

        void rendu_mode_liste_opaques_immediat_transparentes(dksOeil* oeil)
        {
            //Surfaces opaques:
            glCallList(liste_id);
            //Surfaces transparentes:
            if (drapeau_rendu_faces_transparentes) rendu_transparentes_mode_immediat(oeil);
        }

        void rendu_mode_liste_opaques_tableaux_transparentes(dksOeil* oeil)
        {
            //Surfaces opaques:
            glCallList(liste_id);
            //Surfaces transparentes:
            if (drapeau_rendu_faces_transparentes) rendu_transparentes_mode_tableaux(oeil);
        }

        void rendu_surfaces_luminescentes_liste()
        {
            if (glIsList(liste_surfaces_luminescentes_id)) glCallList(liste_surfaces_luminescentes_id);
        }

        void rendu_profondeur_liste()
        {
            if (glIsList(liste_profondeur_id)) glCallList(liste_profondeur_id);
        }

        //===========================================================================
        //      Gestion des listes d'affichage du volume
        //===========================================================================

        //------------------------------
        //      Mise à jour des listes
        //------------------------------
    public:
        void maj_listes()
        {
            if(drapeau_liste_luminescences) maj_liste_luminescences();
            if(drapeau_liste_profondeurs) maj_liste_profondeurs();
            maj_liste_affichage();
        }

        void maj_liste_luminescences()
        {
            if (glIsList(liste_surfaces_luminescentes_id)) glDeleteLists(liste_surfaces_luminescentes_id,1);
            genere_liste_surfaces_luminescentes();
        }

        void maj_liste_profondeurs()
        {
            if (glIsList(liste_profondeur_id)) glDeleteLists(liste_profondeur_id,1);
            genere_liste_profondeur();
        }

        void maj_liste_affichage()
        {
            if (
                mode_rendu==DKSVOLUME_MODE_LISTE
                || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES
                ||mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES
                )
                {
                    if(glIsList(liste_id)) glDeleteLists(liste_id,1);
                    genere_liste_affichage();
                }

        }

        //Génère ou met à jour la liste d'affichage des surfaces luminescentes:
        ///Appeler maj_liste_xxx si besoin.
        void determine_liste_luminescences(bool d)
        {
            if(d && (!glIsList(liste_surfaces_luminescentes_id)))genere_liste_surfaces_luminescentes();
            drapeau_liste_luminescences=d;
        }

        //Génère ou met à jour la liste d'affichage des profondeurs:
        ///Appeler maj_liste_xxx si besoin.
        void determine_liste_profondeurs(bool d)
        {
            if(d && (!glIsList(liste_profondeur_id)))genere_liste_profondeur();
            drapeau_liste_profondeurs=d;
        }

        bool renvoie_drapeau_liste_luminescences()
        {
            return drapeau_liste_luminescences;
        }
        bool renvoie_drapeau_liste_profondeurs()
        {
            return drapeau_liste_profondeurs;
        }

        //------- Supprime les listes d'affichage OpenGl:

    protected:
        void supprime_listes()
        {
            if (glIsList(liste_id)) glDeleteLists(liste_id,1);
            if (glIsList(liste_surfaces_luminescentes_id)) glDeleteLists(liste_surfaces_luminescentes_id,1);
            if (glIsList(liste_profondeur_id)) glDeleteLists(liste_profondeur_id,1);
            liste_id=0;
            liste_surfaces_luminescentes_id=0;
            liste_profondeur_id=0;
        }

        void genere_listes()
        {
            supprime_listes();

            //---------Liste du volume entier:

            genere_liste_affichage();

            //-----------Liste des surfaces luminescentes:

            if (drapeau_liste_luminescences) genere_liste_surfaces_luminescentes();

            //-----------Liste des profondeurs:

            if(drapeau_liste_profondeurs) genere_liste_profondeur();

        }

        //--------------------------------------
        //  Liste d'affichage normale du volume
        //--------------------------------------

        void genere_liste_affichage()
        {
            liste_id=glGenLists(1);
            glNewList(liste_id,GL_COMPILE);
            rendu_opaques_mode_immediat();
            if (mode_rendu==DKSVOLUME_MODE_LISTE) rendu_transparentes_mode_immediat_sans_tri();
            glEndList();
        }

        //---------------------------------------------------
        //  Liste d'affichage des surfaces luminescentes
        //---------------------------------------------------
        void genere_liste_surfaces_luminescentes()
        {
           liste_surfaces_luminescentes_id=glGenLists(1);
            glNewList(liste_surfaces_luminescentes_id,GL_COMPILE);
                rendu_surfaces_luminescentes_immediat();
            glEndList();
        }

        //-------------------------------------------
        //  Liste d'affichage des profondeurs
        //-------------------------------------------
        void genere_liste_profondeur()
        {
            liste_profondeur_id=glGenLists(1);
            glNewList(liste_profondeur_id,GL_COMPILE);
                rendu_profondeur_immediat();
            glEndList();
        }
