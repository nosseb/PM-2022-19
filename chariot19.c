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

#define L_BRAS      0.15000 // m
#define D_BRAS      0.00300 // m
#define M_BRAS      3.20000 // Kg

#define H_MASSE     0.00600 // m
#define D_MASSE     0.00700 // m
#define M_MASSE     2.00000 // Kg

#define DIST_MASSE  0.14600 // m

#define M_CHARIOT   3.00000 // Kg
#define F_CHARIOT   26.00 * 1000 // N * m^-1 * s^-1

#define F_PIVOT     0.00900 // N * m * rad^-1 * s^-1


/**
 * Affiche 5 doubles en notation scientifique séparés par des tabulations.
 * @param temps Temps écoulé depuis le début de la simulation.
 * @param pos Position du chariot sur le rail.
 * @param vit Vitesse du chariot.
 * @param angle Position angulaire du pendule.
 * @param vitAng Vitesse angulaire du pendule.
 */
void impLigneDonnees(double temps, double pos, double vit, double angle, double vitAng) {
    printf("%e\t%e\t%e\t%e\t%e\n", temps, pos, vit, angle, vitAng);
}


/* Execution */
int main(int argc, char *argv[]) {

    // Set locale
	char *s=setlocale(LC_NUMERIC,"fr_FR");

    // Check arguments
	if (s == NULL) {
		/* Sur Linux vérifier la présence des paquets locales et locales-all  */
		printf("Échec de la francisation des nombres !\n");
		return EXIT_FAILURE;
	}
	if (argc!=4) {
		printf("Usage : %s pas durée angle\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Récupération des valeurs numériques des arguments par atof() */
    double pas = atof(argv[1]);
    double duree = atof(argv[2]);
    double angle = atof(argv[3]);


	/* Votre code */

    // TODO: Calculer le moment d'inertie (variable statique).

    // TODO: Vérifier que le temps d'échantillonnage est infferieur à tau.

    // Affiche l'en-tête du tableau.
    printf("Temps (s)    \tpos. (cm)    \tvit (cm/s)    \tangle (°)    \tvit. ang. (°/s)\n");

    /*
    impLigneDonnées(0, 78798, 34567, 34567, 345);
    impLigneDonnées(678, 6789, 56789, 5678, 6789);
    impLigneDonnées(456, 4567, 678, 5678, 45678);
    */

	return EXIT_SUCCESS;
}
