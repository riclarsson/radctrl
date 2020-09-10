#include "lbl.h"
#include "lineshapes.h"
#include "mathhelpers.h"

void test001() {
  constexpr double T = 275;
  constexpr double dT = 0.01;
  constexpr double df = 1e1;
  constexpr double P = 1e-5;
  constexpr double vmrn2 = 0.78;
  constexpr double vmro2 = 0.2095;
  constexpr double vmrh2o = 400e-06;
  auto VMRS = std::vector<VMR<VMRType::ratio>>{
      VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                          vmrn2},
      VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0), vmro2},
      VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                          vmrh2o}};

  const Species::Isotope O266(Species::Species::Oxygen, 0);
  const std::vector<Quantum::Number> g(
      getGlobalQuantumNumberCount(Species::Species::Oxygen));
  const std::vector<Quantum::Number> l(
      getLocalQuantumNumberCount(Species::Species::Oxygen));
  Absorption::Band band(
      O266, Absorption::Mirroring::None, Absorption::Normalization::None,
      Absorption::Population::ByLTE, Absorption::Cutoff::ByLineOffset,
      Absorption::Shape::VP, false, 296, 750e9, g, g, 1);
  const Absorption::LineShape::Model m{Species::Species::Oxygen, 10e3, 15e3,
                                       300, 0.7};
  band.Lines()[0] =
      Absorption::Line(O266, 100e9, 1e-18, 1e-20, {0, 0}, 1, 1, 1e-20, l, l, m);
  auto& line = band.Lines()[0];
  const double GDpart = band.GD_giv_F0(T);
  const double GDpart_dT = band.GD_giv_F0(T + dT);
  const auto X = line.ShapeModel()(T, band.T0(), P, VMRS);
  const auto dX = line.ShapeModel().dT(T, band.T0(), P, VMRS);
  const auto X_dT = line.ShapeModel()(T + dT, band.T0(), P, VMRS);

  auto VP = Absorption::LineShape::Base::Voigt(line.F0(), X, GDpart, 0);
  auto VP_dT =
      Absorption::LineShape::Base::Voigt(line.F0(), X_dT, GDpart_dT, 0);
  auto VP_df = Absorption::LineShape::Base::Voigt(line.F0(), X, GDpart, 0);

  auto freqs = linspace(99.999e9, 100.001e9, 11);
  for (auto f : freqs) {
    VP(f);
    VP_dT(f);
    VP_df(f+df);
    auto dVPdT = VP.dFdT(dX, T);
    auto dVPdf = VP.dFdf();
    std::cout << ' ' << f << ' '
              << std::real(VP.F) << ' ' << std::imag(VP.F) << ' '
              << std::real(dVPdT) << ' ' << std::imag(dVPdT) << ' '
              << std::real(VP_dT.F - VP.F) / dT << ' '
              << std::imag(VP_dT.F - VP.F) / dT << ' '
              << std::real(dVPdf) << ' ' << std::imag(dVPdf) << ' '
              << std::real(VP_df.F - VP.F) / df << ' '
              << std::imag(VP_df.F - VP.F) / df << '\n';
  }
}

int main() {
  std::setprecision(20);
  test001();  // Test Voigt Line Shape and some derivatives
}
