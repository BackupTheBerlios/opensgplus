#include <OSGConfig.h>
#include <OSGBaseFunctions.h>
#include <OSGMulticastConnection.h>

//#define COUNT 10000000
#define COUNT 1000000

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
            MulticastConnection con;
            for(i=2;(int)i<argc;i+=1)
            {
                string addr=argv[i];
                cout << addr << endl;
                con.connect( addr );
            }
            for(i=0;i<COUNT;i++)
            {
//                cout << i << endl;
                con.putUInt32(i);
            }
            con.flush();
            for(i=2;(int)i<argc;i+=1)
            {
                string who;
                while(!con.selectChannel())
                {
                    cout << "no channe" << endl;
                }
                con.getString(who);
                cout << "response:" << who << endl;
            }
//            con.printStatistics();
        }
        if(argc>1 && (strcmp(argv[1],"-r")==0))
        {
            MulticastConnection con;
            string addr="224.0.0.50:5555";
            if(argc>2)
                addr=argv[2];
            addr = con.bind(addr);
            cout << "bound addr " << addr << endl;
            con.accept();
            while(!con.selectChannel());
            for(i=0;i<COUNT;i++)
            {
                con.getUInt32(j);
//                cout << j << endl;
                if(j!=i)
                {
                    cout << "Error: Unexpected data!! " << j << endl;
                }
            }
            con.putString(addr);
            con.flush();
//            con.printStatistics();
        }
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
