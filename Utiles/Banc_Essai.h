#ifndef BANC_ESSAI_H_INCLUDED
#define BANC_ESSAI_H_INCLUDED

#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

//*************************************************************************
//          Banc d'essai: images reproductibles de la démo
//
//  Passages.exe --scene tunnel --temps-fixe 60 --captures 5,10,20 --references references
//
//  --scene         scène de départ: logo, tunnel, caducee, starfield, arbre, greetings, credits, fin
//  --temps-fixe N  le temps avance de 1/N s à chaque image, quelle que soit la vitesse de la machine
//  --captures      temps (en s) des images enregistrées en PNG; la démo s'arrête après la dernière
//  --sortie        dossier des captures (par défaut: captures)
//  --references    compare chaque capture à l'image de même nom de ce dossier, et enregistre
//                  les différences (amplifiées) dans le dossier des captures
//
//  Code de sortie: 0 si toutes les captures sont conformes aux références, 1 sinon, 2 si
//  les paramètres sont invalides.
//*************************************************************************

#define BANC_ESSAI_NBR_SCENES 8

class Banc_Essai
{
    public:
        bool actif;                 //true si des captures sont demandées
        int scene_depart;           //Indice de la scène de départ, -1: scènes choisies dans main.cpp
        double images_par_seconde;  //0: temps réel
        bool drapeau_erreur;        //Paramètres invalides

        //Seuils de comparaison avec les références:
        int seuil_pixel;            //Écart (sur 255) au-delà duquel un pixel est différent
        double seuil_image;         //Pourcentage de pixels différents au-delà duquel une capture est différente

    private:
        vector<double> temps_captures;
        size_t prochaine_capture;
        string dossier_sortie;
        string dossier_references;
        int nbr_differentes;
        vector<unsigned char> pixels;

    public:
        static const char* nom_scene(int indice)
        {
            static const char* noms[BANC_ESSAI_NBR_SCENES]={"logo","tunnel","caducee","starfield","arbre","greetings","credits","fin"};
            return noms[indice];
        }

        Banc_Essai(int argc,char* argv[])
        {
            actif=false;
            scene_depart=-1;
            images_par_seconde=0.;
            drapeau_erreur=false;
            seuil_pixel=16;
            seuil_image=0.5;
            prochaine_capture=0;
            dossier_sortie="captures";
            nbr_differentes=0;

            for (int i=1;i<argc && !drapeau_erreur;i++)
            {
                string option(argv[i]);
                if (i+1>=argc) { erreur_parametres(option+" : valeur manquante"); break; }
                string valeur(argv[++i]);
                if (option=="--scene")
                {
                    for (int s=0;s<BANC_ESSAI_NBR_SCENES;s++) if (valeur==nom_scene(s)) scene_depart=s;
                    if (scene_depart<0) erreur_parametres("scène inconnue: "+valeur);
                }
                else if (option=="--temps-fixe")
                {
                    images_par_seconde=atof(valeur.c_str());
                    if (images_par_seconde<=0.) erreur_parametres("--temps-fixe attend un nombre d'images par seconde");
                }
                else if (option=="--captures")
                {
                    size_t debut=0;
                    while (debut<=valeur.size())
                    {
                        size_t fin=valeur.find(',',debut);
                        if (fin==string::npos) fin=valeur.size();
                        temps_captures.push_back(atof(valeur.substr(debut,fin-debut).c_str()));
                        debut=fin+1;
                    }
                    sort(temps_captures.begin(),temps_captures.end());
                    actif=true;
                }
                else if (option=="--sortie") dossier_sortie=valeur;
                else if (option=="--references") dossier_references=valeur;
                else erreur_parametres("option inconnue: "+option);
            }
            if (!drapeau_erreur && actif) std::filesystem::create_directories(dossier_sortie);
        }

        //-------------------------------------------------------------------------------
        //  Appelée après le rendu de chaque image, avant l'échange des tampons.
        //  Renvoie true quand la dernière capture est faite.
        //-------------------------------------------------------------------------------
        bool apres_rendu(double t,const char* nom_scene_courante,int largeur,int hauteur)
        {
            if (!actif || prochaine_capture>=temps_captures.size()) return false;
            //Tolérance d'une demi-image, pour les arrondis du temps:
            double tolerance= images_par_seconde>0. ? 0.5/images_par_seconde : 0.;
            if (t<temps_captures[prochaine_capture]-tolerance) return false;

            char nom[256];
            snprintf(nom,sizeof(nom),"%s_%07.2f.png",nom_scene_courante,temps_captures[prochaine_capture]);
            lit_image(largeur,hauteur);
            string chemin=dossier_sortie+"/"+nom;
            stbi_flip_vertically_on_write(1);   //OpenGL range les lignes de bas en haut
            stbi_write_png(chemin.c_str(),largeur,hauteur,3,pixels.data(),largeur*3);
            cout<<"Capture: "<<chemin<<endl;
            if (!dossier_references.empty()) compare(nom,largeur,hauteur);

            while (prochaine_capture<temps_captures.size() && temps_captures[prochaine_capture]<=t+tolerance) prochaine_capture++;
            return prochaine_capture>=temps_captures.size();
        }

