#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
#define F_CHARIOT   26.00 * 1000 // N * m^-1 * s^-1

#define F_PIVOT     0.00900 // N * m * rad^-1 * s^-1

// Constantes dépendates des valeurs précédentes
#define M_SUBSYS    M_BRAS + M_MASSE // Kg
#define DIST_PG     ((M_MASSE * DIST_MASSE) + ((M_BRAS * L_BRAS) / 2.0 )) \
    / (M_BRAS + M_MASSE) // m

// Moment d'inerties (Kg * m^2)
#define I_MASSE     M_MASSE * DIST_MASSE * DIST_MASSE + M_MASSE * ( 3.0 * \
    (R_BRAS * R_BRAS + R_MASSE * R_MASSE ) + H_MASSE * H_MASSE ) / 12.0
#define I_BRAS      M_BRAS * (R_BRAS * R_BRAS + L_BRAS * L_BRAS / 3.0) / 4.0



/**
 * Affiche 5 doubles en notation scientifique séparés par des tabulations.
 * @param temps Temps écoulé depuis le début de la simulation.
 * @param pos Position du chariot sur le rail.
 * @param vit Vitesse du chariot.
 * @param angle Position angulaire du pendule.
 * @param vitAng Vitesse angulaire du pendule.
 */
void impLigneDonnees( double temps, double pos, double vit, double angle, \
    double vitAng) {

    printf("%e\t%e\t%e\t%e\t%e\n", temps, pos, vit, angle, vitAng);
}


/**
 * Fonction principale.
 * @param argc Nombre d'arguments passés en ligne de commande.
 * @param argv Tableau contenant les arguments passés en ligne de commande.
 * @return Code de sortie.
 */
int main(int argc, char *argv[]) {

    // Set locale
	char *s=setlocale(LC_NUMERIC,"fr_FR");
	double float a = 
	double float b = 
	double float c = 
	double float d =
	double float e = 
	double float f = 
	double float g = 
	double float h = 
	 

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
    double pas      = atof(argv[1]);
    double duree    = atof(argv[2]);
    double angle    = atof(argv[3]);


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
