#ifndef DKSSHADERS_H_INCLUDED
#define DKSSHADERS_H_INCLUDED

#include "../../Utiles/Gestion_Shaders.h"
#include "../../Utiles/Bruit.h"

/*
//*******************************************************************

                    Gestion des shaders au sein du moteur 3d

//*******************************************************************
*/

//------------- Compatibilité:
#define DKSSHADERS_FABRIQUANT_ATI 1
#define DKSSHADERS_FABRIQUANT_NVIDIA 2

//------------ Codes d'erreur:

#define DKSSHADERS_DEBUG true

#define DKSSHADERS_OK 0
#define DKSSHADERS_ERREUR_SOURCE 1
#define DKSSHADERS_ERREUR_OBJETS 2
#define DKSSHADERS_ERREUR_COMPILATION 3
#define DKSSHADERS_ERREUR_PROGRAMME 4
#define DKSSHADERS_ERREUR_LIAISON 5
#define DKSSHADERS_ERREUR_VALIDATION 6
#define DKSSHADERS_ERREUR_OPENGL 7
#define DKSSHADERS_ERREUR_FABRICANT 8


class dksInitialiseShaders: public Gestion_Shaders
{
    public:
        uint16_t erreur;
        uint8_t id_fabriquant;  //La marque de la carte graphique

        //Rendu des éclairages simples:
        GLuint programme_eclairage;
        GLint eclairage_drapeaux_sources;

        //Rendu des éclairges en Phong:
        GLuint programme_phong;
        GLint phong_drapeaux_sources;

        // Rendu avec texture de couleur + texture de rugosité:
        GLuint programme_rugosite_couleur;
        GLint rugosite_couleur_texture_couleur; //Variables "uniform"
        GLint rugosite_couleur_texture_rugosite;
        GLint rugosite_couleur_tangente;        //Variables "attribute"
        GLint rugosite_couleur_binormale;

        // Rendu avec texture de rugosité seule:
        GLuint programme_rugosite;
        GLint rugosite_texture_rugosite;
        GLint rugosite_tangente;        //Variables "attribute"
        GLint rugosite_binormale;

        // Floutage de texture:
        GLuint programme_flou_x;
        GLint flou_x_taille;
        GLint flou_x_saturation;
        GLint flou_x_texture;

        GLuint programme_flou_y;
        GLint flou_y_taille;
        GLint flou_y_saturation;
        GLint flou_y_texture;

        //Ombres:
        GLuint programme_ombres;
        GLint ombres_drapeaux_sources;
        GLint ombres_texture_profondeur;
        GLint ombres_matrice_projection_vision_source;
        GLint ombres_composantes_convertion;
        GLint ombres_matrice_vision_source;
        GLint ombres_matrice_source_uv;
        GLint ombres_seuil;
        GLint ombres_intensite;

        //Phong + Ombres:
        GLuint programme_phong_ombres;
        GLint phong_ombres_drapeaux_sources;
        GLint phong_ombres_texture_profondeur;
        GLint phong_ombres_matrice_projection_vision_source;
        GLint phong_ombres_composantes_convertion;
        GLint phong_ombres_matrice_vision_source;
        GLint phong_ombres_matrice_source_uv;
        GLint phong_ombres_seuil;
        GLint phong_ombres_intensite;

        // Décallage de sommets:
        GLuint programme_decallage_sommets;
        GLint decallage_sommets_tangente_face;  //Variables "attribute"
        GLint decallage_sommets_normale_face;
        GLint decallage_sommets_binormale_face;
        GLint decallage_sommets_centre_gravite_face;
        GLint decallage_sommets_deplacement;   //Variables "uniform"
        GLint decallage_sommets_niveau_eclairage;

        //Réflexion du cube d'arrière-plan sans texture:
        GLuint programme_reflexion_couleur_cube;
        GLint   reflexion_couleur_cube_matrice_volume;
        GLint   reflexion_couleur_cube_texture;
        GLint   reflexion_couleur_cube_position_oeil;
        GLint   reflexion_couleur_cube_drapeaux_sources;
        GLint   reflexion_couleur_cube_niveau;

        //Rendu des ondes sans textures:
        #define TAILLE_ONDES_TEXTURE_BRUIT 256
        GLuint programme_ondes;
        GLint ondes_drapeaux_sources;

        GLint ondes_quantite;
        GLint ondes_frequence;
        GLint ondes_amplitude;
        GLint ondes_niveau_perturbations;
        GLint ondes_echelle_texture_bruit;
        GLint ondes_texture_bruit;
        GLuint ondes_texture_bruit_id;

        GLint ondes_intensite;
        GLint ondes_vitesse;
        GLint ondes_position_temps;
        GLint ondes_centre_gravite_face;
        GLint ondes_tangente;
        GLint ondes_binormale;

        //Rendu des ondes sans textures avec réflexion du cube d'arrière-plan:
        GLuint programme_reflexion_couleur_cube_ondes;
        GLint reflexion_couleur_cube_ondes_drapeaux_sources;

        GLint   reflexion_couleur_cube_ondes_matrice_volume;
        GLint   reflexion_couleur_cube_ondes_texture_cube;
        GLint   reflexion_couleur_cube_ondes_position_oeil;
        GLint   reflexion_couleur_cube_ondes_niveau_reflexion;

        GLint reflexion_couleur_cube_ondes_quantite;
        GLint reflexion_couleur_cube_ondes_frequence;
        GLint reflexion_couleur_cube_ondes_amplitude;
        GLint reflexion_couleur_cube_ondes_niveau_perturbations;
        GLint reflexion_couleur_cube_ondes_echelle_texture_bruit;
        GLint reflexion_couleur_cube_ondes_texture_bruit;

        GLint reflexion_couleur_cube_ondes_intensite_ondes;
        GLint reflexion_couleur_cube_ondes_vitesse;
        GLint reflexion_couleur_cube_ondes_position_temps;
        GLint reflexion_couleur_cube_ondes_centre_gravite_face;
        GLint reflexion_couleur_cube_ondes_tangente;
        GLint reflexion_couleur_cube_ondes_binormale;

        // Fondu entre deux textures de couleur, avec une troisième qui sert de facteur de transition
        //La texture de transformation doit être en niveaux de gris.
        GLuint programme_transition_textures_couleur;
        GLint transition_textures_couleur_texture_depart; //La texture de départ
        GLint transition_textures_couleur_texture_destination; //La texture de destination
        GLint transition_textures_couleur_texture_transition; //La texture de transition.
        GLint transition_textures_couleur_niveau;

         // Fondu entre deux textures de couleur, le long de l'axe Z du volume
        GLuint programme_transition_textures_couleur_axe;
        GLint transition_textures_couleur_axe_texture_depart; //La texture de départ
        GLint transition_textures_couleur_axe_texture_destination; //La texture de destination
        GLint transition_textures_couleur_axe_zmin;
        GLint transition_textures_couleur_axe_zmax;
        GLint transition_textures_couleur_axe_niveau;


