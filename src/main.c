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

    /*clean up*/
    pthread_t remote_control_thread;
    pthread_create(&remote_control_thread, NULL, &remote_control_thread, NULL);

    primary->Release(primary);
    dfbInterface->Release(dfbInterface);

    pthread_join(remote_control_thread, NULL);
    return 0;
}
