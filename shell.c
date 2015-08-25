#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#define MAX_ARGS 20

typedef struct Job
{
	char cmd[256];
	char status[20];
	int id;
	int pid;
	struct Job *next;
} job;

job *head = NULL;
int globalChild=0;
char globalCmd[256];
int globalId=1;

void trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;
    while (isspace(str[begin]))
        begin++;
    while ((end >= begin) && isspace(str[end]))
        end--;
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];
    str[i - begin] = '\0';
}

void insert(job value)
{
      job *temp; 
      temp=head;
      job *var=(job *)malloc(sizeof (job));
      strcpy(var->cmd,value.cmd);
	  strcpy(var->status,value.status);
	  var->id = value.id;
	  var->pid = value.pid;
      if(head==NULL)
      {
          head=var;
          head->next=NULL;
      }
      else
      {
          while(temp->next!=NULL)
          {     
               temp=temp->next;
          }
          var->next=NULL;
          temp->next=var;
      }
}

job delete(int value)
{
     job *temp,*var;
     temp=head;
	 job j;
	 j.id = -1;
     while(temp!=NULL)
     {
          if(temp->id == value)
          {
                if(temp==head)
                {
                     head=temp->next; 
					 strcpy(j.cmd,temp->cmd);
					 strcpy(j.status,temp->status);
					 j.id = temp->id;
					 j.pid = temp->pid;
                     free(temp);
                     return j;
                }
                else
                {
                     var->next=temp->next;
				 	 strcpy(j.cmd,temp->cmd);
					 strcpy(j.status,temp->status);
					 j.id = temp->id;
					 j.pid = temp->pid;
                     free(temp);
                     return j;
                }
          }
          else
          {
               var=temp;
               temp=temp->next;
          }
     }
	 if(head==NULL)
		globalId=1;
	 return j;
}

job update(int value)
{
     job *temp,*var;
     temp=head;
	 job j;
	 j.id = -1;
     while(temp!=NULL)
     {
          if(temp->id == value)
          {
                strcpy(temp->status,"Running");
				strcpy(j.cmd,temp->cmd);
				strcpy(j.status,temp->status);
				j.id = temp->id;
				j.pid = temp->pid;
				return j;
          }
          else
          {
               var=temp;
               temp=temp->next;
          }
     }
	 return j;
}

job deleteByPid(int value)
{
     job *temp,*var;
     temp=head;
	 job j;
	 j.id = -1;
     while(temp!=NULL)
     {
          if(temp->pid == value)
          {
                if(temp==head)
                {
                     head=temp->next; 
					 strcpy(j.cmd,temp->cmd);
					 strcpy(j.status,temp->status);
					 j.id = temp->id;
					 j.pid = temp->pid;
                     free(temp);
                     return j;
                }
                else
                {
                     var->next=temp->next;
				 	 strcpy(j.cmd,temp->cmd);
					 strcpy(j.status,temp->status);
					 j.id = temp->id;
					 j.pid = temp->pid;
                     free(temp);
                     return j;
                }
          }
          else
          {
               var=temp;
               temp=temp->next;
          }
     }
	 if(head==NULL)
		globalId=1;
	 return j;
}


void sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		if(globalChild)
		{
			kill(globalChild,SIGINT);
			printf("\nProcess Terminated\n");
		}
		else
			printf("\b\b  \b\b");
	}
	if (signo == SIGTSTP)
	{
		if(globalChild)
		{
			setpgid(globalChild,0);
			kill(globalChild,SIGTSTP);
			job j;
			j.id = globalId;
			j.pid = globalChild;
			strcpy(j.cmd,globalCmd);
			strcpy(j.status,"Stopped");
			insert(j);
			printf("\n[%d] Stopped\t\t\t%s\n",globalId,globalCmd);
			globalId++;
		}
		else
			printf("\b\b  \b\b");
	}
	if (signo == SIGCHLD)
	{
		int pid  = waitpid(-1,NULL,WNOHANG);
		if(pid!=-1)
			deleteByPid(pid);
	}
}

int parsecmd(char *cmd,char *argv[])
{
	char *token;
	token = strtok(cmd, " ");
	int i = 0;
	while( token != NULL ) 
	{
		argv[i] = malloc(20*sizeof(char));
		strcpy(argv[i++],token); 
		token = strtok(NULL, " ");
	}
	argv[i++] = NULL;
	return i;
}

void printJobs()
{
	job *temp = head;
	while(temp!=NULL)
	{
		printf("[%d]+   %s\t\t\t%s\n",temp->id,temp->status,temp->cmd);
		temp=temp->next;
	}
}

