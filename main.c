#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <conio.h> // Windows uniquement pour _getch()
#include <windows.h>
#define SLEEP(x) Sleep((x) * 1000)// Windows : Sleep prend des millisecondes

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

// Prototypes:

// Demande à l'utilisateur le nombre de joueurs et s'assure qu'il est valide (>= 6).
// Retourne : Le nombre de joueurs sélectionnés.
int select_number_of_players();


//Attends que la touche espace soit appuyé
//Utile pour distribuer les rôles sans voir les rôles de tous
void wait_for_space();

// Assigne les rôles aux joueurs de manière aléatoire (Villageois, Loups, etc.).
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void assign_roles(Player players[], int nbPlayers);

// Retourne une chaîne de caractères représentant le nom d'un rôle.
// Paramètres :
//   - role : Rôle à convertir en chaîne.
// Retourne : Une chaîne constante contenant le nom du rôle (ex : "Villageois", "Loup-Garou").
const char* roleToString(Role role);

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

    printf("\n--- Distribution des rôles en cours ---\n");
    SLEEP(1);
    assign_roles(players, nbPlayers);
}

int select_number_of_players() {
    int n;
    do {
        printf("Combien de joueurs ? (minimum 6) : ");
        if (scanf("%d", &n) != 1 || n < 6) {
            printf("Entrée invalide. Veuillez entrer un nombre >= 6.\n");
            while (getchar() != '\n'); //Nettoyer le buffer
        }
    } while (n < 6);
    while (getchar() != '\n'); //Nettoyer le buffer
    return n;
}


void wait_for_space() {
    printf("Appuie sur [Espace] pour continuer...\n");
    while (_getch() != ' ') {
        // Ne fait rien tant que la touche pressée n'est pas l'espace
    }
}

void assign_roles(Player players[], int nbPlayers) {

    //Création d'une liste avec Role
    Role roles[nbPlayers];
    int index = 0;

    //Ajout des rôles spéciaux uniques à la liste
    roles[index++] = SORCIERE;
    roles[index++] = CHASSEUR;
    roles[index++] = CUPIDON;
    roles[index++] = PETITE_FILLE;

    //Nombre de loups par rapport au nombre de joueur
    int nbLoups = nbPlayers / 3;
    for (int i = 0; i < nbLoups; i++) roles[index++] = LOUP;

    //Compléter les villageois restants
    while (index < nbPlayers) roles[index++] = VILLAGEOIS;

    // Mélange des rôles
    for (int i = 0; i < nbPlayers; i++) {
        int r = rand() % nbPlayers;
        Role temp = roles[i];
        roles[i] = roles[r];
        roles[r] = temp;
    }

    //Attribution des rôles aux joueurs
    for (int i = 0; i < nbPlayers; i++) {
        wait_for_space(); // Attend un appui sur ESPACE
        players[i].role = roles[i];
        printf("%s est %s\n", players[i].name, roleToString(players[i].role)); //Affichage du rôle
        wait_for_space(); // Attend un appui sur ESPACE
    }
}

const char* roleToString(Role role) {
    switch (role) {
        case VILLAGEOIS: return "Villageois";
        case SORCIERE: return "Sorcière";
        case PETITE_FILLE: return "Petite Fille";
        case CHASSEUR: return "Chasseur";
        case LOUP: return "Loup-Garou";
        case CUPIDON: return "Cupidon";
        default: return "Inconnu";
    }
}
