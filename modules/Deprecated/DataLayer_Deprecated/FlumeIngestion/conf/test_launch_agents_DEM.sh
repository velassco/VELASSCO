#!/bin/bash

echo "Launching globaltable VELaSSCo models agent..."
./bin/flume-ng agent --conf conf --conf-file conf/VELaSSCo_Models/Test_PropertiesCF-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55555 &
sleep 5
echo "...agent launched!" 

echo "Launching Metadata C agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_MetadataC-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55556 &
sleep 5
echo "...agent launched!"

echo "Launching Metadata M agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_MetadataM-conf.properties --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55557 &
sleep 5
echo "...agent launched!"

echo "Launching Metadata G agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_MetadataG-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55558 &
sleep 5
echo "...agent launched!"

echo "Launching Metadata R agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_MetadataR-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55559 &
sleep 5
echo "...agent launched!"

echo "Launching SimulationData C agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_SimulationDataC-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55560 &
sleep 5
echo "...agent launched!"

echo "Launching SimulationData M agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_SimulationDataM-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55561 &
sleep 5
echo "...agent launched!"

echo "Launching Simulation Data R agent..."
./bin/flume-ng agent --conf conf --conf-file conf/globalTable/Test_SimulationDataR-conf.properties  --name agent -Dflume.root.logger=INFO,console -Dflume.monitoring.type=http -Dflume.monitoring.port=55562 &
sleep 5
echo "...agent launched!"

