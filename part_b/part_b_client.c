/*
 * @author Kardelen Erdal
 * 
 * This program is the remote part of the project.
 * This program takes the path of an executable file and executes it in the server program.
 * Then it writes the output of this executable to the given txt file.
 * The program also takes the server IP address and executes the program in that address.
 * It includes creation of remote process.
 *
 * To run the program you should first call make.
 * Then ./part_b_server.out &
 * ./part_b_client.out "path of blackbox" "name of the output file" "server ip address"
 *
*/

#include "part_b.h"
#include <fcntl.h>
#include <unistd.h>

/*
 * Takes the numbers, path of the blackbox executable and path of the output file.
 * Writes the result of the unknown blackbox program to the given output file.
 * If the output file is not existed, it is created.
*/
void
blackbox_prog_1(char *host, int num1, int num2, char pathOfBlackbox[], char pathOfOutput[])
{
	CLIENT *clnt;
	char * *result_1;
	numbers  blackbox_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, BLACKBOX_PROG, BLACKBOX_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	// initializing the parameters
	blackbox_1_arg.a = num1;
	blackbox_1_arg.b = num2;
	memset(blackbox_1_arg.pathOfBlackbox, '\0', sizeof(blackbox_1_arg.pathOfBlackbox));
	strcpy(blackbox_1_arg.pathOfBlackbox, pathOfBlackbox);   

	// getting the result from the blackbox program
	result_1 = blackbox_1(&blackbox_1_arg, clnt);
	
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}

	// writing the result to the output file
	int output = open(pathOfOutput, O_APPEND|O_WRONLY|O_CREAT, 0744);
	int status = write(output, *result_1, strlen(*result_1));
    
    if(status < 0){
      printf("%s\n", "cannot write");
    }
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}

/*
 * the main method that will be called by the client at the very beginning of the program.
*/
int
main (int argc, char *argv[])
{
	char *host;

	// blackbox parameters
	int num1, num2;

	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}

	host = argv[3];
	char* pathOfBlackbox = argv[1];
	char* pathOfOutput = argv[2];
	
	// read paramaters for blackbox program
	scanf("%d", &num1);
	scanf("%d", &num2);
	
	blackbox_prog_1 (host, num1, num2, pathOfBlackbox, pathOfOutput);
exit (0);
}
