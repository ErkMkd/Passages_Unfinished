#ifndef DKSSURFACE_H_INCLUDED
#define DKSSURFACE_H_INCLUDED

/// Les textures de la surface sont détruites par celle-ci !!!

//----------------------------------
//  Structure des surfaces:
//
//  Une surface ne peut être rattachée qu'à un seul volume.
//  Si la surface est opaque, il n'y a pas de tri des faces.
//  Si la surface est transparente, ses faces sont stockées dans la table des faces transparentes du volume.
//     Une surface est considérée comme transparente si:
//                  -son niveau de transparence > 0,
//                  -la texture de couleur a une couche alpha non uniforme ou <> 0xff
//                  -il y a une texture de transparence
//
//          a priori, la transparence d'un fragment est calculé comme suit:
//                  Tf=transparence+Af+Tt avec une limitation à 1. Af=Alpha, Tt=transparence de la texture de transparence
//
// Le nombre maximum de textures prises en compte dépend de l'implémentation de gestion du mode "Multitextures" d'OpenGl.
// En cas de nombre de textures supérieur au nombre maximum de textures max d'OpenGl, un ordre de priorité est appliqué,
// correspondant à l'ordre de définition des vecteurs de textures ci-après.
//
//----------------------------------

#include "../Rendu/Effets_Speciaux/dksEffetSpecial.h"

#define DKSSURFACE_OK 0

//--------------------------------------------
//      Constantes de gestion des rendus
//--------------------------------------------

#define DKSSURFACE_NBR_RENDUS 128

//Drapeaux de rendu des surfaces:
#define DKSSURFACE_BIT_TEXTURE_COULEUR 0
#define DKSSURFACE_BIT_RUGOSITE 1
#define DKSSURFACE_BIT_REFLEXION_CUBE 2
#define DKSSURFACE_BIT_ONDES 3
#define DKSSURFACE_BIT_OMBRES 4
#define DKSSURFACE_BIT_PHONG  5
#define DKSSURFACE_BIT_TRANSITION 6 //Transition entre deux textures de couleur, avec une texture de modulation de transition

#define DKSSURFACE_MASK_TEXTURE_COULEUR (1<<DKSSURFACE_BIT_TEXTURE_COULEUR)
#define DKSSURFACE_MASK_RUGOSITE (1<<DKSSURFACE_BIT_RUGOSITE)
#define DKSSURFACE_MASK_REFLEXION_CUBE (1<<DKSSURFACE_BIT_REFLEXION_CUBE)
#define DKSSURFACE_MASK_ONDES (1<<DKSSURFACE_BIT_ONDES)
#define DKSSURFACE_MASK_OMBRES (1<<DKSSURFACE_BIT_OMBRES)
#define DKSSURFACE_MASK_PHONG (1<<DKSSURFACE_BIT_PHONG)
#define DKSSURFACE_MASK_TRANSITION (1<<DKSSURFACE_BIT_TRANSITION)

//Modes de rendus des surfaces:

#define DKSSURFACE_RENDU_COULEUR                                    0x0000
#define DKSSURFACE_RENDU_TEXTURE_COULEUR                            0x0001
#define DKSSURFACE_RENDU_RUGOSITE                                   0x0002
#define DKSSURFACE_RENDU_RUGOSITE_TEXTURE_COULEUR                   0x0003
#define DKSSURFACE_RENDU_REFLEXION_CUBE                             0x0004
#define DKSSURFACE_RENDU_REFLEXION_CUBE_TEXTURE_COULEUR             0x0005
#define DKSSURFACE_RENDU_REFLEXION_CUBE_RUGOSITE                    0x0006
#define DKSSURFACE_RENDU_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR    0x0007

#define DKSSURFACE_RENDU_ONDES                                          0x0008
#define DKSSURFACE_RENDU_ONDES_TEXTURE_COULEUR                          0x0009
#define DKSSURFACE_RENDU_ONDES_RUGOSITE                                 0x000A
#define DKSSURFACE_RENDU_ONDES_RUGOSITE_TEXTURE_COULEUR                 0x000B
#define DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE                           0x000C
#define DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE_TEXTURE_COULEUR           0x000D
#define DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE_RUGOSITE                  0x000E
#define DKSSURFACE_RENDU_ONDES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR  0x000F

#define DKSSURFACE_RENDU_OMBRES                                         0x0010
#define DKSSURFACE_RENDU_OMBRES_TEXTURE_COULEUR                         0x0011
#define DKSSURFACE_RENDU_OMBRES_RUGOSITE                                0x0012
#define DKSSURFACE_RENDU_OMBRES_RUGOSITE_TEXTURE_COULEUR                0x0013
#define DKSSURFACE_RENDU_OMBRES_REFLEXION_CUBE                          0x0014
#define DKSSURFACE_RENDU_OMBRES_REFLEXION_CUBE_TEXTURE_COULEUR          0x0015
#define DKSSURFACE_RENDU_OMBRES_REFLEXION_CUBE_RUGOSITE                 0x0016
#define DKSSURFACE_RENDU_OMBRES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR 0x0017

