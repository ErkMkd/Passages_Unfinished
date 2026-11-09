#ifndef RAPPORT_ASSERTIONS_H_INCLUDED
#define RAPPORT_ASSERTIONS_H_INCLUDED

//*************************************************************************
//      En Debug, une assertion (ex: indice hors limites d'un vector), une erreur
//      du runtime (ex: corruption du tas) ou une exception non gérée (ex: accès
//      mémoire invalide) écrit son message et la pile d'appels dans la console,
//      puis arrête la démo, au lieu d'ouvrir une boîte de dialogue.
//*************************************************************************

#ifdef _DEBUG

#include <windows.h>
#include <crtdbg.h>
#include <dbghelp.h>
#include <stdio.h>

#pragma comment(lib,"dbghelp.lib")

class Rapport_Assertions
{
    public:
        static void installe()
        {
            _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL,&rapport);
            SetUnhandledExceptionFilter(&exception_non_geree);
        }

    private:
        static int __cdecl rapport(int type,char* message,int* retour)
        {
            if (type==_CRT_WARN) return FALSE;
            fprintf(stderr,"\n!!!! %s: %s\n",type==_CRT_ASSERT ? "ASSERTION" : "ERREUR",message);
            affiche_pile();
            *retour=0;
            ExitProcess(3);
            return TRUE;
        }

        static LONG WINAPI exception_non_geree(EXCEPTION_POINTERS* infos)
        {
            fprintf(stderr,"\n!!!! EXCEPTION 0x%08lX à l'adresse %p\n",
                    infos->ExceptionRecord->ExceptionCode,infos->ExceptionRecord->ExceptionAddress);
            affiche_pile();
            ExitProcess(4);
            return EXCEPTION_EXECUTE_HANDLER;
        }

        static void affiche_pile()
        {
            void* pile[64];
            USHORT nbr_appels=CaptureStackBackTrace(1,64,pile,NULL);
            HANDLE processus=GetCurrentProcess();
            SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
            SymInitialize(processus,NULL,TRUE);

            char tampon[sizeof(SYMBOL_INFO)+256];
            SYMBOL_INFO* symbole=(SYMBOL_INFO*)tampon;
            for (USHORT i=0;i<nbr_appels;i++)
            {
                symbole->SizeOfStruct=sizeof(SYMBOL_INFO);
                symbole->MaxNameLen=255;
                DWORD64 decalage=0;
                DWORD decalage_ligne=0;
                IMAGEHLP_LINE64 ligne={sizeof(IMAGEHLP_LINE64)};
                if (!SymFromAddr(processus,(DWORD64)pile[i],&decalage,symbole)) continue;
                if (SymGetLineFromAddr64(processus,(DWORD64)pile[i],&decalage_ligne,&ligne))
                    fprintf(stderr,"    %s   (%s:%lu)\n",symbole->Name,ligne.FileName,ligne.LineNumber);
                else fprintf(stderr,"    %s\n",symbole->Name);
            }
            fflush(stderr);
        }
};

#else

class Rapport_Assertions
{
    public:
        static void installe() {}
};

#endif

#endif // RAPPORT_ASSERTIONS_H_INCLUDED
