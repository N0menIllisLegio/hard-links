/*
 Написать программу, находящую в заданном каталоге (первый аргумент командной строки) и всех его
 подкаталогах все файлы заданного расширения и создающий для каждого найденного файла жесткую ссылку в заданном каталоге.
 Расширение файла и каталог для жестких ссылок задаются в качестве второго и третьего аргументов командной строки.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>

char *res;
int reti;
int newfilename = 1;
char buf[PATH_MAX];
char *dp;
char *convfp;
char *stroutput;
regex_t _regex;

regex_t extensionToRegexp(char *ext)
{
    regex_t regex;
    char *regexp = alloca(strlen(ext) + 3);
    strcpy(regexp, "\\.");
    strcat(regexp, ext);
    strcat(regexp, "$");
    
    reti = regcomp(&regex, regexp, 0);
    if( reti )
    {
        printf("Could not compile regex\n");
        exit(errno);
    }
    
    return regex;
}

void MYreverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
     {
	     c = s[i];
	     s[i] = s[j];
	     s[j] = c;
     }
 }

void MYitoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0) 
         n = -n;          
     i = 0;
     do {       
         s[i++] = n % 10 + '0';   
     } while ((n /= 10) > 0);     
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';

     MYreverse(s);
}

int createHardLink(char *savedir, char *fullfilepath, char *filename)
{
    char *savedircopy = alloca(PATH_MAX);
    char *buffer = alloca(3);
    strcpy(savedircopy, savedir);
    strcat(savedircopy, "/");
    strcat(savedircopy, filename);
    strcat(savedircopy, "_");
    MYitoa(newfilename, buf);
    strcat(savedircopy, buf);
    newfilename++;
    
    int error = link(fullfilepath, savedircopy);

    if (error == -1)
    {
    	printf("l2_v2.c: Link cannot be created. Permission denied. %s\n", fullfilepath);
    	return 2;
    }
    
    return 0;
}

int findAnswer(char *currdir)
{
    DIR *d;
    struct dirent *dir;
    
    if (d = opendir(currdir)) 
    {
    	while (d)
    	{
    		errno = 0;
    		if ((dir = readdir(d)) != NULL)
	        {
	            if (
	                (dir->d_type == DT_DIR)
	                && ((dir->d_name[0] != '.') && (strlen(dir->d_name) != 1))
	                && ((dir->d_name[0] != '.') && (dir->d_name[1] != '.') && (strlen(dir->d_name) != 2))
	               )
	            {
	                stroutput = malloc(strlen(currdir) + 50);
	                
	                strcpy(stroutput, currdir);
	                strcat(stroutput, "/");
	                strcat(stroutput, dir->d_name);

	                findAnswer(stroutput);

	            }
	            else
	            {
	                if (dir->d_type == DT_REG)
	                {
	                    reti = regexec(&_regex, dir->d_name, 0, NULL, 0);
	                    if( !reti )
	                    {                        
	                        stroutput = malloc(strlen(currdir) + 50);
	                        
	                        strcpy(stroutput, currdir);
	                        strcat(stroutput, "/");
	                        strcat(stroutput, dir->d_name);
	                        
	                        res = realpath(stroutput, buf);
	                        free(stroutput);
	                        
	                        if (res)
	                        {
	                            strcpy(convfp, buf);
	                            createHardLink(dp, convfp, dir->d_name);
	                        } else {
	    						printf("l2_v2.c: %s\n", strerror(errno));
	    						printf("\t%s\n", stroutput);
	                        }
	                    }
	                }
	            }
	        }
	        else
	        {
	        	if (errno != 0)
	        	{
	        		printf("l2_v2.c: Cannot read directory");
    				printf("\t%s\n", currdir);
    				
	        	}
	        	break;
	        }
    	}
        if (closedir(d) == -1)
        {
        	printf("l2_v2.c: Cannot close directory");
    		printf("\t%s\n", currdir);
        }
    }
    else
    {
    	printf("l2_v2.c: Cannot open directory");
    	printf("\t%s\n", currdir);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("l2_v2.c: Wrong number of arguments!\n");
        return 2;
    }
    
    int error = mkdir(argv[1], 0700);
    
    if (error == -1)
    {
        if (errno == EEXIST)
        {
            printf("l2_v2.c: Such directory already exists.\n");
        } else
        {
            printf("l2_v2.c: Can't create such directory.\n");
            return 2;
        }
    }
    
    dp = malloc(PATH_MAX + 1);
    convfp = malloc(PATH_MAX + 1);
    
    realpath(argv[1], buf);
    
    strcpy(dp, buf);
    
    _regex = extensionToRegexp(argv[2]);
    
    findAnswer(argv[3]);

    return 0;
}