    private:
        char chemin_sources[64];
        char nom_fichier[128];
        //Eclairage:
        char *source_eclairage_vertex;
        char *source_eclairage_fragment;
        GLuint eclairage_vertex_shader_id;
        GLuint eclairage_fragment_shader_id;

        //Eclairage Phong:
        char *source_phong_vertex;
        char *source_phong_fragment;
        GLuint phong_vertex_shader_id;
        GLuint phong_fragment_shader_id;

        //Rugosité + couleur:
        char* source_rugosite_couleur_vertex;
        char* source_rugosite_couleur_fragment;

        GLuint rugosite_couleur_vertex_shader_id;
        GLuint rugosite_couleur_fragment_shader_id;

        //Rugosité seule:
        char* source_rugosite_vertex;
        char* source_rugosite_fragment;

        GLuint rugosite_vertex_shader_id;
        GLuint rugosite_fragment_shader_id;

        //Floutage:
        char* source_flou_x;
        char* source_flou_y;

        GLuint flou_x_shader_id;
        GLuint flou_y_shader_id;

        //Ombres:
        char* source_ombres_vertex;
        char* source_ombres_fragment;
        GLuint ombres_vertex_shader_id;
        GLuint ombres_fragment_shader_id;

        //Phong + Ombres:
        char* source_phong_ombres_vertex;
        char* source_phong_ombres_fragment;
        GLuint phong_ombres_vertex_shader_id;
        GLuint phong_ombres_fragment_shader_id;

        //Décallage de sommets:
        char* source_decallage_sommets_vertex;
        char* source_decallage_sommets_fragment;

        GLuint decallage_sommets_vertex_shader_id;
        GLuint decallage_sommets_fragment_shader_id;

        //Réflexion du cube d'arrière-plan:
        char* source_reflexion_couleur_cube_vertex;
        char* source_reflexion_couleur_cube_fragment;

        GLuint reflexion_couleur_cube_vertex_shader_id;
        GLuint reflexion_couleur_cube_fragment_shader_id;

        //Ondes aqueuses:
        char *source_ondes_vertex;
        char *source_ondes_fragment;
        GLuint ondes_vertex_shader_id;
        GLuint ondes_fragment_shader_id;
        uint8_t ondes_tampon_texture_bruit[TAILLE_ONDES_TEXTURE_BRUIT*TAILLE_ONDES_TEXTURE_BRUIT];

        //Réflexion du cube d'arrière-plan et ondes aqueuses:
        char *source_reflexion_couleur_cube_ondes_vertex;
        char *source_reflexion_couleur_cube_ondes_fragment;
        GLuint reflexion_couleur_cube_ondes_vertex_shader_id;
        GLuint reflexion_couleur_cube_ondes_fragment_shader_id;

        //Transition entre deux textures de couleur:
        char *source_transition_textures_couleur_vertex;
        char *source_transition_textures_couleur_fragment;
        GLuint transition_textures_couleur_vertex_shader_id;
        GLuint transition_textures_couleur_fragment_shader_id;

        //Transition entre deux textures de couleur le long de l'axe Z:
        char *source_transition_textures_couleur_axe_vertex;
        char *source_transition_textures_couleur_axe_fragment;
        GLuint transition_textures_couleur_axe_vertex_shader_id;
        GLuint transition_textures_couleur_axe_fragment_shader_id;


        //=================================================================
        //                          Constructeur
        //=================================================================

    public:
        dksInitialiseShaders(uint8_t p_id_fabriquant)
        {
            erreur=DKSSHADERS_OK;

            id_fabriquant=p_id_fabriquant;
            if (id_fabriquant==DKSSHADERS_FABRIQUANT_ATI) strcpy(chemin_sources,"sources_shaders/ATI/");
            else if (id_fabriquant==DKSSHADERS_FABRIQUANT_NVIDIA) strcpy(chemin_sources,"sources_shaders/NVIDIA/");
            else
            {
                erreur=DKSSHADERS_ERREUR_FABRICANT;
                cout<<"ERREUR dans dksInitialiseShaders::dksInitialiseShaders() - Carte graphique inconnue!"<<endl;
                return;
            }

            source_eclairage_vertex=NULL;
            source_eclairage_fragment=NULL;
            eclairage_vertex_shader_id=0;
            eclairage_fragment_shader_id=0;
            programme_eclairage=0;

            source_phong_vertex=NULL;
            source_phong_fragment=NULL;
            phong_vertex_shader_id=0;
            phong_fragment_shader_id=0;
            programme_phong=0;

            source_rugosite_couleur_fragment=NULL;
            source_rugosite_couleur_vertex=NULL;
            rugosite_couleur_vertex_shader_id=0;
            rugosite_couleur_fragment_shader_id=0;
            programme_rugosite_couleur=0;

            source_rugosite_fragment=NULL;
            source_rugosite_vertex=NULL;
            rugosite_vertex_shader_id=0;
            rugosite_fragment_shader_id=0;
            programme_rugosite=0;

            source_flou_x=NULL;
            flou_x_shader_id=0;
            programme_flou_x=0;

            source_flou_y=NULL;
            flou_y_shader_id=0;
            programme_flou_y=0;

            source_ombres_vertex=NULL;
            source_ombres_fragment=NULL;
            programme_ombres=0;
            ombres_vertex_shader_id=0;
            ombres_fragment_shader_id=0;

            source_phong_ombres_vertex=NULL;
            source_phong_ombres_fragment=NULL;
            programme_phong_ombres=0;
            phong_ombres_vertex_shader_id=0;
            phong_ombres_fragment_shader_id=0;

            source_decallage_sommets_fragment=NULL;
            source_decallage_sommets_vertex=NULL;
            decallage_sommets_fragment_shader_id=0;
            decallage_sommets_vertex_shader_id=0;
            programme_decallage_sommets=0;

            source_reflexion_couleur_cube_vertex=NULL;
            source_reflexion_couleur_cube_fragment=NULL;
            programme_reflexion_couleur_cube=0;
            reflexion_couleur_cube_vertex_shader_id=0;
            reflexion_couleur_cube_fragment_shader_id=0;

            source_ondes_vertex=NULL;
            source_ondes_fragment=NULL;
            ondes_vertex_shader_id=0;
            ondes_fragment_shader_id=0;
            programme_ondes=0;
            ondes_texture_bruit_id=0;

            source_reflexion_couleur_cube_ondes_vertex=NULL;
            source_reflexion_couleur_cube_ondes_fragment=NULL;
            reflexion_couleur_cube_ondes_vertex_shader_id=0;
            reflexion_couleur_cube_ondes_fragment_shader_id=0;
            programme_reflexion_couleur_cube_ondes=0;

            source_transition_textures_couleur_vertex=NULL;
            source_transition_textures_couleur_fragment=NULL;
            transition_textures_couleur_vertex_shader_id=0;
            transition_textures_couleur_fragment_shader_id=0;

            source_transition_textures_couleur_axe_vertex=NULL;
            source_transition_textures_couleur_axe_fragment=NULL;
            transition_textures_couleur_axe_vertex_shader_id=0;
            transition_textures_couleur_axe_fragment_shader_id=0;

            if (!creation_eclairage()) return;
            if (!creation_eclairage_phong()) return;
            if (!creation_rugosite_couleur()) return;
            if (!creation_rugosite()) return;
            if (!creation_floutage()) return;
            if (!creation_ombres()) return;
            if (!creation_phong_ombres()) return;
            if (!creation_decallage_sommets()) return;
            if (!creation_reflexion_couleur_cube()) return;
            if (!creation_ondes()) return;
            if (!creation_reflexion_couleur_cube_ondes()) return;
            if (!creation_transition_textures_couleur()) return;
            if (!creation_transition_textures_couleur_axe()) return;
        }

