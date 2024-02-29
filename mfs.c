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
#include <stdint.h>
#include <ctype.h>


#define MAX_NUM_ARGUMENTS 5

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

struct __attribute__((__packed__)) DirectoryEntry 
{
	char DIR_Name[11];
	uint8_t DIR_Attr;
	uint8_t Unused1[8];
	uint16_t DIR_FirstClusterHigh;
	uint8_t Unused2[4];
	uint16_t DIR_FirstClusterLow;
	uint32_t DIR_FileSize;
};
struct DirectoryEntry dir[16]; 

 
FILE *fp;
FILE *get_FP;
int16_t BPB_BytsPerSec;
int8_t  BPB_SecPerClus;
int16_t BPB_RsvdSecCnt;
int8_t  BPB_NumFATs; 
int32_t BPB_FATSz32; 
int16_t BPB_RootEntCnt; 
int32_t BPB_RootClus; 

int visited_CD;
char temp_CD[MAX_COMMAND_SIZE];
int counter;

char *token[MAX_NUM_ARGUMENTS];


int16_t NextLB( uint32_t sector )
{
	uint32_t FATAddress = ( BPB_BytsPerSec * BPB_RsvdSecCnt ) + ( sector * 4 );
	int16_t val;
	fseek( fp, FATAddress, SEEK_SET );
	fread( &val, 2, 1, fp );
	return val; 
}
int LBAToOffset(int32_t sector) 
{
	return (( sector - 2 ) * BPB_BytsPerSec) + (BPB_BytsPerSec * BPB_RsvdSecCnt) 
	+ (BPB_NumFATs * BPB_FATSz32 * BPB_BytsPerSec);
} 

void do_cd()
{
	int cd_address;
	char name[12];
	strcpy(name,token[1]);
	int i;
	for( i = 0; i < 11; i++ )
	{
		name[i] = toupper(name[i]);
	}
	
	char compareName[12];

	for(i=0; i<16; i++)
	{
		strncpy(compareName,dir[i].DIR_Name, 12);
		if(strncmp(compareName,name,strlen(name))==0)
		{
			if(dir[i].DIR_FirstClusterLow == 0)dir[i].DIR_FirstClusterLow = 2;
			
			cd_address = LBAToOffset(dir[i].DIR_FirstClusterLow);
			fseek(fp, cd_address, SEEK_SET);
			fread(dir, 16, sizeof(struct DirectoryEntry), fp);
			visited_CD = 1;
		}
	}
}

