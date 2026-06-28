#include "game.h"

/* --- bfs --- */

static uint8_t  s_visited[GRID_ROWS][GRID_COLS];
static uint16_t s_queue[GRID_TOTAL];

static void bfs_visit(int r, int c, uint8_t owner_id, int *tail) 
{
    if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) return;
    if (s_visited[r][c]) return;
    if (CELL_OWNER(g_grid[r][c]) == owner_id) return;
    s_visited[r][c] = 1;
    s_queue[(*tail)++] = (uint16_t)(r * GRID_COLS + c);
}

void area_fill(uint8_t owner_id) 
{
    memset(s_visited, 0, sizeof(s_visited));
    int head = 0, tail = 0;

    for (int c = 0; c < GRID_COLS; c++) {
        bfs_visit(0,           c, owner_id, &tail);
        bfs_visit(GRID_ROWS-1, c, owner_id, &tail);
    }
    for (int r = 1; r < GRID_ROWS-1; r++) {
        bfs_visit(r, 0,           owner_id, &tail);
        bfs_visit(r, GRID_COLS-1, owner_id, &tail);
    }

    while (head < tail) {
        int idx = s_queue[head++];
        int r = idx / GRID_COLS, c = idx % GRID_COLS;
        bfs_visit(r-1, c,   owner_id, &tail);
        bfs_visit(r+1, c,   owner_id, &tail);
        bfs_visit(r,   c-1, owner_id, &tail);
        bfs_visit(r,   c+1, owner_id, &tail);
    }

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            Cell cell = g_grid[r][c];
            uint8_t co = CELL_OWNER(cell);
            uint8_t ct = CELL_TYPE(cell);
            if (co == owner_id && ct == CTYPE_TRAIL) {
                g_grid[r][c] = CELL_ENCODE(owner_id, CTYPE_AREA);
                entity_adjust_area(owner_id, +1);
            } else if (!s_visited[r][c] && co != owner_id) {
                if (co != OWNER_NONE && ct == CTYPE_AREA)
                    entity_adjust_area(co, -1);
                g_grid[r][c] = CELL_ENCODE(owner_id, CTYPE_AREA);
                entity_adjust_area(owner_id, +1);
            }
        }
    }
}