        //=============================================================================
        //                                  Destructeur
        //=============================================================================

        ~dksInitialiseShaders()
        {
            cout<<"DESTRUCTION DES SHADERS DU MOTEUR"<<endl;

            if (programme_eclairage!=0) glDeleteProgram(programme_eclairage);
            if(eclairage_fragment_shader_id!=0) glDeleteShader(eclairage_fragment_shader_id);
            if(eclairage_vertex_shader_id!=0) glDeleteShader(eclairage_vertex_shader_id);
            if (source_eclairage_vertex!=NULL) free(source_eclairage_vertex);
            if (source_eclairage_fragment!=NULL) free(source_eclairage_fragment);

            if (programme_phong!=0) glDeleteProgram(programme_phong);
            if(phong_fragment_shader_id!=0) glDeleteShader(phong_fragment_shader_id);
            if(phong_vertex_shader_id!=0) glDeleteShader(phong_vertex_shader_id);
            if (source_phong_vertex!=NULL) free(source_phong_vertex);
            if (source_phong_fragment!=NULL) free(source_phong_fragment);

            if(programme_rugosite_couleur!=0) glDeleteProgram(programme_rugosite_couleur);
            if(rugosite_couleur_vertex_shader_id!=0) glDeleteShader(rugosite_couleur_vertex_shader_id);
            if(rugosite_couleur_fragment_shader_id!=0) glDeleteShader(rugosite_couleur_fragment_shader_id);
            if (source_rugosite_couleur_vertex!=NULL) free(source_rugosite_couleur_vertex);
            if (source_rugosite_couleur_fragment!=NULL) free(source_rugosite_couleur_fragment);

            if(programme_rugosite!=0) glDeleteProgram(programme_rugosite);
            if(rugosite_vertex_shader_id!=0) glDeleteShader(rugosite_vertex_shader_id);
            if(rugosite_fragment_shader_id!=0) glDeleteShader(rugosite_fragment_shader_id);
            if (source_rugosite_vertex!=NULL) free(source_rugosite_vertex);
            if (source_rugosite_fragment!=NULL) free(source_rugosite_fragment);

            if (programme_flou_x!=0) glDeleteProgram(programme_flou_x);
            if(flou_x_shader_id!=0) glDeleteShader(flou_x_shader_id);
            if (source_flou_x!=NULL) free(source_flou_x);

            if (programme_flou_y!=0) glDeleteProgram(programme_flou_y);
            if(flou_y_shader_id!=0) glDeleteShader(flou_y_shader_id);
            if (source_flou_y!=NULL) free(source_flou_y);

            if (programme_decallage_sommets!=0) glDeleteProgram(programme_decallage_sommets);
            if(decallage_sommets_fragment_shader_id!=0) glDeleteShader(decallage_sommets_fragment_shader_id);
            if(decallage_sommets_vertex_shader_id!=0) glDeleteShader(decallage_sommets_vertex_shader_id);
            if (source_decallage_sommets_vertex!=NULL) free(source_decallage_sommets_vertex);
            if (source_decallage_sommets_fragment!=NULL) free(source_decallage_sommets_fragment);

            if (programme_ombres!=0) glDeleteProgram(programme_ombres);
            if (ombres_vertex_shader_id!=0) glDeleteShader(ombres_vertex_shader_id);
            if (ombres_fragment_shader_id!=0) glDeleteShader(ombres_fragment_shader_id);
            if (source_ombres_vertex!=NULL) free(source_ombres_vertex);
            if (source_ombres_fragment!=NULL) free(source_ombres_fragment);

            if (programme_phong_ombres!=0) glDeleteProgram(programme_phong_ombres);
            if (phong_ombres_vertex_shader_id!=0) glDeleteShader(phong_ombres_vertex_shader_id);
            if (phong_ombres_fragment_shader_id!=0) glDeleteShader(phong_ombres_fragment_shader_id);
            if (source_phong_ombres_vertex!=NULL) free(source_phong_ombres_vertex);
            if (source_phong_ombres_fragment!=NULL) free(source_phong_ombres_fragment);

            if (programme_reflexion_couleur_cube!=0) glDeleteProgram(programme_reflexion_couleur_cube);
            if (reflexion_couleur_cube_vertex_shader_id!=0) glDeleteShader(reflexion_couleur_cube_vertex_shader_id);
            if (reflexion_couleur_cube_fragment_shader_id!=0) glDeleteShader(reflexion_couleur_cube_fragment_shader_id);
            if (source_reflexion_couleur_cube_vertex!=NULL) free(source_reflexion_couleur_cube_vertex);
            if (source_reflexion_couleur_cube_fragment!=NULL) free(source_reflexion_couleur_cube_fragment);

            if (programme_ondes!=0) glDeleteProgram(programme_ondes);
            if(ondes_fragment_shader_id!=0) glDeleteShader(ondes_fragment_shader_id);
            if(ondes_vertex_shader_id!=0) glDeleteShader(ondes_vertex_shader_id);
            if (source_ondes_vertex!=NULL) free(source_ondes_vertex);
            if (source_ondes_fragment!=NULL) free(source_ondes_fragment);
            if (glIsTexture(ondes_texture_bruit_id))glDeleteTextures(1,&ondes_texture_bruit_id);

            if(programme_transition_textures_couleur!=0) glDeleteProgram(programme_transition_textures_couleur);
            if(transition_textures_couleur_vertex_shader_id!=0) glDeleteShader(transition_textures_couleur_vertex_shader_id);
            if(transition_textures_couleur_fragment_shader_id!=0) glDeleteShader(transition_textures_couleur_fragment_shader_id);
            if (source_transition_textures_couleur_vertex!=NULL) free(source_transition_textures_couleur_vertex);
            if (source_transition_textures_couleur_fragment!=NULL) free(source_transition_textures_couleur_fragment);

            if(programme_transition_textures_couleur_axe!=0) glDeleteProgram(programme_transition_textures_couleur_axe);
            if(transition_textures_couleur_axe_vertex_shader_id!=0) glDeleteShader(transition_textures_couleur_axe_vertex_shader_id);
            if(transition_textures_couleur_axe_fragment_shader_id!=0) glDeleteShader(transition_textures_couleur_axe_fragment_shader_id);
            if (source_transition_textures_couleur_axe_vertex!=NULL) free(source_transition_textures_couleur_axe_vertex);
            if (source_transition_textures_couleur_axe_fragment!=NULL) free(source_transition_textures_couleur_axe_fragment);

        }

