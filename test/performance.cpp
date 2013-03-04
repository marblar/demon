
#include <cppunit/XmlOutputter.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>

#include "Clocker/ClockerListener.h"
#include "Clocker/ClockerModel.h"
#include "Clocker/ClockerXmlHook.h"

#include "IsingPerformance.h"

int main(int argc, char* argv[]) {
    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    ClockerModel clockModel;
    
    ClockerListener timer(&clockModel,true);
    controller.addListener(&result);
    controller.addListener(&timer);
    
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    
    runner.addTest(suite);
    runner.run(controller);
    
    ClockerXmlHook xmlHook(&clockModel);
    
    // Change the default outputter to a compiler error format
    // outputter
    std::ofstream xmlFileOut("performance.stats");
    CppUnit::XmlOutputter xmlOut(&result, xmlFileOut);
    xmlOut.addHook(&xmlHook);
    xmlOut.write();
    
    CppUnit::CompilerOutputter compileOut(&result,std::cerr);
    compileOut.write();

    // Return error code 1 if the one of test failed.
    return result.testFailuresTotal() ? 1 : 0 ;
}
