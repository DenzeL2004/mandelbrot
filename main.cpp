#include <stdio.h>
#include <stdlib.h>

#include "src/log_info/log_errors.h"
#include "src/generals_func/generals.h"

#include "mandelbrot/mandelbrot.h"

int main ()
{  
    #ifdef USE_LOG
        if (OpenLogsFile()) 
            return OPEN_FILE_LOG_ERR;
    #endif

    Mandelbrot_struct mandelbrot_struct = {};
    if (MandelbrotCtor(&mandelbrot_struct, Window_hight, Window_width, 
                        Init_start_x, Init_start_x, Init_delta))
        return PROCESS_ERROR (MANDELBROT_GET_ERR, "Ctor failed. MandelbrotExe execute failed\n");

    if (MandelbrotExe(&mandelbrot_struct))
        return PROCESS_ERROR(EXIT_FAILURE, "Get mandelbrot imagination failed\n");

    if (MandelbrotDtor(&mandelbrot_struct))
        return PROCESS_ERROR (MANDELBROT_GET_ERR, "Dtor failed. MandelbrotExe execute failed\n");



    #ifdef USE_LOG
        if (CloseLogsFile ())
            return OPEN_FILE_LOG_ERR;
    #endif

    return EXIT_SUCCESS;
}