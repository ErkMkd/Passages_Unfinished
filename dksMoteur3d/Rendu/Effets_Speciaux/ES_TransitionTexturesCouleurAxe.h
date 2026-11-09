#ifndef ES_TRANSITIONTEXTURESCOULEURAXE_H_INCLUDED
#define ES_TRANSITIONTEXTURESCOULEURAXE_H_INCLUDED

/*
    Transition entre deux textures, le long de l'axe Z d'un volume.
*/

#include "dksEffetSpecial.h"

class ES_TransitionTexturesCouleurAxe : public dksEffetSpecial
{
    public:
        float zmin; //extrémités de la transition (par défaut, la dimension z du volume)
        float zmax;
        float niveau; //Position sur l'axe Z
        float largeur_transition;  // varie entre 0 et 1
        GLint drapeaux_textures;

    //============================
    //      Constructeur
    //============================
    ES_TransitionTexturesCouleurAxe():dksEffetSpecial()
    {

    }

    //============================
    //      Destructeur
    //============================

    //=========================================================================================
    //  Fonctions
    //=========================================================================================

    //---------------------------------------------------------
    //  Les limites de transition sont callée sur un volume
    //---------------------------------------------------------
    void initialise_limites(dksVolume* volume)
    {
        zmin=volume->boite_collision->zmin;
        zmax=volume->boite_collision->zmax;
    }

