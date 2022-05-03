#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <GL/glut.h>

#include "ants.h"

GLint FPS = 1000;
GLint window_width  = 900;
GLint window_height = 900;
GLfloat left   = 0.0;
GLfloat right  = 1.0;
GLfloat bottom = 0.0;
GLfloat top    = 1.0;
GLint game_width  = 200;
GLint game_height = 200;

Map *map;

int generations = 0;

void MyInit(){
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Reshape(int w, int h) {
	window_width  = w;
	window_height = h;

	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}

void Update(int value) {
	if(generations % 10000 == 0){
		printf("GENERATION %i\n", generations);
	}

	if(generations < 10000000){
		for (int i = 0; i < 1000; ++i){
			map_iterate(map);
		}

		generations += 1000;

		glutPostRedisplay();
		glutTimerFunc(1000 / FPS, Update, 0);
	}else {
		while(verify_end(map) == 0){
			map_last_iterate(map);
			glutPostRedisplay();
		}
	}
	
}

void RenderScene(){
	
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	GLfloat xSize = (right - left) / game_width;
	GLfloat ySize = (top - bottom) / game_height;

	glBegin(GL_QUADS);
		for (GLint x = 0; x < game_width; ++x) {
			for (GLint y = 0; y < game_height; ++y){
				glColor3f(1.0f, 1.0f, 1.0f);

				
				if(tem_formiga_morta(map, x, y)){
					glColor3f(0.5f, 0.5f, 0.5f);
				}

				glVertex2f(    x*xSize,    y*ySize);
				glVertex2f((x+1)*xSize,    y*ySize);
				glVertex2f((x+1)*xSize,(y+1)*ySize);
				glVertex2f(    x*xSize,(y+1)*ySize);

			}
		}
		
	glEnd();

	glFlush();
}

int main(int argc, char **argv){

	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);	
	glutCreateWindow("Ant Clustering");
	
	glutReshapeFunc(Reshape);
	glutDisplayFunc(RenderScene);
	
	srand(time(NULL));

	map = map_init();
	map->width  = game_width;
	map->height = game_height;
	map->n_alive_ants = 100;
	map->n_dead_ants = 10000;

	create_ants(map);

	MyInit();
	
	Update(0);

	glutMainLoop();
	return 0;
}

/* Insere uma formiga em determinada posição
 * na grid de formigas mortas
 */
void set_dead_ants_grid(Map *map, Ant *ant, int x, int y){
	map->dead_ants_grid[map->width * y + x] = ant;
}

/* Remove uma formiga em determinada posição
 * na grid de formigas mortas
 */
void rem_dead_ants_grid(Map *map, int x, int y){
	map->dead_ants_grid[map->width * y + x] = NULL;
}

/* Recupera uma formiga em determinada posição
 * na grid de formigas mortas
 */
Ant* get_dead_ants_grid(Map *map, int x, int y){
	return map->dead_ants_grid[map->width * y + x];
}

/* Insere uma formiga em determinada posição
 * na grid de formigas vivas
 */
void set_alive_ants_grid(Map *map, Ant *ant, int x, int y){
	map->alive_ants_grid[map->width * y + x] = ant;
}

/* Remove uma formiga em determinada posição
 * na grid de formigas vivas
 */
void rem_alive_ants_grid(Map *map, int x, int y){
	map->alive_ants_grid[map->width * y + x] = NULL;
}

/* Recupera uma formiga em determinada posição
 * na grid de formigas vivas
 */
Ant* get_alive_ants_grid(Map *map, int x, int y){
	return map->alive_ants_grid[map->width * y + x];
}


int tem_formiga_morta(Map *map, int x, int y){
	if(get_dead_ants_grid(map, x, y) == NULL){
		return 0;
	}else {
		return 1;
	}
}

int tem_formiga_viva(Map *map, int x, int y){
	if(get_alive_ants_grid(map, x, y) == NULL){
		return 0;
	}else {
		return 1;
	}
}

int is_valid_coordinates(Map *map, int x, int y){
	if(
		x >= 0 &&
		y >= 0 &&
		x < map->width &&
		y < map->height
	){
		return 1;
	}

	return 0;
}

int counts_dead_neighbor_ants(Map *map, Ant ant, int radius){
	int n_neighbor = -1;
	
	for(int i = ant.x - radius; i  <=  ant.x + radius; i++){
		for(int j = ant.y - radius; j  <=  ant.y + radius; j++){
			if(is_valid_coordinates(map, i, j) && tem_formiga_morta(map, i, j)){
				n_neighbor++;
			}
		}
	}

	if(n_neighbor == -1){
		n_neighbor = 0;
	}
	
	return n_neighbor;
}


Map* map_init(){
	return malloc(sizeof(Map));
}

Ant* ant_init(){
	return malloc(sizeof(Ant));
}

