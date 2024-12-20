#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Pour sleep()
#define SLEEP(x) sleep(x)

typedef enum {
    VILLAGEOIS,
    SORCIERE,
    VOYANTE,
    CHASSEUR,
    LOUP,
    CUPIDON,
    LOUP_BLANC,
    SALVATEUR,
} Role;

typedef struct {
    char name[50];
    Role role;
    int alive; // 1 en vie, 0 mort
    int votes;
    int lover; // stocke l'indice de l'amoureux (-1 si pas amoureux)
    int is_protected; // 0 pour pas protégé, 1 est protégé, 2 était protégé au tour d'avant
} Player;

// Prototypes:

// Demande à l'utilisateur le nombre de joueurs et s'assure qu'il est valide (>= 6).
// Retourne : Le nombre de joueurs sélectionnés.
int select_number_of_players();

//Demande à l'utilisateur d'appuyer sur entrée afin de finir une étape
void wait_for_enter();

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

// Gère la phase de nuit où les Loups attaquent, la Voyante découvre un rôle,
// et la Sorcière peut utiliser ses potions.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
//   - potionVie : Pointeur vers la potion de vie de la Sorcière (utilisation unique).
//   - potionMort : Pointeur vers la potion de mort de la Sorcière (utilisation unique).
void night_phase(Player players[], int nbPlayers, int *potionVie, int *potionMort);

// Gère la phase de vote pendant le jour pour éliminer un joueur.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void day_vote(Player players[], int nbPlayers);

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

// Permet à la Voyante de découvrir le rôle d'un joueur pendant la nuit.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
void voyante_action(Player players[], int nbPlayers);

// Permet à la Sorcière d'utiliser ses potions pendant la nuit : sauver ou tuer un joueur.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
//   - victimIndex : Indice de la victime des loups.
//   - potionVie : Pointeur vers la potion de vie (1 si disponible, 0 sinon).
//   - potionMort : Pointeur vers la potion de mort (1 si disponible, 0 sinon).
void sorciere_action(Player players[], int nbPlayers, int victimIndex, int *potionVie, int *potionMort);

// Permet aux Loups-Garous de choisir une victime pendant la nuit.
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
// Retourne : L'indice du joueur choisi comme victime.
int loups_action(Player players[], int nbPlayers);


// Retourne une chaîne de caractères représentant le nom d'un rôle.
// Paramètres :
//   - role : Rôle à convertir en chaîne.
// Retourne : Une chaîne constante contenant le nom du rôle (ex : "Villageois", "Loup-Blanc").
int loup_blanc_action(Player players[], int nbPlayers, int nuit);


// Permet au Salvateur de choisir une personne à protéger
// Paramètres :
//   - players : Tableau contenant les joueurs.
//   - nbPlayers : Nombre total de joueurs.
// Retourne : L'indice du joueur choisi.
int Salvateur_action(Player players[], int nbPlayers);

// Retourne une chaîne de caractères représentant le nom d'un rôle.
// Paramètres :
//   - role : Rôle à convertir en chaîne.
// Retourne : Une chaîne constante contenant le nom du rôle (ex : "Villageois", "Loup-Garou").
char* roleToString(Role role);


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
        players[i].is_protected = 0;
    }

    printf("\n--- Distribution des roles en cours ---\n");
    SLEEP(1);
    assign_roles(players, nbPlayers);

    cupidon_action(players, nbPlayers);

    // Début de la boucle de jeu
    while (!check_win_condition(players, nbPlayers)) {
        night_phase(players, nbPlayers, &potionVie, &potionMort);
        check_lovers_death(players, nbPlayers);
        if (check_win_condition(players, nbPlayers)) break;

        day_vote(players, nbPlayers);
        check_lovers_death(players, nbPlayers);
    }

    int nbLoups = count_alive_role(players, nbPlayers, LOUP);
    int nbLoupBlanc = count_alive_role(players, nbPlayers, LOUP_BLANC);
    int nbVivs = count_alive_players(players, nbPlayers);
    int nbVillage = count_alive_role(players, nbPlayers, VILLAGEOIS) +
                count_alive_role(players, nbPlayers, SORCIERE) +
                count_alive_role(players, nbPlayers, VOYANTE) +
                count_alive_role(players, nbPlayers, CHASSEUR) +
                count_alive_role(players, nbPlayers, CUPIDON);

    printf("\n--- Fin de la partie ---\n");
    if (nbLoups == 0 && nbLoupBlanc ==0 && nbVillage >= 1) {
        printf("Les Villageois ont gagne !\n");
    } else if (nbVillage == 0 && nbLoupBlanc ==0 && nbLoups >= 1) {
        printf("Les Loups-Garous ont gagne !\n");
    } else if (nbVillage == 0 && nbLoups == 0 && nbLoupBlanc >= 1) {
        printf("Les Loups-Blanc ont gagne !\n");
    }

    free(players);
    return 0;
}