#define DKSSURFACE_RENDU_OMBRES_ONDES                                           0x0018
#define DKSSURFACE_RENDU_OMBRES_ONDES_TEXTURE_COULEUR                           0x0019
#define DKSSURFACE_RENDU_OMBRES_ONDES_RUGOSITE                                  0x001A
#define DKSSURFACE_RENDU_OMBRES_ONDES_RUGOSITE_TEXTURE_COULEUR                  0x001B
#define DKSSURFACE_RENDU_OMBRES_ONDES_REFLEXION_CUBE                            0x001C
#define DKSSURFACE_RENDU_OMBRES_ONDES_REFLEXION_CUBE_TEXTURE_COULEUR            0x001D
#define DKSSURFACE_RENDU_OMBRES_ONDES_REFLEXION_CUBE_RUGOSITE                   0x001E
#define DKSSURFACE_RENDU_OMBRES_ONDES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR   0x001F

#define DKSSURFACE_RENDU_PHONG                                              0x0020
#define DKSSURFACE_RENDU_PHONG_TEXTURE_COULEUR                              0x0021

//#define DKSSURFACE_RENDU_PHONG_RUGOSITE                                   0x0022    ///Inutilisé
#define DKSSURFACE_RENDU_EFFET_SPECIAL                                      0x0022    ///Mode de rendu des effets spéciaux

//#define DKSSURFACE_RENDU_PHONG_RUGOSITE_TEXTURE_COULEUR                   0x0023    ///Inutilisé
#define DKSSURFACE_RENDU_APLAT                                              0x0023    ///A la place de DKSSURFACE_RENDU_PHONG_RUGOSITE_TEXTURE_COULEUR

#define DKSSURFACE_RENDU_PHONG_REFLEXION_CUBE                               0x0024
#define DKSSURFACE_RENDU_PHONG_REFLEXION_CUBE_TEXTURE_COULEUR               0x0025
//#define DKSSURFACE_RENDU_PHONG_REFLEXION_CUBE_RUGOSITE                    0x0026    ///Inutilisé
//#define DKSSURFACE_RENDU_PHONG_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR    0x0027    ///Inutilisé

/*Inutilisés
#define DKSSURFACE_RENDU_PHONG_ONDES                                          0x0028
#define DKSSURFACE_RENDU_PHONG_ONDES_TEXTURE_COULEUR                          0x0029
#define DKSSURFACE_RENDU_PHONG_ONDES_RUGOSITE                                 0x002A
#define DKSSURFACE_RENDU_PHONG_ONDES_RUGOSITE_TEXTURE_COULEUR                 0x002B
#define DKSSURFACE_RENDU_PHONG_ONDES_REFLEXION_CUBE                           0x002C
#define DKSSURFACE_RENDU_PHONG_ONDES_REFLEXION_CUBE_TEXTURE_COULEUR           0x002D
#define DKSSURFACE_RENDU_PHONG_ONDES_REFLEXION_CUBE_RUGOSITE                  0x002E
#define DKSSURFACE_RENDU_PHONG_ONDES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR  0x002F
*/

#define DKSSURFACE_RENDU_PHONG_OMBRES                                         0x0030
#define DKSSURFACE_RENDU_PHONG_OMBRES_TEXTURE_COULEUR                         0x0031
//#define DKSSURFACE_RENDU_PHONG_OMBRES_RUGOSITE                                0x0032    ///Inutilisé
#define DKSSURFACE_RENDU_PHONG_OMBRES_RUGOSITE_TEXTURE_COULEUR                0x0033
#define DKSSURFACE_RENDU_PHONG_OMBRES_REFLEXION_CUBE                          0x0034
#define DKSSURFACE_RENDU_PHONG_OMBRES_REFLEXION_CUBE_TEXTURE_COULEUR          0x0035
//#define DKSSURFACE_RENDU_PHONG_OMBRES_REFLEXION_CUBE_RUGOSITE                 0x0036    ///Inutilisé
//#define DKSSURFACE_RENDU_PHONG_OMBRES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR 0x0037    ///Inutilisé

/*Inutilisés
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES                                           0x0038
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_TEXTURE_COULEUR                           0x0039
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_RUGOSITE                                  0x004A
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_RUGOSITE_TEXTURE_COULEUR                  0x004B
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_REFLEXION_CUBE                            0x004C
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_REFLEXION_CUBE_TEXTURE_COULEUR            0x004D
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_REFLEXION_CUBE_RUGOSITE                   0x004E
#define DKSSURFACE_RENDU_PHONG_OMBRES_ONDES_REFLEXION_CUBE_RUGOSITE_TEXTURE_COULEUR   0x004F
*/

//Effets(priorité sur le rendu):
#define DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR 50


//=======================================================================================
//
//=======================================================================================

class dksSurface
{
    private:
        uint16_t bits_rendus;   //Drapeaux constituant le code du shader utilisé pour la surface
                                //Le mode de rendu est généré à partir de ces bits, car il y a parfois des adaptations à faire (shaders incompatibles)
    public:
        uint16_t erreur;
        char nom[DKS_TAILLE_NOMS];
        dksVolume* volume;  //Le volume auquel appartient la surface
        uint16_t mode_rendu;    //Code du rendu de la surface. Ce code correspond à la fonction de rendu qui va être utilisée.
        dksEffetSpecial* effet_special;
        bool drapeau_recoit_ombres;  //True si la surface peut recevoir des ombres portées.
        bool drapeau_projete_ombres;
        bool drapeau_phong;     //True si la surface est éclairée en Phong
        bool drapeau_effet_special; //True si un effet spécial est utilisé pour la surface (priorité sur les autres rendus)
        bool drapeau_transition;    //True si la transition entre deux textures est activée

