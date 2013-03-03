
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>

#include "SystemTest.h"
#include "MeasurementTest.h"

int main(int argc, char* argv[]) {
    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    controller.addListener(&result);
    
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    
    runner.addTest( suite );
    runner.run(controller);
    
    // Change the default outputter to a compiler error format
    // outputter
    std::ofstream xmlFileOut("cpptestresults.xml");
    CppUnit::XmlOutputter xmlOut(&result, xmlFileOut);
    xmlOut.write();

    // Return error code 1 if the one of test failed.
    return result.testFailuresTotal() ? 1 : 0 ;
}
