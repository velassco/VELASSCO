# VTU2MeshRes converter

This application converts .VTU files into .Mesh and .Res which the injector can use to inject into HBase.

## Requirements
- VTK
- GLM

## Running
To convert a file you need adjust the filename in the main.cpp:

```cpp
...
int main(int argc, char *argv[])
{
	std::string filename = "example.vtu";
...
}
...
```

Afterward you build the project and place your example.vtu besides the executable. After the application has finished running you should find your .msh and .res files in the same directory. 
