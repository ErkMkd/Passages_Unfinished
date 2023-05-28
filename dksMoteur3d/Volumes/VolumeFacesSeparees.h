#ifndef VOLUMEFACESSEPAREES_H_INCLUDED
#define VOLUMEFACESSEPAREES_H_INCLUDED

/*
        Sépare les faces d'un volume et les gère comme des particules
        Les faces sont individualisées via un vertex-shader.
        Les paramètres envoyés au shader sont:
        -Décallage de la face,
        -Orientation de la face,
        -Niveau d'éclairement, pour l'adaptation au rendu des auras.
*/


#include "../Particules/dksParticules.h"

//--------------------------------------------------
//          Paramètres spécifiques des particules
//--------------------------------------------------
class Particule_Face_Separee :public Element_particule
{
    public:
    //Paramètres envoyés au vertex-shader:
    float niveau_eclairage;    //Sert lors du rendu des faces luminescentes - Pour pouvoir jouer avec la taille de l'aura.
    float deplacement_centre_gravite[3];   //Le déplacement du centre de gravité d'origine de la face, dans le repère TNB.
    Interpolation_Parametre_3f* interpolation_deplacement;
    Interpolation_Parametre* interpolation_niveau_eclairage;

    //L'orientation de la face peut se faire via les angles TLR, ou par rotation quelconque  de son repère TNB
    float tanguage;
    float lacet;
    float roulis;



        Particule_Face_Separee(uint32_t p_index,dksFace* p_face):Element_particule(p_index)
        {
            niveau_eclairage=0.;
            deplacement_centre_gravite={0.,0.,0.};
            tanguage=0.;
            lacet=0.;
            roulis=0.;
            float depart_defaut[3]={0.,0.,0.};
            float fin_defaut[3]={0.,1.,0.};
            interpolation_deplacement=new Interpolation_Parametre_3f(-1.,depart_defaut,fin_defaut,10.);
            interpolation_niveau_eclairage=new Interpolation_Parametre(-1.,1.,0.,0.5);
        }

        ~Particule_Face_Separee()
        {
            if (interpolation_deplacement!=NULL) delete interpolation_deplacement;
        }
};


//***********************************************************************************************
//                                              LA CLASSE
//**********************************************************************************************

class VolumeFacesSeparees : public dksParticules
{
    public:

    //===============================================
    //          Constructeur
    //===============================================
    VolumeFacesSeparees(char* p_nom,
                        dksVolume* p_volume_origine,
                    double px=0.,double py=0.,double pz=0.,
                    dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL)
                    :dksParticules(p_nom,px,py,pz,0,axe1,axe2,axe3)

    {
        drapeaux_liberation_memoire=DKSVOLUME_LIBERE_FACES|DKSVOLUME_LIBERE_SOMMETS|DKSVOLUME_LIBERE_SURFACES;
        determine_sens_polygones(p_volume_origine->renvoie_sens_polygones());

        if (!copie_sommets(p_volume_origine->renvoie_sommets(),p_volume_origine->nbrSommets)) return;
        if (!copie_faces(p_volume_origine)) return;
        if (!copie_surfaces(p_volume_origine)) return;
        if (!determine_nombre_de_points(nbrFaces)) return;
        reset_particules();
        determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);

