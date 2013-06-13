#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "applicationFiles.h"

int scanFor_applicationFiles(char * str,unsigned int strLength) 
{
 if (strLength<3) { return 0; } 

 switch (toupper(str[0])) { 
 case 'C' : 
     if (strLength<3) { return 0; } 
     if ( strncasecmp(str,"CPL",3) == 0 ) { return APPLICATIONFILES_CPL; } 
 break; 
 case 'D' : 
     if (strLength<3) { return 0; } 
     if ( strncasecmp(str,"DLL",3) == 0 ) { return APPLICATIONFILES_DLL; } 
 break; 
 case 'E' : 
     if (strLength<3) { return 0; } 
     if ( strncasecmp(str,"EXE",3) == 0 ) { return APPLICATIONFILES_EXE; } 
 break; 
 case 'P' : 
     if (strLength<3) { return 0; } 
     if ( strncasecmp(str,"PDF",3) == 0 ) { return APPLICATIONFILES_PDF; } 
 break; 
 case 'S' : 
     switch (toupper(str[1])) { 
     case 'C' : 
         if (strLength<3) { return 0; } 
         if ( strncasecmp(str,"SCR",3) == 0 ) { return APPLICATIONFILES_SCR; } 
     break; 
     case 'W' : 
         if (strLength<3) { return 0; } 
         if ( strncasecmp(str,"SWF",3) == 0 ) { return APPLICATIONFILES_SWF; } 
     break; 
    }; 
 break; 
}; 
 return 0;
}