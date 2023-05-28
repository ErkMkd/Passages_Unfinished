#ifndef DKSFACE_H_INCLUDED
#define DKSFACE_H_INCLUDED


//-----------------------------------------------------------------------------------
//  Classe des faces:
/// ATTENTION: les tables des arêtes sont libérées par le destructeur des faces.
//-----------------------------------------------------------------------------------

#define DKSFACE_OK 0
#define DKSFACE_ERREUR_SURFACE 1 //Si aucune surface n'est définie pour la face.
#define DKSFACE_ERREUR_MEMOIRE 2

#define DKSFACE_LIBERE_RIEN 0x00
#define DKSFACE_LIBERE_ARETES 0x01

//Structure utilisée pour le tri des faces transparentes:
class dksFace;

class tri_face
{   public:
    double distance;        //Distance de la face à l'oeil
    dksFace* face;   //L'indice de la face
};

//===========================================================================================
//                                  LA CLASSE
//===========================================================================================

class dksFace
{
    public:
        //------ Données fournies au volume à sa création:
        uint16_t erreur;
        uint16_t nbrSommets;
        uint32_t* aretes;   //Les aretes doivent boucler (dernier sommets lié au premier)


        //------- Données initialisées automatiquement:
        uint32_t    indice; //L'indice de la face dans le volume
        dksVolume*  volume; //Le volume auquel appartient la face
        char nom_surface[128];  //Le nom de la surface à laquelle est liée la face. Ca sert dans le cas d'une modification des surfaces du volume, pour refaire les liens.
        dksSurface* surface;   //La surface à laquelle est affectée la face. Ce champ doit être rempli par l'utilisateur. Une erreur est générée si il est vide.

        GLdouble normale[3];
        GLdouble* normales_sommets; //La table des normales aux sommets de la face. Créée lors de l'initialisation des surfaces

        bool drapeau_partiellement_adoucie; //True si au moins une arête n'est pas adoucie (angle superieur à la limite de la surface)

        GLdouble tangente[3];
        GLdouble binormale[3];

        double centre_gravite_relatif[3]; //Le centre de gravité de la face
        double centre_gravite_absolu[3]; //Le centre de gravite absolu sert pour le tri des faces transparentes

        uint32_t* liens_aretes; //Indices des arêtes de la face dans la table des arêtes du volume.

        uint8_t drapeaux_liberation_memoire;

        //=================================================
        //              CONSTRUCTEUR
        //=================================================
        dksFace(uint16_t p_nbrSommets, uint32_t* p_aretes, char* p_nom_surface,uint8_t p_drapeaux_liberation_memoire=DKSFACE_LIBERE_ARETES)
        {

            nbrSommets=p_nbrSommets;
            aretes=p_aretes;
            erreur=DKSFACE_OK;

            volume=NULL;
            surface=NULL;
            liens_aretes=NULL;
            normales_sommets=NULL;
            drapeau_partiellement_adoucie=false;
            drapeaux_liberation_memoire=p_drapeaux_liberation_memoire;

            if(!allocation_normales_sommets())
            {
                cout<<"ERREUR dans dksFace::dksFace() - Mémoire insuffisante pour allouer la table des normales des sommets"<<endl;
                erreur=DKSFACE_ERREUR_MEMOIRE;

            }

            if (!copie_nom_surface(p_nom_surface)) erreur=DKSFACE_ERREUR_SURFACE;
        }




        //=================================================
        //              CONSTRUCTEUR de copie
        //  La face copiée n'est pas attribuée à un volume.
        //
        //=================================================
        public:
        dksFace(dksFace& face_originale)
        {
            nbrSommets=face_originale.nbrSommets;
            aretes=NULL;
            erreur=DKSFACE_OK;
            volume=NULL;
            surface=NULL;
            liens_aretes=NULL;
            normales_sommets=NULL;
            drapeau_partiellement_adoucie=false;

            if(!allocation_aretes() || !allocation_normales_sommets())
            {
               cout<<"ERREUR dans dksFace::dksFace(dksFace&) - Mémoire insuffisante pour allouer les tables"<<endl;
                erreur=DKSFACE_ERREUR_MEMOIRE;
            }

            //Copie les arêtes:
            for(uint32_t i=0;i<nbrSommets;i++) aretes[i]=face_originale.aretes[i];

            if (!copie_nom_surface(face_originale.nom_surface)) erreur=DKSFACE_ERREUR_SURFACE;
        }

