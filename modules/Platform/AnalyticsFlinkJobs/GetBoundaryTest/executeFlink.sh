
#Execute as VELaSSCo user.

/exports/applications/apps/community/VELaSSCo/flink/flink-1.0.2/build-target/bin/flink run -m yarn-cluster -yn 8 -yjm 1024 -ytm 2048 ./target/getBoundaryOfAMeshFlink-1.0-SNAPSHOT.jar
