#include <iostream>
#include <stdlib.h>
#include "OSGStreamSocket.h"
#include "OSGDgramSocket.h"
#include "OSGSelection.h"

using namespace OSG;
using namespace std;

void streamServer()
{
    char buffer[6];
    Selection sel,res;
    StreamSocket sock,client;

    cout << "Stream socket server: Start" << endl;
    // open socket
    sock.open();
    // reuse port. Might be dangerous
    sock.setReusePort(true);
    // bind to port
    sock.bind(AnyAddress(23344));
    // listen to port
    sock.listen();
    // Wait for Client. This is optional!
    // every 2 seconds a message is printed
    sel.setRead(sock);
    while(!sel.select(2,res))
    {
        cout << "Waiting for client on port:" << sock.getAddress().getPort() << endl;
    }
    // accept client
    client=sock.accept();
    client.read(buffer,6);
    sock.close();
    client.close();
    cout << "Stream socket server: OK" << endl;
}

void dgramServer()
{
    char buffer[6];
    DgramSocket sock;
    Address client;

    cout << "Dgram socket server: start" << endl;
    sock.open();
    // reuse port. Optional and can cause problems with other apps
    sock.setReusePort(true);
    // bind socket to port 23344
    sock.bind(AnyAddress(23344));
    // read from client
    sock.readFrom(buffer,6,client);
    // replay to client
    sock.writeTo(buffer,6,client);
    sock.close();
    cout << "Dgram socket server: OK" << endl;
}

void broadcastServer()
{
    char buffer[6];
    DgramSocket sock;
    Address client;

    cout << "Dgram broadcast server: Start" << endl;
    sock.open();
    sock.setReusePort(true);
    sock.bind(AnyAddress(23344));
    sock.readFrom(buffer,6,client);
    sock.writeTo(buffer,6,client);
    sock.close();
    cout << "Dgram broadcast server: OK" << endl;
}

void multicastServer()
{
    char buffer[6];
    DgramSocket sock;
    Address client;

    cout << "Dgram multicast server: Start" << endl;
    sock.open();
    sock.setReusePort(true);
    sock.bind(AnyAddress(23344));
    sock.join(Address("224.0.0.50"));
    sock.join(Address("224.0.0.52"));
    sock.join(Address("224.0.0.53"));
    sock.leave(Address("224.0.0.52"));
    do
    {
        sock.readFrom(buffer,6,client);
        sock.writeTo(buffer,6,client);
        cout << client.getHost() << endl;
    } while(strcmp(buffer,"_END_"));
    sock.close();
    cout << "Dgram multicast server: OK" << endl;
}

int main(int argc,char **argv)
{
    try
    {
        streamServer();
        dgramServer();
        broadcastServer();
        multicastServer();
    }
    catch(exception &e)
    {
        cerr << "Error" << endl;
        cerr << e.what() << endl;
    }
}



