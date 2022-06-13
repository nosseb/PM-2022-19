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
     * Groupe gagnant 
	 *
	 ********************************************************************/





/*******************************************************************
 *                                                                 *
 *                          Configuration                          *
 *                                                                 *
 *******************************************************************/


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



/*******************************************************************
 *                                                                 *
 *                      Structures de données                      *
 *                                                                 *
 *******************************************************************/


/**
 * Une structure pour représenter un vecteur de taille 2.
 */
typedef struct Vecteur {
    double x;
    double y;
    bool memlocked;
} vecteur;


/**
 * @brief Structure pour stocker les résultat de RK4 (position et vitesse).
 * 
 */
typedef struct RK4Result {
    vecteur position;
    vecteur vitesse;
} rk4_result;



/*******************************************************************
 *                                                                 *
 *                          Fonctions maths                        *
 *                                                                 *
 *******************************************************************/


/**
 * @brief Calcul la somme de vecteurs.
 * Prend en paramètre un nombre variable de vecteurs (1 minimum) et calcule
 * leur somme.
 * Tout vecteur n'aitant pas marqué comme "memlocked" est supprimé de la 
 * mémoire.
 * Le résultat n'est pas marqué comme "memlocked".
 *
 * @param ftm Pointeur vers le premier vecteur.
 * @return Pointeur vers vecteur résultat de la somme.
 */
vecteur *vectSum(vecteur * ftm, ...) {
    // ftm est le dernier paramètre dont on connais l'adresse.
    // Il est utilisé pour récupérer la liste des autres paramètres.
    // Le ... représente la liste variable d'arguments.

    // Initialisation liste de paramètres variable.
    va_list ap;
    va_start(ap, ftm); // On indique l'adresse du dernier paramètre.

    // Initialisation du résultat.
    vecteur *res = malloc(sizeof(vecteur));
    res->x = 0;
    res->y = 0;
    // Par défaut, le vecteur résultat n'est pas verrouillé.
    res->memlocked = false;

    // Calcul de la somme.
    // Lors de la première itération, ftm est le premier paramètre, donnée de
    // manière explicite. Dans les itérations suivantes, ftm est récupéré via
    // la va_arg.
    while (ftm != NULL) {
        res->x += ftm->x;
        res->y += ftm->y;

        // Si le vecteur n'est pas verrouillé, on le libère.
        if (!(ftm->memlocked)) free(ftm);

        // On récupère le prochain paramètre.
        ftm = va_arg(ap, vecteur *);
    }

    // Fin nb de paramètres variable.
    va_end(ap);

    return res;
}


/**
 * @brief Calcul le produit entre un scalar et un vecteur.
 * Dans le cas ou le vecteur paramètre n'est pas marqué comme "memlocked", il 
 * est supprimé.
 * Le résultat n'est pas marqué comme "memlocked".
 *
 * @param v Pointeur vers un vecteur.
 * @param s Scalaire.
 * @return Pointeur vers vecteur résultat du produit.
 */
vecteur *vectScalar(vecteur *v, double s) {
    // Initialisation du résultat.
    vecteur *res = malloc(sizeof(vecteur));
    // Par défaut, le vecteur résultat n'est pas verrouillé.
    res->memlocked = false;

    // Calcul du produit.
    res->x = v->x * s;
    res->y = v->y * s;

    // Si le vecteur paramètre n'est pas verrouillé, on le libère.
    if (!(v->memlocked)) free(v);

    return res;
};


/**
 * @brief Exécute une itération de la méthode de Runge-Kutta de 4e ordre.
 *
 * @param dsec Pointeur vers la fonction à utiliser pour la dérivée seconde.
 * @param time Temps écoulé depuis le début de la simulation.
 * @param pos Pointeur vers le vecteur position.
 * @param vit Pointeur vers le vecteur vitesse.
 * @param dt Temps écoulé entre deux itérations.
 * @return Pointeur vers le résultat de la méthode de Runge-Kutta.
 */
rk4_result *rangeKutta(
    vecteur* (*dsec)(double, vecteur *, vecteur *),
    double time, vecteur *pos, vecteur *vit, double dt) {
    // Initialisation du résultat.
    rk4_result *res = malloc(sizeof(rk4_result));

    // Valeurs intermédiaires.
    // On vérrouille les vecteurs car ils sont utilisés plusieurs fois.
    vecteur *Ka = dsec(time, pos, vit);
    Ka->memlocked = true;
    
    vecteur *Kb = dsec(
        time + dt/2.0, 
        vectSum(pos, vectScalar(vit, dt/2.0), NULL),
        vectSum(vit, vectScalar(Ka, dt/2.0), NULL));
    Kb->memlocked = true;
    
    vecteur *Kc = dsec(
        time + dt/2.0,
        vectSum(pos, vectScalar(vit, dt/2.0), 
            vectScalar(Ka, dt*dt/4.0), NULL),
        vectSum(vit, vectScalar(Kb, dt/2.0), NULL));
    Kc->memlocked = true;
    
    vecteur *Kd = dsec(
        time + dt,
        vectSum(pos, vectScalar(vit, dt), vectScalar(Kb, dt/2.0), NULL),
        vectSum(vit, vectScalar(Kc, dt), NULL));
    Kd->memlocked = true;

    // Calcul de la position et de la vitesse.
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
    
    // Libération des variables intermédiaires.
    free(Ka);
    free(Kb);
    free(Kc);
    free(Kd);
    
    return res;
}


