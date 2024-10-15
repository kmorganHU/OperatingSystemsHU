
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Lakers | sort".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv) {
  int pipefd[2];
	int pipefd2[2];
  int pid, pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
	char *sort_args[] = {"sort", NULL};

  // make a pipe (fds go in pipefd[0] and pipefd[1])
	if (pipe(pipefd) == -1) { 
			fprintf(stderr, "Pipe Failed" ); 
			return 1; 
	} 
	if (pipe(pipefd2) == -1) { 
			fprintf(stderr, "Pipe Failed" ); 
			return 1; 
	} 

	pid = fork();

  if (pid == 0) {
		  
			pid2 = fork();
			if (pid2 == 0) {
				// child 2 gets here and sorts
				// replace standard input with input part of pipe
				dup2(pipefd2[0], 0);

				// close pipes
				close(pipefd[0]);
				close(pipefd[1]);
				close(pipefd2[0]);
				close(pipefd2[1]);

				// execute sort
				execvp(sort_args[0], sort_args);

			} else {
				// child gets here and handles grep

				// replace standard input with input part of pipe
				// replace standard output with output part of pipe
				dup2(pipefd[0], 0);
				dup2(pipefd2[1], 1);
				
				// close pipes
				close(pipefd[0]);
				close(pipefd[1]);
				close(pipefd2[0]);
				close(pipefd2[1]);

				// execute grep
				execvp(grep_args[0], grep_args);
			}
    }
  else {
      // parent gets here and handles "cat scores"
      // replace standard output with output part of pipe
      dup2(pipefd[1], 1);

      // close pipes
			close(pipefd[0]);
			close(pipefd[1]);
			close(pipefd2[0]);
			close(pipefd2[1]);

      // execute cat
      execvp(cat_args[0], cat_args);
    }
}
