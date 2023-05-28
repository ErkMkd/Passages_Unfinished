//-------------------------------------------------------------------------
//          Rendu des faces transparentes de la scène en mode immédiat
//-------------------------------------------------------------------------
    void rendu_faces_transparentes_immediat()
    {
        double* sommets=volume_courant->sommets;
        double* sommet;
        GLdouble* nml;
        uint32_t* at;
        uint32_t k;

        for (;indice_face_transparente<nbr_faces_transparentes;indice_face_transparente++)
        {
            dksFace*face=faces_transparentes_triables[indice_face_transparente].face;
            //cout<<faces_transparentes_triables[indice_face_transparente].distance<<endl;
            //Au besoin, définir le matériau:

            if(face->surface!=surface_courante)
            {
                //Si le volume change:
                if (face->volume!=volume_courant)
                {
                    if (face->volume->mode_rendu==DKSVOLUME_MODE_IMMEDIAT || face->volume->mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_IMMEDIAT_TRANSPARENTES)
                    {
                        volume_courant=face->volume;
                        sommets=volume_courant->sommets;
                        initialise_mode_rendu_volume();
                        glPopMatrix();
                        glPushMatrix();
                        glMultMatrixd(volume_courant->matrice.matrice);
                    }
                    //Si le volume a changé et qu'il est en mode de rendu par tableaux, on retourne à la boucle principale
                    else return;
                }
                surface_courante=face->surface;
                volume_courant->initialise_materiau(surface_courante);
            }

            glBegin(GL_POLYGON);

            //===================================== Rendu sans textures:
            if (surface_courante->mode_rendu==DKSSURFACE_RENDU_COULEUR)
            {
                //cout<<"la!"<<endl;
                //------------------- Face en aplats:
                if(!surface_courante->drapeau_adoucissement)
                {
                    glNormal3dv(face->normale);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        glVertex3dv(sommet);
                    }
                }

                //------------------- Face adoucie;
                else
                {
                    at=face->aretes;
                    if (face->drapeau_partiellement_adoucie)
                    {
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&face->normales_sommets[k*3];
                            glNormal3dv(nml);
                            glVertex3dv(sommet);
                        }
                    }
                    else
                    {
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&volume_courant->normales_sommets[at[k]*3];
                            glNormal3dv(nml);
                            glVertex3dv(sommet);
                        }
                    }

                }

            }

            //======================================Rendu avec texture de couleur:

            else if (surface_courante->mode_rendu==DKSSURFACE_RENDU_TEXTURE_COULEUR)
            {
                dksTexture* texture=surface_courante->textures_couleur[0];

                //------------------- Face en aplats:
                if(!surface_courante->drapeau_adoucissement)    ///A optimiser
                {
                    glNormal3dv(face->normale);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        sommet=&sommets[at[k]*3];
                        glTexCoord2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                        glVertex3dv(sommet);
                    }
                }

                //------------------- Face adoucie;
                else
                {
                    at=face->aretes;
                    if (face->drapeau_partiellement_adoucie)
                    {
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&face->normales_sommets[k*3];
                            glNormal3dv(nml);
                            glTexCoord2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                            glVertex3dv(sommet);
                        }
                    }
                    else
                    {
                        for(k=0;k<face->nbrSommets;k++)
                        {
                            sommet=&sommets[at[k]*3];
                            nml=&volume_courant->normales_sommets[at[k]*3];
                            glNormal3dv(nml);
                            glTexCoord2fv(&texture->coordonnees_UV_tableau[2*at[k]]);
                            glVertex3dv(sommet);
                        }
                    }

                }
            }

            //======================================Rendu avec texture de rugosité:
            else if (surface_courante->mode_rendu==DKSSURFACE_RENDU_RUGOSITE)
            {
                /// A ajouter si besoin, mais l'usage des tableaux est plus indiqué
            }


            //======================================Rendu avec texture de rugosité et texture de couleur:
            else if (surface_courante->mode_rendu==DKSSURFACE_RENDU_RUGOSITE_TEXTURE_COULEUR)
            {
                /// A ajouter si besoin, mais l'usage des tableaux est plus indiqué
            }

            //========================== Effet spécial:
            else if(surface_courante->mode_rendu==DKSSURFACE_RENDU_EFFET_SPECIAL)
            {
                surface_courante->effet_special->rendu_transparente_immediat(face);
            }

            glEnd();
        }
    }
