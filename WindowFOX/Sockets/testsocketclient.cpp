#include <iostream>
#include <stdlib.h>
#include "OSGStreamSocket.h"
#include "OSGDgramSocket.h"
#include "OSGSelection.h"

using namespace OSG;
using namespace std;

void streamClient()
{
    StreamSocket sock;

    cout << "Stream socket client: Start" << endl;
    sock.open();
    sock.connect(Address("localhost",23344));
    sock.send("hallo",6);
    sock.close();
    cout << "Stream socket client: OK" << endl;
}

void dgramClient()
{
    char buffer[6];
    DgramSocket sock;
    Address server;
    bool retry;

    cout << "Dgram socket client: Start" << endl;
    sock.open();
    // bind socket
    do
    {
        do
        {
            // write to server
            sock.sendTo("hallo",6,Address("localhost",23344));
        }
        while(sock.waitReadable(0.2)==false); // wait 0.2 sec for response
        try
        {
            sock.recvFrom(buffer,6,server);
            retry=false;
        }
        catch(SocketConnReset &e)
        {
            // this could happen, when we have send a package to
            // a port which was unbound and the host responds
            // this with an icmp package. In this case, this is
            // not an error, because we whant to wait for this
            // port to be open.
            retry=true;
        }
    }
    while(retry);
    sock.close();
    cout << "Dgram socket client: OK" << endl;
}

void broadcastClient()
{
    char buffer[6];
    DgramSocket sock;
    Address server;
    Selection sel;

    cout << "Dgram broadcast client: Start" << endl;
    sock.open();
    // broadcast until answer from server
    do
    {
        // write to server
        sock.sendTo("hallo",6,BroadcastAddress(23344));
        // check for server response
        sel.setRead(sock);
    } 
    while(sel.select(0.2)==0); // wait 0.2 sec for response
    sock.recvFrom(buffer,6,server);
    sock.close();
    cout << "Dgram broadcast client: OK" << endl;
}

void multicastClient()
{
    char buffer[6];
    DgramSocket sock;
    Address server;
    Selection sel;
    Address addr[3]={
        Address("224.0.0.50",23344),
        Address("224.0.0.53",23344),
        Address("224.0.0.50",23344)
    };
    char *msg[3]={
        "msg:1","msg:2","_END_"
    };
    int i;

    cout << "Dgram multicast client: Start" << endl;
    sock.open();
    for(i=0;i<3;i++)
    {
        // broadcast until answer from server
        do
        {
            // write to server
            sock.sendTo(msg[i],6,addr[i]);
            // check for server response
            sel.setRead(sock);
        } 
        while(sel.select(0.2)==0); // wait 0.2 sec for response
        sock.recvFrom(buffer,6,server);
    }
    sock.close();
    cout << "Dgram multicast client: OK" << endl;
}

int main(int argc,char **argv)
{
    try
    {
        streamClient();
        dgramClient();
        broadcastClient();
        multicastClient();
    }
    catch(exception &e)
    {
        cerr << "Error" << endl;
        cerr << e.what() << endl;
    }
	return 0;
}

