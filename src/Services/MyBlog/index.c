#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "database.h"


unsigned int indexLength=0;
unsigned char * indexContent=0;

#warning "Memory Managment in MyBlog while creating a buffer is a bit shabby :P"


int injectDataToBuffer(unsigned char * entryPoint , unsigned char * data , unsigned char * buffer,  unsigned int currentBufferLength , unsigned int totalBufferLength )
{
  char * where2inject = strstr ((const char*) buffer,(const char*) entryPoint);
  if (where2inject==0) { fprintf(stderr,"Cannot inject Data to Buffer , could not find our entry point!\n"); return 0; }

 // if ()

  return 1;
}


unsigned char * getMenuListHTML(struct website * configuration)
{
  unsigned int totalSize=CONTENT_BUFFER,currentSize=0;
  unsigned char * buffer = (unsigned char*) malloc (sizeof(unsigned char) * totalSize );
  if (buffer==0) { fprintf(stderr,"Cannot allocate a big enough buffer for string"); return 0; }

  unsigned int i=0;
  for (i=0; i<configuration->menu.currentItems; i++)
  {
    //TODO PROPER MEMORY HANDLING ,, REALLOC ETC ..
    currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"<li class=\"page_item page-item-%u\"><a href=\"%s\">%s</a></li>" ,i, configuration->menu.item[i].link , configuration->menu.item[i].label);
  }

 return buffer;
}

unsigned char * getWidgetListHTML(struct website * configuration)
{
  unsigned int totalSize=CONTENT_BUFFER,currentSize=0;
  unsigned char * buffer = (unsigned char*) malloc (sizeof(unsigned char) * totalSize );
  if (buffer==0) { fprintf(stderr,"Cannot allocate a big enough buffer for string"); return 0; }

  unsigned int i=0;
  for (i=0; i<configuration->menu.currentItems; i++)
  {
    currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,
                      "<li id=\"text-%u\" class=\"widget widget_text\">\
                       <h2 class=\"widgettitle\">%s</h2>\
                       <div class=\"textwidget\">%s/div>\
		           </li>" , i , configuration->widget.item[i].label , configuration->widget.item[i].content.totalDataLength);

  }

 return buffer;
}

unsigned char * getPostListHTML(struct website * configuration)
{
  unsigned int totalSize=CONTENT_BUFFER,currentSize=0;
  unsigned char * buffer = (unsigned char*) malloc (sizeof(unsigned char) * totalSize );
  if (buffer==0) { fprintf(stderr,"Cannot allocate a big enough buffer for string"); return 0; }

  unsigned int i=0;
  for (i=0; i<configuration->menu.currentItems; i++)
  {
     currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"<div class=\"post-%u post type-post status-publish format-standard hentry category-post ", i);

     //Print Tag CSS Classes
     unsigned int z=0;
     for (z=0; z<configuration->post.item[i].tags.currentTags; z++)
     {
       currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"tag-%s ",configuration->post.item[i].tags.item[z].tag );
     }

      currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"\" id=\"post-%u\">\
	                  <div class=\"posttitle\">\
		                 <h2 class=\"pagetitle\">\
                          <a href=\"post.html?id=%u\" rel=\"bookmark\" title=\"%s\">%s</a></h2>\
		                   <small>Posted: %s by <strong>%s</strong> in <a href=\"post.html?id=%u\" title=\"View all posts in Post\" rel=\"category\">Post</a><br>\
			               " , i , i ,
	   configuration->post.item[i].title ,
	   configuration->post.item[i].title ,
	   configuration->post.item[i].dateStr ,
	   configuration->post.item[i].author ,
       i );


     currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"Tags: ");
     for (z=0; z<configuration->post.item[i].tags.currentTags; z++)
     {
       currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"<a href=\"tag.html?id=%s\" rel=\"tag\">%s</a> ", configuration->post.item[i].tags.item[z].tag , configuration->post.item[i].tags.item[z].tag);
     }

    currentSize+=snprintf(buffer+currentSize,totalSize-currentSize,"</small></div>\
	                  <div class=\"postcomments\"><a href=\"post.html?id=%u#respond\" title=\"Comment on %s..\">0</a></div>\
                      <div class=\"entry\">%s</div>\
	                 </div>"
            , i , configuration->post.item[i].title  , configuration->post.item[i].content.data );

  }

 return buffer;
}



