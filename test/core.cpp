
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "SystemTest.h"

int main(int argc, char* argv[])
{
    
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    
    runner.addTest( suite );
    

    // Change the default outputter to a compiler error format
    // outputter
    std::ofstream xmlFileOut("cpptestresults.xml");
    CppUnit::XmlOutputter xmlOut(&runner.result(), xmlFileOut);
    xmlOut.write();
    
    // Run the tests.
    bool wasSucessful = runner.run();
    

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;    
}
