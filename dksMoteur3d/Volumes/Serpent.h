#ifndef SERPENT_H_INCLUDED
#define SERPENT_H_INCLUDED

/*
*****************************************************************************************************

                                    Animation d'un serpent
        Les parties du corps sont dans un fichier LWO2
            Calque 0: la tête
            Calque 1: machoire inférieure
            Calque 2: oeil droit
            Calque 3: oeil gauche

            Le corps est généré par extrusion du raccord de la tête (points 234 à 245)

*******************************************************************************************************
*/

#include "../Elements_Fondamentaux/dksVolume.h"
#include "../LWO/dksChargeLWO2.h"
#include "../LWO/dksConvertionLWO2.h"

//débogage:
#define SERPENT_DEBUG true

//Les type de longueurs pour les segments:

#define SERPENT_SEGMENTS_EGAUX 1 //Les longeurs des segments sont toutes les mêmes.
#define SERPENT_SEGMENTS_PROPORTIONNELS 2 //Le longeurs entre les segments diminuent du même facteur que les diamètres des sections

//Erreurs:

#define SERPENT_OK 0
#define SERPENT_ERREUR_CHARGEMENT_VOLUMES 0x100+1
#define SERPENT_ERREUR_CONVERTION_VOLUMES 0x100+2
#define SERPENT_ERREUR_MEMOIRE 0x100+3

//==================================================================================================
//                      Définition de la classe
//==================================================================================================

class Serpent : public dksVolume
{
    private:
        dksChargeLWO2* volumes_lwo2;
        dksConvertionLWO2* volumes_convertis;

        dksVolume* tete_modele;
        dksVolume* machoire;
        dksVolume* oeil_droit;
        dksVolume* oeil_gauche;
        dksVolume* dents;

        uint32_t tete_corps_nbrSommets;
        double* tete_corps_sommets;

        uint32_t corps_nbrSommets;  //Le nombre de sommets composants le corps
        double* corps_sommets;      //Les sommets dans leurs positions d'origine
        double* corps_sommets_tampon; //Les sommets animés

        uint32_t tete_corps_nbrFaces;
        uint32_t corps_nbrFaces;
        dksFace* tete_corps_faces;
        uint32_t* tables_aretes;
        uint32_t tete_corps_nbrSurfaces;
        dksSurface** tete_corps_surfaces;

        uint32_t decallage_sommets_corps;

        uint8_t type_longueurs_segments; //Longueurs
        uint32_t nbrSegments;
        uint32_t nbrCotes;
        float longueur_corps;

        float longueur_premier_segment; //Sert lorsque type longeurs segments=PROPORTIONNELS
        float facteur_extrusion;    //facteur d'échelle de la dernière section par rapport à la première

        uint32_t indices_cou[12];
        double modele_section[12*3];    //Coordonnées des points de la section-modèle du cou

        double lien_tete_colonne[3]; //Le premier point de la colonne vertébrale dans le repère du volume
        double* colonne_vertebrale;
        double* tangentes_colonne;  //Les repères d'orientations des sections de la colonne vertébrale. Le serpent peut se torsader.
        double* normales_colonne;
        double* binormales_colonne;

        double* colonne_vertebrale_tampon;
        double* tangentes_colonne_tampon;
        double* normales_colonne_tampon;
        double* binormales_colonne_tampon;

        float* longueurs_segments;
        bool drapeau_triangles;         //True si les faces du corps sont des triangle, false si ce sont des rectangles

        bool drapeau_colonne_repere_parent; //True si la colonne vertébrale est située dans le repère du parent(true) ou dans celui du volume(false).

    public:

    //=======================================
    //          Constructeur
    //=======================================
    Serpent(
                char * p_nom, double px, double py, double pz,
                char* nom_fichier,
                uint32_t p_nbrSegments=50,
                float p_longueur_corps=5.,
                float p_facteur_extrusion=0.5,
                uint8_t p_type_longueurs_segments=SERPENT_SEGMENTS_EGAUX,
                bool p_triangles=true,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                )
                :dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
    {
            #if SERPENT_DEBUG
                cout<<"***------------------------Création du Serpent: "<<nom<<endl;
            #endif

            erreur=SERPENT_OK;
            volumes_lwo2=NULL;
            volumes_convertis=NULL;
            tete_modele=NULL;
            colonne_vertebrale=NULL;
            longueurs_segments=NULL;
            tete_corps_sommets=NULL;
            tete_corps_faces=NULL;
            tete_corps_surfaces=NULL;
            tables_aretes=NULL;

            echelle_geometrique=1.;

            drapeau_colonne_repere_parent=false;

            nbrSegments=p_nbrSegments;
            longueur_corps=p_longueur_corps;
            facteur_extrusion=p_facteur_extrusion;
            type_longueurs_segments=p_type_longueurs_segments;
            drapeau_triangles=p_triangles;

            sens_polygones=GL_CW;

        //------------- Chargement des volumes:

            //char nom_fichier[]={"LWO2/snake.lwo"};
            volumes_lwo2=new dksChargeLWO2(nom_fichier);
            if (volumes_lwo2==NULL || volumes_lwo2->erreur!=DKSCHARGELWO2_OK)
            {
                cout<<"ERREUR dans Serpent::Serpent() - Impossible de charger les volumes - "<<nom_fichier<<endl;
                erreur=SERPENT_ERREUR_CHARGEMENT_VOLUMES;
                return;
            }

            volumes_convertis=new dksConvertionLWO2(volumes_lwo2);
            if (volumes_convertis==NULL || volumes_convertis->erreur!=DKSCONVERTIONLWO2_OK)
            {
                cout<<"ERREUR dans Serpent::Serpent() - Impossible de convertir les volumes - "<<nom_fichier<<endl;
                erreur=SERPENT_ERREUR_CONVERTION_VOLUMES;
                return;
            }
        //------------- Assignations:

            tete_modele=volumes_convertis->renvoie_volume_index(0);
            machoire=volumes_convertis->renvoie_volume_index(1);
            oeil_droit=volumes_convertis->renvoie_volume_index(2);
            oeil_gauche=volumes_convertis->renvoie_volume_index(3);
            dents=volumes_convertis->renvoie_volume_index(5);

            if (!creation_corps()) return;
            determine_liste_luminescences(false);
            determine_liste_profondeurs(false);

            //ajoute_enfant(tete_modele);
            ajoute_enfant(machoire);
            ajoute_enfant(oeil_droit);
            ajoute_enfant(oeil_gauche);
            ajoute_enfant(dents);

            //Initialise les repères:
            //transfere_colonne_repere_parent();
    }

