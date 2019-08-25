#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h>
#include<dirent.h>
#include<error.h>
#include<limits.h> 
#include<sys/types.h> 
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/stat.h>
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h>
#include<ctype.h>
 
char cwd[1024], h_name[1024], *uname,inp[1024],cmd[1024];
char HOME[1024],*com;
char til[1024]="~";
int no_of_tokens_space,no_of_tokens_com,i,j;
int alpha[200];

typedef struct token
{
	char arr[100];	
}token;

token tokens_space[100],tokens_com[100];
void  tokenise_space(char *str);
void tokenise_com(char *str);
void cd(char *a,char *b);
void execute(char *a);
int pinfo(char *pid);
void ls(int len);
void ls_l(char *fname);
char * presentDir() 
{ 
    getcwd(cwd, sizeof(cwd));
    return cwd;  
} 

char * Hostname()
{
	gethostname(h_name,sizeof(h_name));
	return h_name;
}

char * Username()
{
	uname=getenv("USER");
	return uname;
}

void clear()
{
	printf("\033[H\033[J");
}
void ls(int no_token)
{
	//send length as l-1
	struct dirent *dp;
        DIR *dir;
        int check = 0;
	char temp[1024];
	if(no_token == 0)
	{
		check = 1;
		dir = opendir(".");
	}
	else
	{
		int i = 0;
		while(tokens_space[i].arr[0] != '\0')
		{
			if(tokens_space[i].arr[0] == '-')
			{
				int j = 1;
				while(tokens_space[i].arr[j] != '\0')
				{
					alpha[tokens_space[i].arr[j]] = 1;
					j++;
				}
			}
			else
			{
				check = 2;
				//printf("here");
				getcwd(temp,sizeof(temp));
				//printf("here2");
				cd(tokens_space[i].arr,HOME);
				//printf("here 3");
				dir = opendir(".");
			}
			i++;
		}
	}
	if(check == 0)
	{
		dir = opendir(".");
	}
	while((dp = readdir(dir)) != NULL)
        {
                if(alpha['a'] == 1)
                {
                        if(alpha['l'] == 1)
                                ls_l(dp->d_name);
                        else
                                printf("%s\n", dp->d_name);
                }
                else if (alpha['a'] == 0)
                {
                        if(dp->d_name[0] != '.')
                        {
                                if(alpha['l'] == 1)
                                {
                                        ls_l(dp->d_name);
                                }
                                else
                                        printf("%s\n", dp->d_name);
                        }
                }

        }
	if(check == 2)
	{
		cd(temp,HOME);
	}
	printf("\n");
	closedir(dir);
}
void ls_l(char *fname)
{	
	struct stat fs;
        if(stat(fname, &fs) < 0)
                return;
        struct passwd *pws;
        pws = getpwuid(fs.st_uid);
        struct group *grp;
        grp = getgrgid(fs.st_gid);
        printf( (S_ISDIR(fs.st_mode)) ? "d" : "-");
        printf( (fs.st_mode & S_IRUSR) ? "r" : "-");
        printf( (fs.st_mode & S_IWUSR) ? "w" : "-");
        printf( (fs.st_mode & S_IXUSR) ? "x" : "-");
        printf( (fs.st_mode & S_IRGRP) ? "r" : "-");
        printf( (fs.st_mode & S_IWGRP) ? "w" : "-");
        printf( (fs.st_mode & S_IXGRP) ? "x" : "-");
        printf( (fs.st_mode & S_IROTH) ? "r" : "-");
        printf( (fs.st_mode & S_IWOTH) ? "w" : "-");
        printf( (fs.st_mode & S_IXOTH) ? "x" : "-");
        printf("%4ld", fs.st_nlink);
        printf("%12s", pws->pw_name);
        printf("%12s", grp->gr_name);
        printf("%9ld", fs.st_size);
        char time[100];
        strcpy(time, ctime(&fs.st_mtime));
        time[strlen(time) - 1] = '\0';
        printf("%22s ",time);
        printf("%11s ", fname);
        printf("\n");
}
void tokenise_com(char *str)
{
	char delim=';';
	// for(int j=0;j<100;j++)
	// {
	// 	memset(tokens_com[j].arr,0,sizeof(tokens_com[j].arr));
	// }

	char *temp;
	int p;
	no_of_tokens_com=0;
	temp=strtok(str,";");

	while(temp!=NULL)
	{	
		// printf("%s\n",temp );
		strcpy(tokens_com[no_of_tokens_com].arr,temp);
		temp=strtok(NULL,";");
		no_of_tokens_com++;
	}
	
	for(i=0;i<no_of_tokens_com;i++)
	{
		tokenise_space(tokens_com[i].arr);
	}
	
}

