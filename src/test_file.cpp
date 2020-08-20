#include <type_traits>

#include "file.h"
#include "xml_config.h"

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& x) {
  for (auto& b : x) os << b << ' ';
  return os;
}

void test001() {
  int x;
  double y;
  File::File<File::Operation::Write, File::Type::Raw> w(
      "test_file_test001.rawtext");
  w << "1";
  w.close();
  File::File<File::Operation::Append, File::Type::Raw> a(
      "test_file_test001.rawtext");
  a << "2";
  a.close();
  File::File<File::Operation::Read, File::Type::Raw> r(
      "test_file_test001.rawtext");
  r >> x;
  r.close();
  File::File<File::Operation::Read, File::Type::Raw> r2(
      "test_file_test001.rawtext");
  r2 >> y;
  r2.close();
  std::cout << "write: " << 1 << 2 << '\n';
  std::cout << "read (as int, then double): " << x << ' ' << y << '\n';
}

void test002() {
  int x;
  double y, z;

  File::File<File::Operation::WriteBinary, File::Type::Raw> w(
      "test_file_test002.rawbin");
  w.write(int(12));
  w.close();
  File::File<File::Operation::AppendBinary, File::Type::Raw> a(
      "test_file_test002.rawbin");
  a.write(double(12 + 1.0 / 3));
  a.close();
  File::File<File::Operation::ReadBinary, File::Type::Raw> r(
      "test_file_test002.rawbin");
  r.read(x);
  r.close();
  File::File<File::Operation::ReadBinary, File::Type::Raw> r2(
      "test_file_test002.rawbin");
  r2.seek(sizeof(int));
  r2.read(y);
  r2.close();
  File::File<File::Operation::ReadBinary, File::Type::Raw> r3(
      "test_file_test002.rawbin");
  r3.seek<false>(sizeof(int));
  r3.read(z);
  r3.close();
  std::cout << "write: " << 12 << ' ' << 12 << '\n';
  std::cout << "read (last twice): " << x << ' ' << y << ' ' << z << '\n';
}

void test003() {
  std::vector<double> x{5.1, 2, 3, 1};
  std::vector<double> y(4);
  std::vector<int> z{5, 2, 3, 1};
  std::vector<int> p(4);

  File::File<File::Operation::WriteBinary, File::Type::Raw> w(
      "test_file_test003.rawbin");
  w.write(x);
  w.write(z);
  w.close();
  File::File<File::Operation::ReadBinary, File::Type::Raw> r(
      "test_file_test003.rawbin");
  r.read(y);
  r.read(p);
  r.close();
  std::cout << "print (double): " << x << '\n';
  std::cout << "print (int): " << z << '\n';
  std::cout << "read (double): " << y << '\n';
  std::cout << "read (int): " << p << '\n';
}

void test004() {
  File::File<File::Operation::Write, File::Type::Xml> w(
      "test_file_test004.xml");
  w.new_child("MyChild");
  w.add_attribute("Scream", "I am here");
  w.add_attribute("Born", false);
  w.add_attribute("Age", 0);
  w.add_attribute("Height", 0.5);
  w << '\n' << 3 << '\n';
  w.close();
  std::cout << "print attribute 'Scream=\"I am here\"'\n";
  std::cout << "print attribute 'Age=\"0\"'\n";
  std::cout << "print attribute 'Born=\"false\"'\n";
  std::cout << "print attribute 'Height=\"0.5\"'\n";
  std::cout << "print attribute 'size=\"2\"'\n";
  std::cout << "print data double: 3\n";

  File::File<File::Operation::Read, File::Type::Xml> r("test_file_test004.xml");
  auto c = r.get_child("MyChild");
  std::cout << "read attribute 'Scream': " << c.attribute("Scream").as_string()
            << '\n';
  std::cout << "read attribute 'Age': " << c.attribute("Age").as_int() << '\n';
  std::cout << std::boolalpha
            << "read attribute 'Born': " << c.attribute("Born").as_bool()
            << '\n';
  std::cout << "read attribute 'Height': " << c.attribute("Height").as_double()
            << '\n';
  int x;
  r >> x;
  std::cout << "read data (int): " << x << '\n';
  double y;
  r >> y;
  std::cout << "read data (double): " << y << '\n';
  r.close();
}

