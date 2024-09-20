// include all necessary libraries and files
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>
#include<dirent.h>
#include<ctype.h>

// define constants 
// max size of buffer
#define MAX_BUFFER_SIZE 1024 
// reserved separators (tab and newline)
#define SEPARATORS "\t\n"
// max num of arguments
#define MAX_ARGS 64 
// max num of input redirection files
#define MAX_TOKENS 10
// max length of file or folder
#define MAX_PATH 100;

// define data types
// file redirection 
typedef struct
{ 
	char *filename;
	char open[3];
	char opentype[3];
} Redirect; 

// define error num and envir array
extern int errno;
extern char **environ;

// define all the necessary functions
// command execution
int Exec(char *buffer);
int Command_exec(char **args, const Redirect *Inputs,const Redirect *Outputs,int *states);
// check command structure
int Command_strtok(char *buf,char **args,int *states,Redirect *InPuts,Redirect *OutPuts);
// 'cd' command
int Command_cd(char **args,const Redirect *Inputs, int *states);
// 'pwd' command
int show_pwd(void);
// 'clear' command
void clr(void);
// 'dir' command
int dir(char **args,const Redirect *Inputs, int *states);
// 'environ' command
int list_environ(void);
// 'echo' command
int Command_echo(char **args,const Redirect *Inputs,int *states);
// 'help' command
int Command_help(char **args,const Redirect *Outputs,int *states);
// print error messages
int Error(int errortype,char **args,const Redirect *  IOputs,const int *states, char * msg) ;
// keep reading lines from stdin and call Exec()
int Command_shell(FILE *inputfile,const Redirect *Outputs,const int *states);

// add delay to help order of processes
void Command_delay(int n);
// get the path of a file or folder
void get_fullpath(char *fullpath,const char *shortpath);
// 'myshell' command with batchfile
int Command_bat(char **args,const Redirect *Inputs,const Redirect *Outputs,int *states); 
