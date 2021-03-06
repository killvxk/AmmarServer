/*
AmmarServer , main executable

URLs: http://ammar.gr
Written by Ammar Qammaz a.k.a. AmmarkoV 2012

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../../InputParser/InputParser_C.h"
#include "../../AmmClient/AmmClient.h"
#include "allAmmMessages.h"



int main(int argc, char *argv[])
{
    printf("Client starting up I will now emitting bogus move messages..\n");

    if (argc<3)
    {
       fprintf(stderr,"Please call with parameters.. \nLike : \n");
       fprintf(stderr,"./AmmMessagesTestClient 127.0.0.1 8080\n");
       return 1;
    }

    struct AmmClient_Instance * connection         =  AmmClient_Initialize(argv[1],atoi(argv[2]),30 /*Seconds of timeout*/);
    struct AmmClient_Instance * spammingConnection =  AmmClient_Initialize(argv[1],atoi(argv[2]),30 /*Seconds of timeout*/);
    if (connection!=0)
    {
         while ( 1 )
           {
             moveStatic.velocityX        = (float) (rand()%1000);
             moveStatic.velocityY        = (float) (rand()%1000);
             moveStatic.orientationTheta = (float) (rand()%1000);



             printf("Sending ");
              print_move(&moveStatic);
             printf("\n");

             unsigned int startTime = AmmClient_GetTickCountMilliseconds();
             if (  sendToServer_move(connection , &moveStatic) )
              {
                  fprintf(stderr,GREEN "sendToServer_move Success.. \n" NORMAL);
              } else
              {
                  fprintf(stderr,RED "sendToServer_move Failed.. \n" NORMAL);
              }
             unsigned int endTime = AmmClient_GetTickCountMilliseconds();

             if ((endTime-startTime)!=0)
              { fprintf(stderr,"sendToServer_move: Achieved a rate of %0.2f Hz \n",(float) (1000/(endTime-startTime) ) ); }
             //sleep(1);

             //Lets forget it..!
             moveStatic.velocityX        = (float) (rand()%1000);
             moveStatic.velocityY        = (float) (rand()%1000);
             moveStatic.orientationTheta = (float) (rand()%1000);

             startTime = AmmClient_GetTickCountMilliseconds();
              if ( readStateFromServer_move(spammingConnection,&moveStatic) )
              {
                  fprintf(stderr,GREEN "readStateFromServer_move Success.. \n" NORMAL);
              } else
              {
                  fprintf(stderr,RED "readStateFromServer_move Failed.. \n" NORMAL);
              }
              endTime = AmmClient_GetTickCountMilliseconds();

             if ((endTime-startTime)!=0)
              { fprintf(stderr,"readStateFromServer_move: Achieved a rate of %0.2f Hz \n",(float) (1000/(endTime-startTime) ) ); }


             printf("What we have now");
              print_move(&moveStatic);
             printf("\n");
           }

     AmmClient_Close(connection);
    }

    return 0;
}