void test005() {
  File::File<File::Operation::Write, File::Type::Xml> w(
      "test_file_test005.xml");
  w.new_child("MyChild");
  w.add_attribute("Scream", "I am here");
  w.add_attribute("Born", false);
  w.add_attribute("Age", 0);
  w.add_attribute("Height", 0.5);
  w << '\n' << std::vector<double>{3.1, 2} << '\n';
  w.close();
  std::cout << "print attribute 'Scream=\"I am here\"'\n";
  std::cout << "print attribute 'Age=\"0\"'\n";
  std::cout << "print attribute 'Born=\"false\"'\n";
  std::cout << "print attribute 'Height=\"0.5\"'\n";
  std::cout << "print data double: 3.1 2\n";

  File::File<File::Operation::Read, File::Type::Xml> r("test_file_test005.xml");
  auto c = r.get_child("MyChild");
  std::cout << "read attribute 'Scream': " << c.attribute("Scream").as_string()
            << '\n';
  std::cout << "read attribute 'Age': " << c.attribute("Age").as_int() << '\n';
  std::cout << std::boolalpha
            << "read attribute 'Born': " << c.attribute("Born").as_bool()
            << '\n';
  std::cout << "read attribute 'Height': " << c.attribute("Height").as_double()
            << '\n';
  std::cout << "read attribute 'size': " << c.attribute("size").as_int()
            << '\n';
  std::vector<int> x(2);
  r >> x;
  std::cout << "read data (int): " << x << '\n';
  std::vector<double> y(2);
  r >> y;
  std::cout << "read data (double): " << y << '\n';
  r.close();

  File::File<File::Operation::Append, File::Type::Xml> a(
      "test_file_test005.xml");
  a.get_child("MyChild");
  a << 5.55555555555555555555555 << '\n';
  a.close();

  y.resize(3);
  File::File<File::Operation::Read, File::Type::Xml> r2(
      "test_file_test005.xml");
  r2.get_child("MyChild");
  r2 >> y;
  std::cout << "read data (double): " << y << '\n';
}

void test006() {
  File::File<File::Operation::WriteBinary, File::Type::Xml> w(
      "test_file_test006.xml");
  w.new_child("MyChild");
  w.add_attribute("Scream", "I am here");
  w.add_attribute("Born", false);
  w.add_attribute("Age", 0);
  w.add_attribute("Height", 0.5);
  w.write(double(3));
  w.close();
  std::cout << "print attribute 'Scream=\"I am here\"'\n";
  std::cout << "print attribute 'Age=\"0\"'\n";
  std::cout << "print attribute 'Born=\"false\"'\n";
  std::cout << "print attribute 'Height=\"0.5\"'\n";
  std::cout << "write data (double): " << double(3) << '\n';

  File::File<File::Operation::AppendBinary, File::Type::Xml> a(
      "test_file_test006.xml");
  a.get_child("MyChild");
  a.write(int(2));
  std::cout << "append data (int): " << 2 << '\n';
  a.close();

  File::File<File::Operation::ReadBinary, File::Type::Xml> r(
      "test_file_test006.xml");
  auto c = r.get_child("MyChild");
  std::cout << "read attribute 'Scream': " << c.attribute("Scream").as_string()
            << '\n';
  std::cout << "read attribute 'Age': " << c.attribute("Age").as_int() << '\n';
  std::cout << std::boolalpha
            << "read attribute 'Born': " << c.attribute("Born").as_bool()
            << '\n';
  std::cout << "read attribute 'Height': " << c.attribute("Height").as_double()
            << '\n';
  double x;
  int y;
  r.read(x);
  std::cout << "read data (double): " << x << '\n';
  r.read(y);
  std::cout << "read data (int): " << y << '\n';
}