int select_number_of_players() {
    int n;
    do {
        printf("Combien de joueurs ? (minimum 6) : ");
        if (scanf("%d", &n) != 1 || n < 6) {
            printf("Entree invalide. Veuillez entrer un nombre >= 6.\n");
            while (getchar() != '\n'); //Nettoyer le buffer
        }
    } while (n < 6);
    while (getchar() != '\n'); //Nettoyer le buffer
    return n;
}


void wait_for_enter() {
    printf("Appuyez sur Entree pour continuer...\n");
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
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

    if (nbLoups > 2) { // Condition pour ajouter le Loup Blanc
        roles[index++] = LOUP_BLANC;
    }

    // Ajout du Salvateur à la composition à partir de 8 joueur
    if (nbPlayers >= 8) {
        roles[index++] = SALVATEUR;
    }
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

void night_phase(Player players[], int nbPlayers, int *potionVie, int *potionMort) {
    static int nuit = 0; // Compteur pour suivre les nuits
    nuit++;
    printf("\n--- Phase de Nuit ---\n");

    // 1. Action de la Voyante
    voyante_action(players, nbPlayers);

    // 2. Action du Salvateur
    int protectIndex = Salvateur_action(players, nbPlayers);
    players[protectIndex].is_protected = 1;

    // 3. Action des Loups-Garous
    int victimIndex = loups_action(players, nbPlayers);
    players[victimIndex].alive = 0;

    // 4. Action de la Sorcière
    sorciere_action(players, nbPlayers, victimIndex, potionVie, potionMort);

    // 5. Action du Loup Blanc
    loup_blanc_action(players, nbPlayers, nuit);
}


void day_vote(Player players[], int nbPlayers) {
    printf("\n--- Phase de Jour : Vote pour eliminer un joueur ---\n");

    // Réinitialiser les votes
    for (int i = 0; i < nbPlayers; i++) {
        players[i].votes = 0;
    }

    // Chaque joueur vivant vote
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive) {
            printf("\n%s, choisissez un joueur a eliminer :\n", players[i].name);
            display_alive_players(players, nbPlayers, -1);
            int vote;
            do {
                printf("Entrez l'index du joueur : ");
                int res = scanf("%d", &vote);
                while (getchar() != '\n');
                if (res != 1) {
                    printf("Entree invalide, reessayez.\n");
                    // Retourner au début de la boucle qui demande un vote
                    continue;
                }
                while (getchar() != '\n'); // Nettoie le buffer
                if (vote < 0 || vote >= nbPlayers || !players[vote].alive || vote == i) {
                    printf("Choix invalide. Vous ne pouvez pas voter pour vous-même ou un joueur mort.\n");
                }
            } while (vote < 0 || vote >= nbPlayers || !players[vote].alive || vote == i);
            players[vote].votes++;
        }
    }

    // Trouver le joueur avec le plus de votes
    int maxVotes = 0;
    int eliminatedIndex = -1;
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive && players[i].votes > maxVotes) {
            maxVotes = players[i].votes;
            eliminatedIndex = i;
        }
    }

    if (eliminatedIndex != -1) {
        printf("\n%s a ete elimine par le village !\n", players[eliminatedIndex].name);
        printf("\n son role était %s !\n", players[eliminatedIndex].role);
        players[eliminatedIndex].alive = 0;

        // Si le joueur éliminé est un Chasseur, il tire avant de mourir
        if (players[eliminatedIndex].role == CHASSEUR) {
            hunter_revenge(players, nbPlayers, eliminatedIndex);
        }
    } else {
        printf("\nPersonne n'a ete elimine aujourd'hui.\n");
    }

    wait_for_enter();

    // Actualise le statut de la personne protégé
    for(int i = 0; i < nbPlayers; i++) {
        if (players[i].is_protected == 1 && players[i].alive) {
            players[i].is_protected = 2; // L'état 2 permet d'identifier au tour suivant si le joueur vient de recevoir la salvation.

        } else if (players[i].is_protected == 2 || players[i].alive) {
            players[i].is_protected = 0;
        }
    }
}

