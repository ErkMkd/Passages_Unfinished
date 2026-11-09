#ifndef DKSTABLEAUX_H_INCLUDED
#define DKSTABLEAUX_H_INCLUDED

//*************************************************************************
//          Émulation des tableaux de sommets en mémoire client
//          (glVertexPointer, glEnableClientState, glDrawElements...)
//
//  Le profil Core n'accepte que des buffers de sommets. Ces fonctions reprennent
//  l'interface des tableaux client: les pointeurs et les tableaux actifs sont
//  mémorisés, et chaque tracé relit les sommets désignés pour les transmettre à
//  dksImmediat, qui les trace depuis son buffer de sommets.
//*************************************************************************

#define DKSTABLEAUX_NBR_UNITES_TEXTURE 4
#define DKSTABLEAUX_NBR_ATTRIBUTS 16

class dksTableaux
{
    private:
        struct Tableau
        {
            bool actif;
            GLint taille;           //Nombre de composantes
            GLenum type;            //GL_FLOAT ou GL_DOUBLE
            GLsizei pas;            //En octets (0: composantes contiguës)
            const char* donnees;
        };

        inline static Tableau tableau_positions={};
        inline static Tableau tableau_normales={};
        inline static Tableau tableau_couleurs={};
        inline static Tableau tableaux_textures[DKSTABLEAUX_NBR_UNITES_TEXTURE]={};
        inline static Tableau tableaux_attributs[DKSTABLEAUX_NBR_ATTRIBUTS]={};
        inline static GLuint unite_client=0;

        static void definit(Tableau& t,GLint taille,GLenum type,GLsizei pas,const void* donnees)
        {
            t.taille=taille;
            t.type=type;
            t.pas=pas;
            t.donnees=(const char*)donnees;
        }

        //Composantes du sommet i (comme dans OpenGL, les composantes absentes valent 0, et 1 pour la quatrième):
        static void lit(const Tableau& t,GLuint i,GLdouble* valeurs)
        {
            valeurs[0]=0.; valeurs[1]=0.; valeurs[2]=0.; valeurs[3]=1.;
            size_t taille_composante= t.type==GL_DOUBLE ? sizeof(GLdouble) : sizeof(GLfloat);
            size_t pas= t.pas!=0 ? size_t(t.pas) : t.taille*taille_composante;
            const char* sommet=t.donnees+i*pas;
            for (GLint c=0;c<t.taille && c<4;c++)
                valeurs[c]= t.type==GL_DOUBLE ? ((const GLdouble*)sommet)[c] : GLdouble(((const GLfloat*)sommet)[c]);
        }

        static Tableau* tableau(GLenum nom)
        {
            switch (nom)
            {
                case GL_VERTEX_ARRAY: return &tableau_positions;
                case GL_NORMAL_ARRAY: return &tableau_normales;
                case GL_COLOR_ARRAY: return &tableau_couleurs;
                case GL_TEXTURE_COORD_ARRAY: return &tableaux_textures[unite_client];
                default: return NULL;
            }
        }

        //Transmet le sommet i des tableaux actifs à dksImmediat:
        static void transmet_sommet(GLuint i)
        {
            GLdouble v[4];
            if (tableau_normales.actif)
            {
                lit(tableau_normales,i,v);
                dksImmediat::normale3dv(v);
            }
            if (tableau_couleurs.actif)
            {
                lit(tableau_couleurs,i,v);
                dksImmediat::couleur4f(GLfloat(v[0]),GLfloat(v[1]),GLfloat(v[2]),GLfloat(v[3]));
            }
            for (GLuint u=0;u<DKSTABLEAUX_NBR_UNITES_TEXTURE;u++)
            {
                if (!tableaux_textures[u].actif) continue;
                lit(tableaux_textures[u],i,v);
                GLfloat st[2]={GLfloat(v[0]),GLfloat(v[1])};
                dksImmediat::multi_texture2fv(GL_TEXTURE0+u,st);
            }
            for (GLuint a=1;a<DKSTABLEAUX_NBR_ATTRIBUTS;a++)
            {
                if (!tableaux_attributs[a].actif) continue;
                lit(tableaux_attributs[a],i,v);
                dksImmediat::attribut3dv(a,v);
            }
            if (tableau_positions.actif)
            {
                lit(tableau_positions,i,v);
                dksImmediat::sommet3dv(v);
            }
        }