    //=======================================
    //          Destructeur
    //=======================================
    ~Serpent()
    {
        if (volumes_lwo2!=NULL) delete volumes_lwo2;
        if (volumes_convertis!=NULL) delete volumes_convertis;
        if(colonne_vertebrale!=NULL) free(colonne_vertebrale);
        if(longueurs_segments!=NULL) free(longueurs_segments);
        if (tables_aretes!=NULL) free(tables_aretes);
    }

    //=====================================================================================
    //              Génère le corps.
    //      -La tête est dupliquée,
    //      -Le corps est ajouté au même volume par extrusion du la base du cou.
    //=====================================================================================
    private:
    bool creation_corps()
    {
        nbrCotes=12;
        longueur_premier_segment=   longueur_corps / ( nbrSegments - (1.-facteur_extrusion) * (nbrSegments+1.) / 2. );

        indices_cou={234,235,236,237,238,239,240,241,242,243,244,245};  //La section de raccord à la tête
        decallage_sommets_corps=tete_modele->nbrSommets;
        corps_nbrSommets=nbrSegments*nbrCotes;
        tete_corps_nbrSommets=tete_modele->nbrSommets+corps_nbrSommets;

        //------- Génère la colone vertébrale:
            if (!genere_colonne_vertebrale()) return false;

        //-------Création des sommets:
            if (!genere_sommets()) return false;
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SOMMETS;

        //------- Création des faces:
            if (!genere_faces()) return false;
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;

        //------- Création des surfaces:
            if (!genere_surfaces()) return false;
            drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SURFACES;

        return true;
    }

    //=============================================================
    //          Génère la colonne vertébrale
    //=============================================================
    bool genere_colonne_vertebrale()
    {
        uint32_t i,n;


        colonne_vertebrale=(double*)malloc((nbrSegments+1)*sizeof(double)*3*8);
        longueurs_segments=(float*)malloc((nbrSegments)*sizeof(float));
        if (colonne_vertebrale==NULL || longueurs_segments==NULL)
        {
            cout<<"ERREUR dans Serpent::creation_corps() - Impossible de créer la colonne vertébrale"<<endl;
            erreur=SERPENT_ERREUR_MEMOIRE;
            return false;
        }
        tangentes_colonne=&colonne_vertebrale[3*(nbrSegments+1)];
        normales_colonne=&tangentes_colonne[3*(nbrSegments+1)];
        binormales_colonne=&normales_colonne[3*(nbrSegments+1)];

        colonne_vertebrale_tampon=&binormales_colonne[3*(nbrSegments+1)];
        tangentes_colonne_tampon=&colonne_vertebrale_tampon[3*(nbrSegments+1)];
        normales_colonne_tampon=&tangentes_colonne_tampon[3*(nbrSegments+1)];
        binormales_colonne_tampon=&normales_colonne_tampon[3*(nbrSegments+1)];


        //1er point de la colonne: centre géométrique des sommets de raccord:
        double* tete_modele_sommets=tete_modele->renvoie_sommets();

        lien_tete_colonne[0]=0.;
        lien_tete_colonne[1]=0.;
        lien_tete_colonne[2]=0.;

        tangentes_colonne[0]=1.;
        tangentes_colonne[1]=0.;
        tangentes_colonne[2]=0.;

        normales_colonne[0]=0.;
        normales_colonne[1]=1.;
        normales_colonne[2]=0.;

        binormales_colonne[0]=0.;
        binormales_colonne[1]=0.;
        binormales_colonne[2]=1.;

        for (i=0;i<nbrCotes;i++)
        {
            n=indices_cou[i];
            lien_tete_colonne[0]+=tete_modele_sommets[3*n];
            lien_tete_colonne[1]+=tete_modele_sommets[3*n+1];
            lien_tete_colonne[2]+=tete_modele_sommets[3*n+2];

        }
        lien_tete_colonne[0]/=double(nbrCotes);
        lien_tete_colonne[1]/=double(nbrCotes);
        lien_tete_colonne[2]/=double(nbrCotes);

        colonne_vertebrale[0]=lien_tete_colonne[0];
        colonne_vertebrale[1]=lien_tete_colonne[1];
        colonne_vertebrale[2]=lien_tete_colonne[2];

        #if SERPENT_DEBUG
                cout<<"Premier point de la colonne: x:"<<colonne_vertebrale[0]<<" y:"<<colonne_vertebrale[1]<<" z:"<<colonne_vertebrale[2]<<endl;
            #endif

        //Coordonnées de la colonne, dans la direction des Z, sens négatif:
        float l=longueur_corps/nbrSegments;

        for (i=1;i<nbrSegments+1;i++)
        {
            colonne_vertebrale[3*i]=colonne_vertebrale[0];
            colonne_vertebrale[3*i+1]=colonne_vertebrale[1];
            if (type_longueurs_segments==SERPENT_SEGMENTS_PROPORTIONNELS)
            {
                l=longueur_premier_segment*(1-i*(1-facteur_extrusion)/nbrSegments);
                colonne_vertebrale[3*i+2]=colonne_vertebrale[(i-1)*3+2]-l;
            }

            else if (type_longueurs_segments==SERPENT_SEGMENTS_EGAUX)
            {
                colonne_vertebrale[3*i+2]=colonne_vertebrale[2]-i*l;
            }
            longueurs_segments[i-1]=l;

            tangentes_colonne[3*i]=1.;
            tangentes_colonne[3*i+1]=0.;
            tangentes_colonne[3*i+2]=0.;

            normales_colonne[3*i]=0.;
            normales_colonne[3*i+1]=1.;
            normales_colonne[3*i+2]=0.;

            binormales_colonne[3*i]=0.;
            binormales_colonne[3*i+1]=0.;
            binormales_colonne[3*i+2]=1.;

        }
        return true;
    }