int check_win_condition(Player players[], int nbPlayers) {
    int nbLoups = count_alive_role(players, nbPlayers, LOUP);
    int nbLoupBlanc = count_alive_role(players, nbPlayers, LOUP_BLANC);
    int nbVivs = count_alive_players(players, nbPlayers);
    int nbVillage = nbVivs - nbLoups;

    // Si seul le Loup Blanc est vivant
    if (nbLoupBlanc >= 1 && nbLoupBlanc > nbLoups && nbLoupBlanc > nbVillage) {
        return 1;
    }
    if (nbLoups == 0 && nbLoupBlanc == 0) {
        // Les villageois gagnent
        return 1;
    }
    if (nbLoups >= nbVillage && nbLoups >= nbLoupBlanc) {
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
    display_alive_players(players, nbPlayers, -1);

    int index, validInput = 0;
    while (!validInput) {
        printf("Entrez l'indice du joueur a tuer : ");
        int res = scanf("%d", &index);
        while (getchar() != '\n');
        if (res == 1 && index >= 0 && index < nbPlayers && players[index].alive && index != hunterIndex) {
            validInput = 1;
        } else {
            printf("Choix invalide, reessayez.\n");
        }
    }

    players[index].alive = 0;
    printf("%s est tue par le chasseur.\n", players[index].name);
    wait_for_enter();
}

void cupidon_action(Player players[], int nbPlayers) {
    int first, second;
    printf("\n--- Cupidon choisit deux amoureux ---\n");
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].alive) {
            printf("%d. %s\n", i, players[i].name);
        }
    }

    printf("Premier amoureux (index) : ");
    while (scanf("%d", &first) != 1 || first < 0 || first >= nbPlayers || !players[first].alive) {
        printf("Index invalide. Réessayez : ");
        while (getchar() != '\n'); // Nettoyer le buffer
    }
    while (getchar() != '\n'); // Nettoyer le buffer

    printf("Deuxième amoureux (index) : ");
    while (scanf("%d", &second) != 1 || second < 0 || second >= nbPlayers || !players[second].alive || second == first) {
        printf("Index invalide. Réessayez : ");
        while (getchar() != '\n'); // Nettoyer le buffer
    }
    while (getchar() != '\n'); // Nettoyer le buffer

    players[first].lover = second;
    players[second].lover = first;
    printf("%s et %s sont amoureux.\n", players[first].name, players[second].name);
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

void voyante_action(Player players[], int nbPlayers) {
    printf("\n--- Action de la Voyante ---\n");

    // Rechercher si la Voyante est en vie
    int voyanteIndex = -1;
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].role == VOYANTE && players[i].alive) {
            voyanteIndex = i;
            break;
        }
    }

    // Si la Voyante n'est pas en vie, elle ne peut pas agir
    if (voyanteIndex == -1) {
        return;
    }

    printf("Voyante (%s), choisis un joueur pour connaitre son role :\n", players[voyanteIndex].name);
    display_alive_players(players, nbPlayers, -1); // Affiche les joueurs vivants

    int choice;
    do {
        printf("Entrez l index du joueur : ");
        int res = scanf("%d", &choice);
        while (getchar() != '\n');
        if (res != 1) {
            printf("Entree invalide, reessayez.\n");
        }
    } while (choice < 0 || choice >= nbPlayers || !players[choice].alive);

    printf("Le role de %s est : %s\n", players[choice].name, roleToString(players[choice].role));
    wait_for_enter();
}

void sorciere_action(Player players[], int nbPlayers, int victimIndex, int *potionVie, int *potionMort) {
    printf("\n--- Action de la Sorciere ---\n");
    int sorciereIndex = -1;
    for (int i = 0; i < nbPlayers; i++) {
        if (players[i].role == SORCIERE && players[i].alive) {
            sorciereIndex = i;
            break;
        }
    }

    if (sorciereIndex == -1) {
        // Sorcière morte
        return;
    }

    // Potion de vie
    if (*potionVie && !players[victimIndex].alive) {
        printf("La victime des loups est %s. Voulez-vous la sauver ? (1 = Oui, 0 = Non) : ", players[victimIndex].name);
        int choice;
        int res = scanf("%d", &choice);
        while (getchar() != '\n');
        if (res != 1) {
            printf("Entree invalide, la sorciere ne fait rien pour la potion de vie.\n");
        } else if (choice == 1) {
            players[victimIndex].alive = 1;
            *potionVie = 0;
            printf("Vous avez sauve %s.\n", players[victimIndex].name);
        }
    }

    // Potion de mort
    if (*potionMort) {
        printf("Voulez-vous utiliser la potion de mort pour tuer un joueur ? (1 = Oui, 0 = Non) : ");
        int choice;
        int res = scanf("%d", &choice);
        while (getchar() != '\n');
        if (res != 1) {
            printf("Entree invalide, la sorciere ne fait rien pour la potion de mort.\n");
        } else if (choice == 1) {
            printf("Choisissez un joueur a tuer :\n");
            display_alive_players(players, nbPlayers, sorciereIndex);

            int target;
            int valid = 0;
            while (!valid) {
                printf("Entrez l'index du joueur : ");
                int res2 = scanf("%d", &target);
                while (getchar() != '\n');
                if (res2 == 1 && target >= 0 && target < nbPlayers && players[target].alive && target != sorciereIndex) {
                    valid = 1;
                } else {
                    printf("Choix invalide, reessayez.\n");
                }
            }

            players[target].alive = 0;
            *potionMort = 0;
            printf("Vous avez tue %s avec la potion de mort.\n", players[target].name);
        }
    }

    printf("La Sorciere a termine son action.\n");
    wait_for_enter();
}

