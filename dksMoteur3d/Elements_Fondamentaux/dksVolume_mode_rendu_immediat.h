//----------------------------------------------------------------------------
//      Initialise les pointeurs sur les fonctions de rendu en immédiat
//----------------------------------------------------------------------------
        void initialise_pointeurs_rendus_immediat()
        {
            //---------------- Opaques:
            rendus_immediats_opaques[DKSSURFACE_RENDU_APLAT]=&dksVolume::rendu_opaque_immediat_aplat;
            rendus_immediats_opaques[DKSSURFACE_RENDU_COULEUR]=&dksVolume::rendu_opaque_immediat_couleur;
            rendus_immediats_opaques[DKSSURFACE_RENDU_TEXTURE_COULEUR]=&dksVolume::rendu_opaque_immediat_texture_couleur;
            rendus_immediats_opaques[DKSSURFACE_RENDU_RUGOSITE]=&dksVolume::rendu_opaque_immediat_rugosite;
            rendus_immediats_opaques[DKSSURFACE_RENDU_RUGOSITE_TEXTURE_COULEUR]=&dksVolume::rendu_opaque_immediat_rugosite_texture_couleur;
            rendus_immediats_opaques[DKSSURFACE_RENDU_REFLEXION_CUBE]=&dksVolume::rendu_opaque_immediat_reflexion_cube;

            rendus_immediats_opaques[DKSSURFACE_RENDU_ONDES]=&dksVolume::rendu_opaque_immediat_ondes;
            rendus_immediats_opaques[DKSSURFACE_RENDU_ONDES_TEXTURE_COULEUR]=&dksVolume::rendu_opaque_immediat_ondes_texture_couleur;
            rendus_immediats_opaques[DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE]=&dksVolume::rendu_opaque_immediat_ondes_reflexion_cube;

            rendus_immediats_opaques[DKSSURFACE_RENDU_OMBRES]=&dksVolume::rendu_opaque_immediat_ombres;

            rendus_immediats_opaques[DKSSURFACE_RENDU_PHONG]=&dksVolume::rendu_opaque_immediat_couleur;
            rendus_immediats_opaques[DKSSURFACE_RENDU_PHONG_OMBRES]=&dksVolume::rendu_opaque_immediat_ombres;

            //Effets spéciaux:
            rendus_immediats_opaques[DKSSURFACE_RENDU_EFFET_SPECIAL]=&dksVolume::rendu_opaque_immediat_effet_special;

            rendus_immediats_opaques[DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR]=&dksVolume::rendu_opaque_immediat_transition_textures_couleur;

            //------------- Transparentes:
            rendus_immediats_transparentes[DKSSURFACE_RENDU_APLAT]=&dksVolume::rendu_transparente_immediat_aplat;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_APLAT]=&dksVolume::rendu_transparente_immediat_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_COULEUR]=&dksVolume::rendu_transparente_immediat_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_TEXTURE_COULEUR]=&dksVolume::rendu_transparente_immediat_texture_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_RUGOSITE]=&dksVolume::rendu_transparente_immediat_rugosite;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_RUGOSITE_TEXTURE_COULEUR]=&dksVolume::rendu_transparente_immediat_rugosite_texture_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_REFLEXION_CUBE]=&dksVolume::rendu_transparente_immediat_reflexion_cube;

            rendus_immediats_transparentes[DKSSURFACE_RENDU_ONDES]=&dksVolume::rendu_transparente_immediat_ondes;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_ONDES_TEXTURE_COULEUR]=&dksVolume::rendu_transparente_immediat_ondes_texture_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE]=&dksVolume::rendu_transparente_immediat_ondes_reflexion_cube;

            rendus_immediats_transparentes[DKSSURFACE_RENDU_OMBRES]=&dksVolume::rendu_transparente_immediat_ombres;

            rendus_immediats_transparentes[DKSSURFACE_RENDU_PHONG]=&dksVolume::rendu_transparente_immediat_couleur;
            rendus_immediats_transparentes[DKSSURFACE_RENDU_PHONG_OMBRES]=&dksVolume::rendu_transparente_immediat_ombres;

            //Effets spéciaux:
            rendus_immediats_transparentes[DKSSURFACE_RENDU_EFFET_SPECIAL]=&dksVolume::rendu_transparente_immediat_effet_special;

            rendus_immediats_transparentes[DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR]=&dksVolume::rendu_transparente_immediat_transition_textures_couleur;
        }

