# Thrift Injector

This application understands .mesh and .res files and can inject them into HBase. To do so you need to complete several steps which are desrcibed below.

## Building the ThriftInjector
First of all clone the velassco repository into your preferred location and build it with the following steps
```bash
cd
git clone https://github.com/velassco/VELASSCO.git
```

Then you have to get the following dependencies:

- CMake
- Boost - http://www.boost.org/doc/libs/1_63_0/more/getting_started/unix-variants.html
- Thrift 0.9.3 - https://thrift.apache.org/docs/install/

Afterwards have to regenrate some files, therefor navigate to the `VELASSCO/ThriftInjector/HBaseThrift` directory and run:
```bash
thrift -gen cpp HBase.thrift
```
Then build the Injector:
```bash
cd VELASSCO/ThriftInjector
mkdir build
cd build
cmake ..
make -j 8
``` 

## (Optional) Converting the data

If you don't have any data available, we have built a VTU File Converter to generate the .msh and .res files from at VTU File. You can find the converter in the [VTUConverter](https://github.com/velassco/VELASSCO/tree/master/ThriftInjector/VTUConverter) directory. 

## Injecting the data
First of all make sure Hadoop and HBase are running. Afterwards you should start a thrift server with
```bash
hbase thrift -t 9000000 start
```
where `-t` is the paremeter for the thread timeout which handles the connections. It should be long enough so that the connection doesn't close during usage. 

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

Result "VELOCITY" "test" 1 Vector OnNodes
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