int main()
{
	strcpy(temp_CD,"..");

	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
	int count = 0;

	while( 1 )
	{
		// Print out the mfs prompt
		printf ("mfs> ");

		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

		/* Parse input */
		//char *token[MAX_NUM_ARGUMENTS];

		int   token_count = 0;                                 
                                                           
		// Pointer to point to the token
		// parsed by strsep
		char *arg_ptr;                                         
															   
		char *working_str  = strdup( cmd_str );                

		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;

		// Tokenize the input stringswith whitespace used as the delimiter
		while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
				  (token_count<MAX_NUM_ARGUMENTS))
		{
		  token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
		  if( strlen( token[token_count] ) == 0 )
		  {
			token[token_count] = NULL;
		  }
			token_count++;
		}
	
		//Saving off the names as backup
		char File_Name[MAX_COMMAND_SIZE];
		if(token[1] != NULL) strcpy(File_Name, token[1]);
		
		char New_File[MAX_COMMAND_SIZE];
		if(token[2] != NULL) strcpy(New_File, token[2]);
		
		
		if(token[0]!=NULL)
		{
			int Opened; 
			int Closed;
			int i,j;
			int found=0;
			char tempString[12];
			int wrong_File=0;
			
			//checking for quit or exit right at the beginning so code can easily quit/exit
			if(strcmp(token[0],"quit")==0||strcmp(token[0],"exit")==0)
			{
				exit(0);
			}
			else if(strcmp(token[0],"open")==0)
			{
				
				Opened = 1;	
				Closed = 0;
				fp = fopen(token[1],"r");
				if(fp == NULL)
				{
					printf("Error: File Not Found\n");
				}
				else
				{					
					
					//Do Initial Calculations
					fseek(fp, 11, SEEK_SET);
					fread(&BPB_BytsPerSec, 2, 1, fp);
					
					fseek(fp, 13, SEEK_SET);
					fread(&BPB_SecPerClus, 1, 1, fp);
					
					fseek(fp, 14, SEEK_SET);
					fread(&BPB_RsvdSecCnt, 2, 1, fp);
					
					fseek(fp, 16, SEEK_SET);
					fread(&BPB_NumFATs, 1, 1, fp);
					
					fseek(fp, 17, SEEK_SET);
					fread(&BPB_RootEntCnt, 2, 1, fp);
					
					fseek(fp, 36, SEEK_SET);
					fread(&BPB_FATSz32, 4, 1, fp);
			
					BPB_RootClus = (BPB_NumFATs * BPB_FATSz32 * BPB_BytsPerSec)
					+ (BPB_RsvdSecCnt * BPB_BytsPerSec);
					
					
					int myRoot = BPB_RootClus;
					
					//Set to Root Directory
					fseek(fp, BPB_RootClus, SEEK_SET);
					fread(dir, 16, sizeof(struct DirectoryEntry), fp);
				}
				
			} 
			else if(strcmp(token[0],"close")==0)
			{
				if(Opened)
				{
					Closed = 1;
					fclose(fp);
				}
				else
				{
					printf("Error: File system not open.\n");
				}
				
			}
			else if((strcmp(token[0],"bpb")==0) && (Closed != 1))
			{	
				printf("BPB_BytsPerSec: %d\n", BPB_BytsPerSec);
				printf("BPB_SecPerClus: %d\n", BPB_SecPerClus);
				printf("BPB_RsvdSecCnt: %d\n", BPB_RsvdSecCnt);
				printf("BPB_NumFATs: %d\n", BPB_NumFATs);
				printf("BPB_FATSz32: %d\n", BPB_FATSz32);
				printf("BPB_RootClus: %x\n", BPB_RootClus);

			}
			else if((strcmp(token[0],"stat")==0) && (Closed != 1))
			{
				
				
				char expanded_name[12];
				memset( expanded_name, ' ', 12 );

				char *myTok = strtok( token[1], "." );

				strncpy( expanded_name, myTok, strlen( myTok ) );

				myTok = strtok( NULL, "." );

				if( myTok )
				{
					strncpy( (char*)(expanded_name+8), myTok, strlen(myTok ) );
				}	
		
				expanded_name[11] = '\0';

				//int i;
				for( i = 0; i < 11; i++ )
				{
					expanded_name[i] = toupper( expanded_name[i] );
				}
				
				for(i=0; i<16; i++)
				{
					if( strncmp( expanded_name, dir[i].DIR_Name, 11 ) == 0 )
					{
						printf("\nFile Attribute: %d\nSize: %d\nStarting Cluster Number: %d\n\n", 
						dir[i].DIR_Attr, dir[i].DIR_FileSize, dir[i].DIR_FirstClusterLow);
						found = 1;
						break;
					}
				}
				if(found != 1)
				{
					printf("Error: File Not Found\n");
				} 
			}
			
			else if((strcmp(token[0],"ls")==0) && (Closed != 1))
			{
				int dot_flag = 0;
				int j, changed;
				changed = 0;
				if(token[1] != NULL)
				{
					if((strcmp(token[1],".")==0))
					{
						dot_flag = 1;
					}
					if((strcmp(token[1],"..")==0))
					{
						do_cd();
						for(j=0; j<16; j++)
						{
							if((dir[j].DIR_Attr == 0x01 || dir[j].DIR_Attr == 0x10 || dir[j].DIR_Attr == 0x20) && dir[i].DIR_Name[0] != 0xffffffe5)
							{ 
								strncpy(tempString, dir[j].DIR_Name, 12);
								printf("%s\n",tempString);
							}
						}
						changed = 1;
					}
					
				}
				if(dot_flag || !changed)
				{
					for(i=0; i<16; i++)
					{
						if((dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20) && dir[i].DIR_Name[0] != 0xffffffe5) 
						{
							strncpy(tempString, dir[i].DIR_Name, 12);
							printf("%s\n",tempString);
						}					
					}
				}
				if(changed == 1)
				{
					strcpy(token[1],temp_CD);
					do_cd();
					visited_CD = 0;
				}
			}
			else if(((strcmp(token[0],"cd")==0) && (Closed != 1)))
			{
				
				do_cd();
				counter++;
				if(token[1] != NULL && counter > 0) strcpy(temp_CD, token[1]);
				
				if((strcmp(token[1],"..")) == 0) 
				{	
					counter--;
					if(counter == 0)
					{
						strcpy(token[1],"..");
					}
				}
				
				if(!visited_CD)
				{
					printf("Error: Directory Not Found Please Try Again\n");
				}
				else
				{
					visited_CD = 0;
				}
				
			}
			else if((strcmp(token[0],"read")==0) && (Closed != 1))
			{
				unsigned char read[MAX_COMMAND_SIZE];
				int logical_Address;
				char expanded_name[12];
				memset( expanded_name, ' ', 12 );
				char *myTok = strtok( token[1], "." );

				strncpy( expanded_name, myTok, strlen( myTok ) );

				myTok = strtok( NULL, "." );

				if( myTok )
				{
					strncpy( (char*)(expanded_name+8), myTok, strlen(myTok ) );
				}	
		
				expanded_name[11] = '\0';

				for( i = 0; i < 11; i++ )
				{
					expanded_name[i] = toupper( expanded_name[i] );
				}
				for(i=0; i<16; i++)
				{
					if( strncmp( expanded_name, dir[i].DIR_Name, 11 ) == 0 )
					{
						if(token[2]!=NULL && token[3]!=NULL)
						{
							logical_Address = (LBAToOffset(dir[i].DIR_FirstClusterLow));
							logical_Address += (atoi(token[2]));
							int token_3 = (atoi(token[3]));
							fseek(fp, logical_Address, SEEK_SET);
							fread(read,token_3, 1, fp);
							
							for(j=0; j<token_3; j++)
							{
								printf("%d ", read[j]);
							}
							printf("\n");
						}
						else
						{
							printf("Wrong Parameters Try again!\n");
							found = 1;
							break;
						}
						
						found = 1;
						break;
					}
				}
				if(found != 1)
				{
					printf("Error: File Not Found\n");
				} 
				
				
				
			}
			else if((strcmp(token[0],"get")==0) && (Closed != 1))
			{
				
				unsigned char write[MAX_COMMAND_SIZE];
				int logical_Address;
				char expanded_name[12];
				memset( expanded_name, ' ', 12 );
				char *myTok = strtok( token[1], "." );

				strncpy( expanded_name, myTok, strlen( myTok ) );

				myTok = strtok( NULL, "." );

				if( myTok )
				{
					strncpy( (char*)(expanded_name+8), myTok, strlen(myTok ) );
				}	
		
				expanded_name[11] = '\0';

				for( i = 0; i < 11; i++ )
				{
					expanded_name[i] = toupper( expanded_name[i] );
				}
				for(i=0; i<16; i++)
				{
					if( strncmp( expanded_name, dir[i].DIR_Name, 11 ) == 0 )
					{
						
						if(token[2] != NULL)
						{
							strcpy(File_Name, New_File);
						}
						
						int value[512];
						get_FP = fopen(File_Name,"w");
						
						int curr_File_Size = dir[i].DIR_FileSize;
						int next_cluster = dir[i].DIR_FirstClusterLow;
						while(curr_File_Size >= 512)
						{
							logical_Address = (LBAToOffset(next_cluster ));
							fseek(fp, logical_Address, SEEK_SET);
							fread(value, 1, 512, fp);
							fwrite(value, 1, 512, get_FP);
							next_cluster = NextLB(next_cluster );
							curr_File_Size -= 512;
								
						}
						if(curr_File_Size != 0)
						{
							logical_Address = (LBAToOffset(next_cluster ));
							fseek(fp, logical_Address, SEEK_SET);
							fread(value, 1, curr_File_Size, fp);
							fwrite(value, 1, curr_File_Size, get_FP);
						}
								
						found = 1;
						break;
					}
				}
				if(found != 1)
				{
					printf("Error: File Not Found\n");
				} 
				
			}
			else
			{
				printf("Error: File system image must be opened first.\n");
			}
			
			
		}	
	
		free( working_root );

	}
  
	return 0;
}
