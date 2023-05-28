#ifndef DKSSHADER_H_INCLUDED
#define DKSSHADER_H_INCLUDED

/*
        Classe de base de définition des shaders
*/

#include "../../../Utiles/Gestion_Shaders.h"

#define DKSSHADER_DEBUG true

//------- Codes d'erreur:
#define DKSSHADER_OK 0
#define DKSSHADER_ERREUR_SOURCE 1
#define DKSSHADER_ERREUR_OBJETS 2
#define DKSSHADER_ERREUR_COMPILATION 3
#define DKSSHADER_ERREUR_PROGRAMME 4
#define DKSSHADER_ERREUR_LIAISON 5
#define DKSSHADER_ERREUR_VALIDATION 6
#define DKSSHADER_ERREUR_OPENGL 7
#define DKSSHADER_ERREUR_VARIABLES 8    //Sert pour les héritiers, lorsque les variables ne sont pas trouvées dans le programme.

class dksShader: public Gestion_Shaders
{
    public:
        GLuint programme_id;
        uint16_t erreur;
        char nom[256];

    protected:

        char *source_vertex;
        char *source_fragment;
        GLuint vertex_id;
        GLuint fragment_id;

        bool drapeau_vertex;
        bool drapeau_fragment;
        const GLchar* tableau_sources[1];

    //==============================================
    //  Constructeur
    //==============================================
    public:
        dksShader(char* p_nom,char* p_source_vertex,char* p_source_fragment)
        {
            erreur=DKSSHADER_OK;
            programme_id=0;
            source_vertex=NULL;
            source_fragment=NULL;
            vertex_id=0;
            fragment_id=0;

            memcpy(nom,p_nom,strlen(p_nom)+1);

            if(DKSSHADER_DEBUG) cout<<endl<<"$$$$$$$$$$$$$$$$$$$$ Création du Shader: "<<nom<<endl;

            if(p_source_vertex!=NULL) drapeau_vertex=true; else drapeau_vertex=false;
            if(p_source_fragment!=NULL) drapeau_fragment=true; else drapeau_fragment=false;

            if(drapeau_vertex) source_vertex=charge_source_shader(p_source_vertex);
            if(drapeau_fragment) source_fragment=charge_source_shader(p_source_fragment);

            if ((drapeau_vertex && source_vertex==NULL) || (drapeau_fragment && source_fragment==NULL))
            {
                erreur=DKSSHADER_ERREUR_SOURCE;
                return;
            }

            //---------- Création des shaders:
            if (DKSSHADER_DEBUG) cout<<"Création des objets shaders...";
            if(drapeau_vertex) vertex_id=glCreateShader(GL_VERTEX_SHADER);
            if(drapeau_fragment) fragment_id=glCreateShader(GL_FRAGMENT_SHADER);
            if ((drapeau_vertex && vertex_id==0) || (drapeau_fragment && fragment_id==0))
            {
                cout<<"ERREUR dans dksShader::dksShader() - Impossible de créer les objets Shader"<<endl;
                erreur=DKSSHADER_ERREUR_OBJETS;
                return;
            }
            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //---------- Lie les sources avec les objets shader:
            if (DKSSHADER_DEBUG) cout<<"Liaisons des sources aux shaders...";

            if(drapeau_vertex)
            {
                tableau_sources={source_vertex};
                glShaderSource(vertex_id,1,tableau_sources,NULL);
            }
            if(drapeau_fragment)
            {
                tableau_sources[0]=source_fragment;
                glShaderSource(fragment_id,1,tableau_sources,NULL);
            }

            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //---------- Compilation des shaders:
            if (DKSSHADER_DEBUG) cout<<"Compilation des shaders...";
            if(drapeau_vertex)
            {
                if(!compile_shader(vertex_id))
                {
                    erreur=DKSSHADER_ERREUR_COMPILATION;
                    return;
                }

            }

            if(drapeau_fragment)
            {
                if(!compile_shader(fragment_id))
                {
                    erreur=DKSSHADER_ERREUR_COMPILATION;
                    return;
                }

            }

            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //----------- Création du programme
            if (DKSSHADER_DEBUG) cout<<"Création du programme...";

            programme_id=glCreateProgram();
            if (programme_id==0)
            {
                cout<<"ERREUR dans dksShader::dksShader() - Impossible de créer le programme"<<endl;
                erreur=DKSSHADER_ERREUR_PROGRAMME;
                return;
            }

            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //----------- Liaison des shaders aux programmes:
            if(drapeau_vertex)glAttachShader(programme_id,vertex_id);
            if(drapeau_fragment)glAttachShader(programme_id,fragment_id);

            //----------- Liaisons des programmes de shaders:

            if (DKSSHADER_DEBUG) cout<<"Liaison du programme...";

            if (!relie_programme_shader(programme_id))
            {
                cout<<"ERREUR dans dksShaders::dksShaders() - Impossible de lier le programme"<<endl;
                erreur=DKSSHADER_ERREUR_LIAISON;
                return;
            }

            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //----------- Vérifie la validité des shaders:
            if (DKSSHADER_DEBUG) cout<<"Validation du programme ...";
            if (!valide_programme_shader(programme_id))
            {
                cout<<"ERREUR dans dksShader::dksShader() - Le programme est invalide sur cette machine!"<<endl;
                erreur=DKSSHADER_ERREUR_VALIDATION;
                return;
            }
            if (DKSSHADER_DEBUG) cout<<"OK"<<endl;

            //--------- Erreur OpenGl:
            if (erreur_openGl("ERREUR dans dksShader::dksShader() :")) erreur=DKSSHADER_ERREUR_OPENGL;

        }

        //=====================================
        //      Destructeur
        //=====================================
        ~dksShader()
        {
            if (programme_id!=0) glDeleteProgram(programme_id);
            if(fragment_id!=0) glDeleteShader(fragment_id);
            if(vertex_id!=0) glDeleteShader(vertex_id);
            if (source_vertex!=NULL) free(source_vertex);
            if (source_fragment!=NULL) free(source_fragment);
        }

};

#endif // DKSSHADER_H_INCLUDED
