#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

	/********************************************************************
	 *
	 * CRETIN Nicolas & BESSON Robinson
	 * Groupe n° 19
	 *
	 ********************************************************************/


/* Configuration */

#define L_BRAS      1.50000 // m
#define D_BRAS      0.03000 // m
#define R_BRAS      0.01500 // m
#define M_BRAS      3.20000 // Kg

#define H_MASSE     0.06000 // m
#define D_MASSE     0.07000 // m
#define R_MASSE     0.03500 // m
#define M_MASSE     2.00000 // Kg

#define DIST_MASSE  1.46000 // m

#define M_CHARIOT   3.00000 // Kg
#define F_CHARIOT   2.600 * 1000 // N * m^-1 * s^-1

#define F_PIVOT     0.0900 // N * m * s* rad^-1 

// Constantes dépendates des valeurs précédentes
#define M_SUBSYS    M_BRAS + M_MASSE // Kg
#define DIST_PG     ((M_MASSE * DIST_MASSE) + ((M_BRAS * L_BRAS) / 2.0 )) \
    / (M_BRAS + M_MASSE) // m

// Moment d'inerties (Kg * m^2)
#define I_MASSE     M_MASSE * DIST_MASSE * DIST_MASSE + M_MASSE * ( 3.0 * \
    (R_BRAS * R_BRAS + R_MASSE * R_MASSE ) + H_MASSE * H_MASSE ) / 12.0
#define I_BRAS      M_BRAS * (R_BRAS * R_BRAS + L_BRAS * L_BRAS / 3.0) / 4.0


/**
 * Une structure pour représenter un vecteur de taille 3.
 */
typedef struct Vecteur {
    double x;
    double y;
    double z;
} vecteur;


/**
 * @brief Structure pour stocker les résultat de RK4 (position et vitesse).
 * 
 */
typedef struct RK4Result {
    vecteur position;
    vecteur vitesse;
} rk4_result;


/**
 * @brief Calcul la somme de vecteurs.
 *
 * @param v1 Pointeur vers le premier vecteur.
 * @param v2 Pointeur vers le deuxième vecteur.
 * @param NULL Indicateuur de fin de liste.
 * @return vecteur résultat de la somme.
 */
vecteur *vectSum(const vecteur * ftm, ...) {
    // Initialisation nb de paramètres variable.
    va_list ap;
    va_start(ap, ftm);

    // Initialisation du résultat.
    vecteur *res = malloc(sizeof(vecteur));
    res->x = 0;
    res->y = 0;
    res->z = 0;

    // Calcul de la somme.
    while (ftm != NULL) {
        res->x += ftm->x;
        res->y += ftm->y;
        res->z += ftm->z;
        ftm = va_arg(ap, vecteur *);
    }

    // Fin nb de paramètres variable.
    va_end(ap);

    return res;
}


/**
 * @brief Calcul le produit entre un scalar et un vecteur.
 *
 * @param v1 Vecteur.
 * @param s Scalar.
 */
vecteur *vectScalar(vecteur *v, double s) {
    vecteur *res = malloc(sizeof(vecteur));
    res->x = v->x * s;
    res->y = v->y * s;
    res->z = v->z * s;
    return res;
};


/**
 * @brief Affichage des valeurs formattées.
 * Inclus les convertions d'unités.
 *
 * @param temps Temps (s) écoulé depuis le début de la simulation.
 * @param pos Position (m) du chariot sur le rail.
 * @param vit Vitesse (m/s) du chariot.
 * @param angle Position (rad) angulaire du pendule.
 * @param vitAng Vitesse (rad/s) angulaire du pendule.
 */
void impLigneDonnees( double temps, double pos, double vit, double angle, \
    double vitAng) {

    // Convertion des unitées
    pos = pos*100; // m -> cm
    vit = vit*100; // m/s ->  cm*/s
    angle = angle*360/(2.0*3.14);
    vitAng=vitAng*360/(2.0*3.14);
    
    printf("%e\t%e\t%e\t%e\t%e\n", temps, pos, vit, angle, vitAng);
}


/**
 * @brief Calcul la dérivée seconde.
 *
 * @param time Temps écoulé depuis le début de la simulation.
 * @param *pos Pointeur vers vecteur position.
 * @param *vit Pointeur vers vecteur vitesse.
 * @return Le vecteur dérivée seconde.
 */
