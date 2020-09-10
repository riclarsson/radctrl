#include <iomanip>
#include <iostream>

#include "rte.h"

void test001() {
  constexpr RTE::RadVec<4> a4{1, 2, 3, 4};
  constexpr RTE::RadVec<3> a3{1, 2, 3};
  constexpr RTE::RadVec<2> a2{1, 2};
  constexpr RTE::RadVec<1> a1{1};
  std::cout << a4 << '\n' << a3 << '\n' << a2 << '\n' << a1 << '\n';
}

void test002() {
  constexpr RTE::RadVec<4> a4{1, 2, 3, 4};
  constexpr RTE::RadVec<3> a3{1, 2, 3};
  constexpr RTE::RadVec<2> a2{1, 2};
  constexpr RTE::RadVec<1> a1{1};

  constexpr RTE::TraMat<4> a44{1, 2,  3,  4,  5,  6,  7,  8,
                               9, 10, 11, 12, 13, 14, 15, 16};
  constexpr RTE::TraMat<3> a33{1, 2, 3, 4, 5, 6, 7, 8, 9};
  constexpr RTE::TraMat<2> a22{1, 2, 3, 4};
  constexpr RTE::TraMat<1> a11{1};

  std::cout << a44 << '\n' << a33 << '\n' << a22 << '\n' << a11 << '\n';
}

void test003() {
  constexpr Absorption::PropMat<4> pm44{1, 1e-1, 1e-3, -5e-3, 1e-3, -1e-2, 0};
  constexpr Absorption::PropMat<3> pm33{1, 1e-1, 1e-3, -5e-3};
  constexpr Absorption::PropMat<2> pm22{1, 1e-1};
  constexpr Absorption::PropMat<1> pm11{1};

  std::cout << pm44 << '\n' << RTE::TraMat<4>{pm44} << '\n';
  std::cout << pm33 << '\n' << RTE::TraMat<3>{pm33} << '\n';
  std::cout << pm22 << '\n' << RTE::TraMat<2>{pm22} << '\n';
  std::cout << pm11 << '\n' << RTE::TraMat<1>{pm11} << '\n';
}

void test004() {
  constexpr RTE::RadVec<4> a4{1, 2, 3, 4};
  constexpr RTE::RadVec<3> a3{1, 2, 3};
  constexpr RTE::RadVec<2> a2{1, 2};
  constexpr RTE::RadVec<1> a1{1};
  constexpr RTE::RadVec<4> j4{5, 6, 7, 8};
  constexpr RTE::RadVec<3> j3{5, 6, 7};
  constexpr RTE::RadVec<2> j2{5, 6};
  constexpr RTE::RadVec<1> j1{5};

  constexpr RTE::TraMat<4> a44{1, 2,  3,  4,  5,  6,  7,  8,
                               9, 10, 11, 12, 13, 14, 15, 16};
  constexpr RTE::TraMat<3> a33{1, 2, 3, 4, 5, 6, 7, 8, 9};
  constexpr RTE::TraMat<2> a22{1, 2, 3, 4};
  constexpr RTE::TraMat<1> a11{1};

  std::cout << RTE::update(a4, a44, j4) << '\n';
  std::cout << RTE::update(a3, a33, j3) << '\n';
  std::cout << RTE::update(a2, a22, j2) << '\n';
  std::cout << RTE::update(a1, a11, j1) << '\n';

  static_assert(RTE::update(a4, a44, j4)[0] == -35);
  static_assert(RTE::update(a4, a44, j4)[1] == -98);
  static_assert(RTE::update(a4, a44, j4)[2] == -161);
  static_assert(RTE::update(a4, a44, j4)[3] == -224);
  static_assert(RTE::update(a3, a33, j3)[0] == -19);
  static_assert(RTE::update(a3, a33, j3)[1] == -54);
  static_assert(RTE::update(a3, a33, j3)[2] == -89);
  static_assert(RTE::update(a2, a22, j2)[0] == -7);
  static_assert(RTE::update(a2, a22, j2)[1] == -22);
  static_assert(RTE::update(a1, a11, j1)[0] == 1);
}

