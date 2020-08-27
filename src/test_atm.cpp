#include "atm.h"

void test001() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{10e-6, 10e-6, 30e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  std::stringstream ss;
  std::cout << "Write: " << a << '\n';
  ss << a;

  Atmosphere::Point b(a.size());
  ss >> b;

  std::cout << "Read:  " << b << '\n';
}

void test002() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{10e-6, 10e-6, 30e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b;

  File::File<File::Operation::Write, File::Type::Xml> save(
      "test_atm_test002.xml");
  savePoint(save, a);
  save.close();
  std::cout << "Write: " << a << '\n';

  File::File<File::Operation::Read, File::Type::Xml> read(
      "test_atm_test002.xml");
  readPoint(read, b);
  read.close();
  std::cout << "Read:  " << b << '\n';
}

void test003() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{10e-6, 10e-6, 30e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b;

  File::File<File::Operation::WriteBinary, File::Type::Xml> save(
      "test_atm_test003.xml");
  savePoint(save, a);
  save.close();
  std::cout << "Write: " << a << '\n';

  File::File<File::Operation::ReadBinary, File::Type::Xml> read(
      "test_atm_test003.xml");
  readPoint(read, b);
  read.close();
  std::cout << "Read:  " << b << '\n';
}

void test004() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{10e-6, 10e-6, 30e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Atm atm({Time(), Time() + TimeStep(1)}, {0, 1}, {0, 2}, {0, 4},
                      Grid<Atmosphere::Point, 4>(a, 2, 2, 2, 2));
  std::cout << "Write:\n" << atm << '\n';

  Atmosphere::Atm batm(2, 2, 2, 2, 3);
  std::stringstream ss;
  ss << atm;
  ss >> batm;

  std::cout << "Read:\n" << batm << '\n';
}

void test005() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{11e-6, 11e-6, 31e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b(
      10e3, 292., std::array<double, 3>{15e-6, 15e-6, 35e-6},
      std::array<double, 3>{15., 2., 0.2},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.74},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2055},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              600e-06}});
  Time t;
  Atmosphere::Atm atm({t, t + TimeStep(1)}, {0, 1}, {0, 2}, {0, 4},
                      Grid<Atmosphere::Point, 4>(a, 2, 2, 2, 2));
  std::cout << "Write (original):\n" << atm << '\n';
  atm(0, 0, 0, 0) = b;

  std::cout << "Write (manipulated):\n" << atm << '\n';

  std::cout << "Write (Point at middle time first rest):\n"
            << atm(t + TimeStep(0.5), 0, 0, 0) << '\n';
  std::cout << "Write (Point at middle alt first rest): \n"
            << atm(t, 0.5, 0, 0) << '\n';
  std::cout << "Write (Point at middle lat first rest): \n"
            << atm(t, 0, 1, 0) << '\n';
  std::cout << "Write (Point at middle lon first rest): \n"
            << atm(t, 0, 0, 2) << '\n';

  std::cout << "Write (Point at quarter time first rest):\n"
            << atm(t + TimeStep(0.25), 0, 0, 0) << '\n';
  std::cout << "Write (Point at quarter alt first rest): \n"
            << atm(t, 0.25, 0, 0) << '\n';
  std::cout << "Write (Point at quarter lat first rest): \n"
            << atm(t, 0, 0.5, 0) << '\n';
  std::cout << "Write (Point at quarter lon first rest): \n"
            << atm(t, 0, 0, 1) << '\n';

  std::cout << "Write (Point at three-quarter time first rest):\n"
            << atm(t + TimeStep(0.75), 0, 0, 0) << '\n';
  std::cout << "Write (Point at three-quarter alt first rest): \n"
            << atm(t, 0.75, 0, 0) << '\n';
  std::cout << "Write (Point at three-quarter lat first rest): \n"
            << atm(t, 0, 1.5, 0) << '\n';
  std::cout << "Write (Point at three-quarter lon first rest): \n"
            << atm(t, 0, 0, 3) << '\n';

  std::cout << "Write (Point at middle all):\n"
            << atm(t + TimeStep(0.5), 0.5, 1, 2) << '\n';
  std::cout << "Write (Point at quarter all):\n"
            << atm(t + TimeStep(0.25), 0.25, 0.5, 1) << '\n';
  std::cout << "Write (Point at three-quarter all):\n"
            << atm(t + TimeStep(0.75), 0.75, 1.5, 3) << '\n';
}

