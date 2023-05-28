#ifndef MUSIQUE_H_INCLUDED
#define MUSIQUE_H_INCLUDED

#include "bass/bass.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MUSIQUE_DEBUG TRUE

#define MUSIQUE_OK BASS_OK

//#include "main.h"

//*************************************************************
// Chargement et contrôls d'une musique
//*************************************************************


class Musique
{
    public:

    int erreur;
    HSTREAM musique_id;
    float tempon_FFT[2048];
    static bool drapeau_son_initialise;

    //================ Constructeur:

    Musique (const char *fichier_musique)
    {
        if (!initialise_device()) return;

        musique_id = BASS_StreamCreateFile(
                                            false,  //true si la musique est lue depuis la mémoire
                                            fichier_musique,    //Le fichier
                                            0,                //Départ de la musique
                                            0,                //Longueur de la musique
                                            BASS_SAMPLE_LOOP //Libération de la mémoire à la fin de la musique
                                            );

        erreur=BASS_ErrorGetCode();

        //BASS_INFO Infos;
        //BASS_GetInfo(&Infos);
        //BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_OFF);
        //BASS_SetConfig(BASS_CONFIG_BUFFER, Infos.minbuf * 16);
        //BASS_Stop();

        if (MUSIQUE_DEBUG) cout<<"Initialisation musique:"<<erreur<<endl;
    }

    //====================== Destructeur:
    ~Musique()
    {
        if (erreur==BASS_OK)
        {

            BASS_ChannelStop(musique_id);
            BASS_StreamFree(musique_id);
            BASS_Stop();
            cout<<"DESTRUCTION MUSIQUE"<<endl;
        }
    }
    //====================================================
    //          Initialisations:
    //====================================================
    private:
    static bool initialise_device()
    {
        if(Musique::drapeau_son_initialise) return true;

        BASS_Init(
                    -1,     //L'audio-device utilisée. -1 = device par défaut du sytème
                    44100,  //Fréquence d'échantillonnage
                    BASS_DEVICE_LATENCY, //Active la latence du device (durée entre la demande d'un son et le moment où il va commencer à être joué.)
                    0, //Fenêtre actuellement au premier plan. Il vaut mieux avoir ouvert l'écran avant de créer un objet Musique.
                    NULL //Classe de l'objet à créer ?
                );

            int e=BASS_ErrorGetCode();
            if(e!=BASS_OK)
            {
                cout<<"ERREUR lors de l'initialisation de la librairie BASS !!! Code: "<<e<<endl;
                return false;
            }
            Musique::drapeau_son_initialise=true;
            return true;
    }

    //====================================================
    //===================== Fonctions
    //====================================================
    public:
    void joue(bool repete=false)
    {
        if (erreur==BASS_OK)
        {
            //BASS_Start();
            BASS_ChannelPlay(musique_id,repete); //Le booléen sert à préciser si l'on rejoue depuis le début ou pas.
        }
    }

    void stop()
    {
        if(erreur==BASS_OK)
        {
            BASS_ChannelStop(musique_id);
        }
    }

    void determine_volume(float v) //Entre 0 et 1
    {
            if(erreur==BASS_OK)
            {
                BASS_ChannelSetAttribute(musique_id,BASS_ATTRIB_VOL,v);
            }
    }

    void renvoie_FFT()
    {
        BASS_ChannelGetData(musique_id,tempon_FFT,BASS_DATA_FFT512);
    }

};

#endif
