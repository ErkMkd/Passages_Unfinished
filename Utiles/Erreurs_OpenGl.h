#ifndef ERREURS_OPENGL_H_INCLUDED
#define ERREURS_OPENGL_H_INCLUDED

#include <iostream>
#include <map>

//*************************************************************************
//      Nom d'une erreur d'OpenGL, pour les messages
//  Remplace gluErrorString: GLU ne fait pas partie d'OpenGL moderne.
//*************************************************************************

inline const char* texte_erreur_openGl(GLenum erreur)
{
    switch (erreur)
    {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        default: return "erreur inconnue";
    }
}

//*************************************************************************
//      Messages de débogage d'OpenGL (KHR_debug)
//  Avec un contexte de débogage, le pilote signale chaque erreur au moment
//  où elle se produit, avec sa description. Les erreurs sont écrites dans
//  la console, préfixées par "ERREUR OpenGL" (relevées par le banc d'essai);
//  les avertissements (performances, comportements à éviter) aussi.
//  Un même message n'est écrit que ses premières fois.
//*************************************************************************

inline void GLAPIENTRY message_debogage_openGl(GLenum source,GLenum type,GLuint id,GLenum gravite,
                                               GLsizei longueur,const GLchar* message,const void* utilisateur)
{
    static std::map<GLuint,int> nbr_messages;
    if (++nbr_messages[id]>3) return;
    bool erreur= type==GL_DEBUG_TYPE_ERROR || type==GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
    std::cout<<(erreur ? "ERREUR OpenGL: " : "Avertissement OpenGL: ")<<message<<std::endl;
}

//Active les messages, si le contexte a été créé pour le débogage:
inline void active_debogage_openGl()
{
    GLint drapeaux=0;
    glGetIntegerv(GL_CONTEXT_FLAGS,&drapeaux);
    if ((drapeaux&GL_CONTEXT_FLAG_DEBUG_BIT)==0) return;
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);     //Message émis pendant l'appel fautif (pile d'appels utilisable)
    glDebugMessageCallback(message_debogage_openGl,NULL);
    glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DEBUG_SEVERITY_NOTIFICATION,0,NULL,GL_FALSE);
    std::cout<<"Messages de débogage d'OpenGL actifs"<<std::endl;
}

#endif // ERREURS_OPENGL_H_INCLUDED
