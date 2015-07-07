hadoop fs -mkdir       /user
hadoop fs -mkdir       /tmp
hadoop fs -mkdir       /user/vagrant
hadoop fs -mkdir       /user/root
hadoop fs -mkdir       /user/lange
hadoop fs -mkdir       /user/hbase
hadoop fs -mkdir       /hbase
hadoop fs -mkdir       /res
hadoop fs -mkdir       /user/hive
hadoop fs -mkdir       /user/hive/warehouse
hadoop fs -mkdir -p /tmp/logs
hadoop fs -mkdir -p /mapred/history/done
hadoop fs -mkdir -p /mapred/history/done_intermediate
hadoop fs -mkdir -p /mr-history/done
hadoop fs -mkdir -p /mr-history/tmp

hadoop fs -chmod -R 777   /tmp
hadoop fs -chmod -R 777 /hbase
hadoop fs -chmod -R 777 /user
hadoop fs -chmod -R 777 /mr-history
hadoop fs -chmod -R 777 /mapred