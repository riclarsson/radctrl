#include <iomanip>
#include <iostream>

#include "rte.h"

void test001() {
  constexpr RTE::RadVec<4> a4{1,2,3,4};
  constexpr RTE::RadVec<3> a3{1,2,3};
  constexpr RTE::RadVec<2> a2{1,2};
  constexpr RTE::RadVec<1> a1{1};
  std::cout << a4 << '\n'<< a3 << '\n'<< a2 << '\n'<< a1 << '\n';
}

void test002() {
  constexpr RTE::RadVec<4> a4{1,2,3,4};
  constexpr RTE::RadVec<3> a3{1,2,3};
  constexpr RTE::RadVec<2> a2{1,2};
  constexpr RTE::RadVec<1> a1{1};
  
  constexpr RTE::TraMat<4> a44{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  constexpr RTE::TraMat<3> a33{1,2,3,4,5,6,7,8,9};
  constexpr RTE::TraMat<2> a22{1,2,3,4};
  constexpr RTE::TraMat<1> a11{1};
  
  std::cout << a44 << '\n'<< a33 << '\n'<< a22 << '\n'<< a11 << '\n';
}

void test003() {
  constexpr Absorption::PropMat<4> pm44{1, 1e-1, 1e-3, - 5e-3, 1e-3, - 1e-2, 0};
  constexpr Absorption::PropMat<3> pm33{1, 1e-1, 1e-3, - 5e-3};
  constexpr Absorption::PropMat<2> pm22{1, 1e-1};
  constexpr Absorption::PropMat<1> pm11{1};
  
  std::cout << pm44 << '\n' << RTE::TraMat<4>{pm44} << '\n';
  std::cout << pm33 << '\n' << RTE::TraMat<3>{pm33} << '\n';
  std::cout << pm22 << '\n' << RTE::TraMat<2>{pm22} << '\n';
  std::cout << pm11 << '\n' << RTE::TraMat<1>{pm11} << '\n';
}

void test004() {
  constexpr RTE::RadVec<4> a4{1,2,3,4};
  constexpr RTE::RadVec<3> a3{1,2,3};
  constexpr RTE::RadVec<2> a2{1,2};
  constexpr RTE::RadVec<1> a1{1};
  constexpr RTE::RadVec<4> j4{5,6,7,8};
  constexpr RTE::RadVec<3> j3{5,6,7};
  constexpr RTE::RadVec<2> j2{5,6};
  constexpr RTE::RadVec<1> j1{5};
  
  constexpr RTE::TraMat<4> a44{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  constexpr RTE::TraMat<3> a33{1,2,3,4,5,6,7,8,9};
  constexpr RTE::TraMat<2> a22{1,2,3,4};
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
  constexpr Absorption::PropMat<4> pm44{1, 1e-1, 1e-3, - 5e-3, 1e-3, - 1e-2, 0};
  constexpr Absorption::PropMat<3> pm33{1, 1e-1, 1e-3, - 5e-3};
  constexpr Absorption::PropMat<2> pm22{1, 1e-1};
  constexpr Absorption::PropMat<1> pm11{1};
  constexpr double r = 1e1;
  
  std::cout << RTE::transmat(pm44, r) << '\n';
  std::cout << RTE::transmat(pm33, r) << '\n';
  std::cout << RTE::transmat(pm22, r) << '\n';
  std::cout << RTE::transmat(pm11, r) << '\n';
}

void test006() {
  auto lj0 = [](double T){return T*1e-3;};
  auto lj1 = [](double T){return 2*std::exp(-2*(T/296));};
  auto lj2 = [](double T){return std::exp(T/296);};
  auto lj3 = [](double T){return 3*std::sin(T/296);};
  auto la00 = [](double T){return T*1e-3;};
  auto la01 = [](double T){return T*2e-3;};
  auto la02 = [](double T){return T*3e-3;};
  auto la03 = [](double T){return T*4e-3;};
  auto la10 = [](double T){return T*5e-3;};
  auto la11 = [](double T){return T*6e-3;};
  auto la12 = [](double T){return T*7e-3;};
  auto la13 = [](double T){return T*8e-3;};
  auto la20 = [](double T){return T*9e-3;};
  auto la21 = [](double T){return T*10e-3;};
  auto la22 = [](double T){return T*11e-3;};
  auto la23 = [](double T){return T*12e-3;};
  auto la30 = [](double T){return T*13e-3;};
  auto la31 = [](double T){return T*14e-3;};
  auto la32 = [](double T){return T*15e-3;};
  auto la33 = [](double T){return T*16e-3;};
  constexpr double T = 300;
  constexpr double dT = 0.01;
  
  RTE::RadVec<4> a4{300,100,50,-50};
  RTE::RadVec<3> a3{300,100,50};
  RTE::RadVec<2> a2{300,100};
  RTE::RadVec<1> a1{300};
  RTE::RadVec<4> j4{lj0(T),lj1(T),lj2(T),lj3(T)};
  RTE::RadVec<3> j3{lj0(T),lj1(T),lj2(T)};
  RTE::RadVec<2> j2{lj0(T),lj1(T)};
  RTE::RadVec<1> j1{lj0(T)};
  RTE::TraMat<4> a44{la00(T),la01(T),la02(T),la03(T),la10(T),la11(T),la12(T),la13(T),la20(T),la21(T),la22(T),la23(T),la30(T),la31(T),la32(T),la33(T)};
  RTE::TraMat<3> a33{la00(T),la01(T),la02(T),la10(T),la11(T),la12(T),la20(T),la21(T),la22(T)};
  RTE::TraMat<2> a22{la00(T),la01(T),la10(T),la11(T)};
  RTE::TraMat<1> a11{la00(T)};
  
  RTE::RadVec<4> dj4{lj0(T+dT),lj1(T+dT),lj2(T+dT),lj3(T+dT)};
  RTE::RadVec<3> dj3{lj0(T+dT),lj1(T+dT),lj2(T+dT)};
  RTE::RadVec<2> dj2{lj0(T+dT),lj1(T+dT)};
  RTE::RadVec<1> dj1{lj0(T+dT)};
  RTE::TraMat<4> da44{la00(T+dT),la01(T+dT),la02(T+dT),la03(T+dT),la10(T+dT),la11(T+dT),la12(T+dT),la13(T+dT),la20(T+dT),la21(T+dT),la22(T+dT),la23(T+dT),la30(T+dT),la31(T+dT),la32(T+dT),la33(T+dT)};
  RTE::TraMat<3> da33{la00(T+dT),la01(T+dT),la02(T+dT),la10(T+dT),la11(T+dT),la12(T+dT),la20(T+dT),la21(T+dT),la22(T+dT)};
  RTE::TraMat<2> da22{la00(T+dT),la01(T+dT),la10(T+dT),la11(T+dT)};
  RTE::TraMat<1> da11{la00(T+dT)};
  
  RTE::RadVec<4> Dj4{(lj0(T+dT)-lj0(T))/dT,(lj1(T+dT)-lj1(T))/dT,(lj2(T+dT)-lj2(T))/dT,(lj3(T+dT)-lj3(T))/dT};
  RTE::RadVec<3> Dj3{(lj0(T+dT)-lj0(T))/dT,(lj1(T+dT)-lj1(T))/dT,(lj2(T+dT)-lj2(T))/dT};
  RTE::RadVec<2> Dj2{(lj0(T+dT)-lj0(T))/dT,(lj1(T+dT)-lj1(T))/dT};
  RTE::RadVec<1> Dj1{(lj0(T+dT)-lj0(T))/dT};
  RTE::TraMat<4> Da44{(la00(T+dT) - la00(T))/dT,(la01(T+dT) - la01(T))/dT,(la02(T+dT) - la02(T))/dT,(la03(T+dT) - la03(T))/dT,(la10(T+dT) - la10(T))/dT,(la11(T+dT) - la11(T))/dT,(la12(T+dT) - la12(T))/dT, (la13(T+dT) - la13(T))/dT,(la20(T+dT) - la20(T))/dT,(la21(T+dT) - la21(T))/dT,(la22(T+dT) - la22(T))/dT,(la23(T+dT) - la23(T))/dT,(la30(T+dT) - la30(T))/dT,(la31(T+dT) - la31(T))/dT,(la32(T+dT) - la32(T))/dT,(la33(T+dT) - la33(T))/dT};
  RTE::TraMat<3> Da33{(la00(T+dT) - la00(T))/dT,(la01(T+dT) - la01(T))/dT,(la02(T+dT) - la02(T))/dT, (la10(T+dT) - la10(T))/dT,(la11(T+dT) - la11(T))/dT,(la12(T+dT) - la12(T))/dT,(la20(T+dT) - la20(T))/dT,(la21(T+dT) - la21(T))/dT,(la22(T+dT) - la22(T))/dT};
  RTE::TraMat<2> Da22{(la00(T+dT) - la00(T))/dT,(la01(T+dT) - la01(T))/dT,(la10(T+dT) - la10(T))/dT,(la11(T+dT) - la11(T))/dT};
  RTE::TraMat<1> Da11{(la00(T+dT) - la00(T))/dT};
  
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
  
  std::cout << RTE::update(a4, a44, j4) << '\n' << RTE::update(a4, da44, dj4) << '\n' << "Manual derivat: ";
  for (int i=0; i<4; i++) std::cout << (RTE::update(a4, da44, dj4)[i] - RTE::update(a4, a44, j4)[i])/dT<<' ';
  std::cout<< '\n' << "Computed deriv: " << RTE::dupdate(a4, a44, Da44, j4, Dj4) << '\n' << '\n';
  std::cout << RTE::update(a3, a33, j3) << '\n' << RTE::update(a3, da33, dj3) << '\n' << "Manual derivat: ";
  for (int i=0; i<3; i++) std::cout << (RTE::update(a3, da33, dj3)[i] - RTE::update(a3, a33, j3)[i])/dT<<' ';
  std::cout<< '\n' << "Computed deriv: " << RTE::dupdate(a3, a33, Da33, j3, Dj3) << '\n' << '\n';
  std::cout << RTE::update(a2, a22, j2) << '\n' << RTE::update(a2, da22, dj2) << '\n' << "Manual derivat: ";
  for (int i=0; i<2; i++) std::cout << (RTE::update(a2, da22, dj2)[i] - RTE::update(a2, a22, j2)[i])/dT<<' ';
  std::cout<< '\n' << "Computed deriv: " << RTE::dupdate(a2, a22, Da22, j2, Dj2) << '\n' << '\n';
  std::cout << RTE::update(a1, a11, j1) << '\n' << RTE::update(a1, da11, dj1) << '\n' << "Manual derivat: ";
  for (int i=0; i<1; i++) std::cout << (RTE::update(a1, da11, dj1)[i] - RTE::update(a1, a11, j1)[i])/dT<<' ';
  std::cout<< '\n' << "Computed deriv: " << RTE::dupdate(a1, a11, Da11, j1, Dj1) << '\n' << '\n';
}

int main() {
  std::cout<<std::setprecision(20);
  test001();  // RadVec creation, stream output
  test002();  // TraMat creation, stream output
  test003();  // PropMat creation, stream output
  test004();  // RTE equation, stream output
  test005();  // PropMat to TraMat, stream output
  test006();  // RTE equation, RTE derivative
}
