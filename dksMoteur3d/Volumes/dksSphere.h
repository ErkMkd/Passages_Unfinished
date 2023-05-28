#ifndef DKSSPHERE_H_INCLUDED
#define DKSSPHERE_H_INCLUDED

//#include "../Elements_Fondamentaux/dksVolume.h"

#define DKSSPHERE_DEBUG false

class dksSphere : public dksVolume
{
    public:
    double* sommets_sphere;
    dksSurface* surfaces_sphere[2];
    dksFace** faces_sphere;
    uint32_t* aretes_faces_sphere;

    uint32_t nbrLatitudes;
    uint32_t nbrLongitudes;
    float rayon;

    //======================================================
    //          Constructeur
    //======================================================
    dksSphere(  char* p_nom=NULL,
                double px = 0., double py = 0., double pz = 0.,
                uint32_t p_nbrLatitudes=12,uint32_t p_nbrLongitudes=12,
                float p_rayon=0.5,
                uint32_t p_couleur=0xffff0000,
                dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL
                ):dksVolume(p_nom,0,px,py,pz,axe1,axe2,axe3)
                {
                    nbrLatitudes=p_nbrLatitudes;
                    nbrLongitudes=p_nbrLongitudes;
                    rayon=p_rayon;


                    sens_polygones=GL_CW;

                    surfaces_sphere={NULL,NULL};
                    faces_sphere=NULL;
                    aretes_faces_sphere=NULL;
                    sommets_sphere=NULL;

                    if(!creation_sommets_sphere()) return;
                    if(!creation_faces_sphere()) return;
                    if(!creation_surfaces_sphere()) return;

                    determine_couleur(p_couleur);


                    #if DKSSPHERE_DEBUG
                    cout<<"*** dksSphere : "<<nom<<endl;
                    cout<<" - Nombre de sommets: "<<nbrSommets<<endl;
                    cout<<" - Nombre de faces: "<<nbrFaces<<endl;
                    #endif

                    mode_remplissage=DKSVOLUME_REMPLISSAGE_SURFACES;

                    //determine_mode_rendu(DKSVOLUME_MODE_LISTE);
                    determine_mode_rendu(DKSVOLUME_MODE_TABLEAUX);
                    maj_listes();

                }
    //================================
    //      Destructeur
    //================================
    ~dksSphere()
    {
        if(aretes_faces_sphere!=NULL) free(aretes_faces_sphere);
        //if(faces_sphere!=NULL) free(faces_sphere);
        if(surfaces_sphere[0]!=NULL) delete surfaces_sphere[0];
        if(surfaces_sphere[1]!=NULL) delete surfaces_sphere[1];
    }

    //======================================================================
    //          Gestion des sommets
    //======================================================================
    bool creation_sommets_sphere()
    {
        uint32_t p_nbrSommets=2+nbrLongitudes*nbrLatitudes;
        sommets_sphere=(double*)malloc(p_nbrSommets*3*sizeof(double));
        if(sommets_sphere==NULL)
        {
            cout<<"ERREUR dans dksSphere::creation_sommets_sphere() - Impossible d'allouer les sommets"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }

        //Calcul les sommets:
        sommets_sphere[0]=0.;
        sommets_sphere[1]=rayon;
        sommets_sphere[2]=0.;
        sommets_sphere[(p_nbrSommets-1)*3]=0.;
        sommets_sphere[(p_nbrSommets-1)*3+1]=-rayon;
        sommets_sphere[(p_nbrSommets-1)*3+2]=0.;
        uint32_t i,j,k;
        k=1;
        double px,py,pz;
        double ry,a;
        for(i=0;i<nbrLongitudes;i++)
        {
            py=rayon*cos(M_PI/double(nbrLongitudes+1)*double(i+1));

            ry=sqrt((rayon*rayon)-(py*py));
            for(j=0;j<nbrLatitudes;j++)
            {
                a=M_PI*2./double(nbrLatitudes)*double(j);
                px=ry*cos(a);
                pz=ry*sin(a);
                sommets_sphere[3*k]=px;
                sommets_sphere[3*k+1]=py;
                sommets_sphere[3*k+2]=pz;
                k++;
            }
        }

        if(!determine_sommets(sommets_sphere,p_nbrSommets))return false;
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SOMMETS;
        return true;
    }

