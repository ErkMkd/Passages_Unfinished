#ifndef PANNEAU_DEVELOPPEMENT_H_INCLUDED
#define PANNEAU_DEVELOPPEMENT_H_INCLUDED

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Horloge.h"

//*************************************************************************
//      Panneau de développement (Dear ImGui), affiché par-dessus la démo:
//      scène, temps, durée de calcul des images, pause, vitesse.
//*************************************************************************

class Panneau_Developpement
{
    private:
        static const int NBR_MESURES=120;
        float durees_calcul[NBR_MESURES];   //Durées de calcul des dernières images (en ms)
        int indice_mesure;

    public:
        //Les fonctions clavier et souris déjà installées sur la fenêtre restent appelées.
        Panneau_Developpement(GLFWwindow* fenetre)
        {
            for (int i=0;i<NBR_MESURES;i++) durees_calcul[i]=0.f;
            indice_mesure=0;

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io=ImGui::GetIO();
            io.IniFilename=NULL;    //Pas de fichier imgui.ini
            const char* police="C:\\Windows\\Fonts\\segoeui.ttf";   //Pour les accents
            if (GetFileAttributesA(police)!=INVALID_FILE_ATTRIBUTES) io.Fonts->AddFontFromFileTTF(police,15.f);
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(fenetre,true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }

        ~Panneau_Developpement()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        //Quand le panneau utilise le clavier ou la souris, ils ne sont pas transmis à la démo.
        static bool utilise_clavier()
        {
            return ImGui::GetCurrentContext()!=NULL && ImGui::GetIO().WantCaptureKeyboard;
        }

        static bool utilise_souris()
        {
            return ImGui::GetCurrentContext()!=NULL && ImGui::GetIO().WantCaptureMouse;
        }

        //À appeler à la fin de l'image, juste avant l'échange des tampons.
        void affiche(Horloge* horloge,const char* nom_scene,double duree_calcul)
        {
            durees_calcul[indice_mesure]=float(duree_calcul*1000.);
            indice_mesure=(indice_mesure+1)%NBR_MESURES;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Replié, le panneau n'affiche que sa barre de titre: scène, temps et cadence.
            char titre[160];
            snprintf(titre,sizeof(titre),"%s   %.2f s   %.0f i/s%s###Passages",nom_scene,horloge->renvoie_temps(),
                     ImGui::GetIO().Framerate,horloge->pause ? "   (pause)" : "");
            ImGui::SetNextWindowPos(ImVec2(6.f,6.f),ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowCollapsed(true,ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.65f);
            if (ImGui::Begin(titre,NULL,ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Calcul de l'image : %.1f ms",duree_calcul*1000.);
                ImGui::PlotLines("##durees",durees_calcul,NBR_MESURES,indice_mesure,NULL,0.f,33.f,ImVec2(240.f,32.f));

                ImGui::Checkbox("Pause",&horloge->pause);
                if (horloge->pause)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Image suivante")) horloge->image_suivante();
                }
                float vitesse=float(horloge->vitesse);
                if (ImGui::SliderFloat("Vitesse",&vitesse,0.1f,4.f,"x%.2f",ImGuiSliderFlags_Logarithmic)) horloge->vitesse=vitesse;
                ImGui::SameLine();
                if (ImGui::SmallButton("x1")) horloge->vitesse=1.;

                if (ImGui::CollapsingHeader("OpenGL"))
                {
                    ImGui::TextUnformatted((const char*)glGetString(GL_RENDERER));
                    ImGui::TextUnformatted((const char*)glGetString(GL_VERSION));
                }
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
};

#endif // PANNEAU_DEVELOPPEMENT_H_INCLUDED