void test005() {
  constexpr Absorption::PropMat<4> pm44{1, 1e-1, 1e-3, -5e-3, 1e-3, -1e-2, 0};
  constexpr Absorption::PropMat<3> pm33{1, 1e-1, 1e-3, -5e-3};
  constexpr Absorption::PropMat<2> pm22{1, 1e-1};
  constexpr Absorption::PropMat<1> pm11{1};
  constexpr double r = 1e1;

  std::cout << RTE::single_transmat(pm44, r) << '\n';
  std::cout << RTE::single_transmat(pm33, r) << '\n';
  std::cout << RTE::single_transmat(pm22, r) << '\n';
  std::cout << RTE::single_transmat(pm11, r) << '\n';
}

void test006() {
  auto lj0 = [](double T) { return T * 1e-3; };
  auto lj1 = [](double T) { return 2 * std::exp(-2 * (T / 296)); };
  auto lj2 = [](double T) { return std::exp(T / 296); };
  auto lj3 = [](double T) { return 3 * std::sin(T / 296); };
  auto la00 = [](double T) { return T * 1e-3; };
  auto la01 = [](double T) { return T * 2e-3; };
  auto la02 = [](double T) { return T * 3e-3; };
  auto la03 = [](double T) { return T * 4e-3; };
  auto la10 = [](double T) { return T * 5e-3; };
  auto la11 = [](double T) { return T * 6e-3; };
  auto la12 = [](double T) { return T * 7e-3; };
  auto la13 = [](double T) { return T * 8e-3; };
  auto la20 = [](double T) { return T * 9e-3; };
  auto la21 = [](double T) { return T * 10e-3; };
  auto la22 = [](double T) { return T * 11e-3; };
  auto la23 = [](double T) { return T * 12e-3; };
  auto la30 = [](double T) { return T * 13e-3; };
  auto la31 = [](double T) { return T * 14e-3; };
  auto la32 = [](double T) { return T * 15e-3; };
  auto la33 = [](double T) { return T * 16e-3; };
  constexpr double T = 300;
  constexpr double dT = 0.01;

  RTE::RadVec<4> a4{300, 100, 50, -50};
  RTE::RadVec<3> a3{300, 100, 50};
  RTE::RadVec<2> a2{300, 100};
  RTE::RadVec<1> a1{300};
  RTE::RadVec<4> j4{lj0(T), lj1(T), lj2(T), lj3(T)};
  RTE::RadVec<3> j3{lj0(T), lj1(T), lj2(T)};
  RTE::RadVec<2> j2{lj0(T), lj1(T)};
  RTE::RadVec<1> j1{lj0(T)};
  RTE::TraMat<4> a44{la00(T), la01(T), la02(T), la03(T), la10(T), la11(T),
                     la12(T), la13(T), la20(T), la21(T), la22(T), la23(T),
                     la30(T), la31(T), la32(T), la33(T)};
  RTE::TraMat<3> a33{la00(T), la01(T), la02(T), la10(T), la11(T),
                     la12(T), la20(T), la21(T), la22(T)};
  RTE::TraMat<2> a22{la00(T), la01(T), la10(T), la11(T)};
  RTE::TraMat<1> a11{la00(T)};

  RTE::RadVec<4> dj4{lj0(T + dT), lj1(T + dT), lj2(T + dT), lj3(T + dT)};
  RTE::RadVec<3> dj3{lj0(T + dT), lj1(T + dT), lj2(T + dT)};
  RTE::RadVec<2> dj2{lj0(T + dT), lj1(T + dT)};
  RTE::RadVec<1> dj1{lj0(T + dT)};
  RTE::TraMat<4> da44{la00(T + dT), la01(T + dT), la02(T + dT), la03(T + dT),
                      la10(T + dT), la11(T + dT), la12(T + dT), la13(T + dT),
                      la20(T + dT), la21(T + dT), la22(T + dT), la23(T + dT),
                      la30(T + dT), la31(T + dT), la32(T + dT), la33(T + dT)};
  RTE::TraMat<3> da33{la00(T + dT), la01(T + dT), la02(T + dT),
                      la10(T + dT), la11(T + dT), la12(T + dT),
                      la20(T + dT), la21(T + dT), la22(T + dT)};
  RTE::TraMat<2> da22{la00(T + dT), la01(T + dT), la10(T + dT), la11(T + dT)};
  RTE::TraMat<1> da11{la00(T + dT)};

  RTE::RadVec<4> Dj4{(lj0(T + dT) - lj0(T)) / dT, (lj1(T + dT) - lj1(T)) / dT,
                     (lj2(T + dT) - lj2(T)) / dT, (lj3(T + dT) - lj3(T)) / dT};
  RTE::RadVec<3> Dj3{(lj0(T + dT) - lj0(T)) / dT, (lj1(T + dT) - lj1(T)) / dT,
                     (lj2(T + dT) - lj2(T)) / dT};
  RTE::RadVec<2> Dj2{(lj0(T + dT) - lj0(T)) / dT, (lj1(T + dT) - lj1(T)) / dT};
  RTE::RadVec<1> Dj1{(lj0(T + dT) - lj0(T)) / dT};
  RTE::TraMat<4> Da44{
      (la00(T + dT) - la00(T)) / dT, (la01(T + dT) - la01(T)) / dT,
      (la02(T + dT) - la02(T)) / dT, (la03(T + dT) - la03(T)) / dT,
      (la10(T + dT) - la10(T)) / dT, (la11(T + dT) - la11(T)) / dT,
      (la12(T + dT) - la12(T)) / dT, (la13(T + dT) - la13(T)) / dT,
      (la20(T + dT) - la20(T)) / dT, (la21(T + dT) - la21(T)) / dT,
      (la22(T + dT) - la22(T)) / dT, (la23(T + dT) - la23(T)) / dT,
      (la30(T + dT) - la30(T)) / dT, (la31(T + dT) - la31(T)) / dT,
      (la32(T + dT) - la32(T)) / dT, (la33(T + dT) - la33(T)) / dT};
  RTE::TraMat<3> Da33{
      (la00(T + dT) - la00(T)) / dT, (la01(T + dT) - la01(T)) / dT,
      (la02(T + dT) - la02(T)) / dT, (la10(T + dT) - la10(T)) / dT,
      (la11(T + dT) - la11(T)) / dT, (la12(T + dT) - la12(T)) / dT,
      (la20(T + dT) - la20(T)) / dT, (la21(T + dT) - la21(T)) / dT,
      (la22(T + dT) - la22(T)) / dT};
  RTE::TraMat<2> Da22{
      (la00(T + dT) - la00(T)) / dT, (la01(T + dT) - la01(T)) / dT,
      (la10(T + dT) - la10(T)) / dT, (la11(T + dT) - la11(T)) / dT};
  RTE::TraMat<1> Da11{(la00(T + dT) - la00(T)) / dT};

  std::cout << '\n' << '\n' << '\n' << '\n';
  std::cout << a4 << '\n';
  std::cout << a3 << '\n';
  std::cout << a2 << '\n';
  std::cout << a1 << '\n';
  std::cout << j4 << '\n';
  std::cout << j3 << '\n';
  std::cout << j2 << '\n';
  std::cout << j1 << '\n';
  std::cout << a44 << '\n';
  std::cout << a33 << '\n';
  std::cout << a22 << '\n';
  std::cout << a11 << '\n' << '\n';

  std::cout << RTE::update(a4, a44, j4) << '\n'
            << RTE::update(a4, da44, dj4) << '\n'
            << "Manual derivat: ";
  for (int i = 0; i < 4; i++)
    std::cout << (RTE::update(a4, da44, dj4)[i] - RTE::update(a4, a44, j4)[i]) /
                     dT
              << ' ';
  std::cout << '\n'
            << "Computed deriv: " << RTE::dupdate(a4, a44, Da44, j4, Dj4)
            << '\n'
            << '\n';
  std::cout << RTE::update(a3, a33, j3) << '\n'
            << RTE::update(a3, da33, dj3) << '\n'
            << "Manual derivat: ";
  for (int i = 0; i < 3; i++)
    std::cout << (RTE::update(a3, da33, dj3)[i] - RTE::update(a3, a33, j3)[i]) /
                     dT
              << ' ';
  std::cout << '\n'
            << "Computed deriv: " << RTE::dupdate(a3, a33, Da33, j3, Dj3)
            << '\n'
            << '\n';
  std::cout << RTE::update(a2, a22, j2) << '\n'
            << RTE::update(a2, da22, dj2) << '\n'
            << "Manual derivat: ";
  for (int i = 0; i < 2; i++)
    std::cout << (RTE::update(a2, da22, dj2)[i] - RTE::update(a2, a22, j2)[i]) /
                     dT
              << ' ';
  std::cout << '\n'
            << "Computed deriv: " << RTE::dupdate(a2, a22, Da22, j2, Dj2)
            << '\n'
            << '\n';
  std::cout << RTE::update(a1, a11, j1) << '\n'
            << RTE::update(a1, da11, dj1) << '\n'
            << "Manual derivat: ";
  for (int i = 0; i < 1; i++)
    std::cout << (RTE::update(a1, da11, dj1)[i] - RTE::update(a1, a11, j1)[i]) /
                     dT
              << ' ';
  std::cout << '\n'
            << "Computed deriv: " << RTE::dupdate(a1, a11, Da11, j1, Dj1)
            << '\n'
            << '\n';
}

