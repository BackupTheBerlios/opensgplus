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

#define OSG_COMPILECLUTERLIB

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGClusterException.h"
#include "OSGFieldDescription.h"
#include "OSGFieldContainerPtr.h"
#include "OSGRemoteAspect.h"
#include "OSGFieldContainer.h"
#include "OSGLog.h"
#include "OSGGeometry.h"
#include "OSGTextureChunk.h"
#include "OSGWindow.h"
#include "OSGQTWindow.h"

OSG_USING_NAMESPACE

/** \enum OSGVecBase::VectorSizeE
 *  \brief 
 */

/** \var OSGVecBase::VectorSizeE OSGVecBase::_iSize
 * 
 */

/** \fn const char *OSGVecBase::getClassname(void)
 *  \brief Classname
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

char RemoteAspect::cvsid[] = "@(#)$Id: $";

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

RemoteAspect::RemoteAspect():
    _receivedFC(),
    _receivedType(),
    _sentType(),
    _fieldFilter(),
    _createdFunctors(),
    _destroyedFunctors(),
    _changedFunctors()
{
    _buffer    =NULL;
    _bufferSize=0;
    _dataSize  =0;

    // initialize field filter
    _fieldFilter[Geometry::getClassType().getId()] 
    = Geometry::GLIdFieldMask;
    _fieldFilter[TextureChunk::getClassType().getId()] 
    = TextureChunk::GLIdFieldMask;
    _fieldFilter[Window::getClassType().getId()] 
    = Window::GlObjectStatusFieldMask;
    _fieldFilter[QTWindow::getClassType().getId()] 
    = Window::GlObjectStatusFieldMask;
}

/** \brief Destructor
 */

