//---------------------------------------------------------
//          Rendus des modes "liste"
//
//  Les listes d'affichage d'OpenGl n'existent pas dans le profil Core.
//  Ces modes rendent directement, en mode immédiat, ce que contenaient
//  les listes. Comme avant, une "liste" n'est rendue que si elle a été
//  générée (drapeaux liste_xxx_generee).
//---------------------------------------------------------
        void rendu_liste_affichage()
        {
            if (!liste_generee) return;
            rendu_opaques_mode_immediat();
            if (liste_avec_transparentes) rendu_transparentes_mode_immediat_sans_tri();
        }

        void rendu_mode_liste()
        {
            rendu_liste_affichage();
        }

        void rendu_mode_liste_opaques_immediat_transparentes(dksOeil* oeil)
        {
            //Surfaces opaques:
            rendu_liste_affichage();
            //Surfaces transparentes:
            if (drapeau_rendu_faces_transparentes) rendu_transparentes_mode_immediat(oeil);
        }

        void rendu_mode_liste_opaques_tableaux_transparentes(dksOeil* oeil)
        {
            //Surfaces opaques:
            rendu_liste_affichage();
            //Surfaces transparentes:
            if (drapeau_rendu_faces_transparentes) rendu_transparentes_mode_tableaux(oeil);
        }

        void rendu_surfaces_luminescentes_liste()
        {
            if (liste_luminescences_generee) rendu_surfaces_luminescentes_immediat();
        }

        void rendu_profondeur_liste()
        {
            if (liste_profondeur_generee) rendu_profondeur_immediat();
        }

        //===========================================================================
        //      Gestion des "listes" du volume
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
            genere_liste_surfaces_luminescentes();
        }

        void maj_liste_profondeurs()
        {
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
                    genere_liste_affichage();
                }

        }

        //Génère la liste des surfaces luminescentes si elle n'existe pas:
        ///Appeler maj_liste_xxx si besoin.
        void determine_liste_luminescences(bool d)
        {
            if(d && !liste_luminescences_generee) genere_liste_surfaces_luminescentes();
            drapeau_liste_luminescences=d;
        }

        //Génère la liste des profondeurs si elle n'existe pas:
        ///Appeler maj_liste_xxx si besoin.
        void determine_liste_profondeurs(bool d)
        {
            if(d && !liste_profondeur_generee) genere_liste_profondeur();
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

        //------- Supprime les listes:

    protected:
        void supprime_listes()
        {
            liste_generee=false;
            liste_avec_transparentes=false;
            liste_luminescences_generee=false;
            liste_profondeur_generee=false;
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
        //  Liste normale du volume
        //--------------------------------------

        void genere_liste_affichage()
        {
            liste_generee=true;
            liste_avec_transparentes= mode_rendu==DKSVOLUME_MODE_LISTE;
        }

        //---------------------------------------------------
        //  Liste des surfaces luminescentes
        //---------------------------------------------------
        void genere_liste_surfaces_luminescentes()
        {
            liste_luminescences_generee=true;
        }

        //-------------------------------------------
        //  Liste des profondeurs
        //-------------------------------------------
        void genere_liste_profondeur()
        {
            liste_profondeur_generee=true;
        }
