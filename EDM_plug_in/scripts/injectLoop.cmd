for /L %%o in (0,1,127) do (
ECHO command FEMfiles > inject_FEM.cmd
ECHO dbFolder "C:\VELaSSCo\installation\database\db0" >> inject_FEM.cmd
ECHO dbName VELaSSCo >> inject_FEM.cmd
ECHO dbPassword v >> inject_FEM.cmd
ECHO repository DataRepository >> inject_FEM.cmd
ECHO model Telescope_%%o >> inject_FEM.cmd
ECHO File fine_mesh-ascii_%%o.post.msh >> inject_FEM.cmd
ECHO File fine_mesh-ascii_%%o.post.res >> inject_FEM.cmd
set path=%path%;C:\edm\edmsix.1.100.12\bin;C:\VELaSSCo\installation\software\edm\edmsix_VELaSSCo\bin
C:\VELaSSCo\installation\software\VELaSSCo\EDMinjector\DEM_inject_server.exe inject_FEM.cmd batchMode
)