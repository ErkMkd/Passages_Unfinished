#ifndef TABLEAUX_H_INCLUDED
#define TABLEAUX_H_INCLUDED

#include <cstddef>
#include <initializer_list>

//*************************************************************************
//          Affecte une liste de valeurs à un tableau C:
//              affecte(tableau,a,b,c);   remplace   tableau={a,b,c};
//              affecte(tableau_2d,{{a,b},{c,d}});   remplace   tableau_2d={{a,b},{c,d}};
//          (affectation acceptée par l'ancien GCC, mais pas par le C++ standard)
//          Les éléments non fournis sont mis à zéro, comme pour une initialisation.
//          Les valeurs sont copiées avant l'affectation, comme avec la liste d'origine
//          (ex: affecte(t,t[1],t[0]) échange bien les deux valeurs).
//*************************************************************************

template<typename T,size_t N,typename... V>
inline void affecte(T (&tableau)[N],V... valeurs)
{
    static_assert(sizeof...(V)<=N,"affecte() : trop de valeurs pour le tableau");
    size_t i=0;
    ((tableau[i++]=T(valeurs)),...);
    for(;i<N;i++) tableau[i]=T();
}

template<typename T> struct type_identique { typedef T type; };

template<typename T,size_t N,size_t M>
inline void affecte(T (&tableau)[N][M],std::initializer_list<std::initializer_list<typename type_identique<T>::type>> valeurs)
{
    size_t i=0;
    for(const auto& ligne:valeurs)
    {
        size_t j=0;
        for(const auto& valeur:ligne) tableau[i][j++]=valeur;
        for(;j<M;j++) tableau[i][j]=T();
        i++;
    }
    for(;i<N;i++) for(size_t j=0;j<M;j++) tableau[i][j]=T();
}

#endif // TABLEAUX_H_INCLUDED
