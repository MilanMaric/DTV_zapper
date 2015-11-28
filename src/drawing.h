#ifndef drawing_h
#define drawing_h

/* helper macro for error checking */
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}
#include <stdint.h>
#include <directfb.h>

 IDirectFBSurface *primary ;
    IDirectFB *dfbInterface ;
    int screenWidth ;
     int screenHeight;
	DFBSurfaceDescription surfaceDesc;
	 int initialized;

void setProgramParams(int32_t* argci, char*** argvi);
void drawTextInfo(int32_t service_number);
#endif