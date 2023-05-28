#ifndef ANNELIDE_H_INCLUDED
#define ANNELIDE_H_INCLUDED

#include "../Elements_Fondamentaux/dksVolume.h"

#define ANNELIDE_DEBUG false
#define ANNELIDE_DEBUG_UVS false

#define ANNELIDE_SURFACE_TETE 0
#define ANNELIDE_SURFACE_DOS 1
#define ANNELIDE_SURFACE_VENTRE 2
#define ANNELIDE_SURFACE_QUEUE 3
/*
==================================================================================================================

                                    Génère un ver.
                Terminologie:
                    Section:tranche circulaire formée par des sommets
                    Anneau=intervalle entre deux sections

==================================================================================================================
*/

class Annelide :public dksVolume
{
    private:
        uint16_t nbrSections;
        uint16_t nbrCotes;
        double longueur;
        double rayon_min;
        double rayon_max;
        double nombre_bulbes;

        bool drapeau_double_faces;  //True si les faces sont affichées des deux côtés

        double* colonne_fixe; //Les centres des sections, qui servent à l'ondulation
        double* colonne;

        double* sommets_fixes;

        uint32_t* tables_aretes;
        bool drapeau_triangles; //True si les faces sont des triangles. False si ce sont des quadrilatères.

        dksSurface* surfaces_annelide[4]; //surface_tete, surface_dos, surface_ventre, surface_queue

        double vecteur_generateur_position[3];   //Le vecteur qui génère l'ondulation (mouvement de la tête)
        double vecteur_generateur_orientation[3];   //Le vecteur qui génère l'ondulation (mouvement de la tête)

        Bruit vecteur_generateur_x;
        Bruit vecteur_generateur_y;
        Bruit vecteur_generateur_z;

        double taille_section;

        //Uvs:
        uint32_t premier_sommet_dos; //dans une section, l'indice relatif du premier sommet ou démarre le dos
        uint32_t premier_sommet_ventre; //dans une section, l'indice relatif du premier sommet ou démarre le ventre
        uint32_t nbr_cotes_ventre;
        uint32_t nbr_cotes_dos;

        float* uvs_texture_sections_dos;    //Les coordonnées UV pour les textures se répétant à chaque section
        float* uvs_texture_sections_ventre;    //Les coordonnées UV pour les textures se répétant à chaque section
        float* uvs_texture_tete;
        float* uvs_texture_queue;

        uint32_t* indices_uvs_dos;
        uint32_t* indices_uvs_ventre;
        uint32_t* indices_uvs_tete;
        uint32_t* indices_uvs_queue;

    public:
        float elasticite;

        float amplitude_perturbation_x;
        float amplitude_perturbation_y;
        float intervalle_perturbation_x;
        float intervalle_perturbation_y;

        double amplitude_ondulatoire_x;
        double vitesse_ondulatoire_x; //en rad/s

        double amplitude_ondulatoire_y;
        double vitesse_ondulatoire_y; //en rad/s


    //==============================================================================================
    //                                  Constructeur
    //==============================================================================================
    Annelide(   char* p_nom=NULL,
                double px = 0., double py = 0., double pz = 0.,
                uint16_t p_nbrSections=10, uint16_t p_nbrCotes=24,
                double p_rayon_min=.75,double p_rayon_max=1.,
                double p_nombre_bulbes=1., double p_longueur=5.,
                uint32_t p_couleur=0xffffff00,
                bool p_triangles=true,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                ):dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
                {
                    nbrSections=p_nbrSections;
                    nbrCotes=p_nbrCotes;
                    rayon_min=p_rayon_min;
                    rayon_max=p_rayon_max;
                    longueur=p_longueur;
                    tables_aretes=NULL;
                    surfaces_annelide={NULL,NULL,NULL,NULL};
                    drapeau_triangles=p_triangles;
                    colonne=NULL;
                    colonne_fixe=NULL;
                    sommets_fixes=NULL;
                    nombre_bulbes=p_nombre_bulbes;
                    uvs_texture_sections_dos=NULL;
                    uvs_texture_sections_ventre=NULL;
                    uvs_texture_queue=NULL;
                    uvs_texture_tete=NULL;
                    indices_uvs_dos=NULL;
                    indices_uvs_ventre=NULL;
                    indices_uvs_tete=NULL;
                    indices_uvs_queue=NULL;

                    sens_polygones=GL_CCW;

                    if (!creation_colonne()) return;
                    if (!creation_sommets()) return;

                    if (!creation_faces())
                    {
                        cout<<"Erreur dans Annelide::creation_faces()- mémoire insuffisante"<<endl; //Le message est ici, pour alléger creation_faces()
                        return;
                    }
                    drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;

                    if (!creation_surfaces(p_couleur)) return;
                    //drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SURFACES;

                    determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                    determine_liste_luminescences(false);
                    determine_liste_profondeurs(false);
                    drapeau_rendu_faces_transparentes=true;

                    vecteur_generateur_position[0]=0.;
                    vecteur_generateur_position[1]=0.;
                    vecteur_generateur_position[2]=longueur/2.;
                    vecteur_generateur_orientation[0]=0.;
                    vecteur_generateur_orientation[1]=0.;
                    vecteur_generateur_orientation[2]=1.;

                    //Paramètres de l'ondulation par défaut:
                    amplitude_perturbation_x=2.;
                    amplitude_perturbation_y=2.;
                    intervalle_perturbation_x=0.5;
                    intervalle_perturbation_y=0.75;

                    amplitude_ondulatoire_x=2.;
                    vitesse_ondulatoire_x=.2 * M_PI*2.; //en rad/s

                    amplitude_ondulatoire_y=3.;
                    vitesse_ondulatoire_y=.3 * M_PI*2.; //en rad/s

                    elasticite=3.;

                    //mode_remplissage=DKSVOLUME_REMPLISSAGE_ARETES;
                }

