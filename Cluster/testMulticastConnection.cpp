#include <OSGMulticastConnection.h>

using namespace OSG;

int main(int argc,char **argv)
{
    UInt32 i,j;

    try
    {
        if(argc>1 && (strcmp(argv[1],"-s")==0))
        {
            MulticastConnection con;
            for(i=2;(int)i<argc;i+=2)
            {
                string addr=argv[i];
                addr+=":5555";
                cout << addr << endl;
                con.connect( addr );
            }
            for(i=0;i<10000000;i++)
                con.putUInt32(i);
            con.flush();
            con.printStatistics();
        }
        if(argc>1 && (strcmp(argv[1],"-r")==0))
        {
            MulticastConnection con;
            con.accept("5555");
            for(i=0;i<10000000;i++)
            {
                con.getUInt32(j);
//                cout << j << endl;
                if(j!=i)
                {
                    cout << "Error: Unexpected data!!" << endl;
                }
            }
            con.printStatistics();
        }
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
