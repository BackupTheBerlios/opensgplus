//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:12:36 $
//                                                                            
//=============================================================================

#ifndef _OSGADAPTERDRAWACTION_H_
#define _OSGADAPTERDRAWACTION_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <vector>
#include "OSGBaseTypes.h"
#include "OSGDrawAction.h"
#include "OSGGVCache.h"

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------

class Material;

//---------------------------------------------------------------------------
//   Types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

/*! \brief DrawAction class
 */

class OSG_GENVISLIB_DLLMAPPING AdapterDrawAction : public DrawAction
{
public:

    //-----------------------------------------------------------------------
    //   enums                                                               
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   types                                                               
    //-----------------------------------------------------------------------

    typedef DrawAction        Inherited;
    typedef AdapterDrawAction Self;

    //-----------------------------------------------------------------------
    //   class functions                                                     
    //-----------------------------------------------------------------------

    static const char* getClassname(void) { return "AdapterDrawAction"; }

    // create a new AdapterDrawAction by cloning the prototype
    static AdapterDrawAction* create( void );
    
    // prototype access
    // after setting the prototype all new AdapterDrawActions are clones of it
    static void               setPrototype( AdapterDrawAction* proto);
    static AdapterDrawAction* getPrototype( void );

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------

    virtual ~AdapterDrawAction(void); 

    /*------------------------- your_category -------------------------------*/

    inline bool getLocalMode () const;
    inline void setLocalMode (bool flag=true);

    /*------------------------- your_operators ------------------------------*/

    /*------------------------- assignment ----------------------------------*/

    /*------------------------- comparison ----------------------------------*/

    bool operator < (const AdapterDrawAction &other) const;
    
    bool operator == (const AdapterDrawAction &other) const;
    bool operator != (const AdapterDrawAction &other) const;

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

    bool m_mode;

    //-----------------------------------------------------------------------
    //   instance functions                                                  
    //-----------------------------------------------------------------------
    
    // select all visible nodes
    virtual UInt32  selectVisibles       (void);
    UInt32          selectVisiblesLocal  (void);
    UInt32          selectVisiblesGlobal (void);

    typedef UInt32 (Self::*SelectVisiblesFunc)(void);
    SelectVisiblesFunc f_selectVisibles;

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

    //-----------------------------------------------------------------------
    //   friend functions                                                    
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //   class variables                                                     
    //-----------------------------------------------------------------------

    static char cvsid[];

    // the prototype which is copied to create new actions
    static AdapterDrawAction* _prototype;

#if 0
    // default functors for instantiation
    static vector<Functor>* _defaultEnterFunctors;
    static vector<Functor>* _defaultLeaveFunctors;
#endif
    
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

    AdapterDrawAction (void);
    AdapterDrawAction (const AdapterDrawAction &source);
    AdapterDrawAction& operator =(const AdapterDrawAction &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef AdapterDrawAction* AdapterDrawActionP;

OSG_END_NAMESPACE

#include "OSGAdapterDrawAction.inl"

#endif /* _OSGADAPTERDRAWACTION_H_ */


