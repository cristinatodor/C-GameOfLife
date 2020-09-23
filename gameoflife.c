#include<stdio.h>
#include"gol.h"
#include<string.h>
#include<stdlib.h>

int main(int argc, char *argv[]) {
  struct universe *u = NULL;
  u = (struct universe*) malloc(sizeof(struct universe));

  int gen_number = 5, flag_statistics = 0, flag_infile = 0, flag_outfile = 0, flag_generations = 0;
  char input_filename[30] = "", output_filename[30] = "";
  int (*funct_ptr)(struct universe*, int, int); 
  funct_ptr = &will_be_alive;

  for (int optind = 1; optind < argc; optind++) {
    if (argv[optind][0] == '-') {
      switch (argv[optind][1]){
        case 'i':
          if (argv[optind+1] == NULL) {
            fprintf(stderr, "%s", "Error: no input file specified after -i command\n");
            exit(1);
          }
          else {
            if (argv[optind+1][0] != '-') {
              if (flag_infile == 0) {
                strcpy(input_filename, argv[optind+1]); 
                flag_infile = 1;
              }
              else {
                if (strcmp(input_filename, argv[optind+1]) != 0) {
                  fprintf(stderr, "%s", "Error: conflicting input file names\n");
                  exit(1);
                }
              }
            }
            else {
              fprintf(stderr, "%s", "Error: no input file specified after -i command\n");
              exit(1);
            }
          }
          break;
        
        case 'o':
          if (argv[optind+1] == NULL) {
            fprintf(stderr, "%s", "Error: no output file specified after -o command\n");
            exit(1);
          }
          else {
            if (argv[optind+1][0] != '-') {
               if (flag_outfile == 0) {
                strcpy(output_filename, argv[optind+1]);
                flag_outfile = 1;
               }
               else {
                 if (strcmp(output_filename, argv[optind+1]) != 0) {
                  fprintf(stderr, "%s", "Error: conflicting output file names\n");
                  exit(1);
                }
               }
            }
            else {
              fprintf(stderr, "%s", "Error: no output file specified after -o command\n");
              exit(1);
            }
          }
          break;

        case 'g':
          if (argv[optind+1] == NULL) {
              fprintf(stderr, "%s", "Eror: number of generations not specified after -g command\n");
              exit(1);
          }
          else {  
            if (argv[optind+1][0] != '-') {
              if (flag_generations == 0) {
                gen_number = atoi(argv[optind+1]);
                flag_generations = 1;
              }
              else {
                if (gen_number != atoi(argv[optind+1])) {
                  fprintf(stderr, "%s", "Error: conflicting number of generations\n");
                  exit(1);
                }
              }
            }
            else {
              fprintf(stderr, "%s", "Error: number of generations not specified after -g command\n");
              exit(1);
            }
          }
          break;

        case 's':
          flag_statistics = 1;
          break;

        case 't':
          funct_ptr = &will_be_alive_torus;
          break;
        
        default:
          fprintf(stderr, "%s", "Error: unknown command\n");
          exit(1);
          break;
        }
    }
  }
  
  FILE *infile = NULL, *outfile = NULL;

  //read initial generation
  if (strcmp(input_filename, "") != 0 ) {
    infile = fopen(input_filename, "r");
    read_in_file(infile, u);
  }
  else {
    read_in_file(stdin, u);
  }

  //create generations
  for (int i = 1; i <= gen_number; i++) {
    evolve(u, funct_ptr);
  }
 
  //output final generation
  if (strcmp(output_filename, "") != 0 ) {
    outfile = fopen(output_filename, "w");
    write_out_file(outfile, u);
  }
  else {
    write_out_file(stdout, u);
  }

  //print statistics
  if (flag_statistics == 1)
    print_statistics(u);
  
  free(u);

  return 0;
}
