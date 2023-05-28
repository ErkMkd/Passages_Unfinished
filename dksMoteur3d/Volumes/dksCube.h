#ifndef DKSCUBE_H_INCLUDED
#define DKSCUBE_H_INCLUDED

//#include "../Elements_Fondamentaux/dksVolume.h"

class dksCube : public dksVolume
{
    public:
    double sommets_cube[8*3];
    dksSurface* surfaces_cube[6];
    dksFace* faces_cube[6];
    uint32_t aretes_faces_cube[6][4];

    private:
        //Les textures d'arrière-plan:
        dksTexture* texture_avant;
        dksTexture* texture_arriere;
        dksTexture* texture_droite;
        dksTexture* texture_gauche;
        dksTexture* texture_haut;
        dksTexture* texture_bas;

        float reflexion;
        float luminescence;
        float intensite_ondes;
        float perturbation_ondes;


        //========================================
        //              Constructeur
        //========================================
    public:
        dksCube(char* p_nom=NULL,
                double px = 0., double py = 0., double pz = 0.,
                double tx=1., double ty=1., double tz=1.,
                uint32_t p_couleur=0xffff4040,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                ):dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
                {
                    uint16_t i;

                    sens_polygones=GL_CCW;

                    surfaces_cube={NULL,NULL,NULL,NULL,NULL,NULL};
                    faces_cube={NULL,NULL,NULL,NULL,NULL,NULL};
                    texture_avant=NULL;
                    texture_arriere=NULL;
                    texture_droite=NULL;
                    texture_gauche=NULL;
                    texture_haut=NULL;
                    texture_bas=NULL;

                    sommets_cube={
                                -.5*tx,.5*ty,.5*tz,
                                -.5*tx,-.5*ty,.5*tz,
                                .5*tx,-.5*ty,.5*tz,
                                .5*tx,.5*ty,.5*tz,
                                .5*tx,.5*ty,-.5*tz,
                                .5*tx,-.5*ty,-.5*tz,
                                -.5*tx,-.5*ty,-.5*tz,
                                -.5*tx,.5*ty,-.5*tz
                                };

                    aretes_faces_cube={{2,3,0,1},{5,4,3,2},{6,7,4,5},{1,0,7,6},{3,4,7,0},{5,2,1,6}}; ///Ne pas changer l'ordre des sommets, à cause des UV d'arrière-plan

                    determine_sommets(sommets_cube,8);

                    faces_cube[0]=new dksFace(4,aretes_faces_cube[0],"surface_1",DKSFACE_LIBERE_RIEN);
                    faces_cube[1]=new dksFace(4,aretes_faces_cube[1],"surface_2",DKSFACE_LIBERE_RIEN);
                    faces_cube[2]=new dksFace(4,aretes_faces_cube[2],"surface_3",DKSFACE_LIBERE_RIEN);
                    faces_cube[3]=new dksFace(4,aretes_faces_cube[3],"surface_4",DKSFACE_LIBERE_RIEN);
                    faces_cube[4]=new dksFace(4,aretes_faces_cube[4],"surface_5",DKSFACE_LIBERE_RIEN);
                    faces_cube[5]=new dksFace(4,aretes_faces_cube[5],"surface_6",DKSFACE_LIBERE_RIEN);

                    determine_faces(faces_cube,6);

                    surfaces_cube[0]=new dksSurface("surface_1");
                    surfaces_cube[1]=new dksSurface("surface_2");
                    surfaces_cube[2]=new dksSurface("surface_3");
                    surfaces_cube[3]=new dksSurface("surface_4");
                    surfaces_cube[4]=new dksSurface("surface_5");
                    surfaces_cube[5]=new dksSurface("surface_6");

                    for(i=0;i<6;i++)surfaces_cube[i]->determine_couleur_diffusion(p_couleur);

                    determine_surfaces(surfaces_cube,6);
                    maj_listes();

                    //tableau_faces[0]=new dksFace(4,)

                    reflexion=0.;
                    intensite_ondes=0.;
                    perturbation_ondes=5.;

                }

        //==============================================
        //              Constructeur de copie
        //==============================================


        //========================================
        //              Destructeur
        //========================================

        ~dksCube()
        {
            uint16_t i;
            for(i=0;i<6;i++)
            {
                if (faces_cube[i]!=NULL) delete (faces_cube[i]);
                if (surfaces_cube[i]!=NULL) delete surfaces_cube[i];
            }
            ///Pas besoin de détruire les textures, les destructeurs des surfaces s'en chargent.
            /*
            if (texture_avant!=NULL) delete texture_avant;
            if (texture_arriere!=NULL) delete texture_arriere;
            if (texture_droite!=NULL) delete texture_droite;
            if (texture_gauche!=NULL) delete texture_gauche;
            if (texture_haut!=NULL) delete texture_haut;
            if (texture_bas!=NULL) delete texture_bas;
            */
        }