    //=============================================================
    //          Génère les sommets du corps
    //=============================================================
    bool genere_sommets()
    {
        uint32_t i,j,k;

        //Allocation:
        tete_corps_sommets=(double*)malloc((tete_corps_nbrSommets+corps_nbrSommets)*sizeof(double)*3);

        if (tete_corps_sommets==NULL)
        {
            cout<<"ERREUR dans Serpent::creation_corps() - Impossible d'allouer la table des sommets"<<endl;
            erreur=SERPENT_ERREUR_MEMOIRE;
            return false;
        }

        corps_sommets_tampon=&tete_corps_sommets[3*decallage_sommets_corps];
        corps_sommets=&corps_sommets_tampon[3*corps_nbrSommets];

        //Copie les sommets de la tête:

        double* tete_modele_sommets=tete_modele->renvoie_sommets();
        for (i=0;i<tete_modele->nbrSommets;i++)
        {
            tete_corps_sommets[3*i]=tete_modele_sommets[3*i];
            tete_corps_sommets[3*i+1]=tete_modele_sommets[3*i+1];
            tete_corps_sommets[3*i+2]=tete_modele_sommets[3*i+2];
        }

        //Saisie de la section-modèle qui va être extrudée:
        for (i=0;i<nbrCotes;i++)
        {
            j=indices_cou[i];
            modele_section[3*i]=tete_modele_sommets[3*j]-colonne_vertebrale[0];
            modele_section[3*i+1]=tete_modele_sommets[3*j+1]-colonne_vertebrale[1];
            modele_section[3*i+2]=0.;//tete_modele_sommets[3*j+2]-colonne_vertebrale[2];
        }


        //Génère les sections:
        double pas_f=(1-facteur_extrusion)/nbrSegments;

        for(i=0;i<nbrSegments;i++)
        {
            double f=1.-(i+1)*pas_f;
            for(j=0;j<nbrCotes;j++)
            {
                k=j+i*nbrCotes;
                corps_sommets[3*k]=modele_section[3*j]*f+colonne_vertebrale[3*(i+1)];
                corps_sommets[3*k+1]=modele_section[3*j+1]*f+colonne_vertebrale[3*(i+1)+1];
                corps_sommets[3*k+2]=0.;//modele_section[3*j+2]*f+colonne_vertebrale[3*(i+1)+2];
                corps_sommets_tampon[3*k]=corps_sommets[3*k];
                corps_sommets_tampon[3*k+1]=corps_sommets[3*k+1];
                corps_sommets_tampon[3*k+2]=colonne_vertebrale[3*(i+1)+2];//corps_sommets[3*k+2];
            }
        }


        return determine_sommets(tete_corps_sommets,tete_corps_nbrSommets);
    }

    //====================================================================
    //          Génère les faces du corps et recopie celles de la tête
    //====================================================================
    bool genere_faces()
    {
        //Répartition des surfaces dans un segments:
            char dos[]={"peau"};
            char ventre[]={"peau_ventre"};
            char* repartition_surfaces_segment[]={dos,dos,dos,dos,dos,dos,dos,ventre,ventre,ventre,ventre,dos};

        //Nombre de faces:
            corps_nbrFaces = nbrCotes*nbrSegments;
            if(drapeau_triangles) corps_nbrFaces*=2;
            tete_corps_nbrFaces=corps_nbrFaces+tete_modele->nbrFaces;
            #if SERPENT_DEBUG
                cout<<"Nombre de faces dans le corps: "<<corps_nbrFaces<<endl;
            #endif

        //Allocation de la table des faces:
            dksFace** tete_corps_faces=(dksFace**)calloc(sizeof(dksFace*),tete_corps_nbrFaces);

            if (tete_corps_faces==NULL)
            {
                cout<<"ERREUR dans Serpent::genere_faces - Impossible d'allouer la table des faces"<<endl;
                erreur=SERPENT_ERREUR_MEMOIRE; return false;
            }
        //Copie les faces de la tête:
            uint32_t i;
            for (i=0;i<tete_modele->nbrFaces;i++)
            {
                tete_corps_faces[i]=new dksFace(*(tete_modele->renvoie_face(i)));
            }

        //Génère les faces du corps:
            if (drapeau_triangles) return genere_faces_triangulaires(tete_corps_faces,repartition_surfaces_segment);
            else return genere_faces_rectangulaires(tete_corps_faces,repartition_surfaces_segment);
    }

    bool genere_faces_triangulaires(dksFace** tete_corps_faces, char** repartition_surfaces_segment)
    {
        return true;
    }

