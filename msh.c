/*
Name: Ijaz Mohamed Umar
ID: 1001496537
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // This Mav Shell would support 10 arguments

int main()
{   
    //Declaring & initializing any counters or arrays that I need later on
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    int pidCounter=0;
    int pidIndex=0;
    
    int status;
    int MyPid_Array[15];
 
    int historyCounter=0; 
    int historyIndex=0;
    
    //Going to intialize the pid array with zeros so 
    // I can support my for loop counter when showing it
    int initialCounter;
    for(initialCounter=0; initialCounter<15; initialCounter++)
    {
       MyPid_Array[initialCounter]=0;
    }

	while( 1 )
	{
		// Print out the msh prompt
		printf ("msh> ");

		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

		/* Parse input */
		char *token[MAX_NUM_ARGUMENTS];

		int token_count = 0;                                 
								   
		//going to store the cmd in a history array
		//so that when calling the history cmd we will
		// be able to show the last 15 history
		char History[15][MAX_COMMAND_SIZE];
		strcpy(History[historyCounter],(cmd_str));
		historyCounter++;
		//this is for the for loop to print out the values
		// so it prints only the amount entered
		if(historyIndex<=14)
		{
			historyIndex++;
		}
		//Only 15 allowed so resetting when past it
		if(historyCounter>14)
		{
			historyCounter=0;
		}
		
		//this is where I change the cmd_str to the cmd that
		//the user asked for from the history to execute
		if(cmd_str[0]=='!')
		{
			char tempNum = cmd_str[1];
			int tempCMD = tempNum - '0';
			cmd_str=History[tempCMD];
		}                               
															   
		// Pointer to point to the token
		// parsed by strsep
		char *argument_ptr;                                         
															   
		char *working_str  = strdup( cmd_str );        
		
		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;

		// Tokenize the input stringswith whitespace used as the delimiter
		while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
			(token_count<MAX_NUM_ARGUMENTS))
		{
			token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
			token_count++;
		}
		
		//doing quit or exit here so that it doesn't get 
		//duplicated when doing fork, and to not cause problems
		if(token[0]!=NULL)
		{
			//checking for quit or exit right at the beginning so code can easily quit/exit
			if(strcmp(token[0],"quit")==0||strcmp(token[0],"exit")==0)
			{
				exit(0);
			}
			
			//Checking to see if the input enterd is my built in
			//or if it's apart of user bin, so that exec doesn't
			//keep giving error for command not found
			int counter;
			int isBuiltIn=0;
			char *MyBuiltIn[3]={"cd","showpids","history"};
			for(counter=0;counter<3;counter++)
			{
				//we are setting the flag to notice if its a built in
				if(strcmp(token[0],MyBuiltIn[counter])==0)
				{
					isBuiltIn=1;
				}
			}
		  
		   
			//forking to start the parent and child process
			//we will then check is its parent or child
			// if it's child we do the user bin commands
			// if it's the parent then we do the built ins
			pid_t pid = fork();
			
			//storing the pids so we can access them when a command comes
			// and will adjust the counters for easy accesbility 
			MyPid_Array[pidCounter++]=pid;
			
			//this is for the for loop to print out the values
			// so it prints only the amount entered
			if(pidIndex<=14)
			{
				pidIndex++;
			}
			//Only 15 allowed so resetting when past it
			if(pidCounter>14)
			{
				pidCounter=0;
			}	
			
			//we are checking if its the child pid and if thats the case
			//we will execute the user cmd if it's not one of the built ins
			if(pid == 0)
			{
				// Checking to see that to command is in user bin and not built in
				if(isBuiltIn!=1)
				{
					int ret = execvp(token[0],token);
					//if exec fails we show the user that the command entered is wrong
					if( ret == -1 )
					{
						printf("%s: Command Not Found\n\n",cmd_str);
					}
				}
				exit(0);
			}
			else
			{
				//waiting till the child process finishes to start the parent
				wait( & status );
				
				//if the cmd is cd, then I use chdir to change
				// to the specifed directory
				if(strcmp(token[0],"cd")==0)
				{
					chdir(token[1]);
				}
				
				//Will print out the pids if cmd is showpids
				//already stored it prev, so just printing
				if(strcmp(token[0],"showpids")==0)
				{
					int i;
					for(i=0;i<pidIndex;i++)
					{
						printf("%d: %d\n",i,MyPid_Array[i]);    
					}
					
				}
				//Already stored history in the beginning
				// going to print it if cmd is history
				if(strcmp(token[0],"history")==0)
				{
					int j;
					for(j=0;j<(historyIndex);j++)
					{
						printf("%d: %s\n",j,History[j]);
					}
					
				}
				
			}
		}	
		
		free( working_root );

	}
	return 0;
}



