 #ifndef __EXTENSIONS__
#define __EXTENSIONS__


#include "utils.h"
#include "node.h"

#define MAX_DIRECTION 4

//////////////////////////////////////////////////////////////////////
// Place the game colors into a set order
void game_order_colors(game_info_t* info, game_state_t* state);

//////////////////////////////////////////////////////////////////////
// Check for dead-end regions of freespace where there is no way to
// put an active path into and out of it. Any freespace node which
// has only one free neighbor represents such a dead end. For the
// purposes of this check, cur and goal positions count as "free".
int game_check_deadends(const game_info_t* info, const game_state_t* state);

// Count and return number of free cells exist surround the current cell
// If the number of free cells is less than or equal to 1, it is dead end
int deadend_detect(const game_info_t* info, const game_state_t* state, pos_t pos);

// Check if the cell is a valid position on board ie. in bound
// And has not been occupied 
int check_free_cells(const game_info_t *info, const game_state_t *state, pos_t pos);

#endif