        bool double_face;   //Affiche les deux côtés des faces
        bool drapeau_adoucissement; //True si les faces sont adoucies
        bool drapeau_transparence;  //Champ initialisé par le volume - True si la surface est transparente.

        float angle_limite_adoucissement;   //en radians
        GLdouble* normales_sommets; //Table utilisée pour le mode de rendu par tableaux de pointeurs
        uint32_t nbrNormales;

        float couleur_diffusion[4];
        float couleur_speculaire[4];
        float couleur_ambiance[4]; //Réflexion de la lumière ambiante
        float couleur_luminescence[4]; //Luminescence de la surface

        float luminescence;
        float diffusion;
        float ambiance;
        float specularite;

        float brillance;    //=Shininess
        float transparence;
        float transparence_speculaire;
        float translucidite; //Comment le gérer?
        float reflexion;// Gérée par les shaders
        float rugosite; //"Bump", géré par les shaders

        //Effet d'ondulations aqueuses
        float intensite_ondes;  //Mettre à 0 pour désactiver l'effet
        float amplitude_ondes;  //défaut: 0.2
        float frequence_ondes;  //défaut: 2
        float vitesse_ondes;    //défaut: 1
        float quantite_ondes;   //défaut: 5
        float perturbation_ondes; //défaut: 10
        float echelle_perturbation_ondes; //défaut: 10

        //Effet de transition entre 2 textures = DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR
        // -1 si pas de texture au poste respectif. (c'est alors la couleur de la surface qui peut servir...)
        int32_t indice_texture_depart;
        int32_t indice_texture_destination;
        int32_t indice_texture_transition;     // texture utilisée comme masque de transition
        float niveau_transition;    //0: texture depart, 1:texture destination


        //Rendus avec une texture de couleur:
        uint32_t indice_texture_couleur;    //L'indice de la texture de couleur utilisée par les rendus avec une texture de couleur

        //Table globale des textures:
        vector<dksTexture*> textures;

        //Textures actives:
        vector<dksTexture*> textures_couleur;       //Image RVB,ARVB,Gris
        vector<dksTexture*> textures_diffusion;     //Image en niveaux de gris
        vector<dksTexture*> textures_ambiance;      //NG
        vector<dksTexture*> textures_luminescence;   //NG
        vector<dksTexture*> textures_transparence;  //NG
        vector<dksTexture*> textures_reflexion;     //NG
        vector<dksTexture*> textures_rugosite;      //Plusieurs modes de rugosité possibles: par décallage (NG) ou déviation de normales (RVB, ARVB)

        //Textures inactives:
        vector<dksTexture*> textures_couleur_inactives;
        vector<dksTexture*> textures_diffusion_inactives;
        vector<dksTexture*> textures_ambiance_inactives;
        vector<dksTexture*> textures_luminescence_inactives;
        vector<dksTexture*> textures_transparence_inactives;
        vector<dksTexture*> textures_reflexion_inactives;
        vector<dksTexture*> textures_rugosite_inactives;

        //Sommets liés à la surface: sert pour les générateurs de cooronnées UV de textures - table initialisée par dksVolume::initialise_indices_sommets_surfaces()
        vector<uint32_t> indices_sommets;
        //faces liées à la surface: Ces champs sont remplis automatiquement par le volume.
        uint32_t nbr_faces;
        vector<dksFace*> faces;

        //Ces tableaux ne servent que pour le rendu en mode immédiat (donc aussi pour générer les listes openGl...)
        vector<dksFace*> faces_adoucies;
        vector<dksFace*> faces_partiellement_adoucies; //Les faces dont une ou plusieurs arêtes forment un angle > à l'angle d'adoucissement.

        //----------------------------------------------------
        //Constructeur:
        //----------------------------------------------------
        public:
        dksSurface(char* p_nom=NULL)
        {
            erreur=DKSSURFACE_OK;
            uint32_t i,taille;
            normales_sommets=NULL;
            nbr_faces=0;

            if (p_nom==NULL) nom[0]=0;
            else
            {
                taille=strlen(p_nom);
                if (taille>DKS_TAILLE_NOMS-1)taille=DKS_TAILLE_NOMS-1;
                for(i=0;i<taille;i++)
                {
                    nom[i]=p_nom[i];
                }
                nom[i]=0;
            }

            //Couleurs:
            couleur_ambiance={1.,1.,1.};
            couleur_diffusion={0.8,0.8,0.8};
            couleur_speculaire={1.,1.,1.};
            couleur_luminescence={1.,1.,1.};

            //Paramètres de base:
            ambiance=1.;
            specularite=0.5;
            diffusion=1.;
            luminescence=0.;
            brillance=10.;
            transparence=0.;
            transparence_speculaire=0.;
            translucidite=0.;
            reflexion=0.;
            rugosite=0.;

            drapeau_transparence=false;
            drapeau_adoucissement=false;
            angle_limite_adoucissement=89.;
            double_face=false;

            //Ondes aqueuses:
            intensite_ondes=0.;
            amplitude_ondes=0.2;
            frequence_ondes=2.;
            vitesse_ondes=1.5;
            quantite_ondes=5.;
            perturbation_ondes=7;
            echelle_perturbation_ondes=10.;

            //Transition:
            indice_texture_depart=-1;
            indice_texture_destination=-1;
            indice_texture_transition=-1;

            //Mode de rendu par défaut:
            bits_rendus=0;
            mode_rendu=DKSSURFACE_RENDU_COULEUR;
            drapeau_projete_ombres=false;
            drapeau_recoit_ombres=false;
            drapeau_phong=false;
            drapeau_transition=false;
            drapeau_effet_special=false;

            //Indices des textures par défaut:
            indice_texture_couleur=0;
        }