int builtin(char *argv[])
{
	if(!strcmp(argv[0],"exit") || !strcmp(argv[0],"bye"))
	{
		write_history (NULL);
		history_truncate_file(NULL,100);
		exit(0);
	}
	else if(!strcmp(argv[0],"jobs"))
	{
		if(argv[1]==NULL)
		{
			printJobs();
			return 1;
		}
	}
	else if(!strcmp(argv[0],"fg"))
	{
		int id;
		if(argv[1]==NULL)
			id = --globalId;
		else
			id = atoi(argv[1]);
		job j = delete(id);
		if(j.id==-1)
		{
			printf("bash: fg: %d: no such job\n",id);
			return 1;
		}
		printf("%s\n",j.cmd,j.pid);
		int child_status;
		strcpy(globalCmd,j.cmd);
		globalChild = j.pid;
		kill(j.pid,SIGCONT);
		setpgid(j.pid,getpgid(getpid()));
		waitpid(j.pid,&child_status,WUNTRACED);
		return 1;
	}
	else if(!strcmp(argv[0],"bg"))
	{
		int id;
		if(argv[1]==NULL)
			id = globalId-1;
		else
			id = atoi(argv[1]);
		job j = update(id);
		if(j.id==-1)
		{
			printf("bash: bg: %d: no such job\n",id);
			return 1;
		}
		/*if(!strcmp(j.status,"Running"))
		{
			printf("Job %d already running!!\n",j.id);
			return 1;
		}*/
		printf("%s\n",j.cmd,j.pid);
		int child_status;
		strcpy(globalCmd,j.cmd);
		globalChild = j.pid;
		kill(j.pid,SIGCONT);
		return 1;
	}
	else if(!strcmp(argv[0],"cd"))
	{
		char path[1024];
		if(argv[1]==NULL)
			strcpy(path,getenv("HOME"));
		else
			strcpy(path,argv[1]);
		int r = chdir(path);
		if(r!=0)
			printf("No Such Directory.\n");
		return 1;
	}
	else if(!strcmp(argv[0],"mkdir"))
	{
		if(argv[1]==NULL)
			printf("Missing Operand.\n");
		else
		{
			mkdir(argv[1],0777);
		}
		return 1;
	}
	else if(!strcmp(argv[0],"rmdir"))
	{
		if(argv[1]==NULL)
			printf("Missing Operand.\n");
		else
		{
			int r=rmdir(argv[1]);
			if(r!=0)
			printf("No Such Directory.\n");
		}
		return 1;
	}
	else if(!strcmp(argv[0], "history"))
	{
		HIST_ENTRY **the_list;
		int i;
		the_list = history_list ();
		if(the_list)
			for (i = 0; the_list[i]; i++)
				printf ("%d: %s\n", i + history_base, the_list[i]->line);
		return 1;
	}
	return 0;
}

void runcmd(char *cmd)
{
	char *argv[MAX_ARGS];
	pid_t child_pid;
	int isBg=0;
	int child_status;
	int noOfArg = parsecmd(cmd,argv);
	if(argv[noOfArg-2][0]=='&')
	{
		argv[noOfArg-2]=NULL;
		isBg= 1;
	}
	if(builtin(argv))
	{
		globalChild = 0;
		return;
	}
	child_pid = fork();
	globalChild = child_pid;
	if(child_pid == 0) 
	{
		execvp(argv[0], argv);
		printf("bash: %s: command not found\n",argv[0]);
		exit(0);
	}
	else 
	{
		if(!isBg)
		{
			waitpid(child_pid,&child_status,WUNTRACED);
			globalChild = 0;
		}
		else
		{
			setpgid(globalChild,0);
			job j;
			j.id = globalId;
			j.pid = child_pid;
			strcpy(j.cmd,globalCmd);
			strcpy(j.status,"Running");
			insert(j);
			printf("[%d] %d\t\t\t\n",globalId,child_pid);
			globalId++;
		}

	}
}

