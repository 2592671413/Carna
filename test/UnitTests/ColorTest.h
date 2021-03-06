/*
 *  Copyright (C) 2010 - 2015 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#pragma once

namespace Carna
{

namespace testing
{



// ----------------------------------------------------------------------------------
// ColorTest
// ----------------------------------------------------------------------------------

/** \brief
  * Unit-tests of the \ref Carna::base::Color class.
  *
  * \author Leonid Kostrykin
  * \date   4.4.15
  */
class ColorTest : public QObject
{

    Q_OBJECT

private slots:

    /** \brief
      * Called before the first test function is executed.
      */
    void initTestCase();

    /** \brief
      * Called after the last test function is executed.
      */
    void cleanupTestCase();

    /** \brief
      * Called before each test function is executed.
      */
    void init();

    /** \brief
      * Called after each test function is executed.
      */
    void cleanup();

 // ----------------------------------------------------------------------------------

    void test_RED();
    
    void test_copy_ctor();
    
    void test_assignment_operator();
    
}; // mathTest



}  // namespace Carna :: testing

}  // namespace Carna
