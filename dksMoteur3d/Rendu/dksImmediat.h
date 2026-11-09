#ifndef DKSIMMEDIAT_H_INCLUDED
#define DKSIMMEDIAT_H_INCLUDED

#include <vector>
#include <stddef.h>
#include <algorithm>

//*************************************************************************
//          Émulation du mode immédiat d'OpenGL (glBegin, glVertex, glEnd...)
//
//  Le mode immédiat n'existe pas dans le profil Core. Ces fonctions en
//  reprennent l'interface: les sommets sont accumulés entre debut() et fin(),
//  puis tracés en une fois depuis un buffer de sommets.
//  Comme dans OpenGL, la normale, la couleur, les coordonnées de texture et
//  les attributs génériques "courants" sont associés à chaque sommet, et
//  restent valables d'un tracé à l'autre.
//
//  Pendant le portage (profil de compatibilité):
//      - les sommets alimentent les attributs de l'ancien pipeline (gl_Vertex,
//        gl_Normal...), lus par le pipeline fixe et par les shaders actuels;
//      - les primitives d'origine sont conservées (GL_POLYGON, GL_QUADS...);
//      - chaque valeur courante est aussi transmise à OpenGL, pour les tracés
//        qui ne passent pas encore par ces fonctions.
//  Tous les tracés du moteur passent par ces fonctions (et par dksTableaux): le VAO
//  de dksImmediat reste lié en permanence.
//*************************************************************************

#define DKSIMMEDIAT_NBR_UNITES_TEXTURE 4
#define DKSIMMEDIAT_NBR_ATTRIBUTS 8

//Le buffer est rempli à la suite, segment après segment, puis réutilisé depuis le début. Un segment n'est
//réécrit qu'une fois que le GPU a fini de tracer ses sommets (barrière posée quand on le quitte).
#define DKSIMMEDIAT_CAPACITE (1<<17)    //Nombre de sommets du buffer
#define DKSIMMEDIAT_NBR_SEGMENTS 4
#define DKSIMMEDIAT_TAILLE_SEGMENT (DKSIMMEDIAT_CAPACITE/DKSIMMEDIAT_NBR_SEGMENTS)

class dksImmediat
{
    private:
        struct Sommet
        {
            GLfloat position[4];
            GLfloat normale[3];
            GLfloat couleur[4];
            GLfloat textures[DKSIMMEDIAT_NBR_UNITES_TEXTURE][4];
            GLfloat attributs[DKSIMMEDIAT_NBR_ATTRIBUTS][4];
        };

        static void copie(GLfloat* destination,GLfloat a,GLfloat b,GLfloat c,GLfloat d)
        {
            destination[0]=a; destination[1]=b; destination[2]=c; destination[3]=d;
        }

        //Valeurs initiales d'OpenGL:
        static Sommet sommet_initial()
        {
            Sommet s;
            copie(s.position,0.f,0.f,0.f,1.f);
            s.normale[0]=0.f; s.normale[1]=0.f; s.normale[2]=1.f;
            copie(s.couleur,1.f,1.f,1.f,1.f);
            for (int i=0;i<DKSIMMEDIAT_NBR_UNITES_TEXTURE;i++) copie(s.textures[i],0.f,0.f,0.f,1.f);
            for (int i=0;i<DKSIMMEDIAT_NBR_ATTRIBUTS;i++) copie(s.attributs[i],0.f,0.f,0.f,1.f);
            return s;
        }

