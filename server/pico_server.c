#include <microhttpd.h>
#include "pico.h"

/**********************************************************/
/* parse command line arguments  and fill pico_spars_t structure */
int pico_spars_parser(pico_spars_t * pars, int *argc, char ***argv){

  /* default values */
  pars->port = 8081;
  pars->dev  = "";
  pars->h    = 0;
  pars->verb = 0;

  /* parse  options */
  while(1){
    switch (getopt(*argc, *argv, "p:d:lhv")){
      case -1: return 0; /* end*/
      case '?':
      case ':': continue; /* error msg is printed by getopt*/
      case 'p': pars->port = atoi(optarg); break;
      case 'd': pars->dev = optarg; break;
      case 'l': dev_list(); return 1;
      case 'v': pars->verb = 1; break;
      case 'h':
        printf("pico_server -- server for picoscope device control\n"
               "Usage: pico_serv [options]\n"
               "Options:p\n"
               " -p <port> -- tcp port for connections\n"
               " -d <dev>  -- device serial number (empty for autodetect)\n"
               " -l        -- list all connected devices and exit\n"
               " -v        -- be verbose\n"
               " -h        -- write this help message and exit\n"
              );
        return 1;
    }
  }
  return 0;
}

/**********************************************************/
/* initialize the command parameters structure */
void
pico_cpars_init(pico_cpars_t * pars){
#undef CPAR
#define CPAR(comm, type, par, short, cnv, def, descr) pars->comm ## _ ## par = cnv(#def);
#include "pars.def"
}

/**********************************************************/
/* fill command parameters from key-value pairs,
 * libmicrohttpd callback for parsing GET arguments. */
int arg_parser(void *cls, enum MHD_ValueKind kind, const char *key, const char *val){
  pico_cpars_t *pars = (pico_cpars_t * )cls; /* command parameters */
  if (kind != MHD_GET_ARGUMENT_KIND) return MHD_NO; /* parse onlu GET arguments */
  /*printf(">> %s %s\n", key, val);*/
#undef CPAR
#define CPAR(comm, type, par, short, cnv, def, descr)\
  if ( (strlen(#comm)==0 || strcmp(pars->command, #comm)==0) && strcmp(key, #par)==0)\
    { pars->comm ## _ ## par = cnv(val); return MHD_YES; }
#include "pars.def"
  return MHD_YES;
}

/* libmicrohttpd callback for processing a requent. */
static int ahc_echo(void * cls, struct MHD_Connection * connection, const char * url,
	    const char * method, const char * version,
	    const char * upload_data, size_t * upload_data_size, void ** ptr) {
  static int dummy;
  struct MHD_Response * response;
  int ret;
  pico_cpars_t  cpars;                        /* command parameters */
  pico_opars_t  opars;                        /* output parameters */
  pico_spars_t *spars = (pico_spars_t *) cls; /* server parameters */
  str_pair_t *hh;

  /* check if the request is valid */
  if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */
  if (&dummy != *ptr) {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }
  if (0 != *upload_data_size) return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL; /* clear context pointer */
  if (url==NULL || strlen(url)==0 || url[0]!='/') return MHD_NO; /* just to be sure */

  /* parse arguments */
  cpars.command = url+1;
  pico_cpars_init(&cpars);
  MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, arg_parser, &cpars);

  /* run the command */
  pico_command(spars, &cpars, &opars);

  response = MHD_create_response_from_buffer(opars.dsize, opars.data, MHD_RESPMEM_MUST_FREE);
  ret = MHD_add_response_header (response, "Content-Type", "text/plain");
  ret = MHD_add_response_header (response, "Command",  cpars.command);
  ret = MHD_add_response_header (response, "Status",   opars.status);

  for(hh = opars.headers; hh->key; hh++)
    ret = MHD_add_response_header (response, hh->key, hh->val);

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

/**********************************************************/
int main(int argc, char ** argv) {
  struct MHD_Daemon * d;
  pico_spars_t spars; /* server parameters*/

  /* parse server parameters, exit if no server is needed */
  if (pico_spars_parser(&spars, &argc, &argv)) return 0;

  if (dev_open(&spars)>0) return 1;

  d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, spars.port, NULL, NULL, &ahc_echo,
	       &spars, MHD_OPTION_END);
  if (d != NULL){
    printf ("Server is running; press enter to stop it.\n");
    (void) getc(stdin);
    MHD_stop_daemon(d);
  }
  else {
    printf ("Error: can't start the http server\n");
  }

  if (dev_close(&spars)>0) return 1;
  return 0;
}
