#define NOT_CARRING 0;
#define CARRING 1

typedef struct Ant Ant;
typedef struct Map Map;

struct Ant{
	int state;

	Ant *ant;

	int x, y;
	int x_old, y_old;
};

struct Map{
	int width;
	int height;

	int n_alive_ants;
	int n_dead_ants;

	Ant *alive_ants;
	Ant *dead_ants;

	Ant **alive_ants_grid;
	Ant **dead_ants_grid;
};


void set_dead_ants_grid(Map *map, Ant *ant, int x, int y);
void rem_dead_ants_grid(Map *map, int x, int y);
Ant* get_dead_ants_grid(Map *map, int x, int y);

void set_alive_ants_grid(Map *map, Ant *ant, int x, int y);
void rem_alive_ants_grid(Map *map, int x, int y);
Ant* get_alive_ants_grid(Map *map, int x, int y);

int tem_formiga_morta(Map *map, int x, int y);
int tem_formiga_viva(Map *map, int x, int y);
int is_valid_coordinates(Map *map, int x, int y);
int counts_dead_neighbor_ants(Map *map, Ant ant, int radius);

Map* map_init();
Ant* ant_init();
void create_ants(Map *map);
void move_ant(Map *map, Ant *ant);
void map_iterate(Map* map);