    //-----------------------------------------------------------------
    //
    //-----------------------------------------------------------------
    virtual void initialisation(dksSurface* surface)
    {
        dksTexture* texture_depart;
        dksTexture* texture_dest;
        dksVolume* vol=surface->volume;

        //Texture départ:
        drapeaux_textures=0;
        if(surface->indice_texture_depart>=0)
        {
            drapeaux_textures|=0x1;
            texture_depart=surface->textures_couleur[surface->indice_texture_depart];
            dksEtat::unite_active(GL_TEXTURE0);
            dksEtat::active(GL_TEXTURE_2D);
            dksEtat::lie_texture(GL_TEXTURE_2D,texture_depart->image->texture_id);
            glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,vol->couleur_diffusion);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture_depart->recouvrement_s);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture_depart->recouvrement_t);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture_depart->type_grossissement);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture_depart->type_diminution);
            dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        }
        //Texture destination:
        if(surface->indice_texture_destination>=0)
        {
            drapeaux_textures|=0x2;
            texture_dest=surface->textures_couleur[surface->indice_texture_destination];
            dksEtat::unite_active(GL_TEXTURE1);
            dksEtat::active(GL_TEXTURE_2D);
            dksEtat::lie_texture(GL_TEXTURE_2D,texture_dest->image->texture_id);
            glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,vol->couleur_diffusion);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture_dest->recouvrement_s);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture_dest->recouvrement_t);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture_dest->type_grossissement);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture_dest->type_diminution);
            dksEtat::enveloppe_i(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        }
        //Gestion des tableaux:
        if (vol->mode_rendu==DKSVOLUME_MODE_TABLEAUX || vol->mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            if(drapeaux_textures&0x1)
            {
                dksTableaux::unite(GL_TEXTURE0);
                dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture_depart->coordonnees_UV_tableau);
                dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            }
            if(drapeaux_textures&0x2)
            {
                dksTableaux::unite(GL_TEXTURE1);
                dksTableaux::pointeur_textures(2,GL_FLOAT,0,texture_dest->coordonnees_UV_tableau);
                dksTableaux::active(GL_TEXTURE_COORD_ARRAY);
            }
        }

        //Initialise le shader:
        dksEtat::utilise_programme(dksShaders::bibliotheque->transition_textures_couleur_axe->programme_id);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur_axe->drapeaux_sources,dksSourceLumineuse::allocation_lumieres_openGl);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur_axe->texture_depart,0);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur_axe->texture_destination,1);
        glUniform1f(dksShaders::bibliotheque->transition_textures_couleur_axe->zmin,zmin);
        glUniform1f(dksShaders::bibliotheque->transition_textures_couleur_axe->zmax,zmax);
        glUniform1f(dksShaders::bibliotheque->transition_textures_couleur_axe->niveau,niveau);
        glUniform1f(dksShaders::bibliotheque->transition_textures_couleur_axe->largeur_fondu,largeur_transition);
        glUniform1i(dksShaders::bibliotheque->transition_textures_couleur_axe->drapeaux_textures,drapeaux_textures);
    }

    //--------------------------------------------------------------------------------------------------------------------
    //
    //  Rendus en mode immédiat
    /// ATTENTION ce n'est pas optimisé, le mode immédiat étant surtout utilisé provisoirement pour le développement.
    //--------------------------------------------------------------------------------------------------------------------
    virtual void rendu_opaque_immediat(dksSurface* surface)
    {
        dksVolume* vol=surface->volume;
        double* sommets=vol->sommets;
        double* normales_sommets=vol->normales_sommets;
        uint32_t j;
        uint16_t k;
        uint32_t* at;
        dksFace* face;

        //Indice -1: pas de texture (drapeaux_textures est alors à 0 pour cette texture)
        dksTexture* texture_depart=surface->indice_texture_depart>=0 ? surface->textures_couleur[surface->indice_texture_depart] : NULL;
        dksTexture* texture_destination=surface->indice_texture_destination>=0 ? surface->textures_couleur[surface->indice_texture_destination] : NULL;

        //----------- Rendu en aplats:

        if (!surface->drapeau_adoucissement)
        {
            for (j=0;j<surface->nbr_faces;j++)
            {
                face=surface->faces[j];

                dksImmediat::debut(GL_POLYGON);
                    dksImmediat::normale3dv(face->normale);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                        if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                dksImmediat::fin();
            }
        }
        //----------- Surface adoucie:
        else
        {
            for (j=0;j<surface->faces_adoucies.size();j++)
            {
                face=surface->faces_adoucies[j];

                dksImmediat::debut(GL_POLYGON);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                        if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::normale3dv(&normales_sommets[at[k]*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                dksImmediat::fin();
            }
            for (j=0;j<surface->faces_partiellement_adoucies.size();j++)
            {
                face=surface->faces_partiellement_adoucies[j];

                dksImmediat::debut(GL_POLYGON);
                    at=face->aretes;
                    for(k=0;k<face->nbrSommets;k++)
                    {
                        if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                        if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                        dksImmediat::normale3dv(&face->normales_sommets[k*3]);
                        dksImmediat::sommet3dv(&sommets[at[k]*3]);
                    }
                dksImmediat::fin();
            }
        }
    }


    //-----------------------------------------------------------------
    //
    //-----------------------------------------------------------------
    virtual void rendu_transparente_immediat(dksFace* face)
    {
        dksSurface* surface=face->surface;
        double* sommets=surface->volume->sommets;
        double* normales_sommets=surface->volume->normales_sommets;
        uint16_t k;
        uint32_t* at=face->aretes;
        GLdouble* sommet;
        GLdouble* nml;
        //Indice -1: pas de texture (drapeaux_textures est alors à 0 pour cette texture)
        dksTexture* texture_depart=surface->indice_texture_depart>=0 ? surface->textures_couleur[surface->indice_texture_depart] : NULL;
        dksTexture* texture_destination=surface->indice_texture_destination>=0 ? surface->textures_couleur[surface->indice_texture_destination] : NULL;

        //------------------- Face en aplats:
        if(!surface->drapeau_adoucissement)
        {
            dksImmediat::normale3dv(face->normale);
            for(k=0;k<face->nbrSommets;k++)
            {
                sommet=&sommets[at[k]*3];
                if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
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
                    if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                    if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
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
                    if(drapeaux_textures&0x1)dksImmediat::multi_texture2fv(GL_TEXTURE0,&texture_depart->coordonnees_UV_tableau[2*at[k]]);
                    if(drapeaux_textures&0x2)dksImmediat::multi_texture2fv(GL_TEXTURE1,&texture_destination->coordonnees_UV_tableau[2*at[k]]);
                    dksImmediat::sommet3dv(sommet);
                }
            }

        }
    }
};

#endif // ES_TRANSITIONTEXTURESCOULEURAXE_H_INCLUDED
