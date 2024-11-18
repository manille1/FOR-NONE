#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY 50000 // Vitesse d'exécution en microsecondes
#define MAX_BULLETS 5
#define ENEMY_ROWS 5
#define ENEMY_COLS 10
#define ENEMY_MOVE_DELAY 30 // Délais en frames pour le mouvement des ennemis

typedef struct {
    int x, y;
    int active;
} Bullet;

typedef struct {
    int x, y;
    int alive;
} Enemy;

void draw_player(int x, int y);
void draw_bullets(Bullet bullets[], int max_bullets);
void draw_enemies(Enemy enemies[][ENEMY_COLS], int rows, int cols);
void move_bullets(Bullet bullets[], int max_bullets);
void move_enemies(Enemy enemies[][ENEMY_COLS], int rows, int cols, int *direction, int *game_over);
void check_collision(Bullet bullets[], int max_bullets, Enemy enemies[][ENEMY_COLS], int rows, int cols, int *score);

int main() {
    int max_x, max_y, player_x, player_y, score = 0, game_over = 0;
    int enemy_direction = 1; // 1 = droite, -1 = gauche
    int frame_count = 0;
    int ch;

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Joueur
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Ennemis
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Balles

    // Récupération des dimensions de l'écran
    getmaxyx(stdscr, max_y, max_x);
    player_x = max_x / 2;
    player_y = max_y - 2;

    // Initialisation des balles
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }

    // Initialisation des ennemis
    Enemy enemies[ENEMY_ROWS][ENEMY_COLS];
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            enemies[i][j].x = j * 4 + 2;
            enemies[i][j].y = i * 2 + 1;
            enemies[i][j].alive = 1;
        }
    }

    while (!game_over) {
        clear();

        // Afficher le score
        mvprintw(0, 0, "Score: %d", score);

        // Dessiner le joueur
        draw_player(player_x, player_y);

        // Dessiner les balles
        draw_bullets(bullets, MAX_BULLETS);

        // Dessiner les ennemis
        draw_enemies(enemies, ENEMY_ROWS, ENEMY_COLS);

        // Gérer les entrées utilisateur
        ch = getch();
        if (ch == KEY_LEFT && player_x > 0) {
            player_x--;
        } else if (ch == KEY_RIGHT && player_x < max_x - 1) {
            player_x++;
        } else if (ch == ' ') { // Tirer une balle
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    bullets[i].x = player_x;
                    bullets[i].y = player_y - 1;
                    bullets[i].active = 1;
                    break;
                }
            }
        } else if (ch == 'q') { // Quitter le jeu
            break;
        }

        // Déplacer les balles
        move_bullets(bullets, MAX_BULLETS);

        // Vérifier les collisions
        check_collision(bullets, MAX_BULLETS, enemies, ENEMY_ROWS, ENEMY_COLS, &score);

        // Déplacer les ennemis (toutes les ENEMY_MOVE_DELAY frames)
        if (frame_count % ENEMY_MOVE_DELAY == 0) {
            move_enemies(enemies, ENEMY_ROWS, ENEMY_COLS, &enemy_direction, &game_over);
        }

        // Pause pour ralentir le jeu
        usleep(DELAY);
        frame_count++;
    }

    clear();
    mvprintw(max_y / 2, max_x / 2 - 5, "GAME OVER!");
    mvprintw(max_y / 2 + 1, max_x / 2 - 7, "Final Score: %d", score);
    refresh();
    usleep(3000000);

    endwin();
    return 0;
}

void draw_player(int x, int y) {
    attron(COLOR_PAIR(1));
    mvprintw(y, x, "^");
    attroff(COLOR_PAIR(1));
}

void draw_bullets(Bullet bullets[], int max_bullets) {
    attron(COLOR_PAIR(3));
    for (int i = 0; i < max_bullets; i++) {
        if (bullets[i].active) {
            mvprintw(bullets[i].y, bullets[i].x, "|");
        }
    }
    attroff(COLOR_PAIR(3));
}

void draw_enemies(Enemy enemies[][ENEMY_COLS], int rows, int cols) {
    attron(COLOR_PAIR(2));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (enemies[i][j].alive) {
                mvprintw(enemies[i][j].y, enemies[i][j].x, "M");
            }
        }
    }
    attroff(COLOR_PAIR(2));
}

void move_bullets(Bullet bullets[], int max_bullets) {
    for (int i = 0; i < max_bullets; i++) {
        if (bullets[i].active) {
            bullets[i].y--;
            if (bullets[i].y < 0) {
                bullets[i].active = 0;
            }
        }
    }
}

void move_enemies(Enemy enemies[][ENEMY_COLS], int rows, int cols, int *direction, int *game_over) {
    int shift_down = 0;

    // Vérifier les limites pour changer de direction
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (enemies[i][j].alive) {
                if ((enemies[i][j].x <= 0 && *direction == -1) || (enemies[i][j].x >= COLS - 1 && *direction == 1)) {
                    *direction *= -1;
                    shift_down = 1;
                }
            }
        }
    }

    // Déplacer les ennemis
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (enemies[i][j].alive) {
                enemies[i][j].x += *direction;
                if (shift_down) {
                    enemies[i][j].y++;
                    if (enemies[i][j].y >= LINES - 1) {
                        *game_over = 1;
                    }
                }
            }
        }
    }
}

void check_collision(Bullet bullets[], int max_bullets, Enemy enemies[][ENEMY_COLS], int rows, int cols, int *score) {
    for (int i = 0; i < max_bullets; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < rows; j++) {
                for (int k = 0; k < cols; k++) {
                    if (enemies[j][k].alive && bullets[i].x == enemies[j][k].x && bullets[i].y == enemies[j][k].y) {
                        enemies[j][k].alive = 0;
                        bullets[i].active = 0;
                        (*score)++;
                        break;
                    }
                }
            }
        }
    }
}