    public:
        //-------------------------------------------------------------
        //      Équivalents de glClientActiveTexture, glEnableClientState et glDisableClientState
        //-------------------------------------------------------------
        static void unite(GLenum unite_texture)
        {
            unite_client=std::min(GLuint(unite_texture-GL_TEXTURE0),GLuint(DKSTABLEAUX_NBR_UNITES_TEXTURE-1));
        }

        static void active(GLenum nom)
        {
            Tableau* t=tableau(nom);
            if (t!=NULL) t->actif=true;
        }

        static void desactive(GLenum nom)
        {
            Tableau* t=tableau(nom);
            if (t!=NULL) t->actif=false;
        }

        //-------------------------------------------------------------
        //      Équivalents de glVertexPointer, glNormalPointer, glColorPointer et glTexCoordPointer
        //-------------------------------------------------------------
        static void pointeur_sommets(GLint taille,GLenum type,GLsizei pas,const void* donnees)
        {
            definit(tableau_positions,taille,type,pas,donnees);
        }

        static void pointeur_normales(GLenum type,GLsizei pas,const void* donnees)
        {
            definit(tableau_normales,3,type,pas,donnees);
        }

        static void pointeur_couleurs(GLint taille,GLenum type,GLsizei pas,const void* donnees)
        {
            definit(tableau_couleurs,taille,type,pas,donnees);
        }

        static void pointeur_textures(GLint taille,GLenum type,GLsizei pas,const void* donnees)
        {
            definit(tableaux_textures[unite_client],taille,type,pas,donnees);
        }

        //-------------------------------------------------------------
        //      Équivalents de glVertexAttribPointer, glEnableVertexAttribArray et glDisableVertexAttribArray
        //-------------------------------------------------------------
        static void pointeur_attribut(GLuint indice,GLint taille,GLenum type,GLboolean normalise,GLsizei pas,const void* donnees)
        {
            if (indice<DKSTABLEAUX_NBR_ATTRIBUTS) definit(tableaux_attributs[indice],taille,type,pas,donnees);
        }

        static void active_attribut(GLuint indice)
        {
            if (indice<DKSTABLEAUX_NBR_ATTRIBUTS) tableaux_attributs[indice].actif=true;
        }

        static void desactive_attribut(GLuint indice)
        {
            if (indice<DKSTABLEAUX_NBR_ATTRIBUTS) tableaux_attributs[indice].actif=false;
        }

        //-------------------------------------------------------------
        //      Équivalents de glDrawElements et glDrawArrays
        //-------------------------------------------------------------
        static void trace_elements(GLenum mode,GLsizei nbr,GLenum type_indices,const void* indices)
        {
            dksImmediat::debut(mode);
            for (GLsizei k=0;k<nbr;k++)
            {
                GLuint i;
                switch (type_indices)
                {
                    case GL_UNSIGNED_BYTE: i=((const GLubyte*)indices)[k]; break;
                    case GL_UNSIGNED_SHORT: i=((const GLushort*)indices)[k]; break;
                    default: i=((const GLuint*)indices)[k]; break;
                }
                transmet_sommet(i);
            }
            dksImmediat::fin();
        }

        static void trace_tableaux(GLenum mode,GLint premier,GLsizei nbr)
        {
            dksImmediat::debut(mode);
            for (GLsizei k=0;k<nbr;k++) transmet_sommet(GLuint(premier+k));
            dksImmediat::fin();
        }
};

#endif // DKSTABLEAUX_H_INCLUDED