        //========================================
        //              Fonctions diverses
        //========================================


        void determine_couleur_diffusion(uint32_t p_couleur)
        {
            for(uint16_t i=0;i<6;i++)surfaces_cube[i]->determine_couleur_diffusion(p_couleur);
        }

        void determine_luminescence(float p=1.)
        {
            luminescence=p;
            for(uint16_t i=0;i<6;i++)surfaces_cube[i]->determine_luminescence(p);
            maj_liste_luminescences();
        }

        float renvoie_luminescence()
        {
            return luminescence;
        }

        void determine_reflexion(float p)
        {
            reflexion=p;
            for (uint16_t i=0;i<6;i++) surfaces_cube[i]->determine_reflexion(p);
            initialise_modes_rendus_surfaces();

        }

        float renvoie_reflexion()
        {
            return reflexion;
        }


        void determine_mode_rendu_surfaces(uint8_t rendu_id)
        {
            for (uint16_t i=0;i<6;i++) surfaces_cube[i]->mode_rendu=rendu_id;
        }

        void determine_rendu_transition_surfaces(bool p_drapeau)
        {
            for (uint16_t i=0;i<6;i++) surfaces_cube[i]->determine_transition(p_drapeau);
        }

        void determine_niveau_transition_textures_couleur(float niveau)
        {
            for (uint16_t i=0;i<6;i++) surfaces_cube[i]->niveau_transition=niveau;
        }

        void determine_indices_textures_transition(uint32_t indice_texture_depart, uint32_t indice_texture_destination,uint32_t indice_texture_transition)
        {
            for (uint16_t i=0;i<6;i++)
            {
                surfaces_cube[i]->indice_texture_depart=indice_texture_depart;
                surfaces_cube[i]->indice_texture_destination=indice_texture_destination;
                surfaces_cube[i]->indice_texture_transition=indice_texture_transition;
            }
        }

        //----------------------------------
        //      Gestion des ondes aqueuses
        //----------------------------------
        void determine_intensite_ondes(float p_intensite)
        {
            intensite_ondes=p_intensite;
            for(uint16_t i=0;i<6;i++)surfaces_cube[i]->determine_intensite_ondes(p_intensite);
            initialise_modes_rendus_surfaces();
        }

        float renvoie_intensite_ondes()
        {
            return intensite_ondes;
        }

        void determine_perturbation_ondes(float p_intensite)
        {
            perturbation_ondes=p_intensite;
            for(uint16_t i=0;i<6;i++)surfaces_cube[i]->perturbation_ondes=p_intensite;
        }

        //==============================================================================================
        //      Gestion des textures d'arrière-plan.
        //  Sert lorsque le cube est utilisé comme arrière-plan.
        /// Il faut veiller à inverser le sens des polygones avec
        /// "inverse_sens_polygones()" pour que le cube soit vu de l'intérieur.
        //==============================================================================================
        bool ajoute_textures_arriere_plan(Image* p_avant, Image* p_arriere, Image* p_droite, Image* p_gauche, Image* p_haut, Image* p_bas)
        {
            float uvs[4*2]={1,1, 1,0, 0,0, 0,1};
            texture_avant=new dksTexture("Texture_avant",p_avant,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[0]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
            texture_gauche=new dksTexture("Texture_gauche",p_gauche,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[1]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
            texture_arriere=new dksTexture("Texture_arriere",p_arriere,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[2]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
            texture_droite=new dksTexture("Texture_droite",p_droite,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[3]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
            texture_haut=new dksTexture("Texture_haut",p_haut,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[4]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);
            texture_bas=new dksTexture("Texture_bas",p_bas,DKSTEXTURE_CANAL_COULEUR,uvs,faces_cube[5]->aretes,4,false,GL_MIRRORED_REPEAT,GL_MIRRORED_REPEAT,GL_LINEAR,GL_LINEAR);



            surfaces[0]->ajoute_texture(texture_avant);
            surfaces[1]->ajoute_texture(texture_gauche);
            surfaces[2]->ajoute_texture(texture_arriere);
            surfaces[3]->ajoute_texture(texture_droite);
            surfaces[4]->ajoute_texture(texture_haut);
            surfaces[5]->ajoute_texture(texture_bas);

            if(!initialise_textures())return false;
            initialise_modes_rendus_surfaces();
            return true;
        }

        dksSurface* renvoie_surface_avant()
        {
            return surfaces[0];
        }
        dksSurface* renvoie_surface_gauche()
        {
            return surfaces[1];
        }
        dksSurface* renvoie_surface_arriere()
        {
            return surfaces[2];
        }
        dksSurface* renvoie_surface_droite()
        {
            return surfaces[3];
        }
        dksSurface* renvoie_surface_haut()
        {
            return surfaces[4];
        }
        dksSurface* renvoie_surface_bas()
        {
            return surfaces[5];
        }
};

#endif // DKSCUBE_H_INCLUDED
