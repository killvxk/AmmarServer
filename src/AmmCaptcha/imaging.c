
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imaging.h"

#define READ_CREATES_A_NEW_PIXEL_BUFFER 1
#define PPMREADBUFLEN 256


struct Image * createImage(unsigned int width,unsigned int height,unsigned int depth)
{
  struct Image * img = (struct Image *) malloc(sizeof(struct Image));
  if (img != 0)
  {
      img->width=width;
      img->height=height;
      img->depth=depth;

      img->pixels = (unsigned char *) malloc(sizeof(char) * width * height * depth );
      if (img->pixels!=0)
      {
          memset(img->pixels,255,sizeof(char) * width * height * depth );
      }
  }
  return img;
}

int bitBltImage(struct Image * target , unsigned int targetX,unsigned int targetY ,
                struct Image * source , unsigned int sourceX,unsigned int sourceY ,  unsigned int width , unsigned int height )
{
 if ( (target==0)||(source==0) ) { return 0; }
 if ( (target->pixels==0)||(source->pixels==0) ) { return 0; }

 unsigned int targetWidthStep = target->width * 3;
 char * targetPixelsStart   = target->pixels + ( (targetX*3) + targetY * targetWidthStep );
 char * targetPixelsLineEnd = targetPixelsStart + (width*3);
 char * targetPixelsEnd     = targetPixelsLineEnd + ((height-1) * targetWidthStep );
 char * targetPixels = targetPixelsStart;

 unsigned int sourceWidthStep = source->width * 3;
 char * sourcePixelsStart   = source->pixels + ( (sourceX*3) + sourceY * sourceWidthStep);
 char * sourcePixelsLineEnd = sourcePixelsStart  + (width*3);
 char * sourcePixelsEnd     = sourcePixelsLineEnd + ((height-1) * sourceWidthStep);
 char * sourcePixels = sourcePixelsStart;

 char * srcR, * srcG, * srcB , * tarR , * tarG , * tarB;
 do
 {
   if (targetPixels>=targetPixelsLineEnd)
   {
      targetPixelsStart+=targetWidthStep;
      targetPixelsLineEnd+=targetWidthStep;
      targetPixels=targetPixelsStart;

      sourcePixelsStart+=sourceWidthStep;
      sourcePixelsLineEnd+=sourceWidthStep;
      sourcePixels=sourcePixelsStart;
   }

   if (targetPixels < targetPixelsEnd)
   {
    srcR=sourcePixels;  srcG=sourcePixels+1;  srcB=sourcePixels+2;

    if ( (*srcR!=255) || (*srcG!=255) || (*srcB!=255) )
    {
     *targetPixels=*sourcePixels; ++targetPixels; ++sourcePixels;
     *targetPixels=*sourcePixels; ++targetPixels; ++sourcePixels;
     *targetPixels=*sourcePixels; ++targetPixels; ++sourcePixels;
    } else
    {
      sourcePixels+=3;
      targetPixels+=3;
    }
   }
 }
 while (targetPixels < targetPixelsEnd);


 return 1;
}

int ReadPPM(char * filename,struct Image * pic,char read_only_header)
{
    FILE *pf=0;
    pf = fopen(filename,"rb");

    if (pf!=0 )
    {
        char buf[PPMREADBUFLEN], *t;
        unsigned int w=0, h=0, d=0;
        int r=0;

        t = fgets(buf, PPMREADBUFLEN, pf);
        if ( (t == 0) || ( strncmp(buf, "P6\n", 3) != 0 ) ) { fclose(pf); return 0; }
        do
        { /* Px formats can have # comments after first line */
           t = fgets(buf, PPMREADBUFLEN, pf);
           if ( t == 0 ) { fclose(pf); return 0; }
        } while ( strncmp(buf, "#", 1) == 0 );
        r = sscanf(buf, "%u %u", &w, &h);
        if ( r < 2 ) { fclose(pf); return 0; }
        // The program fails if the first byte of the image is equal to 32. because
        // the fscanf eats the space and the image is read with some bit less
        r = fscanf(pf, "%u\n", &d);
        if ( (r < 1) || ( d != 255 ) ) { fclose(pf); return 0; }

        pic->width=w;
        pic->height=h;
        pic->depth=3;

        if (read_only_header) { fclose(pf); return 1; }

      #if READ_CREATES_A_NEW_PIXEL_BUFFER
	    pic->pixels = (unsigned char * ) malloc(pic->height * pic->width * 3 * sizeof(char));
	    pic->image_size = pic->height * pic->width * 3 * sizeof(char);
	  #endif


        if ( pic->pixels != 0 )
        {
            size_t rd = fread(pic->pixels,3, w*h, pf);
            fclose(pf);
            if ( rd < w*h )
            {
               return 0;
            }
            return 1;
        }
        fclose(pf);
    }
  return 0;
}

int WritePPM(char * filename,struct Image * pic)
{

    FILE *fd=0;
    fd = fopen(filename,"wb");

    if (fd!=0)
	{
     unsigned int n=0;

     fprintf(fd, "P6\n%d %d\n255\n", pic->width, pic->height);
     n = (unsigned int ) ( pic->width * pic->height ) ;

     fwrite(pic->pixels, 3, n, fd);

     fflush(fd);
     fclose(fd);

     return 1;
	}

  return 0;
}
