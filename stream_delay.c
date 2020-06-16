/** @file stream_delay,c
 */

#include <math.h>

#include "CommandLineInterface/CLIcore.h"
#include "image_ID.h"
#include "stream_sem.h"
#include "create_image.h"
#include "delete_image.h"

#include "COREMOD_tools/COREMOD_tools.h"







// ==========================================
// Forward declaration(s)
// ==========================================



errno_t COREMOD_MEMORY_streamDelay_FPCONF(
    char    *fpsname,
    uint32_t CMDmode
);

imageID COREMOD_MEMORY_streamDelay_RUN(
    char *fpsname
);


errno_t COREMOD_MEMORY_streamDelay(
    const char *IDin_name,
    const char *IDout_name,
    long        delayus,
    long        dtus
);



// ==========================================
// Command line interface wrapper function(s)
// ==========================================


static errno_t COREMOD_MEMORY_streamDelay__cli()
{
    char fpsname[200];

    // First, we try to execute function through FPS interface
    if(0
            + CLI_checkarg(1, 5)
            + CLI_checkarg(2, CLIARG_LONG)
            == 0)   // check that first arg is string, second arg is int
    {
        unsigned int OptionalArg00 = data.cmdargtoken[2].val.numl;

        // Set FPS interface name
        // By convention, if there are optional arguments, they should be appended to the fps name
        //
        if(data.processnameflag ==
                0)   // name fps to something different than the process name
        {
            sprintf(fpsname, "streamDelay-%06u", OptionalArg00);
        }
        else     // Automatically set fps name to be process name up to first instance of character '.'
        {
            strcpy(fpsname, data.processname0);
        }

        if(strcmp(data.cmdargtoken[1].val.string,
                  "_FPSINIT_") == 0)    // Initialize FPS and conf process
        {
            printf("Function parameters configure\n");
            COREMOD_MEMORY_streamDelay_FPCONF(fpsname, FPSCMDCODE_FPSINIT);
            return RETURN_SUCCESS;
        }

        if(strcmp(data.cmdargtoken[1].val.string,
                  "_CONFSTART_") == 0)    // Start conf process
        {
            printf("Function parameters configure\n");
            COREMOD_MEMORY_streamDelay_FPCONF(fpsname, FPSCMDCODE_CONFSTART);
            return RETURN_SUCCESS;
        }

        if(strcmp(data.cmdargtoken[1].val.string,
                  "_CONFSTOP_") == 0)   // Stop conf process
        {
            printf("Function parameters configure\n");
            COREMOD_MEMORY_streamDelay_FPCONF(fpsname, FPSCMDCODE_CONFSTOP);
            return RETURN_SUCCESS;
        }

        if(strcmp(data.cmdargtoken[1].val.string, "_RUNSTART_") == 0)   // Run process
        {
            printf("Run function\n");
            COREMOD_MEMORY_streamDelay_RUN(fpsname);
            return RETURN_SUCCESS;
        }
        /*
                if(strcmp(data.cmdargtoken[1].val.string, "_RUNSTOP_") == 0) { // Cleanly stop process
                    printf("Run function\n");
                    COREMOD_MEMORY_streamDelay_STOP(OptionalArg00);
                    return RETURN_SUCCESS;
                }*/
    }

    // non FPS implementation - all parameters specified at function launch
    if(0
            + CLI_checkarg(1, CLIARG_IMG)
            + CLI_checkarg(2, 5)
            + CLI_checkarg(3, CLIARG_LONG)
            + CLI_checkarg(4, CLIARG_LONG)
            == 0)
    {
        COREMOD_MEMORY_streamDelay(
            data.cmdargtoken[1].val.string,
            data.cmdargtoken[2].val.string,
            data.cmdargtoken[3].val.numl,
            data.cmdargtoken[4].val.numl
        );
        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }

}



// ==========================================
// Register CLI command(s)
// ==========================================