    //======================================================================
    //          Gestion des faces
    //======================================================================
    bool creation_faces_sphere()
    {
        //Allocation des arètes:
        uint32_t nbrTriangles=nbrLatitudes*2;
        uint32_t nbrQuads=(nbrLongitudes-1)*nbrLatitudes;
        aretes_faces_sphere=(uint32_t*)malloc((nbrTriangles*3+nbrQuads*4)*sizeof(uint32_t));
        if (aretes_faces_sphere==NULL)
        {
            cout<<"ERREUR dans dksSphere::creation_faces_sphere() - Impossible de créer la table des arètes"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }
        //Allocation de la table des faces:
        uint32_t p_nbrFaces=nbrTriangles+nbrQuads;
        faces_sphere=(dksFace**)malloc(p_nbrFaces*sizeof(dksFace*));
        if (faces_sphere==NULL)
        {
            cout<<"ERREUR dans dksSphere::creation_faces_sphere() - Impossible de créer la table des faces"<<endl;
            erreur=DKSVOLUME_ERREUR_MEMOIRE;
            return false;
        }
        //Création des faces:
        uint32_t i;
        uint32_t indice_face=0;
        uint32_t indice_aretes=0;
        uint32_t indice_sommet;
        uint32_t indice_sommet_segment;
        char* nom_surface;
        char nom_surface_haut[]={"surface_haut"};   ///les faces alignées sur Y sont inclues dans la surface du haut
        char nom_surface_bas[]={"surface_bas"};

        //La calotte du haut:
        #if DKSSPHERE_DEBUG
        cout<<"***** Creation du chapeau haut ***********"<<endl;
        #endif
        nom_surface=nom_surface_haut;
        for(i=0;i<nbrLatitudes;i++)
        {
            indice_sommet=i;
            aretes_faces_sphere[indice_aretes]=0;
            aretes_faces_sphere[indice_aretes+1]=indice_sommet+1;
            aretes_faces_sphere[indice_aretes+2]=((indice_sommet+1)%nbrLatitudes)+1;
            #if DKSSPHERE_DEBUG
            cout<<aretes_faces_sphere[indice_aretes]<<" , "<<aretes_faces_sphere[indice_aretes+1]<<" , "<<aretes_faces_sphere[indice_aretes+2]<<endl;
            #endif

            faces_sphere[indice_face]=new dksFace(3,&aretes_faces_sphere[indice_aretes],nom_surface,DKSFACE_LIBERE_RIEN);
            if(faces_sphere[indice_face]==NULL || faces_sphere[indice_face]->erreur!=DKSFACE_OK)
            {
                cout<<"ERREUR dans dksSphere::creation_faces_sphere() - Impossible de créer les faces"<<endl;
                erreur=DKSVOLUME_ERREUR_FACES;
                return false;
            }
            indice_aretes+=3;
            indice_face++;
        }

        //les segments:
        #if DKSSPHERE_DEBUG
        cout<<"***** Creation des segments ***********"<<endl;
        #endif
        nom_surface=nom_surface_haut;
        for(i=0;i<nbrQuads;i++)
        {
            indice_sommet=i;
            indice_sommet_segment=i%nbrLatitudes;

            aretes_faces_sphere[indice_aretes]=indice_sommet+1;
            aretes_faces_sphere[indice_aretes+1]=indice_sommet+nbrLatitudes+1;
            aretes_faces_sphere[indice_aretes+2]=(i/nbrLatitudes+1)*nbrLatitudes+((indice_sommet_segment+1)%nbrLatitudes)+1;
            aretes_faces_sphere[indice_aretes+3]=(i/nbrLatitudes)*nbrLatitudes+((indice_sommet_segment+1)%nbrLatitudes)+1;

            #if DKSSPHERE_DEBUG
            cout<<aretes_faces_sphere[indice_aretes]<<" , "<<aretes_faces_sphere[indice_aretes+1]<<" , "<<aretes_faces_sphere[indice_aretes+2]<<" , "<<aretes_faces_sphere[indice_aretes+3]<<endl;
            #endif

            faces_sphere[indice_face]=new dksFace(4,&aretes_faces_sphere[indice_aretes],nom_surface,DKSFACE_LIBERE_RIEN);
            if(faces_sphere[indice_face]==NULL || faces_sphere[indice_face]->erreur!=DKSFACE_OK)
            {
                cout<<"ERREUR dans dksSphere::creation_faces_sphere() - Impossible de créer les faces"<<endl;
                erreur=DKSVOLUME_ERREUR_FACES;
                return false;
            }
            indice_aretes+=4;
            indice_face++;
        }

        //La calotte du bas:
        #if DKSSPHERE_DEBUG
        cout<<"***** Creation du chapeau bas ***********"<<endl;
        #endif
        nom_surface=nom_surface_haut;
        for(i=0;i<nbrLatitudes;i++)
        {
            indice_sommet=i;
            aretes_faces_sphere[indice_aretes]=nbrSommets-1;
            aretes_faces_sphere[indice_aretes+1]=((indice_sommet+1)%nbrLatitudes)+ nbrSommets-1-nbrLatitudes;
            aretes_faces_sphere[indice_aretes+2]=nbrSommets-1-nbrLatitudes+indice_sommet;

            #if DKSSPHERE_DEBUG
            cout<<aretes_faces_sphere[indice_aretes]<<" , "<<aretes_faces_sphere[indice_aretes+1]<<" , "<<aretes_faces_sphere[indice_aretes+2]<<endl;
            #endif

            faces_sphere[indice_face]=new dksFace(3,&aretes_faces_sphere[indice_aretes],nom_surface,DKSFACE_LIBERE_RIEN);
            if(faces_sphere[indice_face]==NULL || faces_sphere[indice_face]->erreur!=DKSFACE_OK)
            {
                cout<<"ERREUR dans dksSphere::creation_faces_sphere() - Impossible de créer les faces"<<endl;
                erreur=DKSVOLUME_ERREUR_FACES;
                return false;
            }
            indice_aretes+=3;
            indice_face++;
        }

        //Ajoute les faces au volume:
        if (!determine_faces(faces_sphere,p_nbrFaces)) return false;
        drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_FACES;
        return true;
    }