        //==========================================
        //      Gestion des noms de fichier
        //==========================================
        char* renvoie_nom_fichier(char* nom_source)
        {
            strcpy(nom_fichier,chemin_sources);
            strcat(nom_fichier,nom_source);
            return nom_fichier;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //                          des éclairages simples
        //=============================================================================
        bool creation_eclairage()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu des éclairages #############"<<endl;

            source_eclairage_vertex=charge_source_shader(renvoie_nom_fichier("eclairage_base_vertex.txt"));
            source_eclairage_fragment=charge_source_shader(renvoie_nom_fichier("eclairage_base_fragment.txt"));

            if (source_eclairage_vertex==NULL || source_eclairage_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            eclairage_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            eclairage_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (eclairage_vertex_shader_id==0 || eclairage_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rendu des éclairages"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_eclairage_vertex};
            glShaderSource(eclairage_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_eclairage_fragment;
            glShaderSource(eclairage_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(eclairage_vertex_shader_id)
                ||  !compile_shader(eclairage_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu des éclairages...";

            programme_eclairage=glCreateProgram();
            if (programme_eclairage==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu des éclairages"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_eclairage,eclairage_vertex_shader_id);
            glAttachShader(programme_eclairage,eclairage_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des éclairages...";

            if (!relie_programme_shader(programme_eclairage))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu des éclairages"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des éclairages...";
            if (!valide_programme_shader(programme_eclairage))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu des éclairages est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":
            eclairage_drapeaux_sources=glGetUniformLocation(programme_eclairage,"drapeaux_sources");

            if (erreur_openGl("ERREUR dans dksShaders::creation_eclairage() :")) return false;
            else return true;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //                   Des éclairages en Phong (précision au pixel) sans texture
        //=============================================================================
        bool creation_eclairage_phong()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu des éclairages Phong #############"<<endl;

            source_phong_vertex=charge_source_shader(renvoie_nom_fichier("eclairage_phong_vertex.txt"));
            source_phong_fragment=charge_source_shader(renvoie_nom_fichier("eclairage_phong_fragment.txt"));

            if (source_phong_vertex==NULL || source_phong_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            phong_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            phong_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (phong_vertex_shader_id==0 || phong_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rendu des eclairages en Phong"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_phong_vertex};
            glShaderSource(phong_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_phong_fragment;
            glShaderSource(phong_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(phong_vertex_shader_id)
                ||  !compile_shader(phong_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu des éclairages en Phong...";

            programme_phong=glCreateProgram();
            if (programme_phong==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu des éclairages en Phong"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_phong,phong_vertex_shader_id);
            glAttachShader(programme_phong,phong_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des éclairages en Phong...";

            if (!relie_programme_shader(programme_phong))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu des éclairages en Phong"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des éclairages en Phong...";
            if (!valide_programme_shader(programme_phong))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu des éclairages en Phong est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":
            phong_drapeaux_sources=glGetUniformLocation(programme_phong,"drapeaux_sources");

            if (erreur_openGl("ERREUR dans dksShaders::creation_eclairage_phong() :")) return false;
            else return true;
        }


        //=============================================================================
        //                          Création du programme de rendu
        //              avec textures de rugosité + couleur
        //=============================================================================
        bool creation_rugosite_couleur()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de rugosité + couleur #############"<<endl;

            source_rugosite_couleur_vertex=charge_source_shader(renvoie_nom_fichier("rugosite_couleur_vertex.txt"));
            source_rugosite_couleur_fragment=charge_source_shader(renvoie_nom_fichier("rugosite_couleur_fragment.txt"));

            if (source_rugosite_couleur_vertex==NULL || source_rugosite_couleur_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            rugosite_couleur_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            rugosite_couleur_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (rugosite_couleur_vertex_shader_id==0 || rugosite_couleur_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rugosité/couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_rugosite_couleur_vertex};

            glShaderSource(rugosite_couleur_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_rugosite_couleur_fragment;
            glShaderSource(rugosite_couleur_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(rugosite_couleur_vertex_shader_id)
                ||  !compile_shader(rugosite_couleur_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rugosité/couleur...";

            programme_rugosite_couleur=glCreateProgram();
            if (programme_rugosite_couleur==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rugosité/couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_rugosite_couleur,rugosite_couleur_vertex_shader_id);
            glAttachShader(programme_rugosite_couleur,rugosite_couleur_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rugosité/couleur...";

            if (!relie_programme_shader(programme_rugosite_couleur))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rugosité/couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rugosité/couleur...";
            if (!valide_programme_shader(programme_rugosite_couleur))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rugosité/couleur est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            rugosite_couleur_texture_couleur=glGetUniformLocation(programme_rugosite_couleur,"texture_couleur");
            rugosite_couleur_texture_rugosite=glGetUniformLocation(programme_rugosite_couleur,"texture_rugosite");

            //--------- Création des liens avec les variables type "attribute":

            rugosite_couleur_tangente=glGetAttribLocation(programme_rugosite_couleur,"tangente");
            rugosite_couleur_binormale=glGetAttribLocation(programme_rugosite_couleur,"binormale");

            if (erreur_openGl("ERREUR dans dksShaders::creation_rugosite_couleur() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //                          avec texture de rugosité seule
        //=============================================================================
        bool creation_rugosite()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de rugosité #############"<<endl;

            source_rugosite_vertex=charge_source_shader(renvoie_nom_fichier("rugosite_vertex.txt"));
            source_rugosite_fragment=charge_source_shader(renvoie_nom_fichier("rugosite_fragment.txt"));

            if (source_rugosite_vertex==NULL || source_rugosite_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            rugosite_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            rugosite_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (rugosite_vertex_shader_id==0 || rugosite_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rugosité"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_rugosite_vertex};

            glShaderSource(rugosite_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_rugosite_fragment;
            glShaderSource(rugosite_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(rugosite_vertex_shader_id)
                ||  !compile_shader(rugosite_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rugosité...";

            programme_rugosite=glCreateProgram();
            if (programme_rugosite==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rugosité"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_rugosite,rugosite_vertex_shader_id);
            glAttachShader(programme_rugosite,rugosite_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rugosité...";

            if (!relie_programme_shader(programme_rugosite))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rugosité"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rugosité...";
            if (!valide_programme_shader(programme_rugosite))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rugosité est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            rugosite_texture_rugosite=glGetUniformLocation(programme_rugosite,"texture_rugosite");

            //--------- Création des liens avec les variables type "attribute":

            rugosite_tangente=glGetAttribLocation(programme_rugosite,"tangente");
            rugosite_binormale=glGetAttribLocation(programme_rugosite,"binormale");

            if (erreur_openGl("ERREUR dans dksShaders::creation_rugosite() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }


        //=============================================================================
        //                          Création des programmes de floutage
        //=============================================================================
        bool creation_floutage()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de floutage #############"<<endl;
            source_flou_x=charge_source_shader(renvoie_nom_fichier("flou_x.txt"));
            source_flou_y=charge_source_shader(renvoie_nom_fichier("flou_y.txt"));

            if (source_flou_x==NULL || source_flou_y==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            flou_x_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            flou_y_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (flou_x_shader_id==0 || flou_y_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de floutage"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_flou_x};
            glShaderSource(flou_x_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_flou_y;
            glShaderSource(flou_y_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(flou_x_shader_id)
                ||  !compile_shader(flou_y_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création des programmes de floutage...";

            programme_flou_x=glCreateProgram();
            programme_flou_y=glCreateProgram();

            if (programme_flou_x==0 || programme_flou_y==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les programmes de floutage"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_flou_x,flou_x_shader_id);
            glAttachShader(programme_flou_y,flou_y_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison des programme de floutage...";

            if (!relie_programme_shader(programme_flou_x) || !relie_programme_shader(programme_flou_y))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier les programmes de floutage"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation des programmes de floutage...";
            if (!valide_programme_shader(programme_flou_x) || !valide_programme_shader(programme_flou_y))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Les programmes de floutage sont invalides sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            flou_x_texture=glGetUniformLocation(programme_flou_x,"texture");
            flou_x_taille=glGetUniformLocation(programme_flou_x,"taille_flou");
            flou_x_saturation=glGetUniformLocation(programme_flou_x,"saturation_flou");

            flou_y_texture=glGetUniformLocation(programme_flou_y,"texture");
            flou_y_taille=glGetUniformLocation(programme_flou_y,"taille_flou");
            flou_y_saturation=glGetUniformLocation(programme_flou_y,"saturation_flou");

            if (erreur_openGl("ERREUR dans dksShaders::creation_floutage() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }


        //=============================================================================
        //                 Création du programme de rendu des ombres
        //=============================================================================
        bool creation_ombres()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu des ombres #############"<<endl;
            source_ombres_vertex=charge_source_shader(renvoie_nom_fichier("ombres_vertex.txt"));
            source_ombres_fragment=charge_source_shader(renvoie_nom_fichier("ombres_fragment.txt"));

            if (source_ombres_fragment==NULL || source_ombres_vertex==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            ombres_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            ombres_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);

            if (ombres_fragment_shader_id==0 || ombres_vertex_shader_id==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader des rendu des ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_ombres_fragment};
            glShaderSource(ombres_fragment_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_ombres_vertex;
            glShaderSource(ombres_vertex_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if ( (!compile_shader(ombres_fragment_shader_id)) || (!compile_shader(ombres_vertex_shader_id)) )
                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création du programme
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu des ombres ...";

            programme_ombres=glCreateProgram();

            if (programme_ombres==0 )
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les programmes de rendu des ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders au programme:
            glAttachShader(programme_ombres,ombres_vertex_shader_id);
            glAttachShader(programme_ombres,ombres_fragment_shader_id);

            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des ombres...";

            if (!relie_programme_shader(programme_ombres))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu des ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des ombres...";
            if (!valide_programme_shader(programme_ombres) )
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu des ombre est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":

            ombres_drapeaux_sources=glGetUniformLocation(programme_ombres,"drapeaux_sources");
            ombres_texture_profondeur=glGetUniformLocation(programme_ombres,"texture_profondeur");
            ombres_matrice_source_uv=glGetUniformLocation(programme_ombres,"matrice_source_uv");
            ombres_matrice_projection_vision_source=glGetUniformLocation(programme_ombres,"matrice_projection_vision_source");
            ombres_matrice_vision_source=glGetUniformLocation(programme_ombres,"matrice_vision_source");
            ombres_composantes_convertion=glGetUniformLocation(programme_ombres,"composantes_convertion");
            ombres_seuil=glGetUniformLocation(programme_ombres,"seuil_ombres");
            ombres_intensite=glGetUniformLocation(programme_ombres,"intensite_ombres");

            if (erreur_openGl("ERREUR dans dksShaders::creation_ombres() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }

        //========================================================================================
        //                 Création du programme de rendu éclairage phong avec ombres portées
        //========================================================================================
        bool creation_phong_ombres()
        {

            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu phong + ombres #############"<<endl;
            source_phong_ombres_vertex=charge_source_shader(renvoie_nom_fichier("phong_ombres_vertex.txt"));
            source_phong_ombres_fragment=charge_source_shader(renvoie_nom_fichier("phong_ombres_fragment.txt"));

            if (source_phong_ombres_fragment==NULL || source_phong_ombres_vertex==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            phong_ombres_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            phong_ombres_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);

            if (phong_ombres_fragment_shader_id==0 || phong_ombres_vertex_shader_id==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader des rendu phong + ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_phong_ombres_fragment};
            glShaderSource(phong_ombres_fragment_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_phong_ombres_vertex;
            glShaderSource(phong_ombres_vertex_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if ( (!compile_shader(phong_ombres_fragment_shader_id)) || (!compile_shader(phong_ombres_vertex_shader_id)) )
                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création du programme
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu phong + ombres ...";

            programme_phong_ombres=glCreateProgram();

            if (programme_phong_ombres==0 )
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les programmes de rendu phong + ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders au programme:
            glAttachShader(programme_phong_ombres,phong_ombres_vertex_shader_id);
            glAttachShader(programme_phong_ombres,phong_ombres_fragment_shader_id);

            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu phong + ombres...";

            if (!relie_programme_shader(programme_phong_ombres))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu phong + ombres"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu phong + ombres...";
            if (!valide_programme_shader(programme_phong_ombres) )
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu phong + ombres est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":

            phong_ombres_drapeaux_sources=glGetUniformLocation(programme_ombres,"drapeaux_sources");
            phong_ombres_texture_profondeur=glGetUniformLocation(programme_ombres,"texture_profondeur");
            phong_ombres_matrice_source_uv=glGetUniformLocation(programme_ombres,"matrice_source_uv");
            phong_ombres_matrice_projection_vision_source=glGetUniformLocation(programme_ombres,"matrice_projection_vision_source");
            phong_ombres_matrice_vision_source=glGetUniformLocation(programme_ombres,"matrice_vision_source");
            phong_ombres_composantes_convertion=glGetUniformLocation(programme_ombres,"composantes_convertion");
            phong_ombres_seuil=glGetUniformLocation(programme_ombres,"seuil_ombres");
            phong_ombres_intensite=glGetUniformLocation(programme_ombres,"intensite_ombres");

            if (erreur_openGl("ERREUR dans dksShaders::creation_phong_ombres() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }



        //=============================================================================
        //                          Création du programme de rendu
        //                          avec décallage des sommets
        //=============================================================================
        bool creation_decallage_sommets()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu avec décallage de sommets #############"<<endl;

            source_decallage_sommets_vertex=charge_source_shader(renvoie_nom_fichier("decallage_sommets_vertex.txt"));
            source_decallage_sommets_fragment=charge_source_shader(renvoie_nom_fichier("decallage_sommets_fragment.txt"));

            if (source_decallage_sommets_vertex==NULL || source_decallage_sommets_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            decallage_sommets_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            decallage_sommets_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (decallage_sommets_vertex_shader_id==0 || decallage_sommets_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de décallage de sommets"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_decallage_sommets_vertex};

            glShaderSource(decallage_sommets_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_decallage_sommets_fragment;
            glShaderSource(decallage_sommets_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(decallage_sommets_vertex_shader_id)
                ||  !compile_shader(decallage_sommets_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de décallage de sommets...";

            programme_decallage_sommets=glCreateProgram();
            if (programme_decallage_sommets==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de décallage de sommets"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_decallage_sommets,decallage_sommets_vertex_shader_id);
            glAttachShader(programme_decallage_sommets,decallage_sommets_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de décallage de sommets...";

            if (!relie_programme_shader(programme_decallage_sommets))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de décallage de sommets"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de décallage de sommets...";
            if (!valide_programme_shader(programme_decallage_sommets))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de décallage de sommets est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            decallage_sommets_deplacement=glGetUniformLocation(programme_decallage_sommets,"deplacement");
            decallage_sommets_niveau_eclairage=glGetUniformLocation(programme_decallage_sommets,"niveau_eclairage");

            //--------- Création des liens avec les variables type "attribute":

            decallage_sommets_tangente_face=glGetAttribLocation(programme_decallage_sommets,"tangente_face");
            decallage_sommets_normale_face=glGetAttribLocation(programme_decallage_sommets,"normale_face");
            decallage_sommets_binormale_face=glGetAttribLocation(programme_decallage_sommets,"binormale_face");
            decallage_sommets_centre_gravite_face=glGetAttribLocation(programme_decallage_sommets,"centre_gravite_face");
            //decallage_sommets_binormale=glGetAttribLocation(programme_rugosite,"binormale");

            if (erreur_openGl("ERREUR dans dksShaders::creation_decallage_sommets() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }

        //===================================================================================
        //                          Création du programme de rendu
        //                          de la réflexion du cube d'arrière plan sans texture
        //===================================================================================
        bool creation_reflexion_couleur_cube()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de la réflexion du cube d'arrière-plan sans texture #############"<<endl;

            source_reflexion_couleur_cube_vertex=charge_source_shader(renvoie_nom_fichier("reflexion_couleur_cube_vertex.txt"));
            source_reflexion_couleur_cube_fragment=charge_source_shader(renvoie_nom_fichier("reflexion_couleur_cube_fragment.txt"));

            if (source_reflexion_couleur_cube_vertex==NULL || source_reflexion_couleur_cube_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            reflexion_couleur_cube_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            reflexion_couleur_cube_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (reflexion_couleur_cube_vertex_shader_id==0 || reflexion_couleur_cube_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de la réflexion du cube d'arrière-plan sans texture"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK "<<reflexion_couleur_cube_vertex_shader_id<<" "<<reflexion_couleur_cube_fragment_shader_id<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_reflexion_couleur_cube_vertex};
            glShaderSource(reflexion_couleur_cube_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_reflexion_couleur_cube_fragment;
            glShaderSource(reflexion_couleur_cube_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(reflexion_couleur_cube_vertex_shader_id)
                ||  !compile_shader(reflexion_couleur_cube_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de la réflexion du cube d'arrière-plan sans texture...";

            programme_reflexion_couleur_cube=glCreateProgram();
            if (programme_reflexion_couleur_cube==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu de la réflexion du cube d'arrière-plan sans texture"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_reflexion_couleur_cube,reflexion_couleur_cube_vertex_shader_id);
            glAttachShader(programme_reflexion_couleur_cube,reflexion_couleur_cube_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu de la réflexion du cube d'arrière-plan sans texture...";

            if (!relie_programme_shader(programme_reflexion_couleur_cube))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu de la réflexion du cube d'arrière-plan sans texture"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu de la réflexion du cube d'arrière-plan sans texture...";
            if (!valide_programme_shader(programme_reflexion_couleur_cube))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu de la réflexion du cube d'arrière-plan sans texture est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            reflexion_couleur_cube_matrice_volume=glGetUniformLocation(programme_reflexion_couleur_cube,"matrice_volume");
            reflexion_couleur_cube_texture=glGetUniformLocation(programme_reflexion_couleur_cube,"cube_arriere_plan");
            reflexion_couleur_cube_position_oeil=glGetUniformLocation(programme_reflexion_couleur_cube,"position_oeil");
            reflexion_couleur_cube_drapeaux_sources=glGetUniformLocation(programme_reflexion_couleur_cube,"drapeaux_sources");
            reflexion_couleur_cube_niveau=glGetUniformLocation(programme_reflexion_couleur_cube,"niveau_reflexion");

            if (erreur_openGl("ERREUR dans dksShaders::creation_reflexion_couleur() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //           d'ondes aqueuses avec éclairage - sans texture de couleur
        //=============================================================================
        bool creation_ondes()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu des ondes aqueuses simples #############"<<endl;

            source_ondes_vertex=charge_source_shader(renvoie_nom_fichier("ondes_vertex.txt"));
            source_ondes_fragment=charge_source_shader(renvoie_nom_fichier("ondes_fragment.txt"));

            if (source_ondes_vertex==NULL || source_ondes_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            ondes_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            ondes_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (ondes_vertex_shader_id==0 || ondes_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::creation_ondes() - Impossible de créer les objets Shader de rendu des ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_ondes_vertex};
            glShaderSource(ondes_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_ondes_fragment;
            glShaderSource(ondes_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(ondes_vertex_shader_id)
                ||  !compile_shader(ondes_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu des ondes aqueuses...";

            programme_ondes=glCreateProgram();
            if (programme_ondes==0)
            {
                cout<<"ERREUR dans dksShaders::creation_ondes() - Impossible de créer le programme de rendu des ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_ondes,ondes_vertex_shader_id);
            glAttachShader(programme_ondes,ondes_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des ondes aqueuses...";

            if (!relie_programme_shader(programme_ondes))
            {
                cout<<"ERREUR dans dksShaders::creation_ondes() - Impossible de lier le programme de rendu des ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des ondes aqueuses...";
            if (!valide_programme_shader(programme_ondes))
            {
                cout<<"ERREUR dans dksShaders::creation_ondes() - Le programme de rendu des ondes aqueuses est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":
            ondes_drapeaux_sources=glGetUniformLocation(programme_ondes,"drapeaux_sources");
            ondes_intensite=glGetUniformLocation(programme_ondes,"intensite_ondes");
            ondes_frequence=glGetUniformLocation(programme_ondes,"frequence_ondes");
            ondes_echelle_texture_bruit=glGetUniformLocation(programme_ondes,"echelle_texture_bruit_ondes");
            ondes_niveau_perturbations=glGetUniformLocation(programme_ondes,"niveau_perturbations_ondes");
            ondes_texture_bruit=glGetUniformLocation(programme_ondes,"texture_bruit");
            ondes_vitesse=glGetUniformLocation(programme_ondes,"vitesse_ondes");
            ondes_quantite=glGetUniformLocation(programme_ondes,"quantite_ondes");
            ondes_amplitude=glGetUniformLocation(programme_ondes,"amplitude_ondes");
            ondes_position_temps=glGetUniformLocation(programme_ondes,"position_temps");
            ondes_centre_gravite_face=glGetUniformLocation(programme_ondes,"centre_gravite_face");
            ondes_tangente=glGetUniformLocation(programme_ondes,"tangente");
            ondes_binormale=glGetUniformLocation(programme_ondes,"binormale");

            //---------- Génère la texture de bruit:
            if (!Bruit::genere_bruit_de_Perlin_2D_8bits(4,4,TAILLE_ONDES_TEXTURE_BRUIT,TAILLE_ONDES_TEXTURE_BRUIT,12,0.75,ondes_tampon_texture_bruit))
            {
                cout<<"ERREUR dans dksShaders::creation_ondes() - Impossible de créer la texture de bruit"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }

            glGenTextures(1,&ondes_texture_bruit_id);
            glBindTexture(GL_TEXTURE_2D,ondes_texture_bruit_id);

            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    //Il semble que ce paramètre active la texture.....
            glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,TAILLE_ONDES_TEXTURE_BRUIT,TAILLE_ONDES_TEXTURE_BRUIT,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,ondes_tampon_texture_bruit);

            //glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);


            //--------- Erreur OpenGl:
            if (erreur_openGl("ERREUR dans dksShaders::creation_ondes() :")) return false;
            else return true;
        }

        //=====================================================================================================
        //                          Création du programme de rendu
        //                          de la réflexion du cube d'arrière plan sans texture avec ondes aqueuses
        //=====================================================================================================
        bool creation_reflexion_couleur_cube_ondes()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses #############"<<endl;

            source_reflexion_couleur_cube_ondes_vertex=charge_source_shader(renvoie_nom_fichier("reflexion_couleur_cube_ondes_vertex.txt"));
            source_reflexion_couleur_cube_ondes_fragment=charge_source_shader(renvoie_nom_fichier("reflexion_couleur_cube_ondes_fragment.txt"));

            if (source_reflexion_couleur_cube_ondes_vertex==NULL || source_reflexion_couleur_cube_ondes_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            reflexion_couleur_cube_ondes_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            reflexion_couleur_cube_ondes_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);
            if (reflexion_couleur_cube_ondes_vertex_shader_id==0 || reflexion_couleur_cube_ondes_fragment_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK "<<reflexion_couleur_cube_ondes_vertex_shader_id<<" "<<reflexion_couleur_cube_ondes_fragment_shader_id<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_reflexion_couleur_cube_ondes_vertex};
            glShaderSource(reflexion_couleur_cube_ondes_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_reflexion_couleur_cube_ondes_fragment;
            glShaderSource(reflexion_couleur_cube_ondes_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(reflexion_couleur_cube_ondes_vertex_shader_id)
                ||  !compile_shader(reflexion_couleur_cube_ondes_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses...";

            programme_reflexion_couleur_cube_ondes=glCreateProgram();
            if (programme_reflexion_couleur_cube_ondes==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_reflexion_couleur_cube_ondes,reflexion_couleur_cube_ondes_vertex_shader_id);
            glAttachShader(programme_reflexion_couleur_cube_ondes,reflexion_couleur_cube_ondes_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses...";

            if (!relie_programme_shader(programme_reflexion_couleur_cube_ondes))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses...";
            if (!valide_programme_shader(programme_reflexion_couleur_cube_ondes))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu de la réflexion du cube d'arrière-plan sans texture avec ondes aqueuses est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //--------- Création des liens avec les variables type "uniform":

            reflexion_couleur_cube_ondes_matrice_volume=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"matrice_volume");
            reflexion_couleur_cube_ondes_texture_cube=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"cube_arriere_plan");
            reflexion_couleur_cube_ondes_position_oeil=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"position_oeil");
            reflexion_couleur_cube_ondes_drapeaux_sources=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"drapeaux_sources");
            reflexion_couleur_cube_ondes_niveau_reflexion=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"niveau_reflexion");

            reflexion_couleur_cube_ondes_intensite_ondes=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"intensite_ondes");
            reflexion_couleur_cube_ondes_frequence=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"frequence_ondes");
            reflexion_couleur_cube_ondes_echelle_texture_bruit=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"echelle_texture_bruit_ondes");
            reflexion_couleur_cube_ondes_niveau_perturbations=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"niveau_perturbations_ondes");
            reflexion_couleur_cube_ondes_texture_bruit=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"texture_bruit");
            reflexion_couleur_cube_ondes_vitesse=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"vitesse_ondes");
            reflexion_couleur_cube_ondes_quantite=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"quantite_ondes");
            reflexion_couleur_cube_ondes_amplitude=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"amplitude_ondes");
            reflexion_couleur_cube_ondes_position_temps=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"position_temps");
            reflexion_couleur_cube_ondes_centre_gravite_face=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"centre_gravite_face");
            reflexion_couleur_cube_ondes_tangente=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"tangente");
            reflexion_couleur_cube_ondes_binormale=glGetUniformLocation(programme_reflexion_couleur_cube_ondes,"binormale");

            if (erreur_openGl("ERREUR dans dksShaders::creation_reflexion_couleur_cube_ondes() :"))
            {
                erreur=DKSSHADERS_ERREUR_OPENGL;
                return false;
            }
            else return true;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //                   de transition entre deux textures de couleur
        //                  Pas d'éclairage pour le moment.
        //                  A ajouter ici quand il y en aura besoin.
        //=============================================================================
        bool creation_transition_textures_couleur()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de transition entre deux textures de couleur #############"<<endl;

            source_transition_textures_couleur_vertex=charge_source_shader(renvoie_nom_fichier("transition_textures_couleur_vertex.txt"));
            source_transition_textures_couleur_fragment=charge_source_shader(renvoie_nom_fichier("transition_textures_couleur_fragment.txt"));

            if (source_transition_textures_couleur_vertex==NULL || source_transition_textures_couleur_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            transition_textures_couleur_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            transition_textures_couleur_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);

            if (transition_textures_couleur_fragment_shader_id==0 || transition_textures_couleur_vertex_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rendu de transition des textures de couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_transition_textures_couleur_vertex};
            glShaderSource(transition_textures_couleur_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_transition_textures_couleur_fragment;
            glShaderSource(transition_textures_couleur_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(transition_textures_couleur_vertex_shader_id)
                ||  !compile_shader(transition_textures_couleur_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu de transition entre les textures de couleur...";

            programme_transition_textures_couleur=glCreateProgram();
            if (programme_transition_textures_couleur==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu de transition entre les textures de couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_transition_textures_couleur,transition_textures_couleur_vertex_shader_id);
            glAttachShader(programme_transition_textures_couleur,transition_textures_couleur_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des transitions entre les textures de couleur...";

            if (!relie_programme_shader(programme_transition_textures_couleur))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu des transitions entre les textures de couleur"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des transitions entre les textures de couleur...";
            if (!valide_programme_shader(programme_transition_textures_couleur))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu des transitions entre les textures de couleur est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":
            transition_textures_couleur_texture_depart=glGetUniformLocation(programme_transition_textures_couleur,"texture_depart");
            transition_textures_couleur_texture_destination=glGetUniformLocation(programme_transition_textures_couleur,"texture_destination");
            transition_textures_couleur_texture_transition=glGetUniformLocation(programme_transition_textures_couleur,"texture_transition");
            transition_textures_couleur_niveau=glGetUniformLocation(programme_transition_textures_couleur,"niveau_transition");

            if (erreur_openGl("ERREUR dans dksShaders::creation_transition_textures_couleur() :")) return false;
            else return true;
        }

        //=============================================================================
        //                          Création du programme de rendu
        //                          de transition entre deux textures de couleur
        //                          le long de l'axe Z
        //=============================================================================
        bool creation_transition_textures_couleur_axe()
        {
            if (DKSSHADERS_DEBUG) cout<<"############# Création du programme de rendu de transition entre deux textures de couleur le long de l'axe Z #############"<<endl;

            source_transition_textures_couleur_axe_vertex=charge_source_shader(renvoie_nom_fichier("transition_textures_couleur_axe_vertex.txt"));
            source_transition_textures_couleur_axe_fragment=charge_source_shader(renvoie_nom_fichier("transition_textures_couleur_axe_fragment.txt"));

            if (source_transition_textures_couleur_axe_vertex==NULL || source_transition_textures_couleur_axe_fragment==NULL)
            {
                erreur=DKSSHADERS_ERREUR_SOURCE;
                return false;
            }

            //---------- Création des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Création des objets shaders...";
            transition_textures_couleur_axe_vertex_shader_id=glCreateShader(GL_VERTEX_SHADER);
            transition_textures_couleur_axe_fragment_shader_id=glCreateShader(GL_FRAGMENT_SHADER);

            if (transition_textures_couleur_axe_fragment_shader_id==0 || transition_textures_couleur_axe_vertex_shader_id == 0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer les objets Shader de rendu de transition des textures de couleur axeZ"<<endl;
                erreur=DKSSHADERS_ERREUR_OBJETS;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADERS_DEBUG) cout<<"Liaisons des sources aux shaders...";

            const GLchar* tableau_sources[]={source_transition_textures_couleur_axe_vertex};
            glShaderSource(transition_textures_couleur_axe_vertex_shader_id,1,tableau_sources,NULL);
            tableau_sources[0]=source_transition_textures_couleur_axe_fragment;
            glShaderSource(transition_textures_couleur_axe_fragment_shader_id,1,tableau_sources,NULL);

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;


            //---------- Compilation des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Compilation des shaders...";
            if (
                    !compile_shader(transition_textures_couleur_axe_vertex_shader_id)
                ||  !compile_shader(transition_textures_couleur_axe_fragment_shader_id)
                )

                {
                    erreur=DKSSHADERS_ERREUR_COMPILATION;
                    return false;
                }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Création des programmes
            if (DKSSHADERS_DEBUG) cout<<"Création du programme de rendu de transition entre les textures de couleur axeZ...";

            programme_transition_textures_couleur_axe=glCreateProgram();
            if (programme_transition_textures_couleur_axe==0)
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de créer le programme de rendu de transition entre les textures de couleur axeZ"<<endl;
                erreur=DKSSHADERS_ERREUR_PROGRAMME;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            glAttachShader(programme_transition_textures_couleur_axe,transition_textures_couleur_axe_vertex_shader_id);
            glAttachShader(programme_transition_textures_couleur_axe,transition_textures_couleur_axe_fragment_shader_id);
            //----------- Liaisons des programmes de shaders:

            if (DKSSHADERS_DEBUG) cout<<"Liaison du programme de rendu des transitions entre les textures de couleur axeZ...";

            if (!relie_programme_shader(programme_transition_textures_couleur_axe))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme de rendu des transitions entre les textures de couleur axeZ"<<endl;
                erreur=DKSSHADERS_ERREUR_LIAISON;
                return false;
            }

            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADERS_DEBUG) cout<<"Validation du programme de rendu des transitions entre les textures de couleur axeZ...";
            if (!valide_programme_shader(programme_transition_textures_couleur_axe))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Le programme de rendu des transitions entre les textures de couleur axeZ est invalide sur cette machine!"<<endl;
                erreur=DKSSHADERS_ERREUR_VALIDATION;
                return false;
            }
            if (DKSSHADERS_DEBUG) cout<<"OK"<<endl;

            //--------- Création des liens avec les variables type "uniform":
            transition_textures_couleur_axe_texture_depart=glGetUniformLocation(programme_transition_textures_couleur_axe,"texture_depart");
            transition_textures_couleur_axe_texture_destination=glGetUniformLocation(programme_transition_textures_couleur_axe,"texture_destination");
            transition_textures_couleur_axe_niveau=glGetUniformLocation(programme_transition_textures_couleur_axe,"niveau_transition");
            transition_textures_couleur_axe_zmin=glGetUniformLocation(programme_transition_textures_couleur_axe,"zmin");
            transition_textures_couleur_axe_zmax=glGetUniformLocation(programme_transition_textures_couleur_axe,"zmax");

            if (erreur_openGl("ERREUR dans dksShaders::creation_transition_textures_couleur() :")) return false;
            else return true;
        }


};

class dksShaders
{
    public: static dksInitialiseShaders* bibliotheque;

};

#endif // DKSSHADERS_H_INCLUDED
