#include "hbaseaccess.h"


hbaseaccess::hbaseaccess()
{
    //Set server  inforamtion ?
}
hbaseaccess::~hbaseaccess()
{
    
}

int hbaseaccess::create(char *path, char *filename, char *data)
{
    
}

//read all data from a table
const char* hbaseaccess::cat(char *path, char *filename)
{
    std::stringstream _data;
    boost::shared_ptr<TTransport> socket(new TSocket(path, 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        
        std::map<std::string,std::string> m;
        vector<TRowResult> rowsResult;
        TScan ts;
        
        
        ScannerID id = client.scannerOpenWithScan( filename ,ts, m);
        do
        {
            client.scannerGetList(rowsResult, id, 1000);
            //Read output data
            
            for (int i = 0; i < rowsResult.size(); i++)
            {
                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {
                    _data << "collname : " << it->first << "\n";
                    _data << "\tvalue : " << it->second.value <<"\n";
                }
            }
            
        }
        while (rowsResult.size() > 0);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    return _data.str().c_str();
}

//read a subPart of data from a table
const char* hbaseaccess::grep(char *path, char *filename, char *dataToFind)
{
    std::stringstream _data;
    boost::shared_ptr<TTransport> socket(new TSocket(path, 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        
        std::map<std::string,std::string> m;
        vector<TRowResult> rowsResult;
        TScan ts;
        
        std::stringstream filter;
        filter.str("");
        filter << dataToFind;
        ts.__set_filterString(filter.str());
        
        ScannerID id = client.scannerOpenWithScan( filename ,ts, m);
        do
        {
            client.scannerGetList(rowsResult, id, 1000);
            //Read output data
            
            for (int i = 0; i < rowsResult.size(); i++)
            {
                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {
                    _data << "collname : " << it->first << "\n";
                    _data << "\tvalue : " << it->second.value <<"\n";
                }
            }
            
        }
        while (rowsResult.size() > 0);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    return _data.str().c_str();
}

//extract from a vertice ID the row.
const char*  hbaseaccess::GetResultOnVerticesId(
                                                char *path,
                                                char *filename,
                                                
                                                char* session_Id,
                                                char* model_Id,
                                                char* analysis_Id,
                                                double timeStep,
                                                char* result_Id,
                                                vector<int> vertices
                                                )
{
    std::stringstream _data;
    boost::shared_ptr<TTransport> socket(new TSocket(path, 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        
        std::map<std::string,std::string> m;
        vector<TRowResult> rowsResult;
        TScan ts;
        std::vector<string> keys;
        for (int i = 0 ; i < vertices.size(); i++)
        {
            stringstream key;
            
            key.str("");
//            key << vertices;
            keys.push_back(key.str());
        }
                client.getRows(rowsResult, filename ,keys, m);
        for( int i = 0 ; i < rowsResult.size(); i++)
        {
            for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                 it != rowsResult.at(i).columns.end(); ++it)
            {
                _data << "collname : " << it->first << "\n";
                _data << "\tvalue : " << it->second.value <<"\n";
            }

        }
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    return _data.str().c_str();
    
}