unsigned char * getLeftBlogRollHTML(struct website * configuration)
{
    return 0;
}

unsigned char * getRightBlogRollHTML(struct website * configuration )
{
    return 0;
}

unsigned char * getFooterLinksHTML(struct website * configuration )
{
    return 0;
}


unsigned char * prepare_index_prototype(char * filename , struct website * configuration)
{
  indexContent=AmmServer_ReadFileToMemory(filename,&indexLength);
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++YEAR+++","20xx");
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++BLOGTITLE+++",configuration->blogTitle);
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++SITENAME+++",configuration->siteName);
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++SITEDESCRIPTION+++",configuration->siteDescription);
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++RSSLINK+++","rss.html");
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++RSSCOMMENT+++","rssComments.html");

  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++ATOMLINK+++","atom.html");
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++PINGBACKLINK+++","pingback.html");

  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++TAGLIST+++","no tags");
  AmmServer_ReplaceVarInMemoryFile(indexContent,indexLength,"+++ARCHIVELIST+++","no archives");

  unsigned char * htmlData = 0;
  htmlData = getMenuListHTML(configuration);
  injectDataToBuffer("+++MENULIST+++",htmlData,indexContent,indexLength,indexLength);
  if (htmlData!=0) { free(htmlData); htmlData=0; }


  htmlData = getWidgetListHTML(configuration);
  injectDataToBuffer("+++WIDGETLIST+++",htmlData,indexContent,indexLength,indexLength);
  if (htmlData!=0) { free(htmlData); htmlData=0; }

  htmlData = getLeftBlogRollHTML(configuration);
  injectDataToBuffer("+++LEFTBLOGROLL+++",htmlData,indexContent,indexLength,indexLength);
  if (htmlData!=0) { free(htmlData); htmlData=0; }

  htmlData = getRightBlogRollHTML(configuration);
  injectDataToBuffer("+++RIGHTBLOGROLL+++",htmlData,indexContent,indexLength,indexLength);
  if (htmlData!=0) { free(htmlData); htmlData=0; }

  htmlData = getFooterLinksHTML(configuration);
  injectDataToBuffer("+++FOOTERLINKS+++",htmlData,indexContent,indexLength,indexLength);
  if (htmlData!=0) { free(htmlData); htmlData=0; }

  return indexContent;
}



//This function prepares the content of  stats context , ( stats.content )
void * prepare_index(struct AmmServer_DynamicRequest  * rqst)
{

  //No range check but since everything here is static max_stats_size should be big enough not to segfault with the strcat calls!
  /*
  snprintf(rqst->content,rqst->MAXcontentSize,
           "<html><head><title>Dynamic Content Enabled</title><meta http-equiv=\"refresh\" content=\"1\"></head>\
            <body>The date and time in AmmarServer is<br><h2>%02d-%02d-%02d %02d:%02d:%02d\n</h2>\
            The string you see is updated dynamically every time you get a fresh copy of this file!<br><br>\n\
            To include your own content see the <a href=\"https://github.com/AmmarkoV/AmmarServer/blob/master/src/main.c#L46\">\
            Dynamic content code label in ammarserver main.c</a><br>\n\
            If you dont need dynamic content at all consider disabling it from ammServ.conf or by setting DYNAMIC_CONTENT_RESOURCE_MAPPING_ENABLED=0; in \
            <a href=\"https://github.com/AmmarkoV/AmmarServer/blob/master/src/AmmServerlib/file_caching.c\">file_caching.c</a> and recompiling.!</body></html>",
           tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,   tm.tm_hour, tm.tm_min, tm.tm_sec);*/

  rqst->contentSize=strlen(rqst->content);
  return 0;
}
