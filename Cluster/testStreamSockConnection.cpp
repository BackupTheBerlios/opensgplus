#include <OSGConfig.h>
#include <OSGBaseFunctions.h>
#include <OSGStreamSockConnection.h>

using namespace OSG;

int main(int argc,char **argv)
{
    UInt32 i,j;

 	// OSG init
    osgInit(argc, argv);
    try
    {
        if(argc>1 && (strcmp(argv[1],"-s")==0))
        {
            StreamSockConnection con;
            for(i=2;(int)i<argc;i+=2)
            {
                string addr=argv[i];
                addr+=":5555";
                cout << addr << endl;
                con.connect( addr );
            }
            for(i=0;i<10000000;i++)
            {
//                cout << i << endl;
                con.putUInt32(i);
            }
            con.flush();
        }
        if(argc>1 && (strcmp(argv[1],"-r")==0))
        {
            StreamSockConnection con;
            con.bind("5555");
            con.accept();
            while(!con.selectChannel());
            for(i=0;i<10000000;i++)
            {
                con.getUInt32(j);
//                cout << j << endl;
                if(j!=i)
                {
                    cout << "Error: Unexpected data!!" << endl;
                }
            }
        }
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
