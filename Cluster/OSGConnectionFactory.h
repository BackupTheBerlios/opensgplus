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

#ifndef _CONNECTIONFACTORY_H_
#define _CONNECTIONFACTORY_H_
#ifdef __sgi
#pragma once
#endif

#include <map>
#include <OSGBaseTypes.h>
#include <OSGClusterDef.h>
#include <OSGConnectionType.h>

OSG_BEGIN_NAMESPACE

//! Brief
//! \ingroup baselib

class OSG_CLUSTERLIB_DLLMAPPING ConnectionFactory 
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      types                                   */
    /*! \{                                                                 */

    typedef std::map<std::string,ConnectionType *> TypeMap;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   access                                     */
    /*! \{                                                                 */

    static ConnectionFactory &the();

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   interface                                  */
    /*! \{                                                                 */

    Connection *create(const string &name);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                        Dump                                  */
    /*! \{                                                                 */

    void dump(void);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                   fields                                     */
    /*! \{                                                                 */

    TypeMap _map;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    ConnectionFactory(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~ConnectionFactory(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Type information                            */
    /*! \{                                                                 */

    void addType(ConnectionType *type);
    void subType(ConnectionType *type);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    static ConnectionFactory *_the;

    /*!\brief prohibit default function (move to 'public' if needed) */
    ConnectionFactory(const ConnectionFactory &source);
    /*!\brief prohibit default function (move to 'public' if needed) */
    void operator =(const ConnectionFactory &source);

    friend class ConnectionType;
};

OSG_END_NAMESPACE

#define OSG_CONNECTIONFACTORYHEADER_CVSID "@(#)$Id:$"

#endif /* _CONNECTIONFACTORY_H_ */
