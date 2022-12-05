#include "extensions.h"
#include "options.h"

//////////////////////////////////////////////////////////////////////
// For sorting colors

int color_features_compare(const void* vptr_a, const void* vptr_b) {

	const color_features_t* a = (const color_features_t*)vptr_a;
	const color_features_t* b = (const color_features_t*)vptr_b;

	int u = cmp(a->user_index, b->user_index);
	if (u) { return u; }

	int w = cmp(a->wall_dist[0], b->wall_dist[0]);
	if (w) { return w; }

	int g = -cmp(a->wall_dist[1], b->wall_dist[1]);
	if (g) { return g; }

	return -cmp(a->min_dist, b->min_dist);

}

//////////////////////////////////////////////////////////////////////
// Place the game colors into a set order

void game_order_colors(game_info_t* info,
                       game_state_t* state) {

	if (g_options.order_random) {
    
		srand(now() * 1e6);
    
		for (size_t i=info->num_colors-1; i>0; --i) {
			size_t j = rand() % (i+1);
			int tmp = info->color_order[i];
			info->color_order[i] = info->color_order[j];
			info->color_order[j] = tmp;
		}

	} else { // not random

		color_features_t cf[MAX_COLORS];
		memset(cf, 0, sizeof(cf));

		for (size_t color=0; color<info->num_colors; ++color) {
			cf[color].index = color;
			cf[color].user_index = MAX_COLORS;
		}
    

		for (size_t color=0; color<info->num_colors; ++color) {
			
			int x[2], y[2];
			
			for (int i=0; i<2; ++i) {
				pos_get_coords(state->pos[color], x+i, y+i);
				cf[color].wall_dist[i] = get_wall_dist(info, x[i], y[i]);
			}

			int dx = abs(x[1]-x[0]);
			int dy = abs(y[1]-y[0]);
			
			cf[color].min_dist = dx + dy;
			
		

		}


		qsort(cf, info->num_colors, sizeof(color_features_t),
		      color_features_compare);

		for (size_t i=0; i<info->num_colors; ++i) {
			info->color_order[i] = cf[i].index;
		}
    
	}

	if (!g_options.display_quiet) {

		printf("\n************************************************"
		       "\n*               Branching Order                *\n");
		if (g_options.order_most_constrained) {
			printf("* Will choose color by most constrained\n");
		} else {
			printf("* Will choose colors in order: ");
			for (size_t i=0; i<info->num_colors; ++i) {
				int color = info->color_order[i];
				printf("%s", color_name_str(info, color));
			}
			printf("\n");
		}
		printf ("*************************************************\n\n");

	}

}



//////////////////////////////////////////////////////////////////////
// Check for dead-end regions of freespace where there is no way to
// put an active path into and out of it. Any freespace node which
// has only one free neighbor represents such a dead end. For the
// purposes of this check, cur and goal positions count as "free".

int game_check_deadends(const game_info_t* info,
                        const game_state_t* state) {


	/**
	 * FILL CODE TO DETECT DEAD-ENDS
	 */

	int x_coor, y_coor;
	pos_t curr = state->pos[state->last_color];
	pos_get_coords(curr, &x_coor, &y_coor);
	int x, y;

	for (int i = 0; i < MAX_DIRECTION; i++){
		// check if new position is valid or not
		pos_t new_pos = offset_pos(info, x_coor, y_coor, i); 
		
		if (new_pos != INVALID_POS && !state->cells[new_pos] && (deadend_detect(info, state, new_pos) <= 1)){
			return 1;
		}else{
			pos_get_coords(new_pos, &x, &y);
			for (int j = 0; j < MAX_DIRECTION; j++){
				pos_t neighbor = offset_pos(info, x, y, j);

				if (neighbor != INVALID_POS && !state->cells[neighbor] && (deadend_detect(info, state, neighbor) <= 1)){
					return 1;
				}
			}
		}
	}	 
	
	return 0;

}


/**
 * A cell is free if it is not painted with a path colour,
 * is unopcuppied, a goal state, is an initial state and
 * is a valid board position
 * @return int 
 * inspire by game_num_free_pos & game_num_free_coords 
 */
int deadend_detect(const game_info_t* info, const game_state_t* state, pos_t pos) {

	int x, y;
	pos_get_coords(pos, &x, &y);

	int free_cell_count = 0;

	for (int direction = 0; direction < MAX_DIRECTION; direction++){

		pos_t neighbor_pos = offset_pos(info, x, y, direction);

		// Check valid postion on board, and unoccupied is treated as free space
		if (check_free_cells(info, state, neighbor_pos)){
			free_cell_count++;
		}else{
			// The current position of an in-progress pipe and goal position is treated as free space
			for (int color = 0; color < info->num_colors; color++){
				if (!(state->completed & (1 << color)) && (neighbor_pos == info->goal_pos[color] || neighbor_pos == state->pos[color])){
					free_cell_count++;
				}
			}
		}

	}

	return free_cell_count;
	
}

int check_free_cells(const game_info_t *info, const game_state_t *state, pos_t pos){

	if (pos != INVALID_POS && state->cells[pos] == 0){
		return 1;
	}

	return 0;
	
}
