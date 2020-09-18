#include "geom.h"

#include <Eigen/Dense>

namespace Geom {
void saveNav(File::File<File::Operation::Write, File::Type::Xml> &out,
             const Nav &n) {
  out.new_child("Nav");
  out << '\n' << n << '\n';
  out.leave_child();
}

void saveNav(File::File<File::Operation::WriteBinary, File::Type::Xml> &out,
             const Nav &n) {
  out.new_child("Nav");
  out.write(n);
  out.leave_child();
}

void readNav(File::File<File::Operation::Read, File::Type::Xml> &in, Nav &n) {
  in.get_child("Nav");
  in >> n;
  in.leave_child();
}
void readNav(File::File<File::Operation::ReadBinary, File::Type::Xml> &in,
             Nav &n) {
  in.get_child("Nav");
  in.read(n);
  in.leave_child();
}

Los<LosType::Xyz> rotate(const Los<LosType::Xyz> a, const Los<LosType::Xyz> b,
                         const double angle) {
  using Constant::pow2;
  using Conversion::sind;
  const double sa = sind(angle);
  const double s2 = pow2(sind(angle / 2));
  return Los<LosType::Xyz>{std::array<double, 3>{
      -b.dx() * (2 * s2 * (pow2(a.dy()) + pow2(a.dz())) - 1) +
          b.dy() * (2 * a.dx() * a.dy() * s2 - a.dz() * sa) +
          b.dz() * (2 * a.dx() * a.dz() * s2 + a.dy() * sa),
      b.dx() * (2 * a.dx() * a.dy() * s2 + a.dz() * sa) -
          b.dy() * (2 * s2 * (pow2(a.dx()) + pow2(a.dz())) - 1) -
          b.dz() * (a.dx() * sa - 2 * a.dy() * a.dz() * s2),
      b.dx() * (2 * a.dx() * a.dz() * s2 - a.dy() * sa) +
          b.dy() * (a.dx() * sa + 2 * a.dy() * a.dz() * s2) -
          b.dz() * (2 * s2 * (pow2(a.dx()) + pow2(a.dy())) - 1)}};
}

}  // namespace Geom
