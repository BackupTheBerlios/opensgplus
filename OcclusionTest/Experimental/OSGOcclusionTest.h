/*---------------------------------------------------------------------------*\
 *                     OpenSG PLUS OcclusionTest Test                            *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2002 by the WSI/GRIS Uni Tuebingen              *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: staneker@gris.uni-tuebingen.de                                 *
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

#ifndef _OSGOCCLUSIONTEST_H_
#define _OSGOCCLUSIONTEST_H_

#ifdef __sgi
#pragma once
#endif

#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>

#include <OSGOcclusionStatistics.h>

#include <OSGHPTest.h>
#include <OSGNVTest.h>
#include <OSGOcclusionTest.h>
#include <OSGStencilTest.h>

#include <OSGOcclusionTypes.h>

OSG_BEGIN_NAMESPACE

class Viewport;

//! OcclusionTest
//! \ingroup RenderingBackendLib

template<typename octest, typename ocstatistics>
class OSG_SYSTEMLIB_DLLMAPPING OcclusionTest
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Statistic                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    OcclusionTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~OcclusionTest(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const UInt16&, Viewport* v=NULL, const UInt32 maxpix=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const UInt16&, const OCTestNode*);
    UInt32 result(const UInt16&);
    void exit(void);
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */
    ocstatistics _stats;
    octest _octest;

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    /*! \brief prohibit default function (move to 'public' if needed) */
    OcclusionTest(const OcclusionTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const OcclusionTest &source);
};

typedef OcclusionTest<HPTest, OcclusionStatisticsOff> OcclusionHPTest;
typedef OcclusionTest<HPTest, OcclusionStatisticsOn> OcclusionHPTestStats;
typedef OcclusionTest<NVTest, OcclusionStatisticsOff> OcclusionNVTest;
typedef OcclusionTest<NVTest, OcclusionStatisticsOn> OcclusionNVTestStats;
typedef OcclusionTest<StencilTest, OcclusionStatisticsOff> OcclusionStencilTest;
typedef OcclusionTest<StencilTest, OcclusionStatisticsOn> OcclusionStencilTestStats;

OSG_END_NAMESPACE

#include "OSGOcclusionTest.inl"

#endif /* _OSGOCCLUSIONTEST_H_ */
