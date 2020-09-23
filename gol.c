#include<stdio.h>
#include"gol.h"
#include<string.h>

void read_in_file(FILE *infile, struct universe *u) {
   if(infile == NULL) {
		printf("Can't open file\n");
	}

  char character;
  int column_count = 0;
  while (EOF != fscanf(infile, "%c", &character)) {
    if (character == '\n')
      break;
    if (character != ' ') {
      column_count++;
    }
  }

  char line[2048];
  int row_count = 1;

  while (EOF != fscanf(infile, "%[^\n]\n", line)) {
      if (strlen(line) > 512) {
        fprintf(stderr, "%s", "Error: the grid has too many columns\n");
        exit(1);
      }
      if (column_count != (int)strlen(line)) {
        fprintf(stderr, "%s", "Error: the grid is not rectangular\n");
        exit(1);
      }
      row_count++;
  }

  //set dimensions for universe
  u->rows = row_count;
  u->columns = column_count;
  u->arr = (char *)malloc(u->rows * u->columns * sizeof(char));

  rewind(infile);

  //read the data into the universe
  int row_index = 0, column_index = 0;
  while (EOF != fscanf(infile, "%c", &character)) {
    if (character != ' ' && character != '\n') {
      if (character != '*' &&  character != '.') {
          fprintf(stderr, "%s", "Error: file contains unallowed character");
          exit(1);
      }
      else {
        *(u->arr + row_index*u->columns + column_index) = character;
        column_index++;
        if (column_index == u->columns) {
          column_index = 0;
          row_index++;
        }
      }
    }
  }

  u->dimension = u->rows * u->columns;
  u->gen_number = 1; 
  u->alive_overall = 0;

  for (int i = 0; i < u->rows; i++) 
    for (int j = 0; j < u->columns; j++) 
      if (is_alive(u, j, i))
        u->alive_overall++;

  fclose(infile);
}

void write_out_file(FILE *outfile, struct universe *u) {
  for (int i = 0; i <  u->rows; i++) {
    for (int j = 0; j < u->columns; j++) 
      fputc(*(u->arr + i*u->columns + j), outfile);
    fprintf(outfile, "\n");
  }
}

//function for checking if a cell is currently alive
int is_alive(struct universe *u, int column, int row) {
  char character = *(u->arr + row*u->columns + column);
  
  if (character == '.') {
    return 0;
  }
  else {
    return 1;
  }
}

//function for checking if a cell will be alive in the next generation 
//cells outside the grid are considered dead
int will_be_alive(struct universe *u, int column, int row) {
  int alive_count = 0;

  //count alive neighbours  
  if (row != 0 && column !=0 && is_alive(u, column-1, row-1)) 
      alive_count++;
  if (row != 0 && is_alive(u, column, row-1)) 
      alive_count++;
  if (row != 0 && column != (u->columns-1) && is_alive(u, column+1, row-1)) 
      alive_count++;
  if (column != 0 && is_alive(u, column-1, row)) 
      alive_count++;
  if (column != (u->columns-1) && is_alive(u, column+1, row)) 
      alive_count++;
  if (row != (u->rows-1) && column !=0 && is_alive(u, column-1, row+1)) 
      alive_count++;
  if (row != (u->rows-1) && is_alive(u, column, row+1)) 
      alive_count++;
  if (row != (u->rows-1) && column != (u->columns-1) && is_alive(u, column+1, row+1)) 
      alive_count++;
  
  //check if the current cell is alive
  if (is_alive(u, column, row)) {
    if (alive_count == 2 || alive_count == 3) 
      return 1;
    else 
      return 0;
  }  
  else {
    if (alive_count == 3)
      return 1;
    else 
      return 0; 
  }
}

//function for checking if a cell will be alive in the next generation 
//the grid wraps around 
int will_be_alive_torus(struct universe *u,  int column, int row) {
  int alive_count = 0;

  //count alive neighbours
  if (is_alive(u, (column-1+u->columns)%u->columns, (row-1+u->rows)%u->rows)) 
    alive_count++;
  if (is_alive(u, column, (row-1+u->rows)%u->rows))
    alive_count++;
  if (is_alive(u, (column+1)%u->columns, (row-1+u->rows)%u->rows)) 
    alive_count++;
  if (is_alive(u, (column-1+u->columns)%u->columns, row)) 
    alive_count++;
  if (is_alive(u, (column+1)%u->columns, row)) 
    alive_count++;
  if (is_alive(u, (column-1+u->columns)%u->columns, (row+1)%u->rows)) 
    alive_count++;
  if (is_alive(u, column, (row+1)%u->rows)) 
    alive_count++;
  if (is_alive(u, (column+1)%u->columns, (row+1)%u->rows)) 
    alive_count++;

  //check if the current cell is alive
  if (is_alive(u, column, row)) {
    if (alive_count == 2 || alive_count == 3) 
      return 1;
    else 
      return 0;
  }  
  else {
    if (alive_count == 3)
      return 1;
    else 
      return 0; 
  }  
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)) {
  for (int i = 0; i < u->rows; i++) 
    for (int j = 0; j < u->columns; j++) 
      if (is_alive(u, j, i))
        u->alive_overall++;  

  u->gen_number++;

  struct universe *new_u = NULL;
  new_u = (struct universe*) malloc(sizeof(struct universe)); 

  new_u->rows = u->rows;
  new_u->columns = u->columns;
  new_u->arr = (char *)malloc(new_u->rows * new_u->columns * sizeof(char));

  int will_be_alive_count = 0;
  char star = '*';
  char dot = '.';

  for (int i = 0; i < u->rows; i++) {
    for (int j=0; j < u->columns; j++) {
      if ((*rule)(u, j, i)) {
        will_be_alive_count++;
        *(new_u->arr + i*new_u->columns + j) = star;
      }
      else {
        *(new_u->arr + i*new_u->columns + j) = dot;
      }
    }
  }

  u->rows = new_u->rows;
  u->columns = new_u->columns;
  u->arr = new_u->arr;

  free(new_u);

}
void print_statistics(struct universe *u) {
  int alive_count = 0, current_total_number = u->dimension;
  int total_alive_cells = u->alive_overall, total_cells_number = u->dimension * u->gen_number; 

  for (int i = 0; i < u->rows; i++) {
    for (int j=0; j < u->columns; j++) {
      if (is_alive(u, j, i)) {
        alive_count++;
      }
    }
  }
  float percentage_currently_alive = (float) alive_count/current_total_number; 
  printf("%.3f %% of cells currently alive\n", percentage_currently_alive);

  float percentage_average_alive = (float) total_alive_cells/total_cells_number; 
  printf("%.3f %% of cells alive on average\n", percentage_average_alive);

}


