/**
 * @file    list_fps.c
 * @brief   list function parameter structure
 */


#include "CommandLineInterface/CLIcore.h"





// ==========================================
// Forward declaration(s)
// ==========================================

errno_t list_fps();



// ==========================================
// Command line interface wrapper function(s)
// ==========================================





// ==========================================
// Register CLI command(s)
// ==========================================

errno_t list_fps_addCLIcmd()
{

    RegisterCLIcommand(
        "listfps",
        __FILE__,
        list_fps,
        "list function parameter structures (FPSs)",
        "no argument",
        "listfps",
        "errno_t list_fps()");

    return RETURN_SUCCESS;
}






errno_t list_fps()
{
    long fpsID;
    long fpscnt = 0;

    int NBchar_fpsID = 5;
    int NBchar_fpsname = 12;
    int NBchar_NBparam = 4;

    printf("\n");
    printf("%*s  %*s  %*s\n",
           NBchar_fpsID, "ID",
           NBchar_fpsname, "name",
           NBchar_NBparam, "#par"
          );
          
    for(fpsID = 0; fpsID < data.NB_MAX_FPS; fpsID++)
    {
        if(data.fps[fpsID].SMfd > -1 )
        {
            // connected
            printf("%*ld  %*s  %*ld/%*ld\n",
                   NBchar_fpsID, fpsID,
                   NBchar_fpsname, data.fps[fpsID].md[0].name,
                   NBchar_NBparam, data.fps[fpsID].NBparamActive,
                   NBchar_NBparam, data.fps[fpsID].NBparam
                  );

            fpscnt++;
        }
    }

    printf("\n %ld FPS(s) found\n\n", fpscnt);

    return RETURN_SUCCESS;
}