//----------------------------------------------------------------
//      Fonctions de rendu des surfaces opaques
//----------------------------------------------------------------
        //---------------------------------------------------
        // Rendu de surface opaque avec effet spécial
        //---------------------------------------------------

        void rendu_opaque_immediat_effet_special()
        {
            surface_courante->effet_special->rendu_opaque_immediat(surface_courante);
        }
        //------------------------------------------------------------------------------
        //Rendu en aplats de couleurs:
        //Cette fonction n'est pas dans la table des fonctions de rendu des surfaces.
        //Elle est appelée lorsque le mode de remplissage du volume = COULEUR
        //------------------------------------------------------------------------------

        void rendu_aplats_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            desactive_textures();
            dksEtat::utilise_programme(0);
            dksEtat::ombrage(GL_FLAT);

            //-------- Rendu en aplats:
            for (i=0;i<nbrSurfaces;i++)
            {
                surface_courante=surfaces[i];
                couleur_diffusion[0]=surface_courante->couleur_diffusion[0]*surface_courante->diffusion;
                couleur_diffusion[1]=surface_courante->couleur_diffusion[1]*surface_courante->diffusion;
                couleur_diffusion[2]=surface_courante->couleur_diffusion[2]*surface_courante->diffusion;
                couleur_diffusion[3]=1.;
                dksImmediat::couleur3fv(couleur_diffusion);  //Au cas où les éclairages sont désactivés
                dksEclairage::materiau(GL_FRONT,GL_DIFFUSE, couleur_diffusion);

                for (j=0;j<surface_courante->nbr_faces;j++)
                {
                    face=surface_courante->faces[j];

                    dksImmediat::debut(GL_POLYGON);
                        dksImmediat::normale3dv(face->normale);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
            }
        }


        //------------------------------------------------------------------------------
        //Rendu en aplats de couleurs surface par surface:
        //------------------------------------------------------------------------------

        void rendu_opaque_immediat_aplat()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //dksEclairage::materiau(GL_FRONT,GL_DIFFUSE, couleur_diffusion);

            for (j=0;j<surface_courante->nbr_faces;j++)
            {
                face=surface_courante->faces[j];

                dksImmediat::debut(GL_POLYGON);
                    //dksImmediat::normale3dv(face->normale);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        dksImmediat::sommet3dv(sommet);
                    }
                dksImmediat::fin();
            }
        }



        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_opaque_immediat_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            dksEtat::ombrage(GL_SMOOTH);
            dksImmediat::debut_lot();

            //-------- Rendu en aplats:
                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];

                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //---------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&face->normales_sommets[k*3];
                                    //nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
            dksImmediat::fin_lot();
        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_texture_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            dksTexture* texture=surface_courante->textures_couleur[surface_courante->indice_texture_couleur];

                    //----------- Rendu en aplats:

                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];

                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //----------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }

                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    nml=&face->normales_sommets[k*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }

        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_opaque_immediat_rugosite()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_rugosite=surface_courante->textures_rugosite[0];

                    //----------- Rendu en aplats:

                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];

                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_faces[face->indice*3]);
                                dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_faces[face->indice*3]);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::sommet3dv(&sommets[at[k]*3]);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //----------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                                    dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                                    dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                                    dksImmediat::sommet3dv(&sommets[at[k]*3]);
                                }
                            dksImmediat::fin();
                        }
                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];

                            dksImmediat::debut(GL_POLYGON);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                                    dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                                    dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                                    dksImmediat::sommet3dv(&sommets[at[k]*3]);
                                }
                            dksImmediat::fin();
                        }
                    }

        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_rugosite_texture_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_couleur=surface_courante->textures_couleur[0];
            dksTexture* texture_rugosite=surface_courante->textures_rugosite[0];

            //----------- Rendu en aplats:

            if (!surface_courante->drapeau_adoucissement)
            {
                for (j=0;j<surface_courante->nbr_faces;j++)
                {
                    face=surface_courante->faces[j];

                    dksImmediat::debut(GL_POLYGON);
                        dksImmediat::normale3dv(face->normale);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_faces[face->indice*3]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_faces[face->indice*3]);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
            }
            //----------- Surface adoucie:
            else
            {
                for (j=0;j<surface_courante->faces_adoucies.size();j++)
                {
                    face=surface_courante->faces_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                            dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                            dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
                for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                {
                    face=surface_courante->faces_partiellement_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                            dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                            dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
            }

        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_opaque_immediat_reflexion_cube()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            //-------- Rendu en aplats:
            if (!surface_courante->drapeau_adoucissement)
            {
                for (j=0;j<surface_courante->nbr_faces;j++)
                {
                    face=surface_courante->faces[j];

                    dksImmediat::debut(GL_POLYGON);
                        dksImmediat::normale3dv(face->normale);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
            }
            //---------- Surface adoucie:
            else
            {
                for (j=0;j<surface_courante->faces_adoucies.size();j++)
                {
                    face=surface_courante->faces_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&normales_sommets[at[k]*3];
                            dksImmediat::normale3dv(nml);
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
                for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                {
                    face=surface_courante->faces_partiellement_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&face->normales_sommets[k*3];
                            //nml=&normales_sommets[at[k]*3];
                            dksImmediat::normale3dv(nml);
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
            }
        }



        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_opaque_immediat_ombres()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            //-------- Rendu en aplats:
            if (!surface_courante->drapeau_adoucissement)
            {
                for (j=0;j<surface_courante->nbr_faces;j++)
                {
                    face=surface_courante->faces[j];

                    dksImmediat::debut(GL_POLYGON);
                        dksImmediat::normale3dv(face->normale);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
            }
            //---------- Surface adoucie:
            else
            {
                for (j=0;j<surface_courante->faces_adoucies.size();j++)
                {
                    face=surface_courante->faces_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&normales_sommets[at[k]*3];
                            dksImmediat::normale3dv(nml);
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
                for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                {
                    face=surface_courante->faces_partiellement_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&face->normales_sommets[k*3];
                            //nml=&normales_sommets[at[k]*3];
                            dksImmediat::normale3dv(nml);
                            dksImmediat::sommet3dv(sommet);
                        }
                    dksImmediat::fin();
                }
            }
        }


        //-------------------------------------------------------------------------------
        //          Ondes sans texture de couleur
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_ondes()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //-------- Rendu en aplats:
                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];
                            /*
                            glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,(face->centre_gravite_relatif[0]),
                                                                                                (face->centre_gravite_relatif[1]),
                                                                                                (face->centre_gravite_relatif[2]));
                                                                                                */

                            glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                            glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));



                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //---------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];


                                at=face->aretes;
                                /*
                                glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                                                                                */
                                glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                                glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];


                                at=face->aretes;
                                /*
                                glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                                                                                */
                                glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                                glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&face->normales_sommets[k*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }

        }

        //-------------------------------------------------------------------------------
        //          Ondes avec texture de couleur
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_ondes_texture_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            dksTexture* texture=surface_courante->textures_couleur[surface_courante->indice_texture_couleur];

            //-------- Rendu en aplats:
                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];
                            /*
                            glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,(face->centre_gravite_relatif[0]),
                                                                                                (face->centre_gravite_relatif[1]),
                                                                                                (face->centre_gravite_relatif[2]));
                                                                                                */

                            glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                            glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));



                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //---------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];


                                at=face->aretes;
                                /*
                                glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                                                                                */
                                glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                                glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];


                                at=face->aretes;
                                /*
                                glUniform3f(dksShaders::bibliotheque->ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                                                                                */
                                glUniform3f(dksShaders::bibliotheque->ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                                glUniform3f(dksShaders::bibliotheque->ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&face->normales_sommets[k*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }

        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_ondes_reflexion_cube()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //-------- Rendu en aplats:
                    if (!surface_courante->drapeau_adoucissement)
                    {
                        for (j=0;j<surface_courante->nbr_faces;j++)
                        {
                            face=surface_courante->faces[j];

                            glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->centre_gravite_face,(face->centre_gravite_relatif[0]),
                                                                                                (face->centre_gravite_relatif[1]),
                                                                                                (face->centre_gravite_relatif[2]));

                            glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->tangente,(face->tangente[0]),
                                                                                (face->tangente[1]),
                                                                                (face->tangente[2]));

                            glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->binormale,(face->binormale[0]),
                                                                                    (face->binormale[1]),
                                                                                    (face->binormale[2]));

                            dksImmediat::debut(GL_POLYGON);
                                dksImmediat::normale3dv(face->normale);
                                at=face->aretes;
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }
                    //---------- Surface adoucie:
                    else
                    {
                        for (j=0;j<surface_courante->faces_adoucies.size();j++)
                        {
                            face=surface_courante->faces_adoucies[j];


                                at=face->aretes;
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->tangente,1.,0.,1.);/*(face->tangente[0]),
                                                                                    (face->tangente[1]),
                                                                                    (face->tangente[2]));*/
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->binormale,float(face->binormale[0]),
                                                                                        float(face->binormale[1]),
                                                                                        float(face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&normales_sommets[at[k]*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                        for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                        {
                            face=surface_courante->faces_partiellement_adoucies[j];


                                at=face->aretes;
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->centre_gravite_face,float(face->centre_gravite_relatif[0]),
                                                                                                float(face->centre_gravite_relatif[1]),
                                                                                                float(face->centre_gravite_relatif[2]));
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->tangente,1.,0.,1.);/*(face->tangente[0]),
                                                                                    (face->tangente[1]),
                                                                                    (face->tangente[2]));*/
                                glUniform3f(dksShaders::bibliotheque->reflexion_cube_ondes->binormale,float(face->binormale[0]),
                                                                                        float(face->binormale[1]),
                                                                                        float(face->binormale[2]));
                            dksImmediat::debut(GL_POLYGON);
                                for(k=0;k<face->nbrSommets;k++)
                                {
                                    sommet=&sommets[at[k]*3];
                                    nml=&face->normales_sommets[k*3];
                                    dksImmediat::normale3dv(nml);
                                    dksImmediat::sommet3dv(sommet);
                                }
                            dksImmediat::fin();
                        }
                    }

        }

        //-------------------------------------------------------------------------------
        //Transition entre deux textures, avec une texture supplémentaire pour le facteur de transition.
        //-------------------------------------------------------------------------------
        void rendu_opaque_immediat_transition_textures_couleur()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_depart=surface_courante->textures_couleur[surface_courante->indice_texture_depart];
            dksTexture* texture_destination=surface_courante->textures_couleur[surface_courante->indice_texture_destination];
            dksTexture* texture_transition=surface_courante->textures_couleur[surface_courante->indice_texture_transition];

            //----------- Rendu en aplats:

            if (!surface_courante->drapeau_adoucissement)
            {
                for (j=0;j<surface_courante->nbr_faces;j++)
                {
                    face=surface_courante->faces[j];

                    dksImmediat::debut(GL_POLYGON);
                        dksImmediat::normale3dv(face->normale);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
            }
            //----------- Surface adoucie:
            else
            {
                for (j=0;j<surface_courante->faces_adoucies.size();j++)
                {
                    face=surface_courante->faces_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
                for (j=0;j<surface_courante->faces_partiellement_adoucies.size();j++)
                {
                    face=surface_courante->faces_partiellement_adoucies[j];

                    dksImmediat::debut(GL_POLYGON);
                        at=face->aretes;
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                            dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                            dksImmediat::sommet3dv(&sommets[at[k]*3]);
                        }
                    dksImmediat::fin();
                }
            }

        }