        inline static Sommet courant=sommet_initial();     //Valeurs courantes (normale, couleur...)
        inline static std::vector<Sommet> sommets;          //Sommets du tracé en cours
        inline static GLenum mode=GL_POINTS;
        inline static bool drapeau_trace=false;             //Entre debut() et fin()
        inline static bool drapeau_lot=false;               //Entre debut_lot() et fin_lot()
        inline static GLenum mode_lot=GL_POINTS;            //Primitive du lot en cours
        inline static size_t debut_primitive=0;             //Premier sommet de la primitive en cours, dans le lot
        inline static std::vector<GLint> premiers;          //Premier sommet de chaque primitive du lot
        inline static std::vector<GLsizei> nombres;         //Nombre de sommets de chaque primitive du lot
        inline static unsigned int attributs_utilises=0;    //Attributs génériques déjà utilisés (un bit par indice)
        inline static unsigned int attributs_actives=0;     //Attributs génériques activés dans le VAO
        inline static GLuint vao=0;
        inline static GLuint tampon=0;
        inline static Sommet* memoire=NULL;                 //Le buffer, projeté en mémoire en permanence
        inline static GLint premier_libre=0;                //Premier sommet libre du buffer
        inline static int segment_courant=0;                //Segment du buffer en cours de remplissage
        inline static GLsync barrieres[DKSIMMEDIAT_NBR_SEGMENTS]={};  //Fin de l'utilisation de chaque segment par le GPU

