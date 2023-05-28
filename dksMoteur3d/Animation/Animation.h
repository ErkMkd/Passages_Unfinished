#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

//===========================================================================================
//
//                      La classe chargée de gérer les animations
//
//===========================================================================================

#include "Canal_Animation.h"
#include <vector>

    using namespace std;

class Animation
{
    private:
        vector<Canal_Animation*> canaux;
        bool drapeau_active;

        //===============================================
        //                  CONSTRUCTEUR
        //===============================================
    public:
        Animation(vector<Canal_Animation*>* p_canaux)
        {
            canaux=*(p_canaux);
            drapeau_active=true;
        }

        //================================================
        //      Fonctions de gestion de l'animation:
        //================================================

        void active()
        {
            drapeau_active=true;
        }

        void desactive()
        {
            drapeau_active=false;
        }

        bool est_activee()
        {
            return drapeau_active;
        }
};

#endif // ANIMATION_H_INCLUDED
