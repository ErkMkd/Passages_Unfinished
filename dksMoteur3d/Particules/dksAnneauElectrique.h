#ifndef DKSANNEAUELECTRIQUE_H_INCLUDED
#define DKSANNEAUELECTRIQUE_H_INCLUDED

/*

            Génère un anneau électrique
            des points sont générés au hazard dans un tore, puis reliés entre eux par des lignes.

*/

#include "dksParticules.h"

//============================================================================
//              Classe
//============================================================================


class dksAnneauElectrique : public dksParticules
{
    private:
        float angle_central;
    public:
        float rayon_section;
        float facteur_rayon_section;
        float facteur_couleur;
        float rayon_central;
        float vitesse_eclair;
        float marge_vitesse_eclair;
        float luminescence;
        uint32_t couleur_min;


    //==================================================
    //          Constructeur
    //==================================================
    dksAnneauElectrique(char* p_nom="Anneau_Electrique",
                    double px=0.,double py=0.,double pz=0.,
                    uint32_t p_nbrPoints=0,
                    float p_rayon_section=1.,
                    float p_rayon_central=5.,   //Rayon du cercle central à l'épaisseur de la section
                    dksVecteurUnitaire* axe1 = NULL, dksVecteurUnitaire* axe2 = NULL, dksVecteurUnitaire* axe3 = NULL)
                    :dksParticules(p_nom,px,py,pz,p_nbrPoints,axe1,axe2,axe3)
    {
        rayon_central=p_rayon_central;
        rayon_section=p_rayon_section;
        vitesse_eclair=.1;
        marge_vitesse_eclair=0.095;
        genere_eclairs();
        epaisseur_points=1.;
        luminescence=1.;
        facteur_rayon_section=1.;
        facteur_couleur=1.;
        couleur_min=0xff808080;
        angle_central=0.;
    }

    //-------- Destructeur:
    ~dksAnneauElectrique()
    {

    }

    //===============================================
    //      Génère les éclairs
    //===============================================
    void genere_eclairs()
    {
        uint32_t i;
        float px,py,pz;
        float r;
        float rp;
        float angle_section;

        for (i=0;i<nbrPoints;i++)
        {
            r=float(rand()/32768.)*rayon_section*facteur_rayon_section;
            angle_section=float(rand())/32768.*M_PI*2.;
            px=r*cosf(angle_section);
            pz=r*sinf(angle_section);

            rp=px+rayon_central;
            angle_central+=vitesse_eclair+(marge_vitesse_eclair*(float(rand())/32768.*2.-1.));
            px=rp*cosf(angle_central);
            py=rp*sinf(angle_central);

            points[3*i]=px;
            points[3*i+1]=py;
            points[3*i+2]=pz;
        }
    }

    //=========================================
    //          Affichage
    //=========================================

    virtual void affiche(dksOeil* oeil)
        {
            if(!drapeau_affiche) return;
            uint32_t i;
            glLineWidth(epaisseur_points+(1.*float(rand()/32768.)*2.-1.));
            float rm=float(((couleur_min>>16)&0xff))/255.;
            float vm=float(((couleur_min>>8)&0xff))/255.;
            float bm=float((couleur_min&0xff))/255.;
            float r=facteur_couleur*float(((couleur_points>>16)&0xff))/255.;
            float v=facteur_couleur*float(((couleur_points>>8)&0xff))/255.;
            float b=facteur_couleur*float((couleur_points&0xff))/255.;
            if(r>1.)r=1.;
            if(v>1.)v=1.;
            if(b>1.)b=1.;
            if(r<rm)r=rm;
            if(v<vm)v=vm;
            if(b<bm)b=bm;

            GLboolean light_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);
            glUseProgram(0);
            desactive_textures();
            glColor3f(r,v,b);
            glBegin(GL_LINE_STRIP);
                //glLineWidth(epaisseur_points+(2.*float(rand()/32768.)*2.-1.));
                for (i=0;i<nbrPoints;i++)
                {
                    glVertex3dv(&points[3*i]);
                    if((i&0x0f)==0x0f)
                    {
                        glEnd();
                        glLineWidth(epaisseur_points+(2.*float(rand()/32768.)*2.-1.));
                        glBegin(GL_LINE_STRIP);
                    }
                }
            glEnd();
            if(light_mem)glEnable(GL_LIGHTING);
        }

        virtual void affiche_couleurs_luminescentes(dksOeil* oeil)
        {
            if(!drapeau_affiche) return;
            uint32_t i;
            glLineWidth(epaisseur_points+(1.*float(rand()/32768.)*2.-1.));
            float rm=float(((couleur_min>>16)&0xff))/255.;
            float vm=float(((couleur_min>>8)&0xff))/255.;
            float bm=float((couleur_min&0xff))/255.;
            float r=facteur_couleur*float(((couleur_points>>16)&0xff))/255.;
            float v=facteur_couleur*float(((couleur_points>>8)&0xff))/255.;
            float b=facteur_couleur*float((couleur_points&0xff))/255.;
            if(r>1.)r=1.;
            if(v>1.)v=1.;
            if(b>1.)b=1.;
            if(r<rm)r=rm;
            if(v<vm)v=vm;
            if(b<bm)b=bm;
            //GLboolean light_mem=glIsEnabled(GL_LIGHTING);
            //glDisable(GL_BLEND);
            glColor3f(r,v,b);
            glBegin(GL_LINE_STRIP);
                //glLineWidth(epaisseur_points+(2.*float(rand()/32768.)*2.-1.));
                for (i=0;i<nbrPoints;i++)
                {
                    glVertex3dv(&points[3*i]);
                    if((i&0x0f)==0x0f)
                    {
                        glEnd();
                        glLineWidth(epaisseur_points+(2.*float(rand()/32768.)*2.-1.));
                        glBegin(GL_LINE_STRIP);
                    }
                }
            glEnd();
            //if(light_mem)glEnable(GL_LIGHTING);
        }
};

#endif // DKSANNEAUELECTRIQUE_H_INCLUDED