    //======================================================================
    //          Gestion des surfaces
    //======================================================================
    bool creation_surfaces_sphere()
    {
        surfaces_sphere[0]=new dksSurface("surface_haut");
        surfaces_sphere[1]=new dksSurface("surface_bas");

        dksSurface* surface=surfaces_sphere[0];
        surface->drapeau_adoucissement=true;
        surface->determine_couleur_diffusion(0xffff0000,1.);
        surface->specularite=1.;
        surface->brillance=30.;

        determine_surfaces(surfaces_sphere,2);
        //drapeaux_liberation_memoire|=DKSVOLUME_LIBERE_SURFACES; *** Les surfaces doivent être supprimée par la sphère. Sinon le volume détruit aussi le tableau des surfaces.
        return true;
    }

    void determine_couleur(uint32_t p_couleur)
    {
        surfaces_sphere[0]->determine_couleur_diffusion(p_couleur,1.);
    }

    //======================================================================
    //          Paramétrages
    //======================================================================

    //--------------------------
    //  Définition du rayon
    //--------------------------
    ///------------------ L'échelle géométrique est réinitialisée.
    void determine_rayon(float r)
    {
        uint32_t i;
        rayon=r;
        echelle_geometrique=1.;
        for(i=0;i<nbrSommets;i++)
        {
            double sx=sommets[3*i];
            double sy=sommets[3*i+1];
            double sz=sommets[3*i+2];
            double r0=sqrt(sx*sx+sy*sy+sz*sz);
            r0=rayon/r0;
            sommets[3*i]=sx*r0;
            sommets[3*i+1]=sy*r0;
            sommets[3*i+2]=sz*r0;
        }
        maj_faces();
        maj_listes();
    }

    //======================================================================
    //          Affichages
    //======================================================================
    virtual void affiche(dksOeil* oeil)
    {
        this->dksVolume::affiche(oeil);
        /*
        glPointSize(4.);
        bool mem_lights=glIsEnabled(GL_LIGHTING);
        bool mem_depth_test=glIsEnabled(GL_DEPTH_TEST);
        desactive_textures();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
            glColor3f(1.,0.,0.);
            glVertex3dv(&sommets[0]);
            glColor3f(0.,1.,0.);
            glVertex3dv(&sommets[3]);
            glColor3f(0.,0.,1.);
            glVertex3dv(&sommets[6]);
        glEnd();
        if (mem_lights)glEnable(GL_LIGHTING);
        if (mem_depth_test)glEnable(GL_DEPTH_TEST);
        */
    }

};

#endif // DKSSPHERE_H_INCLUDED
