// le timer en externe pour pouvoir l'utiliser dans le calcul du temps de réveil des processus endormis
extern volatile int timer;

// initialiser le timer pour générer une interruption toutes les millisecondes
void init_timer();

// handler de l'interruption du timer
void handler_timer_C();

// afficher la durée d'exécution du système au format hh:mm:ss
void duree_exec();