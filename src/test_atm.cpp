#include "atm.h"

void test001() {
  Atmosphere::AtmPoint a
                      (100e3,
                       296.,
                       std::array<double, 3>{10e-6, 10e-6,30e-6},
                       std::array<double, 3>{10., 1., 0.1},
                       std::vector<VMR<VMRType::ratio>>{
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0), 0.78},
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0), 0.2095},
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0), 400e-06}}
                      );
  std::stringstream ss;
  std::cout << a << '\n';
  ss << a;
  
  Atmosphere::AtmPoint b(a.size());
  ss >> b;
  
  std::cout << b << '\n';
}

void test002() {
  Atmosphere::AtmPoint a
                      (100e3,
                       296.,
                       std::array<double, 3>{10e-6, 10e-6,30e-6},
                       std::array<double, 3>{10., 1., 0.1},
                       std::vector<VMR<VMRType::ratio>>{
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0), 0.78},
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0), 0.2095},
                         VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0), 400e-06}}
                      );
  Atmosphere::AtmPoint b;
  
  File::File<File::Operation::Write, File::Type::Xml> save("test_atm_test002.xml");
  saveAtmPoint(save, a);
  save.close();
  std::cout << a << '\n';
  
  File::File<File::Operation::Read, File::Type::Xml> read("test_atm_test002.xml");
  readAtmPoint(read, b);
  read.close();
  std::cout << b << '\n';
  
}

int main()
{
  test001();  // Printing to screen and reading printed values (AtmPoint)
  test002();  // Printing to XML file and reading XML file (AtmPoint; ASCII)
}