void test007() {
  File::File<File::Operation::WriteBinary, File::Type::Xml> w(
      "test_file_test007.xml");
  w.new_child("MyChild");
  w.add_attribute("Scream", "I am here");
  w.add_attribute("Born", false);
  w.add_attribute("Age", 0);
  w.add_attribute("Height", 0.5);
  std::vector<double> tmp1 = {5, 1, .5555, 3};
  w.write(tmp1);
  w.close();
  std::cout << "print attribute 'Scream=\"I am here\"'\n";
  std::cout << "print attribute 'Age=\"0\"'\n";
  std::cout << "print attribute 'Born=\"false\"'\n";
  std::cout << "print attribute 'Height=\"0.5\"'\n";
  std::cout << "write data (double): " << std::vector<double>{5, 1, .5555, 3}
            << '\n';

  File::File<File::Operation::AppendBinary, File::Type::Xml> a(
      "test_file_test007.xml");
  a.get_child("MyChild");
  std::vector<int> tmp2 = {5, 1, 5555, 3};
  a.write(tmp2);
  std::cout << "append data (int): " << std::vector<int>{5, 1, 5555, 3} << '\n';
  a.close();

  File::File<File::Operation::ReadBinary, File::Type::Xml> r(
      "test_file_test007.xml");
  auto c = r.get_child("MyChild");
  std::cout << "read attribute 'Scream': " << c.attribute("Scream").as_string()
            << '\n';
  std::cout << "read attribute 'Age': " << c.attribute("Age").as_int() << '\n';
  std::cout << std::boolalpha
            << "read attribute 'Born': " << c.attribute("Born").as_bool()
            << '\n';
  std::cout << "read attribute 'Height': " << c.attribute("Height").as_double()
            << '\n';
  std::vector<double> x(4);
  std::vector<int> y(4);
  r.read(x);
  std::cout << "read data (double): " << x << '\n';
  r.read(y);
  std::cout << "read data (int): " << y << '\n';
}

void test008() {
  File::File<File::Operation::Write, File::Type::Xml> w("test_config.xml");
  w.new_child("MyChild");
  w.add_attribute("Scream", "I am here");
  w.add_attribute("Born", false);
  w.add_attribute("Age", 0);
  w.add_attribute("Height", 0.5);
  w.leave_child();
  w.new_child("MyOtherChild");
  w.add_attribute("Scream", "I am also here");
  w.add_attribute("Born", true);
  w.add_attribute("Age", 1);
  w.add_attribute("Height", 0.9);
  w.leave_child();
  w.close();

  std::cout << File::ConfigParser("test_config.xml",
                                  {"MyChild", "MyOtherChild"})
            << '\n';
}

int main() {
  std::cout << "---------------------------------------Raw Text\n";
  test001();  // Test Raw Text IO
  std::cout << "---------------------------------------Raw Binary\n";
  test002();  // Test Raw Binary IO
  std::cout << "---------------------------------------Raw Binary Vector\n";
  test003();  // Test Raw Binary IO std::vector<>
  std::cout << "---------------------------------------Xml Text\n";
  test004();  // Test XML Text IO Scalar
  std::cout << "---------------------------------------Xml Text Vector\n";
  test005();  // Test XML Text IO Vector and Append
  std::cout << "---------------------------------------Xml Binary\n";
  test006();  // Test XML Binary IO Scalar
  std::cout << "---------------------------------------Xml Binary Vector\n";
  test007();  // Test XML Binary IO Vector
  std::cout << "---------------------------------------Config Parser\n";
  test008();  // Test the configuration parser
  std::cout << "---------------------------------------\n";
}
