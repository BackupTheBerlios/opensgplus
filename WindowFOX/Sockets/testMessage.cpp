#include <iostream>
#include <string>
#include <OSGStreamSocket.h>
#include <OSGBinSocketMessage.h>

OSG_USING_NAMESPACE

using namespace std;

int main(int argc, char **argv)
{
    BinSocketMessage msg;
    UInt32 uint32;
    Int32  int32;
    UInt16 uint16;
    Int16  int16;
    UInt8  uint8;
    Int8   int8;
    Real32 real32;
    string str;

    msg.putUInt32(220);
    msg.putInt32 (221);
    msg.putUInt16(222);
    msg.putInt16 (223);
    msg.putUInt8 (224);
    msg.putInt8  (225);
    msg.putReal32(226.0);
    msg.putString("227");

    cout << msg.getUInt32() << endl;
    cout << msg.getInt32 () << endl;
    cout << msg.getUInt16() << endl;
    cout << msg.getInt16 () << endl;
    msg.getUInt8 (uint8);
    msg.getInt8  (int8);
    msg.getReal32(real32);
    msg.getString(str);
    cout << (int)uint8 << endl;
    cout << (int)int8 << endl;
    cout << real32 << endl;
    cout << str << endl;

    try
    {
        if(argc>1 &&argv[1][0] == 's')
        {
            StreamSocket sock,client;
            BinSocketMessage msg;
            sock.open();
            sock.setReusePort(true);
            sock.bind(AnyAddress(23344));
            sock.listen();
            client=sock.accept();
            msg.putInt32(12345);
            msg.putString("Hallo Hallo");
            client.send(msg);
            client.close();
            sock.close();
        }
        if(argc>1 &&argv[1][0] == 'c')
        {
            BinSocketMessage msg;
            StreamSocket sock;
            char *host="localhost";
            if(argc>2)
                host=argv[2];
            sock.open();
            sock.connect(Address(host,23344));
            sock.recv(msg);
            cout << msg.getInt32() << endl;
            cout << msg.getString() << endl;
            sock.close();
        }
    } catch(exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
