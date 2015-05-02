#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#include <cca.h>
#include <cca_config.h>		/* Header for config file related functions. parsing, validation etc */
#include <cca_worker.h>

struct cli_opts opts;
struct cca_conf conf;

void getopt_msg(){
	fprintf(stderr,"Unrecognized option\nUsage: ccs <option> <arg> ....\n\
Required options:\n\
-d: Data directory\n\
-c: Configuration file\n\
-p: Prefix of output files\n\
-n: Number of child processes. Generally it is a good idea to use Num_cores - 1 for the system to be responsive.\n\n\
Environment Variables - optional:\n\
GSL_RNG_TYPE: Should contain a valid RNG type. Defaults to ranlxd2\n\n\
In case an option is provided multiple times, only the last provided value is used.\n");
}

int read_cli( struct cli_opts * opts, int argc, char ** argv ){
	char c ;
	char * cvalue;

	memset( opts , 0 , sizeof( *opts));
	fprintf(stderr,"sizeof(*opts): %lu\n",sizeof(*opts));
	while (( c = getopt (argc, argv, "d:c:p:n:")) != -1 ){
		switch (c){
			case 'd':
				opts->data_dir = optarg;
				break;
			case 'c':
				opts->conf = optarg;
				break;
			case 'p':
				opts->prefix = optarg;
				break;
			case 'n':
				opts->procs = atoi(optarg);
				break;
			case '?':
				getopt_msg();
				return(1);
				break;
			default:
				getopt_msg();
				return(2);
		}
	}
	if( opts->data_dir == NULL || opts->conf == NULL || opts->prefix == NULL || opts->procs == 0){
		getopt_msg();	
		return(3);
	}
	if( optind < argc){
		getopt_msg();
		return(4);
	}
	fprintf(stderr,"data_dir=%s\nconf=%s\nprefix=%s\nprocs=%d\n",opts->data_dir, opts->conf,opts->prefix, opts->procs);
	return(0);
}

int cca_main_loop(){

	pid_t pid;
	int count;
	int status;
	int ret;
	int active_workers;
	for( count = 0, active_workers=0 ; count < conf.iterations; count++){
		if(! (pid=fork())){
			ret = worker_fork(count);
			break;
		}
		else if ( pid == -1){
			fprintf(stderr, "Unable to fork. Will wait for worker processes to finish before exiting\n");
			ret = 1;
		}
		else{
			active_workers++;
			if( active_workers i< opts.procs)
				continue;

			if( (pid = wait(&status)) != -1)
				active_workers--;
		}
	}
	if( pid > 0){
		while ( (pid = wait(&status)) != -1);
		fprintf(stderr, "All children exited. Now exiting\n");
	}
	return ret;
}

int main(int argc, char ** argv){
	int ret;
	ret = 0;
	if ((ret = read_cli(&opts, argc, argv)))
		exit (290+ret);
	

	if ( ( ret = parse_conf( opts, &conf ) ) ){
		/*	Error reading the configuration file, crib and exit	*/
		fprintf(stderr,"Unable to parse Configuration file. Please verify.\n");
		exit (300+ret);
	}

	if ( ( ret = check_conf( &conf ))){
		/*	Check if the conf file contains all necessary things	*/
		fprintf(stderr,"Config file verification failed.\n");
		exit ( 310 + ret );
	}

	ret = cca_main_loop (  )  ;
	ret += ret ? 500:0;
exit (ret);
}