errno_t stream_delay_addCLIcmd()
{

    RegisterCLIcommand(
        "streamdelay",
        __FILE__,
        COREMOD_MEMORY_streamDelay__cli,
        "delay 2D image stream",
        "<image2d in> <image2d out> <delay [us]> <resolution [us]>",
        "streamdelay instream outstream 1000 10",
        "long COREMOD_MEMORY_streamDelay(const char *IDin_name, const char *IDout_name, long delayus, long dtus)");

    return RETURN_SUCCESS;
}










/**
 * @brief Manages configuration parameters for streamDelay
 *
 * ## Purpose
 *
 * Initializes configuration parameters structure\n
 *
 * ## Arguments
 *
 * @param[in]
 * char*		fpsname
 * 				name of function parameter structure
 *
 * @param[in]
 * uint32_t		CMDmode
 * 				Command mode
 *
 *
 */
errno_t COREMOD_MEMORY_streamDelay_FPCONF(
    char    *fpsname,
    uint32_t CMDmode
)
{

    FPS_SETUP_INIT(fpsname, CMDmode);
    uint64_t FPFLAG;

    FPFLAG = FPFLAG_DEFAULT_INPUT | FPFLAG_MINLIMIT;
    FPFLAG &= ~FPFLAG_WRITERUN;

    long delayus_default[4] = { 1000, 1, 10000, 1000 };
    long fp_delayus = function_parameter_add_entry(&fps, ".delayus", "Delay [us]",
                      FPTYPE_INT64, FPFLAG, &delayus_default);
    (void) fp_delayus; // suppresses unused parameter compiler warning

    long dtus_default[4] = { 50, 1, 200, 50 };
    long fp_dtus    = function_parameter_add_entry(&fps, ".dtus",
                      "Loop period [us]", FPTYPE_INT64, FPFLAG, &dtus_default);
    (void) fp_dtus; // suppresses unused parameter compiler warning


    FPS_ADDPARAM_STREAM_IN(stream_inname,   ".in_name",  "input stream", NULL);
    FPS_ADDPARAM_STREAM_OUT(stream_outname, ".out_name", "output stream");

    long timeavemode_default[4] = { 0, 0, 3, 0 };
    FPS_ADDPARAM_INT64_IN(
        option_timeavemode,
        ".option.timeavemode",
        "Enable time window averaging (>0)",
        &timeavemode_default);

    double avedt_default[4] = { 0.001, 0.0001, 1.0, 0.001};
    FPS_ADDPARAM_FLT64_IN(
        option_avedt,
        ".option.avedt",
        "Averaging time window width",
        &avedt_default);

    // status
    FPS_ADDPARAM_INT64_OUT(zsize, ".status.zsize",  "cube size");
    FPS_ADDPARAM_INT64_OUT(framelog, ".status.framelag", "lag in frame unit");
    FPS_ADDPARAM_INT64_OUT(kkin, ".status.kkin", "input cube slice index");
    FPS_ADDPARAM_INT64_OUT(kkout, ".status.kkout", "output cube slice index");




	// start function parameter conf loop, defined in function_parameter.h
    FPS_CONFLOOP_START 


    if(fps.parray[fp_option_timeavemode].val.l[0] != 0)
    { // time averaging enabled
        fps.parray[fp_option_avedt].fpflag |= FPFLAG_WRITERUN;
        fps.parray[fp_option_avedt].fpflag |= FPFLAG_USED;
        fps.parray[fp_option_avedt].fpflag |= FPFLAG_VISIBLE;
    }
    else
    {
        fps.parray[fp_option_avedt].fpflag &= ~FPFLAG_WRITERUN;
        fps.parray[fp_option_avedt].fpflag &= ~FPFLAG_USED;
        fps.parray[fp_option_avedt].fpflag &= ~FPFLAG_VISIBLE;
    }


    // stop function parameter conf loop, defined in function_parameter.h
    FPS_CONFLOOP_END


    return RETURN_SUCCESS;
}














