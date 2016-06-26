SET EDM_BIN=O:\edm\v6.0.xxx\output\x64\debug
rem SET EDM_BIN=%EDM_HOME%\bin

SET BASE_FOLDER=O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_cluster

set EDM_SERVER_CLIENTS_FILES_PATH=O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_cluster\temp

start /min %EDM_BIN%\edmserver.exe -ul10007 -o -w -s9090 -l"%BASE_FOLDER%\db0" -nVELaSSCo -pv
