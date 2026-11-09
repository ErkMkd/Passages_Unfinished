//-----------------------------------------------------------------------------
//      Initialise les pointeurs sur les fonctions d'initialisation des rendus
//-----------------------------------------------------------------------------
        void initialise_pointeurs_init_rendus()
        {


            //---- Opaques:
            init_rendus[DKSSURFACE_RENDU_APLAT]=&dksVolume::init_rendu_aplat;
            init_rendus[DKSSURFACE_RENDU_COULEUR]=&dksVolume::init_rendu_couleur;
            init_rendus[DKSSURFACE_RENDU_TEXTURE_COULEUR]=&dksVolume::init_rendu_texture_couleur;
            init_rendus[DKSSURFACE_RENDU_RUGOSITE]=&dksVolume::init_rendu_rugosite;
            init_rendus[DKSSURFACE_RENDU_RUGOSITE_TEXTURE_COULEUR]=&dksVolume::init_rendu_rugosite_texture_couleur;
            init_rendus[DKSSURFACE_RENDU_REFLEXION_CUBE]=&dksVolume::init_rendu_reflexion_cube;

            init_rendus[DKSSURFACE_RENDU_ONDES]=&dksVolume::init_rendu_ondes;
            init_rendus[DKSSURFACE_RENDU_ONDES_TEXTURE_COULEUR]=&dksVolume::init_rendu_ondes_texture_couleur;
            init_rendus[DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE]=&dksVolume::init_rendu_ondes_reflexion_cube;

            init_rendus[DKSSURFACE_RENDU_OMBRES]=&dksVolume::init_rendu_ombres;

            init_rendus[DKSSURFACE_RENDU_PHONG]=&dksVolume::init_rendu_phong;
            init_rendus[DKSSURFACE_RENDU_PHONG_OMBRES]=&dksVolume::init_rendu_phong_ombres;

            //Effets spéciaux:
            init_rendus[DKSSURFACE_RENDU_EFFET_SPECIAL]=&dksVolume::init_rendu_effet_special;


            init_rendus[DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR]=&dksVolume::init_rendu_transition_textures_couleur;
            //init_rendus[DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR_AXE]=&dksVolume::init_rendu_transition_textures_couleur_axe;
        }