    bool genere_faces_rectangulaires(dksFace** tete_corps_faces, char** repartition_surfaces_segment)
    {
        uint32_t f,fp;
        uint16_t i,j,k;

        uint32_t* p_aretes;
        uint32_t p_nbrAretes;

        //Allocation des tables d'aretes:
        p_nbrAretes=4*corps_nbrFaces;
        #if SERPENT_DEBUG
            cout<<"Nombre d'arètes dans le corps: "<<p_nbrAretes<<endl;
        #endif
        tables_aretes=(uint32_t*)malloc(p_nbrAretes*sizeof(uint32_t));
        if (tables_aretes==NULL)
        {
            cout<<"ERREUR dans Serpent::genere_faces_rectangulaires - Impossible d'allouer la table des aretes"<<endl;
            erreur=SERPENT_ERREUR_MEMOIRE;
            return false;
        }

        //La jointure entre la tête et le corps (premier segment):
        fp=0;
        f=tete_modele->nbrFaces;
        p_aretes=tables_aretes;

        for (j=0;j<nbrCotes;j++)
        {
            p_aretes[0]=indices_cou[j];
            p_aretes[1]=fp+decallage_sommets_corps;
            p_aretes[2]=((fp+1)%nbrCotes)+decallage_sommets_corps;
            p_aretes[3]=indices_cou[(j+1)%nbrCotes];
            tete_corps_faces[f++]=new dksFace(4,p_aretes,repartition_surfaces_segment[j],DKSFACE_LIBERE_RIEN);
            fp++;
            p_aretes=&p_aretes[4];
        }
        //Le corps:

        fp=0;

        for(i=1;i<nbrSegments;i++)
        {

           for (j=0;j<nbrCotes;j++)
            {
                p_aretes[0]=fp+decallage_sommets_corps;
                p_aretes[1]=fp+nbrCotes+decallage_sommets_corps;
                p_aretes[2]=i*nbrCotes+((fp+1)%nbrCotes)+decallage_sommets_corps;
                p_aretes[3]=(i-1)*nbrCotes+((fp+1)%nbrCotes)+decallage_sommets_corps;
                tete_corps_faces[f++]=new dksFace(4,p_aretes,repartition_surfaces_segment[j],DKSFACE_LIBERE_RIEN);
                fp++;
                p_aretes=&p_aretes[4];
            }
        }
        return determine_faces(tete_corps_faces,tete_corps_nbrFaces);//tete_modele->nbrFaces
    }

    //====================================================================
    //          Génère les surfaces du corps et recopie celles de la tête
    //====================================================================
    bool genere_surfaces()
    {
        tete_corps_nbrSurfaces=tete_modele->nbrSurfaces;
        tete_corps_surfaces=(dksSurface**)calloc(sizeof(dksSurface*),tete_corps_nbrSurfaces);
        if (tete_corps_surfaces==NULL)
        {
            cout<<"ERREUR dans Serpent::genere_surfaces - Impossible d'allouer la table des surfaces"<<endl;
            erreur=SERPENT_ERREUR_MEMOIRE;
            return false;
        }

        uint32_t i;
        for (i=0;i<tete_corps_nbrSurfaces;i++)
        {
            tete_corps_surfaces[i]=new dksSurface(*(tete_modele->renvoie_surface(i)));
        }

        return determine_surfaces(tete_corps_surfaces,tete_corps_nbrSurfaces);
    }

    //===========================================================
    //              Fonctions détermine/renvoie
    //===========================================================
    public:

    virtual void determine_mode_rendu(uint8_t p_mode)
    {
        this->dksVolume::determine_mode_rendu(p_mode);
        machoire->determine_mode_rendu(p_mode);
        oeil_droit->determine_mode_rendu(p_mode);
        oeil_gauche->determine_mode_rendu(p_mode);
        dents->determine_mode_rendu(p_mode);
    }

    dksVolume* renvoie_tete()
    {
        return tete_modele;
    }

    dksVolume* renvoie_machoire()
    {
        return machoire;
    }

    dksVolume* renvoie_dents()
    {
        return dents;
    }

    dksVolume* renvoie_oeil_droit()
    {
        return oeil_droit;
    }

    dksVolume* renvoie_oeil_gauche()
    {
        return oeil_gauche;
    }

    //---------------------------------------
    //  Place les points de la colonne le long de la trajectoire.
    //  La trajectoire doit être situé dans le repère parent du serpent.
    /// Au besoin, développer la reconnaissance du drapeau_repere_parent
    //  position_depart:la position du premier points de la colonne (lien tête-colonne)
    //  direction: true=colonne dans le sens de la trajectoire
    //             false= colonne dans le sens inverse de la trajectoire
    //---------------------------------------
    void determine_trajectoire_colonne_vertebrale(dksTrajectoire* trajectoire, float position_depart=0., bool direction=true)
    {

        uint32_t i;
        float position[3];
        float longueur_trajectoire=trajectoire->renvoie_longueur_totale();
        float pt=position_depart*longueur_trajectoire;

        //Positionne la tête:

        float distance_tete_lien_colonne=sqrtf(powf(float(lien_tete_colonne[0]),2)+powf(float(lien_tete_colonne[1]),2)+powf(float(lien_tete_colonne[2]),2));
        trajectoire->renvoie_position(pt/longueur_trajectoire,position);
        determine_position(double(position[0]),double(position[1]),double(position[2]));
        //Alignement à la trajectoire:
        trajectoire->renvoie_tangente(pt/longueur_trajectoire,position);
        double nouvel_axez[3]={double(position[0]),double(position[1]),double(position[2])};
        if (direction){nouvel_axez={-nouvel_axez[0],-nouvel_axez[1],-nouvel_axez[2]};}
        initialisation_repere_orthonorme(NULL,NULL,nouvel_axez);
        if(direction)pt+=distance_tete_lien_colonne; else pt-=distance_tete_lien_colonne;

        //Positionne la colonne:
        for (i=0;i<nbrSegments+1;i++)
        {
            trajectoire->renvoie_position(pt/longueur_trajectoire,position);
            colonne_vertebrale[3*i]=double(position[0]);
            colonne_vertebrale[3*i+1]=double(position[1]);
            colonne_vertebrale[3*i+2]=double(position[2]);
            if (i<nbrSegments) {if(direction)pt+=longueurs_segments[i]; else pt-=longueurs_segments[i];}
        }
    }


