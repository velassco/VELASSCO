/* -*- C++ -*- */
#ifndef abstractAcces_H
#define abstractAcces_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;
class abstractAcces
{
public:
     virtual int create(char *path, char *filename, char *data) = 0;
     virtual const char* cat(char *path, char *filename) = 0;
     virtual const char* grep(char *path, char *filename, char *dataToFind) = 0;
    
     virtual char* GetResultOnVerticesId(
                                  const char* session_Id,
                                  const char* model_Id,
                                  const char* analysis_Id,
                                  const double timeStep,
                                  const char* result_Id,
                                  // vector<int> vertices
                                  const int *vertices, // should be i64 ...
				  const int num_vertices // should be i64 ...
                                  ) = 0;
    
    void CheckIfAllVerticesArePresent();
private:
    
    
};

#endif
