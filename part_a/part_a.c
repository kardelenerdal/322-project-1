/*
 * @author Kardelen Erdal
 * 
 * This program is the local part of the project.
 * This program takes the path of an executable file and executes it in its child process.
 * Then it writes the output of this executable to the given txt file.
 * It includes creation of some processes and pipes.
 *
 * To run the program, you should first call make.
 * Then ./part_a.out "path of blacbbox" "name of the output file"
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>

#define READ_END  0
#define WRITE_END 1 

int main(int argc, char** argv) {

  // child pid
  pid_t childpid;

  // pipes
  // parent sends numbers, child receives
  int first_pipe[2]; 
  // child sends the result, parent receives
  int second_pipe[2];
  // child sends error, parent recieves 
  int third_pipe[2]; 

  // buffers: write buffer for parent to write parameters. read buffer for parent to read the result. error buffer for parent to read the error.
  char w_buf[100], r_buf[10000], e_buf[10000];

  // output file
  int output = open(argv[2], O_APPEND|O_WRONLY|O_CREAT, 0744);
  
  if(output<0){
    printf("%s\n", "cannot open file");
  }
   
  if (pipe(first_pipe) == -1) {
    fprintf(stderr,"First Pipe failed");
    return 1;
  }
  
  if (pipe(second_pipe) == -1) {
      fprintf(stderr,"Second Pipe failed");
      return 1;
  }

  if(pipe(third_pipe) == -1){
    fprintf(stderr, "%s\n", "Third Pipe failed");
  }

  childpid = fork();
  if (childpid == -1)  {
    perror("Failed to fork");
    return 1; 
  }

  // CHILD 
  if (childpid == 0) {   

    // changing the file descriptor 
    dup2(first_pipe[0], STDIN_FILENO);
    dup2(second_pipe[1], STDOUT_FILENO);
    dup2(third_pipe[1], STDERR_FILENO);

    // closing all the pipes because they are changed with dup2 function
    close(first_pipe[WRITE_END]); 
    close(second_pipe[READ_END]);  
    close(first_pipe[READ_END]);   
    close(second_pipe[WRITE_END]); 
    close(third_pipe[READ_END]);
    close(third_pipe[WRITE_END]);

    // replacing the code for child with the blackbox executable
    // as parameters, the path of the blackbox executable is given
    execl(argv[1], argv[1], NULL);
    return 1; 
  }

  // PARENT
  if (childpid > 0) {  

    // closing the pipe ends that will not be used              
    close(first_pipe[READ_END]); 
    close(second_pipe[WRITE_END]); 
    close(third_pipe[WRITE_END]);

    // parameters for blackbox executable
    int num1, num2;

    // reading the integers from the standard input
    scanf("%d", &num1);
    scanf("%d", &num2);

    // writing the arguments of blackbox to the write buffer to send it to the pipe
    sprintf(w_buf, "%d\n%d", num1, num2);

     // the parameters are sent to the pipe
    write(first_pipe[1], w_buf, strlen(w_buf));

    // first_pipe write end is closed because write part is done for parent
    close(first_pipe[1]);

    // reading possible output and error that comes from child from the pipe
    int out = read(second_pipe[0], r_buf, sizeof(r_buf));
    int err = read(third_pipe[0], e_buf, sizeof(e_buf));

    // closing the read ends of the pipes
    close(third_pipe[0]);
    close(second_pipe[0]);

    // result buffer
    char buffer[10000];

    // status of the output file
    int status;

    // if there is an error, write as fail. If it is a normal output, write as success.
    if(err > 0){
      e_buf[err] = 0;
      sprintf(buffer,"%s\n%s", "FAIL:", e_buf);
      status = write(output, &buffer, strlen(buffer));
    } else {
      r_buf[out] = 0;
      sprintf(buffer, "%s\n%s", "SUCCESS:", r_buf);
      status = write(output, &buffer, strlen(buffer));
    }

    if(status < 0){
      printf("%s\n", "cannot write");
    }
   return 0; 
  }
}