        //=================================================
        //              DESTRUCTEUR
        //=================================================
        public:
        ~dksFace()
        {
            //La table des arêtes est libérée par le volume, au besoin.
            if (liens_aretes!=NULL) free(liens_aretes);
            if ( ((drapeaux_liberation_memoire&DKSFACE_LIBERE_ARETES)!=0 ) &&  aretes!=NULL ) free (aretes);
            if (normales_sommets!=NULL) free(normales_sommets);
        }

        private:

        //---------------------------------------------------
        //  Allocation du tableau des normales des sommets
        //---------------------------------------------------

        bool allocation_normales_sommets()
        {
            normales_sommets=(GLdouble*)malloc(nbrSommets*sizeof(GLdouble)*3);
            if (normales_sommets==NULL) return false;
            return true;
        }

        //-----------------------------------------
        //  Allocation du tableau des aretes
        //  Sert uniquement lors de la copie
        //-----------------------------------------
        bool allocation_aretes()
        {
            aretes=(uint32_t*)malloc(nbrSommets*sizeof(uint32_t));
            if (aretes==NULL) return false;
            drapeaux_liberation_memoire=DKSFACE_LIBERE_ARETES;
            return true;
        }
        //----------------------------------
        //      Copie le nom de la surface
        //----------------------------------
        bool copie_nom_surface(char* p_nom)
        {
            uint32_t i,taille;
            if (p_nom==NULL) return false;
            else
            {
                taille=strlen(p_nom);
                if (taille>DKS_TAILLE_NOMS-1)taille=DKS_TAILLE_NOMS-1;  //Il peut y avoir une erreur, mais il faut le faire!
                for(i=0;i<taille;i++) nom_surface[i]=p_nom[i];
                nom_surface[i]=0;
                return true;
            }
        }
        public:
        //---------------------------------------------------------------
        //      Calcul le centre de gravité absolu de la face.
        //      Sert notamment pour le tri des faces transparentes
        //---------------------------------------------------------------
        void calcul_centre_gravite_absolu(dksRepere* repere_absolu)
        {
            double xt=centre_gravite_relatif[0];
            double yt=centre_gravite_relatif[1];
            double zt=centre_gravite_relatif[2];
            centre_gravite_absolu[0] = repere_absolu->x + xt * repere_absolu->ax_x + yt * repere_absolu->ay_x + zt * repere_absolu->az_x;
            centre_gravite_absolu[1] = repere_absolu->y + xt * repere_absolu->ax_y + yt * repere_absolu->ay_y + zt * repere_absolu->az_y;
            centre_gravite_absolu[2] = repere_absolu->z + xt * repere_absolu->ax_z + yt * repere_absolu->ay_z + zt * repere_absolu->az_z;
        }

        void affiche_centre_gravite_relatif()
        {
            cout<<centre_gravite_relatif[0]<<" "<<centre_gravite_relatif[1]<<" "<<centre_gravite_relatif[2]<<endl;
        }

        void affiche_centre_gravite_absolu()
        {
            cout<<centre_gravite_absolu[0]<<" "<<centre_gravite_absolu[1]<<" "<<centre_gravite_absolu[2]<<endl;
        }

        //--------------------------------------------
        //      Inverse les normales de la face
        //--------------------------------------------
        void inverse_normales()
        {
            normale[0]=-normale[0];
            normale[1]=-normale[1];
            normale[2]=-normale[2];
            for (uint16_t i=0;i<nbrSommets;i++)
            {
                normales_sommets[3*i]=-normales_sommets[3*i];
                normales_sommets[3*i+1]=-normales_sommets[3*i+1];
                normales_sommets[3*i+2]=-normales_sommets[3*i+2];
            }
        }


};
#endif // DKSFACE_H_INCLUDED