    virtual void determine_echelle_geometrique(double e)
    {
        double f=e/echelle_geometrique;
        tete_modele->determine_echelle_geometrique(e);
        oeil_droit->determine_echelle_geometrique(e);
        oeil_gauche->determine_echelle_geometrique(e);
        machoire->determine_echelle_geometrique(e);
        dents->determine_echelle_geometrique(e);

        double position[3]={machoire->x,machoire->y,machoire->z};
        position[0]*=f;
        position[1]*=f;
        position[2]*=f;
        machoire->determine_position(position);

        position={dents->x,dents->y,dents->z};
        position[0]*=f;
        position[1]*=f;
        position[2]*=f;
        dents->determine_position(position);

        position={oeil_droit->x,oeil_droit->y,oeil_droit->z};
        position[0]*=f;
        position[1]*=f;
        position[2]*=f;
        oeil_droit->determine_position(position);

        position={oeil_gauche->x,oeil_gauche->y,oeil_gauche->z};
        position[0]*=f;
        position[1]*=f;
        position[2]*=f;
        oeil_gauche->determine_position(position);

        lien_tete_colonne[0]*=f;
        lien_tete_colonne[1]*=f;
        lien_tete_colonne[2]*=f;

        uint32_t i;
        for (i=0;i<nbrSegments;i++)
        {
            colonne_vertebrale[3*i]*=f;
            colonne_vertebrale[3*i+1]*=f;
            colonne_vertebrale[3*i+2]*=f;
            longueurs_segments[i]*=f;
        }
            colonne_vertebrale[3*i]*=f;
            colonne_vertebrale[3*i+1]*=f;
            colonne_vertebrale[3*i+2]*=f;


        for(i=0;i<tete_modele->nbrSommets;i++)
        {
            tete_corps_sommets[3*i]*=f;
            tete_corps_sommets[3*i+1]*=f;
            tete_corps_sommets[3*i+2]*=f;
        }

        for(i=0;i<corps_nbrSommets;i++)
        {
            corps_sommets[3*i]*=f;
            corps_sommets[3*i+1]*=f;
            corps_sommets[3*i+2]*=f;
            corps_sommets_tampon[3*i]*=f;
            corps_sommets_tampon[3*i+1]*=f;
            corps_sommets_tampon[3*i+2]*=f;
        }


        //this->dksVolume::determine_echelle_geometrique(e);
        echelle_geometrique=e;
    }

    //===========================================================================================================================================
    //                                                              Mouvements
    //===========================================================================================================================================
    private:
    //-------------------------------------------------------------------------------------------------------------------------
    //      Le corps suit la trajectoire de la tête
    //  Les coordonnées de la colonne vertébrale sont
    //  situées dans le repère du conteneur parent.
    //  Utiliser la fonction transfere_colonne_repere_parent() pour positionner correctement la colonne par rapport à la tête
    //  Pour utiliser les fonctions d'animation dans le repère du volume: transfere_colonne_repere_volume()
    //-------------------------------------------------------------------------------------------------------------------------

    void trajectoire_tete()
    {
        uint32_t i,j;

        double pA[3];
        double pB[3];
        double ABx,ABy,ABz;
        double n;

        //------- Positionne la colonne vertébrale par rapport à la tête:

        //Point d'attache dans le repère du parent:
        dksRepere::translation_point(lien_tete_colonne,this,colonne_vertebrale);
        pA={colonne_vertebrale[0],colonne_vertebrale[1],colonne_vertebrale[2]};
        tangentes_colonne[0]=this->axe_x->x;
        tangentes_colonne[1]=this->axe_x->y;
        tangentes_colonne[2]=this->axe_x->z;
        normales_colonne[0]=this->axe_y->x;
        normales_colonne[1]=this->axe_y->y;
        normales_colonne[2]=this->axe_y->z;
        binormales_colonne[0]=this->axe_z->x;
        binormales_colonne[1]=this->axe_z->y;
        binormales_colonne[2]=this->axe_z->z;

        //Calcul les points de la colonne vertébrale:
        for (i=0;i<nbrSegments;i++)
        {
            j=(i+1)*3;
            pB={colonne_vertebrale[j],colonne_vertebrale[j+1],colonne_vertebrale[j+2]};
            ABx=pB[0]-pA[0];
            ABy=pB[1]-pA[1];
            ABz=pB[2]-pA[2];
            n=sqrt(ABx*ABx+ABy*ABy+ABz*ABz);
            n=longueurs_segments[i]/n;
            ABx*=n;
            ABy*=n;
            ABz*=n;

            colonne_vertebrale[j]=ABx+pA[0];
            colonne_vertebrale[j+1]=ABy+pA[1];
            colonne_vertebrale[j+2]=ABz+pA[2];

            pA={colonne_vertebrale[j],colonne_vertebrale[j+1],colonne_vertebrale[j+2]};
        }

        //--------- Calcule les repères des sections:

        calcul_reperes_TNB_colonne();

        //--------- Transfère de repère:
        //Copie le point d'attache:

        colonne_vertebrale_tampon[0]=lien_tete_colonne[0];
        colonne_vertebrale_tampon[1]=lien_tete_colonne[1];
        colonne_vertebrale_tampon[2]=lien_tete_colonne[2];

        tangentes_colonne_tampon[0]=1.;
        tangentes_colonne_tampon[1]=0.;
        tangentes_colonne_tampon[2]=0.;

        normales_colonne_tampon[0]=0.;
        normales_colonne_tampon[1]=1.;
        normales_colonne_tampon[2]=0.;

        binormales_colonne_tampon[0]=0.;
        binormales_colonne_tampon[1]=0.;
        binormales_colonne_tampon[2]=1.;

        //Transfère de repère de la colonne vertébrale:
        dksRepere::points_relatifs(&colonne_vertebrale[3],this,&colonne_vertebrale_tampon[3],nbrSegments);
        dksRepere::axes_relatifs(&tangentes_colonne[3],this,&tangentes_colonne_tampon[3],nbrSegments);
        dksRepere::axes_relatifs(&normales_colonne[3],this,&normales_colonne_tampon[3],nbrSegments);
        dksRepere::axes_relatifs(&binormales_colonne[3],this,&binormales_colonne_tampon[3],nbrSegments);
    }

