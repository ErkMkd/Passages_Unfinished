//======================================================================
//      Attributs des sommets, ajoutés par le moteur devant chaque vertex
//      shader (après communs.glsl). Leurs emplacements sont ceux du VAO
//      de dksImmediat.
//      Emplacements 4 à 7: attributs propres à chaque shader (tangentes...),
//      déclarés par le shader avec layout(location=...).
//======================================================================

layout(location=0) in vec4 sommet_position;
layout(location=1) in vec3 sommet_normale;
layout(location=2) in vec4 sommet_couleur;
layout(location=8) in vec4 sommet_texture0;
layout(location=9) in vec4 sommet_texture1;
layout(location=10) in vec4 sommet_texture2;
layout(location=11) in vec4 sommet_texture3;
