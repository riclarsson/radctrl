#include "mathhelpers.h"
#include "xsec_lbl.h"

void test001() {
  Species::Isotope H2O161(Species::Species::Water, 0);
  Species::Isotope O266(Species::Species::Oxygen, 0);

  for (double T = 70; T < 600; T += 0.1) {
    std::cout << T << ' ' << H2O161.QT(T) << ' ' << O266.QT(T) << '\n';
  }
}

void test002() {
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a * a - b * b) / (a * a)));

  auto x = Geom::Pos<Geom::PosType::Xyz>({0, a + 1, 0});
  auto dx = Geom::Los<Geom::LosType::Xyz>({0, 1, 0});
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
  Atmosphere::Atm atm({Time()}, A, {0}, {0}, {3, {1, N, 1, 1}});
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
  auto nav = Path::calc_single_geometric_path(n, atm, 1e3, 90e3);

  const Species::Isotope O266(Species::Species::Oxygen, 0);
  const std::vector<Quantum::Number> g(
      getGlobalQuantumNumberCount(Species::Species::Oxygen));
  const std::vector<Quantum::Number> l(
      getLocalQuantumNumberCount(Species::Species::Oxygen));
  Absorption::Band band(O266, Absorption::Mirroring::None,
                        Absorption::Normalization::None,
                        Absorption::Population::ByLTE, Absorption::Cutoff::ByLineOffset,
                        Absorption::Shape::VP, 296, 1e9, g, g, 1);
  Absorption::LineShape::Model m{Species::Species::Oxygen, 10e3, 15e3, 0, 0.7};
  band.Lines()[0] =
      Absorption::Line(O266, 100e9, 1e-20, 1e-20, {0, 0}, 1, 1, 1e-20, l, l, m);

  constexpr size_t M = 100;
  std::vector<std::vector<Complex>> sumx(
      N, std::vector<Complex>(M, Complex{0, 0}));
  std::vector<Complex> partx(M, Complex{0, 0});
  auto f = linspace(90e9, 110e9, M);

  for (size_t i = 0; i < N; i++) {
    Absorption::Xsec::Lbl::compute(sumx[i], partx, f, band, nav[i]);
  }

  for (size_t j = 0; j < M; j++) {
    std::cout << f[j];
    for (size_t i = 0; i < N; i++) {
      std::cout << ' ' << sumx[i][j].real() << ' ' << sumx[i][j].imag();
    }
    std::cout << '\n';
  }
}

int main() {
  //   test001();  // Test a few partition functions
  test002();  // Test some LBL
}
