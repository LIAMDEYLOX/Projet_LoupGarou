#include <stdio.h>
#include <stdbool.h>



typedef struct {
    char name[50];      // Nom du joueur
    char role[20];      // Rôle (ex : "Loup-Garou", "Villageois", "Voyante")
    bool is_alive;      // Statut : vivant ou mort
} Player;



int main(void) {
    int nb_joueur = 0;
    printf("Nombre de joueurs : \n");
    scanf("%d" , &nb_joueur);

}

