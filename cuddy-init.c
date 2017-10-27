#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

// a place to keep the options
struct _options {
    char* command;
    unsigned int delay;
    char* error;
    char* initial;
    unsigned int splay;
    unsigned int verbose;
} options;

// usage text (-h)
void print_usage() {
    fprintf(stdout, "Cuddy INIT, v###VERSION###\n");
    fprintf(stdout, "Usage: cuddy-init [-c string] [-d int] [-e string] [-h bool] [-i string] [-s int] [-v bool]\n");
    fprintf(stdout, "  -c  (command)   A command to execute every cycle (use '-c' to disable).\n");
    fprintf(stdout, "  -d  (delay)     The delay between cycles.\n");
    fprintf(stdout, "  -e  (error)     A command to execute if '-c' exit code > 0.\n");
    fprintf(stdout, "  -h  (help)      Display help (this text).\n");
    fprintf(stdout, "  -i  (initial)   An initial command to execute before the cycle starts.\n");
    fprintf(stdout, "  -s  (splay)     Used to generate a random number to delay between each cycle.\n");
    fprintf(stdout, "  -v  (verbose)   Verbose output.\n");
    exit(1);
}

// a function to parse arguments, and set defaults
void parse_args(const int argc, char* const argv[]) {
    // set defaults
    options.command  = "cuddy-client";
    options.initial  = "";
    options.error    = "";
    options.delay    = 240;
    options.splay    = 60;
    options.verbose  = 0;

    // parse args
	int c;
	while ((c = getopt(argc, argv, "c:d:e:hi:s:v")) != -1) {
		switch (c) {
            case 'c':
				options.command = optarg;
				break;
            case 'd':
				options.delay = atoi(optarg);
				break;
            case 'e':
				options.error = optarg;
				break;
            case 'h':
				print_usage();
                exit(1);
				break;
            case 'i':
				options.initial = optarg;
				break;
            case 's':
				options.splay = atoi(optarg);
				break;
            case 'v':
				options.verbose++;;
				break;
			case '?':
				return;
			default:
				/* Should never happen */
				return;
		}
	}
}

// a usefull wrapper function for the system() command
int _exec(char *cmd)
{
    unsigned int _code;

    // make sure a command is provided
    if (*cmd) {
        if (options.verbose > 0) {
            fprintf(stdout, "executing command: %s\n", cmd);
        }

        // execute the provided command
        _code = system(cmd);
        if (_code > 0 && options.verbose > 0) {
            fprintf(stdout, "failed to execute command: %s (exit code: %d)\n", cmd, _code);
        }

        // return the exit code
        return _code;
    }

    // "everything is fine"
    return 0;
}

// this is where the magic happens
int main(int argc, char **argv)
{
    // parse provided arguments
	parse_args(argc, argv);

    // define vars, and seed rand()
    unsigned int _splay, _code;
    srand((unsigned int)time(NULL));

    // execute initial command, if any
    _code = _exec(options.initial);
    if (_code > 0) {
        exit(_code);
    }

    // beginning the cycle
    while(1) {

        // if splay has been defined, delay for a random amount of time
        if (options.splay > 0) {
            _splay = rand() % options.splay;
            if (options.verbose > 0) {
                fprintf(stdout, "sleeping for %d seconds (splay)\n", _splay);
            }
            sleep(_splay);
        }

        // execute command
        if (options.command != NULL) {
            if (options.verbose > 0) {
                fprintf(stdout, "executing command: %s\n", options.command);
            }

            // execute the main command
            _code = _exec(options.command);
            if (_code > 0) {
                // execute error command, if any, or exit
                if (options.error) {
                    // execute error cmd, exit if code > 0
                    if (_exec(options.error) > 0) {
                        exit(_code);
                    }
                } else {
                    exit(_code);
                }
            }
        }

        // if interval has been defined, sleep for a given amount of time
        if (options.delay > 0) {
            if (options.verbose > 0) {
                fprintf(stdout, "sleeping for %d seconds (delay)\n", options.delay);
            }

            // delay next cycle
            sleep(options.delay);
        }
    }

    /* Should never happen */
    exit(1);
}