vecteur *dSec(double time, vecteur *pos, vecteur *vit) {

     // Variables intermédiaires
    float a = 8.2;
	float b = 5.3196*pow(10,-4)*cos(pos->x);
	float c = -2600; // valeur à vérifier : semble très grande
	float d = 5.3196*pow(10,-4)*sin(pos->x);
	float e = 5.3196*pow(10,-4)*cos(pos->x);
	float f = 4.865000054;
	float g1= -5.2185276*pow(10,-3)*sin(pos->x);
	float h = -0.09;
	
	// équations de notre système    
    vecteur *res = malloc(sizeof(vecteur));

	res->x = // dérivée seconde de x 
        (g1-vit->x*(c*e/a)
		+vit->y*h
		-pow(vit->y,2)*(d*e/a))
		/(f-b*e/a);

	res->y = // dérivée seconde de y
        (g1-vit->x*(c*f/b) 
		+vit->y*h
		-pow(vit->y,2)*(d*f/b))
		/(e-a*f/b);

    return res;
}

/**
 * @brief Exécute la méthode de Runge-Kutta de 4e ordre.
 *
 * @param *dsec Fonction à utiliser pour calculer la dérivée seconde.
 * @param time Temps écoulé depuis le début de la simulation.
 * @param *pos Pointeur vers le vecteur position.
 * @param *vit Pointeur vers le vecteur vitesse.
 * @param dt Temps écoulé entre deux itérations.
 */
rk4_result *rangeKutta(
    void (*dsec)(vecteur *, vecteur *, double),
    double time, vecteur *pos, vecteur *vit, double dt) {

    // Valeurs intermédiaires.
    vecteur *Ka = dSec(time, pos, vit);
    
    vecteur *Kb = dSec(
        time + dt/2.0, 
        vectSum(pos, vectScalar(vit, dt/2.0), NULL),
        vectSum(vit, vectScalar(Ka, dt/2.0), NULL));
    
    vecteur *Kc = dSec(
        time + dt/2.0,
        vectSum(pos, vectScalar(vit, dt/2.0), 
            vectScalar(Ka, dt*dt/4.0), NULL),
        vectSum(vit, vectScalar(Kb, dt/2.0), NULL));
    
    vecteur *Kd = dSec(
        time + dt,
        vectSum(pos, vectScalar(vit, dt), vectScalar(Kb, dt/2.0), NULL),
        vectSum(vit, vectScalar(Kc, dt), NULL));
    
    rk4_result *res = malloc(sizeof(rk4_result));

    // Calcul de la position et de la vitesse.
    // TODO: check si les deux termes sont inversés.
    res->position = *vectSum(
        pos, 
        vectScalar(vit, dt), 
        vectScalar(Ka, dt*dt/6.0),
        vectScalar(Kb, dt*dt/6.0),
        vectScalar(Kc, dt*dt/6.0),
        NULL);
    res->vitesse = *vectSum(
        vit,
        vectScalar(Ka, dt/6.0),
        vectScalar(Kb, dt/3.0),
        vectScalar(Kc, dt/3.0),
        vectScalar(Kd, dt/6.0),
        NULL);
    
    return res;
}


/**
 * @brief Fonction principale.
 *
 * @param argc Nombre d'arguments passés en ligne de commande.
 * @param argv Tableau contenant les arguments passés en ligne de commande.
 * @return Code de sortie.
 */
int main(int argc, char *argv[]) {

    // Set locale
	char *s=setlocale(LC_NUMERIC,"fr_FR");
	

    // Check arguments
	if (s == NULL) {
		/* Sur Linux vérifier la présence des paquets locales et locales-all */
		printf("Échec de la francisation des nombres !\n");
		return EXIT_FAILURE;
	}
	if (argc!=4) {
		printf("Usage : %s pas durée angle\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Récupération des valeurs numériques des arguments par atof() */
    double pas      = atof(argv[1]); // (s)
    double duree    = atof(argv[2]); // (s)
    double angle    = atof(argv[3]); // (deg)
    // conversion edeg -> rad.
    angle = angle*2.0*3.14/360.0;


	/* Votre code */

    // TODO: Calculer le moment d'inertie (variable statique).

    // TODO: Vérifier que le temps d'échantillonnage est infferieur à tau.

    // Affiche l'en-tête du tableau.
    printf("Temps (s)    \tpos. (cm)    \tvit (cm/s)    \tangle (°)    "
        "\tvit. ang. (°/s)\n");

    /*
    impLigneDonnées(0, 78798, 34567, 34567, 345);
    impLigneDonnées(678, 6789, 56789, 5678, 6789);
    impLigneDonnées(456, 4567, 678, 5678, 45678);
    */

	return EXIT_SUCCESS;
}
