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
#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGClusterException.h"
#include "OSGFieldDescription.h"
#include "OSGRemoteAspect.h"
#include "OSGFieldContainer.h"
#include "OSGLog.h"
#include "OSGGeometry.h"
#include "OSGTextureChunk.h"
#include "OSGWindow.h"

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

char RemoteAspect::cvsid[] = "@(#)$Id: OSGRemoteAspect.cpp,v 1.1 2001/08/12 20:34:25 dirk Exp $";

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
    FieldContainerType  *type;

    // initialize field filter
    _fieldFilter[Geometry::getClassType().getId()] 
    = Geometry::GLIdFieldMask;
    _fieldFilter[TextureChunk::getClassType().getId()] 
    = TextureChunk::GLIdFieldMask;
    _fieldFilter[Window::getClassType().getId()] 
    = Window::GlObjectStatusFieldMask;

    type=FieldContainerFactory::the()->findType("GLUTWindow");
    if(type)
    {
        _fieldFilter[type->getId()] = Window::GlObjectStatusFieldMask;
    }
    type=FieldContainerFactory::the()->findType("QTWindow");
    if(type)
    {
        _fieldFilter[type->getId()] = Window::GlObjectStatusFieldMask;
    }
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
    UInt8  cmd;
    UInt32 remoteTypeId;
    UInt32 localTypeId;
    UInt32 remoteId;
    std::string name;
    FieldContainerFactory *factory=FieldContainerFactory::the();
    FieldContainerType *fcType;
    FieldContainerPtr fcPtr;
    BitVector mask;
    RemoteAspectFieldContainerMapper mapper;
    ReceivedTypeT::iterator receivedTypeI;
    ReceivedFCT::iterator receivedFCI;

    connection.selectChannel();
    // register mapper into factory
    mapper._remoteAspect=this;
    factory->setMapper(&mapper);
    do
    {
        connection.getUInt8(cmd);
        switch(cmd)
        {
            case NEWTYPE:
            {
                connection.getUInt32(remoteTypeId);
                connection.getString(name);
                // find local type
                fcType=FieldContainerFactory::the()->findType(name.c_str());
                if(!fcType)
                {
                    SWARNING << "Unknown Type: " << name << endl; 
                }
                else
                {
                    localTypeId=FieldContainerFactory::the()->
                        findType(name.c_str())->getId();
                    // insert remote type id into map
                    _receivedType[remoteTypeId]=localTypeId;
                }
                break;
            }
            case CREATED:
            {
                connection.getUInt32(remoteTypeId);
                connection.getUInt32(remoteId);
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
            }
            case DESTROYED:
            {
                connection.getUInt32(remoteId);
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
            }
            case CHANGED:
            {
                connection.getUInt32(remoteId);
                connection.getUInt32(mask);
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
                    fcPtr->copyFromBin(connection,mask);
                    // do we need to call this?
                    changedCP(fcPtr,mask);
                    callChanged(fcPtr);
                }
                break;
            }
            case SYNCENDED:
            {
                finish=true;
                break;
            }
            case ADDREFED:
            {
                connection.getUInt32(remoteId);
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
            }
            case SUBREFED:
            {
                connection.getUInt32(remoteId);
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
            default:
            {
                SFATAL << "Unknown tag:" << (int)cmd << endl;
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
    ChangeList::idrefd_const_iterator  createdI;
    ChangeList::idrefd_const_iterator  destroyedI;
    ChangeList::refd_const_iterator    addRefedI;
    ChangeList::refd_const_iterator    subRefedI;
    FieldFilterT::iterator filterI;
    FieldContainerFactory *fcFactory = FieldContainerFactory::the();
    FieldContainerPtr fcPtr;
    UInt32 typeId;
    BitVector mask;

    if(!changeList)
    {
        changeList=OSG::Thread::getCurrentChangeList();
    }

    // created fct
    for(createdI =changeList->beginCreated() ;
        createdI!=changeList->endCreated() ;
        createdI++)
    {
        fcPtr=fcFactory->getContainer(*createdI);
        
        if(fcPtr == NullFC)
            continue;

        typeId = fcPtr->getTypeId();
        // type unknown by remote context ?
        if(_sentType.count(typeId)==0)
        {
            // mark type as known
            _sentType.insert(typeId);
            // send new type
            connection.putUInt8(NEWTYPE);
            connection.putUInt32(typeId);
            connection.putString(fcPtr->getType().getName().str());
        }
        // send container to create
        connection.putUInt8(CREATED);
        connection.putUInt32(typeId);
        connection.putUInt32(*createdI);
    }

    // changed fields
    for(changedI =changeList->beginChanged() ;
        changedI!=changeList->endChanged() ;
        changedI++)
    {
        FieldContainerPtr fcPtr = 
            FieldContainerFactory::the()->getContainer(changedI->first);
        if(fcPtr == NullFC)
            continue;
        mask = changedI->second;
        filterI=_fieldFilter.find(fcPtr->getType().getId());
        // apply field filter
        if(filterI != _fieldFilter.end())
        {
            FDEBUG (( "SyncFieldFilter: :%s \n",
                      fcPtr->getType().getName().str() ))
            mask &= 0xFFFFFFFF ^ filterI->second;
        }
        // send changes
        connection.putUInt8(CHANGED);
        connection.putUInt32(fcPtr.getFieldContainerId());   // id
        connection.putUInt32(mask);                          // mask
        fcPtr->copyToBin(connection,mask);
        FDEBUG (( "Changed: ID:%d Mask:%d Size:%d\n",
                 fcPtr.getFieldContainerId(),
                 mask ))
    }

    // destroy fct
    for(destroyedI =changeList->beginDestroyed() ;
        destroyedI!=changeList->endDestroyed() ;
        destroyedI++)
    {
        /*
          !!! BUG, If it is destroyed, then there is no 
          container ID. -> Bug in opensg sync

          connection.putUInt8(DESTROYED);
          connection.putUInt32(*destroyedI);
          FDEBUG (( "Destroyed: ID:%d\n",*destroyedI ))
        */
    }

    // addref
    for(addRefedI =changeList->beginAddRefd();
        addRefedI!=changeList->endAddRefd();
        addRefedI++)
    {
        connection.putUInt8(ADDREFED);
        connection.putUInt32(*addRefedI);
    }
    
    // subref
    for(subRefedI =changeList->beginSubRefd();
        subRefedI!=changeList->endSubRefd();
        subRefedI++)
    {
        connection.putUInt8(SUBREFED);
        connection.putUInt32(*subRefedI);
    }

    connection.putUInt8(SYNCENDED);
    // write buffer 
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
                                           RemoteAspect *)
{
    FDEBUG (( "Created:%s %d\n",
              fcp->getType().getName().str(),
              fcp.getFieldContainerId() ))
    return true;
}

Bool RemoteAspect::_defaultDestroyedFunction(FieldContainerPtr& fcp,
                                           RemoteAspect *)
{
    FDEBUG (( "Destroyed:%s %d\n\n",
              fcp->getType().getName().str(), 
              fcp.getFieldContainerId() ))
    return true;
}

Bool RemoteAspect::_defaultChangedFunction(FieldContainerPtr& fcp,
                                           RemoteAspect *)
{
    FDEBUG (( "Changed:%s %d\n",
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
        SWARNING << "Can't find container id:\n" << uiId << endl;
    }
    mappedId=i->second;
    FDEBUG (( "Map: %d to %d\n",uiId,mappedId ))
    return mappedId;
}
