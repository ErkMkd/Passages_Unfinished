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
            GLint taille_message;
            GLint status;

            glCompileShader(shader_id);
            glGetShaderiv(shader_id,GL_COMPILE_STATUS,&status);
            if (status==GL_FALSE)
            {
                glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&taille_message);
                vector<char> message_erreur(taille_message+1,0);
                glGetShaderInfoLog(shader_id,taille_message,NULL,message_erreur.data());
                cout<<"ERREUR de compilation du shader "<<shader_id<<":"<<endl;
                cout<<"Taille du message:"<<taille_message<<endl;
                cout<<message_erreur.data()<<endl;
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
            GLint taille_message;
            GLint status;

            glLinkProgram(programme_id);
            glGetProgramiv(programme_id,GL_LINK_STATUS,&status);
            if (status==GL_FALSE)
            {
                glGetProgramiv(programme_id,GL_INFO_LOG_LENGTH,&taille_message);
                vector<char> message_erreur(taille_message+1,0);
                glGetProgramInfoLog(programme_id,taille_message,NULL,message_erreur.data());
                cout<<"ERREUR de liaison du programme de shaders "<<programme_id<<":"<<endl;
                cout<<message_erreur.data()<<endl;
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

            //Après la liaison, les samplers sont sur l'unité de texture 0 (sauf ceux qui en fixent une avec
            //layout(binding)). Deux samplers de types différents (ex: sampler2D et samplerCube) sur la même
            //unité rendent le programme invalide (règle appliquée par Intel et AMD). Le temps de la validation,
            //chaque sampler reçoit sa propre unité, puis retrouve la sienne.
            vector<GLint> samplers=renvoie_samplers(programme_id);
            vector<GLint> unites(samplers.size(),0);
            for (size_t i=0;i<samplers.size();i++) glGetUniformiv(programme_id,samplers[i],&unites[i]);
            GLint programme_courant;
            glGetIntegerv(GL_CURRENT_PROGRAM,&programme_courant);
            dksEtat::utilise_programme(programme_id);
            for (GLint i=0;i<GLint(samplers.size());i++) glUniform1i(samplers[i],i);

            glValidateProgram(programme_id);
            glGetProgramiv(programme_id,GL_VALIDATE_STATUS,&status);

            for (size_t i=0;i<samplers.size();i++) glUniform1i(samplers[i],unites[i]);
            dksEtat::utilise_programme(programme_courant);

            if (status==GL_FALSE)
            {
                GLint taille_message;
                glGetProgramiv(programme_id,GL_INFO_LOG_LENGTH,&taille_message);
                vector<char> message(taille_message+1,0);
                if (taille_message>0) glGetProgramInfoLog(programme_id,taille_message,NULL,message.data());
                cout<<"ERREUR le programme de shader "<<programme_id<<" n'est pas valide!"<<endl;
                cout<<message.data()<<endl;
                return false;
            }
            #if GESTION_SHADERS_DEBUG
            cout<<"Le programme de shader "<<programme_id<<" est opérationnel"<<endl;
            #endif
            return true;
        }

        //-----------------------------------------------------------
        //  Renvoie les emplacements des samplers actifs d'un programme
        //  (un emplacement par unité de texture, tableaux compris)
        //-----------------------------------------------------------
        static vector<GLint> renvoie_samplers(GLuint programme_id)
        {
            vector<GLint> samplers;
            GLint nbr_uniformes=0;
            glGetProgramiv(programme_id,GL_ACTIVE_UNIFORMS,&nbr_uniformes);
            for (GLint i=0;i<nbr_uniformes;i++)
            {
                char nom[256];
                GLint taille;
                GLenum type;
                glGetActiveUniform(programme_id,i,sizeof(nom),NULL,&taille,&type,nom);
                switch (type)
                {
                    case GL_SAMPLER_1D: case GL_SAMPLER_2D: case GL_SAMPLER_3D: case GL_SAMPLER_CUBE:
                    case GL_SAMPLER_1D_SHADOW: case GL_SAMPLER_2D_SHADOW: case GL_SAMPLER_2D_RECT:
                    case GL_SAMPLER_2D_RECT_SHADOW: case GL_SAMPLER_1D_ARRAY: case GL_SAMPLER_2D_ARRAY:
                    {
                        string base(nom);
                        if (taille>1 && base.size()>3 && base.compare(base.size()-3,3,"[0]")==0) base.resize(base.size()-3);
                        for (GLint j=0;j<taille;j++)
                        {
                            string element= taille>1 ? base+"["+to_string(j)+"]" : base;
                            samplers.push_back(glGetUniformLocation(programme_id,element.c_str()));
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            return samplers;
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
