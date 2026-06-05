#include <../bin/miniShell.h>
#include <../bin/idle.h>
#include <n7OS/keyboard.h>
#include <n7OS/processus.h>
#include <string.h>
#include <n7OS/sys.h>
#include <n7OS/syscall_defs.h>
#include <unistd.h>

char history[10][MAX_COMMAND_LENGTH];//historique des commandes
int history_index = 0;

void get_input_line(char* buffer) {
    int index = 0;

    buffer[0] = '\0';

    while (1) {
        char c = kgetch();
        if (c == -1) {       // pas de touche disponible
            continue;
        }
        if (c == '\n' || c == KEY_RETURN) {
            buffer[index] = '\0';
            if (index > 0) {
                if (history_index < 10) {
                    // Si l'historique n'est pas plein, on copie à la suite
                    strcpy(history[history_index], buffer);
                    history_index++;
                } else {
                    // Si l'historique est plein, on décale tout vers le haut (on oublie la plus ancienne)
                    for (int i = 1; i < 10; i++) {
                        strcpy(history[i-1], history[i]);
                    }
                    // On place la nouvelle commande à la fin
                    strcpy(history[9], buffer);
                }
            }
            printf("\n");
            break;
        }
        if (c == KEY_BACKSPACE) {
            if (index > 0) {
                index--;
                buffer[index] = '\0';
                printf("\b \b");
            }
            continue;
        }
        buffer[index] = c;
        index++;
        printf("%c", c);
    }
}

void execute_command(char* cmd){
    if (strcmp(cmd, "ps") == 0){
        ps();
    }else if (strcmp(cmd, "clear") == 0){
        printf("\f");
    } else if (strcmp(cmd, "help") == 0) {
        printf(".__________________________________________________________________.\n");
        printf("| Commande dispos | Description                                    |\n");
        printf("|-----------------+------------------------------------------------|\n");
        printf("| help            | Afficher cet ecran d'aide                      |\n");
        printf("| ps              | Afficher la liste des processus                |\n");
        printf("| fork <nom>      | Creer un nouveau processus avec le nom specifie|\n");
        printf("|                 | (parmi les processus disponibles (voir ps))    |\n");
        printf("| kill <pid>      | Tuer le processus ayant le PID indique         |\n");
        printf("| sleep <duree>   | Mettre le processus en sommeil (en secondes)   |\n");
        printf("| echo <message>  | Afficher un message                            |\n");
        printf("| history         | Afficher l'historique des commandes            |\n");
        printf("| history -c      | Effacer l'historique des commandes             |\n");
        printf("| clear           | Effacer l'ecran                                |\n");
        printf("| exit | shutdown | Eteindre le systeme et quitter le minishell    |\n");
        printf("|_________________|________________________________________________|\n");
    }else if (strcmp(cmd, "shutdown") == 0 || strcmp(cmd, "exit") == 0){
        shutdown(1);
    }else if (strncmp(cmd, "kill ", 5) == 0) {
        int pid = atoi(cmd + 5);
        kill(pid);
    }else if (strncmp(cmd, "fork ", 5) == 0) {
        char name[MAX_COMMAND_LENGTH];
        strncpy(name, cmd + 5, MAX_COMMAND_LENGTH);
        name[MAX_COMMAND_LENGTH - 1] = '\0'; // Assurer la terminaison de la chaîne
        fnptr fnct = process_exists(name);
        if (fnct != NULL  && fnct != miniShell) {
            fork(name, fnct);
            return;
        } else if (fnct == miniShell) {
            printf("Le miniShell est deja en cours d'execution.\n");
            return;
        } else {
            printf("Processus avec le nom '%s' n'existe pas encore.\n", name);
        }
    }else if (strncmp(cmd, "sleep ", 6) == 0) {
        int duration = atoi(cmd + 6);
        sleep(duration);
    }else if (strncmp(cmd, "history -c", 10) == 0){
        for (int i = 0; i < history_index; i++) {
            strcpy(history[i], "");
        }
        history_index = 0;
    }else if ((strncmp(cmd, "history", 7) == 0)) {
        for (int i = 0; i < history_index; i++) {
            printf("%d : %s\n", i, history[i]);
        }
    }else if (strncmp(cmd, "echo ", 5) == 0) {
        printf("%s\n", cmd + 5);
    }else{
        printf("Commande inconnue : %s\n", cmd);
    }
    
}

void miniShell(){
    char cmd_buffer[MAX_COMMAND_LENGTH];

    while (kgetch() != -1); // vider le buffer du clavier

    while(1){
        printf("Marouanux> ");
        while (kgetch() != -1);   // vider les touches en attente
        memset(cmd_buffer, 0, MAX_COMMAND_LENGTH);

        get_input_line(cmd_buffer);
        execute_command(cmd_buffer);
    }
}