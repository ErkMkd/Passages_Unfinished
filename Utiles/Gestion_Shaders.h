#ifndef GESTION_SHADERS_H_INCLUDED
#define GESTION_SHADERS_H_INCLUDED

/*

        Fonctions de base pour la création des shaders.

*/

#define GESTION_SHADERS_DEBUG false

class Gestion_Shaders
{
    public:

        //-----------------------------------------
        //  Charge un fichier source d'un shader:
        //-----------------------------------------
        static char* charge_source_shader(char* nom_fichier)
        {
            #if GESTION_SHADERS_DEBUG
            cout <<"------- Chargement du fichier source: "<<nom_fichier<<endl;
            #endif
            ifstream fichier_flux;
            uint32_t taille_fichier;

            fichier_flux.open(nom_fichier, ios::in | ios::binary | ios::ate);
            if (!fichier_flux.is_open())
                {
                    std::cout<<"ERREUR - Impossible d'ouvrir le fichier "<<nom_fichier<<std::endl;
                    return NULL;
                }
            //---Taille du fichier:
            taille_fichier=fichier_flux.tellg();
            #if GESTION_SHADERS_DEBUG
            cout<<"Taille du fichier: "<<taille_fichier<<endl;
            #endif
            //--- Pointeur au début du fichier:
            fichier_flux.seekg(0,ios::beg);
            GLchar* texte =(GLchar*)malloc((taille_fichier+1)*sizeof(GLchar));
            if (texte==NULL)
            {
                cout<<"ERREUR - Mémoire insuffisante pour charger le fichier "<<nom_fichier<<endl;
                fichier_flux.close();
                return NULL;
            }
            fichier_flux.read(texte,taille_fichier);
            fichier_flux.close();
            texte[taille_fichier]=0;
            return texte;
        }

        //-----------------------------------------
        //  Compile un shader
        //-----------------------------------------
        static bool compile_shader(GLuint shader_id)
        {
            char message_erreur[1024];
            GLint taille_message;
            GLint status;
            message_erreur[1023]=0;

            glCompileShader(shader_id);
            glGetShaderiv(shader_id,GL_COMPILE_STATUS,&status);
            if (status==GL_FALSE)
            {
                glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&taille_message);
                glGetShaderInfoLog(shader_id,1023,&taille_message,message_erreur);
                cout<<"ERREUR de compilation du shader "<<shader_id<<":"<<endl;
                cout<<"Taille du message:"<<taille_message<<endl;
                cout<<message_erreur<<endl;
                return false;
            }
            #if GESTION_SHADERS_DEBUG
            cout<<"Shader "<<shader_id<<" compilé avec succès"<<endl;
            #endif
            return true;
        }

        //-----------------------------------------
        //  Liaisons d'un programme de shaders
        //-----------------------------------------
        static bool relie_programme_shader(GLuint programme_id)
        {
            char message_erreur[1024];
            GLint taille_message;
            GLint status;
            message_erreur[1023]=0;

            glLinkProgram(programme_id);
            glGetProgramiv(programme_id,GL_LINK_STATUS,&status);
            if (status==GL_FALSE)
            {
                glGetProgramiv(programme_id,GL_INFO_LOG_LENGTH,&taille_message);
                glGetProgramInfoLog(programme_id,1023,&taille_message,message_erreur);
                cout<<"ERREUR de liaison du programme de shaders "<<programme_id<<":"<<endl;
                cout<<message_erreur<<endl;
                return false;
            }

            #if GESTION_SHADERS_DEBUG
            cout<<"Programme de shaders "<<programme_id<<" lié avec succès"<<endl;
            #endif
            return true;
        }

        //-----------------------------------------
        //  Validation d'un programme de shaders
        //-----------------------------------------
        static bool valide_programme_shader(GLuint programme_id)
        {
            GLint status;
            glValidateProgram(programme_id);
            glGetProgramiv(programme_id,GL_VALIDATE_STATUS,&status);
            if (status==GL_FALSE)
            {
                cout<<"ERREUR le programme de shader "<<programme_id<<" n'est pas valide!"<<endl;
                return false;
            }
            #if GESTION_SHADERS_DEBUG
            cout<<"Le programme de shader "<<programme_id<<" est opérationnel"<<endl;
            #endif
            return true;
        }

        //----------------------------------------------
        //Renvoie true si une erreur s'est produite
        //----------------------------------------------
        protected:
            static bool erreur_openGl(char* message)
            {
                GLenum erreur_openGl=glGetError();
                if (erreur_openGl!=GL_NO_ERROR)
                {
                    cout<<message<<" : "<<gluErrorString(erreur_openGl)<<endl;
                    return true;
                }
                 return false;
            }

};

#endif // GESTION_SHADERS_H_INCLUDED
