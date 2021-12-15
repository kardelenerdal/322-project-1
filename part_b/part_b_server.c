/*
 * @author Kardelen Erdal
 * 
 * This program is the remote part of the project.
 * This is the server program that creates the child process and replaces the code in the child process with the blackbox executable.
 * Then it returns the output of this executable to the client.
 *
*/

#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

char **
blackbox_1_svc(numbers *argp, struct svc_req *rqstp)
{
	static char * result;

    // pid for child
    pid_t childpid;

    // pipes
    int p2c[2], c2p[2], error[2];

    // buffers
    // write buffer for parent to write, read buffer for parent to read a normal output, error buffer for parent to read error (if there is any)
    char w_buf[1000], r_buf[10000], e_buf[10000];

    // result buffer
    char buffer[10000];

    pipe(p2c);
    pipe(c2p);
    pipe(error);
    
    childpid = fork();

    if (childpid == -1)  {
        perror("Failed to fork");
    }

    // child code
    if (childpid == 0) {           
        
        // changing the file descriptor                  
        dup2(p2c[0], STDIN_FILENO);
        dup2(c2p[1], STDOUT_FILENO);
        dup2(error[1], STDERR_FILENO);

        // closing all the pipes because they are changed with dup2 function
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(p2c[1]);
        close(error[0]);
        close(error[1]);

        // replacing the code for child with the blackbox executable
        // as parameters, the path of the blackbox executable is given
        execl(argp->pathOfBlackbox, argp->pathOfBlackbox, NULL);
    }

    // parent code
    if (childpid != 0) { 

        // parent will not write to c2p and error pipes                 
        close(c2p[1]);    
        close(error[1]);

        // writing the arguments of blackbox to the write buffer to send it to the pipe
        sprintf(w_buf, "%d\n%d", argp->a, argp->b);

        // the parameters are sent to the pipe
        write(p2c[1], w_buf, strlen(w_buf));

        // p2c write end is closed because write part is done for parent
        close(p2c[1]);

        // reading possible output and error that comes from child from the pipe
        int out = read(c2p[0], r_buf, sizeof(r_buf));
        int err = read(error[0], e_buf, sizeof(e_buf));
        
        // if there is an error, return as fail. If it is a normal output, return as success.
        if(err > 0){
        	e_buf[err] = 0;
        	sprintf(buffer,"%s\n%s", "FAIL:", e_buf);
        } else {
        	r_buf[out] = 0;
        	sprintf(buffer, "%s\n%s", "SUCCESS:", r_buf);
        }
        
        // closing the read ends of the pipes
        close(c2p[0]);
        close(error[0]);
    }

    // the result is in the buffer
    result = buffer;
    return &result;
}
