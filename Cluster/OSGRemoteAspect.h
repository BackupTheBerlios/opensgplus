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


#ifndef _REMOTEASPECT_H_
#define _REMOTEASPECT_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <map>
#include <set>
#include <OSGClusterDef.h>
#include <OSGBaseTypes.h>
#include <OSGFieldContainerPtr.h>
#include <OSGFieldContainerFactory.h>
#include <OSGChangeList.h>
#include <OSGConnection.h>
#include <OSGFunctors.h>

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

class OSG_CLUSTERLIB_DLLMAPPING RemoteAspect
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

    enum DataTypes 
    {
        SYNCENDED =1,
        CREATED   =2,
        DESTROYED =3,
        NEWTYPE   =4,
        CHANGED   =5,
        ADDREFED  =6,
        SUBREFED  =7
    };
    typedef Functor2Base<Bool,    FieldContainerPtr &, RemoteAspect *> Functor;
    typedef map<UInt32,UInt32>    ReceivedFCT;
    typedef map<UInt32,UInt32>    ReceivedTypeT;
    typedef map<UInt32,BitVector> FieldFilterT;
    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

    RemoteAspect();
    virtual ~RemoteAspect(void); 

    /*------------------------- your_category -------------------------------*/

    void receiveSync(Connection &connection);
    void sendSync(Connection &connection,OSG::ChangeList *changeList=NULL);

    void registerCreated(const FieldContainerType &type, 
                         const Functor &func);
    void registerDestroyed(const FieldContainerType &type, 
                           const Functor &func);
    void registerChanged(const FieldContainerType &type, 
                         const Functor &func);

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

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------

    // remote id to fieldcontainer mapping
    ReceivedFCT                       _receivedFC;
    // remote typeid mapping
    ReceivedTypeT                     _receivedType;
    // is type mapped
    set<UInt32>                       _sentType;
    // fild filter
    FieldFilterT                      _fieldFilter;

    vector<Functor>                   _createdFunctors;
    vector<Functor>                   _destroyedFunctors;
    vector<Functor>                   _changedFunctors;
    
    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------
    
    void   send(Connection &connection);
    void   receive(Connection &connection);
    Bool   callCreated( FieldContainerPtr &node );
    Bool   callDestroyed( FieldContainerPtr &node );
    Bool   callChanged( FieldContainerPtr &node );

  private:

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   friend classes                                                      
    //-----------------------------------------------------------------------
    friend class RemoteAspectFieldContainerMapper;

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

    static Bool _defaultCreatedFunction(FieldContainerPtr& fcp,
                                                      RemoteAspect * aspect);
    static Bool _defaultDestroyedFunction(FieldContainerPtr& fcp,
                                                      RemoteAspect * aspect);
    static Bool _defaultChangedFunction(FieldContainerPtr& fcp,
                                                      RemoteAspect * aspect);

    //-----------------------------------------------------------------------
    //   instance variables                                                  
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

	// prohibit default functions (move to 'public' if you need one)

    RemoteAspect(const RemoteAspect &source);
    RemoteAspect &operator =(const RemoteAspect &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef RemoteAspect *RemoteAspectP;

struct RemoteAspectFieldContainerMapper:public FieldContainerMapper
{                                              
    virtual UInt32 map(UInt32 uiId);
    RemoteAspect *_remoteAspect;
};

OSG_END_NAMESPACE

#endif /* _REMOTEASPECT_H_ */
