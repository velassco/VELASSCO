#!/bin/bash

echo "Launching globaltable VELaSSCo models agent..."
./bin/flume-ng agent --conf conf --conf-file conf/VELaSSCo_Models/PropertiesCF-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55555 &
sleep 5
echo "...agent launched!" 

#echo "Launching Metadata C agent..."
#./bin/flume-ng agent --conf conf --conf-file conf/globalTable/MetadataC-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34546 &
#sleep 10
#echo "...agent launched!"

echo "Launching Metadata M agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/MetadataM_FEM-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34550 &
sleep 5
echo "...agent launched!"

#echo "Launching Metadata G agent..."
#./bin/flume-ng agent --conf conf --conf-file conf/globalTable/MetadataG-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34548 &
#sleep 10
#echo "...agent launched!"

echo "Launching Metadata R agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/MetadataR_FEM-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34551 &
sleep 5
echo "...agent launched!"

#echo "Launching SimulationData M agent..."
#./bin/flume-ng agent --conf conf --conf-file conf/globalTable/SimulationDataC-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34550 &
#sleep 10
#echo "...agent launched!"

echo "Launching SimulationData M agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/SimulationDataM_FEM-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34552 &
sleep 5
echo "...agent launched!"

echo "Launching Simulation Data R agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/SimulationDataR_FEM-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=34553 &
sleep 5
echo "...agent launched!"

