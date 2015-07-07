#include "data.h"

data::data()
{

    this->N = 100000;
    this->ts = 10;
    particles = new particle[N*ts];
    gen();
}
data::data(int N, int ts)
{

    this->N = N;
    this->ts = ts;
    particles = new particle[N*ts];
    gen();
}


void data::gen()
{
    int k = 0;
    for (int j = 0; j < ts; j++)
    {
        for (int i = 0 ; i < N; i++)
        {
            particle p;
            p.id = i;
            p.timeStep = j;
            p.coord.x = (rand() % 1000 )/1000.0f;
            p.coord.y = (rand() % 1000 )/1000.0f;
            p.coord.z = (rand() % 1000 )/1000.0f;
            p.acc.x = (rand() % 1000 )/1000.0f;
            p.acc.y = (rand() % 1000 )/1000.0f;
            p.acc.z = (rand() % 1000 )/1000.0f;
            particles[k] = p;
            k++;
        }
    }
}

QString data::csv(int start, int end)
{
    QString content;
    {
        for (uint i = 0 ; i < N*ts; i++)
        {

            content.append(ligne(i, start, end));
        }
    }
    return content;
}

QString data::csv(QChar c, int start, int end)
{
    QString content;
    {
        for (uint i = 0 ; i < N*ts; i++)
        {
            content.append(c);
            content.append(ligne(i, start, end));
        }
    }
    return content;
}

QString data::ligne(int i, int start, int end)
{
    QString content;
    int cpt = 0;
    {
        {
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].id));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].timeStep));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].coord.x));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].coord.y));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].coord.z));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].acc.x));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].acc.y));
                content.append(";");

            }
            cpt++;
            if(cpt >=start && cpt <end)
            {
                content.append(QString::number(particles[i].acc.z));
                content.append("\n");

            }
            cpt++;
        }
    }
    return content;
}
