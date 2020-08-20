#include <Eigen/Core>
#include <chrono>

#include "python_interface.h"
#include "timeclass.h"

// Python must be created and survive until the end of the python calls
auto pyinterp = Python::createPython();

void test001() {
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
  std::cout << ho.toString() << ' ' << ha.toDouble() << '\n';

  // Print class variables
  help_p1();
  help_p2();
  help_p3();

  // Puts a plot on screen and freeze the test until it is quit
  py::print(
      "Now look at and enjoy this nice plot for your effort (close it to "
      "finish the test)");
  py::exec(R"(
    plt.plot(np.linspace(0, 6, 101), np.sin(np.linspace(0, 6, 101)))
    plt.show()
  )");
}

void test002() {
  // Executes a very simply script importing important libraries and
  // defining some random classes and functions and puts it in main scope
  py::exec(R"(
    import numpy as np
    
    def mul(arraya, arrayb):
        arraya*=np.array(arrayb)
        return 2*arraya
    
    def none(a):
        return a
    
    def printy(a):
        for x in a:
            for y in x:
                print(y)
        
  )");

  const Python::Function mul{"mul"};
  const Python::Function none{"none"};
  const Python::Function print{"printy"};
  py::print(mul(1, 3.).Obj);

  std::vector<double> a{1, 2, 3, 4};
  auto copy = py::array(a.size(), a.data());
  auto arr = mul(copy, 3.0);
  py::print(arr.Obj);
  py::print(copy);
  auto n = Python::Object<Python::Type::NumpyVector>(none(arr.Obj))
               .toVector<double>();
  for (auto i : n) std::cout << i << ' ';
  std::cout << '\n';

  Eigen::MatrixXd eigtest;
  eigtest.resize(2, 2);
  eigtest << 1, 2, 3, 4;
  std::cout << eigtest << '\n';
  py::print(mul(Eigen::Ref<Eigen::MatrixXd>(eigtest), 3.).Obj);
  std::cout << eigtest << '\n';
  print(Eigen::Ref<Eigen::MatrixXd>(eigtest));
}

void test003() {
  // Executes a very simply script importing important libraries and
  // defining some random classes and functions and puts it in main scope
  py::exec(R"(
    def f():
        return {"a": "b", "c": "d", "1": "3.14"};
    def e():
        return {"a": 1, "c": 2.2, "1": 3.14};
  )");

  const Python::Function dict_fun1{"f"};
  const Python::Function dict_fun2{"e"};
  const Python::Object<Python::Type::Dict> dict1{dict_fun1()};
  const Python::Object<Python::Type::Dict> dict2{dict_fun2()};
  for (auto& a : dict1.keysDict())
    std::cout << a << ": " << dict1.fromDict<Python::Type::String>(a).toString()
              << '\n';
  for (auto& a : dict2.keysDict())
    std::cout << a << ": " << dict2.fromDict<Python::Type::Double>(a).toDouble()
              << '\n';
}

int main() {
  //   test001();  // Standard python
  //   test002();  // Numpy arrays
  test003();  // Numpy arrays
}
