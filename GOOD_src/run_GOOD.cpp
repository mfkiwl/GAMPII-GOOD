/*------------------------------------------------------------------------------
* Created by Feng Zhou on 2020-09-25
*
* main entry of run_GOOD.exe
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "PreProcess.h"
#include "StringUtil.h"


/* ppp_run main --------------------------------------------------------------*/
int main(int argc, char * argv[])
{
#ifdef _WIN32  /* for Windows */
    //char cfgFile[1000] = "D:\\data\\gamp_good.cfg";
    char *cfgFile;
    if (argc == 1)
    {
        cout << " * The input command-line parameter indicating configure file is lost, please check it!" << endl;

        return 0;
    }
    else cfgFile = argv[1];
#else          /* for Linux or Mac */
    char *cfgFile;
    if (argc == 1)
    {
        cout << " * The input command-line parameter indicating configure file is lost, please check it!" << endl;
        
        return 0;
    }
    else cfgFile = argv[1];
#endif

#if (defined(_WIN32) && !defined(MinGW))  /* for Windows */
    _setmaxstdio(8192);   /* to open many files */
#endif

    PreProcess preProc;
    preProc.run(cfgFile);

#ifdef _WIN32  /* for Windows */
    /*cout << "Press any key to exit!" << endl;
    getchar();*/
#endif

    return 0;
}