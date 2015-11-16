rem SET EDM_HOME=O:\edm\v6.0.xxx\output\x64\debug
SET EDM_BIN=%EDM_HOME%\bin

SET BASE_FOLDER=O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_cluster

set EDM_SERVER_CLIENTS_FILES_PATH=O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_cluster\temp

start /min %EDM_BIN%\edmserver.exe -ul10007 -o -w -s9090 -l"%BASE_FOLDER%\db1" -nVELaSSCo -pVELaSSCo
start /min %EDM_BIN%\edmappserver.exe -w -s9090

start /min %EDM_BIN%\edmserver.exe -ul10175 -o -w -s9080 -l"%BASE_FOLDER%\db2" -nVELaSSCo -pVELaSSCo
start /min %EDM_BIN%\edmappserver.exe -w -s9080

start /min %EDM_BIN%\edmserver.exe -ul10176 -o -w -s9070 -l"%BASE_FOLDER%\db3" -nVELaSSCo -pVELaSSCo
start /min %EDM_BIN%\edmappserver.exe -w -s9070
