#include <chrono>

#include "python_interface.h"
#include "timeclass.h"

void test001()
{
  // Python must be created and survive until the end of the python calls
  auto py = Python::createPython();
  
  // Executes a very simply script importing important libraries and
  // defining some random classes and functions and puts it in main scope
  py::exec(R"(
    import sympy
    import scipy as sp
    import numpy as np
    import serial
    import matplotlib.pyplot as plt
    import re
    
    a = sympy.symbols('b')
  
    print(a)
    
    def oi(a=3):
        print(a)
    
    def yo(a):
        return a+a
    
    class helper:
        def __init__(self, somestuff):
            self.some = somestuff
        def p(self):
            print(self.some)
  )");
  
  // Define functions and classes
  const Python::Function oi{"oi"};
  const Python::Function yo{"yo"};
  const Python::ClassInterface helper{"helper"};
  const Python::ClassInstance help1 = helper("I am testing!");
  const Python::ClassInstance help2 = helper("Does it work?");
  const Python::ClassInstance help3 = helper("Congratulations! It works!");
  const Python::Function help_p1 = help1("p");
  const Python::Function help_p2 = help2("p");
  const Python::Function help_p3 = help3("p");

  // Print inputs to screen
  oi("HI");
  oi(3);

  // Print results to screen twice
  const Python::Object<Python::Type::String> ho{yo("hoho")};
  const Python::Object<Python::Type::Double> ha{yo(3)};
  py::print(ho.Obj, ha.Obj);
  std::cout<<ho.toString() << ' ' << ha.toDouble() << '\n';
  
  // Print class variables
  help_p1();
  help_p2();
  help_p3();
  
  // Puts a plot on screen and freeze the program until it is quit
  py::print("Now look at and enjoy this nice plot for your effort (close it to finish the program)");
  py::exec(R"(
    plt.plot(np.linspace(0, 6, 101), np.sin(np.linspace(0, 6, 101)))
    plt.show()
  )");
}

int main() {
  test001();
}
