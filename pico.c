#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico_lst.c"  /* functions for converting numbers to strings (errors, ranges etc.)*/
#include "pico_opts.c" /* option handling */
#include "pico_cmd.c"  /* functions for working with the pico device */


/**********************************************************/

int
main(int argc, char *argv[]){
  pico_pars_t glob_pars, pars;

  if (argc==1){
    printf("pico -- a program for manipulation picoscope devices\n"
           "Usage: pico [global options] cmd [cmd options] ...\n"
           "try 'pico -h' or 'pico <cmd> -h' for help\n");
    exit(0);
  }

  /* global parameters */
  par_ini(&glob_pars);
  get_pars(&argc, &argv, "pico", &glob_pars);

  run_cmd("pico", &glob_pars);
  if (glob_pars.help) exit(0);

  /* commands */
  while (argc>0) {
    char *cmd = argv[0];
    pars = glob_pars;
    get_pars(&argc, &argv, cmd, &pars);
    run_cmd(cmd, &pars);
  }

  /* close the device if needed */
  dev_close(&pars);
  exit(0);
}