void create_ants(Map *map){
	map->alive_ants = malloc(sizeof(Ant) * map->n_alive_ants);
	map->dead_ants  = malloc(sizeof(Ant) * map->n_dead_ants);

	map->alive_ants_grid = malloc(sizeof(Ant*) * (map->width * map->height));
	map->dead_ants_grid  = malloc(sizeof(Ant*) * (map->width * map->height));


	for(int i = 0; i < map->width * map->height; i++){
		map->alive_ants_grid[i] = NULL;
		map->dead_ants_grid[i]  = NULL;
	}

	int coor[map->width * map->height][2];
	int size = 0;

	for(int i = 0; i < map->width; i++){
		for(int j = 0; j < map->height; j++){
			coor[size][0] = i;
			coor[size][1] = j;

			size++;
		}
	}

	for (int i = 0; i < map->width * map->height; ++i){
		int index = rand() % (map->width * map->height);
		
		int temp_x = coor[i][0];
		int temp_y = coor[i][1];


		coor[i][0] = coor[index][0];
		coor[i][1] = coor[index][1];

		coor[index][0] = temp_x;
		coor[index][1] = temp_y;
	}

	int pos = 0;
	
	for (int i = 0; i < map->n_alive_ants; ++i){
		map->alive_ants[i].state = NOT_CARRING;

		map->alive_ants[i].x = coor[pos][0];
		map->alive_ants[i].y = coor[pos][1];

		set_alive_ants_grid(map, &map->alive_ants[i], coor[pos][0], coor[pos][1]);

		pos++;
	}

	for (int i = 0; i < map->n_dead_ants; ++i){
		map->dead_ants[i].state = NOT_CARRING;

		map->dead_ants[i].x = coor[pos][0];
		map->dead_ants[i].y = coor[pos][1];

		set_dead_ants_grid(map, &map->alive_ants[i], coor[pos][0], coor[pos][1]);

		pos++;
	}
}

void move_ant(Map *map, Ant *ant){
	int neighbor_coor[8][2] = {
		{ant->x-1, ant->y-1},
		{ant->x-1, ant->y},
		{ant->x-1, ant->y+1},
		{ant->x, ant->y-1},
		{ant->x, ant->y+1},
		{ant->x+1, ant->y-1},
		{ant->x+1, ant->y},
		{ant->x+1, ant->y+1}
	};

	int direction = rand() % 8;

	for (int i = 0; i < 8; ++i){
		if(is_valid_coordinates(map, neighbor_coor[direction][0], neighbor_coor[direction][1])){
			if(!tem_formiga_viva(map, neighbor_coor[direction][0], neighbor_coor[direction][1])){

				rem_alive_ants_grid(map, ant->x, ant->y);
				set_alive_ants_grid(map, ant, neighbor_coor[direction][0], neighbor_coor[direction][1]);

				ant->x = neighbor_coor[direction][0];
				ant->y = neighbor_coor[direction][1];
				break;
			}
		}
	}
}

void map_iterate(Map *map){
	#pragma omp parallel for num_threads(6)
	for (int i = 0; i < map->n_alive_ants; ++i){
		int n_dead_neighbor_ants = counts_dead_neighbor_ants(map, map->alive_ants[i], 5);

		if(map->alive_ants[i].state == CARRING){
			if(!tem_formiga_morta(map, map->alive_ants[i].x, map->alive_ants[i].y)){

				int prob_jogar = pow((n_dead_neighbor_ants / 120.0), 2) * 100;
					
				if(prob_jogar == 0){
					prob_jogar = 1;
				}else if(prob_jogar == 100){
					prob_jogar = 99;
				}

				if(rand() % 101 <= prob_jogar){
						
					map->alive_ants[i].state = NOT_CARRING;

					set_dead_ants_grid(map, map->alive_ants[i].ant, map->alive_ants[i].x, map->alive_ants[i].y);
				}
					
			}

		}else {

			if(tem_formiga_morta(map, map->alive_ants[i].x, map->alive_ants[i].y)){			

				int prob_pegar = pow((120.0 - n_dead_neighbor_ants) / 120.0, 2) * 100;

				if(prob_pegar == 0){
					prob_pegar = 1;
				}else if(prob_pegar == 100){
					prob_pegar = 99;
				}

				if(rand() % 101 <= prob_pegar){
					
					map->alive_ants[i].state = CARRING;
					map->alive_ants[i].ant = get_dead_ants_grid(map, map->alive_ants[i].x, map->alive_ants[i].y);

					rem_dead_ants_grid(map, map->alive_ants[i].x, map->alive_ants[i].y);
				}
			}
		}

		#pragma omp critical
		move_ant(map, &map->alive_ants[i]);
	}
}

int verify_end(Map *map){
	for (int i = 0; i < map->n_alive_ants; ++i){
		if(map->alive_ants[i].state == CARRING){
			return 0;		
		}	
	}
	
	return 1;
}

void map_last_iterate(Map *map){
	#pragma omp parallel for num_threads(6)
	for (int i = 0; i < map->n_alive_ants; ++i){

		if(map->alive_ants[i].state == CARRING){
			if(!tem_formiga_morta(map, map->alive_ants[i].x, map->alive_ants[i].y)){

				map->alive_ants[i].state = NOT_CARRING;

				set_dead_ants_grid(map, map->alive_ants[i].ant, map->alive_ants[i].x, map->alive_ants[i].y);
			}

		}

		#pragma omp critical
		move_ant(map, &map->alive_ants[i]);
	}
}