        int code_sortie()
        {
            if (drapeau_erreur) return 2;
            if (!dossier_references.empty())
                cout<<"Banc d'essai: "<<nbr_differentes<<" capture(s) différente(s) des références"<<endl;
            return nbr_differentes>0 ? 1 : 0;
        }

    private:
        void erreur_parametres(const string& message)
        {
            cout<<"ERREUR dans les paramètres - "<<message<<endl;
            cout<<"Usage: Passages.exe [--scene logo|tunnel|caducee|starfield|arbre|greetings|credits|fin]"<<endl;
            cout<<"                    [--temps-fixe images_par_seconde] [--captures t1,t2,...]"<<endl;
            cout<<"                    [--sortie dossier] [--references dossier]"<<endl;
            drapeau_erreur=true;
        }

        //Lit l'image rendue dans le tampon arrière de la fenêtre:
        void lit_image(int largeur,int hauteur)
        {
            GLint fb_lecture,tampon_pixels,alignement,tampon_lecture;
            glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&fb_lecture);
            glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,&tampon_pixels);
            glGetIntegerv(GL_PACK_ALIGNMENT,&alignement);
            glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
            glGetIntegerv(GL_READ_BUFFER,&tampon_lecture);
            glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
            glReadBuffer(GL_BACK);
            glPixelStorei(GL_PACK_ALIGNMENT,1);

            pixels.resize(size_t(largeur)*hauteur*3);
            glReadPixels(0,0,largeur,hauteur,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());

            glReadBuffer(tampon_lecture);
            glPixelStorei(GL_PACK_ALIGNMENT,alignement);
            glBindBuffer(GL_PIXEL_PACK_BUFFER,tampon_pixels);
            glBindFramebuffer(GL_READ_FRAMEBUFFER,fb_lecture);
        }

        //Compare la dernière image lue à la référence de même nom:
        void compare(const char* nom,int largeur,int hauteur)
        {
            string chemin=dossier_references+"/"+nom;
            int l,h,n;
            unsigned char* reference=stbi_load(chemin.c_str(),&l,&h,&n,3);
            if (reference==NULL)
            {
                cout<<"    pas de référence: "<<chemin<<endl;
                nbr_differentes++;
                return;
            }
            if (l!=largeur || h!=hauteur)
            {
                cout<<"    DIFFÉRENTE: la référence mesure "<<l<<"x"<<h<<endl;
                stbi_image_free(reference);
                nbr_differentes++;
                return;
            }

            //Les lignes de la référence vont de haut en bas, celles d'OpenGL de bas en haut.
            vector<unsigned char> difference(pixels.size());
            double somme=0.;
            size_t nbr_pixels_differents=0;
            for (int y=0;y<hauteur;y++)
            {
                const unsigned char* ligne_image=&pixels[size_t(y)*largeur*3];
                const unsigned char* ligne_reference=&reference[size_t(hauteur-1-y)*largeur*3];
                unsigned char* ligne_difference=&difference[size_t(y)*largeur*3];
                for (int x=0;x<largeur;x++)
                {
                    int ecart_max=0;
                    for (int c=0;c<3;c++)
                    {
                        int ecart=abs(int(ligne_image[3*x+c])-int(ligne_reference[3*x+c]));
                        somme+=ecart;
                        ecart_max=max(ecart_max,ecart);
                        ligne_difference[3*x+c]=(unsigned char)min(255,ecart*4);
                    }
                    if (ecart_max>seuil_pixel) nbr_pixels_differents++;
                }
            }
            stbi_image_free(reference);

            double moyenne=somme/(double(largeur)*hauteur*3);
            double pourcentage=100.*double(nbr_pixels_differents)/(double(largeur)*hauteur);
            bool differente=pourcentage>seuil_image;
            if (differente) nbr_differentes++;
            printf("    %s: écart moyen %.2f/255, pixels différents %.2f %%\n",differente ? "DIFFÉRENTE" : "conforme",moyenne,pourcentage);
            fflush(stdout);
            if (somme>0.)
            {
                string chemin_difference=dossier_sortie+"/difference_"+nom;
                stbi_write_png(chemin_difference.c_str(),largeur,hauteur,3,difference.data(),largeur*3);
            }
        }
};

#endif // BANC_ESSAI_H_INCLUDED