        //----------------------------------------------------
        //Constructeur de copie:
        //Les faces ne sont pas copiées.
        //----------------------------------------------------
        dksSurface(dksSurface& surface_originale)
        {
            uint32_t i;
            nbr_faces=0;
            erreur=surface_originale.erreur;
            for(i=0;i<DKS_TAILLE_NOMS;i++)
            {
                nom[i]=surface_originale.nom[i];
                if (nom[i]==0) break;
            }
            volume=surface_originale.volume;    ///COHERENCE A REVOIR - Logiquement, la nouvelle surface n'est pas encore liée à un volume.
                                                ///C'est ce dernier qui va initialiser l'ensemble de la nouvelle surface.
            double_face=surface_originale.double_face;
            drapeau_adoucissement=surface_originale.drapeau_adoucissement;
            angle_limite_adoucissement=surface_originale.angle_limite_adoucissement;
            drapeau_transparence=surface_originale.drapeau_transparence;        /// Idem
            for(i=0;i<4;i++)
            {
                couleur_diffusion[i]=surface_originale.couleur_diffusion[i];
                couleur_luminescence[i]=surface_originale.couleur_luminescence[i];
                couleur_ambiance[i]=surface_originale.couleur_ambiance[i];
                couleur_speculaire[i]=surface_originale.couleur_speculaire[i];
            }
            luminescence=surface_originale.luminescence;
            diffusion=surface_originale.diffusion;
            specularite=surface_originale.specularite;
            ambiance=surface_originale.ambiance;
            transparence=surface_originale.transparence;
            transparence_speculaire=surface_originale.transparence_speculaire;
            translucidite=surface_originale.translucidite;
            brillance=surface_originale.brillance;
            reflexion=surface_originale.reflexion;
            rugosite=surface_originale.rugosite;

            intensite_ondes=surface_originale.intensite_ondes;
            amplitude_ondes=surface_originale.amplitude_ondes;
            frequence_ondes=surface_originale.frequence_ondes;
            vitesse_ondes=surface_originale.vitesse_ondes;
            quantite_ondes=surface_originale.quantite_ondes;
            perturbation_ondes=surface_originale.perturbation_ondes;
            echelle_perturbation_ondes=surface_originale.echelle_perturbation_ondes;


            mode_rendu=surface_originale.mode_rendu;
            bits_rendus=surface_originale.bits_rendus;
            drapeau_projete_ombres=surface_originale.drapeau_projete_ombres;
            drapeau_recoit_ombres=surface_originale.drapeau_recoit_ombres;
            drapeau_phong=surface_originale.drapeau_phong;
            drapeau_transition=surface_originale.drapeau_transition;
            drapeau_effet_special=surface_originale.drapeau_effet_special;

            indice_texture_couleur=surface_originale.indice_texture_couleur;

            effet_special=surface_originale.effet_special;

            //---------Copie les textures

            // Ambiance
            for (i=0;i<surface_originale.textures_ambiance.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_ambiance[i]);
                textures_ambiance.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_ambiance_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_ambiance_inactives[i]);
                textures_ambiance_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Couleur
            for (i=0;i<surface_originale.textures_couleur.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_couleur[i]);
                textures_couleur.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_couleur_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_couleur_inactives[i]);
                textures_couleur_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Diffusion
            for (i=0;i<surface_originale.textures_diffusion.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_diffusion[i]);
                textures_diffusion.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_diffusion_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_diffusion_inactives[i]);
                textures_diffusion_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Luminescence
            for (i=0;i<surface_originale.textures_luminescence.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_luminescence[i]);
                textures_luminescence.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_luminescence_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_luminescence_inactives[i]);
                textures_luminescence_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Réflexion:
            for (i=0;i<surface_originale.textures_reflexion.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_reflexion[i]);
                textures_reflexion.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_reflexion_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_reflexion_inactives[i]);
                textures_reflexion_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Rugosité:
            for (i=0;i<surface_originale.textures_rugosite.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_rugosite[i]);
                textures_rugosite.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_rugosite_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_rugosite_inactives[i]);
                textures_rugosite_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }

            //Transparence:
            for (i=0;i<surface_originale.textures_transparence.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_transparence[i]);
                textures_transparence.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
            for (i=0;i<surface_originale.textures_transparence_inactives.size();i++)
            {
                dksTexture* nouvelle_texture=new dksTexture(*surface_originale.textures_transparence_inactives[i]);
                textures_transparence_inactives.push_back(nouvelle_texture);
                textures.push_back(nouvelle_texture);
            }
        }

        //----------------------------------------------------
        //Destructeur:
        /// Les textures sont détruites !
        //----------------------------------------------------
        ~dksSurface()
        {
            uint32_t i;
            active_toutes_les_textures();
            for(i=0;i<textures_ambiance.size();i++)
            {
                if (textures_ambiance[i]!=NULL) delete textures_ambiance[i];
            }

            for(i=0;i<textures_couleur.size();i++)
            {
                if (textures_couleur[i]!=NULL) delete textures_couleur[i];
            }

            for(i=0;i<textures_diffusion.size();i++)
            {
                if (textures_diffusion[i]!=NULL) delete textures_diffusion[i];
            }

            for(i=0;i<textures_luminescence.size();i++)
            {
                if (textures_luminescence[i]!=NULL) delete textures_luminescence[i];
            }

            for(i=0;i<textures_reflexion.size();i++)
            {
                if (textures_reflexion[i]!=NULL) delete textures_reflexion[i];
            }

            for(i=0;i<textures_rugosite.size();i++)
            {
                if (textures_rugosite[i]!=NULL) delete textures_rugosite[i];
            }

            for(i=0;i<textures_transparence.size();i++)
            {
                if (textures_transparence[i]!=NULL) delete textures_transparence[i];
            }
            if (normales_sommets!=NULL) free (normales_sommets);
        }

        //----------------------------------------------------
        //              Gestion du matériau:
        //----------------------------------------------------
        public:
        void determine_couleur_ambiance(uint32_t couleur, float intensite=0.2)
        {
            determine_couleur_ARVB(couleur_ambiance,couleur);
            ambiance=intensite;
        }

        void determine_couleur_diffusion(uint32_t couleur, float intensite=1.)
        {
            determine_couleur_ARVB(couleur_diffusion,couleur);
            diffusion=intensite;
        }

        void determine_couleur_speculaire(uint32_t couleur, float intensite=0.5)
        {
            determine_couleur_ARVB(couleur_speculaire,couleur);
            specularite=intensite;
        }

        void determine_couleur_luminescence(uint32_t couleur, float intensite=0.2)
        {
            determine_couleur_ARVB(couleur_luminescence,couleur);
            luminescence=intensite;
        }

        void determine_ambiance(float intensite)
        {
            ambiance=intensite;
        }

        void determine_diffusion(float intensite)
        {
            diffusion=intensite;
        }

        void determine_specularite(float intensite)
        {
            specularite=intensite;
        }

        void determine_luminescence(float intensite)
        {
            luminescence=intensite;
        }

        void determine_transparence(float p_transparence)
        {
            transparence=p_transparence;
            if (transparence>0.)drapeau_transparence=true;
            else drapeau_transparence=false;
        }

        void determine_reflexion(float p_reflexion)
        {
            reflexion=p_reflexion;
            initialise_mode_rendu();
        }

        void determine_transparence_speculaire(float p_transparence)
        {
            transparence_speculaire=p_transparence;
        }

        void determine_intensite_ondes(float p_intensite)
        {
            intensite_ondes=p_intensite;
            initialise_mode_rendu();
        }

        void applique_teinte_diffusion(uint32_t teinte)
        {
            applique_teinte_ARVB(couleur_diffusion,teinte);
        }

        void applique_teinte_specularite(uint32_t teinte)
        {
            applique_teinte_ARVB(couleur_speculaire,teinte);
        }
        void applique_teinte_luminescence(uint32_t teinte)
        {
            applique_teinte_ARVB(couleur_luminescence,teinte);
        }
        void applique_teinte_ambiance(uint32_t teinte)
        {
            applique_teinte_ARVB(couleur_ambiance,teinte);
        }

        private:
        void determine_couleur_ARVB(float* tab,uint32_t couleur)
        {
            tab[0]=(float)((couleur>>16)&0xff)/255.;
            tab[1]=(float)((couleur>>8)&0xff)/255.;
            tab[2]=(float)(couleur&0xff)/255.;
            tab[3]=(float)((couleur>>24)&0xff)/255.;
        }

        void applique_teinte_ARVB(float* tab, uint32_t teinte)
        {
            float r=(float)((teinte>>16)&0xff)/255.;
            float v=(float)((teinte>>8)&0xff)/255.;
            float b=(float)(teinte & 0xff)/255.;
            float a=(float)((teinte>>24)&0xff)/255.;
            tab[0]*=r;
            tab[1]*=v;
            tab[2]*=b;
            tab[3]*=a;

            /*
            tab[0]/=2.;
            tab[1]/=2.;
            tab[2]/=2.;
            tab[3]/=2.;
            */
        }

        //----------------------------------------------------
        //              Gestion des textures:
        //----------------------------------------------------
        public:

        dksTexture* renvoie_texture_nom(char* nom_texture)
        {
            for (uint32_t i=0;i<textures.size();i++)
            {
                if(strcmp(textures[i]->nom,nom_texture)==0) return textures[i];
            }
            return NULL;
        }


        //Ajoute une texture:
        //La texture est active par défaut
        void ajoute_texture(dksTexture* texture)
        {
            textures.push_back(texture);
            switch(texture->canal)
            {
                case DKSTEXTURE_CANAL_AMBIANCE:
                    if(texture->image==NULL)textures_ambiance_inactives.push_back(texture);
                    else textures_ambiance.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_COULEUR:
                    if(texture->image==NULL)textures_couleur_inactives.push_back(texture);
                    else textures_couleur.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_DIFFUSION:
                    if(texture->image==NULL)textures_diffusion_inactives.push_back(texture);
                    else textures_diffusion.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_LUMINESCENCE:
                    if(texture->image==NULL)textures_luminescence_inactives.push_back(texture);
                    else textures_luminescence.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_REFLEXION:
                    if(texture->image==NULL)textures_reflexion_inactives.push_back(texture);
                    else textures_reflexion.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_RUGOSITE:
                    if(texture->image==NULL)textures_rugosite_inactives.push_back(texture);
                    else textures_rugosite.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_TRANSPARENCE:
                    if(texture->image==NULL)textures_transparence_inactives.push_back(texture);
                    else textures_transparence.push_back(texture);
                    break;
                default:
                    cout<<"Avertissement dans dksSurface::ajoute_texture(): texture de type inconnu"<<endl;
                    break;
            }
        }

        bool texture_est_active(dksTexture* texture)
        {
            uint16_t i;
            switch (texture->canal)
            {
                case DKSTEXTURE_CANAL_COULEUR: for (i=0;i<textures_couleur.size();i++) if (textures_couleur[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_AMBIANCE: for (i=0;i<textures_ambiance.size();i++) if (textures_ambiance[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_DIFFUSION: for (i=0;i<textures_diffusion.size();i++) if (textures_diffusion[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_LUMINESCENCE: for (i=0;i<textures_luminescence.size();i++) if (textures_luminescence[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_REFLEXION: for (i=0;i<textures_reflexion.size();i++) if (textures_reflexion[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_RUGOSITE: for (i=0;i<textures_rugosite.size();i++) if (textures_rugosite[i]==texture) return true; break;
                case DKSTEXTURE_CANAL_TRANSPARENCE: for (i=0;i<textures_transparence.size();i++) if (textures_transparence[i]==texture) return true; break;
            }
            return false;
        }

        //Renvoie 0xffff si la texture n'est pas dans les tables:
        uint16_t renvoie_texture_active_index(dksTexture* texture)
        {
            uint16_t i;
            switch (texture->canal)
            {
                case DKSTEXTURE_CANAL_COULEUR: for (i=0;i<textures_couleur.size();i++) if (textures_couleur[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_AMBIANCE: for (i=0;i<textures_ambiance.size();i++) if (textures_ambiance[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_DIFFUSION: for (i=0;i<textures_diffusion.size();i++) if (textures_diffusion[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_LUMINESCENCE: for (i=0;i<textures_luminescence.size();i++) if (textures_luminescence[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_REFLEXION: for (i=0;i<textures_reflexion.size();i++) if (textures_reflexion[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_RUGOSITE: for (i=0;i<textures_rugosite.size();i++) if (textures_rugosite[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_TRANSPARENCE: for (i=0;i<textures_transparence.size();i++) if (textures_transparence[i]==texture) return i; break;
            }
            return 0xffff;
        }

        //Renvoie 0xffff si la texture n'est pas dans les tables:
        uint16_t renvoie_texture_inactive_index(dksTexture* texture)
        {
            uint16_t i;
            switch (texture->canal)
            {
                case DKSTEXTURE_CANAL_COULEUR: for (i=0;i<textures_couleur_inactives.size();i++) if (textures_couleur_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_AMBIANCE: for (i=0;i<textures_ambiance_inactives.size();i++) if (textures_ambiance_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_DIFFUSION: for (i=0;i<textures_diffusion_inactives.size();i++) if (textures_diffusion_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_LUMINESCENCE: for (i=0;i<textures_luminescence_inactives.size();i++) if (textures_luminescence_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_REFLEXION: for (i=0;i<textures_reflexion_inactives.size();i++) if (textures_reflexion_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_RUGOSITE: for (i=0;i<textures_rugosite_inactives.size();i++) if (textures_rugosite_inactives[i]==texture) return i; break;
                case DKSTEXTURE_CANAL_TRANSPARENCE: for (i=0;i<textures_transparence_inactives.size();i++) if (textures_transparence_inactives[i]==texture) return i; break;
            }
            return 0xffff;
        }

        void desactive_texture(dksTexture* texture)
        {
            uint16_t i=renvoie_texture_active_index(texture);
            if (i==0xffff) return;

            //vector<dksTexture*>::iterator it;

            switch (texture->canal)
            {
                case DKSTEXTURE_CANAL_COULEUR: textures_couleur.erase(textures_couleur.begin()+i); textures_couleur_inactives.push_back(texture); break;
                case DKSTEXTURE_CANAL_AMBIANCE: textures_ambiance.erase(textures_ambiance.begin()+i); textures_ambiance_inactives.push_back(texture); break;
                case DKSTEXTURE_CANAL_DIFFUSION: textures_diffusion.erase(textures_diffusion.begin()+i); textures_diffusion_inactives.push_back(texture); break;
                case DKSTEXTURE_CANAL_LUMINESCENCE: textures_luminescence.erase(textures_luminescence.begin()+i); textures_luminescence_inactives.push_back(texture); break;
                case DKSTEXTURE_CANAL_REFLEXION: textures_reflexion.erase(textures_reflexion.begin()+i); textures_reflexion_inactives.push_back(texture); break;
                case DKSTEXTURE_CANAL_RUGOSITE:
                    cout<<"DESACTIVE LA TEXTURE DE RUGOSITE"<<endl;
                    textures_rugosite.erase(textures_rugosite.begin()+i); textures_rugosite_inactives.push_back(texture);
                    break;
                case DKSTEXTURE_CANAL_TRANSPARENCE: textures_transparence.erase(textures_transparence.begin()+i); textures_transparence_inactives.push_back(texture); break;
            }
        }

        void active_texture(dksTexture* texture)
        {
            if(texture->image==NULL)return;
            uint16_t i=renvoie_texture_inactive_index(texture);
            if (i==0xffff) return;

            switch (texture->canal)
            {
                case DKSTEXTURE_CANAL_COULEUR: textures_couleur_inactives.erase(textures_couleur_inactives.begin()+i); textures_couleur.push_back(texture); break;
                case DKSTEXTURE_CANAL_AMBIANCE: textures_ambiance_inactives.erase(textures_ambiance_inactives.begin()+i); textures_ambiance.push_back(texture); break;
                case DKSTEXTURE_CANAL_DIFFUSION: textures_diffusion_inactives.erase(textures_diffusion_inactives.begin()+i); textures_diffusion.push_back(texture); break;
                case DKSTEXTURE_CANAL_LUMINESCENCE: textures_luminescence_inactives.erase(textures_luminescence_inactives.begin()+i); textures_luminescence.push_back(texture); break;
                case DKSTEXTURE_CANAL_REFLEXION: textures_reflexion_inactives.erase(textures_reflexion_inactives.begin()+i); textures_reflexion.push_back(texture); break;
                case DKSTEXTURE_CANAL_RUGOSITE: textures_rugosite_inactives.erase(textures_rugosite_inactives.begin()+i); textures_rugosite.push_back(texture); break;
                case DKSTEXTURE_CANAL_TRANSPARENCE: textures_transparence_inactives.erase(textures_transparence_inactives.begin()+i); textures_transparence.push_back(texture); break;
            }
        }

        ///ATTENTION !!! MEME LES TEXTRES SANS IMAGE DEFINIE SONT ACTIVEES!! CRASH ASSURE!!!
        void active_toutes_les_textures()
        {
            uint16_t i;
            for(i=0;i<textures_ambiance_inactives.size();i++) textures_ambiance.push_back(textures_ambiance_inactives[i]);
            for(i=0;i<textures_couleur_inactives.size();i++) textures_couleur.push_back(textures_couleur_inactives[i]);
            for(i=0;i<textures_diffusion_inactives.size();i++) textures_diffusion.push_back(textures_diffusion_inactives[i]);
            for(i=0;i<textures_luminescence_inactives.size();i++) textures_luminescence_inactives.push_back(textures_luminescence_inactives[i]);
            for(i=0;i<textures_reflexion_inactives.size();i++) textures_reflexion.push_back(textures_reflexion_inactives[i]);
            for(i=0;i<textures_rugosite_inactives.size();i++) textures_rugosite.push_back(textures_rugosite_inactives[i]);
            for(i=0;i<textures_transparence_inactives.size();i++) textures_transparence.push_back(textures_transparence_inactives[i]);
            textures_ambiance_inactives.clear();
            textures_couleur_inactives.clear();
            textures_diffusion_inactives.clear();
            textures_luminescence_inactives.clear();
            textures_reflexion_inactives.clear();
            textures_rugosite_inactives.clear();
            textures_transparence_inactives.clear();

        }

         //----------------------------------------------------
        //              Gestion de l'adoucissement
        //----------------------------------------------------

        //Réinitialise les listes d'adoucissement:

         ///Les drapeaux des faces doivent être correctement initialisés
        void rafraichi_faces_adoucies()
        {
            faces_adoucies.clear();
            faces_partiellement_adoucies.clear();
            for (uint32_t i=0;i< nbr_faces;i++ )
            {
                if(faces[i]->drapeau_partiellement_adoucie) faces_partiellement_adoucies.push_back(faces[i]);
                else faces_adoucies.push_back(faces[i]);
            }
        }

        void ajoute_face_adoucie(dksFace* face)
        {
            faces_adoucies.push_back(face);
        }

        void ajoute_face_partiellement_adoucie(dksFace* face)
        {
            faces_partiellement_adoucies.push_back(face);
        }

        //-------------------------------------------------------
        //      Gestion du tableau de normales des sommets
        //-------------------------------------------------------
        bool alloue_tableau_normales_sommets(uint32_t p_nbrSommets)
        {
            normales_sommets=(GLdouble*)malloc(p_nbrSommets*sizeof(GLdouble)*3);
            if (normales_sommets==NULL) return false;
            nbrNormales=p_nbrSommets;
            return true;
        }

        void inverse_normales()
        {
            for (uint32_t i=0;i<nbrNormales;i++)
            {
                normales_sommets[3*i]=-normales_sommets[3*i];
                normales_sommets[3*i+1]=-normales_sommets[3*i+1];
                normales_sommets[3*i+2]=-normales_sommets[3*i+2];
            }
        }

        //=======================================================
        //              Gestion des modes de rendu:
        //=======================================================
        void determine_effet_special(dksEffetSpecial* effet)
        {
            effet_special=effet;
        }

        void active_effet_special()
        {
            drapeau_effet_special=true;
            mode_rendu=DKSSURFACE_RENDU_EFFET_SPECIAL;
        }

        void desactive_effet_special()
        {
            drapeau_effet_special=false;
            initialise_mode_rendu();
        }

        void determine_phong(bool p_drapeau)
        {
            drapeau_phong=p_drapeau;
            initialise_mode_rendu();
        }

        void determine_transition(bool p_drapeau)
        {
            drapeau_transition=p_drapeau;
            initialise_mode_rendu();
        }


        //------- Le mode de rendu est déterminé en fonction des paramètres de la surface:
        void initialise_mode_rendu()
        {
            bits_rendus=0;
            if(drapeau_effet_special)
            {
                mode_rendu=DKSSURFACE_RENDU_EFFET_SPECIAL;
                return;
            }
            if (textures_couleur.size()>0 && textures_couleur[0]->type_projection==DKSTEXTURE_PROJECTION_UV) bits_rendus|=DKSSURFACE_MASK_TEXTURE_COULEUR;
            if (textures_rugosite.size()>0 && textures_rugosite[0]->type_projection==DKSTEXTURE_PROJECTION_UV) bits_rendus|=DKSSURFACE_MASK_RUGOSITE;
            if (reflexion>0.) bits_rendus|=DKSSURFACE_MASK_REFLEXION_CUBE;
            if (intensite_ondes>0.) bits_rendus|=DKSSURFACE_MASK_ONDES;
            if (drapeau_recoit_ombres) bits_rendus|=DKSSURFACE_MASK_OMBRES;
            if(drapeau_phong)
            {
               if((bits_rendus&(DKSSURFACE_MASK_ONDES|DKSSURFACE_MASK_RUGOSITE))==0) bits_rendus|=DKSSURFACE_MASK_PHONG;
            }
            //Pour le moment, la transition se fait sans autre rendu.
            if(drapeau_transition)
            {
                mode_rendu=DKSSURFACE_RENDU_TRANSITION_TEXTURES_COULEUR;
                return;
            }

            //Génère le code du shader:
            mode_rendu=bits_rendus;
        }

        //-----------------------------------------------------
        //          Gestion des ombres
        //-----------------------------------------------------
        void determine_recoit_ombres(bool d)
        {
            drapeau_recoit_ombres=d;
            initialise_mode_rendu();
        }

        void determine_projete_ombres(bool d)
        {
            drapeau_projete_ombres=d;
            initialise_mode_rendu();
        }

        void determine_ombre_portees(bool d)
        {
            drapeau_projete_ombres=d;
            drapeau_recoit_ombres=d;
            initialise_mode_rendu();
        }

        //--------------------------------------------------------
        //                  Débugage:
        //--------------------------------------------------------
        public:
        void affiche_debug()
        {
            cout<<"            Surface: "<<nom<<endl;
            cout<<"                     double-face                 : "<<boolalpha<<double_face<<endl;
            cout<<"                     angle limite d'adoucissement: "<<angle_limite_adoucissement<<endl;
            cout<<"                     adoucissement               : "<<boolalpha<<drapeau_adoucissement<<endl;
            cout<<"                     diffusion            : "<<dec<<diffusion<<hex<<" couleur: #"<<(((uint32_t)(couleur_diffusion[0]*255)<<16)|((uint32_t)(couleur_diffusion[0]*255)<<8)|(uint32_t)(couleur_diffusion[0]*255))<<endl;
            cout<<"                     specularite          : "<<dec<<specularite<<hex<<" couleur: #"<<(((uint32_t)(couleur_speculaire[0]*255)<<16)|((uint32_t)(couleur_speculaire[0]*255)<<8)|(uint32_t)(couleur_speculaire[0]*255))<<endl;
            cout<<"                     ambiance             : "<<dec<<ambiance<<hex<<" couleur: #"<<(((uint32_t)(couleur_ambiance[0]*255)<<16)|((uint32_t)(couleur_ambiance[0]*255)<<8)|(uint32_t)(couleur_ambiance[0]*255))<<endl;
            cout<<"                     luminescence         : "<<dec<<luminescence<<hex<<" couleur: #"<<(((uint32_t)(couleur_luminescence[0]*255)<<16)|((uint32_t)(couleur_luminescence[0]*255)<<8)|(uint32_t)(couleur_luminescence[0]*255))<<endl;
            cout<<"                     brillance            : "<<dec<<brillance<<endl;
            cout<<"                     transparence         : "<<dec<<transparence<<endl;
            cout<<"                     translucidite        : "<<dec<<translucidite<<endl;
            cout<<"                     reflexion            : "<<dec<<reflexion<<endl;
            cout<<"                     rugosite             : "<<dec<<rugosite<<endl;

        }
};


#endif // DKSSURFACE_H_INCLUDED