        initialise_pointeurs_init_rendus_faces_separees();
        //initialise_pointeurs_rendus_immediat();


    }

    //-----------------------------------
    //          Destructeur
    //-----------------------------------
    ~VolumeFacesSeparees()
    {

    }

    //---------------------------------------------------------
    //          Création des paramètres des particules
    //      Fonction appelée par determine_nombre_de_points
    ///     Les faces du volume doivent avoir été initialisées
    //---------------------------------------------------------
    virtual void creation_particules()
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            particules[i]=new Particule_Face_Separee(i,faces[i]);
        }
    }

    virtual void destruction_particules()
    {
        dksParticules::destruction_particules();
    }


    private:
    //-----------------------------------------------
    //      Copiage des sommets du volume originel
    //-----------------------------------------------
    bool copie_sommets(double* org_sommets,uint32_t p_nbrSommets)
    {
        uint32_t i;
        //Allocation:
        double* p_sommets=(double*)malloc(p_nbrSommets*sizeof(double)*3);
        if (p_sommets==NULL)
        {
            cout<<"Erreur dans VolumeFacesSeparee.copie_sommets() - Mémoire insuffisante"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        //Copie les sommets:
        for (i=0;i<p_nbrSommets;i++)
        {
            p_sommets[3*i]=org_sommets[3*i];
            p_sommets[3*i+1]=org_sommets[3*i+1];
            p_sommets[3*i+2]=org_sommets[3*i+2];
        }

        //Initialise les sommets et tout ce qui va avec (normales...):
        return determine_sommets(p_sommets,p_nbrSommets);
    }

    //-----------------------------------------------
    //      Copiage des faces du volume originel
    //-----------------------------------------------
    bool copie_faces(dksVolume* p_volume_origine)
    {
        uint32_t i;
        //Allocation de la table des faces:
        dksFace** p_faces=(dksFace**)calloc(p_volume_origine->nbrFaces,sizeof(dksFace*));
        if (p_faces==NULL)
        {
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            cout<<"Erreur dans VolumeFacesSeparees.copie_faces() - Mémoire insuffisante"<<endl;
            return false;
        }

        //Copiage:
        for (i=0;i<p_volume_origine->nbrFaces;i++)
        {
            dksFace* nouvelle_face=new dksFace(*p_volume_origine->renvoie_face(i));
            if (nouvelle_face==NULL)
            {
                erreur=DKSVOLUME_ERREUR_MEMOIRE;
                cout<<"Erreur dans VolumeFacesSeparees.copie_faces() - Impossible de copier les faces du volume original"<<endl;
                return false;
            }
            p_faces[i]=nouvelle_face;
        }
        //Initialisation:
        return determine_faces(p_faces,p_volume_origine->nbrFaces);
    }

    //-----------------------------------------------
    //      Copiage des surfaces du volume originel
    //-----------------------------------------------
    bool copie_surfaces(dksVolume* p_volume_origine)
    {
        uint32_t i;
        //Allocation de la table des faces:
        dksSurface** p_surfaces=(dksSurface**)calloc(p_volume_origine->nbrSurfaces,sizeof(dksSurface*));
        if (p_surfaces==NULL)
        {
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            cout<<"Erreur dans VolumeFacesSeparees.copie_surfaces() - Mémoire insuffisante"<<endl;
            return false;
        }
        //Copiage:
        for (i=0;i<p_volume_origine->nbrSurfaces;i++)
        {
            dksSurface* nouvelle_surface=new dksSurface(*p_volume_origine->renvoie_surface(i));
            if (nouvelle_surface==NULL)
            {
                erreur=DKSVOLUME_ERREUR_MEMOIRE;
                cout<<"Erreur dans VolumeFacesSeparees.copie_surfaces() - Impossible de copier les surfaces du volume original"<<endl;
                return false;
            }
            p_surfaces[i]=nouvelle_surface;
        }
        //Initialisation:
        return determine_surfaces(p_surfaces,p_volume_origine->nbrSurfaces);
    }

    //------------------------------------------------------------------
    //         Restaure les positions et les orientations des faces
    //          par rapport au volume d'origine
    //------------------------------------------------------------------
    void reset_particules()
    {
        uint32_t i;
        for (i=0;i<nbrPoints;i++)
        {
            points[3*i]=faces[i]->centre_gravite_relatif[0];
            points[3*i+1]=faces[i]->centre_gravite_relatif[1];
            points[3*i+2]=faces[i]->centre_gravite_relatif[2];
            Particule_Face_Separee* particule=(Particule_Face_Separee*)particules[i];
            particule->roulis=0.;
            particule->tanguage=0.;
            particule->lacet=0.;
        }
    }


    //===============================================================================================================================
    //===============================================================================================================================
    //                          Animation
    //===============================================================================================================================
    //===============================================================================================================================
    private:
    //---------------------------------------------------
    //  Calcul les vecteurs de déplacement des faces
    //---------------------------------------------------
    void maj_faces()
    {
        uint32_t i;
        for(i=0;i<nbrPoints;i++)
        {
            dksFace* face=faces[i];
            Particule_Face_Separee* particule=(Particule_Face_Separee*)particules[i];
            particule->deplacement_centre_gravite[0]=points[3*i]-face->centre_gravite_relatif[0];
            particule->deplacement_centre_gravite[1]=points[3*i+1]-face->centre_gravite_relatif[1];
            particule->deplacement_centre_gravite[2]=points[3*i+2]-face->centre_gravite_relatif[2];
        }
    }
    public:
    void interpolation_positions_faces(float t)
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Face_Separee* particule = (Particule_Face_Separee*)particules[i];
            particule->interpolation_deplacement->renvoie_valeur_parametre(t,particule->deplacement_centre_gravite);
        }
    }

    //--------- Anime les faces dans la fourchette spécifiée:
    void interpolation_positions_fourchette_faces(float t,uint32_t index_depart, uint32_t index_fin)
    {
        index_fin++;
        if (index_fin>nbrPoints) index_fin=nbrPoints;
        for (uint32_t i=index_depart;i<index_fin;i++)
        {
            Particule_Face_Separee* particule = (Particule_Face_Separee*)particules[i];
            if (!particule->interpolation_deplacement->drapeau_terminee) particule->interpolation_deplacement->renvoie_valeur_parametre(t,particule->deplacement_centre_gravite);
        }
    }

    //--------- Anime les faces dans la fourchette spécifiée:
    // Lorsque l'interpolation de position est terminée, l'animation du clignotement est jouée.
    void interpolation_positions_clignottement_fourchette_faces(float t,uint32_t index_depart, uint32_t index_fin)
    {
        index_fin++;
        if (index_fin>nbrPoints) index_fin=nbrPoints;
        for (uint32_t i=index_depart;i<index_fin;i++)
        {
            Particule_Face_Separee* particule = (Particule_Face_Separee*)particules[i];
            if (!particule->interpolation_deplacement->drapeau_terminee) particule->interpolation_deplacement->renvoie_valeur_parametre(t,particule->deplacement_centre_gravite);
            else if (!particule->interpolation_niveau_eclairage->drapeau_terminee) particule->niveau_eclairage=particule->interpolation_niveau_eclairage->renvoie_valeur_parametre(t);
        }
    }



    //-----------------------------------------------------------
    //  Détermine le déplacement des faces dans leurs repères TNB
    //-----------------------------------------------------------
    void determine_deplacements_faces(float dx,float dy,float dz)
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Face_Separee* particule=(Particule_Face_Separee*)particules[i];
            particule->deplacement_centre_gravite={dx,dy,dz};
        }
    }

    //--------------------------------------------------------------------------------
    //  Détermine des déplacements aléatoires des faces dans leurs repères TNB
    //--------------------------------------------------------------------------------
    void random_deplacements_faces(float xmin,float xmax,float ymin, float ymax, float zmin, float zmax)
    {
        for (uint32_t i=0;i<nbrPoints;i++)
        {
            Particule_Face_Separee* particule=(Particule_Face_Separee*)particules[i];

            float dx=float(rand())/32768.*(xmax-xmin)+xmin;
            float dy=float(rand())/32768.*(ymax-ymin)+ymin;
            float dz=float(rand())/32768.*(zmax-zmin)+zmin;

            particule->deplacement_centre_gravite={dx,dy,dz};
        }
    }

    //==========================================================================
    //        Affichage
    //==========================================================================

    virtual void affiche(dksOeil* oeil)
    {
        dksVolume::affiche(oeil);
    }

    virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
    {
        dksVolume::affiche_couleurs_luminescentes(oeil);
    }

    //---------------------------------------------------------------
    // Rendu d'un materiau luminescent. Sert pour le rendu des auras
    //---------------------------------------------------------------
    protected:
    virtual void initialise_materiau_luminescent(dksSurface* surface)
    {
        surface_courante=surface;
        GLuint face_materiaux;
        if (surface->luminescence>0.)
        {
            couleur_luminescence[0]=surface->couleur_luminescence[0]*surface->luminescence;
            couleur_luminescence[1]=surface->couleur_luminescence[1]*surface->luminescence;
            couleur_luminescence[2]=surface->couleur_luminescence[2]*surface->luminescence;
            couleur_luminescence[3]=1.-surface->transparence;

            //cout<<"R:"<<ambiance_faces[0]<<" V:"<<ambiance_faces[1]<<" B:"<<ambiance_faces[2]<<endl;


            //Gestion de l'affichage double-face:
            //Attention, les normales ne sont pas recalculées selon l'orientation de la face par rapport à l'oeil.

            if(surface->double_face)
            {
                glDisable(GL_CULL_FACE);
                face_materiaux=GL_FRONT_AND_BACK;
                //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
            }
            else
            {
                glEnable(GL_CULL_FACE); //Active la distinction face avant/face arrière des polygones pour le rendu.
                face_materiaux=GL_FRONT;
                //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
            }
            glColor3fv(couleur_luminescence);
            //glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        }

        //Surface opaque (sert juste à masquer les surfaces lumineuses)
        else
        {
            if(surface->double_face) glDisable(GL_CULL_FACE);
            else glEnable(GL_CULL_FACE); //Active la distinction face avant/face arrière des polygones pour le rendu.
            glColor4fv(noir);
            //glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        }


        //Gestion des tableaux:
        //glDisableClientState(GL_NORMAL_ARRAY); //Déjà désactivé par rendu_surfaces_luminescentes()

        //Gestion des textures et shaders:
        desactive_textures();
        glUseProgram(dksShaders::bibliotheque->decallage_sommets->programme_id);
    }

    //----------------------------------------------------------------------------------------
    //          Rendu des surfaces opaques avec l'utilisation des tableaux:
    //----------------------------------------------------------------------------------------

        virtual void rendu_opaques_mode_tableaux()
        {
            uint32_t i,j;
            dksSurface* surface;
            dksFace* face;
            Particule_Face_Separee* particule;

            //************** Surfaces opaques:
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);


            for (i=0;i<nbrSurfaces;i++)
            {
                surface=surfaces[i];
                if (surface->drapeau_transparence) continue;

                initialise_materiau(surface);

                //============================= Rendu avec textures:

                //============================= Rendu sans textures:

                //else //if (mode_remplissage==DKSVOLUME_REMPLISSAGE_COULEUR)
                //{
                    //------------------- Surface adoucie:
                    if(surface->drapeau_adoucissement)
                    {
                        for(j=0;j<surface->nbr_faces;j++)
                        {
                            face=surface->faces[j];
                            particule=(Particule_Face_Separee*)particules[face->indice];
                            glUniform3fv(dksShaders::bibliotheque->decallage_sommets->deplacement,1,particule->deplacement_centre_gravite);
                            glUniform1f(dksShaders::bibliotheque->decallage_sommets->niveau_eclairage,1.);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->centre_gravite_face,&points[3*j]);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->tangente_face,face->tangente);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->normale_face,face->normale);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->binormale_face,face->binormale);
                            glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                        }
                    }

                    //------------------- Surface en aplats;
                    else
                    {
                        for(j=0;j<surface->nbr_faces;j++)
                        {
                            face=surface->faces[j];
                            particule=(Particule_Face_Separee*)particules[face->indice];
                            glNormal3dv(face->normale);
                            glUniform3fv(dksShaders::bibliotheque->decallage_sommets->deplacement,1,particule->deplacement_centre_gravite);
                            glUniform1f(dksShaders::bibliotheque->decallage_sommets->niveau_eclairage,1.);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->centre_gravite_face,&points[3*j]);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->tangente_face,face->tangente);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->normale_face,face->normale);
                            glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->binormale_face,face->binormale);
                            glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                        }
                    }
                //}

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
            dksFace* face;
            GLdouble* sommet;
            GLdouble* nml;
            Particule_Face_Separee* particule;

            duree_rendu=glutGet(GLUT_ELAPSED_TIME);

            //**************** Surfaces opaques:
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            for (i=0;i<nbrSurfaces;i++)
            {
                surface_courante=surfaces[i];
                //if(surface->luminescence>0.)
                //{
                    initialise_materiau_luminescent(surface_courante);

                    //========================= Rendu à plat sans texture:

                    for (j=0;j<surface_courante->nbr_faces;j++)
                    {
                        dksFace* face=surface_courante->faces[j];
                        particule=(Particule_Face_Separee*)particules[face->indice];
                        glUniform3fv(dksShaders::bibliotheque->decallage_sommets->deplacement,1,particule->deplacement_centre_gravite);
                        glUniform1f(dksShaders::bibliotheque->decallage_sommets->niveau_eclairage,particule->niveau_eclairage);
                        glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->centre_gravite_face,&points[3*j]);
                        glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->tangente_face,face->tangente);
                        glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->normale_face,face->normale);
                        glVertexAttrib3dv(dksShaders::bibliotheque->decallage_sommets->binormale_face,face->binormale);
                        glDrawElements(GL_POLYGON,face->nbrSommets,GL_UNSIGNED_INT,face->aretes);
                    }
                //}
            }
        }
