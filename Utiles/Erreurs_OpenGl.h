#ifndef ERREURS_OPENGL_H_INCLUDED
#define ERREURS_OPENGL_H_INCLUDED

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

#endif // ERREURS_OPENGL_H_INCLUDED
