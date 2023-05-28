#ifndef IMAGE_INFOS_H_INCLUDED
#define IMAGE_INFOS_H_INCLUDED


//---------------------------------------------------------------
//          Information sur l'image
//      C'est une structure intermédiaire, renvoyée par
//      les classes statiques gérants les différents formats
//---------------------------------------------------------------

class Image_infos
{
    public:
        uint32_t    largeur;
        uint32_t    hauteur;
        uint8_t*       image;
        uint16_t    bits_par_pixel;
        uint16_t    octets_par_pixel;
        uint32_t    palette[256];
        //uint16_t    octets_par_pixels;

    Image_infos()
    {
        largeur=0;
        hauteur=0;
        image=NULL;
        bits_par_pixel=0;
        octets_par_pixel=0;
    }

};

#endif // IMAGE_INFOS_H_INCLUDED
