//-------------------------------------------------------------------------
//          Rendu des faces transparentes de la scène en mode tableaux
//-------------------------------------------------------------------------

        void rendu_faces_transparentes_tableaux()
        {
            double* sommets=volume_courant->sommets;
            double* sommet,nml;
            uint32_t* at;
            uint32_t k;

            for (;indice_face_transparente<nbr_faces_transparentes;indice_face_transparente++)
            {
                dksFace*face=faces_transparentes_triables[indice_face_transparente].face;

                //Au besoin, définir le matériau:
                if(face->surface!=surface_courante)
                {
                    //Si le volume change:
                    if (face->volume!=volume_courant)
                    {
                        if (face->volume->mode_rendu==DKSVOLUME_MODE_TABLEAUX || face->volume->mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
                        {
                            volume_courant=face->volume;
                            sommets=volume_courant->sommets;
                            initialise_mode_rendu_volume();
                            glPopMatrix();
                            glPushMatrix();
                            glMultMatrixd(volume_courant->matrice.matrice);
                        }
                        //Si le volume a changé et qu'il est en mode de rendu immédiat, on retourne à la boucle principale
                        else return;
                    }
                    surface_courante=face->surface;
                    volume_courant->initialise_materiau(surface_courante);
                }

                if (!surface_courante->drapeau_adoucissement) glNormal3dv(face->normale);
                glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
            }
        }
