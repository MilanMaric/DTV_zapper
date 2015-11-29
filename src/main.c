#include <stdio.h>
#include <directfb.h>
#include <stdint.h>
#include "remote.h"
#include "drawing.h"
/* helper macro for error checking */


int32_t main(int32_t argc, char** argv)
{
       
    /* initialize DirectFB */
    
	DFBCHECK(DirectFBInit(&argc, &argv));
   
	
    
    /* rectangle drawing */
    
    DFBCHECK(primary->SetColor(primary, 0x03, 0x03, 0xff, 0xff));
    DFBCHECK(primary->FillRectangle(primary, screenWidth/5, screenHeight/5, screenWidth/3, screenHeight/3));
    
    
	/* line drawing */
    
	DFBCHECK(primary->SetColor(primary, 0xff, 0x80, 0x80, 0xff));
	DFBCHECK(primary->DrawLine(primary,
                               /*x coordinate of the starting point*/ 150,
                               /*y coordinate of the starting point*/ screenHeight/3,
                               /*x coordinate of the ending point*/screenWidth-200,
                               /*y coordinate of the ending point*/ (screenHeight/3)*2));
	
    
	/* draw text */

	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
    /* specify the height of the font by raising the appropriate flag and setting the height value */
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 48;
	
    /* create the font and set the created font for primary surface text drawing */
	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));
    
    /* draw the text */
	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ "Text Example",
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 100,
                                 /*y coordinate of the lower left corner of the resulting text*/ 100,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
	
    
	/* draw image from file */
    
	IDirectFBImageProvider *provider;
	IDirectFBSurface *logoSurface = NULL;
	int32_t logoHeight, logoWidth;
	
    /* create the image provider for the specified file */
	DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, "dfblogo_alpha.png", &provider));
    /* get surface descriptor for the surface where the image will be rendered */
	DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
    /* create the surface for the image */
	DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &logoSurface));
    /* render the image to the surface */
	DFBCHECK(provider->RenderTo(provider, logoSurface, NULL));
	
    /* cleanup the provider after rendering the image to the surface */
	provider->Release(provider);
	
    /* fetch the logo size and add (blit) it to the screen */
	DFBCHECK(logoSurface->GetSize(logoSurface, &logoWidth, &logoHeight));
	DFBCHECK(primary->Blit(primary,
                           /*source surface*/ logoSurface,
                           /*source region, NULL to blit the whole surface*/ NULL,
                           /*destination x coordinate of the upper left corner of the image*/50,
                           /*destination y coordinate of the upper left corner of the image*/screenHeight - logoHeight -50));
    
    
    /* switch between the displayed and the work buffer (update the display) */
	DFBCHECK(primary->Flip(primary,
                           /*region to be updated, NULL for the whole surface*/NULL,
                           /*flip flags*/0));
    
    
    /* wait 5 seconds before terminating*/
	sleep(5);
	
    
    /*clean up*/
    pthread_t thread_id;    
	pthread_create(&thread_id,NULL,&remote_control_thread,NULL);
	pthread_join(thread_id,NULL);
	
	
	primary->Release(primary);
	dfbInterface->Release(dfbInterface);

    
    //TODO 1: display the keycode of the button on the remote each time any of the buttons are pressed
    //        add background and a frame to the keycode diplay
    //        the displayed keycode should disappear after 3 seconds 
	
    //TODO 2: add fade-in and fade-out effects
    //TODO 3: add animation - the display should start at the left part of the screen and at the right
    //TODO 4: add support for displaying up to 3 keycodes at the same time, each in a separate row

    return 0;
}
