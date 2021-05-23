# GAMPII-GOOD
GAMP II - GOOD (Gnss Observations and prOducts Downloader) is a powerful and easy-to-use lightweight GNSS observations and product downloading toolkit, which is developed by the SASIN (SpAtial SurveyIng and Navigation) group at Shandong University of Science and Technology (SDUST). The current version is 1.6, which is more stable. Hence, we decide to make it available for global GNSS users, and we are convinced that more and more GNSS users will benefit from GAMP II - GOOD. The source codes of GAMP II - GOOD is now available at GitHub (https://github.com/zhouforme0318/GAMPII-GOOD). We dedicated to create a more powerful GNSS data downloading tool, which can allow every GNSSer to completely get rid of the trouble in GNSS data and product downloading.
# Features
Main feature of GAMP II - GOOD are various GNSS observations and products donwloading, i.e., 
- IGS observation (RINEX version 2.xx, short name 'd')
- MGEX observation (RINEX version 3.xx, long name 'crx')
- various type of broadcast ephemeris
- various type of precise orbit 'sp3' and precise clock 'clk'
- earth rotation parameter
- IGS weekly SINEX solution
- CODE and/or MGEX differential code bias (DCB)
- global ionosphere map (GIM)
- rate of TEC index (ROTI)
- CODE and/or IGS tropospheric product
- real-time precise orbit and clock products from CNES offline files
- real-time code and phase bias products from CNES offline files
- ANTEX format antenna phase center correction
But it is not limited to these, users can use the source code to expand its functionalities according to their needs.
# Install
The source codes are written in ANSI C/C++, which are cross-platform and can be compiled on Windows, Linux/Unix and Mac OS. NOTE: Double-clicking the executable program does not work for data downloading. The correct way is to type ‘run_GAMP_GOOD  gamp_good.cfg’ on the terminal.
# How to use it
Please download GAMP II - GOOD Users Guide FROM https://github.com/zhouforme0318/GAMPII-GOOD/tree/master/Doc. The detailed description of how to use GAMP II - GOOD can be found in the document.
# Attention!
If some GNSS data cannot be successfully downloaded, check your network first. Then, check if the file exists on the remote server. Finally, rerun ‘run_GAMP_GOOD  gamp_good.cfg’ to try again. Don’t worry, once the data has been downloaded, it will not be downloaded repeatedly.
# Acknowledgments
- Dr Cheng Wang @ BUAA provided the C++ code for observation downloading as reference
- the suggestion of adding the option of the day before and after the current day for precise satellite orbit and clock products downloading is provide by Zan Liu @ CUMT
- the suggestion of adding the option of the full path of third-party softwares is provide by Dr Yudan Yi
- the suggestion of adding the option of rate of TEC index (ROTI) is provide by Lei Liu @ WHU
- the suggestion of adding the option of 'printInfoWget' in configure file for (not) printing the information of running 'wget' by Dr Hong Hu @ AHU
- the suggestion of adding the option of IGR, IGU, GFU (from GFZ), and WUU (from WHU) products as well as the site-specific broadcast ephemeris is provide by Menghao Li @ HEU
