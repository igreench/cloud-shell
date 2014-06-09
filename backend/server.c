#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wait.h>
#include "server.h"

char *infile;
char *outfile;
char *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

void shell(char *str)
{
	int i;
	char line[256];      /*  allow large command lines  */
	int ncmds;
	memset(line, 0, sizeof(line));
	strcpy(line, strcat(str,"\n"));
	ncmds = parseline(line);
	for (i = 0; i < ncmds; i++) {
		pid_t pid = fork();
		if(0 == pid) {
			if(bkgrnd) {
				if(-1 == setpgid(0, 0)) {
					perror("Couldn't set process group ID");
					return;
				}
			}
			execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);
			perror("Couldn't execute command");
			return;
		} else {
			if(-1 != pid) {
				if(!bkgrnd) {
					if(-1 == waitpid(pid, NULL, 0)) {
						perror("Couldn't wait for child process termination");
					}		
				} else {
					printf("Background process ID: %ld\n", (long)pid);
				}
			} else {
				perror("Couldn't create process");
			}
		}
	}
	return;
}

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}
	listen(sockfd,5);
	pid_t pid;
	for (;;) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			error("ERROR on accept");
			//exit(1);
		}
		if ((pid = fork()) == 0) {
			close(sockfd);
			bzero(buffer, 256);
			n = read(newsockfd, buffer, 255);
			if (n < 0) {
				error("ERROR reading from socket");
			}
			if (n > 0) {
				//printf("read: %s\n", buffer);
				dup2( newsockfd, STDOUT_FILENO );
				dup2( newsockfd, STDERR_FILENO );
				shell(buffer);
				n = write(newsockfd, buffer, n);
				if (n < 0) {
				error("ERROR writing to socket");
				}
				/*if (n > 0) {
					printf("write: %s\n", buffer);
				}*/
			}
			exit(0);
		}
		close(newsockfd);
	}
	return 0;
}