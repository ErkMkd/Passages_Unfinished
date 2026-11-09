#version 460 core
//======================================================================
//      En-tête commun, ajouté par le moteur devant chaque shader, suivi de
//      sommets.glsl (vertex shaders) ou de fragments.glsl (fragment shaders).
//      Les blocs d'uniforms sont remplis par le C++ (disposition std140):
//          binding 0: dksMatrices
//          binding 1 et 2: dksEclairage
//======================================================================

#define NBR_SOURCES 8

layout(std140, binding=0) uniform Matrices
{
    mat4 matrice_modele_vue;
    mat4 matrice_projection;
    mat4 matrice_modele_vue_projection;
    mat4 matrice_texture;
    mat3 matrice_normale;
};

struct Source
{
    vec4 position;              //Repère de l'oeil
    vec4 ambiante;
    vec4 diffuse;
    vec4 speculaire;
    vec4 direction_spot;        //Repère de l'oeil
    float exposant_spot;
    float ouverture_spot;       //En degrés, 180: pas de spot
    float cos_ouverture_spot;
    float attenuation_constante;
    float attenuation_lineaire;
    float attenuation_quadratique;
};

layout(std140, binding=1) uniform Eclairage
{
    Source sources[NBR_SOURCES];
    vec4 ambiante_globale;
};

layout(std140, binding=2) uniform Materiau
{
    vec4 materiau_ambiante;
    vec4 materiau_diffuse;
    vec4 materiau_speculaire;
    vec4 materiau_emission;
    float materiau_brillance;
};
