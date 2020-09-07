#include "mathhelpers.h"
#include "xsec.h"

void test001() {
  constexpr double T = 275;
  constexpr double P = 10e3;
  constexpr double mu = 10e-6;
  constexpr double mv = 10e-6;
  constexpr double mw = 30e-6;
  constexpr double wu = 10;
  constexpr double wv = 1;
  constexpr double ww = 0.1;
  constexpr double vmrn2 = 0.78;
  constexpr double vmro2 = 0.2095;
  constexpr double vmrh2o = 400e-06;
  constexpr size_t nfreq = 11;
  constexpr Frequency<FrequencyType::Freq> flow = 90e9;
  constexpr Frequency<FrequencyType::Freq> fupp = 110e9;
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a * a - b * b) / (a * a)));

  const auto xpos = Geom::Pos<Geom::PosType::Xyz>({0, a + 1, 0});
  const auto dxlos = Geom::Los<Geom::LosType::Xyz>({1, 1, 1});
  const auto nav = Geom::Nav(xpos, dxlos, wgs84);

  const Species::Isotope O266(Species::Species::Oxygen, 0);
  const std::vector<Quantum::Number> g(
      getGlobalQuantumNumberCount(Species::Species::Oxygen));
  const std::vector<Quantum::Number> l(
      getLocalQuantumNumberCount(Species::Species::Oxygen));
  Absorption::Band band(
      O266, Absorption::Mirroring::None, Absorption::Normalization::None,
      Absorption::Population::ByLTE, Absorption::Cutoff::ByLineOffset,
      Absorption::Shape::VP, false, 296, 750e9, g, g, 1);
  const Absorption::LineShape::Model m{Species::Species::Oxygen, 10e3, 15e3, 0,
                                       0.7};
  band.Lines()[0] =
      Absorption::Line(O266, 100e9, 1e-18, 1e-20, {0, 0}, 1, 1, 1e-20, l, l, m);

  const Atmosphere::Point ap = Atmosphere::Point(
      P, T, std::array<double, 3>{mu, mv, mw},
      std::array<double, 3>{wu, wv, ww},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              vmrn2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              vmro2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              vmrh2o}});
  const Path::Point p = {nav, ap};

  double fscale = ap.DopplerShiftRatio(p.nav.sphericalLos().za(),
                                       p.nav.sphericalLos().aa());
  const auto fsensor = linspace<Frequency<FrequencyType::Freq>>(
      flow * fscale, fupp * fscale, nfreq);
  const auto f = scale(fsensor, fscale);

  const std::vector<Derivative::Target> derivs{Derivative::Atm::Temperature,
                                               Derivative::Atm::WindU};

  Absorption::PropagationMatrix::Results<1> res(f.size(), derivs.size());
  Absorption::PropagationMatrix::Results<1> src(f.size(), derivs.size());
  Absorption::PropagationMatrix::compute(res, src, f, {band}, p, derivs);

  const Atmosphere::Point ap_T(
      P, T + 0.1, std::array<double, 3>{mu, mv, mw},
      std::array<double, 3>{wu, wv, ww},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              vmrn2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              vmro2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              vmrh2o}});
  Absorption::PropagationMatrix::Results<1> resdT(f.size(), derivs.size());
  Absorption::PropagationMatrix::compute(resdT, src, f, {band}, {nav, ap_T},
                                         derivs);

  const Atmosphere::Point ap_wu(
      P, T, std::array<double, 3>{mu, mv, mw},
      std::array<double, 3>{wu + 0.1, wv, ww},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              vmrn2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              vmro2},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              vmrh2o}});
  double fscale_wu = ap_wu.DopplerShiftRatio(p.nav.sphericalLos().za(),
                                             p.nav.sphericalLos().aa());
  const auto f_wu = scale(fsensor, fscale_wu);
  Absorption::PropagationMatrix::Results<1> resdwu(f.size(), derivs.size());
  Absorption::PropagationMatrix::compute(resdwu, src, f_wu, {band},
                                         {nav, ap_wu}, derivs);

  for (size_t iv = 0; iv < nfreq; iv++) {
    std::cout << res.x[iv];
    for (size_t it = 0; it < derivs.size(); it++) {
      std::cout << ' ' << res.dx[it][iv];
    }
    std::cout << ' ' << (resdT.x[iv][0] - res.x[iv][0]) / 0.1;
    std::cout << ' ' << (resdwu.x[iv][0] - res.x[iv][0]) / 0.1;
    std::cout << '\n';
  }
}

int main() { test001(); }
