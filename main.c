#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef enum {
    VILLAGEOIS,
    SORCIERE,
    PETITE_FILLE,
    CHASSEUR,
    LOUP,
    CUPIDON
} Role;

typedef struct {
    char name[50];
    Role role;
    int alive;
    int votes;
    int lover; // stocke l'indice de l'amoureux (-1 si pas amoureux)
} Player;

// Prototypes
int select_number_of_players();


//Main
int main() {
    srand(time(NULL));

    int nbPlayers = select_number_of_players();
    Player *players = malloc(sizeof(Player) * nbPlayers);

    int potionVie = 1, potionMort = 1;

    printf("\n--- Initialisation des joueurs ---\n");
    for (int i = 0; i < nbPlayers; i++) {
        printf("Nom du joueur %d : ", i + 1);
        fgets(players[i].name, 50, stdin);
        players[i].name[strcspn(players[i].name, "\n")] = '\0'; // Suppression du saut de ligne
        players[i].alive = 1;
        players[i].votes = 0;
        players[i].lover = -1;
    }
}
int select_number_of_players() {
    int n;
    do {
        printf("Combien de joueurs ? (minimum 6) : ");
        if (scanf("%d", &n) != 1 || n < 6) {
            printf("Entrée invalide. Veuillez entrer un nombre >= 6.\n");
            while (getchar() != '\n');
        }
    } while (n < 6);
    while (getchar() != '\n');
    return n;
}

