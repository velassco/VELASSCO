# VELASSCO

VELaSSCo (http://www.velassco.eu) EC funded project final platform code. The goal of the project was to combine big data tools and technologies with scientific visualization algorithms to provide new visual analysis methods for large-scale simulations. 

# Setting up VELaSSCo platform

This tutorial guides you through the installation process of Hadoop and HBase on a multi node cluster. Furthermore we explain how the setup the Injector and the VELaSSCo Platform to make everything run. In our example we use:

- 4 Machines - 1 Master and 3 Slaves
- Scientific Linux 7.3 as operating system
- OpenJDK 1.8.0_121
- Hadoop 2.7.3 - http://mirror.yannic-bonenberger.com/apache/hadoop/common/hadoop-2.7.3/hadoop-2.7.3.tar.gz
- HBase 1.2.4 - http://mirror.serversupportforum.de/apache/hbase/1.2.4/hbase-1.2.4-bin.tar.gz
- Thrift 0.9.3


## Pre-installation Setup

### User Creation
Create a seperate user on every machine you want to use, e.g. "velassco". Note: don't forget to add the user to the /etc/sudoers file
```bash
su
useradd velassco
passwd velassco
```
Afterwards edit the `/etc/hosts` file on **every machine** so that we can work with aliases instead of IP addresses and the machines know about each other. Therefor you have to find out the IPs of your machines and edit the hosts file accordingly.
```bash 
sudo vim /etc/hosts
```
Although most tutorials say that you only have to add the master and itself to the slave machines host files, I ran into some problems without adding all the others too.
```
# hosts file
x.x.x.1 master
x.x.x.2 slave1
x.x.x.3 slave2
x.x.x.4 slave3
```
### SSH Setup
Afterwards we have to create a ssh key for the velassco user on your master machine and enable passwordless SSH login to the slaves.
**Note:** don't add a password during key creation - just press enter to use the defaults
```bash
su - hadoop
ssh-keygen -t rsa
ssh-copy-id hadoop@slave1
ssh-copy-id hadoop@slave2
ssh-copy-id hadoop@slave3
```
### Java and .bashrc
If you don't have Java installed please do this on every machine. The version in the prerequisites worked for me - for a compatibility list please refer to https://wiki.apache.org/hadoop/HadoopJavaVersions

In my example I used the default java location /usr/bin/java. To be able for Hadoop/HBase to use java you have to specify a envirenment variable in your .bashrc file. In order to minimize the editing of this file please copy the whole block as we are going to need it later as we set up Hadoop/HBase.
```bash
cd
vim .bashrc
```
```bash
# .bashrc file

# java
export JAVA_HOME=/usr
 
# Hadoop
export HADOOP_HOME=/usr/local/hadoop
export HADOOP_MAPRED_HOME=$HADOOP_HOME
export HADOOP_COMMON_HOME=$HADOOP_HOME
export HADOOP_HDFS_HOME=$HADOOP_HOME
export YARN_HOME=$HADOOP_HOME
export HADOOP_COMMON_LIB_NATIVE_DIR=$HADOOP_HOME/lib/native
export PATH=$PATH:$HADOOP_HOME/sbin:$HADOOP_HOME/bin
export HADOOP_INSTALL=$HADOOP_HOME
 
# HBase
export HBASE_HOME=/usr/local/hbase
export PATH=$PATH:$HBASE_HOME/bin
source .bashrc
```
**NOTE: Please turn all firewall services off (e.g. firewalld, iptables) or you will run into some problems**

## Installing Hadoop
First of all we have to create the storage locations for your name and datanodes on you filesystem. In this example I use "/home/hadoop/hadoopinfra" as my location for the data but I think you should be able to use any directory on the machine. Create the following folder structure on every machine:

```bash
cd
mkdir hadoopinfra
mkdir hadoopinfra/hdfs
mkdir hadoopinfra/hdfs/namenode
mkdir hadoopinfra/hdfs/datanode
sudo chown -R hadoop hadoopinfra/
```

The easiest way to install Hadoop (and HBase) is to download and configure it on your master machine and after the configuration duplicating the folder to all the other slave machines.
```bash
# Downloading, unpacking and setting folder rights
cd /usr/local
wget http://mirror.yannic-bonenberger.com/apache/hadoop/common/hadoop-2.7.3/hadoop-2.7.3.tar.gz
tar -xzf hadoop-2.7.3.tar.gz
sudo mv hadoop-2.7.3 hadoop
sudo chown -R hadoop hadoop/
```

To configure Hadoop to work in the fully-distributed mode you have to adjust several files in the `/usr/local/hadoop/etc/hadoop` directory.

First search for and adjust the JAVA_HOME environment variable in `hadoop-env.sh`:
```bash 
export JAVA_HOME=/usr
```
Afterward please copy the block from here to each file between the **<configuration>...</configuration>** tag:
```XML
# core-site.xml
<property>
    <name>fs.defaultFS</name>
    <value>hdfs://master/</value>
    <description>NameNode URI</description>
</property>
```

```XML
# yarn-site.xml
<property>
    <name>yarn.resourcemanager.hostname</name>
    <value>master</value>
    <description>The hostname of the ResourceManager</description>
</property>
<property>
    <name>yarn.nodemanager.aux-services</name>
    <value>mapreduce_shuffle</value>
    <description>shuffle service for MapReduce</description>
</property>
```
```XML
# hdfs-site.xml
<property>
    <name>dfs.datanode.data.dir</name>
    <value>/home/hadoop/hadoopinfra/hdfs/datanode</value>
    <description>DataNode directory for storing data chunks.</description>
</property>
<property>
    <name>dfs.namenode.name.dir</name>
    <value>/home/hadoop/hadoopinfra/hdfs/namenode</value>
    <description>NameNode directory for namespace and transaction logs storage.</description>
</property>
<property>
    <name>dfs.replication</name>
    <value>3</value>
    <description>Number of replication for each chunk.</description>
</property>
```

With the next one you have to first rename the template with:
```bash
cp mapred-site.xml.template mapred-site.xml
```
Then add the following as before
```XML
# mapred-site.xml
<property>
    <name>mapreduce.framework.name</name>
    <value>yarn</value>
    <description>Execution framework.</description>
</property>
```

Also please edit the `slaves` file and add all the machines to it. As our master is also a datanode we have to add it there as well.
```bash
# slaves
master
slave1
slave2
slave3
```

After this has been done we need to copy the hadoop folder to each slave machine. Note that as you don't have permission to write into `/usr/local` you have to move and chown the folder on your slave at the desired location.
```bash
# On your master machine
cd /usr/local
sudo scp -rp hadoop/* hadoop@slave1:~/hadoop
sudo scp -rp hadoop/* hadoop@slave2:~/hadoop
sudo scp -rp hadoop/* hadoop@slave3:~/hadoop
 ```
 ```bash
# On your slave machines
cd
sudo mv hadoop/ /usr/local
cd /usr/local/
sudo chown -R hadoop hadoop/
```
### Verifying Hadoop Installation

Now we have to format the namenode by using the command:
```bash
hdfs namenode -format
```
Sometimes this won't work because of a lack of permissions - then you can try the following command. If you do sudo you have to reown the directory.
```bash
sudo /usr/local/hadoop/bin/hdfs namenode -format
sudo chown -R velassco /home/velassco/hadoopinfra
```
Afterwards the filesystem should be formatted and you should be able to start hadoop and yarn with the following command:
```bash
start-dfs.sh && start-yarn.sh
```
Then you should be able to access hadoop in the webbrowser via http://localhost:8088/

You can test if the installation works correctly by running an example mapreduce computation by navigation and running:
```bash
cd /usr/local/hadoop/share/hadoop/mapreduce
yarn jar hadoop-mapreduce-examples-2.7.3.jar pi 10 100
```
If everything works correctly this should compute a rather bad approximation of pi.

## Installing HBase
The HBase installation should be easy if you set up Hadoop correctly. Just download and setup the HBase directory in `/usr/local` as you did before with Hadoop. Don't forget to chown the directory.

Afterwards navigate to the` /usr/local/hbase/conf` directory and edit the specified files as follows:

Here you have to uncomment and adjust the paths to your preference. 
```bash
# hbase-env.sh
export JAVA_HOME=/usr
export HBASE_PID_DIR=/var/hadoop/pids
export HBASE_MANAGES_ZK=true
export HBASE_HEAPSIZE=4000
export HBASE_OFFHEAPSIZE=8G
```
Then you should create and own the directory **on every machine** for the pids with
```bash
sudo mkdir /var/hadoop
sudo mkdir /var/hadoop/pids
sudo chown -R velassco /var/hadoop
```
Continue to edit the following files on your master. 
```XML
# hbase-site.xml on master
<property>
    <name>hbase.master</name>
    <value>master:60000</value>
</property>
<property>
    <name>hbase.rootdir</name>
    <value>hdfs://master/hbase</value>
</property>
<property>
    <name>hbase.zookeeper.property.dataDir</name>
    <value>/home/velassco/zookeper</value>
</property>
<property>
    <name>hbase.zookeeper.quorum</name>
    <value>master</value>
</property>
<property>
    <name>hbase.cluster.distributed</name>
    <value>true</value>
</property>
<property>
    <name>hbase.hregion.max.filesize</name>
    <value>16777216</value>
</property>
```
```bash
# regionservers
master
slave1
slave2
slave3
```
Finally you have to transfer the hbase folder to your other machines and adjust the `hbase-site.xml` in `/usr/local/hbase/conf` so that it only contains the following entries:
```XML
# hbase-site.xml on slaves
<property>
    <name>hbase.rootdir</name>
    <value>hdfs://master/hbase</value>
</property>
<property>
    <name>hbase.zookeeper.quorum</name>
    <value>master</value>
</property>
<property>
    <name>hbase.cluster.distributed</name>
    <value>true</value>
</property>
<property>
    <name>hbase.hregion.max.filesize</name>
    <value>16777216</value>
</property>
```
Then you have to run:
```bash
start-hbase.sh
``` 
To access the hbase shell type
```bash
hbase shell
 ```
you should get somethig like this if you type `status` as the shell is invoked
```bash
hbase(main):001:0> status
1 active master, 0 backup masters, 4 servers, 0 dead, 0.7500 average load
hbase(main):002:0>
```
## Building the ThriftInjector and injecting the data
First of all clone the velassco repository into your preferred location and build it with the following steps
```bash
cd
git clone https://github.com/velassco/VELASSCO.git
```
### Building
Then you have to get the following dependencies:

- CMake
- Boost - http://www.boost.org/doc/libs/1_63_0/more/getting_started/unix-variants.html
- Thrift 0.9.3 - https://thrift.apache.org/docs/install/

Then we have to regenrate some files, therefor navigate to the `VELASSCO/ThriftInjector/HBaseThrift` directory and run:
```bash
thrift -gen cpp HBase.thrift
```
Afterwards build the Injector:
```bash
cd VELASSCO/ThriftInjector
mkdir build
cd build
cmake ..
make -j 8
``` 
### Injecting the data
First of all make sure Hadoop and HBase are running. Afterwards you should start a thrift server with
```bash
hbase thrift start
```

The files should be in the following format: 

The *.post.msh file:
```bash
MESH "test" Dimension 3 ElemType Tetrahedra Nnode 4

# Coordinates should match with the Dimension
Coordinates
1.0 2.0 3.0
...
4.0 5.0 6.0
End Coordinates

# Number of Elements should be Number of Nodes + 1
Elements
0 1 2 4 0
...
10 3 4 5 0
End Elements
```
The *.post.res file:

```bash
GiD Post Results File 1.0

Result "PARTITION_INDEX" "test" 1 Scalar OnNodes
ComponentNames  "PARTITION_INDEX"
Values
0 1
...
10 10
End Values

Result "PRESSURE" "test" 1 Scalar OnNodes
ComponentNames  "PRESSURE"
Values
0 1.5
...
10 6.2
End Values

Result "VELOCITY" "test" 1 Scalar OnNodes
ComponentNames  "X-VELOCITY", "Y-VELOCITY", "Z-VELOCITY"
Values
0 1 2 3 0
...
10 10 12 24 0
End Values
```

Then navigate to your directory which contains the data and run: 
```bash
sudo /path/to/VELASSCO/ThriftInjector/build/Inject/gid_inject -i ./yourMeshFile_0.post.msh
```
this should iterate over the directory and inject the data into HBase. A test simulation data is provided in following path:
```
ThriftInjector/SampleData/HbaseBasicTest
```
So, the command to inject the test data will look like:
```bash
sudo /path/to/VELASSCO/ThriftInjector/build/Inject/gid_inject -i ../../../SampleData/HbaseBasicTest/VELaSSCo_HbaseBasicTest_part-1.post.msh
```

## Building the Velassco Platform
To build the Velassco Platform you have to simply run the following commands
```bash
cd path/to/VELASSCO/modules 
make
```
Afterwards you need to create a link to the relevant lib folder with the following commands:
```bash
cd path/to/VELASSCO/modules/Platform/AnalyticsYarnJobs
ln -s lib_eddie lib
```
You also have to copy and rename the necessary JAR Files in the AnalyticsYarnJobs directory so that they have your machines name extension (in our example master) as follows:
```bash
# for everyfile
cp GetBoundaryOfAMesh_pez001.jar GetBoundaryOfAMesh_master.jar
cp GetBoundingBoxOfAModel_pez001.jar GetBoundingBoxOfAModel_master.jar
cp GetDiscrete2ContinuumOfAModel_pez001.jar GetDiscrete2ContinuumOfAModel_master.jar
cp GetListOfVerticesFromMesh_pez001.jar GetListOfVerticesFromMesh_master.jar
cp GetMissingIDsOfVerticesWithoutResults_pez001.jar GetMissingIDsOfVerticesWithoutResults_master.jar
cp GetSimplifiedMesh_Average_pez001.jar GetSimplifiedMesh_Average_master.jar
cp GetSimplifiedMeshWithResult_Average_pez001.jar GetSimplifiedMeshWithResult_Average_master.jar
```
## Running
To run the whole Platform you have to have:
- Hadoop (`start-dfs.sh && start-yarn.sh`) and HBase (`start-hbase.sh`) running 
- The Thrift server started (`hbase thrift start`) 
- The Data should be injected (an example data is i

Then you need to start the Query Manager as follows: 
```bash
cd path/to/VELASSCO/modules/Platform/
./QueryManager.exe -port 26267
```
Now that the query manager is running, you are able to use CIMNE's GiD visualization client (http://www.gidhome.com/download/), and its VELaSSCo plugin, to test the query manager. You can find a document in the following link which describes how to connect to a query manager and use its functionalities:

http://velassco.atosresearch.eu/sites/default/files/velassco/files/content-files/articles/VELaSCCo_UC1_FinalEE_GiD_0.pdf

In addition, more infromation about using VELaSSCo platform, and webinar videos can be found here:

http://velassco.atosresearch.eu/content/material-nov-2016-velassco-evaluation-event

# License

| Module | Owner | Proposed license |
| ------ |:-----:|:----------------:|
| Data Injection for the Open Source version | ATOS | Dual (AGPL v3/commercial). Free for research and VELaSSCo partners |
| OS Query Manager | INRIA CIMNE | MIT |
| FEM analytics | CIMNE | MIT. Covering the majority of the VQueries for FEM |
| DEM analytics | UEDIN | Based on Discrete to Continuum (D2C) software, proprietary to UEDIN. Limited set of VQueries. |
| LRB-spline analytics | SINTEF | Dual (AGPL v3/commercial) |
| Kratos + GiD for FSI (Kratos VELaSSCo version) | CIMNE | Kratos OS (BSD), plugin MIT |
| GID commercial / GiD VELaSSCo plugin | CIMNE | MIT |
| iFX  VELaSSCo plugin | Fraunhofer | Commercial (selling license or version ) |
| LRB-splines visualization plugin | SINTEF | Dual (AGPL v3/commercial) |
| Data Ingestion for DEM | JOTNE | MIT (DEM version of the Data Injector for compatibility with EDM.  It is open sourced to foster the creation of new or improved injections to EDM) |
| EDM (EXPRESS Data Manager™) big data extensions | JOTNE | Commercial |
| EDM VELaSSCo queries | JOTNE | MIT (Commercially interesting to foster the creation of VELaSSCo queries for EDM in the future by the EDM community) |
| Standard extension to support DEM simulations ISO10303-209 | UEDIN JOTNE | ISO standard |

