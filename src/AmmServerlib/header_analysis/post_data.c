#include "post_data.h"
#include "generic_header_tools.h"

#include "../tools/http_tools.h"

#include <stdio.h>
#include <string.h>

int wipePOSTData(struct HTTPHeader * output)
{
  output->POSTItemNumber=0;
  memset(output->POSTItem,0,MAX_HTTP_POST_BOUNDARY_COUNT*sizeof(struct POSTRequestBoundaryContent));
  return 1;
}

int createPOSTData(struct HTTPHeader * output)
{
  return (wipePOSTData(output));
}

int addPOSTDataBoundary(struct HTTPHeader * output,char * ptr)
{
  unsigned int n=0;
  if (output->POSTItemNumber>=MAX_HTTP_POST_BOUNDARY_COUNT)
  {
   AmmServer_Warning("Surpassed the maximum number of POST boundaries acceptable ( MAX_HTTP_POST_BOUNDARY_COUNT = %u ), this is a compile-time setting \n",MAX_HTTP_POST_BOUNDARY_COUNT);
   return 0;
  } else
  {
    n=output->POSTItemNumber;
    ++output->POSTItemNumber;
  }

   //Get rid of New line..
   if (*ptr==13) { ++ptr; }
   if (*ptr==10) { ++ptr; }
   if (*ptr==13) { ++ptr; }
   if (*ptr==10) { ++ptr; }

   output->POSTItem[n].pointerStart=ptr;
   output->POSTItem[n].pointerEnd=ptr;
   output->POSTItem[n].contentSize=0;
   output->POSTItem[n].contentType=0;

   //------------------------------------------
   output->POSTItem[n].name=0;
   output->POSTItem[n].nameSize=0;

   output->POSTItem[n].value=0;
   output->POSTItem[n].valueSize=0;


   output->POSTItem[n].filename=0;
   output->POSTItem[n].filenameSize=0;

   output->POSTItem[n].contentDisposition=0;
   output->POSTItem[n].contentDispositionSize=0;

   output->POSTItem[n].contentType=0;
   output->POSTItem[n].contentTypeSize=0;

   output->POSTItem[n].reallocateOnHeaderRAWResize=1;
  return 1;
}


int finalizePOSTData(struct HTTPHeader * output)
{
 //TODO : This calls inserts garbage in data..!
 fprintf(stderr,"finalizePOSTData POSTItems : %p , %u items\n",output->POSTItem , output->POSTItemNumber);

 unsigned int success=0;
 unsigned int i=0;
 unsigned int PNum=output->POSTItemNumber;
 if (PNum>MAX_HTTP_POST_BOUNDARY_COUNT) { PNum=MAX_HTTP_POST_BOUNDARY_COUNT; }

 for (i=0; i<PNum; i++)
 {
  //fprintf(stderr,"output->POSTrequestSize=%u\n",output->POSTrequestSize);
  //fprintf(stderr,"output->POSTrequestBodySize=%u\n",output->POSTrequestBodySize);
  //AmmServer_Success("finalizePOSTData(%u)=`%s`\n",i,output->POSTItem[i].pointerStart);
  unsigned int length=0;
  char * configuration = output->POSTItem[i].pointerStart;
  char * payload = reachNextBlock(
                                   output->POSTItem[i].pointerStart,
                                   _calculateRemainingDataLength(
                                                                  output->headerRAW ,
                                                                  output->headerRAWSize ,
                                                                  output->POSTItem[i].pointerStart
                                                                )  ,
                                   &length
                                   ,1 //Null termination
                                  );
  reachNextLine( payload+1,
                 _calculateRemainingDataLength(
                                               output->headerRAW ,
                                               output->headerRAWSize ,
                                               payload+1
                                              ),
                &length,
                0//THIS NEEDS TO BE SET TO 0 OTHERWISE THIS BLEEDS IN THE DATA AND ADDS A NULL
               );

  unsigned int configurationLength = payload-configuration;

  //AmmServer_Warning("configuration(%u)=`%s`\n",i,configuration);
  //AmmServer_Success("payload(%u)=`%s`\n",i,payload);

  char * filename = strstr(configuration,"filename=\""); //TODO : use memmem
  if (filename!=0)
  {
    output->POSTItem[i].filename = filename+10; //skip filename="
    output->POSTItem[i].filenameSize = countStringUntilQuotesOrNewLine(
                                                                        output->POSTItem[i].filename,
                                                                        configurationLength ,
                                                                        1 //Null termination
                                                                       );


    char * name = strstr(configuration,"name=\""); //TODO : use memmem
    if (name!=0)
     {
       output->POSTItem[i].name = name+6; //skip name="
       output->POSTItem[i].nameSize = countStringUntilQuotesOrNewLine(
                                                                       output->POSTItem[i].name,
                                                                       configurationLength,
                                                                       1 // Null termination
                                                                       );
     }

       output->POSTItem[i].value = payload;
       //TODO : output->boundary value is wrong..
       //AmmServer_Success("Searching for boundary (%s) in file payload..!",output->boundary);
       //fprintf(stderr,"Searching for boundary that points to %p \n",output->boundary); //Do not print all the file here..


       unsigned int payloadSize = _calculateRemainingDataLength
                                              (
                                               output->headerRAW ,
                                               output->headerRAWSize ,
                                               payload
                                              );

/*
       fprintf(stderr,"Payload (%p) size %u / Boundary(%p) size %u \n",
                                     payload,
                                     payloadSize ,
                                     output->boundary ,
                                     output->boundaryLength);*/

       //TODO : This is not perfect..! output->boundary contains fewer chars

       char * payloadEnd  = memmem(
                                     payload,
                                     payloadSize ,
                                     output->boundary ,
                                     output->boundaryLength
                                    );

       if (payloadEnd!=0)
       {
        output->POSTItem[i].valueSize=payloadEnd-payload;
        //AmmServer_Success("Found boundary in file payload, size of payload is %u ..!",output->POSTItem[i].valueSize);
       } else
       {
        AmmServer_Error("Could not detect boundary in file payload, using unsafe length value..!");
        output->POSTItem[i].valueSize= _calculateRemainingDataLength
                                              (
                                               output->headerRAW ,
                                               output->headerRAWSize ,
                                               output->boundary
                                              );
       }
  } else
  {
    char * name = strstr(configuration,"name=\""); //TODO : use memmem
    if (name!=0)
     {
       output->POSTItem[i].name = name+6; //skip name="
       output->POSTItem[i].nameSize = countStringUntilQuotesOrNewLine(
                                                                        output->POSTItem[i].name,
                                                                        configurationLength,
                                                                        1 // Null termination
                                                                     );
     }

    if (payload!=0)
     {
       output->POSTItem[i].value = payload;
       output->POSTItem[i].valueSize=length;
     }

    if (name!=0)
     {
      //fprintf(stderr,"%s=%p\n",output->POSTItem[i].name,output->POSTItem[i].value);
     } else
     {
       AmmServer_Warning("Incorrect name for boundary part %u/%u marking it as empty..\n",i,PNum);
       output->POSTItem[i].value = 0;
       output->POSTItem[i].valueSize=0;
     }
  }
 }

 return (success!=PNum);
}




