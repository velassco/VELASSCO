#ifndef DATA_H
#define DATA_H


#include <vector>
#include <iostream> // pour std::cout
#include <string>   // pour std::string
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


#include <QString>
struct dots
{
    float x;
    float y;
    float z;
};


struct particle
{
    int id;
    int timeStep;
    dots coord;
    dots acc;
};

using namespace std;

class data
{
public:
    int N;
    int ts;

    data();
    data(int N, int ts);

    void gen();
    QString csv(int start, int end);
    QString csv(QChar c, int start, int end);
    QString ligne(int i, int start, int end);


    particle *particles;
};

#endif // DATA_H