    void calcul_reperes_TNB_colonne()
    {
        double Ax,Ay,Az;
        double Bx,By,Bz;
        double Cx,Cy,Cz;
        double ABx,ABy,ABz;
        double BCx,BCy,BCz;
        double ACx,ACy,ACz;
        double Tx,Ty,Tz;    //Vecteur tampon;
        double n;
        uint32_t i;

        Ax=colonne_vertebrale[0];Ay=colonne_vertebrale[1];Az=colonne_vertebrale[2];
        Bx=colonne_vertebrale[3];By=colonne_vertebrale[4];Bz=colonne_vertebrale[5];


        for (i=1;i<nbrSegments;i++)
        {
            Cx=colonne_vertebrale[3*(i+1)];Cy=colonne_vertebrale[3*(i+1)+1];Cz=colonne_vertebrale[3*(i+1)+2];
            ABx=Bx-Ax;ABy=By-Ay;ABz=Bz-Az;  //A optimiser....
            BCx=Cx-Bx;BCy=Cy-By;BCz=Cz-Bz;

            //---- Calcul de la binormale:
            n=longueurs_segments[i-1];//sqrt(ABx*ABx+ABy*ABy+ABz*ABz);
            ABx/=n;ABy/=n;ABz/=n;
            n=longueurs_segments[i];//sqrt(BCx*BCx+BCy*BCy+BCz*BCz);
            BCx/=n;BCy/=n;BCz/=n;

            ACx=ABx+BCx; ACy=ABy+BCy; ACz=ABz=BCz;
            n=sqrt(ACx*ACx+ACy*ACy+ACz*ACz);
            ACx/=-n; ACy/=-n; ACz/=-n;

            binormales_colonne[i*3]=ACx;
            binormales_colonne[i*3+1]=ACy;
            binormales_colonne[i*3+2]=ACz;

            //----- Calcul de la normale:
            /*
            //T = N ^ B


            n=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);
            Tx/=n;
            Ty/=n;
            Tz/=n;

            //N' = B ^ T
            normales_colonne[i*3]=ACy*Tz-ACz*Ty;
            normales_colonne[i*3+1]=ACz*Tx-ACx*Tz;
            normales_colonne[i*3+2]=ACx*Ty-ACy*Tx;
            */

            Tx=this->axe_y->y*ACz-this->axe_y->z*ACy;
            Ty=this->axe_y->z*ACx-this->axe_y->x*ACz;
            Tz=this->axe_y->x*ACy-this->axe_y->y*ACx;

            n=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);


            if (n<1e-4)
            {
                Tx=normales_colonne[i*3+1]*ACz-normales_colonne[i*3+2]*ACy;
                Ty=normales_colonne[i*3+2]*ACx-normales_colonne[i*3]*ACz;
                Tz=normales_colonne[i*3]*ACy-normales_colonne[i*3+1]*ACx;
                n=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);
            }


            Tx/=n;
            Ty/=n;
            Tz/=n;
            normales_colonne[i*3]=ACy*Tz-ACz*Ty;
            normales_colonne[i*3+1]=ACz*Tx-ACx*Tz;
            normales_colonne[i*3+2]=ACx*Ty-ACy*Tx;

            //------ Correction de torsion:
            /*
            if (normales_colonne[i*3]*normales_colonne[(i-1)*3]+normales_colonne[i*3+1]*normales_colonne[(i-1)*3+1]+normales_colonne[i*3+2]*normales_colonne[(i-1)*3+2]<0.)
            {
                normales_colonne[i*3]=-normales_colonne[i*3];
                normales_colonne[i*3+1]=-normales_colonne[i*3+1];
                normales_colonne[i*3+2]=-normales_colonne[i*3+2];
            }
            */


            //------ Calcul de la tangente:
            tangentes_colonne[i*3]=normales_colonne[i*3+1]*binormales_colonne[i*3+2]-normales_colonne[i*3+2]*binormales_colonne[i*3+1];
            tangentes_colonne[i*3+1]=normales_colonne[i*3+2]*binormales_colonne[i*3]-normales_colonne[i*3]*binormales_colonne[i*3+2];
            tangentes_colonne[i*3+2]=normales_colonne[i*3]*binormales_colonne[i*3+1]-normales_colonne[i*3+1]*binormales_colonne[i*3];