/*
----------------------------------------------
              ACCESS POST DATA
----------------------------------------------
*/
const struct POSTRequestBoundaryContent * getPOSTItemFromName(struct AmmServer_DynamicRequest * rqst,const char * nameToLookFor)
{
 unsigned int sizeOfNameToLookFor = strlen(nameToLookFor);

 unsigned int i=0;
 unsigned int PNum=rqst->POSTItemNumber;
 if (PNum>MAX_HTTP_POST_BOUNDARY_COUNT) { PNum=MAX_HTTP_POST_BOUNDARY_COUNT; }

 if (rqst->POSTItem!=0)
 {
  for (i=0; i<PNum; i++)
  {
    struct POSTRequestBoundaryContent * p = &rqst->POSTItem[i];
    //AmmServer_Info("POSTItem[%u].name = %s and we have %s \n",i,p->name,nameToLookFor);
    if (p->name!=0)
    {
     if (strncmp(p->name,nameToLookFor,sizeOfNameToLookFor) == 0)
     {
       return p;
     }
    }
  }
 }
 return 0;
}


char * getPointerToPOSTItemValue(struct AmmServer_DynamicRequest * rqst,const char * nameToLookFor,unsigned int * pointerLength)
{
 const struct POSTRequestBoundaryContent * p = getPOSTItemFromName(rqst,nameToLookFor);

 if (p!=0)
 {
       //AmmServer_Success("getPointerToPOSTItemValue(%s) success => %p \n",nameToLookFor,p->value);
       *pointerLength = p->valueSize;
       return p->value;
 }

 AmmServer_Warning("getPointerToPOSTItemValue called but could not find name=`%s` \n",nameToLookFor);
 *pointerLength=0;
 return 0;
}


char * getPointerToPOSTItemFilename(struct AmmServer_DynamicRequest * rqst,const char * nameToLookFor,unsigned int * pointerLength)
{
 const struct POSTRequestBoundaryContent * p = getPOSTItemFromName(rqst,nameToLookFor);

 if (p!=0)
 {
       *pointerLength = p->filenameSize;
       return p->filename;
 }

 *pointerLength=0;
 return 0;
}



char * getPointerToPOSTItemType(struct AmmServer_DynamicRequest * rqst,const char * nameToLookFor,unsigned int * pointerLength)
{
 const struct POSTRequestBoundaryContent * p = getPOSTItemFromName(rqst,nameToLookFor);

 if (p!=0)
 {
       *pointerLength = p->contentTypeSize;
       return p->contentType;
 }

 *pointerLength=0;
 return 0;
}


int getNumberOfPOSTItems(struct AmmServer_DynamicRequest * rqst)
{
 return rqst->POSTItemNumber;
}