/*******************************************************************
 *                                                                 *
 *                           Fonctions UX                          *
 *                                                                 *
 *******************************************************************/


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



/*******************************************************************
 *                                                                 *
 *                         Fonctions modèle                        *
 *                                                                 *
 *******************************************************************/


/**
 * @brief Calcul la dérivée seconde.
 *
 * @param time Temps écoulé depuis le début de la simulation.
 * @param pos Pointeur vers vecteur position.
 * @param vit Pointeur vers vecteur vitesse.
 * @return Pointeur vers le vecteur dérivée seconde.
 */
vecteur *dSec(double time, vecteur *pos, vecteur *vit) {
    // Initialisation du résultat.
    vecteur *res = malloc(sizeof(vecteur));
    // Par défaut, le vecteur résultat n'est pas verrouillé.
    res->memlocked = false;

    // Variables intermédiaires
    double a = 8.2;
	double b = 5.3196*pow(10.0,-4.0)*cos(pos->x);
	double c = -2600;
	double d = 5.3196*pow(10.0,-4.0)*sin(pos->x);
	double e = 5.3196*pow(10.0,-4.0)*cos(pos->x);
	double f = 4.865000054;
	double g1= -5.2185276*pow(10.0,-3.0)*sin(pos->x);
	double h = -0.09;
	
	// équations de notre système

    // Dérivée seconde de la position.
	res->x =
        (g1-vit->x*(c*e/a)
		+vit->y*h
		-pow(vit->y,2.0)*(d*e/a))
		/(f-b*e/a);

    // Dérivée seconde de l'angle.
	res->y =
        (g1-vit->x*(c*f/b) 
		+vit->y*h
		-pow(vit->y,2.0)*(d*f/b))
		/(e-a*f/b);
    
    // Libération des vecteurs paramètres si non verrouillés.
    if (!(pos->memlocked)) free(pos);
    if (!(vit->memlocked)) free(vit);

    return res;
}



/*******************************************************************
 *                                                                 *
 *                       Programme principal                       *
 *                                                                 *
 *******************************************************************/


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
    double pas      = atof(argv[1]); // (s) // float ????
    double duree    = atof(argv[2]); // (s)
    double angle    = atof(argv[3]); // (deg)
    // conversion edeg -> rad.
    angle = angle*2.0*3.14/360.0;


    // Affiche l'en-tête du tableau.
    printf("Temps (s)    \tpos. (cm)    \tvit (cm/s)    \tangle (°)    "
        "\tvit. ang. (°/s)\n");

    double nbreElemBoucle_s = duree/pas;

    // Initialisation simulation.

    vecteur *pos_v = malloc(sizeof(vecteur));
    vecteur *vit_v = malloc(sizeof(vecteur));
    vecteur *Y = malloc(sizeof(vecteur));
    vecteur *Y_Point = malloc(sizeof(vecteur));
    vecteur *Y_Point_Point = malloc(sizeof(vecteur));
    rk4_result *Y_rk4 = malloc(sizeof(rk4_result));
    
    // TODO Contruction des vecteurs positions et vitesses
    // (sous forme de pointeur)
    double pos_s = 0; // on part de x=zero
    double vit_s = 0; // vitesse initale nulle
    // angle déja défini
    double vitAng_s = 0; 
    double temps_s = 0;
    
    
    // Ce sont cest vecteurs Y et Y_Point qui doivent être donnés en paramètres
    // à rangeKutta. WHAAAAAAATTTTTT ??
    
    // TODO Je pense que ça aussi pourait être déplacé
    // Peut-être suite au calcul du nombre de tour de boucle.

    impLigneDonnees(temps_s, pos_s, vit_s, angle, vitAng_s);

    for (int i=1;i<nbreElemBoucle_s;i++){
        
        // TODO Prendre comme paramètre les vecteurs Y et Y_Point.

        // ancien ligne : 
        //Y_rk4 = rangeKutta(*dSec,temps_s,pos_v,vit_v,pas);

        // Y_point_point = ....
        
        Y_rk4 = rangeKutta(Y_Point_Point,temps_s,pos_v,vit_v,pas);
        // TODO verifier type de sortie rangeKutta vecteur versus pointeur

        temps_s = temps_s + pas;
        Y = Y_rk4->position;
        Y_Point = Y_rk4->vitesse;
        pos_s = Y->x;
        vit_s = Y_Point->x;
        angle = Y->y;
        vitAng_s = Y_Point->y;
        
        impLigneDonnees(temps_s, pos_s, vit_s, angle, vitAng_s);

    }

	return EXIT_SUCCESS;
}
