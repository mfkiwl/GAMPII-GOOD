/*------------------------------------------------------------------------------
* FtpUtil.cpp : GNSS data downloading module
*
* Copyright (C) 2020-2099 by SpAtial SurveyIng and Navigation (SASIN) Group, all rights reserved.
*    This file is part of GAMP II - GOOD (Gnss Observations and prOducts Downloader)
*
* References:
*    Dr Cheng Wang @ BUAA provided the C++ code for observation downloading as reference
*
* history : 2021/04/16 1.0  new (by Feng Zhou @ SDUST)
*           2021/04/20      MGEX multi-GNSS precise products option added (by Feng Zhou @ SDUST)
*           2021/04/23 1.1  IGN and WHU FPT archives added (by Feng Zhou @ SDUST)
*           2021/04/30      the day before and after the current day for precise satellite 
                               orbit and clock products downloading is added (by Feng Zhou @ SDUST and Zan Liu @ CUMT)
*           2021/04/30 1.2  CNES real-time orbit, clock, bias files (in offline mode) option added (by Feng Zhou @ SDUST)
*           2021/05/01      from DOY 345/2020, "brdcDDD0.YYn" was converted from "*.Z" to "*.gz" (by Feng Zhou @ SDUST and Zan Liu @ CUMT)
*           2021/05/01      added the full path of third-party softwares (by Feng Zhou @ SDUST and Yudan Yi)
*           2021/05/05 1.3  add "GetRoti" for Rate of TEC index (ROTI) file downloading (by Feng Zhou @ SDUST and Lei Liu @ WHU)
*           2021/05/06 1.4  add the compatibility for changing directory (chdir) in Windows and Linux OS (by Feng Zhou @ SDUST)
*           2021/05/07      add the option 'printInfoWget' in configure file for (not) printing the information of running 'wget' (by Feng Zhou @ SDUST and Hong Hu @ AHU)
*           2021/05/08      add IGS and MGEX hourly observation (30s) downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/10 1.5  modify some codes to make GOOD adaptable to different compression form (i.e., '*.Z' or '*.gz') (by Feng Zhou @ SDUST)
*           2021/05/12      add the option for IGR, IGU, GFU (from GFZ), and WUU (from WHU) products as well as the site-specific broadcast ephemeris files downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/15      sub-directory (i.e., daily, hourly, and highrate) creation for observation downloading (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/18 1.6  modify some codes to make configuration file and program look more concise (by Feng Zhou @ SDUST and Menghao Li @ HEU)
*           2021/05/21      add the extraction and conversion for the 'all' option in IGS and MGEX observation downloading (by Feng Zhou @ SDUST)
*-----------------------------------------------------------------------------*/
#include "Good.h"
#include "TimeUtil.h"
#include "StringUtil.h"
#include "FtpUtil.h"


/* constants/macros ----------------------------------------------------------*/
#define IDX_OBSD   0    /* index for IGS daily observation (30s) file downloaded (short name "d" files) */
#define IDX_OBSH   1    /* index for IGS hourly observation (30s) file downloaded (short name "d" files) */
#define IDX_OBSHR  2    /* index for IGS high-rate observation (1s) file downloaded (short name "d" files) */
#define IDX_OBMD   3    /* index for MGEX daily observation (30s) file downloaded (long name "crx" files) */
#define IDX_OBMH   4    /* index for MGEX daily observation (30s) file downloaded (long name "crx" files) */
#define IDX_OBMHR  5    /* index for MGEX high-rate observation (1s) file downloaded (long name "crx" files) */
#define IDX_NAV    6    /* index for broadcast ephemeris file downloaded */
#define IDX_SP3    7    /* index for IGS SP3 file downloaded */
#define IDX_CLK    8    /* index for IGS CLK file downloaded */
#define IDX_EOP    9    /* index for IGS EOP file downloaded */
#define IDX_SNX    10   /* index for IGS weekly SINEX file downloaded */
#define IDX_SP3M   11   /* index for MGEX SP3 file downloaded */
#define IDX_CLKM   12   /* index for MGEX CLK file downloaded */
#define IDX_DCBM   13   /* index for MGEX DCB file downloaded */
#define IDX_ION    14   /* index for global ionosphere map (GIM) downloaded */
#define IDX_ROTI   15   /* index for Rate of TEC index (ROTI) downloaded */
#define IDX_ZTD    16   /* index for IGS final tropospheric product downloaded */


/* function definition -------------------------------------------------------*/

/**
* @brief   : init - Get FTP archive for CDDIS, IGN, or WHU
* @param[I]: none
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::init()
{
    /* FTP archive for CDDIS */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* IGS daily observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/hourly");               /* IGS hourly observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/highrate");             /* IGS high-rate observation (1s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* MGEX daily observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/hourly");               /* MGEX hourly observation (30s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/highrate");             /* MGEX high-rate observation (1s) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/data/daily");                /* broadcast ephemeris files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS SP3 files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS CLK files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS EOP files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products");                  /* IGS weekly SINEX files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/mgex");             /* MGEX SP3 files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/mgex");             /* MGEX CLK files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/bias");             /* MGEX DCB files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/ionex");            /* global ionosphere map (GIM) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/ionex");            /* Rate of TEC index (ROTI) files */
    _ftpArchive.CDDIS.push_back("ftps://gdc.cddis.eosdis.nasa.gov/pub/gnss/products/troposphere/zpd");  /* IGS final tropospheric product files */

    /* FTP archive for IGN */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* IGS daily observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/hourly");                                  /* IGS hourly observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/highrate");                                /* IGS high-rate observation (1s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* MGEX daily observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/hourly");                                  /* MGEX hourly observation (30s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data/highrate");                                /* MGEX high-rate observation (1s) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/data");                                         /* broadcast ephemeris files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS SP3 files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS CLK files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS EOP files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products");                                     /* IGS weekly SINEX files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex");                                /* MGEX SP3 files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex");                                /* MGEX CLK files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/mgex/dcb");                            /* MGEX DCB files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/ionosphere");                          /* global ionosphere map (GIM) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/ionosphere");                          /* Rate of TEC index (ROTI) files */
    _ftpArchive.IGN.push_back("ftp://igs.ign.fr/pub/igs/products/troposphere");                         /* IGS final tropospheric product files */

    /* FTP archive for WHU */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* IGS daily observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/hourly");                              /* IGS hourly observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data");                                     /* IGS high-rate observation (1s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* MGEX daily observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/hourly");                              /* MGEX hourly observation (30s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data");                                     /* MGEX high-rate observation (1s) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/data/daily");                               /* broadcast ephemeris files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS SP3 files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS CLK files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS EOP files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products");                                 /* IGS weekly SINEX files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex");                            /* MGEX SP3 files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex");                            /* MGEX CLK files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/mgex/dcb");                        /* MGEX DCB files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/ionex");                           /* global ionosphere map (GIM) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/ionex");                           /* Rate of TEC index (ROTI) files */
    _ftpArchive.WHU.push_back("ftp://igs.gnsswhu.cn/pub/gps/products/troposphere/new");                 /* IGS final tropospheric product files */
} /* end of init */