//-----------------------------------------------------------------------------
//      Initialise les pointeurs sur les fonctions d'initialisation des rendus
//-----------------------------------------------------------------------------
        void initialise_pointeurs_init_rendus_faces_separees()
        {
            //---- Opaques:
            //init_rendus[DKSSURFACE_RENDU_COULEUR]=&dksVolume::VolumeFacesSeparees::init_rendu_opaque_couleur_faces_separees;
            //init_rendus[DKSSURFACE_RENDU_TEXTURE_COULEUR]=&dksVolume::init_rendu_opaque_texture_couleur_faces_separees;

            /*
            init_rendus_opaques[DKSSURFACE_RENDU_TEXTURE_RUGOSITE]=&dksVolume::init_rendu_opaque_texture_rugosite;
            init_rendus_opaques[DKSSURFACE_RENDU_TEXTURE_COULEUR_RUGOSITE]=&dksVolume::init_rendu_opaque_texture_couleur_rugosite;

            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_COULEUR]=&dksVolume::init_rendu_opaque_reflexion_couleur;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_COULEUR]=&dksVolume::init_rendu_opaque_reflexion_texture_couleur;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_RUGOSITE]=&dksVolume::init_rendu_opaque_reflexion_texture_rugosite;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_COULEUR_RUGOSITE]=&dksVolume::init_rendu_opaque_reflexion_texture_couleur_rugosite;

            init_rendus_opaques[DKSSURFACE_RENDU_COULEUR_OMBRES]=&dksVolume::init_rendu_opaque_couleur_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_TEXTURE_COULEUR_OMBRES]=&dksVolume::init_rendu_opaque_texture_couleur_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_TEXTURE_RUGOSITE_OMBRES]=&dksVolume::init_rendu_opaque_texture_rugosite_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_TEXTURE_COULEUR_RUGOSITE_OMBRES]=&dksVolume::init_rendu_opaque_texture_couleur_rugosite_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_COULEUR_OMBRES]=&dksVolume::init_rendu_opaque_reflexion_couleur_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_COULEUR_OMBRES]=&dksVolume::init_rendu_opaque_reflexion_texture_couleur_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_RUGOSITE_OMBRES]=&dksVolume::init_rendu_opaque_reflexion_texture_rugosite_ombres;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_TEXTURE_COULEUR_RUGOSITE_OMBRES]=&dksVolume::init_rendu_opaque_reflexion_texture_couleur_rugosite_ombres;

            init_rendus_opaques[DKSSURFACE_RENDU_COULEUR_ONDES]=&dksVolume::init_rendu_opaque_ondes;
            init_rendus_opaques[DKSSURFACE_RENDU_REFLEXION_COULEUR_ONDES]=&dksVolume::init_rendu_opaque_reflexion_couleur_ondes;
            */
        }