//================================================================
//      Fonctions de rendu des surfaces transparentes
//================================================================

        //---------------------------------------------------
        // Rendu de surface transparente avec effet spécial
        //---------------------------------------------------

        void rendu_transparente_immediat_effet_special(dksFace* face)
        {
            surface_courante->effet_special->rendu_transparente_immediat(face);
        }


        //------------------------------------------------------------------------------
        //Rendu en aplats de couleurs surface par surface:
        //------------------------------------------------------------------------------

        void rendu_transparente_immediat_aplat(dksFace* face)
        {
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;

            GLboolean eclairage_mem=dksEtat::est_active(GL_LIGHTING);
            dksEtat::desactive(GL_LIGHTING);
            dksEclairage::materiau(GL_FRONT,GL_DIFFUSE, couleur_diffusion);
            dksEtat::ombrage(GL_FLAT);

            for(k=0;k<face->nbrSommets;k++)
                {
                    sommet=&sommets[at[k]*3];
                    dksImmediat::sommet3dv(sommet);
                }
            if(eclairage_mem)dksEtat::active(GL_LIGHTING);
        }


        //---------------------------------------------------
        //                      Couleur
        //---------------------------------------------------

        void rendu_transparente_immediat_couleur(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
            //------------------- Face en aplats:
            if(!surface_courante->drapeau_adoucissement)
            {
                dksImmediat::normale3dv(face->normale);
                for(k=0;k<face->nbrSommets;k++)
                {
                    sommet=&sommets[at[k]*3];
                    dksImmediat::sommet3dv(sommet);
                }
            }

            //------------------- Face adoucie;
            else
            {
                if (face->drapeau_partiellement_adoucie)
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&face->normales_sommets[k*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::sommet3dv(sommet);
                    }
                }
                else
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&normales_sommets[at[k]*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::sommet3dv(sommet);
                    }
                }

            }

        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------
        void rendu_transparente_immediat_texture_couleur(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture=surface_courante->textures_couleur[0];

            //------------------- Face en aplats:
            if(!surface_courante->drapeau_adoucissement)
            {
                dksImmediat::normale3dv(face->normale);
                for(k=0;k<face->nbrSommets;k++)
                {
                    sommet=&sommets[at[k]*3];
                    dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::sommet3dv(sommet);
                }
            }

            //------------------- Face adoucie;
            else
            {
                if (face->drapeau_partiellement_adoucie)
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&normales_sommets[at[k]*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::sommet3dv(sommet);
                    }
                }
                else
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&face->normales_sommets[k*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::texture2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::sommet3dv(sommet);
                    }
                }

            }
        }


        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_rugosite(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_rugosite=surface_courante->textures_rugosite[0];

            //----------- Rendu en aplats:

            if (!surface_courante->drapeau_adoucissement)
            {
                dksImmediat::normale3dv(face->normale);
                dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_faces[face->indice*3]);
                dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_faces[face->indice*3]);
                for(k=0;k<face->nbrSommets;k++)
                {
                    dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::sommet3dv(&sommets[at[k]*3]);
                }
            }

            //----------- Surface adoucie:
            else
            {
                if (!face->drapeau_partiellement_adoucie)
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                        dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                }

                else
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        dksImmediat::texture2fv(&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                        dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                }
            }
        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------
        void rendu_transparente_immediat_rugosite_texture_couleur(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_rugosite=surface_courante->textures_rugosite[0];
            dksTexture* texture_couleur=surface_courante->textures_couleur[0];

            //----------- Rendu en aplats:

            if (!surface_courante->drapeau_adoucissement)
            {
                dksImmediat::normale3dv(face->normale);
                dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_faces[face->indice*3]);
                dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_faces[face->indice*3]);
                for(k=0;k<face->nbrSommets;k++)
                {
                    dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::sommet3dv(&sommets[at[k]*3]);
                }
            }

            //----------- Surface adoucie:
            else
            {

                if (!face->drapeau_partiellement_adoucie)
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                        dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                }

                else
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_couleur->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_rugosite->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->tangente,&texture_rugosite->tangentes_sommets[at[k]*3]);
                        dksImmediat::attribut3dv(dksShaders::bibliotheque->couleur_rugosite->binormale,&texture_rugosite->binormales_sommets[at[k]*3]);
                        dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                }

            }
        }


        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_reflexion_cube(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_ondes(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_ondes_texture_couleur(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }

        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_ondes_reflexion_cube(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }


        //-------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------

        void rendu_transparente_immediat_ombres(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }



        //-------------------------------------------------------------------------------
        //  Ombres + Phong:
        //-------------------------------------------------------------------------------
        void rendu_transparente_immediat_phong_ombres(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
        }


        //-------------------------------------------------------------------------------
        //Rendu des surfaces transparentes avec transition entre deux textures de couleur:
        //-------------------------------------------------------------------------------
        void rendu_transparente_immediat_transition_textures_couleur(dksFace* face)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at=face->aretes;
            GLdouble* sommet;
            GLdouble* nml;
            dksTexture* texture_depart=surface_courante->textures_couleur[surface_courante->indice_texture_depart];
            dksTexture* texture_destination=surface_courante->textures_couleur[surface_courante->indice_texture_destination];
            dksTexture* texture_transition=surface_courante->textures_couleur[surface_courante->indice_texture_transition];
            //------------------- Face en aplats:
            if(!surface_courante->drapeau_adoucissement)
            {
                dksImmediat::normale3dv(face->normale);
                for(k=0;k<face->nbrSommets;k++)
                {
                    sommet=&sommets[at[k]*3];
                    dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::sommet3dv(sommet);
                }
            }

            //------------------- Face adoucie;
            else
            {
                if (face->drapeau_partiellement_adoucie)
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&normales_sommets[at[k]*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::sommet3dv(sommet);
                    }
                }
                else
                {
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        nml=&face->normales_sommets[k*3];
                        dksImmediat::normale3dv(nml);
                        dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::multi_texture2fv(GL_TEXTURE2,&texture_transition->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::sommet3dv(sommet);
                    }
                }

            }
        }