/**
* @brief   : GetDailyObsIgs - download IGS RINEX daily observation (30s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        string url, cutDirs = " --cut-dirs=7 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSD] + "/" +
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else if (strcmp(ftpName.c_str(), "IGN") == 0)
        {
            url = _ftpArchive.IGN[IDX_OBSD] + "/" + sYyyy + "/" + sDoy;
            cutDirs = " --cut-dirs=5 ";
        }
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBSD] + "/" +
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else url = _ftpArchive.CDDIS[IDX_OBSD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

        /* it is OK for '*.Z' or '*.gz' format */
        string dFile = "*" + sDoy + "0." + sYy + "d";
        string dxFile = dFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = "." + sYy + "d";
        vector<string> dFiles;
        str.GetFilesAll(subDir, suffix, dFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < dFiles.size(); i++)
        {
            if (access(dFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + " -d -f " + dFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, dFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                dFile = site + sDoy + "0." + sYy + "d";
                if (access(dFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + dFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + dFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                {
                    string url, cutDirs = " --cut-dirs=7 ";
                    if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSD] + "/" +
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else if (strcmp(ftpName.c_str(), "IGN") == 0)
                    {
                        url = _ftpArchive.IGN[IDX_OBSD] + "/" + sYyyy + "/" + sDoy;
                        cutDirs = " --cut-dirs=5 ";
                    }
                    else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBSD] + "/" +
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else url = _ftpArchive.CDDIS[IDX_OBSD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

                    /* it is OK for '*.Z' or '*.gz' format */
                    string dxFile = dFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                    if (access(dgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + dgzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(dzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + dzFile;
                        std::system(cmd.c_str());
                    }
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        cout << "*** WARNING(FtpUtil::GetDailyObsIgs): failed to download IGS daily observation file " << oFile << endl;

                        continue;
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsIgs): successfully download IGS daily observation file " << oFile << endl;

                        /* delete dFile */
                        remove(dFile.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsIgs): IGS daily observation file " << oFile << 
                    " or " << dFile << " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsIgs */

/**
* @brief   : GetHourlyObsIgs - download IGS RINEX hourly observation (30s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHourlyObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('hourly') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        for (int i = 0; i < fopt->hhObs.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObs[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=7 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_OBSH] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBSH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else url = _ftpArchive.CDDIS[IDX_OBSH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObs[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);
            /* it is OK for '*.Z' or '*.gz' format */
            string dFile = "*" + sDoy + sch + "." + sYy + "d";
            string dxFile = dFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* get the file list */
            string suffix = "." + sYy + "d";
            vector<string> dFiles;
            str.GetFilesAll(sHhDir, suffix, dFiles);
            char sitName[MAXCHARS];
            for (int i = 0; i < dFiles.size(); i++)
            {
                if (access(dFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + " -d -f " + dFiles[i];
                    std::system(cmd.c_str());

                    str.StrMid(sitName, dFiles[i].c_str(), 0, 4);
                    string site = sitName;
                    str.ToLower(site);
                    dFile = site + sDoy + sch + "." + sYy + "d";
                    if (access(dFile.c_str(), 0) == -1) continue;
                    string oFile = site + sDoy + sch + "." + sYy + "o";
                    /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + dFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + dFile;
#endif
                    std::system(cmd.c_str());
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObs.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObs[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObs[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string oFile = sitName + sDoy + sch + "." + sYy + "o";
                    string dFile = sitName + sDoy + sch + "." + sYy + "d";
                    if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (strcmp(ftpName.c_str(), "IGN") == 0)
                        {
                            url = _ftpArchive.IGN[IDX_OBSH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBSH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else url = _ftpArchive.CDDIS[IDX_OBSH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        string dxFile = dFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                        if (access(dgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + dgzFile;
                            std::system(cmd.c_str());
                        }
                        else if (access(dzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.Z' */
                            cmd = gzipFull + " -d -f " + dzFile;
                            std::system(cmd.c_str());
                        }
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            cout << "*** WARNING(FtpUtil::GetHourlyObsIgs): failed to download IGS Hourly observation file " << oFile << endl;

                            continue;
                        }

#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        if (access(oFile.c_str(), 0) == 0)
                        {
                            cout << "*** INFO(FtpUtil::GetHourlyObsIgs): successfully download IGS hourly observation file " << oFile << endl;

                            /* delete dFile */
                            remove(dFile.c_str());
                        }
                    }
                    else cout << "*** INFO(FtpUtil::GetHourlyObsIgs): IGS hourly observation file " <<
                        oFile << " or " << dFile << " has existed!" << endl;
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHourlyObsIgs */

/**
* @brief   : GetHrObsIgs - download IGS RINEX high-rate observation (1s) files (short name "d") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHrObsIgs(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('highrate') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "highrate");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obsOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the IGS observation files */
        for (int i = 0; i < fopt->hhObs.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObs[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=8 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" +
                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObs[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);
            /* it is OK for '*.Z' or '*.gz' format */
            string dFile = "*" + sDoy + sch + "*." + sYy + "d";
            string dxFile = dFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
            std::system(cmd.c_str());

            std::vector<string> minuStr = { "00", "15", "30", "45" };
            for (int i = 0; i < minuStr.size(); i++)
            {
                /* get the file list */
                string suffix = minuStr[i] + "." + sYy + "d";
                vector<string> dFiles;
                str.GetFilesAll(sHhDir, suffix, dFiles);
                char sitName[MAXCHARS];
                for (int j = 0; j < dFiles.size(); j++)
                {
                    if (access(dFiles[j].c_str(), 0) == 0)
                    {
                        /* extract it */
                        cmd = gzipFull + " -d -f " + dFiles[j];
                        std::system(cmd.c_str());

                        str.StrMid(sitName, dFiles[j].c_str(), 0, 4);
                        string site = sitName;
                        str.ToLower(site);
                        dFile = site + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(dFile.c_str(), 0) == -1) continue;
                        string oFile = site + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + dFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + dFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the IGS observation file site-by-site */
        if (access(fopt->obsOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obsOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHrObsIgs): open site.list = " << fopt->obsOpt << 
                    " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObs.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObs[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObs[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);

                    std::vector<string> minuStr = { "00", "15", "30", "45" };
                    for (int i = 0; i < minuStr.size(); i++)
                    {
                        str.ToLower(sitName);
                        string oFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        string dFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                        {
                            string url, cutDirs = " --cut-dirs=8 ";
                            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" +
                                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
                            else if (strcmp(ftpName.c_str(), "IGN") == 0)
                            {
                                url = _ftpArchive.IGN[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy;
                                cutDirs = " --cut-dirs=6 ";
                            }
                            else url = _ftpArchive.CDDIS[IDX_OBSHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

                            /* it is OK for '*.Z' or '*.gz' format */
                            string dxFile = dFile + ".*";
                            string cmd = wgetFull + " " + qr + " -nH -A " + dxFile + cutDirs + url;
                            std::system(cmd.c_str());

                            string dgzFile = dFile + ".gz", dzFile = dFile + ".Z";
                            if (access(dgzFile.c_str(), 0) == 0)
                            {
                                /* extract '*.gz' */
                                cmd = gzipFull + " -d -f " + dgzFile;
                                std::system(cmd.c_str());
                            }
                            else if (access(dzFile.c_str(), 0) == 0)
                            {
                                /* extract '*.Z' */
                                cmd = gzipFull + " -d -f " + dzFile;
                                std::system(cmd.c_str());
                            }
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetHrObsIgs): failed to download IGS high-rate observation file " << oFile << endl;

                                continue;
                            }

#ifdef _WIN32  /* for Windows */
                            cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                            cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                            std::system(cmd.c_str());

                            if (access(oFile.c_str(), 0) == 0)
                            {
                                cout << "*** INFO(FtpUtil::GetHrObsIgs): successfully download IGS high-rate observation file " << oFile << endl;

                                /* delete dFile */
                                remove(dFile.c_str());
                            }
                        }
                        else cout << "*** INFO(FtpUtil::GetHrObsIgs): IGS high-rate observation file " <<
                            oFile << " or " << dFile << " has existed!" << endl;
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHrObsIgs */

/**
* @brief   : GetDailyObsMgex - download MGEX RINEX daily observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDailyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('daily') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(subDir.c_str());
#else           /* for Linux or Mac */
    chdir(subDir.c_str());
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        string url, cutDirs = " --cut-dirs=7 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + 
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else if (strcmp(ftpName.c_str(), "IGN") == 0)
        {
            url = _ftpArchive.IGN[IDX_OBMD] + "/" + sYyyy + "/" + sDoy;
            cutDirs = " --cut-dirs=5 ";
        }
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBMD] + "/" + 
            sYyyy + "/" + sDoy + "/" + sYy + "d";
        else url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

        /* it is OK for '*.Z' or '*.gz' format */
        string crxFile = "*_R_" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
        string crxxFile = crxFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* get the file list */
        string suffix = ".crx";
        vector<string> crxFiles;
        str.GetFilesAll(subDir, suffix, crxFiles);
        char sitName[MAXCHARS];
        for (int i = 0; i < crxFiles.size(); i++)
        {
            if (access(crxFiles[i].c_str(), 0) == 0)
            {
                /* extract it */
                cmd = gzipFull + "  -d -f " + crxFiles[i];
                std::system(cmd.c_str());

                str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                string site = sitName;
                str.ToLower(site);
                str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                crxFile = sitName;
                if (access(crxFile.c_str(), 0) == -1) continue;
                string oFile = site + sDoy + "0." + sYy + "o";
                /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                std::system(cmd.c_str());

                /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                cmd = "rm -rf " + crxFile;
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetDailyObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                str.ToLower(sitName);
                string oFile = sitName + sDoy + "0." + sYy + "o";
                string dFile = sitName + sDoy + "0." + sYy + "d";
                if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                {
                    string url, cutDirs = " --cut-dirs=7 ";
                    if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + 
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else if (strcmp(ftpName.c_str(), "IGN") == 0)
                    {
                        url = _ftpArchive.IGN[IDX_OBMD] + "/" + sYyyy + "/" + sDoy;
                        cutDirs = " --cut-dirs=5 ";
                    }
                    else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBMD] + "/" + 
                        sYyyy + "/" + sDoy + "/" + sYy + "d";
                    else url = _ftpArchive.CDDIS[IDX_OBMD] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d";

                    /* it is OK for '*.Z' or '*.gz' format */
                    str.ToUpper(sitName);
                    string crxFile = sitName + "*_R_" + sYyyy + sDoy + "0000_01D_30S_MO.crx";
                    string crxxFile = crxFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    string crxgzFile = crxFile + ".gz";
                    cmd = gzipFull + " -d -f " + crxgzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + crxFile + " " + dFile;
                    std::system(cmd.c_str());
                    if (access(dFile.c_str(), 0) == -1)
                    {
                        /* extract '*.Z' */
                        string crxzFile = crxFile + ".Z";
                        cmd = gzipFull + " -d -f " + crxzFile;
                        std::system(cmd.c_str());

                        cmd = changeFileName + " " + crxFile + " " + dFile;
                        std::system(cmd.c_str());
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                            continue;
                        }
                    }

#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    if (access(oFile.c_str(), 0) == 0)
                    {
                        cout << "*** INFO(FtpUtil::GetDailyObsMgex): successfully download MGEX daily observation file " << oFile << endl;

                        /* delete dFile */
                        remove(dFile.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetDailyObsMgex): MGEX daily observation file " << oFile << 
                    " or " << dFile << " has existed!" << endl;
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetDailyObsMgex */

/**
* @brief   : GetHourlyObsMgex - download MGEX RINEX hourly observation (30s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHourlyObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('hourly') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        for (int i = 0; i < fopt->hhObm.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObm[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=7 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_OBMH] + "/" +
                sYyyy + "/" + sDoy + "/" + sHh;
            else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

            /* it is OK for '*.Z' or '*.gz' format */
            string crxFile = "*_R_" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
            string crxxFile = crxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObm[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            /* get the file list */
            string suffix = ".crx";
            vector<string> crxFiles;
            str.GetFilesAll(sHhDir, suffix, crxFiles);
            char sitName[MAXCHARS];
            for (int i = 0; i < crxFiles.size(); i++)
            {
                if (access(crxFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + "  -d -f " + crxFiles[i];
                    std::system(cmd.c_str());

                    str.StrMid(sitName, crxFiles[i].c_str(), 0, 4);
                    string site = sitName;
                    str.ToLower(site);
                    str.StrMid(sitName, crxFiles[i].c_str(), 0, (int)crxFiles[i].find_last_of('.'));
                    crxFile = sitName;
                    if (access(crxFile.c_str(), 0) == -1) continue;
                    string oFile = site + sDoy + sch + "." + sYy + "o";
                    /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                    cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                    cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                    std::system(cmd.c_str());

                    /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                    cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                    cmd = "rm -rf " + crxFile;
#endif
                    std::system(cmd.c_str());
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObm.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObm[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObm[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string oFile = sitName + sDoy + sch + "." + sYy + "o";
                    string dFile = sitName + sDoy + sch + "." + sYy + "d";
                    if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (strcmp(ftpName.c_str(), "IGN") == 0)
                        {
                            url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        str.ToUpper(sitName);
                        string crxFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_30S_MO.crx";
                        string crxxFile = crxFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        /* extract '*.gz' */
                        string crxgzFile = crxFile + ".gz";
                        cmd = gzipFull + " -d -f " + crxgzFile;
                        std::system(cmd.c_str());
                        string changeFileName;
#ifdef _WIN32  /* for Windows */
                        changeFileName = "rename";
#else          /* for Linux or Mac */
                        changeFileName = "mv";
#endif
                        cmd = changeFileName + " " + crxFile + " " + dFile;
                        std::system(cmd.c_str());
                        if (access(dFile.c_str(), 0) == -1)
                        {
                            /* extract '*.Z' */
                            string crxzFile = crxFile + ".Z";
                            cmd = gzipFull + " -d -f " + crxzFile;
                            std::system(cmd.c_str());

                            cmd = changeFileName + " " + crxFile + " " + dFile;
                            std::system(cmd.c_str());
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                                continue;
                            }
                        }

#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        if (access(oFile.c_str(), 0) == 0)
                        {
                            cout << "*** INFO(FtpUtil::GetHourlyObsMgex): successfully download MGEX hourly observation file " << oFile << endl;

                            /* delete dFile */
                            remove(dFile.c_str());
                        }
                    }
                    else cout << "*** INFO(FtpUtil::GetHourlyObsMgex): MGEX hourly observation file " <<
                        oFile << " or " << dFile << " has existed!" << endl;
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHourlyObsMgex */

/**
* @brief   : GetHrObsMgex - download MGEX RINEX high-rate observation (1s) files (long name "crx") according to 'site.list' file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetHrObsMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* creation of sub-directory ('highrate') */
    char tmpDir[MAXSTRPATH] = { '\0' };
    char sep = (char)FILEPATHSEP;
    sprintf(tmpDir, "%s%c%s", dir, sep, "highrate");
    string subDir = tmpDir;
    if (access(subDir.c_str(), 0) == -1)
    {
        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
        string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
        string cmd = "mkdir -p " + subDir;
#endif
        std::system(cmd.c_str());
    }

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    /* FTP archive: CDDIS, IGN, or WHU */
    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, crx2rnxFull = fopt->crx2rnxFull, qr = fopt->qr;
    if (strlen(fopt->obmOpt) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
    {
        /* download all the MGEX observation files */
        for (int i = 0; i < fopt->hhObm.size(); i++)
        {
            string sHh = str.hh2str(fopt->hhObm[i]);
            char tmpDir[MAXSTRPATH] = { '\0' };
            char sep = (char)FILEPATHSEP;
            sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
            string sHhDir = tmpDir;
            if (access(sHhDir.c_str(), 0) == -1)
            {
                /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + sHhDir;
#endif
                std::system(cmd.c_str());
            }

            /* change directory */
#ifdef _WIN32   /* for Windows */
            _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
            chdir(sHhDir.c_str());
#endif

            string url, cutDirs = " --cut-dirs=8 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" +
                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;

            /* it is OK for '*.Z' or '*.gz' format */
            string crxFile = "*_R_*_15M_01S_MO.crx";
            string crxxFile = crxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
            //std::system(cmd.c_str());

            /* 'a' = 97, 'b' = 98, ... */
            int ii = fopt->hhObm[i] + 97;
            char ch = ii;
            string sch;
            sch.push_back(ch);

            std::vector<string> minuStr = { "00", "15", "30", "45" };
            for (int i = 0; i < minuStr.size(); i++)
            {
                /* get the file list */
                string suffix = minuStr[i] + "_15M_01S_MO.crx";
                vector<string> crxFiles;
                str.GetFilesAll(sHhDir, suffix, crxFiles);
                char sitName[MAXCHARS];
                for (int j = 0; j < crxFiles.size(); j++)
                {
                    if (access(crxFiles[j].c_str(), 0) == 0)
                    {
                        /* extract it */
                        cmd = gzipFull + "  -d -f " + crxFiles[j];
                        std::system(cmd.c_str());

                        str.StrMid(sitName, crxFiles[j].c_str(), 0, 4);
                        string site = sitName;
                        str.ToLower(site);
                        str.StrMid(sitName, crxFiles[j].c_str(), 0, (int)crxFiles[j].find_last_of('.'));
                        crxFile = sitName;
                        if (access(crxFile.c_str(), 0) == -1) continue;
                        string oFile = site + sDoy + sch + minuStr[i] +  "." + sYy + "o";
                        /* convert from 'd' file to 'o' file */
#ifdef _WIN32  /* for Windows */
                        cmd = crx2rnxFull + " " + crxFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                        cmd = "cat " + crxFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                        std::system(cmd.c_str());

                        /* delete 'd' file */
#ifdef _WIN32   /* for Windows */
                        cmd = "del " + crxFile;
#else           /* for Linux or Mac */
                        cmd = "rm -rf " + crxFile;
#endif
                        std::system(cmd.c_str());
                    }
                }
            }
        }
    }
    else  /* the option of full path of site list file is selected */
    {
        /* download the MGEX observation file site-by-site */
        if (access(fopt->obmOpt, 0) == 0)
        {
            ifstream sitLst(fopt->obmOpt);
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHrObsMgex): open site.list = " << fopt->obmOpt << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhObm.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhObm[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhObm[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);

                    std::vector<string> minuStr = { "00", "15", "30", "45" };
                    for (int i = 0; i < minuStr.size(); i++)
                    {
                        str.ToLower(sitName);
                        string oFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "o";
                        string dFile = sitName + sDoy + sch + minuStr[i] + "." + sYy + "d";
                        if (access(oFile.c_str(), 0) == -1 && access(dFile.c_str(), 0) == -1)
                        {
                            string url, cutDirs = " --cut-dirs=8 ";
                            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" +
                                sYyyy + "/" + sDoy + "/" + sYy + "d" + "/" + sHh;
                            else if (strcmp(ftpName.c_str(), "IGN") == 0)
                            {
                                url = _ftpArchive.IGN[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy;
                                cutDirs = " --cut-dirs=6 ";
                            }
                            else url = _ftpArchive.CDDIS[IDX_OBMHR] + "/" + sYyyy + "/" + sDoy + "/" + 
                                sYy + "d" + "/" + sHh;

                            /* it is OK for '*.Z' or '*.gz' format */
                            str.ToUpper(sitName);
                            string crxFile = sitName + "*_R_" + sYyyy + sDoy + sHh + minuStr[i] + "_15M_01S_MO.crx";
                            string crxxFile = crxFile + ".*";
                            string cmd = wgetFull + " " + qr + " -nH -A " + crxxFile + cutDirs + url;
                            std::system(cmd.c_str());

                            /* extract '*.gz' */
                            string crxgzFile = crxFile + ".gz";
                            cmd = gzipFull + " -d -f " + crxgzFile;
                            std::system(cmd.c_str());
                            string changeFileName;
#ifdef _WIN32  /* for Windows */
                            changeFileName = "rename";
#else          /* for Linux or Mac */
                            changeFileName = "mv";
#endif
                            cmd = changeFileName + " " + crxFile + " " + dFile;
                            std::system(cmd.c_str());
                            if (access(dFile.c_str(), 0) == -1)
                            {
                                /* extract '*.Z' */
                                string crxzFile = crxFile + ".Z";
                                cmd = gzipFull + " -d -f " + crxzFile;
                                std::system(cmd.c_str());

                                cmd = changeFileName + " " + crxFile + " " + dFile;
                                std::system(cmd.c_str());
                                if (access(dFile.c_str(), 0) == -1)
                                {
                                    cout << "*** WARNING(FtpUtil::GetDailyObsMgex): failed to download MGEX daily observation file " << oFile << endl;

                                    continue;
                                }
                            }

#ifdef _WIN32  /* for Windows */
                            cmd = crx2rnxFull + " " + dFile + " -f - > " + oFile;
#else          /* for Linux or Mac */
                            cmd = "cat " + dFile + " | " + crx2rnxFull + " -f - > " + oFile;
#endif
                            std::system(cmd.c_str());

                            if (access(oFile.c_str(), 0) == 0)
                            {
                                cout << "*** INFO(FtpUtil::GetHrObsMgex): successfully download MGEX high-rate observation file " << oFile << endl;

                                /* delete dFile */
                                remove(dFile.c_str());
                            }
                        }
                        else cout << "*** INFO(FtpUtil::GetHrObsMgex): MGEX high-rate observation file " << 
                            oFile << " or " << dFile << " has existed!" << endl;
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
} /* end of GetHrObsMgex */

/**
* @brief   : GetNav - download daily GPS, GLONASS and mixed RINEX broadcast ephemeris files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetNav(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string nTyp(fopt->navTyp);
    str.ToLower(nTyp);
    string nOpt(fopt->navOpt);
    str.ToLower(nOpt);
    if (strcmp(nTyp.c_str(), "daily") == 0)
    {
        /* creation of sub-directory ('daily') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        /* change directory */
#ifdef _WIN32   /* for Windows */
        _chdir(subDir.c_str());
#else           /* for Linux or Mac */
        chdir(subDir.c_str());
#endif

        string navFile, nav0File;
        if (strcmp(nOpt.c_str(), "gps") == 0)
        {
            /* GPS broadcast ephemeris file */
            navFile = "brdc" + sDoy + "0." + sYy + "n";
            nav0File = "brdc" + sDoy + "0." + sYy + "n";
        }
        else if (strcmp(nOpt.c_str(), "glo") == 0)
        {
            /* GLONASS broadcast ephemeris file */
            navFile = "brdc" + sDoy + "0." + sYy + "g";
            nav0File = "brdc" + sDoy + "0." + sYy + "g";
        }
        else if (strcmp(nOpt.c_str(), "mixed") == 0)
        {
            /* multi-GNSS broadcast ephemeris file */
            if (strcmp(ftpName.c_str(), "IGN") == 0) navFile = "BRDC00IGN_R_" + sYyyy + sDoy + "0000_01D_MN.rnx";
            else navFile = "BRDC00IGS_R_" + sYyyy + sDoy + "0000_01D_MN.rnx";
            nav0File = "brdm" + sDoy + "0." + sYy + "p";
        }

        if (access(navFile.c_str(), 0) == -1 && access(nav0File.c_str(), 0) == -1)
        {
            string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
            string url, cutDirs = " --cut-dirs=6 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_NAV] + "/" +
                sYyyy + "/brdc";
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_NAV] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=5 ";
            }
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_NAV] + "/" +
                sYyyy + "/brdc";
            else url = _ftpArchive.CDDIS[IDX_NAV] + "/" + sYyyy + "/brdc";

            /* it is OK for '*.Z' or '*.gz' format */
            string navxFile = navFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + cutDirs + url;
            std::system(cmd.c_str());

            string navgzFile = navFile + ".gz", navzFile = navFile + ".Z";
            if (access(navgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + navgzFile;
                std::system(cmd.c_str());
            }
            else if (access(navzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + navzFile;
                std::system(cmd.c_str());
            }
            if (access(navFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetNav): failed to download broadcast ephemeris file " <<
                nav0File << endl;

            if (strcmp(nOpt.c_str(), "mixed") == 0)
            {
                string changeFileName;
#ifdef _WIN32  /* for Windows */
                changeFileName = "rename";
#else          /* for Linux or Mac */
                changeFileName = "mv";
#endif
                cmd = changeFileName + " " + navFile + " " + nav0File;
                std::system(cmd.c_str());
            }

            if (access(nav0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetNav): successfully download broadcast ephemeris file " << nav0File << endl;
        }
        else cout << "*** INFO(FtpUtil::GetNav): broadcast ephemeris file " << nav0File << " or " <<
            navFile << " has existed!" << endl;
    }
    else if (strcmp(nTyp.c_str(), "hourly") == 0)
    {
        /* creation of sub-directory ('hourly') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "hourly");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        string sitFile = fopt->obsOpt;
        if (fopt->getObs) sitFile = fopt->obsOpt;
        else if (fopt->getObm) sitFile = fopt->obmOpt;
        /* download the broadcast ephemeris file site-by-site */
        if (access(sitFile.c_str(), 0) == 0)
        {
            ifstream sitLst(sitFile.c_str());
            if (!sitLst.is_open())
            {
                cerr << "*** ERROR(FtpUtil::GetHourlyObsMgex): open site.list = " << sitFile << " file failed, please check it" << endl;

                return;
            }

            string sitName;
            while (getline(sitLst, sitName))
            {
                for (int i = 0; i < fopt->hhNav.size(); i++)
                {
                    string sHh = str.hh2str(fopt->hhNav[i]);
                    char tmpDir[MAXSTRPATH] = { '\0' };
                    char sep = (char)FILEPATHSEP;
                    sprintf(tmpDir, "%s%c%s", subDir.c_str(), sep, sHh.c_str());
                    string sHhDir = tmpDir;
                    if (access(sHhDir.c_str(), 0) == -1)
                    {
                        /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
                        string cmd = "mkdir " + sHhDir;
#else           /* for Linux or Mac */
                        string cmd = "mkdir -p " + sHhDir;
#endif
                        std::system(cmd.c_str());
                    }

                    /* change directory */
#ifdef _WIN32   /* for Windows */
                    _chdir(sHhDir.c_str());
#else           /* for Linux or Mac */
                    chdir(sHhDir.c_str());
#endif

                    /* 'a' = 97, 'b' = 98, ... */
                    int ii = fopt->hhNav[i] + 97;
                    char ch = ii;
                    string sch;
                    sch.push_back(ch);
                    str.ToLower(sitName);
                    string navnsFile = sitName + sDoy + sch + "." + sYy + "n";  /* short file name */
                    string navgsFile = sitName + sDoy + sch + "." + sYy + "g";
                    
                    str.ToUpper(sitName);
                    string navnlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_GN.rnx";  /* long file name */
                    string navglFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_RN.rnx";
                    string navclFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_CN.rnx";
                    string navelFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_EN.rnx";
                    string navjlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_JN.rnx";
                    string navilFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_IN.rnx";
                    string navmlFile = sitName + "*_R_" + sYyyy + sDoy + sHh + "00_01H_MN.rnx";
                    std::vector<string> navFiles;
                    if (strcmp(nOpt.c_str(), "gps") == 0)
                    {
                        navFiles.push_back(navnsFile);
                        navFiles.push_back(navnlFile);
                    }
                    else if (strcmp(nOpt.c_str(), "glo") == 0)
                    {
                        navFiles.push_back(navgsFile);
                        navFiles.push_back(navglFile);
                    }
                    else if (strcmp(nOpt.c_str(), "bds") == 0)
                    {
                        navFiles.push_back(navclFile);
                    }
                    else if (strcmp(nOpt.c_str(), "gal") == 0)
                    {
                        navFiles.push_back(navelFile);
                    }
                    else if (strcmp(nOpt.c_str(), "qzs") == 0)
                    {
                        navFiles.push_back(navjlFile);
                    }
                    else if (strcmp(nOpt.c_str(), "irn") == 0)
                    {
                        navFiles.push_back(navilFile);
                    }
                    else if (strcmp(nOpt.c_str(), "mixed") == 0)
                    {
                        navFiles.push_back(navmlFile);
                    }
                    else if (strcmp(nOpt.c_str(), "all") == 0)
                    {
                        navFiles.push_back(navnsFile);
                        navFiles.push_back(navnlFile);
                        navFiles.push_back(navgsFile);
                        navFiles.push_back(navglFile);
                        navFiles.push_back(navclFile);
                        navFiles.push_back(navelFile);
                        navFiles.push_back(navjlFile);
                        navFiles.push_back(navilFile);
                        navFiles.push_back(navmlFile);
                    }

                    str.ToLower(sitName);
                    string navns0File = sitName + sDoy + sch + "." + sYy + "n";
                    string navgs0File = sitName + sDoy + sch + "." + sYy + "g";
                    string navnl0File = sitName + sDoy + sch + "." + sYy + "gn";
                    string navgl0File = sitName + sDoy + sch + "." + sYy + "rn";
                    string navcl0File = sitName + sDoy + sch + "." + sYy + "cn";
                    string navel0File = sitName + sDoy + sch + "." + sYy + "en";
                    string navjl0File = sitName + sDoy + sch + "." + sYy + "jn";
                    string navil0File = sitName + sDoy + sch + "." + sYy + "in";
                    string navml0File = sitName + sDoy + sch + "." + sYy + "mn";
                    std::vector<string> nav0Files;
                    if (strcmp(nOpt.c_str(), "gps") == 0)
                    {
                        nav0Files.push_back(navns0File);
                        nav0Files.push_back(navnl0File);
                    }
                    else if (strcmp(nOpt.c_str(), "glo") == 0)
                    {
                        nav0Files.push_back(navgs0File);
                        nav0Files.push_back(navgl0File);
                    }
                    else if (strcmp(nOpt.c_str(), "bds") == 0)
                    {
                        nav0Files.push_back(navcl0File);
                    }
                    else if (strcmp(nOpt.c_str(), "gal") == 0)
                    {
                        nav0Files.push_back(navel0File);
                    }
                    else if (strcmp(nOpt.c_str(), "qzs") == 0)
                    {
                        nav0Files.push_back(navjl0File);
                    }
                    else if (strcmp(nOpt.c_str(), "irn") == 0)
                    {
                        nav0Files.push_back(navil0File);
                    }
                    else if (strcmp(nOpt.c_str(), "mixed") == 0)
                    {
                        nav0Files.push_back(navml0File);
                    }
                    else if (strcmp(nOpt.c_str(), "all") == 0)
                    {
                        nav0Files.push_back(navns0File);
                        nav0Files.push_back(navnl0File);
                        nav0Files.push_back(navgs0File);
                        nav0Files.push_back(navgl0File);
                        nav0Files.push_back(navcl0File);
                        nav0Files.push_back(navel0File);
                        nav0Files.push_back(navjl0File);
                        nav0Files.push_back(navil0File);
                        nav0Files.push_back(navml0File);
                    }
                    
                    for (int i = 0; i < navFiles.size(); i++)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_OBMH] + "/" +
                            sYyyy + "/" + sDoy + "/" + sHh;
                        else if (strcmp(ftpName.c_str(), "IGN") == 0)
                        {
                            url = _ftpArchive.IGN[IDX_OBMH] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else url = _ftpArchive.CDDIS[IDX_OBMH] + "/" + sYyyy + "/" + sDoy + "/" + sHh;

                        /* it is OK for '*.Z' or '*.gz' format */
                        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
                        string navxFile = navFiles[i] + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        /* extract '*.gz' */
                        string navgzFile = navFiles[i] + ".gz";
                        cmd = gzipFull + " -d -f " + navgzFile;
                        std::system(cmd.c_str());
                        string changeFileName;
#ifdef _WIN32  /* for Windows */
                        changeFileName = "rename";
#else          /* for Linux or Mac */
                        changeFileName = "mv";
#endif
                        cmd = changeFileName + " " + navFiles[i] + " " + nav0Files[i];
                        std::system(cmd.c_str());
                        if (access(nav0Files[i].c_str(), 0) == -1)
                        {
                            /* extract '*.Z' */
                            string navzFile = navFiles[i] + ".Z";
                            cmd = gzipFull + " -d -f " + navzFile;
                            std::system(cmd.c_str());

                            cmd = changeFileName + " " + navFiles[i] + " " + nav0Files[i];
                            std::system(cmd.c_str());
                            if (access(nav0Files[i].c_str(), 0) == -1)
                            {
                                cout << "*** WARNING(FtpUtil::GetNav): failed to download hourly broadcast ephemeris file " << 
                                    navFiles[i] << endl;
                            }
                            else
                            {
                                cout << "*** INFO(FtpUtil::GetNav): successfully download hourly broadcast ephemeris file " <<
                                    navFiles[i] << endl;

                                continue;
                            }
                        }
                        else
                        {
                            cout << "*** INFO(FtpUtil::GetNav): successfully download hourly broadcast ephemeris file " <<
                                navFiles[i] << endl;

                            continue;
                        }
                    }
                }
            }

            /* close 'site.list' */
            sitLst.close();
        }
    }
    else if (strcmp(nTyp.c_str(), "RTNAV") == 0)
    {
        /* creation of sub-directory ('daily') */
        char tmpDir[MAXSTRPATH] = { '\0' };
        char sep = (char)FILEPATHSEP;
        sprintf(tmpDir, "%s%c%s", dir, sep, "daily");
        string subDir = tmpDir;
        if (access(subDir.c_str(), 0) == -1)
        {
            /* If the directory does not exist, creat it */
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + subDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + subDir;
#endif
            std::system(cmd.c_str());
        }

        /* change directory */
#ifdef _WIN32   /* for Windows */
        _chdir(subDir.c_str());
#else           /* for Linux or Mac */
        chdir(subDir.c_str());
#endif

        string navFile = "brdm" + sDoy + "z." + sYy + "p";
        if (access(navFile.c_str(), 0) == -1)
        {
            /* download brdmDDDz.YYp.Z file */
            string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
            string url = "ftp://ftp.lrz.de/transfer/steigenb/brdm";

            /* it is OK for '*.Z' or '*.gz' format */
            string navxFile = navFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + navxFile + " --cut-dirs=3 " + url;
            std::system(cmd.c_str());

            string navgzFile = navFile + ".gz", navzFile = navFile + ".Z";
            if (access(navgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + navgzFile;
                std::system(cmd.c_str());
            }
            else if (access(navzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + navzFile;
                std::system(cmd.c_str());
            }

            if (access(navFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetNav): failed to download real-time broadcast ephemeris file " <<
                navFile << endl;
            else if (access(navFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetNav): successfully download real-time broadcast ephemeris file " <<
                navFile << endl;
        }
        else cout << "*** INFO(FtpUtil::GetNav): real-time broadcast ephemeris file " << navFile <<
            " has existed!" << endl;
    }
} /* end of GetNav */

/**
* @brief   : GetOrbClk - download IGS/MGEX precise orbit and clock files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: mode (=1:sp3 downloaded; =2:clk downloadeded)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetOrbClk(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    string sYyyy = str.yyyy2str(yyyy);
    string sDoy = str.doy2str(doy);

    string ac(fopt->orbClkOpt);
    str.ToLower(ac);
    std::vector<string> acIGS = { "igs", "igr", "igu", "cod", "esa", "gfz", "gfu", "grg", "jpl" };
    bool isIGS = false;
    for (int i = 0; i < acIGS.size(); i++)
    {
        if (strcmp(ac.c_str(), acIGS[i].c_str()) == 0) isIGS = true;
    }
    std::vector<string> acMGEX = { "com", "gbm", "grm", "wum", "wuu" };
    bool isMGEX = false;
    for (int i = 0; i < acMGEX.size(); i++)
    {
        if (strcmp(ac.c_str(), acMGEX[i].c_str()) == 0) isMGEX = true;
    }

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (isIGS)  /* IGS products */
    {
        if (strcmp(ac.c_str(), "igu") == 0)
        {
            string url, cutDirs = " --cut-dirs=4 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_SP3] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_SP3] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            for (int i = 0; i < fopt->hhOrbClk.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhOrbClk[i]);
                string sp3File = ac + sWwww + sDow + "_" + sHh + ".sp3";
                if (access(sp3File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }

                    if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download IGS ultra-rapid orbit file " <<
                        sp3File << endl;
                    else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download IGS ultra-rapid orbit file " <<
                        sp3File << endl;

                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): IGS ultra-rapid orbit file " << sp3File <<
                    " has existed!" << endl;
            }
        }
        else if (strcmp(ac.c_str(), "gfu") == 0)
        {
            string url = "ftp://ftp.gfz-potsdam.de/pub/GNSS/products/ultra/w" + sWwww;
            string cutDirs = " --cut-dirs=5 ";
            for (int i = 0; i < fopt->hhOrbClk.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhOrbClk[i]);
                string sp3File = ac + sWwww + sDow + "_" + sHh + ".sp3";
                if (access(sp3File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }

                    if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download GFZ ultra-rapid orbit file " <<
                        sp3File << endl;
                    else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download GFZ ultra-rapid orbit file " <<
                        sp3File << endl;
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): GFZ ultra-rapid orbit file " << sp3File <<
                    " has existed!" << endl;
            }
        }
        else
        {
            string sp3File, clkFile;
            if (strcmp(ac.c_str(), "cod") == 0)
            {
                sp3File = ac + sWwww + sDow + ".eph";
                clkFile = ac + sWwww + sDow + ".clk_05s";
            }
            else if (strcmp(ac.c_str(), "igs") == 0)
            {
                sp3File = ac + sWwww + sDow + ".sp3";
                clkFile = ac + sWwww + sDow + ".clk_30s";
            }
            else
            {
                sp3File = ac + sWwww + sDow + ".sp3";
                clkFile = ac + sWwww + sDow + ".clk";
            }

            string url, cutDirs = " --cut-dirs=4 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_SP3] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_SP3] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SP3] + "/" + sWwww;
            if (mode == 1)       /* SP3 file downloaded */
            {
                if (access(sp3File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }

                    if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download IGS precise orbit file " <<
                        sp3File << endl;
                    else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download IGS precise orbit file " <<
                        sp3File << endl;

                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): IGS precise orbit file " << sp3File <<
                    " has existed!" << endl;
            }
            else if (mode == 2)  /* CLK file downloaded */
            {
                if (access(clkFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string clkxFile = clkFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + clkxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string clkgzFile = clkFile + ".gz", clkzFile = clkFile + ".Z";
                    if (access(clkgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + clkgzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(clkzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + clkzFile;
                        std::system(cmd.c_str());
                    }

                    if (access(clkFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download IGS precise clock file " <<
                        clkFile << endl;
                    else if (access(clkFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download IGS precise clock file " <<
                        clkFile << endl;

                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): IGS precise clock file " << clkFile <<
                    " has existed!" << endl;
            }
        }
    }
    else if (isMGEX)  /* MGEX products */
    {
        string url, cutDirs = " --cut-dirs=5 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;
        else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_SP3M] + "/" + sWwww;
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_SP3M] + "/" + sWwww;
        else url = _ftpArchive.CDDIS[IDX_SP3M] + "/" + sWwww;
        if (strcmp(ac.c_str(), "wuu") == 0)
        {
            for (int i = 0; i < fopt->hhOrbClk.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhOrbClk[i]);
                string sp3File = "WUM0MGXULA_" + sYyyy + sDoy + sHh + "00_01D_05M_ORB.SP3";
                if (access(sp3File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string sp3gzFile = sp3File + ".gz", sp3zFile = sp3File + ".Z";
                    if (access(sp3gzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + sp3gzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(sp3zFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());
                    }

                    if (access(sp3File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download WHU multi-GNSS ultra-rapid orbit file " <<
                        sp3File << endl;
                    else if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download WHU multi-GNSS ultra-rapid orbit file " <<
                        sp3File << endl;
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): WHU multi-GNSS ultra-rapid orbit file " << sp3File <<
                    " has existed!" << endl;
            }
        }
        else
        {
            string sp3File, clkFile, sp30File, clk0File;
            if (strcmp(ac.c_str(), "com") == 0)
            {
                sp3File = "COD0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
                clkFile = "COD0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
                sp30File = "com" + sWwww + sDow + ".sp3";
                clk0File = "com" + sWwww + sDow + ".clk";
            }
            else if (strcmp(ac.c_str(), "gbm") == 0)
            {
                sp3File = "GFZ0MGXRAP_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
                clkFile = "GFZ0MGXRAP_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
                sp30File = "gbm" + sWwww + sDow + ".sp3";
                clk0File = "gbm" + sWwww + sDow + ".clk";
            }
            else if (strcmp(ac.c_str(), "grm") == 0)
            {
                sp3File = "GRG0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
                clkFile = "GRG0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
                sp30File = "grm" + sWwww + sDow + ".sp3";
                clk0File = "grm" + sWwww + sDow + ".clk";
            }
            else if (strcmp(ac.c_str(), "wum") == 0)
            {
                sp3File = "WUM0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_ORB.SP3";
                clkFile = "WUM0MGXFIN_" + sYyyy + sDoy + "0000_01D_" + "*_CLK.CLK";
                sp30File = "wum" + sWwww + sDow + ".sp3";
                clk0File = "wum" + sWwww + sDow + ".clk";
            }

            if (mode == 1)       /* SP3 file downloaded */
            {
                if (access(sp30File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string sp3xFile = sp3File + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + sp3xFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    string sp3gzFile = sp3File + ".gz";
                    cmd = gzipFull + " -d -f " + sp3gzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + sp3File + " " + sp30File;
                    std::system(cmd.c_str());
                    if (access(sp30File.c_str(), 0) == -1)
                    {
                        /* extract '*.Z' */
                        string sp3zFile = sp3File + ".Z";
                        cmd = gzipFull + " -d -f " + sp3zFile;
                        std::system(cmd.c_str());

                        cmd = changeFileName + " " + sp3File + " " + sp30File;
                        std::system(cmd.c_str());

                        if (access(sp30File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download MGEX precise orbit file " <<
                            sp30File << endl;
                    }

                    if (access(sp30File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download MGEX precise orbit file " << sp30File << endl;
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): MGEX precise orbit file " << sp30File <<
                    " has existed!" << endl;
            }
            else if (mode == 2)  /* CLK file downloaded */
            {
                if (access(clk0File.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string clkxFile = clkFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + clkxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    /* extract '*.gz' */
                    string clkgzFile = clkFile + ".gz";
                    cmd = gzipFull + " -d -f " + clkgzFile;
                    std::system(cmd.c_str());
                    string changeFileName;
#ifdef _WIN32  /* for Windows */
                    changeFileName = "rename";
#else          /* for Linux or Mac */
                    changeFileName = "mv";
#endif
                    cmd = changeFileName + " " + clkFile + " " + clk0File;
                    std::system(cmd.c_str());
                    if (access(clk0File.c_str(), 0) == -1)
                    {
                        /* extract '*.Z' */
                        string clkzFile = clkFile + ".Z";
                        cmd = gzipFull + " -d -f " + clkzFile;
                        std::system(cmd.c_str());

                        cmd = changeFileName + " " + clkFile + " " + clk0File;
                        std::system(cmd.c_str());

                        if (access(clkFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetOrbClk): failed to download MGEX precise clock file " <<
                            clk0File << endl;
                    }

                    if (access(clk0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetOrbClk): successfully download MGEX precise clock file " <<
                        clk0File << endl;
                }
                else cout << "*** INFO(FtpUtil::GetOrbClk): MGEX precise clock file " << clk0File <<
                    " has existed!" << endl;
            }
        }
    }
} /* end of GetOrbClk */

/**
* @brief   : GetEop - download earth rotation parameter files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetEop(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ac(fopt->eopOpt);
    str.ToLower(ac);
    std::vector<string> acIGS = { "igs", "igr", "igu", "cod", "esa", "gfz", "gfu", "grg", "jpl" };;
    bool isIGS = false;
    for (int i = 0; i < acIGS.size(); i++)
    {
        if (strcmp(ac.c_str(), acIGS[i].c_str()) == 0) isIGS = true;
    }

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    if (isIGS)  /* IGS products */
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        if (strcmp(ac.c_str(), "igu") == 0)
        {
            for (int i = 0; i < fopt->hhEop.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhEop[i]);
                string eopFile = ac + sWwww + sDow + "_" + sHh + ".erp";
                if (access(eopFile.c_str(), 0) == -1)
                {
                    /* download the EOP file */
                    string url, cutDirs = " --cut-dirs=4 ";
                    if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;
                    else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_EOP] + "/" + sWwww;
                    else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_EOP] + "/" + sWwww;
                    else url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;

                    /* it is OK for '*.Z' or '*.gz' format */
                    string eopxFile = eopFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + eopxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string eopgzFile = eopFile + ".gz", eopzFile = eopFile + ".Z";
                    if (access(eopgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + eopgzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(eopzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + eopzFile;
                        std::system(cmd.c_str());
                    }

                    if (access(eopFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetEop): failed to download IGU EOP file " <<
                        eopFile << endl;
                    else if (access(eopFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetEop): successfully download IGU EOP file " <<
                        eopFile << endl;

                    /* delete some temporary directories */
                    string tmpDir = "repro3";
                    if (access(tmpDir.c_str(), 0) == 0)
                    {
#ifdef _WIN32  /* for Windows */
                        cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                        cmd = "rm -rf " + tmpDir;
#endif
                        std::system(cmd.c_str());
                    }
                }
                else cout << "*** INFO(FtpUtil::GetEop): IGU EOP file " << eopFile << " has existed!" << endl;
            }
        }
        else if (strcmp(ac.c_str(), "gfu") == 0)
        {
            string url = "ftp://ftp.gfz-potsdam.de/pub/GNSS/products/ultra/w" + sWwww;
            string cutDirs = " --cut-dirs=5 ";
            for (int i = 0; i < fopt->hhEop.size(); i++)
            {
                string sHh = str.hh2str(fopt->hhEop[i]);
                string eopFile = ac + sWwww + sDow + "_" + sHh + ".erp";
                if (access(eopFile.c_str(), 0) == -1)
                {
                    /* it is OK for '*.Z' or '*.gz' format */
                    string eopxFile = eopFile + ".*";
                    string cmd = wgetFull + " " + qr + " -nH -A " + eopxFile + cutDirs + url;
                    std::system(cmd.c_str());

                    string eopgzFile = eopFile + ".gz", eopzFile = eopFile + ".Z";
                    if (access(eopgzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.gz' */
                        cmd = gzipFull + " -d -f " + eopgzFile;
                        std::system(cmd.c_str());
                    }
                    else if (access(eopzFile.c_str(), 0) == 0)
                    {
                        /* extract '*.Z' */
                        cmd = gzipFull + " -d -f " + eopzFile;
                        std::system(cmd.c_str());
                    }

                    if (access(eopFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetEop): failed to download GFU EOP file " <<
                        eopFile << endl;
                    else if (access(eopFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetEop): successfully download GFU EOP file " <<
                        eopFile << endl;
                }
                else cout << "*** INFO(FtpUtil::GetEop): GFU EOP file " << eopFile << " has existed!" << endl;
            }
        }
        else
        {
            string eopFile;
            if (strcmp(ac.c_str(), "igr") == 0) eopFile = ac + sWwww + sDow + ".erp";
            else eopFile = ac + sWwww + "7.erp";
            if (access(eopFile.c_str(), 0) == -1)
            {
                /* download the EOP file */
                string url, cutDirs = " --cut-dirs=4 ";
                if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;
                else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_EOP] + "/" + sWwww;
                else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_EOP] + "/" + sWwww;
                else url = _ftpArchive.CDDIS[IDX_EOP] + "/" + sWwww;

                /* it is OK for '*.Z' or '*.gz' format */
                string eopxFile = eopFile + ".*";
                string cmd = wgetFull + " " + qr + " -nH -A " + eopxFile + cutDirs + url;
                std::system(cmd.c_str());

                string eopgzFile = eopFile + ".gz", eopzFile = eopFile + ".Z";
                if (access(eopgzFile.c_str(), 0) == 0)
                {
                    /* extract '*.gz' */
                    cmd = gzipFull + " -d -f " + eopgzFile;
                    std::system(cmd.c_str());
                }
                else if (access(eopzFile.c_str(), 0) == 0)
                {
                    /* extract '*.Z' */
                    cmd = gzipFull + " -d -f " + eopzFile;
                    std::system(cmd.c_str());
                }

                if (access(eopFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetEop): failed to download IGS EOP file " <<
                    eopFile << endl;
                else if (access(eopFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetEop): successfully download IGS EOP file " <<
                    eopFile << endl;

                /* delete some temporary directories */
                string tmpDir = "repro3";
                if (access(tmpDir.c_str(), 0) == 0)
                {
#ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir;
#endif
                    std::system(cmd.c_str());
                }
            }
            else cout << "*** INFO(FtpUtil::GetEop): IGS EOP file " << eopFile << " has existed!" << endl;
        }
    }
} /* end of GetOrbClkEop */

/**
* @brief   : GetSnx - download IGS weekly SINEX file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetSnx(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww;
    int dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string snx0File = "igs" + sWwww + ".snx";
    if (access(snx0File.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;        
        string url, cutDirs = " --cut-dirs=4 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
        else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_SNX] + "/" + sWwww;
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_SNX] + "/" + sWwww;
        else url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
        
        /* it is OK for '*.Z' or '*.gz' format */
        string snxFile = "igs*P" + sWwww + ".snx";
        string snxxFile = snxFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + snxxFile + cutDirs + url;
        std::system(cmd.c_str());

        /* extract '*.Z' */
        string snxzFile = snxFile + ".Z";
        cmd = gzipFull + " -d -f " + snxzFile;
        std::system(cmd.c_str());
        string changeFileName;
#ifdef _WIN32  /* for Windows */
        changeFileName = "rename";
#else          /* for Linux or Mac */
        changeFileName = "mv";
#endif
        cmd = changeFileName + " " + snxFile + " " + snx0File;
        std::system(cmd.c_str());
        if (access(snx0File.c_str(), 0) == -1)
        {
            /* extract '*.Z' */
            string snxgzFile = snxFile + ".gz";
            cmd = gzipFull + " -d -f " + snxgzFile;
            std::system(cmd.c_str());

            cmd = changeFileName + " " + snxFile + " " + snx0File;
            std::system(cmd.c_str());
            if (access(snx0File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetSnx): failed to download IGS weekly SINEX file " <<
                snx0File << endl;
        }

        /* If the weekly SINEX file does not exist,  we use the daily SINEX file instead */
        if (access(snx0File.c_str(), 0) == -1)
        {
            string url, cutDirs = " --cut-dirs=4 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_SNX] + "/" + sWwww;
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_SNX] + "/" + sWwww;
            else url = _ftpArchive.CDDIS[IDX_SNX] + "/" + sWwww;
            
            /* it is OK for '*.Z' or '*.gz' format */
            string snxFile = "igs*P" + sWwww + sDow + ".snx";
            string snxxFile = snxFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + snxxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* extract '*.Z' */
            string snxzFile = snxFile + ".Z";
            cmd = gzipFull + " -d -f " + snxzFile;
            std::system(cmd.c_str());
            string changeFileName;
#ifdef _WIN32  /* for Windows */
            changeFileName = "rename";
#else          /* for Linux or Mac */
            changeFileName = "mv";
#endif
            cmd = changeFileName + " " + snxFile + " " + snx0File;
            std::system(cmd.c_str());
            if (access(snx0File.c_str(), 0) == -1)
            {
                /* extract '*.Z' */
                string snxgzFile = snxFile + ".gz";
                cmd = gzipFull + " -d -f " + snxgzFile;
                std::system(cmd.c_str());

                cmd = changeFileName + " " + snxFile + " " + snx0File;
                std::system(cmd.c_str());
                if (access(snx0File.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetSnx): failed to download IGS weekly SINEX file " <<
                    snx0File << endl;
            }
        }

        if (access(snx0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetSnx): successfully download IGS weekly SINEX file " << 
            snx0File << endl;

        /* delete some temporary directories */
        string tmpDir = "repro3";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetSnx): IGS weekly SINEX file " << snx0File << " has existed!" << endl;
} /* end of GetSnx */

/**
* @brief   : GetDcbMgex - download MGEX daily DCB file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDcbMgex(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string dcbFile = "CAS0MGXRAP_" + sYyyy + sDoy + "0000_01D_01D_DCB.BSX";
    if (access(dcbFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=5 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_DCBM] + "/" + sYyyy;
        else if (strcmp(ftpName.c_str(), "IGN") == 0)
        {
            url = _ftpArchive.IGN[IDX_DCBM] + "/" + sYyyy;
            cutDirs = " --cut-dirs=6 ";
        }
        else if (strcmp(ftpName.c_str(), "WHU") == 0)
        {
            url = _ftpArchive.WHU[IDX_DCBM] + "/" + sYyyy;
            cutDirs = " --cut-dirs=6 ";
        }
        else url = _ftpArchive.CDDIS[IDX_DCBM] + "/" + sYyyy;

        /* it is OK for '*.Z' or '*.gz' format */
        string dcbxFile = dcbFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dcbxFile + cutDirs + url;
        std::system(cmd.c_str());

        string dcbgzFile = dcbFile + ".gz", dcbzFile = dcbFile + ".Z";
        if (access(dcbgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + dcbgzFile;
            std::system(cmd.c_str());
        }
        else if (access(dcbzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + dcbzFile;
            std::system(cmd.c_str());
        }

        if (access(dcbFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetDcbMgex): failed to download multi-GNSS DCB file " <<
            dcbFile << endl;
        else if (access(dcbFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetDcbMgex): successfully download multi-GNSS DCB file " <<
            dcbFile << endl;
    }
    else cout << "*** INFO(FtpUtil::GetDcbMgex): multi-GNSS DCB file " << dcbFile << " has existed!" << endl;
} /* end of GetDcbMgex */

/**
* @brief   : GetDcbCode - download daily P1-P2, P1-C1, and P2-C2 DCB files from CODE
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: dType (DCB type, i.e., "P1P2", "P1C1", or "P2C2")
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetDcbCode(gtime_t ts, const char dir[], const char dType[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy;
    tu.time2yrdoy(ts, &yyyy, nullptr);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    double date[6];
    tu.time2ymdhms(ts, date);
    int mm = (int)date[1];
    string sMm = str.mm2str(mm);

    string dcbFile, dcb0File;
    string type(dType);
    str.ToUpper(type);
    if (strcmp(type.c_str(), "P1P2") == 0)
    {
        dcbFile = "P1P2" + sYy + sMm + ".DCB";
        dcb0File = "P1P2" + sYy + sMm + ".DCB";
    }
    else if (strcmp(type.c_str(), "P1C1") == 0)
    {
        dcbFile = "P1C1" + sYy + sMm + ".DCB";
        dcb0File = "P1C1" + sYy + sMm + ".DCB";
    }
    else if (strcmp(type.c_str(), "P2C2") == 0)
    {
        dcbFile = "P2C2" + sYy + sMm + "_RINEX.DCB";
        dcb0File = "P2C2" + sYy + sMm + ".DCB";
    }
    
    if (access(dcbFile.c_str(), 0) == -1 && access(dcb0File.c_str(), 0) == -1)
    {
        /* download DCB file */
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url = "ftp://ftp.aiub.unibe.ch/CODE/" + sYyyy;

        /* it is OK for '*.Z' or '*.gz' format */
        string dcbxFile = dcbFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + dcbxFile + " --cut-dirs=2 " + url;
        std::system(cmd.c_str());

        string dcbgzFile = dcbFile + ".gz", dcbzFile = dcbFile + ".Z";
        if (access(dcbgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + dcbgzFile;
            std::system(cmd.c_str());
        }
        else if (access(dcbzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + dcbzFile;
            std::system(cmd.c_str());
        }

        if (access(dcbFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetDcbCode): failed to download CODE DCB file " <<
            dcbFile << endl;

        if (strcmp(type.c_str(), "P2C2") == 0)
        {
            string changeFileName;
#ifdef _WIN32  /* for Windows */
            changeFileName = "rename";
#else          /* for Linux or Mac */
            changeFileName = "mv";
#endif
            cmd = changeFileName + " " + dcbFile + " " + dcb0File;
            std::system(cmd.c_str());
        }

        if (access(dcb0File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetDcbCode): successfully download CODE DCB file " << dcb0File << endl;
    }
    else cout << "*** INFO(FtpUtil::GetDcbCode): CODE DCB file " << dcb0File << " or " << dcbFile <<
        " has existed!" << endl;
} /* end of GetDcbCode */

/**
* @brief   : GetIono - download daily GIM file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetIono(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string ac(fopt->ionOpt);
    str.ToLower(ac);
    string ionFile = ac + "g" + sDoy + "0." + sYy + "i";
    if (access(ionFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=6 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_ION] + "/" + 
            sYyyy + "/" + sDoy;
        else url = _ftpArchive.CDDIS[IDX_ION] + "/" + sYyyy + "/" + sDoy;

        /* it is OK for '*.Z' or '*.gz' format */
        string ionxFile = ionFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + ionxFile + cutDirs + url;
        std::system(cmd.c_str());

        string iongzFile = ionFile + ".gz", ionzFile = ionFile + ".Z";
        if (access(iongzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + iongzFile;
            std::system(cmd.c_str());
        }
        else if (access(ionzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + ionzFile;
            std::system(cmd.c_str());
        }

        if (access(ionFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetIono): failed to download GIM file " <<
            ionFile << endl;
        else if (access(ionFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetIono): successfully download GIM file " <<
            ionFile << endl;

        /* delete some temporary directories */
        string tmpDir = "topex";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetIono): GIM file " << ionFile << " has existed!" << endl;
} /* end of GetIono */

/**
* @brief   : GetRoti - download daily Rate of TEC index (ROTI) file
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRoti(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);

    string ftpName = fopt->ftpFrom;
    str.ToUpper(ftpName);
    string rotFile = "roti" + sDoy + "0." + sYy + "f";
    if (access(rotFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string url, cutDirs = " --cut-dirs=6 ";
        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else if (strcmp(ftpName.c_str(), "IGN") == 0) url = _ftpArchive.IGN[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_ROTI] + "/" +
            sYyyy + "/" + sDoy;
        else url = _ftpArchive.CDDIS[IDX_ROTI] + "/" + sYyyy + "/" + sDoy;

        /* it is OK for '*.Z' or '*.gz' format */
        string rotxFile = rotFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH -A " + rotxFile + cutDirs + url;
        std::system(cmd.c_str());

        string rotgzFile = rotFile + ".gz", rotzFile = rotFile + ".Z";
        if (access(rotgzFile.c_str(), 0) == 0)
        {
            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + rotgzFile;
            std::system(cmd.c_str());
        }
        else if (access(rotzFile.c_str(), 0) == 0)
        {
            /* extract '*.Z' */
            cmd = gzipFull + " -d -f " + rotzFile;
            std::system(cmd.c_str());
        }

        if (access(rotFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetRoti): failed to download ROTI file " <<
            rotFile << endl;
        else if (access(rotFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRoti): successfully download ROTI file " <<
            rotFile << endl;

        /* delete some temporary directories */
        string tmpDir = "topex";
        if (access(tmpDir.c_str(), 0) == 0)
        {
#ifdef _WIN32  /* for Windows */
            cmd = "rd /s /q " + tmpDir;
#else          /* for Linux or Mac */
            cmd = "rm -rf " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
    }
    else cout << "*** INFO(FtpUtil::GetRoti): ROTI file " << rotFile << " has existed!" << endl;
} /* end of GetRoti */

/**
* @brief   : GetTrop - download daily tropospheric product file from CDDIS and/or CODE
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetTrop(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute day of year */
    TimeUtil tu;
    int yyyy, doy;
    tu.time2yrdoy(ts, &yyyy, &doy);
    int yy = tu.yyyy2yy(yyyy);
    StringUtil str;
    string sYyyy = str.yyyy2str(yyyy);
    string sYy = str.yy2str(yy);
    string sDoy = str.doy2str(doy);
    int wwww;
    int dow = tu.time2gpst(ts, &wwww, nullptr);
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string ac(fopt->trpOpt);
    str.ToLower(ac);
    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (strcmp(ac.c_str(), "igs") == 0)
    {
        string ftpName = fopt->ftpFrom;
        str.ToUpper(ftpName);
        string sitFile = fopt->obsOpt;
        if (fopt->getObs) sitFile = fopt->obsOpt;
        else if (fopt->getObm) sitFile = fopt->obmOpt;
        if (strlen(sitFile.c_str()) < 9)  /* the option of 'all' is selected; the length of "site.list" is nine */
        {
            /* download all the IGS ZPD files */
            string url, cutDirs = " --cut-dirs=7 ";
            if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_ZTD] + "/" +
                sYyyy + "/" + sDoy;
            else if (strcmp(ftpName.c_str(), "IGN") == 0)
            {
                url = _ftpArchive.IGN[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;
                cutDirs = " --cut-dirs=6 ";
            }
            else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_ZTD] + "/" +
                sYyyy + "/" + sDoy;
            else url = _ftpArchive.CDDIS[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;

            /* it is OK for '*.Z' or '*.gz' format */
            string zpdFile = "*" + sDoy + "0." + sYy + "zpd";
            string zpdxFile = zpdFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + zpdxFile + cutDirs + url;
            std::system(cmd.c_str());

            /* get the file list */
            string suffix = "." + sYy + "zpd";
            vector<string> zpdFiles;
            str.GetFilesAll(dir, suffix, zpdFiles);
            for (int i = 0; i < zpdFiles.size(); i++)
            {
                if (access(zpdFiles[i].c_str(), 0) == 0)
                {
                    /* extract it */
                    cmd = gzipFull + " -d -f " + zpdFiles[i];
                    std::system(cmd.c_str());
                }
            }
        }
        else  /* the option of full path of site list file is selected */
        {
            /* download the IGS ZPD file site-by-site */
            if (access(sitFile.c_str(), 0) == 0)
            {
                ifstream sitLst(sitFile.c_str());
                if (!sitLst.is_open())
                {
                    cerr << "*** ERROR(FtpUtil::GetTrop): open site.list = " << sitFile << " file failed, please check it" << endl;

                    return;
                }

                string sitName;
                while (getline(sitLst, sitName))
                {
                    str.ToLower(sitName);
                    string zpdFile = sitName + sDoy + "0." + sYy + "zpd";
                    if (access(zpdFile.c_str(), 0) == -1)
                    {
                        string url, cutDirs = " --cut-dirs=7 ";
                        if (strcmp(ftpName.c_str(), "CDDIS") == 0) url = _ftpArchive.CDDIS[IDX_ZTD] + "/" +
                            sYyyy + "/" + sDoy;
                        else if (strcmp(ftpName.c_str(), "IGN") == 0)
                        {
                            url = _ftpArchive.IGN[IDX_ZTD] + "/" + sYyyy + "/" + sDoy;
                            cutDirs = " --cut-dirs=6 ";
                        }
                        else if (strcmp(ftpName.c_str(), "WHU") == 0) url = _ftpArchive.WHU[IDX_ZTD] + "/" +
                            sYyyy + "/" + sDoy;
                        
                        /* it is OK for '*.Z' or '*.gz' format */
                        string zpdxFile = zpdFile + ".*";
                        string cmd = wgetFull + " " + qr + " -nH -A " + zpdxFile + cutDirs + url;
                        std::system(cmd.c_str());

                        string zpdgzFile = zpdFile + ".gz", zpdzFile = zpdFile + ".Z";
                        if (access(zpdgzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.gz' */
                            cmd = gzipFull + " -d -f " + zpdgzFile;
                            std::system(cmd.c_str());
                        }
                        else if (access(zpdzFile.c_str(), 0) == 0)
                        {
                            /* extract '*.Z' */
                            cmd = gzipFull + " -d -f " + zpdzFile;
                            std::system(cmd.c_str());
                        }

                        if (access(zpdFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetTrop): failed to download IGS tropospheric product file " <<
                            zpdFile << endl;
                        else if (access(zpdFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetTrop): successfully download IGS tropospheric product file " <<
                            zpdFile << endl;
                    }
                    else cout << "*** INFO(FtpUtil::GetTrop): IGS tropospheric product file " << zpdFile << 
                        " has existed!" << endl;
                }

                /* close 'site.list' */
                sitLst.close();
            }
        }
    }
    else if (strcmp(ac.c_str(), "cod") == 0)
    {
        string trpFile = "COD" + sWwww + sDow + ".TRO";
        if (access(trpFile.c_str(), 0) == -1)
        {
            string url = "ftp://ftp.aiub.unibe.ch/CODE/" + sYyyy;
            string cutDirs = " --cut-dirs=2 ";

            /* it is OK for '*.Z' or '*.gz' format */
            string trpxFile = trpFile + ".*";
            string cmd = wgetFull + " " + qr + " -nH -A " + trpxFile + cutDirs + url;
            std::system(cmd.c_str());

            string trpgzFile = trpFile + ".gz", trpzFile = trpFile + ".Z";
            if (access(trpgzFile.c_str(), 0) == 0)
            {
                /* extract '*.gz' */
                cmd = gzipFull + " -d -f " + trpgzFile;
                std::system(cmd.c_str());
            }
            else if (access(trpzFile.c_str(), 0) == 0)
            {
                /* extract '*.Z' */
                cmd = gzipFull + " -d -f " + trpzFile;
                std::system(cmd.c_str());
            }

            if (access(trpFile.c_str(), 0) == -1) cout << "*** INFO(FtpUtil::GetTrop): failed to download CODE tropospheric product file " <<
                trpFile << endl;
            else if (access(trpFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetTrop): successfully download CODE tropospheric product file " <<
                trpFile << endl;
        }
        else cout << "*** INFO(FtpUtil::GetTrop): CODE tropospheric product file " << trpFile <<
            " has existed!" << endl;
    }
} /* end of GetTrop */

/**
* @brief   : GetRtOrbClkCNT - download real-time precise orbit and clock products from CNES offline files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: mode (=1:sp3 downloaded; =2:clk downloadeded)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRtOrbClkCNT(gtime_t ts, const char dir[], int mode, const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
    if (mode == 1)       /* SP3 file downloaded */
    {
        string sp3File = "cnt" + sWwww + sDow + ".sp3";
        if (access(sp3File.c_str(), 0) == -1)
        {
            string sp3gzFile = sp3File + ".gz";
            string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + sp3gzFile;
            string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
            std::system(cmd.c_str());

            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + sp3gzFile;
            std::system(cmd.c_str());
            if (access(sp3File.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): successfully download CNES real-time precise orbit file " << 
                sp3File << endl;
            else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): failed to download CNES real-time precise orbit file " <<
                sp3File << endl;

            /* delete some temporary directories */
            std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
            for (int i = 0; i < tmpDir.size(); i++)
            {
                if (access(tmpDir[i].c_str(), 0) == 0)
                {
#ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir[i];
#endif
                    std::system(cmd.c_str());
                }
            }
        }
        else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): CNES real-time precise orbit file " << sp3File << 
            " has existed!" << endl;
    }
    else if (mode == 2)  /* CLK file downloaded */
    {
        string clkFile = "cnt" + sWwww + sDow + ".clk";
        if (access(clkFile.c_str(), 0) == -1)
        {
            string clkgzFile = clkFile + ".gz";
            string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + clkgzFile;
            string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
            std::system(cmd.c_str());

            /* extract '*.gz' */
            cmd = gzipFull + " -d -f " + clkgzFile;
            std::system(cmd.c_str());
            if (access(clkFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): successfully download CNES real-time precise clock file " <<
                clkFile << endl;
            else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): failed to download CNES real-time precise clock file " <<
                clkFile << endl;

            /* delete some temporary directories */
            std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
            for (int i = 0; i < tmpDir.size(); i++)
            {
                if (access(tmpDir[i].c_str(), 0) == 0)
                {
#ifdef _WIN32  /* for Windows */
                    cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                    cmd = "rm -rf " + tmpDir[i];
#endif
                    std::system(cmd.c_str());
                }
            }
        }
        else cout << "*** INFO(FtpUtil::GetRtOrbClkCNT): CNES real-time precise clock file " << clkFile << 
            " has existed!" << endl;
    }
} /* end of GetRtOrbClkCNT */

/**
* @brief   : GetRtBiasCNT - download real-time code and phase bias products from CNES offline files
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetRtBiasCNT(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    /* compute GPS week and day of week */
    TimeUtil tu;
    int wwww, dow;
    dow = tu.time2gpst(ts, &wwww, nullptr);
    StringUtil str;
    string sWwww = str.wwww2str(wwww);
    string sDow(to_string(dow));

    /* code and phase bias file downloaded */
    string biaFile = "cnt" + sWwww + sDow + ".bia";
    if (access(biaFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, gzipFull = fopt->gzipFull, qr = fopt->qr;
        string biagzFile = biaFile + ".gz";
        string url = "http://www.ppp-wizard.net/products/REAL_TIME/" + biagzFile;
        string cutDirs = " --cut-dirs=2 ";

        /* it is OK for '*.Z' or '*.gz' format */
        string biaxFile = biaFile + ".*";
        string cmd = wgetFull + " " + qr + " -nH --cut-dirs=2 " + url;
        std::system(cmd.c_str());

        /* extract '*.gz' */
        cmd = gzipFull + " -d -f " + biagzFile;
        std::system(cmd.c_str());
        if (access(biaFile.c_str(), 0) == 0) cout << "*** INFO(GetRtBiasCNT): successfully download CNES real-time code and phase bias file " << 
            biaFile << endl;
        else cout << "*** INFO(FtpUtil::GetRtBiasCNT): failed to download CNES real-time code and phase bias file " <<
            biaFile << endl;

        /* delete some temporary directories */
        std::vector<string> tmpDir = { "FORMAT_BIAIS_OFFI1", "FORMATBIAS_OFF_v1" };
        for (int i = 0; i < tmpDir.size(); i++)
        {
            if (access(tmpDir[i].c_str(), 0) == 0)
            {
#ifdef _WIN32  /* for Windows */
                cmd = "rd /s /q " + tmpDir[i];
#else          /* for Linux or Mac */
                cmd = "rm -rf " + tmpDir[i];
#endif
                std::system(cmd.c_str());
            }
        }
    }
    else cout << "*** INFO(FtpUtil::GetRtBiasCNT): CNES real-time code and phase bias file " << biaFile <<
        " has existed!" << endl;
} /* end of GetRtBiasCNT */

/**
* @brief   : GetAntexIGS - download ANTEX file from IGS
* @param[I]: ts (start time)
* @param[I]: dir (data directory)
* @param[I]: fopt (FTP options)
* @param[O]: none
* @return  : none
* @note    :
**/
void FtpUtil::GetAntexIGS(gtime_t ts, const char dir[], const ftpopt_t *fopt)
{
    /* change directory */
#ifdef _WIN32   /* for Windows */
    _chdir(dir);
#else           /* for Linux or Mac */
    chdir(dir);
#endif

    string atxFile("igs14.atx");
    if (access(atxFile.c_str(), 0) == -1)
    {
        string wgetFull = fopt->wgetFull, qr = fopt->qr;
        string url = "https://files.igs.org/pub/station/general/" + atxFile;
        string cmd = wgetFull + " " + qr + " -nH --cut-dirs=3 " + url;
        std::system(cmd.c_str());

        if (access(atxFile.c_str(), 0) == 0) cout << "*** INFO(FtpUtil::GetAntexIGS): successfully download IGS ANTEX file " << atxFile << endl;
    }
    else cout << "*** INFO(FtpUtil::GetAntexIGS): IGS ANTEX file " << atxFile << " has existed!" << endl;
} /* end of GetAntexIGS */

/**
* @brief     : FtpDownload - GNSS data downloading via FTP
* @param[I]  : popt (processing options)
* @param[I/O]: fopt (FTP options)
* @return    : none
* @note      :
**/
void FtpUtil::FtpDownload(const prcopt_t *popt, ftpopt_t *fopt)
{
    /* Get FTP archive for CDDIS, IGN, or WHU */
    init();

    /* setting of the third-party softwares (.i.e, wget, gzip, crx2rnx etc.) */
    StringUtil str;
    if (fopt->isPath3party)  /* the path of third-party softwares is NOT set in the environmental variable */
    {
        char cmdTmp[MAXCHARS] = { '\0' };
        char sep = (char)FILEPATHSEP;

        /* for wget */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "wget");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->wgetFull, cmdTmp);

        /* for gzip */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "gzip");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->gzipFull, cmdTmp);

        /* for crx2rnx */
        sprintf(cmdTmp, "%s%c%s", fopt->dir3party, sep, "crx2rnx");
        str.TrimSpace(cmdTmp);
        str.CutFilePathSep(cmdTmp);
        strcpy(fopt->crx2rnxFull, cmdTmp);
    }
    else  /* the path of third-party softwares is set in the environmental variable */
    {
        str.SetStr(fopt->wgetFull, "wget", 5);
        str.SetStr(fopt->gzipFull, "gzip", 5);
        str.SetStr(fopt->crx2rnxFull, "crx2rnx", 8);
    }

    /* "-qr" (printInfoWget == false, not print information of 'wget') or "r" (printInfoWget == true, print information of 'wget') in 'wget' command line */
    if (fopt->printInfoWget) str.SetStr(fopt->qr, "-r", 3);
    else str.SetStr(fopt->qr, "-qr", 4);

    /* IGS observation (short name 'd') downloaded */
    if (fopt->getObs)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obsDir, 0) == -1)
        {
            string tmpDir = popt->obsDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obsTyp, "daily") == 0) GetDailyObsIgs(popt->ts, popt->obsDir, fopt);
        else if (strcmp(fopt->obsTyp, "hourly") == 0) GetHourlyObsIgs(popt->ts, popt->obsDir, fopt);
        else if (strcmp(fopt->obsTyp, "highrate") == 0) GetHrObsIgs(popt->ts, popt->obsDir, fopt);
    }

    /* MGEX observation (long name 'crx') downloaded */
    if (fopt->getObm)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->obmDir, 0) == -1)
        {
            string tmpDir = popt->obmDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        if (strcmp(fopt->obmTyp, "daily") == 0) GetDailyObsMgex(popt->ts, popt->obmDir, fopt);
        else if (strcmp(fopt->obmTyp, "hourly") == 0) GetHourlyObsMgex(popt->ts, popt->obmDir, fopt);
        else if (strcmp(fopt->obmTyp, "highrate") == 0) GetHrObsMgex(popt->ts, popt->obmDir, fopt);
    }

    /* broadcast ephemeris downloaded */
    if (fopt->getNav)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->navDir, 0) == -1)
        {
            string tmpDir = popt->navDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetNav(popt->ts, popt->navDir, fopt);
    }

    /* precise orbit and clock product downloaded for the current day */
    if (fopt->getOrbClk)
    {
        /* If the directory 'sp3' does not exist, creat it */
        if (access(popt->sp3Dir, 0) == -1)
        {
            string tmpDir = popt->sp3Dir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }
        GetOrbClk(popt->ts, popt->sp3Dir, 1, fopt);  /* sp3 */

        if (strcmp(fopt->orbClkOpt, "igu") != 0 && strcmp(fopt->orbClkOpt, "gfu") != 0 && strcmp(fopt->orbClkOpt, "wuu") != 0)
        {
            /* If the directory 'clk' does not exist, creat it */
            if (access(popt->clkDir, 0) == -1)
            {
                string tmpDir = popt->clkDir;
#ifdef _WIN32   /* for Windows */
                string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
                string cmd = "mkdir -p " + tmpDir;
#endif
                std::system(cmd.c_str());
            }

            GetOrbClk(popt->ts, popt->clkDir, 2, fopt);  /* clk */
        }

        if (fopt->minusAdd1day && (strcmp(fopt->orbClkOpt, "igu") != 0 && strcmp(fopt->orbClkOpt, "gfu") != 0 &&
            strcmp(fopt->orbClkOpt, "wuu") != 0))
        {
            TimeUtil tu;
            gtime_t tt;
            /* precise orbit and clock product downloaded for the day before */
            tt = tu.TimeAdd(popt->ts, -86400.0);
            GetOrbClk(tt, popt->sp3Dir, 1, fopt);
            GetOrbClk(tt, popt->clkDir, 2, fopt);

            /* precise orbit and clock product downloaded for the day after */
            tt = tu.TimeAdd(popt->ts, 86400.0);
            GetOrbClk(tt, popt->sp3Dir, 1, fopt);
            GetOrbClk(tt, popt->clkDir, 2, fopt);
        }
    }
    
    /* EOP file downloaded */
    if (fopt->getEop)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->eopDir, 0) == -1)
        {
            string tmpDir = popt->eopDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetEop(popt->ts, popt->eopDir, fopt);
    }

    /* IGS SINEX file downloaded */
    if (fopt->getSnx)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->snxDir, 0) == -1)
        {
            string tmpDir = popt->snxDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetSnx(popt->ts, popt->snxDir, fopt);
    }

    /* MGEX and/or CODE DCB files downloaded */
    if (fopt->getDcb)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->dcbDir, 0) == -1)
        {
            string tmpDir = popt->dcbDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetDcbCode(popt->ts, popt->dcbDir, "P1P2", fopt);
        GetDcbCode(popt->ts, popt->dcbDir, "P1C1", fopt);
        GetDcbCode(popt->ts, popt->dcbDir, "P2C2", fopt);
        GetDcbMgex(popt->ts, popt->dcbDir, fopt);
    }

    /* global ionosphere map (GIM) downloaded */
    if (fopt->getIon)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ionDir, 0) == -1)
        {
            string tmpDir = popt->ionDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetIono(popt->ts, popt->ionDir, fopt);
    }

    /* Rate of TEC index (ROTI) file downloaded */
    if (fopt->getRoti)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ionDir, 0) == -1)
        {
            string tmpDir = popt->ionDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRoti(popt->ts, popt->ionDir, fopt);
    }

    /* final tropospheric product downloaded */
    if (fopt->getTrp)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->ztdDir, 0) == -1)
        {
            string tmpDir = popt->ztdDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetTrop(popt->ts, popt->ztdDir, fopt);
    }

    /* real-time precise orbit and clock products from CNES offline files downloaded */
    if (fopt->getRtOrbClk)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->sp3Dir, 0) == -1)
        {
            string tmpDir = popt->sp3Dir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRtOrbClkCNT(popt->ts, popt->sp3Dir, 1, fopt);

        /* If the directory does not exist, creat it */
        if (access(popt->clkDir, 0) == -1)
        {
            string tmpDir = popt->clkDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRtOrbClkCNT(popt->ts, popt->clkDir, 2, fopt);

        if (fopt->minusAdd1day)
        {
            TimeUtil tu;
            gtime_t tt;
            /* real-time precise orbit and clock files downloaded for the day before */
            tt = tu.TimeAdd(popt->ts, -86400.0);
            GetRtOrbClkCNT(tt, popt->sp3Dir, 1, fopt);
            GetRtOrbClkCNT(tt, popt->clkDir, 2, fopt);

            /* real-time precise orbit and clock files downloaded for the day after */
            tt = tu.TimeAdd(popt->ts, 86400.0);
            GetRtOrbClkCNT(tt, popt->sp3Dir, 1, fopt);
            GetRtOrbClkCNT(tt, popt->clkDir, 2, fopt);
        }
    }

    /* real-time code and phase bias products from CNES offline files downloaded */
    if (fopt->getRtBias)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->biaDir, 0) == -1)
        {
            string tmpDir = popt->biaDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetRtBiasCNT(popt->ts, popt->biaDir, fopt);
    }

    /* IGS ANTEX file downloaded */
    if (fopt->getAtx)
    {
        /* If the directory does not exist, creat it */
        if (access(popt->tblDir, 0) == -1)
        {
            string tmpDir = popt->tblDir;
#ifdef _WIN32   /* for Windows */
            string cmd = "mkdir " + tmpDir;
#else           /* for Linux or Mac */
            string cmd = "mkdir -p " + tmpDir;
#endif
            std::system(cmd.c_str());
        }

        GetAntexIGS(popt->ts, popt->tblDir, fopt);
    }
} /* end of FtpDownload */