//=======================================================================
//  Fonctions de rendu du volume par l'usage des tableaux de sommets.
//  Il y a une petite restriction au niveau des adoucissement des surface:
//      L'angle limite d'adoucissement n'est fonctionnel qu'aux bordures de
//      la surface. Les faces à l'intérieur de la surface sont toujours
//      adoucies, quelque soit la limite de cet angle.
//=======================================================================


//----------------------------------------------------------------------------------------
//          Rendu des surfaces opaques avec l'utilisation des tableaux:
//----------------------------------------------------------------------------------------

        virtual void rendu_opaques_mode_tableaux()
        {
            uint32_t i,j;
            dksSurface* surface;
            dksFace* face;

            //************** Surfaces opaques:
            glDisable(GL_BLEND);
            if (drapeau_tempon_profondeur) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);

            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                if (surface->drapeau_transparence) continue;

                initialise_materiau(surface);

                //============================= Rendu avec textures:

                //============================= Rendu sans textures:

                //else //if (mode_remplissage==DKSVOLUME_REMPLISSAGE_COULEUR)
                //{
                    //------------------- Surface en aplat brut (pas d'éclairage)
                    if(surface->mode_rendu==DKSSURFACE_RENDU_APLAT)
                    {
                        GLboolean eclairage_mem=glIsEnabled(GL_LIGHTING);
                        glDisable(GL_LIGHTING);
                        glShadeModel(GL_FLAT);
                        for(j=0;j<surface->nbr_faces;j++)
                        {
                            face=surface->faces[j];
                            glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                        }
                        if(eclairage_mem)glEnable(GL_LIGHTING);
                    }
                    //------------------- Surface adoucie:
                    else if(surface->drapeau_adoucissement)
                    {
                        for(j=0;j<surface->nbr_faces;j++)
                        {
                            face=surface->faces[j];
                            glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                        }
                    }

                    //------------------- Surface en éclairage de Gouraud;
                    else
                    {
                        for(j=0;j<surface->nbr_faces;j++)
                        {
                            //if(drapeau_debug)cout<<"face:"<<i<<"/"<<surface->nbr_faces<<endl;
                            face=surface->faces[j];
                            glNormal3dv(face->normale);
                            glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                        }
                    }
                //}

            }
        }

        //----------------------------------------------------------------
        //  Rendu des surfaces transparentes en utilisant les tableaux
        /// Fonction appelée seulement si le volume doit rendre ses
        /// faces transprentes en interne (drapeau_rendu_face_transprentes=true)
        //----------------------------------------------------------------

        virtual void rendu_transparentes_mode_tableaux(dksOeil* oeil)
        {
            uint32_t i,j;
            dksSurface* surface;
            dksFace* face;

            //************** Surfaces transparentes:
            if ( nbrFaces_transparentes > 0 )
            {
                glEnable(GL_BLEND);
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

                    //------------------- Face adoucie:
                    if(surface_courante->drapeau_adoucissement)
                    {
                        glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                    }

                    //------------------- Face en aplats;
                    else
                    {
                        glNormal3dv(face->normale);
                        glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                    }
                }
            }
        }

        //----------------------------------------------------------------
        //  Rendu des surfaces luminescentes en mode tableaux
        //  Pas de texturage
        //  Pas de transparence
        //----------------------------------------------------------------
        virtual void rendu_surfaces_luminescentes_tableaux()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksSurface* surface;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            duree_rendu=glutGet(GLUT_ELAPSED_TIME);

            //**************** Surfaces opaques:
            glDisable(GL_BLEND);
            if (drapeau_tempon_profondeur) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);

            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                //if(surface->luminescence>0.)
                //{
                    initialise_materiau_luminescent(surface);

                    //========================= Rendu à plat sans texture:

                    for (j=0;j<surface->nbr_faces;j++)
                    {
                        dksFace* face=surface->faces[j];
                        glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                    }
                //}
            }
        }

        //----------------------------------------------------------------
        //  Rendu des surfaces luminescentes en mode tableaux
        //  Pas de texturage
        //  Pas de transparence
        //----------------------------------------------------------------
        virtual void rendu_profondeur_tableaux()
        {
            uint32_t i,j;
            uint16_t k;
            uint32_t* at;
            dksSurface* surface;
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;

            duree_rendu=glutGet(GLUT_ELAPSED_TIME);

            //**************** Surfaces opaques:

            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                //if(surface->luminescence>0.)
                //{
                    initialise_materiau_profondeur(surface);

                    //========================= Rendu à plat sans texture:

                    for (j=0;j<surface->nbr_faces;j++)
                    {
                        dksFace* face=surface->faces[j];
                        glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                    }
                //}
            }
        }

