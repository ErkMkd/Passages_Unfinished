#ifndef HORLOGE_H_INCLUDED
#define HORLOGE_H_INCLUDED

#include <chrono>

//*************************************************************************
//          Temps de la démo
//      Le temps avance en temps réel, ou d'un pas fixe à chaque image
//      (images reproductibles pour le banc d'essai).
//      La pause, la vitesse et l'image par image servent au panneau de développement.
//*************************************************************************

class Horloge
{
    public:
        bool pause;
        double vitesse;     //1: temps réel
        double pas_fixe;    //>0: chaque image avance de ce pas (en s), quelle que soit sa durée réelle

    private:
        double temps;       //Temps de la démo (en s)
        double temps_reel_precedent;
        bool drapeau_demarree;
        bool drapeau_image_suivante;

    public:
        Horloge()
        {
            pause=false;
            vitesse=1.;
            pas_fixe=0.;
            temps=0.;
            temps_reel_precedent=0.;
            drapeau_demarree=false;
            drapeau_image_suivante=false;
        }

        //Temps réel écoulé depuis le lancement du programme (en s), pour mesurer des durées.
        static double temps_reel()
        {
            static const std::chrono::steady_clock::time_point depart=std::chrono::steady_clock::now();
            return std::chrono::duration<double>(std::chrono::steady_clock::now()-depart).count();
        }

        //Même chose en millisecondes (remplace glutGet(GLUT_ELAPSED_TIME)).
        static int temps_reel_ms()
        {
            return int(temps_reel()*1000.);
        }

        //En pause, fait avancer la démo d'une image.
        void image_suivante()
        {
            drapeau_image_suivante=true;
        }

        //Appelée une fois par image. Renvoie le temps de la démo: 0 à la première image.
        double avance()
        {
            double maintenant=temps_reel();
            double ecart=maintenant-temps_reel_precedent;
            temps_reel_precedent=maintenant;
            if (!drapeau_demarree)
            {
                drapeau_demarree=true;
                return temps;
            }
            if (pas_fixe>0.) ecart=pas_fixe;
            if (!pause) temps+=ecart*vitesse;
            else if (drapeau_image_suivante) temps+= pas_fixe>0. ? pas_fixe : 1./60.;
            drapeau_image_suivante=false;
            return temps;
        }

        double renvoie_temps()
        {
            return temps;
        }
};

#endif // HORLOGE_H_INCLUDED
