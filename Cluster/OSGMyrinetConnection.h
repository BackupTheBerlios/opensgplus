/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#ifndef _MYRINETCONNECTION_H_
#define _MYRINETCONNECTION_H_
#ifdef __sgi
#pragma once
#endif

// do we have Myrinet configured in?
#ifdef OSG_WITH_MYRINET

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <vector>
#include <OSGCluster.h>
#include <OSGBaseTypes.h>
#include <OSGConnection.h>
#include <OSGMyrinetException.h>


// Myrinet header
// gm.h doesn't recognize the CPU ?!?
#define i386
#include <gm.h>

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//   Types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

/*! \ingroup baselib
 *  \brief Brief
 *
 *  detailed
 */

// This is the really stupid way of doing Myrinet, but I don't have the time
// to redesign the whole connection stuff right now. Let's do that when I'm
// back in Darmstadt. see OSGConnection.h for more comments

class OSG_CLUSTER_DLLMAPPING MyrinetConnection:public Connection
{
  public:

    //-----------------------------------------------------------------------
    //   constants                                                           
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

    MyrinetConnection( UInt32 port = 2 );
    virtual ~MyrinetConnection(void); 

    /*------------------------- your_category -------------------------------*/

    virtual MemoryHandle getBuffer();
    virtual void resizeBuffer(int size);
    virtual int getBufferSize();
    virtual int getDataSize();
    virtual void setDataSize(int size);
    virtual void send();
    virtual void sendSingle( char * host );
    virtual int receive();
    virtual void flush();
    // virtual void barrier();


    void addTargetNode( UInt32  id   );
    void addTargetNode( Char8  *name );

    /*------------------------- your_operators ------------------------------*/

    /*------------------------- assignment ----------------------------------*/

    /*------------------------- comparison ----------------------------------*/

  protected:

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class variables                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

	// callback for send events
	static void sendCallback(
				struct gm_port *port, void *context, gm_status_t status);

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------

	// the Myrinet connection	
	gm_port *_gmport;

	// connection port
	int 	 _port;

	// pending request count
	UInt32   _pending;
	
	// server ids
	vector<UInt32> _targetIDs;

	// buffer handling
	static const int _bufferLength = 0x200000-8;
    MemoryHandle     _buffer;
    int              _dataSize;
	
    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

  private:

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    typedef Connection Inherited;

    //-----------------------------------------------------------------------
    //   friend classes                                                      
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   friend functions                                                    
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class variables                                                     
    //-----------------------------------------------------------------------

	static char cvsid[];

    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

	// prohibit default functions (move to 'public' if you need one)

    MyrinetConnection(const MyrinetConnection &source);
    MyrinetConnection& operator =(const MyrinetConnection &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef MyrinetConnection *MyrinetConnectionP;

OSG_END_NAMESPACE

#endif  // OSG_WITH_MYRINET

#endif /* _MYRINETCONNECTION_H_ */
