#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#ifdef REQ_SIZE
#define REQ_SIZE 5000
const static struct option long_options[] = {
		{"client", required_argument, NULL, 'c'},
		{"time", required_argument, NULL, 't'},
		{"usage", no_argument, NULL, 'u'},
};

int client = 0;
int time = 30; // defalut 30 sec;
static char reqest[REQ_SIZE];

void usage() {
	printf("-c | --client number of clients\n");
	printf("-t | --time runing time\n");
	printf("-u | --usage usage\n");
	eixt(EXIT_SUCCESS);
};

int main(int argc, char ** argv) {
	int option_index = 0;
	int opt = 0;
	while((opt = getopt_long(argc, argv, "c:t:u", long_options, &option_index)) != -1) {
		switch(opt) {
			case 'c': {
				client = optarg;
				break;
			}
			case 't': {
				time = optarg;
				break;
			}
			case 'u': {
				usage();
			}
		}
	}



}
