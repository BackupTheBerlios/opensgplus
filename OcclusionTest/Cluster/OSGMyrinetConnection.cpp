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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

// do we have Myrinet configured in?
#ifdef OSG_WITH_MYRINET

#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGMyrinetException.h"
#include "OSGMyrinetConnection.h"

OSG_USING_NAMESPACE

/** \enum OSGVecBase::VectorSizeE
 *  \brief 
 */

/** \var OSGVecBase::VectorSizeE OSGVecBase::_iSize
 * 
 */

/** \fn const char *OSGVecBase::getMyrinetConnection(void)
 *  \brief MyrinetConnection
 */

/** \var OSGValueTypeT OSGVecBase::_values[iSize];
 *  \brief Value store
 */

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char MyrinetConnection::cvsid[] = "@(#)$Id: OSGMyrinetConnection.cpp,v 1.1 2002/02/06 08:29:59 jsux Exp $";

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/



/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/



/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */

MyrinetConnection::MyrinetConnection( UInt32 port ):
    Inherited(),
    _port( port ), _buffer(NULL)
{
	gm_status_t st;
	
	st = gm_init();
	if( st != GM_SUCCESS )
	{
	   throw MyrinetError("gm_init()",st);
	}
	
	// open the port
	
	st = gm_open( &_gmport, 0, port, "unnamed", GM_API_VERSION_1_3 );
	if( st != GM_SUCCESS )
	{
	   throw MyrinetError("gm_open()",st);
	}
	
	// allocate buffer

	UInt32 size = _bufferLength;
	size = gm_min_size_for_length( size );
	size = gm_max_length_for_size( size );
	
	_buffer = (MemoryHandle) gm_dma_malloc( _gmport, size );

	if ( ! _buffer )
	{
	   throw MyrinetError("gm_dma_malloc()",st);		
	}
	
	// free the send tokens
	gm_free_send_tokens( _gmport, GM_LOW_PRIORITY, gm_num_send_tokens(_gmport));
}


//MyrinetConnection::MyrinetConnection(const MyrinetConnection &source) :
//	Inherited(source),
//	  // TODO: initialize members
//{
//}

/** \brief Destructor
 */

MyrinetConnection::~MyrinetConnection(void)
{
    if(_buffer)
    {
 		gm_dma_free( _gmport, _buffer );
		_buffer = NULL;
    }

	if ( _gmport )
	{
		gm_close( _gmport );
		_gmport = NULL;
	}	

	gm_finalize();
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

MemoryHandle MyrinetConnection::getBuffer()
{
    return _buffer;
}

void MyrinetConnection::resizeBuffer(int size)
{
	if ( size > _bufferLength )
	{
		throw MyrinetError("resizeBuffer: too big", GM_FAILURE);	
	}
	
    _dataSize = 0;
}

int MyrinetConnection::getBufferSize()
{
    return _bufferLength;
}

int MyrinetConnection::getDataSize()
{
    return _dataSize;
}

void MyrinetConnection::setDataSize(int size)
{
    _dataSize=size;
}


// send callback: error checking and pending decrease
void
MyrinetConnection::sendCallback(
				struct gm_port *port, void *context, gm_status_t status)
{
	MyrinetConnection *c = (MyrinetConnection *) context;
	
	if (status != GM_SUCCESS)
	{
		throw MyrinetError("sendCallback: too big", status);
	}
	--c->_pending;
	gm_free_send_token( c->_gmport, GM_LOW_PRIORITY );
}

// simple, stupid way: send it to all connected targets and wait until they 
// have it

void MyrinetConnection::send()
{
	vector<UInt32>::iterator it;
	bool finished = false;
	it = _targetIDs.begin();
	
	_pending = 0;
	
	while ( it != _targetIDs.end() || _pending )
	{
		// if we have a send token and are not finished yet, send it
		if ( it != _targetIDs.end() && 
			 gm_alloc_send_token( _gmport, GM_LOW_PRIORITY ) 
		   )
		{
			int size = gm_min_size_for_length( _bufferLength );
			
			gm_send_to_peer_with_callback( _gmport, _buffer, size,
				_dataSize,
				GM_LOW_PRIORITY, *it, MyrinetConnection::sendCallback, this );
			++it;
			++_pending;
		}
		
		while ( gm_receive_pending( _gmport ) )
		{
			gm_recv_event_t *e = gm_receive( _gmport );
			gm_unknown( _gmport, e );
		}
	}
	
   _dataSize=0;
}

void MyrinetConnection::sendSingle(char * name)
{
	vector<UInt32>::iterator it;
	bool finished = false;

	UInt32 id = gm_host_name_to_node_id( _gmport, name );
	
	if ( id == GM_NO_SUCH_NODE_ID )
	{
		throw MyrinetError("addTargetNode: unknown node", GM_FAILURE );
	}
	
	it = find( _targetIDs.begin(), _targetIDs.end(), id );
	
	gm_alloc_send_token( _gmport, GM_LOW_PRIORITY );

	int size = gm_min_size_for_length( _bufferLength );
			
	gm_send_to_peer_with_callback( _gmport, _buffer, size,
				_dataSize,
				GM_LOW_PRIORITY, *it, MyrinetConnection::sendCallback, this );
	_pending = 1;
		
	while ( _pending || gm_receive_pending( _gmport ) )
	{
		gm_recv_event_t *e = gm_receive( _gmport );
		gm_unknown( _gmport, e );
	}
	
   _dataSize=0;
}

// simple, stupid way: wait for any receive event and get it

int MyrinetConnection::receive()
{
    int size;
	
	// provide a buffer
	gm_provide_receive_buffer( _gmport, _buffer, 
								gm_min_size_for_length( _bufferLength ),
								GM_LOW_PRIORITY );
	
	// wait for a receive event	
	_dataSize = 0;
	do
	{
	  	gm_recv_event_t *e;
		
		e = gm_blocking_receive( _gmport );
		
		switch ( gm_ntohc( e->recv.type ) )
		{
		case GM_HIGH_RECV_EVENT:
		case GM_RECV_EVENT: 		_dataSize = gm_ntohl( e->recv.length );
									break;
		default:					gm_unknown ( _gmport, e );
									break;
		}
	}
	while ( ! _dataSize );
		
	return _dataSize;	
}

void MyrinetConnection::flush()
{
}

void MyrinetConnection::addTargetNode( UInt32  id   )
{
	_targetIDs.push_back( id );
}

void MyrinetConnection::addTargetNode( Char8 *name )
{
	UInt32 id = gm_host_name_to_node_id( _gmport, name );
	
	if ( id == GM_NO_SUCH_NODE_ID )
	{
		throw MyrinetError("addTargetNode: unknown node", GM_FAILURE );
	}
	_targetIDs.push_back( id );
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

MyrinetConnection& MyrinetConnection::operator = (const MyrinetConnection &source)
{
	if (this == &source)
		return *this;

	// copy parts inherited from parent

	// free mem alloced by members of 'this'

	// alloc new mem for members

	// copy 
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

/** \brief assignment
 */

/** \brief equal
 */

/** \brief unequal
 */

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

#endif
