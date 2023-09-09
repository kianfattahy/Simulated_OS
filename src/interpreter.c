#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> //new included library for chdir
#include <sys/stat.h> //new included library for stat
#include <stdbool.h>
#include <dirent.h> 
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "ready_queue.h"


int MAX_ARGS_SIZE = 7;

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int badcommandMkdir(){
	printf("%s\n", "Bad command: my_mkdir");
	return 4;
}

int badcommandCd(){
	printf("%s\n", "Bad command: my_cd");
	return 5;
}

int bad_command_file_does_not_exist(){
	printf("%s\n", "Bad command: File not found");
	return 1;
}

int badcommand_scheduling_policy_error(){
	printf("%s\n", "Bad command: scheduling policy incorrect");
	return 1;
}

int badcommand_no_mem_space(){
	printf("%s\n", "Bad command: no space left in shell memory");
	return 1;
}

int badcommand_ready_queue_full(){
	printf("%s\n", "Bad command: ready queue is full");
	return 1;
}

int badcommand_same_file_name(){
	printf("%s\n", "Bad command: same file name");
	return 1;
}

int handleError(int error_code){
	//Note: badcommand-too-man-token(), badcommand(), and badcommand-same-file-name needs to be raised by programmer, not this function
	if(error_code == 11){
		return bad_command_file_does_not_exist();
	}else if (error_code == 21)
	{
		return badcommand_no_mem_space();
	}else if (error_code == 14)
	{
		return badcommand_ready_queue_full();
	}else if (error_code == 15){
		return badcommand_scheduling_policy_error();
	}else{
		return 0;
	}
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int echo(char* var);
int my_ls();
int my_mkdir(char* dirname);
int my_touch(char* filename);
int my_cd(char* dirname);
int exec(char *fname1, char *fname2, char *fname3, char* policy, bool background, bool mt);

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	if (args_size < 1){
		return badcommand();
	}

	if (args_size > MAX_ARGS_SIZE){
		return badcommandTooManyTokens();
	}

	for (int i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();	
		int total_len = 0;
		for(int i=2; i<args_size; i++){
			total_len+=strlen(command_args[i])+1;
		}
		char *value = (char*) calloc(1, total_len);
		char spaceChar = ' ';
		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], strlen(command_args[i]));
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		int errCode = set(command_args[1], value);
		free(value);
		return errCode;
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "echo")==0){
		if (args_size > 2) return badcommand();
		return echo(command_args[1]);

	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 1) return badcommand();
		return my_ls();
	
	} else if (strcmp(command_args[0], "my_mkdir")==0) {
		if (args_size > 2) return badcommand();
		return my_mkdir(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_touch")==0) {
		if (args_size > 2) return badcommand();
		return my_touch(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_cd")==0) {
		if (args_size > 2) return badcommand();
		return my_cd(command_args[1]);
	
	} else if (strcmp(command_args[0], "exec")==0) {
		bool MT = false;
		if(strcmp(command_args[args_size-1], "MT")==0){
			MT=true;
			args_size--;
		}
		if (args_size <= 2 || args_size >6) return badcommand();
		if(strcmp(command_args[args_size-1], "#") !=0){
			if(args_size == 3){
				return exec(command_args[1],NULL,NULL,command_args[2], false, MT);
			}else if(args_size == 4){
				return exec(command_args[1],command_args[2],NULL,command_args[3], false, MT);
			}else if(args_size == 5){
				return exec(command_args[1],command_args[2],command_args[3],command_args[4], false, MT);
			}
		}
		else{
			if(args_size == 4){
				return exec(command_args[1],NULL,NULL,command_args[2], true, MT);
			}else if(args_size == 5){
				return exec(command_args[1],command_args[2],NULL,command_args[3], true, MT);
			}else if(args_size == 6){
				return exec(command_args[1],command_args[2],command_args[3],command_args[4], true, MT);
			}
		}
		
	} else {
		return badcommand();
	}
	return 0;
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

void del_backingstore() {
	DIR* dir = opendir("backingstore");
	char cmd[50];
	sprintf(cmd, "rm -r %s", "backingstore");
	system(cmd);
}

int quit(){
	printf("%s\n", "Bye!");
	del_backingstore();
	threads_terminate();
	ready_queue_destory();
	exit(0);
}

int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);
	mem_set_value(var, value);
	return 0;

}