RemoteAspect::~RemoteAspect(void)
{
    FieldContainerFactory *factory=FieldContainerFactory::the();
    ReceivedFCT::iterator i;
    FieldContainerPtr fcPtr;

    // subRef received field container
    for(i =_receivedFC.begin();
        i!=_receivedFC.end();
        i++)
    {
        fcPtr=factory->getContainer(i->second);
        
        callDestroyed(fcPtr);
        // subRefCP(fcPtr);
    }
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void RemoteAspect::receiveSync(Connection &connection)
{
    Bool finish=false;
    UInt32 cmd;
    UInt32 remoteTypeId;
    UInt32 localTypeId;
    UInt32 remoteId;
    UInt32 localId;
    char *name;
    FieldContainerFactory *factory=FieldContainerFactory::the();
    FieldContainerType *fcType;
    FieldContainerPtr fcPtr;
    BitVector mask;
    UInt32 valueSize;
    RemoteAspectFieldContainerMapper mapper;
    ReceivedTypeT::iterator receivedTypeI;
    ReceivedFCT::iterator receivedFCI;

    mapper._remoteAspect=this;

    // register mapper into factory
    factory->setMapper(&mapper);
    do
    {
        receive(connection);
        while((!finish) && (_dataPos<_dataSize))
        {
            cmd=getUInt8();
            switch(cmd)
            {
            case NEWTYPE:
                remoteTypeId=getUInt32();
                name=getString();
                // find local type
                fcType=FieldContainerFactory::the()->findType(name);
                if(!fcType)
                {
                    SWARNING << "Unknown Type: " << name << endl; 
                }
                else
                {
                    localTypeId=FieldContainerFactory::the()->
                                findType(name)->getId();
                    // insert remote type id into map
                    _receivedType[remoteTypeId]=localTypeId;
                }
                break;

            case CREATED:
                remoteTypeId=getUInt32();
                remoteId=getUInt32();
                receivedTypeI=_receivedType.find(remoteTypeId);
                if(receivedTypeI == _receivedType.end())
                {
                    SWARNING << "Unknown TypeID: " << remoteTypeId << endl; 
                }
                else
                {
                    localTypeId=receivedTypeI->second;
                    fcType=factory->findType(localTypeId);
                    fcPtr=fcType->createFieldContainer();
                    _receivedFC[remoteId]=fcPtr.getFieldContainerId();
                    addRefCP(fcPtr);
                    callCreated(fcPtr);
                }
                break;

            case DESTROYED:
                remoteId=getUInt32();
                receivedFCI=_receivedFC.find(remoteId);
                if(receivedFCI == _receivedFC.end())
                {
                    SWARNING << "Can't destroy unknown FC:" << remoteId 
                             << endl; 
                }
                else
                {
                    fcPtr=factory->getContainer(receivedFCI->second);
                    callDestroyed(fcPtr);
                    subRefCP(fcPtr);
                }
                break;

            case CHANGED:
                remoteId=getUInt32();
                mask=getUInt32();
                valueSize=getUInt32();
                receivedFCI=_receivedFC.find(remoteId);
                if(receivedFCI == _receivedFC.end())
                {
                    SWARNING << "Can't change unknown FC:" << remoteId 
                             << endl; 
                }
                else
                {
                    fcPtr=factory->getContainer(receivedFCI->second);
                    /*
                    for(int i=0;i<fcPtr->getType().getNumFieldDescs();i++)
                    {
                        FieldDescription *desc=fcPtr->getType().getFieldDescription(i+1);
                        if(desc->getFieldMask() & mask)
                            cout<< "changed field: " << desc->getName() << endl;
                    }
                    */
                    fcPtr->copyFromBin(&_buffer[_dataPos],mask);
                    changedCP(fcPtr,mask);
                    callChanged(fcPtr);
                }
                _dataPos+=valueSize;
                break;

            case SYNCENDED:
                finish=true;
                break;

            case ADDREFED:
                remoteId=getUInt32();
                /*
                receivedFCI=_receivedFC.find(remoteId);
                if(receivedFCI == _receivedFC.end())
                {
                    SWARNING << "Can't addref unknown FC:" << remoteId 
                             << endl; 
                }
                fcPtr=factory->getContainer(receivedFCI->second);
                addRefCP(fcPtr);
                */
                break;

            case SUBREFED:
                remoteId=getUInt32();
                /*
                receivedFCI=_receivedFC.find(remoteId);
                if(receivedFCI == _receivedFC.end())
                {
                    SWARNING << "Can't subref unknown FC:" << remoteId 
                             << endl; 
                }
                fcPtr=factory->getContainer(receivedFCI->second);
                subRefCP(fcPtr);
                */
                break;
            }
        }
    } while(!finish);
    // unregister mapper into factory
    factory->setMapper(NULL);

}

void RemoteAspect::sendSync(Connection &connection,
                            OSG::ChangeList *changeList)
{
    ChangeList::changed_const_iterator changedI;
    ChangeList::idrefd_const_iterator createdI;
    ChangeList::idrefd_const_iterator destroyedI;
    ChangeList::refd_const_iterator addRefedI;
    ChangeList::refd_const_iterator subRefedI;
    FieldFilterT::iterator filterI;
    FieldContainerFactory *fcFactory = FieldContainerFactory::the();
    FieldContainerPtr fcPtr;
    UInt32 typeId;
    MemoryHandle res;
    UInt32 id;
    BitVector mask;

    if(!changeList)
    {
        changeList=OSG::Thread::getCurrentChangeList();
    }
    _buffer    =connection.getBuffer();
    _bufferSize=connection.getBufferSize();

    // created fct
    for(createdI =changeList->beginCreated() ;
        createdI!=changeList->endCreated() ;
        createdI++)
    {
        fcPtr=fcFactory->getContainer(*createdI);
        if ( fcPtr == FieldContainerPtr::NullPtr ) // already deleted, ignore
			continue;

        typeId = fcPtr->getTypeId();
        // type unknown by remote context ?
        if(_sentType.count(typeId)==0)
        {
            // mark type as known
            _sentType.insert(typeId);
            // send buffer if not enough mem
            if( int(_dataSize + 
                    sizeof(UInt8) + 
                    sizeof(UInt32) + 
                    strlen(fcPtr->getType().getName().str())+1 ) 
                > _bufferSize )
            {
                send(connection);
            }
            // send new type
            putUInt8(NEWTYPE);
            putUInt32(typeId);
            putString(fcPtr->getType().getName().str());
        }
        // send buffer if not enough mem
        if( int(_dataSize +
                sizeof(UInt8) +
                sizeof(UInt32) +
                sizeof(UInt32)) > _bufferSize )
        {
            send(connection);
        }
        putUInt8(CREATED);
        putUInt32(typeId);
        putUInt32(*createdI);
    }

    // changed fields
    for(changedI =changeList->beginChanged() ;
        changedI!=changeList->endChanged() ;
        changedI++)
    {
        //        FieldContainerPtr fcPtr=*((FieldContainerPtr*)(&changedI->first));

        FieldContainerPtr fcPtr=fcFactory->getContainer(changedI->first);
        if ( fcPtr == FieldContainerPtr::NullPtr ) // already deleted, ignore
			continue;
		mask = changedI->second;
        filterI=_fieldFilter.find(fcPtr->getType().getId());
        // apply field filter
        if(filterI != _fieldFilter.end())
        {
            cout << "filtet:" << fcPtr->getType().getName() << endl;
            mask &= 0xFFFFFFFF ^ filterI->second;
        }

        int valueSize=fcPtr->getBinSize(mask);
        int size=sizeof(UInt8) +
                 sizeof(UInt32) + 
                 sizeof(UInt32) + 
                 sizeof(UInt32) + 
                 valueSize;
        if(valueSize==0)
            continue;
        // send buffer if not enough mem
        if( int(_dataSize +
                size) > _bufferSize )
        {
            send(connection);
            if(size > _bufferSize)
                resize(connection,size);
        }
        putUInt8(CHANGED);
        putUInt32(fcPtr.getFieldContainerId());   // id
        putUInt32(mask);                          // mask
        putUInt32(valueSize);
        res=fcPtr->copyToBin(&_buffer[_dataSize],mask);
        if(valueSize != (res - (&_buffer[_dataSize])))
        {
            SFATAL << "getBinSize differes from written size" 
                   << valueSize << " <-> "  
                   << (res - (&_buffer[_dataSize])) << endl;
        }

        _dataSize+=valueSize;
        FDEBUG (( "Changed: ID:%d Mask:%d Size:%d\n",
                 fcPtr.getFieldContainerId(),
                 mask,
                 valueSize ))
    }

    // destroy fct
    for(destroyedI =changeList->beginDestroyed() ;
        destroyedI!=changeList->endDestroyed() ;
        destroyedI++)
    {
        if( int(_dataSize +
                sizeof(UInt8) +
                sizeof(UInt32)) > _bufferSize )
        {
            send(connection);
        }
        /*
          !!! BUG, If it is destroyed, then there is no 
          container ID. -> Bug in opensg sync

        putUInt8(DESTROYED);
        putUInt32(*destroyedI);
        FDEBUG (( "Destroyed: ID:%d\n",*destroyedI ))
        */
    }

    // addref
    for(addRefedI =changeList->beginAddRefd();
        addRefedI!=changeList->endAddRefd();
        addRefedI++)
    {
        //        id=addRefedI->getFieldContainerId();
        if( int(_dataSize +
                sizeof(UInt8) +
                sizeof(UInt32)) > _bufferSize )
        {
            send(connection);
        }
        putUInt8(ADDREFED);
        putUInt32(id);
    }
    
    // subref
    for(subRefedI =changeList->beginSubRefd();
        subRefedI!=changeList->endSubRefd();
        subRefedI++)
    {
        //        id=subRefedI->getFieldContainerId();
        if( int(_dataSize +
                sizeof(UInt8) +
                sizeof(UInt32)) > _bufferSize )
        {
            send(connection);
        }
        putUInt8(SUBREFED);
        putUInt32(id);
    }

    // send buffer if not enough mem
    if( int(_dataSize+sizeof(UInt8)) > _bufferSize )
    {
        send(connection);
    }
    putUInt8(SYNCENDED);
    send(connection);
    connection.flush();
}

void RemoteAspect::registerCreated(const FieldContainerType &type, 
                                   const Functor &func)
{
    while ( type.getId() >= _createdFunctors.size() )
    {
        _createdFunctors.push_back(
             osgFunctionFunctor2(&_defaultCreatedFunction));
    }
    _createdFunctors[ type.getId() ] = func;
}

void RemoteAspect::registerDestroyed(const FieldContainerType &type, 
                                     const Functor &func)
{
    while ( type.getId() >= _destroyedFunctors.size() )
    {
        _destroyedFunctors.push_back(
            osgFunctionFunctor2(&_defaultDestroyedFunction));
    }
    _destroyedFunctors[ type.getId() ] = func;
}

void RemoteAspect::registerChanged(const FieldContainerType &type, 
                                   const Functor &func)
{
    while ( type.getId() >= _changedFunctors.size() )
    {
        _changedFunctors.push_back(
            osgFunctionFunctor2(&_defaultChangedFunction));
    }
    _changedFunctors[ type.getId() ] = func;
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

RemoteAspect &RemoteAspect::operator = (const RemoteAspect &source)
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

void RemoteAspect::send(Connection &connection)
{
    connection.setDataSize(_dataSize);
    connection.send();
    _buffer    =connection.getBuffer();
    _bufferSize=connection.getBufferSize();
    _dataSize  =0;
}

void RemoteAspect::receive(Connection &connection)
{
    int size;
    
    size=connection.receive();
    if(size==0)
    {
        throw ConnectionClosed();
    }
    _buffer    =connection.getBuffer();
    _bufferSize=connection.getBufferSize();
    _dataSize  =connection.getDataSize();
    _dataPos   =0;
}

void RemoteAspect::resize(Connection &connection,int size)
{
    connection.resizeBuffer(size);
    _buffer    =connection.getBuffer();
    _bufferSize=connection.getBufferSize();
    _dataSize  =0;
    _dataPos   =0;
}

void RemoteAspect::putUInt8(UInt8 value)
{
    _buffer[_dataSize]=value;
    _dataSize++;
}

void RemoteAspect::putUInt32(UInt32 value)
{
    memcpy(&_buffer[_dataSize],&value,sizeof(UInt32));
    _dataSize+=sizeof(UInt32);
}

void RemoteAspect::putString(const char *value)
{
    strcpy((char*)&_buffer[_dataSize],value);
    _dataSize+=strlen(value)+1;
}

UInt8 RemoteAspect::getUInt8()
{
    return _buffer[_dataPos++];
}

UInt32 RemoteAspect::getUInt32()
{
    UInt32 value;
    memcpy(&value,&_buffer[_dataPos],sizeof(UInt32));
    _dataPos+=sizeof(UInt32);
    return value;
}

char *RemoteAspect::getString()
{
    char *res=(char*)&_buffer[_dataPos];
    _dataPos+=strlen(res)+1;
    return res;
}

Bool RemoteAspect::callCreated( FieldContainerPtr &fcp )
{
    Bool result;
    UInt32 uiFunctorIndex = fcp->getType().getId();

    if ( uiFunctorIndex < _createdFunctors.size() )
        result = _createdFunctors[uiFunctorIndex].call(fcp,this);
    else 
        result = _defaultCreatedFunction(fcp,this);

    return result;
}

Bool RemoteAspect::callDestroyed( FieldContainerPtr &fcp )
{
    Bool result;
    UInt32 uiFunctorIndex = fcp->getType().getId();

    if ( uiFunctorIndex < _destroyedFunctors.size() )
        result = _destroyedFunctors[uiFunctorIndex].call(fcp,this);
    else 
        result = _defaultDestroyedFunction(fcp,this);

    return result;
}

Bool RemoteAspect::callChanged( FieldContainerPtr &fcp )
{
    Bool result;
    UInt32 uiFunctorIndex = fcp->getType().getId();

    if ( uiFunctorIndex < _changedFunctors.size() )
        result = _changedFunctors[uiFunctorIndex].call(fcp,this);
    else 
        result = _defaultChangedFunction(fcp,this);

    return result;
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/


Bool RemoteAspect::_defaultCreatedFunction(FieldContainerPtr& fcp,
                                           RemoteAspect * aspect)
{
    FDEBUG (( "Created:%s %d\n",
              fcp->getType().getName().str(),
              fcp.getFieldContainerId() ))
    return true;
}

Bool RemoteAspect::_defaultDestroyedFunction(FieldContainerPtr& fcp,
                                           RemoteAspect * aspect)
{
    FDEBUG (( "Destroyed:%s %d\n",
              fcp->getType().getName().str(), 
              fcp.getFieldContainerId() ))
    return true;
}

Bool RemoteAspect::_defaultChangedFunction(FieldContainerPtr& fcp,
                                           RemoteAspect * aspect)
{
    FDEBUG (( "Changed:%s %d",
              fcp->getType().getName().str(), 
              fcp.getFieldContainerId() ))
    return true;
}


UInt32 RemoteAspectFieldContainerMapper::map(UInt32 uiId)
{
    UInt32 mappedId=0;
    std::map<UInt32,UInt32>::iterator i;

    i=_remoteAspect->_receivedFC.find(uiId);
    if(i==_remoteAspect->_receivedFC.end())
    {
        SWARNING << "Can't find container id:" << uiId << endl;
    }
    mappedId=i->second;
    FDEBUG (( "Map: %d to %d\n",uiId,mappedId ))
    return mappedId;
}