//============================================================================================
//          Rendu des surfaces opaques en mode immédiat:
//          Le booléen en entrée est à usage interne. Il sert
//          pour un rendu des faces transparentes sans tri.
//============================================================================================

        virtual void rendu_opaques_mode_immediat(bool drapeau_inverse_transparentes_opaques=false)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //**************** Surfaces opaques:

            if (drapeau_inverse_transparentes_opaques)
            {
                dksEtat::active(GL_BLEND);
                glDepthMask(GL_FALSE);
            }
            else
            {
                dksEtat::desactive(GL_BLEND);
                if (drapeau_tempon_profondeur) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);
            }

            for (i=0;i<nbrSurfaces;i++)
            {
                surface_courante=surfaces[i];
                if (surface_courante->drapeau_transparence!=drapeau_inverse_transparentes_opaques) continue;
                initialise_materiau(surface_courante);
                (this->*rendus_immediats_opaques[surface_courante->mode_rendu])();

            }


        }

        //======================================================================
        //  Rendu des surfaces transparentes en mode immédiat
        //======================================================================

        virtual void rendu_transparentes_mode_immediat(dksOeil* oeil)
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;


            if ( nbrFaces_transparentes > 0 )
            {
                dksEtat::active(GL_BLEND);
                glDepthMask(GL_FALSE);
                tri_faces_transparentes(oeil);
                surface_courante=NULL;

                for (i=0;i<nbrFaces_transparentes;i++)
                {
                    face=indices_faces_transparentes[i].face;
                    //Au besoin, définir le matériau:
                    if(face->surface!=surface_courante)
                    {
                        surface_courante=face->surface;
                        initialise_materiau(surface_courante);
                    }

                    dksImmediat::debut(GL_POLYGON);
                        (this->*rendus_immediats_transparentes[surface_courante->mode_rendu])(face);
                    dksImmediat::fin();
                }

            }
        }

        //----------------------------------------------------------------------------
        //          Rendu des surfaces transparentes en mode immédiat, sans tri:
        //          Sert surtout pour générer la liste openGl
        //----------------------------------------------------------------------------

        virtual void rendu_transparentes_mode_immediat_sans_tri()
        {
           rendu_opaques_mode_immediat(true);
        }

        //----------------------------------------------------------------
        //  Rendu des surfaces luminescentes en mode immédiat
        //  Pas de texturage
        //  Pas de transparence
        //----------------------------------------------------------------
        virtual void rendu_surfaces_luminescentes_immediat()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksSurface* surface;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //**************** Surfaces opaques:
            dksEtat::desactive(GL_BLEND);
            glDepthMask(GL_TRUE);

            //if (drapeau_tempon_profondeur) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);
            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                //if(surface->luminescence>0.)   //Décommenter si les surfaces opaques ne recouvrent pas les surfaces luminescentes
                //{
                    dksImmediat::fin_lot();
                    initialise_materiau_luminescent(surface);
                    dksImmediat::debut_lot();

                    //========================= Rendu à plat sans texture:

                    for (j=0;j<surface->nbr_faces;j++)
                    {
                        dksFace* face=surface->faces[j];

                        dksImmediat::debut(GL_POLYGON);
                            at=face->aretes;
                            for(k=0;k<face->nbrSommets;k++)
                            {
                                sommet=&sommets[at[k]*3];
                                dksImmediat::sommet3dv(sommet);
                            }
                        dksImmediat::fin();
                    }
                //}
            }
            dksImmediat::fin_lot();
        }

        //----------------------------------------------------------------
        //  Rendu des profondeur, pour l'affichage des ombres
        //  Pas de texturage
        //  Pas de transparence
        //----------------------------------------------------------------
        virtual void rendu_profondeur_immediat()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksSurface* surface;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            //**************** Surfaces opaques:


            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                //if(surface->luminescence>0.)
                //{
                    dksImmediat::fin_lot();
                    initialise_materiau_profondeur(surface);
                    dksImmediat::debut_lot();

                    //========================= Rendu à plat sans texture:

                    for (j=0;j<surface->nbr_faces;j++)
                    {
                        dksFace* face=surface->faces[j];

                        dksImmediat::debut(GL_POLYGON);
                            at=face->aretes;
                            for(k=0;k<face->nbrSommets;k++)
                            {
                                sommet=&sommets[at[k]*3];
                                dksImmediat::sommet3dv(sommet);
                            }
                        dksImmediat::fin();
                    }
                //}
            }
            dksImmediat::fin_lot();
        }