            //------ Segment suivant:
            Ax=Bx; Ay=By; Az=Bz;
            Bx=Cx; By=Cy; Bz=Cz;
        }

        // -----Dernière section:
            ABx=Bx-Ax;ABy=By-Ay;ABz=Bz-Az;
            n=sqrt(ABx*ABx+ABy*ABy+ABz*ABz);
            ABx/=-n;ABy/=-n;ABz/=-n;
            binormales_colonne[i*3]=ABx;
            binormales_colonne[i*3+1]=ABy;
            binormales_colonne[i*3+2]=ABz;
            //----- Calcul de la normale:

            Tx=this->axe_y->y*ABz-this->axe_y->z*ABy;
            Ty=this->axe_y->z*ABx-this->axe_y->x*ABz;
            Tz=this->axe_y->x*ABy-this->axe_y->y*ABx;

            n=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);

            if (n<1e-4)
            {
                Tx=normales_colonne[i*3+1]*ABz-normales_colonne[i*3+2]*ABy;
                Ty=normales_colonne[i*3+2]*ABx-normales_colonne[i*3]*ABz;
                Tz=normales_colonne[i*3]*ABy-normales_colonne[i*3+1]*ABx;
                n=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);
            }

            Tx/=n;
            Ty/=n;
            Tz/=n;
            //N' = B ^ T
            normales_colonne[i*3]=ABy*Tz-ABz*Ty;
            normales_colonne[i*3+1]=ABz*Tx-ABx*Tz;
            normales_colonne[i*3+2]=ABx*Ty-ABy*Tx;

            //------ Calcul de la tangente:
            tangentes_colonne[i*3]=normales_colonne[i*3+1]*binormales_colonne[i*3+2]-normales_colonne[i*3+2]*binormales_colonne[i*3+1];
            tangentes_colonne[i*3+1]=normales_colonne[i*3+2]*binormales_colonne[i*3]-normales_colonne[i*3]*binormales_colonne[i*3+2];
            tangentes_colonne[i*3+2]=normales_colonne[i*3]*binormales_colonne[i*3+1]-normales_colonne[i*3+1]*binormales_colonne[i*3];
    }

    public:
    //--------
    //      Transfère les coordonnées de la colonne vertébrale dans le repère du parent ou du volume selon l'usage.
    //      Le premier point est le point d'attache avec la tête, ses coordonnées dans le volume sont situées dans lien_tete_colonne[]
    //--------
    void transfere_colonne_repere_parent()
    {
        dksRepere::translation_points(colonne_vertebrale,this,colonne_vertebrale,nbrSegments+1);
        dksRepere::translation_axes(tangentes_colonne,this,tangentes_colonne,nbrSegments+1);
        dksRepere::translation_axes(normales_colonne,this,normales_colonne,nbrSegments+1);
        dksRepere::translation_axes(binormales_colonne,this,binormales_colonne,nbrSegments+1);
        drapeau_colonne_repere_parent=true;
    }

    void transfere_colonne_repere_volume()
    {
        dksRepere::points_relatifs(colonne_vertebrale,this,colonne_vertebrale,nbrSegments+1);
        dksRepere::axes_relatifs(tangentes_colonne,this,tangentes_colonne,nbrSegments+1);
        dksRepere::axes_relatifs(normales_colonne,this,normales_colonne,nbrSegments+1);
        dksRepere::axes_relatifs(binormales_colonne,this,binormales_colonne,nbrSegments+1);
        colonne_vertebrale[0]=lien_tete_colonne[0];
        colonne_vertebrale[1]=lien_tete_colonne[1];
        colonne_vertebrale[2]=lien_tete_colonne[2];
        tangentes_colonne[0]=this->axe_x->x;
        tangentes_colonne[1]=this->axe_x->y;
        tangentes_colonne[2]=this->axe_x->z;
        normales_colonne[0]=this->axe_y->x;
        normales_colonne[1]=this->axe_y->y;
        normales_colonne[2]=this->axe_y->z;
        binormales_colonne[0]=this->axe_z->x;
        binormales_colonne[1]=this->axe_z->y;
        binormales_colonne[2]=this->axe_z->z;
        drapeau_colonne_repere_parent=false;

    }
    //-----------------------------------------
    //      Ondulation de base pour les tests
    //-----------------------------------------
    private:
    void ondulation_sinusoidale()
    {
        float frequence=.5;
        float amplitude=1.;
        float vitesse=3.;
        int32_t i=0;

        double px=colonne_vertebrale[0]+amplitude*sin(-i*frequence+position_temps*vitesse);
        double py=colonne_vertebrale[1];
        double pz=colonne_vertebrale[2];

        colonne_vertebrale_tampon[0]=colonne_vertebrale[0];
        colonne_vertebrale_tampon[1]=colonne_vertebrale[1];
        colonne_vertebrale_tampon[2]=colonne_vertebrale[2];

        tangentes_colonne_tampon[0]=tangentes_colonne[0];
        tangentes_colonne_tampon[1]=tangentes_colonne[1];
        tangentes_colonne_tampon[2]=tangentes_colonne[2];

        normales_colonne_tampon[0]=normales_colonne[0];
        normales_colonne_tampon[1]=normales_colonne[1];
        normales_colonne_tampon[2]=normales_colonne[2];

        binormales_colonne_tampon[0]=binormales_colonne[0];
        binormales_colonne_tampon[1]=binormales_colonne[1];
        binormales_colonne_tampon[2]=binormales_colonne[2];


        for(i=1;i<nbrSegments+1;i++)
        {
            colonne_vertebrale_tampon[3*i]=colonne_vertebrale[3*i]+amplitude*sin(-i*frequence+position_temps*vitesse)-px;
            colonne_vertebrale_tampon[3*i+1]=colonne_vertebrale[3*i+1];
            colonne_vertebrale_tampon[3*i+2]=colonne_vertebrale[3*i+2];

            //colonne_vertebrale_tampon[3*i]=colonne_vertebrale[3*i];
            //colonne_vertebrale_tampon[3*i+1]=colonne_vertebrale[3*i+1];
            //colonne_vertebrale_tampon[3*i+2]=colonne_vertebrale[3*i+2];

            tangentes_colonne_tampon[3*i]=tangentes_colonne[3*i];
            tangentes_colonne_tampon[3*i+1]=tangentes_colonne[3*i+1];
            tangentes_colonne_tampon[3*i+2]=tangentes_colonne[3*i+2];

            normales_colonne_tampon[3*i]=normales_colonne[3*i];
            normales_colonne_tampon[3*i+1]=normales_colonne[3*i+1];
            normales_colonne_tampon[3*i+2]=normales_colonne[3*i+2];

            binormales_colonne_tampon[3*i]=binormales_colonne[3*i];
            binormales_colonne_tampon[3*i+1]=binormales_colonne[3*i+1];
            binormales_colonne_tampon[3*i+2]=binormales_colonne[3*i+2];

        }
    }

    //-------------------------------------------------------------------
    //      Positionne les sections le long de la colonne vertébrale
    //-------------------------------------------------------------------
    void rafraichi_sections()
    {
        uint32_t i,j,k,m;
        double px,py,pz;

        for(i=0;i<nbrSegments;i++)
        {
            for(j=0;j<nbrCotes;j++)
            {
                k=3*(i*nbrCotes+j);
                m=3*(i+1);
                px=corps_sommets[k];
                py=corps_sommets[k+1];
                pz=corps_sommets[k+2];

                corps_sommets_tampon[k]=colonne_vertebrale_tampon[m]
                                        +px*tangentes_colonne_tampon[m]
                                        +py*normales_colonne_tampon[m]
                                        +pz*binormales_colonne_tampon[m];

                corps_sommets_tampon[k+1]=colonne_vertebrale_tampon[m+1]
                                        +px*tangentes_colonne_tampon[m+1]
                                        +py*normales_colonne_tampon[m+1]
                                        +pz*binormales_colonne_tampon[m+1];

                corps_sommets_tampon[k+2]=colonne_vertebrale_tampon[m+2]
                                        +px*tangentes_colonne_tampon[m+2]
                                        +py*normales_colonne_tampon[m+2]
                                        +pz*binormales_colonne_tampon[m+2];
            }
        }

        rafraichi_normales();

    }

    void rafraichi_normales()
    {
        uint32_t i;
        if (sens_polygones==GL_CW) calcul_normales_faces_CW(tete_modele->nbrFaces,corps_nbrFaces);
        else calcul_normales_faces_CCW(tete_modele->nbrFaces,corps_nbrFaces);
        calcul_normales_sommets(decallage_sommets_corps,corps_nbrSommets);
    }


    //-------------------------------------------------------
    //          Rendu des mouvements:
    //-------------------------------------------------------
    public:
    virtual void applique_mouvements()
    {
        dksVolume::applique_mouvements();
        trajectoire_tete();
        //ondulation_sinusoidale();
        rafraichi_sections();
    }

    //============================================================
    //          Affichage
    //============================================================
    public:

    /*
    virtual void affiche(dksOeil* oeil)
    {
        //glPointSize(3);
        //mode_remplissage=DKSVOLUME_REMPLISSAGE_ARETES;
        dksVolume::affiche(oeil);
        //affiche_colonne();
        //affiche_reperes_colonne();
        //affiche_corps();
    }
    */

    private:
    void affiche_colonne()
    {
        uint16_t i;
        GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        glColor3f(1.,.5,1.);

        glBegin(GL_POINTS);
        for(i=0;i<nbrSegments+1;i++)
        {
            glVertex3dv(&colonne_vertebrale_tampon[i*3]);
        }
        glEnd();

        glColor3f(0.5,0.25,0.5);
        glBegin(GL_LINE_STRIP);
        for(i=0;i<nbrSegments+1;i++)
        {
            glVertex3dv(&colonne_vertebrale_tampon[i*3]);
        }
        glEnd();

        if (lum_mem)glEnable(GL_LIGHTING);
    }

    void affiche_reperes_colonne()
    {
        uint16_t i;
        GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);

        //Tangentes:

        glColor3f(1.,0.,0.);
        glBegin(GL_LINES);
        for(i=0;i<nbrSegments+1;i++)
        {

            glVertex3dv(&colonne_vertebrale_tampon[i*3]);
            glVertex3d(
                       colonne_vertebrale_tampon[i*3]+tangentes_colonne_tampon[i*3],
                       colonne_vertebrale_tampon[i*3+1]+tangentes_colonne_tampon[i*3+1],
                       colonne_vertebrale_tampon[i*3+2]+tangentes_colonne_tampon[i*3+2]
                       );
        }
        glEnd();

        //Normales:

        glColor3f(0.,1.,0.);
        glBegin(GL_LINES);
        for(i=0;i<nbrSegments+1;i++)
        {

            glVertex3dv(&colonne_vertebrale_tampon[i*3]);
            glVertex3d(
                       colonne_vertebrale_tampon[i*3]+normales_colonne_tampon[i*3],
                       colonne_vertebrale_tampon[i*3+1]+normales_colonne_tampon[i*3+1],
                       colonne_vertebrale_tampon[i*3+2]+normales_colonne_tampon[i*3+2]
                       );
        }
        glEnd();

        //Binormales:

        glColor3f(0.,0.,1.);
        glBegin(GL_LINES);
        for(i=0;i<nbrSegments+1;i++)
        {
            glVertex3dv(&colonne_vertebrale_tampon[i*3]);
            glVertex3d(
                       colonne_vertebrale_tampon[i*3]+binormales_colonne_tampon[i*3],
                       colonne_vertebrale_tampon[i*3+1]+binormales_colonne_tampon[i*3+1],
                       colonne_vertebrale_tampon[i*3+2]+binormales_colonne_tampon[i*3+2]
                       );
        }
        glEnd();


        if (lum_mem)glEnable(GL_LIGHTING);
    }



    void affiche_corps()
    {
        uint16_t i,j;
        GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        glColor3f(1.,.5,1.);

        /*
        glBegin(GL_POINTS);
        for(i=0;i<corps_nbrSommets;i++)
        {
            glVertex3dv(&corps_sommets_tampon[i*3]);
        }
        glEnd();
        */

        glColor3f(0.5,0.25,0.5);
        for(j=0;j<nbrSegments;j++)
        {
            glBegin(GL_LINE_LOOP);
            for(i=0;i<nbrCotes;i++)
            {
                glVertex3dv(&corps_sommets_tampon[(i+j*nbrCotes)*3]);
            }
            glEnd();
        }

        if (lum_mem)glEnable(GL_LIGHTING);
    }

};

#endif // SERPENT_H_INCLUDED