void test007() {
  auto a = [](double T) { return 1 + T * 1e-3; };
  auto b = [](double T) { return 1e-2 + T * 1e-3; };
  auto c = [](double T) { return T * 1e-3 - 1e-2; };
  auto d = [](double T) { return 1e-1 + 2 * T * 1e-3; };
  auto u = [](double T) { return 2 * T * 1e-3; };
  auto v = [](double T) { return 3 * T * 1e-3; };
  auto w = [](double T) { return 4 * T * 1e-3; };
  constexpr double T = 300;

  Absorption::PropMat<4> K44(a(T), b(T), c(T), d(T), u(T), v(T), w(T));
  Absorption::PropMat<3> K33(a(T), b(T), c(T), u(T));
  Absorption::PropMat<2> K22(a(T), b(T));
  Absorption::PropMat<1> K11(a(T));

  auto T44 = RTE::linear_transmat(K44, K44, 1.0);
  auto T33 = RTE::linear_transmat(K33, K33, 1.0);
  auto T22 = RTE::linear_transmat(K22, K22, 1.0);
  auto T11 = RTE::linear_transmat(K11, K11, 1.0);

  std::cout << "PropMat<4>:\n"
            << K44.Matrix() << '\n'
            << "Becomes TraMat<4> (after 1 meter):\n"
            << T44 << '\n';
  std::cout << "PropMat<3>:\n"
            << K33.Matrix() << '\n'
            << "Becomes TraMat<3> (after 1 meter):\n"
            << T33 << '\n';
  std::cout << "PropMat<2>:\n"
            << K22.Matrix() << '\n'
            << "Becomes TraMat<2> (after 1 meter):\n"
            << T22 << '\n';
  std::cout << "PropMat<1>:\n"
            << K11.Matrix() << '\n'
            << "Becomes TraMat<1> (after 1 meter):\n"
            << T11 << '\n';
}