int loups_action(Player players[], int nbPlayers) {
    printf("\n--- Action des Loups-Garous ---\n");

    // Afficher les joueurs vivants (exclut les Loups)
    printf("Les Loups se reveillent et choisissent une cible parmi les joueurs vivants :\n");
    display_alive_players(players, nbPlayers, LOUP);

    int target;
    do {
        printf("Entrez l'index de la cible : ");
        int res = scanf("%d", &target);
        while (getchar() != '\n');
        if (res != 1) {
            printf("Entree invalide, reessayez.\n");
            continue; // retourner au début de la boucle de choix
        }
        if (target < 0 || target >= nbPlayers || !players[target].alive || players[target].role == LOUP) {
            printf("Choix invalide. Vous ne pouvez pas attaquer un Loup-Garou ou un joueur mort.\n");
        }
    } while (target < 0 || target >= nbPlayers || !players[target].alive || players[target].role == LOUP);

    printf("Les Loups ont choisi d'attaquer %s.\n", players[target].name);
    wait_for_enter();
    return target; // Retourne l'indice de la victime
}

int loup_blanc_action(Player players[], int nbPlayers, int nuit) {
    if (nuit % 2 == 0) { // Le Loup Blanc agit seulement une nuit sur deux
        printf("\n--- Action du Loup Blanc ---\n");

        // Rechercher le Loup Blanc
        int loupBlancIndex = -1;
        for (int i = 0; i < nbPlayers; i++) {
            if (players[i].role == LOUP_BLANC && players[i].alive) {
                loupBlancIndex = i;
                break;
            }
        }

        // Si le Loup Blanc est en vie, il choisit une victime parmi les loups
        if (loupBlancIndex != -1) {
            printf("%s (Loup Blanc), choisis un Loup-Garou a eliminer :\n", players[loupBlancIndex].name);
            for (int i = 0; i < nbPlayers; i++) {
                if (players[i].alive && players[i].role == LOUP && i != loupBlancIndex) {
                    printf("%d. %s\n", i, players[i].name);
                }
            }

            int choice = 0 ;
            do {
                printf("Entrez l'index du Loup-Garou a tuer : ");
                scanf("%d", &choice);
                while (getchar() != '\n'); // Nettoyage buffer
            } while (choice < 0 || choice >= nbPlayers || players[choice].role != LOUP || !players[choice].alive);

            players[choice].alive = 0;
            printf("%s a été tué par le Loup Blanc.\n", players[choice].name);
            wait_for_enter();
        }
    }
}

int Salvateur_action(Player players[], int nbPlayers) {
    printf("\n--- Action du Salvateur ----\n");

    printf("Le Salvateur choisit un joueur qu'il souhaite protéger:\n");
    display_alive_players(players, nbPlayers, SALVATEUR);

    int target;
    do {
        printf("Entrez l'index de la personne à protéger : ");
        int protect = scanf("%d", &target);

        if (protect != 1) {
            printf("Entree invalide, reessayez.\n");
            continue;
        }
        if (target < 0 || target >= nbPlayers || !players[target].alive || players[target].is_protected == 2) {
            printf("Choix invalide. Vous ne pouvez pas protéger un joueur mort ou deux fois la même personne.\n");
        }
    } while (target < 0 || target >= nbPlayers || !players[target].alive || players[target].is_protected == 2);

    printf("Le salvateur à chosit %s. \n", players[target].name);
    wait_for_enter;
    return target; // Retourne l'indice de la personne protéger
}


char *roleToString(Role role) {
    switch (role) {
        case VILLAGEOIS: return "Villageois";
        case SORCIERE: return "Sorciere";
        case VOYANTE: return "Voyante";
        case CHASSEUR: return "Chasseur";
        case LOUP: return "Loup-Garou";
        case CUPIDON: return "Cupidon";
        case LOUP_BLANC: return "Loup-Blanc";
        case SALVATEUR: return "Salvateur";
        default: return "Inconnu";
    }
}