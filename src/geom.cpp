#include "geom.h"

namespace Geom {
void saveNav(File::File<File::Operation::Write, File::Type::Xml>& out,
             const Nav& n) {
  out.new_child("Nav");
  out << '\n' << n << '\n';
  out.leave_child();
}

void saveNav(File::File<File::Operation::WriteBinary, File::Type::Xml>& out,
             const Nav& n) {
  out.new_child("Nav");
  out.write(n);
  out.leave_child();
}

void readNav(File::File<File::Operation::Read, File::Type::Xml>& in, Nav& n) {
  in.get_child("Nav");
  in >> n;
  in.leave_child();
}
void readNav(File::File<File::Operation::ReadBinary, File::Type::Xml>& in,
             Nav& n) {
  in.get_child("Nav");
  in.read(n);
  in.leave_child();
}
}  // namespace Geom
