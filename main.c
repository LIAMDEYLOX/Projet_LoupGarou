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
    VOYANTE,
    CHASSEUR,
    LOUP,
    CUPIDON,
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

// Assigne les rôles aux joueurs de manière aléatoire (Villageois, Loups, etc.).
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void assign_roles(Player players[], int nbPlayers);

// Affiche la liste des joueurs encore en vie.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
//   - excludeRole : Rôle à exclure de l'affichage (ex : les Loups durant la nuit).
void display_alive_players(Player players[], int nbPlayers, Role excludeRole);

// Compte le nombre de joueurs encore en vie.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
// Retourne : Le nombre de joueurs vivants.
int count_alive_players(Player players[], int nbPlayers);

// Compte le nombre de joueurs vivants ayant un rôle spécifique.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
//   - r : Rôle à rechercher parmi les joueurs vivants.
// Retourne : Le nombre de joueurs vivants ayant le rôle spécifié.
int count_alive_role(Player players[], int nbPlayers, Role r);


// Vérifie si une condition de victoire est atteinte (Villageois ou Loups-Garous gagnants).
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
// Retourne : 1 si une équipe a gagné, sinon 0.
int check_win_condition(Player players[], int nbPlayers);

// Permet au Chasseur d'utiliser sa capacité spéciale pour éliminer un joueur après sa mort.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
//   - hunterIndex : Indice du Chasseur dans le tableau.
void hunter_revenge(Player players[], int nbPlayers, int hunterIndex);

// Gère l'action de Cupidon pour désigner deux joueurs amoureux au début de la partie.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void cupidon_action(Player players[], int nbPlayers);

// Vérifie si un amoureux est mort, et tue l'autre joueur par chagrin si nécessaire.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void check_lovers_death(Player players[], int nbPlayers);


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

    cupidon_action(players, nbPlayers);

    free(players);
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


void wait_for_enter() {
    printf("Appuie sur [Entrer] pour continuer...\n");
    while (_getch() != 13) {
        // Ne fait rien tant que la touche pressée n'est pas entrer
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
    roles[index++] = VOYANTE;

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
    wait_for_enter(); // Attend un appui sur enter
    for (int i = 0; i < nbPlayers; i++) {
        players[i].role = roles[i];
        printf("%s est %s\n", players[i].name, roleToString(players[i].role)); //Affichage du rôle
        wait_for_enter();
    }
}



int check_win_condition(Player players[], int nbPlayers) {
    int nbLoups = count_alive_role(players, nbPlayers, LOUP);
    int nbVivs = count_alive_players(players, nbPlayers);
    int nbVillage = nbVivs - nbLoups;

    if (nbLoups == 0) {
        // Les villageois gagnent
        return 1;
    }
    if (nbLoups >= nbVillage) {
        // Les loups gagnent
        return 1;
    }
    return 0;
}

void display_alive_players(Player players[], int nbPlayers, Role excludeRole) {
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive && (excludeRole == -1 || players[i].role != excludeRole)) {
            printf("%d. %s\n", i, players[i].name);
        }
    }
}

int count_alive_players(Player players[], int nbPlayers) {
    int count = 0;
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive) count++;
    }
    return count;
}

int count_alive_role(Player players[], int nbPlayers, Role r) {
    int count = 0;
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive && players[i].role == r) count++;
    }
    return count;
}

void hunter_revenge(Player players[], int nbPlayers, int hunterIndex) {
    printf("Le chasseur %s est mort ! Il peut tuer une personne.\n", players[hunterIndex].name);
    display_alive_players(players, nbPlayers, CHASSEUR);
    int index;
    scanf("%d", &index);
    if (index >=0 && index < nbPlayers && players[index].alive && index != hunterIndex) {
        players[index].alive = 0;
        printf("%s est tué par le chasseur.\n", players[index].name);
    } else {
        printf("Choix invalide, le chasseur part sans tuer.\n");
    }
}

void cupidon_action(Player players[], int nbPlayers) {
    int first, second;
    printf("\n--- Cupidon choisit deux amoureux ---\n");
    display_alive_players(players, nbPlayers, CUPIDON);

    printf("Premier amoureux (index) : ");
    scanf("%d", &first);
    printf("Deuxième amoureux (index) : ");
    scanf("%d", &second);

    //Actualise les états des joueurs
    players[first].lover = second;
    players[second].lover = first;
    printf("%s et %s sont amoureux.\n", players[first].name, players[second].name);
    while (getchar() != '\n'); // Nettoyage buffer
}

void check_lovers_death(Player players[], int nbPlayers) {
    // Si l'un des amoureux meurt, l'autre meurt aussitôt
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive == 0 && players[i].lover != -1) {
            int loverIndex = players[i].lover;
            if (players[loverIndex].alive == 1) {
                printf("L'amoureux de %s, %s, meurt de chagrin.\n", players[i].name, players[loverIndex].name);
                players[loverIndex].alive = 0;
                // Si le chasseur est amoureux et qu'il meurt, il tire sur quelqu'un
                if (players[loverIndex].role == CHASSEUR) {
                    hunter_revenge(players, nbPlayers, loverIndex);
                }
            }
        }
    }
}


const char* roleToString(Role role) {
    switch (role) {
        case VILLAGEOIS: return "Villageois";
        case SORCIERE: return "Sorciere";
        case VOYANTE: return "VOYANTE";
        case CHASSEUR: return "Chasseur";
        case LOUP: return "Loup-Garou";
        case CUPIDON: return "Cupidon";
        default: return "Inconnu";
    }
}