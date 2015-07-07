#include "serverlistener.h"
serverListener::serverListener(QObject *parent) :
    QThread(parent)
{
    moveToThread(this);
}
void serverListener::run()
{
    printf("Starting Core\n");
    int port = 9090;
    shared_ptr<ServerServiceHandler> handler(new ServerServiceHandler());
    shared_ptr<TProcessor> processor(new ServerServiceProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    printf("Starting Listening ...\n");
    shared_ptr<Thread> thread(server.thread());

    server.serve();

}
