#include "forward.h"
#include "testhelpers.h"

void test001() {
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a * a - b * b) / (a * a)));

  auto x = Geom::Pos<Geom::PosType::Xyz>({0, a + 90001, 0});
  auto dx = Geom::Los<Geom::LosType::Xyz>({1, -1, 1});
  auto n = Geom::Nav(x, dx, wgs84);

  constexpr size_t N = 46;
  std::vector<Altitude<AltitudeType::meter>> A = {
      -7.282161e+02, 0.000000e+00, 1.000000e+03, 2.000000e+03, 3.000000e+03,
      4.000000e+03,  5.000000e+03, 6.000000e+03, 7.000000e+03, 8.000000e+03,
      9.000000e+03,  1.000000e+04, 1.100000e+04, 1.200000e+04, 1.300000e+04,
      1.400000e+04,  1.500000e+04, 1.600000e+04, 1.700000e+04, 1.800000e+04,
      1.900000e+04,  2.000000e+04, 2.100000e+04, 2.200000e+04, 2.300000e+04,
      2.400000e+04,  2.500000e+04, 2.750000e+04, 3.000000e+04, 3.250000e+04,
      3.500000e+04,  3.750000e+04, 4.000000e+04, 4.250000e+04, 4.500000e+04,
      4.750000e+04,  5.000000e+04, 5.500000e+04, 6.000000e+04, 6.500000e+04,
      7.000000e+04,  7.500000e+04, 8.000000e+04, 8.500000e+04, 9.000000e+04,
      9.500000e+04};
  std::vector<double> P = {
      110000, 101300, 90400, 80500, 71500, 63300, 55900, 49200, 43200, 37800,
      32900,  28600,  24700, 21300, 18200, 15600, 13200, 11100, 9370,  7890,
      6660,   5650,   4800,  4090,  3500,  3000,  2570,  1763,  1220,  852,
      600,    426,    305,   220,   159,   116,   85.4,  45.6,  23.9,  12.1,
      5.8,    2.6,    1.1,   0.44,  0.172, 0.069};
  std::vector<double> T = {
      2.997000e+02, 2.997000e+02, 2.937000e+02, 2.877000e+02, 2.837000e+02,
      2.770000e+02, 2.703000e+02, 2.636000e+02, 2.570000e+02, 2.503000e+02,
      2.436000e+02, 2.370000e+02, 2.301000e+02, 2.236000e+02, 2.170000e+02,
      2.103000e+02, 2.037000e+02, 1.970000e+02, 1.948000e+02, 1.988000e+02,
      2.027000e+02, 2.067000e+02, 2.107000e+02, 2.146000e+02, 2.170000e+02,
      2.192000e+02, 2.214000e+02, 2.270000e+02, 2.323000e+02, 2.377000e+02,
      2.431000e+02, 2.485000e+02, 2.540000e+02, 2.594000e+02, 2.648000e+02,
      2.696000e+02, 2.702000e+02, 2.634000e+02, 2.531000e+02, 2.360000e+02,
      2.189000e+02, 2.018000e+02, 1.848000e+02, 1.771000e+02, 1.770000e+02,
      1.843000e+02};

  Atmosphere::Point ap = Atmosphere::Point(
      P[0], T[0], std::array<double, 3>{10e-6, 10e-6, 30e-6},
      std::array<double, 3>{10., 1., 0.1},
      std::vector<VMR<VMRType::ratio>>{
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                              0.78},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                              0.2095},
          VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                              400e-06}});
  Atmosphere::Atm atm({Time()}, A, {0}, {0}, {3, 1, N, 1, 1});
  for (size_t i = 0; i < N; i++)
    atm(0, i, 0, 0) = Atmosphere::Point(
        P[i], T[i], std::array<double, 3>{10e-6, 10e-6, 30e-6},
        std::array<double, 3>{10., 1., 0.1},
        std::vector<VMR<VMRType::ratio>>{
            VMR<VMRType::ratio>{Species::Isotope(Species::Species::Nitrogen, 0),
                                0.78},
            VMR<VMRType::ratio>{Species::Isotope(Species::Species::Oxygen, 0),
                                0.2095},
            VMR<VMRType::ratio>{Species::Isotope(Species::Species::Water, 0),
                                400e-06 - 5e-6 * i}});
  const auto path = Path::calc_single_geometric_path(n, atm, 1e3, 90e3);

  const Species::Isotope O266(Species::Species::Oxygen, 0);
  const std::vector<Quantum::Number> g(
      getGlobalQuantumNumberCount(Species::Species::Oxygen));
  const std::vector<Quantum::Number> l(
      getLocalQuantumNumberCount(Species::Species::Oxygen));
  Absorption::Band band(
      O266, Absorption::Mirroring::None, Absorption::Normalization::None,
      Absorption::Population::ByLTE, Absorption::Cutoff::ByLineOffset,
      Absorption::Shape::VP, false, 296, 750e9, g, g, 1);
  Absorption::LineShape::Model m{Species::Species::Oxygen, 10e3, 15e3, 0, 0.7};
  band.Lines()[0] =
      Absorption::Line(O266, 100e9, 1e-16, 1e-20, {0, 0}, 1, 1, 1e-20, l, l, m);

  constexpr size_t nfreq = 11;
  constexpr Frequency<FrequencyType::Freq> flow = 90e9;
  constexpr Frequency<FrequencyType::Freq> fupp = 110e9;
  auto f = linspace(flow, fupp, nfreq);

  const RTE::Forward::Calculations calcs{
      f, {Derivative::Atm::Temperature}, {band}, path};
  auto rad0 = RTE::source_vec_planck<1>(299.7, calcs.f_grid);
  auto out = RTE::Forward::compute(rad0, calcs);
  out.convert_to_planck(f);

  Grid<double, 2> dT(0, path.size(), nfreq);
  for (size_t ip = 0; ip < path.size(); ip++) {
    auto path_dT(path);
    path_dT[ip].atm.Temp(path_dT[ip].atm.Temp() + 0.1);

    const RTE::Forward::Calculations calcs_dT{
        f, {Derivative::Atm::Temperature}, {band}, path_dT};
    auto rad0_dT = RTE::source_vec_planck<1>(299.7, calcs_dT.f_grid);
    auto out_dT = RTE::Forward::compute(rad0_dT, calcs_dT);
    out_dT.convert_to_planck(f);
    for (size_t iv = 0; iv < nfreq; iv++)
      dT(ip, iv) = (out_dT.x(0, iv)[0] - out.x(0, iv)[0]) / 0.1;
  }

  for (size_t ip = 0; ip < path.size(); ip++) {
    for (size_t iv = 0; iv < nfreq; iv++) {
      std::cout << dT(ip, iv);
      for (size_t it = 0; it < calcs.targets.size(); it++) {
        std::cout << ' ' << out.dx(it, ip, iv)[0];
      }
      std::cout << '\n';
    }
  }
}

int main() { test001(); }