    //==============================
    //  Destructeur
    //==============================
    ~Annelide()
    {
        if (colonne_fixe!=NULL) delete colonne_fixe;
        if (sommets_fixes!=NULL) delete sommets_fixes;
        if (tables_aretes!=NULL) free(tables_aretes);
        if (uvs_texture_sections_dos!=NULL) free(uvs_texture_sections_dos);
        if  (indices_uvs_dos!=NULL) free(indices_uvs_dos);
        ///Ne pas laisser le destructeur du volume détruire les surfaces, car il libère également la table
        /// des surfaces, qui ici est allouée à la construction !!!
        if (surfaces_annelide[0]!=NULL) delete surfaces_annelide[0];

    }

    //==============================================================================================
    //                                  Initialisations
    //==============================================================================================
    private:
    bool creation_colonne()
    {
        uint32_t i;
        colonne_fixe=(double*)malloc(2*(nbrSections+2)*3*sizeof(double));

        if (colonne_fixe==NULL)
        {
            cout<<"Erreur dans Annelide::creation_colonne()- mémoire insuffisante"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        colonne=&colonne_fixe[((nbrSections+2)*3)];

        taille_section=longueur/(nbrSections+1);
        for (i=0;i<nbrSections+2;i++)
        {
            colonne_fixe[i*3]=0.;
            colonne_fixe[i*3+1]=0.;
            colonne_fixe[i*3+2]=longueur/2.-i*taille_section;

            //Recopiage de la colonne fixe dans la colonne animée:
            colonne[3*i]=colonne_fixe[3*i];
            colonne[3*i+1]=colonne_fixe[3*i+1];
            colonne[3*i+2]=colonne_fixe[3*i+2];
        }

        return true;
    }


    bool creation_sommets()
    {
        uint16_t i;
        uint32_t j;
        uint32_t p_nbrSommets=nbrSections*nbrCotes+2;
        sommets_fixes=(double*)calloc((p_nbrSommets)*3,sizeof(double)); //+2 pour les deux sommets aux extrémités du ver.
        double* p_sommets=(double*)calloc((p_nbrSommets)*3,sizeof(double)); //+2 pour les deux sommets aux extrémités du ver.

        if (p_sommets==NULL || sommets_fixes==NULL)
        {
            cout<<"Erreur dans Annelide::creation_sommets()- mémoire insuffisante"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        //-------------Calcul les coordonnées:
        //Tête:
        sommets_fixes[0]=0.;sommets_fixes[1]=0.;sommets_fixes[2]=0.;//longueur/2.;

        //Queue:
        sommets_fixes[(p_nbrSommets-1)*3]=0.;
        sommets_fixes[(p_nbrSommets-1)*3+1]=0.;
        sommets_fixes[(p_nbrSommets-1)*3+2]=-longueur;

        //Corps
        for(i=0;i<nbrSections;i++)
        {
            calcul_coordonnees_section(i,sommets_fixes);
        }

        //Copie dans les sommets mouvants:
        for (j=0;j<p_nbrSommets;j++)
        {
            p_sommets[3*j]=sommets_fixes[3*j];
            p_sommets[3*j+1]=sommets_fixes[3*j+1];
            p_sommets[3*j+2]=sommets_fixes[3*j+2];
        }

        //Initialise les sommets au sein du volume:
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SOMMETS;
        return determine_sommets(p_sommets,p_nbrSommets);
    }

    void calcul_coordonnees_section(uint16_t i,double* p_sommets)
    {
        uint16_t j;
        p_sommets=&p_sommets[3*(1+i*nbrCotes)];
        double pz=-(i+1)*longueur/(nbrSections+1);
        double da=M_PI/nbrCotes*2.;
        double a=M_PI/2.;   //On commence en haut

        double rayon_section=rayon_min+(rayon_max-rayon_min)/2.+(((rayon_max-rayon_min)/2.)*(cos(((double)i/(nbrSections+1))*M_PI*2.*nombre_bulbes)));

        for (j=0;j<nbrCotes;j++)
        {
            p_sommets[j*3]=cos(a)*rayon_section;
            p_sommets[j*3+1]=sin(a)*rayon_section;
            p_sommets[j*3+2]=pz;
            a+=da;
        }
    }

    //------------------------------------------------------------------------
    //      Génère les faces
    //      Eu égard aux déformations, toutes les faces sont triangulaires
    //------------------------------------------------------------------------
    bool creation_faces()
    {
        if (drapeau_triangles) return faces_triangulaires();
        else return faces_quadrangulaires();
    }

    //-----------------------------------------------------------
    //      Les anneaux sont constitués de faces trianglulaires
    //-----------------------------------------------------------
    bool faces_triangulaires()
    {
        uint32_t f,fp;
        uint16_t i,j,k;
        uint32_t p_nbrFaces = 2*nbrCotes + nbrCotes*2*(nbrSections-1);
        #if ANNELIDE_DEBUG
        cout<<"Nombre de faces dans le ver: "<<nom<<" "<<p_nbrFaces<<endl;
        #endif

        //cout<<"nbrFaces:"<<p_nbrFaces<<endl;

        uint32_t* p_aretes;
        uint32_t p_nbrAretes;

        //Allocation de la table des faces:
        dksFace** p_faces=(dksFace**)calloc(p_nbrFaces,sizeof(dksFace*));
        if (p_faces==NULL) {erreur=DKSVOLUME_ERREUR_MEMOIRE; return false; }

        //Allocation des tables d'aretes:
        p_nbrAretes=(3*nbrCotes*2) + (6*nbrCotes)*(nbrSections-1);
        tables_aretes=(uint32_t*)malloc(p_nbrAretes*sizeof(uint32_t));
        if (tables_aretes==NULL) {erreur=DKSVOLUME_ERREUR_MEMOIRE; return false; }


        //La tete:
        f=0;
        p_aretes=tables_aretes;
        for (j=0;j<nbrCotes;j++)
        {
            p_aretes[0]=0;
            p_aretes[1]=1+f;
            p_aretes[2]=((1+f)%nbrCotes)+1;
            p_faces[f++]=new dksFace(3,p_aretes,"surface_tete",DKSFACE_LIBERE_RIEN);
            p_aretes=&p_aretes[3];
        }

        //Le corps:
        char nom_surface_dos[12]={"surface_dos"};
        char nom_surface_ventre[15]={"surface_ventre"};
        char* nom_surface;

        fp=f-(nbrCotes-1);

        for(i=1;i<nbrSections;i++)
        {
           for (j=0;j<nbrCotes;j++)
            {
                uint32_t s1=fp;
                uint32_t s2=fp+nbrCotes;
                uint32_t s3=i*nbrCotes+(fp%nbrCotes)+1;
                uint32_t s4=(i-1)*nbrCotes+(fp%nbrCotes)+1;
                double V1x=sommets_fixes[s2*3]-sommets_fixes[s1*3];
                double V1z=sommets_fixes[s2*3+2]-sommets_fixes[s1*3+2];
                double V2x=sommets_fixes[s3*3]-sommets_fixes[s2*3];
                double V2z=sommets_fixes[s3*3+2]-sommets_fixes[s2*3+2];
                double nY=V1z*V2x-V1x*V2z;
                if(nY>=0.) nom_surface=nom_surface_dos;
                else nom_surface=nom_surface_ventre;

                p_aretes[0]=s1;
                p_aretes[1]=s2;
                p_aretes[2]=s3;
                p_faces[f++]=new dksFace(3,p_aretes,nom_surface,DKSFACE_LIBERE_RIEN);
                p_aretes=&p_aretes[3];

                p_aretes[0]=s1;
                p_aretes[1]=s3;
                p_aretes[2]=s4;
                p_faces[f++]=new dksFace(3,p_aretes,nom_surface,DKSFACE_LIBERE_RIEN);
                p_aretes=&p_aretes[3];

                fp++;
            }
        }

        //La queue:
        for (j=0;j<nbrCotes;j++)
        {
            p_aretes[0]=fp;
            p_aretes[1]=nbrSommets-1;
            p_aretes[2]=(nbrSections-1)*nbrCotes+(fp%nbrCotes)+1;
            p_faces[f++]=new dksFace(3,p_aretes,"surface_queue",DKSFACE_LIBERE_RIEN);
            fp++;
            p_aretes=&p_aretes[3];
        }

        return determine_faces(p_faces,p_nbrFaces);
    }

    //-----------------------------------------------------------
    //      Les anneaux sont constitués de quadrilatères
    //-----------------------------------------------------------
    bool faces_quadrangulaires()
    {
        /*
        uint32_t f,fp;
        uint16_t i,j,k;
        uint32_t p_nbrFaces = nbrCotes*(nbrSections+1); //2*nbrCotes + nbrCotes*2*(nbrSections-1);

        //cout<<"nbrFaces:"<<p_nbrFaces<<endl;

        uint32_t* p_aretes;
        uint32_t p_nbrAretes;

        //Allocation de la table des faces:
        dksFace** p_faces=(dksFace**)calloc(p_nbrFaces,sizeof(dksFace*));
        if (p_faces==NULL) {erreur=DKSVOLUME_ERREUR_MEMOIRE; return false; }

        //Allocation des tables d'aretes:
        p_nbrAretes=(3*nbrCotes*2)+(4*nbrCotes)*(nbrSections-1);
        tables_aretes=(uint32_t*)malloc(p_nbrAretes*sizeof(uint32_t));
        if (tables_aretes==NULL) {erreur=DKSVOLUME_ERREUR_MEMOIRE; return false; }


        //La tete:
        f=0;
        p_aretes=tables_aretes;
        for (j=0;j<nbrCotes;j++)
        {
            p_aretes[0]=0;
            p_aretes[1]=1+f;
            p_aretes[2]=((1+f)%nbrCotes)+1;
            p_faces[f++]=new dksFace(3,p_aretes,"surface_annelide",DKSFACE_LIBERE_RIEN);
            p_aretes=&p_aretes[3];
        }

        //Le corps:
        fp=f-(nbrCotes-1);

        for(i=1;i<nbrSections;i++)
        {
           for (j=0;j<nbrCotes;j++)
            {
                p_aretes[0]=fp;
                p_aretes[1]=fp+nbrCotes;
                p_aretes[2]=i*nbrCotes+(fp%nbrCotes)+1;
                p_aretes[3]=(i-1)*nbrCotes+(fp%nbrCotes)+1;
                p_faces[f++]=new dksFace(4,p_aretes,"surface_annelide",DKSFACE_LIBERE_RIEN);
                fp++;
                p_aretes=&p_aretes[4];
            }
        }

        //La queue:
        for (j=0;j<nbrCotes;j++)
        {
            p_aretes[0]=fp;
            p_aretes[1]=nbrSommets-1;
            p_aretes[2]=(nbrSections-1)*nbrCotes+(fp%nbrCotes)+1;
            p_faces[f++]=new dksFace(3,p_aretes,"surface_annelide",DKSFACE_LIBERE_RIEN);
            fp++;
            p_aretes=&p_aretes[3];
        }

        return determine_faces(p_faces,p_nbrFaces);
        */
        return false;
    }

    //=======================================================================================
    //                      Gestion des surfaces
    //=======================================================================================
    bool creation_surfaces(uint32_t p_couleur)
    {

        surfaces_annelide[ANNELIDE_SURFACE_TETE]=new dksSurface("surface_tete");
        dksSurface* surface=surfaces_annelide[ANNELIDE_SURFACE_TETE];
        surface->determine_couleur_diffusion(0xffffffff,1.);
        surface->determine_couleur_speculaire(0xffffffff,.8);
        surface->brillance=50.;
        surface->angle_limite_adoucissement=130.;
        surface->determine_transparence(0.);
        surface->drapeau_adoucissement=true;
        surface->transparence=0.;

        surfaces_annelide[ANNELIDE_SURFACE_DOS]=new dksSurface("surface_dos");
        surface=surfaces_annelide[ANNELIDE_SURFACE_DOS];
        surface->determine_couleur_diffusion(0xffffffff,1.);
        surface->determine_couleur_speculaire(0xffffffff,.75);
        surface->brillance=50.;
        surface->angle_limite_adoucissement=130.;
        surface->determine_transparence(0.);
        surface->drapeau_adoucissement=true;
        surface->transparence=0.;

        surfaces_annelide[ANNELIDE_SURFACE_VENTRE]=new dksSurface("surface_ventre");
        surface=surfaces_annelide[ANNELIDE_SURFACE_VENTRE];
        surface->determine_couleur_diffusion(0xffffffff,1.);
        surface->determine_couleur_speculaire(0xffffffff,.75);
        surface->brillance=50.;
        surface->angle_limite_adoucissement=130.;
        surface->determine_transparence(0.);
        surface->drapeau_adoucissement=true;
        surface->transparence=0.;

        surfaces_annelide[ANNELIDE_SURFACE_QUEUE]=new dksSurface("surface_queue");
        surface=surfaces_annelide[ANNELIDE_SURFACE_QUEUE];
        surface->determine_couleur_diffusion(0xffffffff,1.);
        surface->determine_couleur_speculaire(0xffffffff,.75);
        surface->brillance=50.;
        surface->angle_limite_adoucissement=130.;
        surface->determine_transparence(0.);
        surface->drapeau_adoucissement=true;
        surface->transparence=0.;

        determine_double_faces(false);

        return determine_surfaces(surfaces_annelide,4);
    }

    //-----------------------------------------------------------
    //      Renvoie la surface via son identifiant (cf entête)
    //-----------------------------------------------------------
    public:
    dksSurface* renvoie_surface_id(uint8_t id)
    {
        if (id>=nbrSurfaces) return NULL;
        return surfaces_annelide[id];
    }

    //-----------------------------------------------------------
    //      Détermine si les surfaces sont affichées en double-face
    //-----------------------------------------------------------
    void determine_double_faces(bool p_drapeau)
    {
        drapeau_double_faces=p_drapeau;
        surfaces_annelide[ANNELIDE_SURFACE_DOS]->double_face=p_drapeau;
        surfaces_annelide[ANNELIDE_SURFACE_VENTRE]->double_face=p_drapeau;
        surfaces_annelide[ANNELIDE_SURFACE_TETE]->double_face=p_drapeau;
        surfaces_annelide[ANNELIDE_SURFACE_QUEUE]->double_face=p_drapeau;
    }

    bool renvoie_double_faces()
    {
        return drapeau_double_faces;
    }

    //======================================================================================
    //                  Gestion des textures
    //======================================================================================
    private:
    bool genere_uvs_texture_sections()
    {
        uint32_t i;
        uint32_t nbrSommets_dos=surfaces_annelide[ANNELIDE_SURFACE_DOS]->indices_sommets.size();
        uint32_t nbrSommets_ventre=surfaces_annelide[ANNELIDE_SURFACE_VENTRE]->indices_sommets.size();

        uvs_texture_sections_dos=(float*)malloc(sizeof(float)*(nbrSommets_dos+nbrSommets_ventre+(nbrCotes+1)*2)*2);
        if (uvs_texture_sections_dos==NULL)
        {
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        uvs_texture_sections_ventre=uvs_texture_sections_dos+2*nbrSommets_dos;
        uvs_texture_tete=uvs_texture_sections_ventre+2*nbrSommets_ventre;
        uvs_texture_queue=uvs_texture_tete+2*(nbrCotes+1);

        indices_uvs_dos=(uint32_t*)malloc(sizeof(uint32_t)*(nbrSommets_dos+nbrSommets_ventre+2*(nbrCotes+1)));
        if (indices_uvs_dos==NULL)
        {
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        indices_uvs_ventre=indices_uvs_dos+nbrSommets_dos;
        indices_uvs_tete=indices_uvs_ventre+nbrSommets_dos;
        indices_uvs_queue=indices_uvs_tete+nbrCotes+1;

        //------ Détermine le nombre de sommets recouverts par la texture du dos (donc du ventre aussi):
        nbr_cotes_dos=0;
        nbr_cotes_ventre=0;
        double x0,x1,ny;
        double ny_prec=1.;
        x0=sommets_fixes[1*3];
        for (i=0;i<nbrCotes;i++)
        {
            x1=sommets_fixes[(((i+1)%nbrCotes)+1)*3];
            ny=x1-x0;

            if (ny<=0)
            {
                nbr_cotes_dos++;
                if (ny_prec>0) premier_sommet_dos=i;
            }
            else if(ny_prec<=0)
            {
                premier_sommet_ventre=i;
            }

            ny_prec=ny;
            x0=x1;
        }
        nbr_cotes_ventre=nbrCotes-nbr_cotes_dos;


        cout<<"Nombre de sommets ventre: "<<nbr_cotes_ventre<<endl;
        cout<<"Nombre de sommets dos: "<<nbr_cotes_dos<<endl;
        cout<<"premier sommet dos: "<<premier_sommet_dos<<endl;
        cout<<"premier sommet ventre: "<<premier_sommet_ventre<<endl;


        //------ Génère les coordonnées UV du dos et du ventre:
        uint32_t i2; //Indice du sommet relativement à la section
        float u,v;
        uint32_t i_dos=0;
        uint32_t i_ventre=0;
        for (i=0;i<nbrSommets-2;i++)
        {
            u=float(i/nbrCotes);
            i2=i%nbrCotes;


            if (i2<premier_sommet_ventre)
            {
                i2=i2+nbrCotes-premier_sommet_dos;
                v=1./float(nbr_cotes_dos)*float(i2);
                indices_uvs_dos[i_dos]=i+1;
                uvs_texture_sections_dos[i_dos*2]=u;
                uvs_texture_sections_dos[i_dos*2+1]=v;
                i_dos++;
            }
            else if(i2==premier_sommet_ventre)
            {
                indices_uvs_dos[i_dos]=i+1;
                uvs_texture_sections_dos[i_dos*2]=u;
                uvs_texture_sections_dos[i_dos*2+1]=1.;
                i_dos++;

                indices_uvs_ventre[i_ventre]=i+1;
                uvs_texture_sections_ventre[i_ventre*2]=u;
                uvs_texture_sections_ventre[i_ventre*2+1]=0.;
                i_ventre++;
            }
            else if(i2<premier_sommet_dos)
            {
                i2-=premier_sommet_ventre;
                v=1./float(nbr_cotes_ventre)*float(i2);
                indices_uvs_ventre[i_ventre]=i+1;
                uvs_texture_sections_ventre[i_ventre*2]=u;
                uvs_texture_sections_ventre[i_ventre*2+1]=v;
                i_ventre++;
            }
            else if (i2==premier_sommet_dos)
            {
                indices_uvs_ventre[i_ventre]=i+1;
                uvs_texture_sections_ventre[i_ventre*2]=u;
                uvs_texture_sections_ventre[i_ventre*2+1]=1.;
                i_ventre++;

                indices_uvs_dos[i_dos]=i+1;
                uvs_texture_sections_dos[i_dos*2]=u;
                uvs_texture_sections_dos[i_dos*2+1]=0.;
                i_dos++;
            }
            else if (i2>premier_sommet_dos)
            {
                i2-=premier_sommet_dos;
                v=1./float(nbr_cotes_dos)*float(i2);
                indices_uvs_dos[i_dos]=i+1;
                uvs_texture_sections_dos[i_dos*2]=u;
                uvs_texture_sections_dos[i_dos*2+1]=v;
                i_dos++;
            }
            //Et voilà, i2 est recallé !
        }

        //--------- UVs de la tête et de la queue:
        indices_uvs_tete[0]=0;
        uvs_texture_tete[0]=0.5;
        uvs_texture_tete[1]=0.5;
        indices_uvs_queue[0]=nbrSommets-1;
        uvs_texture_queue[0]=0.5;
        uvs_texture_queue[1]=0.5;
        float a=M_PI/2.;
        float aq=a;
        for (i=1;i<nbrCotes+1;i++)
        {
            u=cosf(a)/2.+.5;
            v=sin(a)/2.+.5;
            indices_uvs_tete[i]=i;
            uvs_texture_tete[2*i]=u;
            uvs_texture_tete[2*i+1]=v;

            u=cosf(aq)/2.+.5;
            v=sin(aq)/2.+.5;

            indices_uvs_queue[i]=i+nbrCotes*(nbrSections-1);
            uvs_texture_queue[2*i]=u;
            uvs_texture_queue[2*i+1]=v;

            a+=(M_PI*2.)/float(nbrCotes);
            aq-=(M_PI*2.)/float(nbrCotes);

        }

        //------- Debogage:
        #if ANNELIDE_DEBUG_UVS
        cout<<" ------ Annelide - uvs du dos"<<endl;
        for(i=0;i<nbrSommets_dos;i++)
        {
            cout<<"sommet:"<<indices_uvs_dos[i]<<" U="<<uvs_texture_sections_dos[2*i]<<" V="<<uvs_texture_sections_dos[2*i+1]<<endl;
        }
        cout<<" ------ Annelide - uvs du ventre"<<endl;
        for(i=0;i<nbrSommets_ventre;i++)
        {
            cout<<"sommet:"<<indices_uvs_ventre[i]<<" U="<<uvs_texture_sections_ventre[2*i]<<" V="<<uvs_texture_sections_ventre[2*i+1]<<endl;
        }

        cout<<"-------- Annelide - uvs tête:"<<endl;

        for(i=0;i<nbrCotes+1;i++)
        {
            cout<<"sommet:"<<indices_uvs_tete[i]<<" U="<<uvs_texture_tete[2*i]<<" V="<<uvs_texture_tete[2*i+1]<<endl;
        }

        cout<<"-------- Annelide - uvs queue:"<<endl;

        for(i=0;i<nbrCotes+1;i++)
        {
            cout<<"sommet:"<<indices_uvs_queue[i]<<" U="<<uvs_texture_queue[2*i]<<" V="<<uvs_texture_queue[2*i+1]<<endl;
        }

        #endif

        return true;
    }

    public:
    //-----------------------------------------------------------------------
    // Ajoute une texture de couleur, qui se répète à chaque section
    /// Appeler initialise_textures() après avoir ajouter la ou les texture(s) à l'Annelide !!!
    //-----------------------------------------------------------------------
    bool ajoute_texture_couleur_sections(Image* image,uint16_t surface_id)
    {
        //---------- Algenere_uvs_texture_sectionslocation de la table des UVs:
        if (uvs_texture_sections_dos==NULL)
        {
            if (!genere_uvs_texture_sections())
            {
                cout<<"ERREUR dans Annelide::ajoute_texture_couleur_sections - Impossible de générer les UVs"<<endl;
               return false;
            }
        }

        float* uvs;
        uint32_t* indices;
        uint32_t nbr;

        ///Choisir la bonne surface!!!
        switch(surface_id)
        {
            case ANNELIDE_SURFACE_DOS:
                uvs=uvs_texture_sections_dos;
                indices=indices_uvs_dos;
            break;
            case ANNELIDE_SURFACE_VENTRE:
                uvs=uvs_texture_sections_ventre;
                indices=indices_uvs_ventre;
            break;
            case ANNELIDE_SURFACE_TETE:
                uvs=uvs_texture_tete;
                indices=indices_uvs_tete;
            break;
            case ANNELIDE_SURFACE_QUEUE:
                uvs=uvs_texture_queue;
                indices=indices_uvs_queue;
            break;
        }

        dksSurface* surface=surfaces_annelide[surface_id];
        nbr=surface->indices_sommets.size();
        dksTexture* texture_couleur=new dksTexture(NULL,image,DKSTEXTURE_CANAL_COULEUR,uvs,indices,nbr,false,GL_REPEAT,GL_REPEAT,GL_LINEAR,GL_LINEAR);
        surface->ajoute_texture(texture_couleur);

        return true;

        /*


        */
    }
    //-----------------------------------------------------------------------
    // Ajoute une texture de luminescence, qui se répète à chaque section
    // L'image doit être en niveaux de gris
    //-----------------------------------------------------------------------
    bool ajoute_texture_luminescence_sections(Image* image,uint8_t surface_id)
    {
        return true;
    }


    //=======================================================================================
    //                      Mouvements
    //=======================================================================================
    protected:
    virtual void applique_mouvements()
    {

        uint32_t i,j,k;

        //double tm0=glutGet(GLUT_ELAPSED_TIME);

        dksVolume::applique_mouvements();

        //ondulation_sinusoides(t,dt);
        ondulation_vecteur_generateur();

        //Rafraichi les normales des faces:
        if(sens_polygones==GL_CW) calcul_normales_faces_CW();
        else calcul_normales_faces_CCW();
        calcul_normales_sommets();

        //tm0-=glutGet(GLUT_ELAPSED_TIME);
        //cout<<-tm0<<endl;
    }


    void ondulation_vecteur_generateur()
    {
        uint32_t i,j,k;
        double pA[2];
        double pB[2];
        double ABx,ABy,ABz;
        double n;

        //répercution de l'ondulation sur la colonne:
        ///Pas très bon, si l'ordi rame, l'ondulation n'est plus du tout la même.
        /*
        for (i=nbrSections+1;i>0;i--)
        {
            colonne[i*3]=colonne[(i-1)*3];
            colonne[i*3+1]=colonne[(i-1)*3+1];
            //colonne[i*3+2]=colonne[(i-1)*3+2];
        }
        */


        //Position du vecteur générateur:
        vecteur_generateur_orientation[0]=(vecteur_generateur_x.valeur_aleatoire_temporelle(position_temps,intervalle_perturbation_x)*2.-1.)*amplitude_perturbation_x;
        vecteur_generateur_orientation[1]=(vecteur_generateur_y.valeur_aleatoire_temporelle(position_temps,intervalle_perturbation_y)*2-1.)*amplitude_perturbation_y;
        vecteur_generateur_orientation[2]=1.;//vecteur_generateur_z.valeur_aleatoire_temporelle(t,1.);

        vecteur_generateur_position[0]=sin(position_temps*vitesse_ondulatoire_x)*amplitude_ondulatoire_x+vecteur_generateur_orientation[0];
        vecteur_generateur_position[1]=sin(position_temps*vitesse_ondulatoire_y)*amplitude_ondulatoire_y+vecteur_generateur_orientation[1];
        vecteur_generateur_position[2]=longueur/2.;

        colonne[0]=vecteur_generateur_position[0];
        colonne[1]=vecteur_generateur_position[1];
        colonne[2]=vecteur_generateur_position[2];

        //Calcul les points de la colonne vertébrale:

        pA={colonne[0],colonne[1]};

        for (i=0;i<nbrSections+1;i++)
        {
            j=(i+1)*3;
            pB={colonne[j],colonne[j+1]};
            ABx=pB[0]-pA[0];
            ABy=pB[1]-pA[1];
            n=ABx*ABx+ABy*ABy;

            //if(n>1e-2)
            //{
                double dx=ABx/elasticite;
                double dy=ABy/elasticite;

                colonne[j]-=dx;
                colonne[j+1]-=dy;
                //colonne[j+2]=colonne_fixe[j+2];

                //colonne[j]=pA[0];
                //dcolonne[j+1]=pA[1];
            //}
            pA={colonne[j],colonne[j+1]};
        }



        for (i=0;i<nbrSections+2;i++)
        {
            //Tête:
            if (i==0)
            {
                //x+=colonne[0];
                //y+=colonne[1];
                a_ete_deplace();
                sommets[0]=sommets_fixes[0]+colonne[0];
                sommets[1]=sommets_fixes[1]+colonne[1];
                sommets[2]=sommets_fixes[2];//+colonne[2];
            }

            //Corps:
            else if (i==nbrSections+1)
            {
                sommets[(nbrSommets-1)*3]=sommets_fixes[(nbrSommets-1)*3]+colonne[(nbrSections+1)*3];
                sommets[(nbrSommets-1)*3+1]=sommets_fixes[(nbrSommets-1)*3+1]+colonne[(nbrSections+1)*3+1];
                sommets[(nbrSommets-1)*3+2]=sommets_fixes[(nbrSommets-1)*3+2];//+colonne[(nbrSections+1)*3+2];
            }

            //Queue:
            else
            {
                k=3*(1+(i-1)*nbrCotes); //Indice des sommets de la section
                for (j=0;j<nbrCotes;j++)
                {
                    sommets[k+3*j]=sommets_fixes[k+3*j]+colonne[3*i];
                    sommets[k+3*j+1]=sommets_fixes[k+3*j+1]+colonne[3*i+1];
                    sommets[k+3*j+2]=sommets_fixes[k+3*j+2];
                }
            }

        }
    }

    void ondulation_sinusoides(double t, double dt)
    {
        uint32_t i,j,k;
        double amplitude_ondulatoire_x=.7;
        double vitesse_ondulatoire_x=.5 * M_PI*2.;
        double frequence_ondulatoire_x=3 * M_PI*2.;
        double phase_x=0;

        double amplitude_ondulatoire_y=0.5;
        double vitesse_ondulatoire_y=.33 * M_PI*2.;
        double frequence_ondulatoire_y=2. * M_PI*2.;
        double phase_y=0;

        for (i=0;i<nbrSections+2;i++)
        {
            colonne[3*i]=colonne_fixe[3*i]+amplitude_ondulatoire_x*sin(t*vitesse_ondulatoire_x+phase_x);
            colonne[3*i+1]=colonne_fixe[3*i+1]+amplitude_ondulatoire_y*sin(t*vitesse_ondulatoire_y+phase_y);
            colonne[3*i+2]=colonne_fixe[3*i+2];
            phase_x-=frequence_ondulatoire_x/(nbrSections+1);
            phase_y-=frequence_ondulatoire_y/(nbrSections+1);

            //Tête:
            if (i==0)
            {
                sommets[0]=sommets_fixes[0]+colonne[0];
                sommets[1]=sommets_fixes[1]+colonne[1];
                sommets[2]=sommets_fixes[2];//+colonne[2];
            }

            //Corps:
            else if (i==nbrSections+1)
            {
                sommets[(nbrSommets-1)*3]=sommets_fixes[(nbrSommets-1)*3]+colonne[(nbrSections+1)*3];
                sommets[(nbrSommets-1)*3+1]=sommets_fixes[(nbrSommets-1)*3+1]+colonne[(nbrSections+1)*3+1];
                sommets[(nbrSommets-1)*3+2]=sommets_fixes[(nbrSommets-1)*3+2];//+colonne[(nbrSections+1)*3+2];
            }

            //Queue:
            else
            {
                k=3*(1+(i-1)*nbrCotes); //Indice des sommets de la section
                for (j=0;j<nbrCotes;j++)
                {
                    sommets[k+3*j]=sommets_fixes[k+3*j]+colonne[3*i];
                    sommets[k+3*j+1]=sommets_fixes[k+3*j+1]+colonne[3*i+1];
                    sommets[k+3*j+2]=sommets_fixes[k+3*j+2];
                }
            }

        }
    }

    //=======================================================================================
    //                      Affichages
    //=======================================================================================
    virtual void affiche(dksOeil* oeil)
    {
        //glPointSize(3);
        dksVolume::affiche(oeil);
        //affiche_sommets();
        //affiche_colonne();
        //affiche_vecteur_generateur();
    }

    void affiche_sommets()
    {
        glPointSize(3);
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        glColor3f(1.,0.,0.);
        glVertex3dv(&sommets[(0+1)*3]);
        glColor3f(1.,1.,0.);
        glVertex3dv(&sommets[(1+1)*3]);
        glEnd();
        glEnable(GL_LIGHTING);

    }

    void affiche_vecteur_generateur()
    {
        GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        glColor3f(1.,1.,0.5);
        glBegin(GL_POINTS);
        glVertex3dv(vecteur_generateur_position);
        glVertex3d(vecteur_generateur_position[0]+vecteur_generateur_orientation[0],
                   vecteur_generateur_position[1]+vecteur_generateur_orientation[1],
                   vecteur_generateur_position[2]+vecteur_generateur_orientation[2]
                   );
        glEnd();

        glColor3f(0.5,0.5,0.25);
        glBegin(GL_LINES);
        glVertex3dv(vecteur_generateur_position);
        glVertex3d(vecteur_generateur_position[0]+vecteur_generateur_orientation[0],
                   vecteur_generateur_position[1]+vecteur_generateur_orientation[1],
                   vecteur_generateur_position[2]+vecteur_generateur_orientation[2]
                   );
        glEnd();
        if (lum_mem)glEnable(GL_LIGHTING);
    }

    void affiche_colonne()
    {
        uint16_t i;
        GLboolean lum_mem=glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        glColor3f(1.,.5,1.);

        glBegin(GL_POINTS);
        for(i=0;i<nbrSections+2;i++)
        {
            glVertex3dv(&colonne[i*3]);
        }
        glEnd();

        glColor3f(0.5,0.25,0.5);
        glBegin(GL_LINE_STRIP);
        for(i=0;i<nbrSections+2;i++)
        {
            glVertex3dv(&colonne[i*3]);
        }
        glEnd();

        if (lum_mem)glEnable(GL_LIGHTING);
    }

};

#endif // ANNELIDE_H_INCLUDED