void test008() {
  auto a = [](double T) { return 5 + T * 1e-3; };
  auto b = [](double T) { return 3e-2 + T * 1e-3; };
  auto c = [](double T) { return T * 1e-3 - 1e-2; };
  auto d = [](double T) { return 1e0 + 2 * T * 1e-3; };
  auto u = [](double T) { return 2 * T * 1e-3; };
  auto v = [](double T) { return 3 * T * 1e-3; };
  auto w = [](double T) { return 4 * T * 1e-3; };
  constexpr double T = 300;
  constexpr double dT = 0.01;

  Absorption::PropMat<4> K44(a(T), b(T), c(T), d(T), u(T), v(T), w(T));
  Absorption::PropMat<3> K33(a(T), b(T), c(T), u(T));
  Absorption::PropMat<2> K22(a(T), b(T));
  Absorption::PropMat<1> K11(a(T));

  Absorption::PropMat<4> ModK44(a(T + dT), b(T + dT), c(T + dT), d(T + dT),
                                u(T + dT), v(T + dT), w(T + dT));
  Absorption::PropMat<3> ModK33(a(T + dT), b(T + dT), c(T + dT), u(T + dT));
  Absorption::PropMat<2> ModK22(a(T + dT), b(T + dT));
  Absorption::PropMat<1> ModK11(a(T + dT));

  Absorption::PropMat<4> dK44((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT,
                              (c(T + dT) - c(T)) / dT, (d(T + dT) - d(T)) / dT,
                              (u(T + dT) - u(T)) / dT, (v(T + dT) - v(T)) / dT,
                              (w(T + dT) - w(T)) / dT);
  Absorption::PropMat<3> dK33((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT,
                              (c(T + dT) - c(T)) / dT, (u(T + dT) - u(T)) / dT);
  Absorption::PropMat<2> dK22((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT);
  Absorption::PropMat<1> dK11((a(T + dT) - a(T)) / dT);

  auto T44 = RTE::linear_transmat(K44, ModK44, 1.0);
  auto T33 = RTE::linear_transmat(K33, ModK33, 1.0);
  auto T22 = RTE::linear_transmat(K22, ModK22, 1.0);
  auto T11 = RTE::linear_transmat(K11, ModK11, 1.0);

  auto ModT44 = RTE::linear_transmat(ModK44, ModK44, 1.0);
  auto ModT33 = RTE::linear_transmat(ModK33, ModK33, 1.0);
  auto ModT22 = RTE::linear_transmat(ModK22, ModK22, 1.0);
  auto ModT11 = RTE::linear_transmat(ModK11, ModK11, 1.0);

  auto dT44 = RTE::dlinear_transmat(T44, K44, ModK44, dK44, 1.0, 0.0);
  auto dT33 = RTE::dlinear_transmat(T33, K33, ModK33, dK33, 1.0, 0.0);
  auto dT22 = RTE::dlinear_transmat(T22, K22, ModK22, dK22, 1.0, 0.0);
  auto dT11 = RTE::dlinear_transmat(T11, K11, K11, dK11, 1.0, 0.0);
  std::cout << "\n\n\n";

  std::cout << "PropMat<1>:\n"
            << K11.Matrix() << '\n'
            << "And PropMat<1>:\n"
            << ModK11.Matrix() << '\n'
            << "Becomes TraMat<1> (after 1 meter):\n"
            << T11 << '\n';
  std::cout << "Computed derivative <1>:\n" << dT11 << '\n';
  std::cout << "Estimated derivative <1>:\n" << (ModT11 - T11) / dT << '\n';

  std::cout << "\n";
  std::cout << "PropMat<2>:\n"
            << K22.Matrix() << '\n'
            << "And PropMat<2>:\n"
            << ModK22.Matrix() << '\n'
            << "Becomes TraMat<2> (after 1 meter):\n"
            << T22 << '\n';
  std::cout << "Computed derivative <2>:\n" << dT22 << '\n';
  std::cout << "Estimated derivative <2>:\n" << (ModT22 - T22) / dT << '\n';

  std::cout << "\n";
  std::cout << "PropMat<3>:\n"
            << K33.Matrix() << '\n'
            << "And PropMat<3>:\n"
            << ModK33.Matrix() << '\n'
            << "Becomes TraMat<3> (after 1 meter):\n"
            << T33 << '\n';
  std::cout << "Computed derivative <3>:\n" << dT33 << '\n';
  std::cout << "Estimated derivative <3>:\n" << (ModT33 - T33) / dT << '\n';

  std::cout << "\n";
  std::cout << "PropMat<4>:\n"
            << K44.Matrix() << '\n'
            << "And PropMat<4>:\n"
            << ModK44.Matrix() << '\n'
            << "Becomes TraMat<4> (after 1 meter):\n"
            << T44 << '\n';
  std::cout << "Computed derivative <4>:\n" << dT44 << '\n';
  std::cout << "Estimated derivative <4>:\n" << (ModT44 - T44) / dT << '\n';
}

void test009() {
  auto a = [](double T) { return 5 + T * 1e-3; };
  auto b = [](double T) { return 3e-2 + T * 1e-3; };
  auto c = [](double T) { return T * 1e-3 - 1e-2; };
  auto d = [](double T) { return 1e0 + 2 * T * 1e-3; };
  auto u = [](double T) { return 2 * T * 1e-3; };
  auto v = [](double T) { return 3 * T * 1e-3; };
  auto w = [](double T) { return 4 * T * 1e-3; };
  constexpr double T = 300;
  //   constexpr double dT = 0.01;

  Absorption::PropMat<4> K44(a(T), b(T), c(T), d(T), u(T), v(T), w(T));
  Absorption::PropMat<3> K33(a(T), b(T), c(T), u(T));
  Absorption::PropMat<2> K22(a(T), b(T));
  Absorption::PropMat<1> K11(a(T));

  const double B = RTE::B(T, 100e9);
  auto J4 = RTE::source(K44, {0, 0, 0, 0}, {0, 0, 0, 0}, B);
  auto J3 = RTE::source(K33, {0, 0, 0}, {0, 0, 0}, B);
  auto J2 = RTE::source(K22, {0, 0}, {0, 0}, B);
  auto J1 = RTE::source(K11, {0}, {0}, B);
  std::cout << '\n' << '\n';
  std::cout << "Planck:\n" << B << '\n';
  std::cout << "Source Vector <4>:\n" << J4 << '\n';
  std::cout << "Source Vector <3>:\n" << J3 << '\n';
  std::cout << "Source Vector <2>:\n" << J2 << '\n';
  std::cout << "Source Vector <1>:\n" << J1 << '\n';
}

void test010() {
  auto a = [](double T) { return 5 + T * 1e-3; };
  auto b = [](double T) { return 3e-2 + T * 1e-3; };
  auto c = [](double T) { return T * 1e-3 - 1e-2; };
  auto d = [](double T) { return 1e0 + 2 * T * 1e-3; };
  auto u = [](double T) { return 2 * T * 1e-3; };
  auto v = [](double T) { return 3 * T * 1e-3; };
  auto w = [](double T) { return 4 * T * 1e-3; };
  constexpr double T = 300;
  constexpr double dT = 0.01;

  Absorption::PropMat<4> K44(a(T), b(T), c(T), d(T), u(T), v(T), w(T));
  Absorption::PropMat<3> K33(a(T), b(T), c(T), u(T));
  Absorption::PropMat<2> K22(a(T), b(T));
  Absorption::PropMat<1> K11(a(T));

  Absorption::PropMat<4> ModK44(a(T + dT), b(T + dT), c(T + dT), d(T + dT),
                                u(T + dT), v(T + dT), w(T + dT));
  Absorption::PropMat<3> ModK33(a(T + dT), b(T + dT), c(T + dT), u(T + dT));
  Absorption::PropMat<2> ModK22(a(T + dT), b(T + dT));
  Absorption::PropMat<1> ModK11(a(T + dT));

  Absorption::PropMat<4> dK44((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT,
                              (c(T + dT) - c(T)) / dT, (d(T + dT) - d(T)) / dT,
                              (u(T + dT) - u(T)) / dT, (v(T + dT) - v(T)) / dT,
                              (w(T + dT) - w(T)) / dT);
  Absorption::PropMat<3> dK33((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT,
                              (c(T + dT) - c(T)) / dT, (u(T + dT) - u(T)) / dT);
  Absorption::PropMat<2> dK22((a(T + dT) - a(T)) / dT, (b(T + dT) - b(T)) / dT);
  Absorption::PropMat<1> dK11((a(T + dT) - a(T)) / dT);

  const double B = RTE::B(T, 100e9);
  const double ModB = RTE::B(T + dT, 100e9);
  const double dBdT = RTE::dBdT(T, 100e9);
  auto J4 = RTE::source(K44, {0, 0, 0, 0}, {0, 0, 0, 0}, B);
  auto J3 = RTE::source(K33, {0, 0, 0}, {0, 0, 0}, B);
  auto J2 = RTE::source(K22, {0, 0}, {0, 0}, B);
  auto J1 = RTE::source(K11, {0}, {0}, B);
  std::cout << '\n' << '\n';
  std::cout << "Planck:\n" << B << '\n';
  std::cout << "Source Vector <4>:\n" << J4 << '\n';
  std::cout << "Source Vector <3>:\n" << J3 << '\n';
  std::cout << "Source Vector <2>:\n" << J2 << '\n';
  std::cout << "Source Vector <1>:\n" << J1 << '\n';

  auto ModJ4 = RTE::source(ModK44, {0, 0, 0, 0}, {0, 0, 0, 0}, ModB);
  auto ModJ3 = RTE::source(ModK33, {0, 0, 0}, {0, 0, 0}, ModB);
  auto ModJ2 = RTE::source(ModK22, {0, 0}, {0, 0}, ModB);
  auto ModJ1 = RTE::source(ModK11, {0}, {0}, ModB);
  std::cout << '\n' << '\n';
  std::cout << "Mod Planck:\n" << ModB << '\n';
  std::cout << "Mod Source Vector <4>:\n" << ModJ4 << '\n';
  std::cout << "Mod Source Vector <3>:\n" << ModJ3 << '\n';
  std::cout << "Mod Source Vector <2>:\n" << ModJ2 << '\n';
  std::cout << "Mod Source Vector <1>:\n" << ModJ1 << '\n';

  auto dJ4 = RTE::dsource(K44, dK44, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
                          {0, 0, 0, 0}, B, dBdT);
  auto dJ3 = RTE::dsource(K33, dK33, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
                          B, dBdT);
  auto dJ2 = RTE::dsource(K22, dK22, {0, 0}, {0, 0}, {0, 0}, {0, 0}, B, dBdT);
  auto dJ1 = RTE::dsource(K11, dK11, {0}, {0}, {0}, {0}, B, dBdT);
  std::cout << '\n' << '\n';
  std::cout << "Derivative Planck:\n" << dBdT << '\n';
  std::cout << "Derivative Source Vector <4>:\n" << dJ4 << '\n';
  std::cout << "Derivative Source Vector <3>:\n" << dJ3 << '\n';
  std::cout << "Derivative Source Vector <2>:\n" << dJ2 << '\n';
  std::cout << "Derivative Source Vector <1>:\n" << dJ1 << '\n';

  std::cout << '\n' << '\n';
  std::cout << "Manual Derivative Planck:\n" << (ModB - B) / dT << '\n';
  std::cout << "Manual Derivative Source Vector <4>:\n"
            << (ModJ4 - J4) / dT << '\n';
  std::cout << "Manual Derivative Source Vector <3>:\n"
            << (ModJ3 - J3) / dT << '\n';
  std::cout << "Manual Derivative Source Vector <2>:\n"
            << (ModJ2 - J2) / dT << '\n';
  std::cout << "Manual Derivative Source Vector <1>:\n"
            << (ModJ1 - J1) / dT << '\n';
}

int main() {
  std::cout << std::setprecision(20);
  test001();  // RadVec creation, stream output
  test002();  // TraMat creation, stream output
  test003();  // PropMat creation, stream output
  test004();  // RTE equation, stream output
  test005();  // PropMat to TraMat, stream output
  test006();  // RTE equation, RTE derivative
  test007();  // Transmission matrix
  test008();  // Transmission matrix derivative
  test009();  // Source vector (LTE, no scattering)
  test010();  // Source vector temperature derivative (LTE, no scattering)
}
