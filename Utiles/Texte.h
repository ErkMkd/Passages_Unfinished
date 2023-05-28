#ifndef TEXTE_H_INCLUDED
#define TEXTE_H_INCLUDED

class Texte
{
    public:


        static void affiche_texte(float xpos,float ypos,char* texte)
        {
            GLboolean eclairage_mem=glIsEnabled(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(-1.,1.,-1.,1.);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            glRasterPos2f(xpos,ypos);
            for(uint32_t i=0 ;i<strlen(texte);++i)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,texte[i]);
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glEnable(GL_DEPTH_TEST);
            if(eclairage_mem)glEnable(GL_LIGHTING);
        }

        static void affiche_nombre(float xpos,float ypos, float nombre,uint16_t precision=3)
        {
            uint16_t i;
            char chaine[64];
            char chaine_precision[16];
            char format_chaine[16];
            sprintf(chaine_precision,"%d",precision);
            format_chaine[0]='%';
            format_chaine[1]='.';
            for(i=0;i<strlen(chaine_precision);i++) format_chaine[2+i]=chaine_precision[i];
            format_chaine[2+i++]='f';
            format_chaine[2+i++]=0;
            sprintf(chaine,format_chaine,nombre);
            affiche_texte(xpos,ypos,chaine);
        }
};
#endif // TEXTE_H_INCLUDED