void test006() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{11e-6, 11e-6, 31e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b(
      10e3, 292., std::array<double, 3>{15e-6, 15e-6, 35e-6},
      std::array<double, 3>{15., 2., 0.2},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.74},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2055},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              600e-06}});
  Time t;
  Atmosphere::Atm atm({t, t + TimeStep(1)}, {0, 1}, {0, 2}, {0, 364},
                      Grid<Atmosphere::Point, 4>(a, 2, 2, 2, 2));
  atm(0, 0, 0, 0) = b;
  std::cout << "Write (original):\n" << atm << '\n';

  File::File<File::Operation::Write, File::Type::Xml> save(
      "test_atm_test006.xml");
  saveAtm(save, atm);
  save.close();

  Atmosphere::Atm batm;
  File::File<File::Operation::Read, File::Type::Xml> read(
      "test_atm_test006.xml");
  readAtm(read, batm);
  read.close();
  std::cout << "Read (loaded):\n" << batm << '\n';
}

void test007() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{11e-6, 11e-6, 31e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b(
      10e3, 292., std::array<double, 3>{15e-6, 15e-6, 35e-6},
      std::array<double, 3>{15., 2., 0.2},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.74},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2055},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              600e-06}});
  Time t;
  Atmosphere::Atm atm({t, t + TimeStep(1)}, {0, 1}, {0, 2}, {0, 364},
                      Grid<Atmosphere::Point, 4>(a, 2, 2, 2, 2));
  atm(0, 0, 0, 0) = b;
  std::cout << "Write (original):\n" << atm << '\n';

  File::File<File::Operation::WriteBinary, File::Type::Xml> save(
      "test_atm_test007.xml");
  saveAtm(save, atm);
  save.close();

  Atmosphere::Atm batm;
  File::File<File::Operation::ReadBinary, File::Type::Xml> read(
      "test_atm_test007.xml");
  readAtm(read, batm);
  read.close();
  std::cout << "Read (loaded):\n" << batm << '\n';
}

void test008() {
  Atmosphere::Point a(
      100e3, 296., std::array<double, 3>{11e-6, 11e-6, 31e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Point b(
      10e3, 292., std::array<double, 3>{15e-6, 15e-6, 35e-6},
      std::array<double, 3>{15., 2., 0.2},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.74},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2055},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              600e-06}});
  Time t;
  Atmosphere::Atm atm({t}, {0, 100e3}, {0}, {0},
                      Grid<Atmosphere::Point, 4>(a, 1, 2, 1, 1));
  atm(0, 0, 0, 0) = b;
  std::cout << "Write (original atmosphere):\n" << atm << '\n';

  for (double alt = 0; alt <= 100e3; alt += 1e3)
    std::cout << alt << ' ' << atm(t, alt, 0, 0) << '\n';
}

int main() {
  std::cout << "Tests of the Atmosphere namespace\n";
  std::cout << '\n'
            << "Printing to screen and reading printed values (Point)" << '\n';
  test001();  // Printing to screen and reading printed values (Point)
  std::cout << '\n'
            << "Printing to XML file and reading XML file (Point; ASCII)"
            << '\n';
  test002();  // Printing to XML file and reading XML file (Point; ASCII)
  std::cout << '\n'
            << "Printing to XML file and reading XML file (Point; Binary)"
            << '\n';
  test003();  // Printing to XML file and reading XML file (Point; Binary)
  std::cout << '\n'
            << "Printing to screen and reading printed values (Atm)" << '\n';
  test004();  // Printing to screen and reading printed values (Atm)
  std::cout << '\n' << "Manipulating values and off-grid (Atm+Point)" << '\n';
  test005();  // Printing to screen and reading printed values (Atm)
  std::cout << '\n'
            << "Printing to XML file and reading XML file (Atm; ASCII)" << '\n';
  test006();  // Printing to XML file and reading XML file (Point; ASCII)
  std::cout << '\n'
            << "Printing to XML file and reading XML file (Atm; Binary)"
            << '\n';
  test007();  // Printing to XML file and reading XML file (Point; Binary)
  std::cout << '\n'
            << "Calc just two points to see how they change with altitude"
            << '\n';
  test008();  // Calc just two points to see how they change with altitude
}