void  tokenise_space(char *str)
{
	char delim1=' ';
	for(int j=0;j<100;j++)
	{
		memset(tokens_space[j].arr,0,sizeof(tokens_space[j].arr));
	}

	char *temp,cpy[1024];
	int p;
	no_of_tokens_space=0;
	strcpy(cpy,str);
	temp=strtok(str, " ");
	//printf("%s\n",cpy);
	while(temp!=NULL)
	{	
		strcpy(tokens_space[no_of_tokens_space].arr,temp);
		temp=strtok(NULL," ");
		no_of_tokens_space++;
	}

	if(strcmp(tokens_space[0].arr,"pwd")==0)			//pwd
	{
		com=presentDir();
		printf("%s\n",com);
	}
	else if (strcmp(tokens_space[0].arr,"c") == 0 || strcmp(tokens_space[0].arr,"clear") == 0)
	{
		clear();
		/* code */
	}
	
	else if(strcmp(tokens_space[0].arr,"echo")==0)    //echo
	{
		for(i=1;i<no_of_tokens_space;i++)
		{
			printf("%s ",tokens_space[i].arr);
		}
		printf("\n");
	}
	else if(strcmp(tokens_space[0].arr,"cd")==0)     //cd
	{
		cd(tokens_space[1].arr,HOME);
	}
    	else if(strcmp(tokens_space[0].arr,"pinfo")==0)
	{
		char * sad;
		if(no_of_tokens_space==1)
		{

			char curpid[1024];
			snprintf(curpid,1024,"%d",getpid());
			pinfo(curpid);
		}
		else
		{
			pinfo(tokens_space[1].arr);
		}
	}
	else if(strcmp(tokens_space[0].arr,"ls")==0)
	{
		//printf("ls");
		for(int i = 0; i<200; i++)
		{
			alpha[i] = 0;
		}
		ls(no_of_tokens_space - 1);
	}//
	else
	{
		execute(cpy);
	}
	//return tokens_space;
}

int pinfo(char *pid)
{
        FILE *statusf; 
        char path[50], line[100], *p;
        snprintf(path, 50, "/proc/%s/status", pid);
        statusf = fopen(path, "r");
        if(!statusf)
                return 0; 
        while(fgets(line, 100, statusf)) { 
                if(strncmp(line, "State:", 6) == 0)
                {
                        p = line + 7;
                        while(isspace(*p)) 
                        {
                                p++;
                        }
                        printf("Process Status : %s", p);
                }
                else if(strncmp(line, "Pid:", 4) == 0)
                {
                        p = line + 5;
                        while(isspace(*p))
                        { 
                                p++;
                        }
                        printf("pid : %s", p);
                }
                else if(strncmp(line, "VmSize:",7) == 0)
                {
                        p = line + 8;
                        while(isspace(*p))
                        {
                                p++;
                        }
                        printf("memory : %s", p);
                }
                else
                {
                        continue;
                }
        }
        char outf[1024],exec[50];
        snprintf(exec, 50, "/proc/%s/exe", pid);
        readlink(exec, outf, 1024);
        printf("Executable Path : %s\n", outf);
        fclose(statusf);
}

void handler(int sig)
{
	pid_t pid;

	pid = wait(NULL);
	if(pid>0)
		fprintf(stderr," \n Emacs with Pid %d exited normally.\n", pid);
	//printf("Pid %d exited.\n", pid);
}

void execute(char *a)
{
	char *args[100],*qwe;
	int k=0,var=0,status,m;
	pid_t c_pid,wait_pid;
	qwe=strtok(a, " ");
	int check = 0;
	//printf("%s\n",cpy);
	while(qwe!=NULL)
	{	
		//strcpy(tokens_space[no_of_tokens_space].arr,temp);
		if(strcmp(qwe,"&") != 0)
		{
			args[k]=qwe;
			k++;
			check = 1;
		}	
		qwe=strtok(NULL," ");
	}
	args[k]=NULL;
	signal(SIGCHLD, handler);
	c_pid = fork();
	if(c_pid == 0)
	{
		if(execvp(args[0],args)<0)
		{
			fprintf(stderr, "Error:'%s' cannot find command\n",args[0]);
		}
	}
	else if ( c_pid >0)
	{
		if(check == 0)
		{
			wait_pid = waitpid(c_pid,&status,0);
		}
	}
	else
	{
		fprintf(stderr, "Error: cannot create process\n");
	}
	// return;
}

void prompt(char *a,char *b,char *c)
{
	printf("\033[1;36m");//Magenta Colour
	printf("%s@%s:",a,b);
	printf("\033[1;32m");//Green
	printf("%s> ",c);
	printf("\033[0;31m");//Red 
}

void cd(char *a,char *b)
{
	char * z,*x;
	char some[1024]="~",a1[1024];
	int y,w;
	if(a[0]=='~')		
	{
		chdir(b);
		w=0;
		for(y=2;y<strlen(a);y++)
		{
			a1[w]=a[y];
			w++;
		}
		a1[w]='\0';
		chdir(a1);
	}
	else
	{
		chdir(a);
	}
	x=presentDir();
	z=strstr(x,b);
	if(z&&(strlen(x)!=strlen(b)))
	{
		w=1;
		for(y=strlen(b);y<strlen(x);y++)
		{
			some[w]=x[y];
			w++;
		}
		some[w] = '\0';
		strcpy(til,some);
	}
	else if (strlen(x)==strlen(b))
	{
		strcpy(til,some);
	}
	else
	{
		strcpy(til,x);
	}
}

int main ()
{
	clear(); 
	char *hostname,*username;
	// token *tok,*tik;
	int t,z;
	hostname=Hostname();
	username=Username();
	presentDir();
	strcpy(HOME,cwd);
	printf("%s\n",HOME);
	while(1)
	{
		prompt(username,hostname,til);
		fgets(inp,1024,stdin);
		strcpy(cmd,inp);
		// for(i=0;i<strlen(inp);i++)
		// {
		// 	if(inp[i]=='"')
		// 	{
		// 		memmove(&inp[i], &inp[i + 1], strlen(inp) - i);
		// 	}
		// }
		for(z=strlen(inp)-1;z>=0;z--)
		{
			if(inp[z]!=' '&&inp[z]!='\n')
			{
				break;
			}
		}
		inp[z+1]='\0';
		tokenise_com(inp);	
	}
}
