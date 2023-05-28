#ifndef DKSMATRICE4_H_INCLUDED
#define DKSMATRICE4_H_INCLUDED

//********************************************************************************
//                  Gestion des matrices de transformations spatiales
//********************************************************************************

#define Ma 0
#define Mb 4
#define Mc 8
#define Md 12

#define Me 1
#define Mf 5
#define Mg 9
#define Mh 13

#define Mi 2
#define Mj 6
#define Mk 10
#define Ml 14

#define Mm 3
#define Mn 7
#define Mo 11
#define Mp 15

class dksMatrice4
{
    public:
        double elements[16];

    dksMatrice4()
    {
        identite();
    }

    //========== Initialise la matrice identité:

    void identite()
    {
        elements[Ma]=1.; elements[Mb]=0.; elements[Mc]=0.; elements[Md]=0.;
        elements[Me]=0.; elements[Mf]=1.; elements[Mg]=0.; elements[Mh]=0.;
        elements[Mi]=0.; elements[Mj]=0.; elements[Mk]=1.; elements[Ml]=0.;
        elements[Mm]=0.; elements[Mn]=0.; elements[Mo]=0.; elements[Mp]=1.;
    }

    //============ Transformations:
    void calcul_rotation(double angle, double ax, double ay, double az)
    {
        double cos_a = cosf(angle);
        double un_moins_cos = 1. - cos_a;

        double sin_a= sinf(angle);

        elements[Ma] = cos_a + ax * ax * un_moins_cos;
        elements[Mb] = -sin_a * az + un_moins_cos * ax * ay;
        elements[Mc] = sin_a * ay + un_moins_cos * ax * az;
        elements[Md] = 0.;

        elements[Me] = az * sin_a + un_moins_cos * ax * ay;
        elements[Mf] = cos_a + un_moins_cos * ay * ay;
        elements[Mg] = -sin_a * ax + un_moins_cos * az * ay;
        elements[Mh] = 0.;

        elements[Mi] = -sin_a * ay + un_moins_cos * az * ax;
        elements[Mj] = ax * sin_a + un_moins_cos * ay * az;
        elements[Mk] = cos_a + un_moins_cos * az * az;
        elements[Ml] = 0.;
    }

    void produit(dksMatrice4* ma_matrice, dksMatrice4* dest_matrice)
    {
        dest_matrice->elements[Ma] = elements[Ma] * ma_matrice->elements[Ma] + elements[Mb] * ma_matrice->elements[Me] + elements[Mc] * ma_matrice->elements[Mi];
        dest_matrice->elements[Mb] = elements[Ma] * ma_matrice->elements[Mb] + elements[Mb] * ma_matrice->elements[Mf] + elements[Mc] * ma_matrice->elements[Mj];
        dest_matrice->elements[Mc] = elements[Ma] * ma_matrice->elements[Mc] + elements[Mb] * ma_matrice->elements[Mg] + elements[Mc] * ma_matrice->elements[Mk];
        dest_matrice->elements[Md] = elements[Ma] * ma_matrice->elements[Md] + elements[Mb] * ma_matrice->elements[Mh] + elements[Mc] * ma_matrice->elements[Ml] + elements[Md];

        dest_matrice->elements[Me] = elements[Me] * ma_matrice->elements[Ma] + elements[Mf] * ma_matrice->elements[Me] + elements[Mg] * ma_matrice->elements[Mi];
        dest_matrice->elements[Mf] = elements[Me] * ma_matrice->elements[Mb] + elements[Mf] * ma_matrice->elements[Mf] + elements[Mg] * ma_matrice->elements[Mj];
        dest_matrice->elements[Mg] = elements[Me] * ma_matrice->elements[Mc] + elements[Mf] * ma_matrice->elements[Mg] + elements[Mg] * ma_matrice->elements[Mk];
        dest_matrice->elements[Mh] = elements[Me] * ma_matrice->elements[Md] + elements[Mf] * ma_matrice->elements[Mh] + elements[Mg] * ma_matrice->elements[Ml] + elements[Mh];

        dest_matrice->elements[Mi] = elements[Mi] * ma_matrice->elements[Ma] + elements[Mj] * ma_matrice->elements[Me] + elements[Mk] * ma_matrice->elements[Mi];
        dest_matrice->elements[Mj] = elements[Mi] * ma_matrice->elements[Mb] + elements[Mj] * ma_matrice->elements[Mf] + elements[Mk] * ma_matrice->elements[Mj];
        dest_matrice->elements[Mk] = elements[Mi] * ma_matrice->elements[Mc] + elements[Mj] * ma_matrice->elements[Mg] + elements[Mk] * ma_matrice->elements[Mk];
        dest_matrice->elements[Ml] = elements[Mi] * ma_matrice->elements[Md] + elements[Mj] * ma_matrice->elements[Mh] + elements[Mk] * ma_matrice->elements[Ml] + elements[Ml];
    }

    //---------- Pour transformer un repère par la matrice:

    void applique_vecteur(dksVecteur* vecteur, dksVecteur* dest_vect)
    {
        dest_vect->x = vecteur->x * elements[Ma] + vecteur->y * elements[Mb] + vecteur->z * elements[Mc] + elements[Md];
        dest_vect->y = vecteur->x * elements[Me] + vecteur->y * elements[Mf] + vecteur->z * elements[Mg] + elements[Mh];
        dest_vect->z = vecteur->x * elements[Mi] + vecteur->y * elements[Mj] + vecteur->z * elements[Mk] + elements[Ml];
    }

    void applique_axe(dksVecteur* vecteur, dksVecteur* dest_vect)
    {
        dest_vect->x = vecteur->x * elements[Ma] + vecteur->y * elements[Mb] + vecteur->z * elements[Mc];
        dest_vect->y = vecteur->x * elements[Me] + vecteur->y * elements[Mf] + vecteur->z * elements[Mg];
        dest_vect->z = vecteur->x * elements[Mi] + vecteur->y * elements[Mj] + vecteur->z * elements[Mk];

        double norme = sqrtf( dest_vect->x * dest_vect->x + dest_vect->y * dest_vect->y + dest_vect->z * dest_vect->z );
        dest_vect->x /= norme;
        dest_vect->y /= norme;
        dest_vect->z /= norme;
    }

};

#endif // DKSMATRICE4_H_INCLUDED
