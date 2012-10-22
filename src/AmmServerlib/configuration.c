#include "configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_tools.h"


/*
   THERE ARE TWO KIND OF CONFIGURATION OPTIONS , SOME THAT ARE CONSTANT , AND DEFINED IN CONFIGURATION.H
   AND SOME THAT CAN BE MODIFIED ON RUNTIME WHICH YOU CAN SEE HERE..
*/
int PASSWORD_PROTECTION=0;
char * USERNAME=0;
char * PASSWORD=0;
char * BASE64PASSWORD=0;

int CHANGE_PRIORITY=-10;

int varSocketTimeoutREAD_ms=10*1000;
int varSocketTimeoutWRITE_ms=10*1000;

int AccessLogEnable=0;
char AccessLog[MAX_FILE_PATH]="access.log";

int ErrorLogEnable=0;
char ErrorLog[MAX_FILE_PATH]="error.log";


char TemplatesInternalURI[MAX_RESOURCE]="_asvres_/";
//Please note that the file server limits filenames _asvres_/filename.jpg is OK
//a filename like _asvres_/filenamemplampla.jpg will return a 404

// ------------------------------------------------------------------------------------------------------





int LoadConfigurationFile(char * conf_file)
{
  /*TODO : Stub*/
  fprintf(stderr,"LoadConfigurationFile(%s) not implemented yet\n",conf_file);
  return 0;
}

/*Kind of twisted function TODO : improve it :P*/
int AssignStr(char ** dest , char * source)
{
    //We want to replace **dest with a pointer to a valid point
    //in memory that contains a copy of source.. :P

   //Beginning we would like to free any allocation and set dest to point at null!
   if (*dest!=0) { free(*dest); }
   *dest=0;
   //If the source is null , so is the dest
   if (source==0) { return 1; }

   unsigned int source_len=strlen(source);
   *dest = malloc(sizeof(char) * (source_len+1) );

   if (*dest==0) { fprintf(stderr,"Could not allocate string to assign value\n"); return 0; }
   char * destination = *dest;

   destination[0]=0;
   strncpy(destination,source,source_len);
   destination[source_len]=0; // <- This should happen automatically but we reinforce it here..!

   return 1;
}


int SetUsernameAndPassword(char * username,char * password)
{

  unsigned int pass_size = 2; // : and \0
  if ( username !=0 ) { pass_size += strlen(username); }
  if ( password !=0 ) { pass_size += strlen(password); }

  if ( pass_size>MAX_QUERY ) { fprintf(stderr,"Error : SetUsernameAndPassword was given a huge string to convert..!"); return 0; }

  char * mixed_string = malloc(sizeof (char) * pass_size );
  if (mixed_string==0) { fprintf(stderr,"Error : Could not allocate memory in SetUsernameAndPassword\n"); return 0; }

  mixed_string[0]=0;
  if (username!=0) { strcpy(mixed_string,username); }
  strcat(mixed_string,":");
  if (password!=0) { strcat(mixed_string,password); }



  char * base64pass = malloc(sizeof (char) *  ((pass_size*2)+1 ));
  if (base64pass==0) { fprintf(stderr,"Error : Could not allocate memory in SetUsernameAndPassword\n"); return 0; }
  base64pass[0]=0;

  int result=encodeToBase64(mixed_string,strlen(mixed_string),base64pass,pass_size*2);
  if (result)
   { fprintf(stderr,"\nUsername and Password %s converted to %s \n",mixed_string,base64pass);
     AssignStr(&BASE64PASSWORD,base64pass);
     PASSWORD_PROTECTION=1;
   } else
   { fprintf(stderr,"\nCould not encode Username and Password %s \n",mixed_string); }

   free(mixed_string);
   free(base64pass);

 return result;
}