//=============================================================================
//      Fonctions d'initialisation des rendus.
//      Ces fonctions se chargent de configurer OpenGL comme il faut.
//=============================================================================

    //-----------------------------------------------------------------------------------------
    //      Rendu des effets spéciaux propres aux surfaces (priorité sur les autres rendus)
    //-----------------------------------------------------------------------------------------
    virtual void init_rendu_effet_special()
    {
        surface_courante->effet_special->initialisation(surface_courante);
    }

    //--------------------------------------------------------------------------------
    //      Rendu de base, sans textures.
    //--------------------------------------------------------------------------------
    virtual void init_rendu_couleur()
    {
        dksEtat::utilise_programme(0);
        //Normalement cette fonction ne sert pas. "initialise_materiau()" fait tout ce qu'il faut.
    }

    //--------------------------------------------------------------------------------
    //      Rendu en aplat, sans éclairage, avec la couleur de diffusion de la surface
    //--------------------------------------------------------------------------------
    virtual void init_rendu_aplat()
    {
        dksEtat::utilise_programme(dksShaders::bibliotheque->aplat_brut->programme_id);
        dksTableaux::desactive(GL_NORMAL_ARRAY); //Au cas où on est en mode TABLEAU
    }

    //--------------------------------------------------------------------------------
    //      Rendu avec éclairage phong
    //--------------------------------------------------------------------------------
    void init_rendu_phong()
    {
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->eclairage_phong->programme_id);
        glUniform1i(dksShaders::bibliotheque->eclairage_phong->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
    }

    //---------------------------------------------
    //      Rendu avec une texture de couleur
    //---------------------------------------------
    virtual void init_rendu_texture_couleur()
    {
        dksEtat::utilise_programme(0);
        dksTexture* texture;
        texture=surface_courante->textures_couleur[surface_courante->indice_texture_couleur];
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture->type_enveloppe);
        //Gestion des tableaux:
        if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            dksTableaux::unite(GL_TEXTURE0);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
        }
    }

    //---------------------------------------------
    //      Rendu avec une texture de rugosité
    //---------------------------------------------
    void init_rendu_rugosite()
    {
        dksTexture* texture;
        texture=surface_courante->textures_rugosite[0];
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture->type_enveloppe);
        //Gestion des tableaux:
        if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            dksTableaux::unite(0);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            dksTableaux::pointeur_attribut(dksShaders::bibliotheque->rugosite->tangente,3,GL_DOUBLE,GL_FALSE,0,texture->tangentes_sommets);
            dksTableaux::pointeur_attribut(dksShaders::bibliotheque->rugosite->binormale,3,GL_DOUBLE,GL_FALSE,0,texture->binormales_sommets);
            dksTableaux::active_attribut(dksShaders::bibliotheque->rugosite->tangente);
            dksTableaux::active_attribut(dksShaders::bibliotheque->rugosite->binormale);
        }
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->rugosite->programme_id);
        glUniform1i(dksShaders::bibliotheque->rugosite->texture_rugosite,0);
    }

    //---------------------------------------------------------------------
    //      Rendu avec une texture de rugosité et une texture de couleur
    //---------------------------------------------------------------------
    void init_rendu_rugosite_texture_couleur()
    {
        dksTexture* texture;
        //Initialise la texture de couleur:
        texture=surface_courante->textures_couleur[surface_courante->indice_texture_couleur];
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture->type_enveloppe);
        //Initialise la texture de rugosité:
        texture=surface_courante->textures_rugosite[0];
        dksEtat::unite_active(GL_TEXTURE1);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture->type_enveloppe);
        //Gestion des tableaux:
        if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            //Texture de couleurs:
            dksTableaux::unite(GL_TEXTURE0);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            //Texture de rugosité:
            dksTableaux::unite(GL_TEXTURE1);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            dksTableaux::pointeur_attribut(dksShaders::bibliotheque->couleur_rugosite->tangente,3,GL_DOUBLE,GL_FALSE,0,texture->tangentes_sommets);
            dksTableaux::pointeur_attribut(dksShaders::bibliotheque->couleur_rugosite->binormale,3,GL_DOUBLE,GL_FALSE,0,texture->binormales_sommets);
            dksTableaux::active_attribut(dksShaders::bibliotheque->couleur_rugosite->tangente);
            dksTableaux::active_attribut(dksShaders::bibliotheque->couleur_rugosite->binormale);
        }
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->couleur_rugosite->programme_id);
        glUniform1i(dksShaders::bibliotheque->couleur_rugosite->texture_couleur,0);
        glUniform1i(dksShaders::bibliotheque->couleur_rugosite->texture_rugosite,1);
    }

    //--------------------------------------------------------------------------------
    //      Rendu avec réflexion du cube d'arrière plan, sans texture sur la surface
    //--------------------------------------------------------------------------------
    void init_rendu_reflexion_cube()
    {
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->reflexion_cube->programme_id);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube->niveau,surface_courante->reflexion);
        glUniform1i(dksShaders::bibliotheque->reflexion_cube->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform3f(dksShaders::bibliotheque->reflexion_cube->position_oeil,oeil_courant->repere_absolu->x,oeil_courant->repere_absolu->y,oeil_courant->repere_absolu->z);
        glUniformMatrix4fv(dksShaders::bibliotheque->reflexion_cube->matrice_volume,1,GL_FALSE,matrice.renvoie_matrice_f());
        glUniform1i(dksShaders::bibliotheque->reflexion_cube->texture,0);
    }



    //-----------------------------------------------------------------------------------------------
    //      Rendu avec les ombres portées sur les surfaces sans texture.
    //-----------------------------------------------------------------------------------------------
    void init_rendu_ombres()
    {
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture_ombres_id);
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->ombres->programme_id);
        glUniform1i(dksShaders::bibliotheque->ombres->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform1i(dksShaders::bibliotheque->ombres->texture_profondeur,0);
        glUniformMatrix4fv(dksShaders::bibliotheque->ombres->matrice_source_uv,1,GL_FALSE,matrice_uv_source_des_ombres.renvoie_matrice_f());
        glUniformMatrix4fv(dksShaders::bibliotheque->ombres->matrice_projection_vision_source,1,GL_FALSE,matrice_projection_vision_source_des_ombres.renvoie_matrice_f());
        glUniform1fv(dksShaders::bibliotheque->ombres->composantes_convertion,2,composantes_convertion);
        glUniformMatrix4fv(dksShaders::bibliotheque->ombres->matrice_vision_source,1,GL_FALSE,matrice_vision_source_des_ombres.renvoie_matrice_f());
        glUniform1f(dksShaders::bibliotheque->ombres->seuil,cosf(angle_max_ombres/180.*M_PI));
        glUniform1f(dksShaders::bibliotheque->ombres->intensite,intensite_ombres);
    }

    //-----------------------------------------------------------------------------------------------
    //      Rendu avec les ombres portées avec éclairage en phong sur les surfaces sans texture.
    //-----------------------------------------------------------------------------------------------
    void init_rendu_phong_ombres()
    {
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture_ombres_id);
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->phong_ombres->programme_id);
        glUniform1i(dksShaders::bibliotheque->phong_ombres->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform1i(dksShaders::bibliotheque->phong_ombres->texture_profondeur,0);
        glUniformMatrix4fv(dksShaders::bibliotheque->phong_ombres->matrice_source_uv,1,GL_FALSE,matrice_uv_source_des_ombres.renvoie_matrice_f());
        glUniformMatrix4fv(dksShaders::bibliotheque->phong_ombres->matrice_projection_vision_source,1,GL_FALSE,matrice_projection_vision_source_des_ombres.renvoie_matrice_f());
        glUniform1fv(dksShaders::bibliotheque->phong_ombres->composantes_convertion,2,composantes_convertion);
        glUniformMatrix4fv(dksShaders::bibliotheque->phong_ombres->matrice_vision_source,1,GL_FALSE,matrice_vision_source_des_ombres.renvoie_matrice_f());
        glUniform1f(dksShaders::bibliotheque->phong_ombres->seuil,cosf(angle_max_ombres/180.*M_PI));
        glUniform1f(dksShaders::bibliotheque->phong_ombres->intensite,intensite_ombres);
    }

    //-----------------------------------------------------------------------------------------------
    //      Rendu avec ondes aqueuses sur les surfaces sans textures.
    //-----------------------------------------------------------------------------------------------
    void init_rendu_ondes()
    {
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->ondes->programme_id);
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::lie_texture(GL_TEXTURE_2D,dksShaders::bibliotheque->texture_bruit_id);
        glUniform1i(dksShaders::bibliotheque->ondes->texture_bruit,0);
        glUniform1i(dksShaders::bibliotheque->ondes->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform1f(dksShaders::bibliotheque->ondes->position_temps,float(position_temps));

        glUniform1f(dksShaders::bibliotheque->ondes->intensite,surface_courante->intensite_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->vitesse,surface_courante->vitesse_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->quantite,surface_courante->quantite_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->frequence,surface_courante->frequence_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->amplitude,surface_courante->amplitude_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->niveau_perturbations,surface_courante->perturbation_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes->echelle_texture_bruit,surface_courante->echelle_perturbation_ondes);
    }

    //-----------------------------------------------------------------------------------------------
    //      Rendu avec ondes aqueuses sur les surfaces avec textures.
    //-----------------------------------------------------------------------------------------------
    void init_rendu_ondes_texture_couleur()
    {
        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->ondes_texture_couleur->programme_id);
        dksTexture* texture;
        texture=surface_courante->textures_couleur[surface_courante->indice_texture_couleur];
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture->type_enveloppe);
        glUniform1i(dksShaders::bibliotheque->ondes_texture_couleur->texture_couleur,0);

        dksEtat::unite_active(GL_TEXTURE1);
        dksEtat::lie_texture(GL_TEXTURE_2D,dksShaders::bibliotheque->texture_bruit_id);
        glUniform1i(dksShaders::bibliotheque->ondes_texture_couleur->texture_bruit,1);
        glUniform1i(dksShaders::bibliotheque->ondes_texture_couleur->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->position_temps,float(position_temps));

        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->intensite,surface_courante->intensite_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->vitesse,surface_courante->vitesse_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->quantite,surface_courante->quantite_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->frequence,surface_courante->frequence_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->amplitude,surface_courante->amplitude_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->niveau_perturbations,surface_courante->perturbation_ondes);
        glUniform1f(dksShaders::bibliotheque->ondes_texture_couleur->echelle_texture_bruit,surface_courante->echelle_perturbation_ondes);
    }

    //-----------------------------------------------------------------------------------------------
    //      Rendu avec réflexion et ondes aqueuses sur les surfaces sans textures.
    //-----------------------------------------------------------------------------------------------
    void init_rendu_ondes_reflexion_cube()
    {
        //Initialise le shader:
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::utilise_programme(dksShaders::bibliotheque->reflexion_cube_ondes->programme_id);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->niveau_reflexion,surface_courante->reflexion);
        glUniform1i(dksShaders::bibliotheque->reflexion_cube_ondes->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->position_oeil,oeil_courant->repere_absolu->x,oeil_courant->repere_absolu->y,oeil_courant->repere_absolu->z);
        glUniformMatrix4fv(dksShaders::bibliotheque->reflexion_cube_ondes->matrice_volume,1,GL_FALSE,matrice.renvoie_matrice_f());
        glUniform1i(dksShaders::bibliotheque->reflexion_cube_ondes->texture_cube,0);

        dksEtat::unite_active(GL_TEXTURE1);
        dksEtat::lie_texture(GL_TEXTURE_2D,dksShaders::bibliotheque->texture_bruit_id);
        glUniform1i(dksShaders::bibliotheque->reflexion_cube_ondes->texture_bruit,1);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->position_temps,float(position_temps));

        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->intensite,               surface_courante->intensite_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->vitesse,                 surface_courante->vitesse_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->quantite,                surface_courante->quantite_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->frequence,               surface_courante->frequence_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->amplitude,               surface_courante->amplitude_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->niveau_perturbations,    surface_courante->perturbation_ondes);
        glUniform1f(dksShaders::bibliotheque->reflexion_cube_ondes->echelle_texture_bruit,   surface_courante->echelle_perturbation_ondes);
    }

    //-----------------------------------------------------------------------------------------------
    //      Rendu avec transition entre 2 textures de couleur
    //-----------------------------------------------------------------------------------------------

    void init_rendu_transition_textures_couleur()
    {
        dksTexture* texture_depart;
        dksTexture* texture_dest;
        dksTexture* texture_trans;

        //Texture départ:
        texture_depart=surface_courante->textures_couleur[surface_courante->indice_texture_depart];
        dksEtat::unite_active(GL_TEXTURE0);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture_depart->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture_depart->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture_depart->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture_depart->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture_depart->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture_depart->type_enveloppe);

        //Texture destination:
        texture_dest=surface_courante->textures_couleur[surface_courante->indice_texture_destination];
        dksEtat::unite_active(GL_TEXTURE1);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture_dest->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture_dest->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture_dest->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture_dest->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture_dest->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture_dest->type_enveloppe);

        //Texture de transition:
        texture_trans=surface_courante->textures_couleur[surface_courante->indice_texture_transition];
        dksEtat::unite_active(GL_TEXTURE2);
        dksEtat::active(GL_TEXTURE_2D);
        dksEtat::lie_texture(GL_TEXTURE_2D,texture_trans->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture_trans->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture_trans->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture_trans->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture_trans->type_diminution);
        dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,texture_trans->type_enveloppe);

        //Gestion des tableaux:
        if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            dksTableaux::unite(GL_TEXTURE0);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture_depart->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            dksTableaux::unite(GL_TEXTURE1);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture_dest->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            dksTableaux::unite(GL_TEXTURE2);
            dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture_trans->coordonnees_UV_tableau);
            dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
        }

        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->transition_textures_couleur->programme_id);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur->texture_depart,0);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur->texture_destination,1);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur->texture_transition,2);
        glUniform1f(dksShaders::bibliotheque->transition_textures_couleur->niveau,surface_courante->niveau_transition);
    }