//=============================================================================
//      Fonctions d'initialisation des rendus.
//      Ces fonctions se chargent de configurer OpenGL comme il faut.
//=============================================================================


    virtual void init_rendu_couleur()
    {
        glUseProgram(dksShaders::bibliotheque->decallage_sommets->programme_id);
    }

    //---------------------------------------------
    //      Rendu avec une texture de couleur
    //---------------------------------------------
    virtual void init_rendu_texture_couleur()
    {
        dksTexture* texture;
        surface_courante->mode_rendu=DKSSURFACE_RENDU_TEXTURE_COULEUR;
        texture=surface_courante->textures_couleur[0];
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texture->image->texture_id);
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,couleur_diffusion);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,texture->recouvrement_s);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,texture->recouvrement_t);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,texture->type_grossissement);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,texture->type_diminution);
        //Mode de combinaisons des fragments:
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        //Gestion des tableaux:
        if (mode_rendu==DKSVOLUME_MODE_TABLEAUX || mode_rendu==DKSVOLUME_MODE_LISTE_OPAQUES_TABLEAUX_TRANSPARENTES)
        {
            glClientActiveTexture(GL_TEXTURE0);
            glTexCoordPointer(2,GL_FLOAT,0,texture->coordonnees_UV_tableau);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        glUseProgram(dksShaders::bibliotheque->decallage_sommets->programme_id);
    }



};

#endif // VOLUMEFACESSEPAREES_H_INCLUDED
