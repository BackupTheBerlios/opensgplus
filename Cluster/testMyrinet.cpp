#include <iostream>
#include <stdlib.h>

// do we have Myrinet configured in?
#ifdef OSG_WITH_MYRINET
#include "OSGMyrinetConnection.h"

using namespace OSG;
using namespace std;


void myrinetServer( MyrinetConnection & myri )
{
	UInt32 cnt = 0;
	
	while (1)
	{
		UInt32 * buf = (UInt32 *) myri.getBuffer();
		buf[0] = cnt++;
		myri.setDataSize( sizeof( UInt32 ) );

		printf( "server: sending value %d\n", cnt-1 );
		
		myri.send();
	}

}

void myrinetClient( MyrinetConnection & myri )
{
	int size;
	
	while (1)
	{
		size = myri.receive();
		UChar8 * buf = (UChar8 *) myri.getBuffer();
		
		printf( "client: received %d byte, buffer 0x%p\n", size, buf );
		
		for ( int i = 0; i < size; i++ )
		{
			printf("%02hx ", buf[i] );
		}
	}
}

int main(int argc,char **argv)
{
	if ( argc < 2 )
	{
		cerr << "Usage: testMyrinet -c or testMyrinet -s [node]+" << endl;
		exit(1); 
	}
	
	bool client = true;
	
	if ( ! strcmp( argv[1], "-s" ) )
		client = false;
	
    try
    {
		MyrinetConnection myri;

		for ( int i = 1; i < argc; i++ )
		{
			if ( argv[i][0] != '-' )
				myri.addTargetNode( argv[i] );
		}
    	
		if ( client )
			myrinetClient( myri );
		else
			myrinetServer( myri );
    }
    catch(exception &e)
    {
        cerr << "Error" << endl;
        cerr << e.what() << endl;
    }
}

#else
int main()
{
    cerr << "Myrinet not available" << endl;
}
#endif