int print(char* var){
	char *value = mem_get_value(var);
	if(value == NULL) value = "\n";
	printf("%s\n", value); 
	return 0;
}

int echo(char* var){
	if(var[0] == '$') print(++var);
	else printf("%s\n", var); 
	return 0; 
}

int my_ls(){
	int errCode = system("ls | sort");
	return errCode;
}

int my_mkdir(char *dirname){
	char *dir = dirname;
	if(dirname[0] == '$'){
		char *value = mem_get_value(++dirname);
		if(value == NULL || strchr(value, ' ') != NULL){
			return badcommandMkdir();
		}
		dir = value;
	}

	int namelen = strlen(dir);
	char* command = (char*) calloc(1, 7+namelen); 
	strncat(command, "mkdir ", 7);
	strncat(command, dir, namelen);
	int errCode = system(command);
	free(command);
	return errCode;
}

int my_touch(char* filename){
	int namelen = strlen(filename);
	char* command = (char*) calloc(1, 7+namelen); 
	strncat(command, "touch ", 7);
	strncat(command, filename, namelen);
	int errCode = system(command);
	free(command);
	return errCode;
}

int my_cd(char* dirname){
	struct stat info;
	if(stat(dirname, &info) == 0 && S_ISDIR(info.st_mode)) {
		//the object with dirname must exist and is a directory
		int errCode = chdir(dirname);
		return errCode;
	}
	return badcommandCd();
}

void create_backingstore_entry(char* input_file_path) {
    FILE *input_file = fopen(input_file_path, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return;
    }

	char output_file_name[100];
	sprintf(output_file_name, "backingstore/%s", input_file_path);

    FILE *output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), input_file) != NULL) {
        char *semicolon_position = strchr(line, ';');
        if (semicolon_position != NULL) {
            *semicolon_position = '\0';  // Replace the semicolon with a null character
			// printf("line = %s\n",line);
			// printf("line after semi = %s\n", semicolon_position + 1);
            fprintf(output_file, "%s\n", line);
            fprintf(output_file, "%s", semicolon_position + 1);
        } else {
            fprintf(output_file, "%s", line);
        }
    }

    fclose(input_file);
    fclose(output_file);

    return;
}


int run(char* script){
	exec(script, NULL, NULL, "RR", false, false);
	return 0;
}


int exec(char *fname1, char *fname2, char *fname3, char* policy, bool background, bool mt){
	create_backingstore_entry(fname1);
	QueueNode* n1 = malloc(sizeof(QueueNode));
	n1->pcb = load_script(fname1);
	ready_queue_add_to_tail(n1);


	if(fname2!=NULL){
		create_backingstore_entry(fname2);
		QueueNode* n2 = malloc(sizeof(QueueNode));
		n2->pcb = load_script(fname2);
		ready_queue_add_to_tail(n2);
		if(strcmp(fname1,fname2)==0){
			return badcommand_same_file_name();
		}
	}
	if(fname3!=NULL){
		create_backingstore_entry(fname3);
		QueueNode* n3 = malloc(sizeof(QueueNode));
		n3->pcb = load_script(fname3);
		ready_queue_add_to_tail(n3);	
		if(strcmp(fname1,fname3)==0 || strcmp(fname2,fname3)==0){
			return badcommand_same_file_name();
		}
		
	}
	
	if(schedule_by_policy(policy, mt) == 15){
		return handleError(15);
	}
	return 0;
}