        //------- Création du buffer et du VAO, au premier tracé:
        static void cree_tampon()
        {
            GLbitfield acces=GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            GLsizeiptr taille=GLsizeiptr(DKSIMMEDIAT_CAPACITE)*sizeof(Sommet);
            glCreateBuffers(1,&tampon);
            glNamedBufferStorage(tampon,taille,NULL,acces);
            memoire=(Sommet*)glMapNamedBufferRange(tampon,0,taille,acces);
            glCreateVertexArrays(1,&vao);

            //Les attributs de l'ancien pipeline se règlent VAO et buffer liés:
            GLint unite_client;
            glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE,&unite_client);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER,tampon);
            GLsizei pas=sizeof(Sommet);
            glVertexPointer(4,GL_FLOAT,pas,(void*)offsetof(Sommet,position));
            glEnableClientState(GL_VERTEX_ARRAY);
            glNormalPointer(GL_FLOAT,pas,(void*)offsetof(Sommet,normale));
            glEnableClientState(GL_NORMAL_ARRAY);
            glColorPointer(4,GL_FLOAT,pas,(void*)offsetof(Sommet,couleur));
            glEnableClientState(GL_COLOR_ARRAY);
            for (int i=0;i<DKSIMMEDIAT_NBR_UNITES_TEXTURE;i++)
            {
                glClientActiveTexture(GL_TEXTURE0+i);
                glTexCoordPointer(4,GL_FLOAT,pas,(void*)(offsetof(Sommet,textures)+i*4*sizeof(GLfloat)));
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            }
            glClientActiveTexture(unite_client);
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }

        //------- Active dans le VAO les attributs génériques utilisés pour la première fois:
        static void active_attributs()
        {
            unsigned int nouveaux=attributs_utilises&~attributs_actives;
            if (nouveaux==0) return;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER,tampon);
            for (GLuint i=1;i<DKSIMMEDIAT_NBR_ATTRIBUTS;i++)
            {
                if ((nouveaux&(1u<<i))==0) continue;
                glVertexAttribPointer(i,4,GL_FLOAT,GL_FALSE,sizeof(Sommet),(void*)(offsetof(Sommet,attributs)+i*4*sizeof(GLfloat)));
                glEnableVertexAttribArray(i);
            }
            glBindBuffer(GL_ARRAY_BUFFER,0);
            attributs_actives|=nouveaux;
        }

        //------- Attend que le GPU ait fini de tracer les sommets d'un segment:
        static void attend_segment(int segment)
        {
            if (barrieres[segment]==0) return;
            while (glClientWaitSync(barrieres[segment],GL_SYNC_FLUSH_COMMANDS_BIT,1000000000)==GL_TIMEOUT_EXPIRED);
            glDeleteSync(barrieres[segment]);
            barrieres[segment]=0;
        }

        //------- Copie des sommets dans le buffer. Renvoie l'indice du premier, -1 en cas d'erreur.
        static GLint copie_sommets(const std::vector<Sommet>& a_copier)
        {
            if (vao==0) cree_tampon();
            GLint nbr=GLint(a_copier.size());
            if (nbr>DKSIMMEDIAT_TAILLE_SEGMENT)
            {
                cout<<"ERREUR dans dksImmediat::copie_sommets() - Trop de sommets: "<<nbr<<endl;
                return -1;
            }
            //Les sommets tiennent dans un segment: s'ils débordent, ils commencent au début du segment suivant.
            if (premier_libre+nbr>(segment_courant+1)*DKSIMMEDIAT_TAILLE_SEGMENT)
            {
                barrieres[segment_courant]=glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
                segment_courant=(segment_courant+1)%DKSIMMEDIAT_NBR_SEGMENTS;
                premier_libre=segment_courant*DKSIMMEDIAT_TAILLE_SEGMENT;
                attend_segment(segment_courant);
            }
            memcpy(memoire+premier_libre,a_copier.data(),size_t(nbr)*sizeof(Sommet));
            active_attributs();
            GLint premier=premier_libre;
            premier_libre+=nbr;
            return premier;
        }

        //------- Trace des sommets:
        static void trace(const std::vector<Sommet>& a_tracer,GLenum mode_trace)
        {
            GLint premier=copie_sommets(a_tracer);
            if (premier<0) return;
            glBindVertexArray(vao);
            bool emulation=dksEtat::prepare_trace(mode_trace);
            glDrawArrays(mode_trace,premier,GLsizei(a_tracer.size()));
            dksEtat::termine_trace(mode_trace,emulation);
        }

        //------- Trace les primitives du lot en un seul appel, chacune avec sa primitive d'origine:
        static void vide_lot()
        {
            if (!premiers.empty())
            {
                GLint base=copie_sommets(sommets);
                if (base>=0)
                {
                    for (GLint& premier:premiers) premier+=base;
                    glBindVertexArray(vao);
                    bool emulation=dksEtat::prepare_trace(mode_lot);
                    glMultiDrawArrays(mode_lot,premiers.data(),nombres.data(),GLsizei(premiers.size()));
                    dksEtat::termine_trace(mode_lot,emulation);
                }
            }
            sommets.clear();
            premiers.clear();
            nombres.clear();
        }

        static void sommet(GLfloat x,GLfloat y,GLfloat z)
        {
            if (!drapeau_trace) return;
            sommets.push_back(courant);
            copie(sommets.back().position,x,y,z,1.f);
        }

        static void normale(GLfloat x,GLfloat y,GLfloat z)
        {
            courant.normale[0]=x; courant.normale[1]=y; courant.normale[2]=z;
        }

        static void texture(GLenum unite,GLfloat s,GLfloat t)
        {
            unsigned int i=unite-GL_TEXTURE0;
            if (i<DKSIMMEDIAT_NBR_UNITES_TEXTURE) copie(courant.textures[i],s,t,0.f,1.f);
        }

        static void couleur(GLfloat r,GLfloat v,GLfloat b,GLfloat a)
        {
            copie(courant.couleur,r,v,b,a);
        }

        //Conversion d'OpenGL des entiers signés en couleur:
        static GLfloat entier(GLint c)
        {
            return std::max(GLfloat(double(c)/2147483647.),-1.f);
        }

    public:
        //-------------------------------------------------------------
        //      Équivalents de glBegin et glEnd
        //-------------------------------------------------------------
        static void debut(GLenum p_mode)
        {
            if (drapeau_lot)
            {
                //Un lot ne contient qu'une sorte de primitive, et tient dans un segment du buffer:
                if (p_mode!=mode_lot || sommets.size()>DKSIMMEDIAT_TAILLE_SEGMENT/2) vide_lot();
                mode_lot=p_mode;
                debut_primitive=sommets.size();
            }
            else sommets.clear();
            mode=p_mode;
            drapeau_trace=true;
        }

        static void fin()
        {
            if (!drapeau_trace) return;
            drapeau_trace=false;
            if (drapeau_lot)
            {
                GLsizei nbr=GLsizei(sommets.size()-debut_primitive);
                if (nbr>0)
                {
                    premiers.push_back(GLint(debut_primitive));
                    nombres.push_back(nbr);
                }
                return;
            }
            if (!sommets.empty()) trace(sommets,mode);
        }

        //-------------------------------------------------------------
        //      Regroupement des primitives
        //  Entre debut_lot() et fin_lot(), les primitives successives de même sorte sont
        //  tracées ensemble (glMultiDrawArrays), au lieu d'un tracé chacune.
        //  À n'utiliser que si aucun état d'OpenGL ne change entre ces primitives.
        //-------------------------------------------------------------
        static void debut_lot()
        {
            if (drapeau_lot) vide_lot();
            else sommets.clear();
            drapeau_lot=true;
        }

        static void fin_lot()
        {
            if (drapeau_lot) vide_lot();
            drapeau_lot=false;
        }

        //-------------------------------------------------------------
        //      Équivalents de glVertex
        //-------------------------------------------------------------
        static void sommet3dv(const GLdouble* v) { sommet(GLfloat(v[0]),GLfloat(v[1]),GLfloat(v[2])); }
        static void sommet3d(GLdouble x,GLdouble y,GLdouble z) { sommet(GLfloat(x),GLfloat(y),GLfloat(z)); }
        static void sommet3fv(const GLfloat* v) { sommet(v[0],v[1],v[2]); }
        static void sommet3f(GLfloat x,GLfloat y,GLfloat z) { sommet(x,y,z); }
        static void sommet2f(GLfloat x,GLfloat y) { sommet(x,y,0.f); }

        //-------------------------------------------------------------
        //      Équivalents de glNormal, glTexCoord, glMultiTexCoord, glColor et glVertexAttrib
        //  Les valeurs courantes sont aussi transmises à OpenGL.
        //-------------------------------------------------------------
        static void normale3dv(const GLdouble* v) { normale(GLfloat(v[0]),GLfloat(v[1]),GLfloat(v[2])); glNormal3dv(v); }
        static void normale3fv(const GLfloat* v) { normale(v[0],v[1],v[2]); glNormal3fv(v); }

        static void texture2f(GLfloat s,GLfloat t) { texture(GL_TEXTURE0,s,t); glTexCoord2f(s,t); }
        static void texture2fv(const GLfloat* v) { texture(GL_TEXTURE0,v[0],v[1]); glTexCoord2fv(v); }
        static void texture2d(GLdouble s,GLdouble t) { texture(GL_TEXTURE0,GLfloat(s),GLfloat(t)); glTexCoord2d(s,t); }
        static void multi_texture2f(GLenum unite,GLfloat s,GLfloat t) { texture(unite,s,t); glMultiTexCoord2f(unite,s,t); }
        static void multi_texture2fv(GLenum unite,const GLfloat* v) { texture(unite,v[0],v[1]); glMultiTexCoord2fv(unite,v); }

        static void couleur3f(GLfloat r,GLfloat v,GLfloat b) { couleur(r,v,b,1.f); glColor3f(r,v,b); }
        static void couleur4f(GLfloat r,GLfloat v,GLfloat b,GLfloat a) { couleur(r,v,b,a); glColor4f(r,v,b,a); }
        static void couleur3fv(const GLfloat* c) { couleur(c[0],c[1],c[2],1.f); glColor3fv(c); }
        static void couleur4fv(const GLfloat* c) { couleur(c[0],c[1],c[2],c[3]); glColor4fv(c); }
        static void couleur3ub(GLubyte r,GLubyte v,GLubyte b) { couleur(r/255.f,v/255.f,b/255.f,1.f); glColor3ub(r,v,b); }
        static void couleur4ub(GLubyte r,GLubyte v,GLubyte b,GLubyte a) { couleur(r/255.f,v/255.f,b/255.f,a/255.f); glColor4ub(r,v,b,a); }
        static void couleur3i(GLint r,GLint v,GLint b) { couleur(entier(r),entier(v),entier(b),1.f); glColor3i(r,v,b); }

        static void attribut3dv(GLuint indice,const GLdouble* v)
        {
            if (indice>0 && indice<DKSIMMEDIAT_NBR_ATTRIBUTS)
            {
                copie(courant.attributs[indice],GLfloat(v[0]),GLfloat(v[1]),GLfloat(v[2]),1.f);
                attributs_utilises|=1u<<indice;
            }
            glVertexAttrib3dv(indice,v);
        }
};

#endif // DKSIMMEDIAT_H_INCLUDED