/**
 * @brief Delay image stream by time offset
 *
 * IDout_name is a time-delayed copy of IDin_name
 *
 */

imageID COREMOD_MEMORY_streamDelay_RUN(
    char *fpsname
)
{
    imageID             IDimc;
    imageID             IDin, IDout;
    uint32_t            xsize, ysize, xysize;
//    long                cnt0old;
    long                ii;
    struct timespec    *t0array;
    struct timespec     tnow;
    double              tdiffv;
    struct timespec     tdiff;
    uint32_t           *arraytmp;
    long                cntskip = 0;
    long                kk;


    // ===========================
    /// ### CONNECT TO FPS
    // ===========================
    FPS_CONNECT(fpsname, FPSCONNECT_RUN);


    // ===============================
    /// ### GET FUNCTION PARAMETER VALUES
    // ===============================
    // parameters are addressed by their tag name
    // These parameters are read once, before running the loop
    //
    char IDin_name[200];
    strncpy(IDin_name,  functionparameter_GetParamPtr_STRING(&fps, ".in_name"),
            FUNCTION_PARAMETER_STRMAXLEN);

    char IDout_name[200];
    strncpy(IDout_name, functionparameter_GetParamPtr_STRING(&fps, ".out_name"),
            FUNCTION_PARAMETER_STRMAXLEN);

    long delayus = functionparameter_GetParamValue_INT64(&fps, ".delayus");

    long dtus    = functionparameter_GetParamValue_INT64(&fps, ".dtus");

    int timeavemode = functionparameter_GetParamValue_INT64(&fps,
                      ".option.timeavemode");
    double *avedt   = functionparameter_GetParamPtr_FLOAT64(&fps, ".option.avedt");

    long *zsize    = functionparameter_GetParamPtr_INT64(&fps, ".status.zsize");
    long *framelag = functionparameter_GetParamPtr_INT64(&fps, ".status.framelag");
    long *kkin     = functionparameter_GetParamPtr_INT64(&fps, ".status.kkin");
    long *kkout    = functionparameter_GetParamPtr_INT64(&fps, ".status.kkout");

    DEBUG_TRACEPOINT(" ");

    // ===========================
    /// ### processinfo support
    // ===========================
    PROCESSINFO *processinfo;

    char pinfodescr[200];
    sprintf(pinfodescr, "streamdelay %.10s %.10s", IDin_name, IDout_name);
    processinfo = processinfo_setup(
                      fpsname,                 // re-use fpsname as processinfo name
                      pinfodescr,    // description
                      "startup",  // message on startup
                      __FUNCTION__, __FILE__, __LINE__
                  );

    // OPTIONAL SETTINGS
    processinfo->MeasureTiming = 1; // Measure timing
    processinfo->RT_priority =
        20;  // RT_priority, 0-99. Larger number = higher priority. If <0, ignore




    // =============================================
    /// ### OPTIONAL: TESTING CONDITION FOR LOOP ENTRY
    // =============================================
    // Pre-loop testing, anything that would prevent loop from starting should issue message
    int loopOK = 1;


    IDin = image_ID(IDin_name);



    // ERROR HANDLING
    if(IDin == -1)
    {
        struct timespec errtime;
        struct tm *errtm;

        clock_gettime(CLOCK_REALTIME, &errtime);
        errtm = gmtime(&errtime.tv_sec);

        fprintf(stderr,
                "%02d:%02d:%02d.%09ld  ERROR [%s %s %d] Input stream %s does not exist, cannot proceed\n",
                errtm->tm_hour,
                errtm->tm_min,
                errtm->tm_sec,
                errtime.tv_nsec,
                __FILE__,
                __FUNCTION__,
                __LINE__,
                IDin_name);

        char msgstring[200];
        sprintf(msgstring, "Input stream %.20s does not exist", IDin_name);
        processinfo_error(processinfo, msgstring);
        loopOK = 0;
    }


    xsize = data.image[IDin].md[0].size[0];
    ysize = data.image[IDin].md[0].size[1];
    *zsize = (long)(2 * delayus / dtus);
    if(*zsize < 1)
    {
        *zsize = 1;
    }
    xysize = xsize * ysize;

    t0array = (struct timespec *) malloc(sizeof(struct timespec) * *zsize);

    IDimc = create_3Dimage_ID("_tmpc", xsize, ysize, *zsize);



    IDout = image_ID(IDout_name);
    if(IDout == -1)   // CREATE IT
    {
        arraytmp = (uint32_t *) malloc(sizeof(uint32_t) * 2);
        arraytmp[0] = xsize;
        arraytmp[1] = ysize;
        IDout = create_image_ID(IDout_name, 2, arraytmp, _DATATYPE_FLOAT, 1, 0);
        COREMOD_MEMORY_image_set_createsem(IDout_name, IMAGE_NB_SEMAPHORE);
        free(arraytmp);
    }


    *kkin = 0;
    *kkout = 0;
//    cnt0old = data.image[IDin].md[0].cnt0;

    float *arraytmpf;
    arraytmpf = (float *) malloc(sizeof(float) * xsize * ysize);

    clock_gettime(CLOCK_REALTIME, &tnow);
    for(kk = 0; kk < *zsize; kk++)
    {
        t0array[kk] = tnow;
    }


    DEBUG_TRACEPOINT(" ");

    // ===========================
    /// ### START LOOP
    // ===========================

    processinfo_loopstart(
        processinfo); // Notify processinfo that we are entering loop

    DEBUG_TRACEPOINT(" ");

    while(loopOK == 1)
    {
        int kkinscan;
        float normframes = 0.0;

        DEBUG_TRACEPOINT(" ");
        loopOK = processinfo_loopstep(processinfo);

        usleep(dtus); // main loop wait

        processinfo_exec_start(processinfo);

        if(processinfo_compute_status(processinfo) == 1)
        {
            DEBUG_TRACEPOINT(" ");

            // has new frame arrived ?
//            cnt0 = data.image[IDin].md[0].cnt0;

//            if(cnt0 != cnt0old) { // new frame
            clock_gettime(CLOCK_REALTIME, &t0array[*kkin]);  // record time of input frame

            DEBUG_TRACEPOINT(" ");
            for(ii = 0; ii < xysize; ii++)
            {
                data.image[IDimc].array.F[(*kkin) * xysize + ii] = data.image[IDin].array.F[ii];
            }
            (*kkin) ++;
            DEBUG_TRACEPOINT(" ");

            if((*kkin) == (*zsize))
            {
                (*kkin) = 0;
            }
            //              cnt0old = cnt0;
            //          }



            clock_gettime(CLOCK_REALTIME, &tnow);
            DEBUG_TRACEPOINT(" ");


            cntskip = 0;
            tdiff = timespec_diff(t0array[*kkout], tnow);
            tdiffv = 1.0 * tdiff.tv_sec + 1.0e-9 * tdiff.tv_nsec;

            DEBUG_TRACEPOINT(" ");


            while((tdiffv > 1.0e-6 * delayus) && (cntskip < *zsize))
            {
                cntskip++;  // advance index until time condition is satisfied
                (*kkout) ++;
                if(*kkout == *zsize)
                {
                    *kkout = 0;
                }
                tdiff = timespec_diff(t0array[*kkout], tnow);
                tdiffv = 1.0 * tdiff.tv_sec + 1.0e-9 * tdiff.tv_nsec;
            }

            DEBUG_TRACEPOINT(" ");

            *framelag = *kkin - *kkout;
            if(*framelag < 0)
            {
                *framelag += *zsize;
            }


            DEBUG_TRACEPOINT(" ");


            switch(timeavemode)
            {


                case 0: // no time averaging - pick more recent frame that matches requirement
                    DEBUG_TRACEPOINT(" ");
                    if(cntskip > 0)
                    {
                        char *ptr; // pointer address

                        data.image[IDout].md[0].write = 1;

                        ptr = (char *) data.image[IDimc].array.F;
                        ptr += SIZEOF_DATATYPE_FLOAT * xysize * *kkout;

                        memcpy(data.image[IDout].array.F, ptr,
                               SIZEOF_DATATYPE_FLOAT * xysize);  // copy time-delayed input to output

                        COREMOD_MEMORY_image_set_sempost_byID(IDout, -1);
                        data.image[IDout].md[0].cnt0++;
                        data.image[IDout].md[0].write = 0;
                    }
                    break;

                default : // strict time window (note: other modes will be coded in the future)
                    normframes = 0.0;
                    DEBUG_TRACEPOINT(" ");

                    for(ii = 0; ii < xysize; ii++)
                    {
                        arraytmpf[ii] = 0.0;
                    }

                    for(kkinscan = 0; kkinscan < *zsize; kkinscan++)
                    {
                        tdiff = timespec_diff(t0array[kkinscan], tnow);
                        tdiffv = 1.0 * tdiff.tv_sec + 1.0e-9 * tdiff.tv_nsec;

                        if((tdiffv > 0) && (fabs(tdiffv - 1.0e-6 * delayus) < *avedt))
                        {
                            float coeff = 1.0;
                            for(ii = 0; ii < xysize; ii++)
                            {
                                arraytmpf[ii] += coeff * data.image[IDimc].array.F[kkinscan * xysize + ii];
                            }
                            normframes += coeff;
                        }
                    }
                    if(normframes < 0.0001)
                    {
                        normframes = 0.0001;    // avoid division by zero
                    }

                    data.image[IDout].md[0].write = 1;
                    for(ii = 0; ii < xysize; ii++)
                    {
                        data.image[IDout].array.F[ii] = arraytmpf[ii] / normframes;
                    }
                    COREMOD_MEMORY_image_set_sempost_byID(IDout, -1);
                    data.image[IDout].md[0].cnt0++;
                    data.image[IDout].md[0].write = 0;

                    break;
            }
            DEBUG_TRACEPOINT(" ");



        }
        // process signals, increment loop counter
        processinfo_exec_end(processinfo);
        DEBUG_TRACEPOINT(" ");
    }

    // ==================================
    /// ### ENDING LOOP
    // ==================================
    processinfo_cleanExit(processinfo);
    function_parameter_RUNexit(&fps);

    DEBUG_TRACEPOINT(" ");

    delete_image_ID("_tmpc");

    free(t0array);
    free(arraytmpf);

    return IDout;
}












errno_t COREMOD_MEMORY_streamDelay(
    const char *IDin_name,
    const char *IDout_name,
    long        delayus,
    long        dtus
)
{
    char fpsname[200];
    unsigned int pindex = 0;
    FUNCTION_PARAMETER_STRUCT fps;

    // create FPS
    sprintf(fpsname, "%s-%06u", __FUNCTION__, pindex);
    COREMOD_MEMORY_streamDelay_FPCONF(fpsname, FPSCMDCODE_FPSINIT);

    function_parameter_struct_connect(fpsname, &fps, FPSCONNECT_RUN);

    functionparameter_SetParamValue_STRING(&fps, ".instreamname", IDin_name);
    functionparameter_SetParamValue_STRING(&fps, ".outstreamname", IDout_name);

    functionparameter_SetParamValue_INT64(&fps, ".delayus", delayus);
    functionparameter_SetParamValue_INT64(&fps, ".dtus", dtus);

    function_parameter_struct_disconnect(&fps);

    COREMOD_MEMORY_streamDelay_RUN(fpsname);

    return RETURN_SUCCESS;
}