int fork_pipes (int n, char *cmd[])
{
	int i;
	pid_t pid;
	int in, fd [2];
	int backupFd[2];
	/*dup2 (0, backupFd[0]);
	dup2 (1, backupFd[1]);*/
	backupFd[0]=dup(0);
	backupFd[1]=dup(1);
	in = 0;
	for (i = 0; i < n - 1; ++i)
    {
		pipe (fd);
		if(in!=0)
		{
			dup2 (in, 0);
			close (in);
		}
		
		//fprintf(stderr,"Claaed22%s\n",cmd[i]);
		//----------
		char* pPosition = strchr(cmd[i], '<');
		char* pPosition2 = strchr(cmd[i], '>');
		if(pPosition!=NULL && pPosition2!=NULL)
		{
			pPosition[0]=0;
			pPosition++;
			pPosition2[0]=0;
			pPosition2++;
			//printf("%s %s %s\n",pPosition,pPosition2,cmd[i]);
			trim(pPosition);
			int f = open(pPosition, O_RDONLY, 0777);
			if(f<0)
			{
				printf("%s: No such file\n",pPosition);
				dup2 (backupFd[1],1);
				dup2 (backupFd[0],0);
				return;
			}
			dup2(f,0);
			close(f);
			trim(pPosition2);
			int f2 = open(pPosition2,  O_CREAT|O_RDWR|O_TRUNC, 0777);
			dup2(f2,1);
			close(f2);
			
		}
		else if(pPosition!=NULL)
		{
			pPosition[0]=0;
			pPosition++;
			//printf("%s %spp\n",pPosition,cmd[i]);
			trim(pPosition);
			int f = open(pPosition, O_RDONLY, 0777);
			if(f<0)
			{
				printf("%s: No such file\n",pPosition);
				dup2 (backupFd[1],1);
				dup2 (backupFd[0],0);
				return;
			}
			dup2(f,0);
			dup2 (fd[1], 1);
			close(f);
			
			

		}
		else if(pPosition2!=NULL)
		{
			pPosition2[0]=0;
			pPosition2++;
			//printf("%s %s\n",pPosition,cmd[i]);
			trim(pPosition2);
			int f = open(pPosition2, O_CREAT|O_RDWR|O_TRUNC , 0777);
			dup2(f,1);
			close(f);
		}
		else
		{
			dup2 (fd[1], 1);
		}
		
		close (fd[1]);
		//-----------
		runcmd(cmd[i]);
		in = fd[0];
    }
	if(in!=0)
	{
		dup2(in,0);
		close (in);
	}
	// redirection
	char* pPosition = strchr(cmd[i], '<');
	char* pPosition2 = strchr(cmd[i], '>');
	if(pPosition!=NULL && pPosition2!=NULL)
	{
		pPosition[0]=0;
		pPosition++;
		pPosition2[0]=0;
		pPosition2++;
		//fprintf(stderr,"%s %s %sboth2\n",pPosition,pPosition2,cmd[i]);
		trim(pPosition);
		int f = open(pPosition, O_RDONLY, 0777);
		if(f<0)
		{
			printf("%s: No such file\n",pPosition);
			dup2 (backupFd[1],1);
			dup2 (backupFd[0],0);
			return;
		}
		dup2(f,0);
		close(f);
		trim(pPosition2);
		int f2 = open(pPosition2,  O_CREAT|O_RDWR|O_TRUNC, 0777);
		dup2(f2,1);//fprintf(stderr,"%d %ddede\n",f,f2);
		close(f2);
	}
	else if(pPosition!=NULL)
	{
		pPosition[0]=0;
		pPosition++;
		//fprintf(stderr,"%s %sp2\n",pPosition,cmd[i]);
		trim(pPosition);
		int f = open(pPosition, O_RDONLY, 0777);
		if(f<0)
		{
			printf("%s: No such file\n",pPosition);
			dup2 (backupFd[1],1);
			dup2 (backupFd[0],0);
			return;
		}
		dup2 (backupFd[1],1);
		dup2(f,0);
		close(f);
	}
	else if(pPosition2!=NULL)
	{
		pPosition2[0]=0;
		pPosition2++;
		//fprintf(stderr,"%s %s\n",pPosition,cmd[i]);
		trim(pPosition2);
		int f = open(pPosition2, O_CREAT|O_RDWR|O_TRUNC , 0777);
		dup2(f,1);
	}
	else
	{
		dup2 (backupFd[1],1);
		//close (backupFd[1]);
	}
	//--------
	runcmd(cmd[i]);
	dup2 (backupFd[0],0);
	dup2 (backupFd[1],1);
	close (backupFd[1]);
}

void execute(char *cmd)
{
	char *token;
	char *cmdToken[MAX_ARGS];
	strcpy(globalCmd,cmd);
	token = strtok(cmd, "|");
	int i = 0;
	while( token != NULL ) 
	{
		cmdToken[i] = malloc(256*sizeof(char));
		strcpy(cmdToken[i++],token); 
		token = strtok(NULL, "|");
	}
	fork_pipes(i,cmdToken);
}

int main()
{
    char* input, shell_prompt[100];
    rl_bind_key('\t', rl_complete);
	read_history (NULL);
	signal(SIGINT, sig_handler);
	signal(SIGTSTP, sig_handler);
	signal(SIGCHLD, sig_handler);
    while(1) 
	{
        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), getcwd(NULL, 1024));
        input = readline(shell_prompt);
		trim(input);
		if(input[0]==0)
			continue;
		if(input[0]=='!')
		{
			char **expansion;
			if(history_expand(input,expansion)==1)
			{
				strcpy(input,expansion[0]);
				printf("%s\n",input);
			}
			else
			{
				printf("Unknown event\n");
				continue;
			}
		}
		else
			add_history(input);
		execute(input);
        free(input);
    }
}

