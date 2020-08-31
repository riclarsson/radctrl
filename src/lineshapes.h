#ifndef lineshapes_h
#define lineshapes_h

#include <Faddeeva.hh>
#include <array>
#include <vector>

#include "complex.h"
#include "constants.h"
#include "enums.h"
#include "units.h"

namespace Absorption {
namespace LineShape {

ENUMCLASS(TemperatureModel, unsigned char, None, T0, T1, T2, T3, T4, T5, LM_AER,
          DPL)

class SingleParameter {
  static constexpr unsigned N = 4;
  Species::Species spec;
  TemperatureModel temp;
  unsigned char pres;
  std::array<double, N> coefs;

#define x0 coefs[0]
#define x1 coefs[1]
#define x2 coefs[2]
#define x3 coefs[3]

  constexpr double special_linemixing_aer(double T) const noexcept {
    if (T < 250)
      return x0 + (T - 200) * (x1 - x0) / (250 - 200);
    else if (T > 296)
      return x2 + (T - 296) * (x3 - x2) / (340 - 296);
    else
      return x1 + (T - 250) * (x2 - x1) / (296 - 250);
  }

  constexpr double special_linemixing_aer_dT(double T) const noexcept {
    if (T < 250)
      return (x1 - x0) / (250 - 200);
    else if (T > 296)
      return (x3 - x2) / (340 - 296);
    else
      return (x2 - x1) / (296 - 250);
  }

 public:
  constexpr SingleParameter() noexcept
      : spec(Species::Species::FINAL),
        temp(TemperatureModel::None),
        pres(0),
        coefs({std::numeric_limits<double>::quiet_NaN(),
               std::numeric_limits<double>::quiet_NaN(),
               std::numeric_limits<double>::quiet_NaN(),
               std::numeric_limits<double>::quiet_NaN()}) {}
  constexpr SingleParameter(Species::Species s, TemperatureModel t,
                            unsigned char p, std::array<double, N> c) noexcept
      : spec(s), temp(t), pres(p), coefs(c) {}

  friend std::ostream &operator<<(std::ostream &os, SingleParameter x) {
    switch (x.temp) {
      case TemperatureModel::None:
        break;
      case TemperatureModel::T0:
        os << x.x0;
        break;
      case TemperatureModel::T1:
        os << x.x0 << ' ' << x.x1;
        break;
      case TemperatureModel::T2:
        os << x.x0 << ' ' << x.x1 << ' ' << x.x2;
        break;
      case TemperatureModel::T3:
        os << x.x0 << ' ' << x.x1;
        break;
      case TemperatureModel::T4:
        os << x.x0 << ' ' << x.x1 << ' ' << x.x2;
        break;
      case TemperatureModel::T5:
        os << x.x0 << ' ' << x.x1;
        break;
      case TemperatureModel::LM_AER:
        os << x.x0 << ' ' << x.x1 << ' ' << x.x2 << ' ' << x.x3;
        break;
      case TemperatureModel::DPL:
        os << x.x0 << ' ' << x.x1 << ' ' << x.x2 << ' ' << x.x3;
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return os;
  }

  friend std::istream &operator>>(std::istream &is, SingleParameter &x) {
    switch (x.temp) {
      case TemperatureModel::None:
        break;
      case TemperatureModel::T0:
        is >> x.x0;
        break;
      case TemperatureModel::T1:
        is >> x.x0 >> x.x1;
        break;
      case TemperatureModel::T2:
        is >> x.x0 >> x.x1 >> x.x2;
        break;
      case TemperatureModel::T3:
        is >> x.x0 >> x.x1;
        break;
      case TemperatureModel::T4:
        is >> x.x0 >> x.x1 >> x.x2;
        break;
      case TemperatureModel::T5:
        is >> x.x0 >> x.x1;
        break;
      case TemperatureModel::LM_AER:
        is >> x.x0 >> x.x1 >> x.x2 >> x.x3;
        break;
      case TemperatureModel::DPL:
        is >> x.x0 >> x.x1 >> x.x2 >> x.x3;
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return is;
  }

  constexpr double operator()(Temperature<TemperatureType::K> T,
                              Temperature<TemperatureType::K> T0,
                              Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = x0;
        break;
      case TemperatureModel::T1:
        out = x0 * pow(T0 / T, x1);
        break;
      case TemperatureModel::T2:
        out = x0 * pow(T0 / T, x1) * (1 + x2 * log(T / T0));
        break;
      case TemperatureModel::T3:
        out = x0 + x1 * (T - T0);
        break;
      case TemperatureModel::T4:
        out = (x0 + x1 * (T0 / T - 1.)) * pow(T0 / T, x2);
        break;
      case TemperatureModel::T5:
        out = x0 * pow(T0 / T, 0.25 + 1.5 * x1);
        break;
      case TemperatureModel::LM_AER:
        out = special_linemixing_aer(T);
        break;
      case TemperatureModel::DPL:
        out = x0 * pow(T0 / T, x1) + x2 * pow(T0 / T, x3);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dX0(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0,
             Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 1;
        break;
      case TemperatureModel::T1:
        out = pow(T0 / T, x1);
        break;
      case TemperatureModel::T2:
        out = pow(T0 / T, x1) * (1 + x2 * log(T / T0));
        break;
      case TemperatureModel::T3:
        out = 1;
        break;
      case TemperatureModel::T4:
        out = pow(T0 / T, x2);
        break;
      case TemperatureModel::T5:
        out = pow(T0 / T, 1.5 * x1 + 0.25);
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = pow(T0 / T, x1);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dX1(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0,
             Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = x0 * pow(T0 / T, x1) * log(T0 / T);
        break;
      case TemperatureModel::T2:
        out = x0 * pow(T0 / T, x1) * (x2 * log(T / T0) + 1.) * log(T0 / T);
        break;
      case TemperatureModel::T3:
        out = (T - T0);
        break;
      case TemperatureModel::T4:
        out = pow(T0 / T, x2) * (T0 / T - 1.);
        break;
      case TemperatureModel::T5:
        out = 1.5 * x0 * pow(T0 / T, 1.5 * x1 + 0.25) * log(T0 / T);
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = x0 * pow(T0 / T, x1) * log(T0 / T);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dX2(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0,
             Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = 0;
        break;
      case TemperatureModel::T2:
        out = x0 * pow(T0 / T, x1) * log(T / T0);
        break;
      case TemperatureModel::T3:
        out = 0;
        break;
      case TemperatureModel::T4:
        out = pow(T0 / T, x2) * (x0 + x1 * (T0 / T - 1)) * log(T0 / T);
        break;
      case TemperatureModel::T5:
        out = 0;
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = pow(T0 / T, x3);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dX3(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0,
             Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = 0;
        break;
      case TemperatureModel::T2:
        out = 0;
        break;
      case TemperatureModel::T3:
        out = 0;
        break;
      case TemperatureModel::T4:
        out = 0;
        break;
      case TemperatureModel::T5:
        out = 0;
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = x2 * pow(T0 / T, x3) * log(T0 / T);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dT(Temperature<TemperatureType::K> T,
            Temperature<TemperatureType::K> T0,
            Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = -x0 * x1 * pow(T0 / T, x1) / T;
        break;
      case TemperatureModel::T2:
        out = -x0 * x1 * pow(T0 / T, x1) * (x2 * log(T / T0) + 1.) / T +
              x0 * x2 * pow(T0 / T, x1) / T;
        break;
      case TemperatureModel::T3:
        out = x1;
        break;
      case TemperatureModel::T4:
        out = -x2 * pow(T0 / T, x2) * (x0 + x1 * (T0 / T - 1.)) / T -
              T0 * x1 * pow(T0 / T, x2) / pow(T, 2);
        break;
      case TemperatureModel::T5:
        out = -x0 * pow(T0 / T, 1.5 * x1 + 0.25) * (1.5 * x1 + 0.25) / T;
        break;
      case TemperatureModel::LM_AER:
        out = special_linemixing_aer_dT(T);
        break;
      case TemperatureModel::DPL:
        out = -x0 * x1 * pow(T0 / T, x1) / T + -x2 * x3 * pow(T0 / T, x3) / T;
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dT0(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0,
             Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = x0 * x1 * pow(T0 / T, x1) / T0;
        break;
      case TemperatureModel::T2:
        out = x0 * x1 * pow(T0 / T, x1) * (x2 * log(T / T0) + 1.) / T0 -
              x0 * x2 * pow(T0 / T, x1) / T0;
        break;
      case TemperatureModel::T3:
        out = -x1;
        break;
      case TemperatureModel::T4:
        out = x2 * pow(T0 / T, x2) * (x0 + x1 * (T0 / T - 1.)) / T0 +
              x1 * pow(T0 / T, x2) / T;
        break;
      case TemperatureModel::T5:
        out = x0 * pow(T0 / T, 1.5 * x1 + 0.25) * (1.5 * x1 + 0.25) / T0;
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = x0 * x1 * pow(T0 / T, x1) / T0 + x2 * x3 * pow(T0 / T, x3) / T0;
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pow(P, pres) * out;
  }

  double dP(Temperature<TemperatureType::K> T,
            Temperature<TemperatureType::K> T0,
            Pressure<PressureType::Pa> P) const noexcept {
    using std::log;
    using std::pow;

    double out = std::numeric_limits<double>::quiet_NaN();
    switch (temp) {
      case TemperatureModel::None:
        out = 0;
        break;
      case TemperatureModel::T0:
        out = 0;
        break;
      case TemperatureModel::T1:
        out = x0 * pow(T0 / T, x1) * log(T0 / T);
        break;
      case TemperatureModel::T2:
        out = x0 * pow(T0 / T, x1) * (x2 * log(T / T0) + 1.) * log(T0 / T);
        break;
      case TemperatureModel::T3:
        out = (T - T0);
        break;
      case TemperatureModel::T4:
        out = pow(T0 / T, x2) * (T0 / T - 1.);
        break;
      case TemperatureModel::T5:
        out = 1.5 * x0 * pow(T0 / T, 1.5 * x1 + 0.25) * log(T0 / T);
        break;
      case TemperatureModel::LM_AER:
        out = 0;
        break;
      case TemperatureModel::DPL:
        out = x0 * pow(T0 / T, x1) * log(T0 / T);
        break;
      case TemperatureModel::FINAL: {
      }
    }
    return pres * pow(P, pres - 1) * out;
  }

  double &X0() noexcept { return x0; }
  double &X1() noexcept { return x1; }
  double &X2() noexcept { return x2; }
  double &X3() noexcept { return x3; }

#undef x0
#undef x1
#undef x2
#undef x3

  void Species(Species::Species x) noexcept { spec = x; }
  void Temperature(TemperatureModel x) noexcept { temp = x; }
  void Pressure(unsigned char x) noexcept { pres = x; }
  constexpr Species::Species Species() const noexcept { return spec; }
  constexpr TemperatureModel Temperature() const noexcept { return temp; }
  constexpr unsigned char Pressure() const noexcept { return pres; }
};  // SingleParameter

ENUMCLASS(Parameter, unsigned char, G0, D0, G2, D2, FVC, ETA, Y, G, DV)

struct Output {
  double G0, D0, G2, D2, FVC, ETA, Y, G, DV;
  constexpr Output() noexcept
      : G0(0), D0(0), G2(0), D2(0), FVC(0), ETA(0), Y(0), G(0), DV(0) {}
  constexpr Output(double g0, double d0, double g2, double d2, double fvc,
                   double eta, double y, double g, double dv) noexcept
      : G0(g0),
        D0(d0),
        G2(g2),
        D2(d2),
        FVC(fvc),
        ETA(eta),
        Y(y),
        G(g),
        DV(dv) {}
  double &operator[](Parameter p) {
    if (p == Parameter::G0)
      return G0;
    else if (p == Parameter::D0)
      return D0;
    else if (p == Parameter::G2)
      return G2;
    else if (p == Parameter::D2)
      return D2;
    else if (p == Parameter::FVC)
      return FVC;
    else if (p == Parameter::ETA)
      return ETA;
    else if (p == Parameter::Y)
      return Y;
    else if (p == Parameter::G)
      return G;
    else if (p == Parameter::DV)
      return DV;
    else
      std::exit(1);
  }
};

constexpr Output mirrored(Output x) noexcept {
  return {x.G0, -x.D0, x.G2, -x.D2, x.FVC, x.ETA, x.Y, x.G, -x.DV};
}

struct ModelHelper {
  Species::Species s;
  Parameter p;
  LineShape::TemperatureModel tm;
  int pres;
};

class Model {
  static constexpr unsigned char N{(unsigned char)Parameter::FINAL};
  std::vector<std::array<SingleParameter, N>> data;

  SingleParameter &G0(int i) noexcept {
    return data[i][(unsigned char)Parameter::G0];
  }
  SingleParameter &D0(int i) noexcept {
    return data[i][(unsigned char)Parameter::D0];
  }

 public:
  Model() noexcept : data(0) {}
  Model(const std::vector<ModelHelper> &vmh) noexcept : data(vmh.size()) {
    for (size_t i = 0; i < data.size(); i++) {
      for (auto &param : data[i]) param.Species(vmh[i].s);
      for (auto &help : vmh) {
        data[i][size_t(help.p)].Temperature(help.tm);
        data[i][size_t(help.p)].Pressure(help.pres);
      }
    }
  }
  Model(const std::vector<std::array<SingleParameter, N>> &d) : data(d) {
    for (auto &a : d) {
      for (size_t i = 0; i < size_t(Parameter::FINAL); i++) {
        if (d[0][0].Species() not_eq a[i].Species())
          throw std::runtime_error("Bad species");
        if (d[0][i].Pressure() not_eq a[i].Pressure())
          throw std::runtime_error("Bad species");
        if (d[0][i].Temperature() not_eq a[i].Temperature())
          throw std::runtime_error("Bad species");
      }
    }
  }

  // HITRAN par-style
  Model(Species::Species self,
        PressureBroadening<FrequencyType::Freq, PressureType::Pa> g0air,
        PressureBroadening<FrequencyType::Freq, PressureType::Pa> g0self,
        PressureBroadening<FrequencyType::Freq, PressureType::Pa> d0air,
        double nair) noexcept
      : data(2) {
    for (auto &x : data[0]) x.Species(Species::Species::Bath);
    for (auto &x : data[1]) x.Species(self);
    G0(0).Species(Species::Species::Bath);
    G0(0).X0() = g0air;
    G0(0).X1() = nair;
    G0(0).Temperature(TemperatureModel::T1);
    G0(0).Pressure(1);
    D0(0).X0() = d0air;
    D0(0).Temperature(TemperatureModel::T0);
    D0(0).Pressure(1);

    G0(1).Species(self);
    G0(1).X0() = g0self;
    G0(1).X1() = nair;
    G0(1).Temperature(TemperatureModel::T1);
    G0(1).Pressure(1);
    D0(1).X0() = d0air;
    D0(1).Temperature(
        TemperatureModel::T0);  // Copy because of how it is normalized
    D0(1).Pressure(1);
  }

  const std::array<SingleParameter, N> &operator[](size_t i) const noexcept {
    return data[i];
  }
  std::array<SingleParameter, N> &operator[](size_t i) noexcept {
    return data[i];
  }
  void resize(size_t n) noexcept { data.resize(n); }

  friend std::ostream &operator<<(std::ostream &os, const Model &x) {
    for (auto arr : x.data) {
      for (size_t i = 0; i < size_t(Parameter::FINAL); i++) {
        if (arr[i].Temperature() not_eq TemperatureModel::None) {
          os << arr[i];
          if (i < size_t(Parameter::FINAL) - 1) os << ' ';
        }
      }
    }
    return os;
  }

  friend std::istream &operator>>(std::istream &is, Model &x) {
    for (auto &arr : x.data) {
      for (size_t i = 0; i < size_t(Parameter::FINAL); i++) {
        if (arr[i].Temperature() not_eq TemperatureModel::None) {
          is >> arr[i];
        }
      }
    }
    return is;
  }

  size_t n_spec() const noexcept { return data.size(); }

  std::string description() const noexcept {
    std::ostringstream os;
    os << n_spec();
    for (auto &arr : data) {
      if (arr[0].Temperature() not_eq TemperatureModel::None)
        os << ' ' << arr[0].Species();
    }
    if (n_spec()) {
      for (size_t i = 0; i < size_t(Parameter::FINAL); i++) {
        auto param = Param(0, Parameter(i));
        if (param.Temperature() not_eq TemperatureModel::None) {
          os << ' ' << Parameter(i) << ' ' << param.Temperature() << ' '
             << int(param.Pressure());
        }
      }
    }
    return os.str();
  }

  const SingleParameter &Param(size_t i, Parameter X) const noexcept {
    return data[i][size_t(X)];
  }

  Output operator()(
      Temperature<TemperatureType::K> T, Temperature<TemperatureType::K> T0,
      Pressure<PressureType::Pa> P,
      const std::vector<VMR<VMRType::ratio>> &vmr) const noexcept {
    Output out;
    double vmrsum = 0;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          for (unsigned char i = 0; i < N; i++) {
            out[Parameter(i)] += params[i](T, T0, P) * v.value();
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dX0(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
             const std::vector<VMR<VMRType::ratio>> &vmr,
             Species::Species spec) const noexcept {
    Output out;
    double vmrsum = 0;
    bool done = false;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          if (not done and spec == v.Species()) {
            for (unsigned char i = 0; i < N; i++) {
              out[Parameter(i)] += params[i].dX0(T, T0, P) * v.value();
            }
            done = true;
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dX1(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
             const std::vector<VMR<VMRType::ratio>> &vmr,
             Species::Species spec) const noexcept {
    Output out;
    double vmrsum = 0;
    bool done = false;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          if (not done and spec == v.Species()) {
            for (unsigned char i = 0; i < N; i++) {
              out[Parameter(i)] += params[i].dX1(T, T0, P) * v.value();
            }
            done = true;
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dX2(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
             const std::vector<VMR<VMRType::ratio>> &vmr,
             Species::Species spec) const noexcept {
    Output out;
    double vmrsum = 0;
    bool done = false;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          if (not done and spec == v.Species()) {
            for (unsigned char i = 0; i < N; i++) {
              out[Parameter(i)] += params[i].dX2(T, T0, P) * v.value();
            }
            done = true;
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dX3(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
             const std::vector<VMR<VMRType::ratio>> &vmr,
             Species::Species spec) const noexcept {
    Output out;
    double vmrsum = 0;
    bool done = false;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          if (not done and spec == v.Species()) {
            for (unsigned char i = 0; i < N; i++) {
              out[Parameter(i)] += params[i].dX3(T, T0, P) * v.value();
            }
            done = true;
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dT(Temperature<TemperatureType::K> T,
            Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
            const std::vector<VMR<VMRType::ratio>> &vmr) const noexcept {
    Output out;
    double vmrsum = 0;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          for (unsigned char i = 0; i < N; i++) {
            out[Parameter(i)] += params[i].dT(T, T0, P) * v.value();
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dT0(Temperature<TemperatureType::K> T,
             Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
             const std::vector<VMR<VMRType::ratio>> &vmr) const noexcept {
    Output out;
    double vmrsum = 0;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          for (unsigned char i = 0; i < N; i++) {
            out[Parameter(i)] += params[i].dT0(T, T0, P) * v.value();
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

  Output dP(Temperature<TemperatureType::K> T,
            Temperature<TemperatureType::K> T0, Pressure<PressureType::Pa> P,
            const std::vector<VMR<VMRType::ratio>> &vmr) const noexcept {
    Output out;
    double vmrsum = 0;
    for (auto &params : data) {
      for (auto &v : vmr) {
        if (v.Species() == params[0].Species()) {
          vmrsum += v.value();
          for (unsigned char i = 0; i < N; i++) {
            out[Parameter(i)] += params[i].dP(T, T0, P) * v.value();
          }
          break;
        }
      }
    }

    if (vmrsum not_eq 0) {
      for (unsigned char i = 0; i < N; i++) {
        out[Parameter(i)] /= vmrsum;
      }
    }

    return out;
  }

};  // Model

namespace Base {
class HartmannTran {
  enum class CalcType : char {
    Noc2tLowZ,
    Noc2tHighZ,
    LowXandHighY,
    LowYandLowX,
    LowYandHighX,
    Full
  };

  double G0;
  double D0;
  double G2;
  double D2;
  double FVC;
  double ETA;
  double mF0;
  double invGD;
  Complex deltax;
  Complex sqrty;

  CalcType calcs;

  Complex x;
  Complex sqrtxy;
  Complex sqrtx;
  Complex z1;
  Complex z2;
  Complex w1;
  Complex w2;
  Complex A;
  Complex B;
  Complex K;
  Complex dw1;
  Complex dw2;

 public:
  Complex F;

  template <typename LineData>
  HartmannTran(double F0_noshift, LineData ls, double GD_div_F0,
               double dZ) noexcept
      : G0(ls.G0),
        D0(ls.D0),
        G2(ls.G2),
        D2(ls.D2),
        FVC(ls.FVC),
        ETA(ls.ETA),
        mF0(F0_noshift + dZ + (1 - ls.ETA) * (ls.D0 - 1.5 * ls.D2)),
        invGD(Constant::sqrt_ln_2 / (GD_div_F0 * mF0)),
        deltax(ls.FVC + (1 - ls.ETA) * (ls.G0 - 3 * ls.G2 / 2), mF0),
        sqrty(1 / (2 * (1 - ls.ETA) * Complex(ls.G2, ls.D2) * invGD)) {
    calc();
  }

  Complex dFdf(double) const noexcept {
    constexpr Complex ddeltax = Complex(0, -1);
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = dx / (2 * sqrtxy);

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Constant::sqrt_pi *
                     ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                      (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                      2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) /
                     (2 * sqrty * (ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB =
            -invGD * (Constant::sqrt_pi * ((Constant::pow2(z1) - 1) *
                                               Complex(0, 1) * dw1 * dz1 +
                                           2 * w1 * z1 * dz1) -
                      dz1);
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = invGD * ddeltax;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (Constant::sqrt_pi * w2 * z2 - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx +
                      (2 * x - 3) * x * dx / 2) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdF0() const noexcept {
    double dGD = (1 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty =
        dinvGD / (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(invGD));
    constexpr Complex ddeltax = Complex(0, 1);
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            Constant::sqrt_pi *
            ((-(Constant::pow2(z1) - 1) * w1 + (Constant::pow2(z2) - 1) * w2) *
                 dsqrty +
             ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
              (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
              2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                 sqrty) /
            (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(sqrty));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (4 * sqrty * dsqrty + dx) *
                           (Constant::sqrt_pi * w2 * z2 - 1)) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) +
                      (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x / 2 -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdD0(double dD0) const noexcept {
    double dmF0 = (1 - ETA) * dD0;
    double dGD = (dmF0 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty =
        dinvGD / (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(invGD));
    Complex ddeltax = Complex(0, 1 - ETA) * dD0;
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            Constant::sqrt_pi *
            ((-(Constant::pow2(z1) - 1) * w1 + (Constant::pow2(z2) - 1) * w2) *
                 dsqrty +
             ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
              (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
              2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                 sqrty) /
            (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(sqrty));
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (4 * sqrty * dsqrty + dx) *
                           (Constant::sqrt_pi * w2 * z2 - 1)) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) +
                      (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x / 2 -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB + Complex(0, ETA * dD0) * A +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdG0(double dG0) const noexcept {
    double ddeltax = (1 - ETA) * dG0;
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = dx / (2 * sqrtxy);

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Constant::sqrt_pi *
                     ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                      (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                      2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) /
                     (2 * sqrty * (ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB =
            -invGD * (Constant::sqrt_pi * ((Constant::pow2(z1) - 1) *
                                               Complex(0, 1) * dw1 * dz1 +
                                           2 * w1 * z1 * dz1) -
                      dz1);
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = invGD * ddeltax;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (Constant::sqrt_pi * w2 * z2 - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx +
                      (2 * x - 3) * x * dx / 2) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB + ETA * A * dG0 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdD2(double dD2) const noexcept {
    double dmF0 = -3 * (1 - ETA) * dD2 / 2;
    double dGD = (dmF0 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty = (Complex(G2, D2) * dinvGD + Complex(0, invGD) * dD2) /
                     (2 * (ETA - 1) * Constant::pow2(Complex(G2, D2)) *
                      Constant::pow2(invGD));
    Complex ddeltax = 1.5 * Complex(0, ETA - 1) * dD2;
    Complex dx = (-Complex(G2, D2) * ddeltax + Complex(0, dD2) * deltax) /
                 ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            (Constant::sqrt_pi * Complex(G2, D2) *
                 ((-(Constant::pow2(z1) - 1) * w1 +
                   (Constant::pow2(z2) - 1) * w2) *
                      dsqrty +
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                   (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                   2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                      sqrty) -
             Complex(0, 1) *
                 (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
                  Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 +
                  2 * sqrty) *
                 sqrty * dD2) /
            (2 * (ETA - 1) * Constant::pow2(Complex(G2, D2)) *
             Constant::pow2(sqrty));
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 *
                     (Constant::sqrt_pi * Complex(G2, D2) *
                          (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) -
                      Complex(0, 1) * (Constant::sqrt_pi * w2 * z2 - 1) * dD2) /
                     ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
        Complex dB = (-2 * Complex(G2, D2) *
                          (Constant::sqrt_pi *
                               (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                               (2 * Constant::pow2(sqrty) + x - 1) +
                           Constant::sqrt_pi * w1 * dz1 +
                           Complex(0, Constant::sqrt_pi) * z1 * dw1 * dz1 +
                           (4 * sqrty * dsqrty + dx) *
                               (Constant::sqrt_pi * w2 * z2 - 1)) +
                      Complex(0, 1) *
                          (2 * Constant::sqrt_pi * w1 * z1 +
                           2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                               (2 * Constant::pow2(sqrty) + x - 1) -
                           1) *
                          dD2) /
                     ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (Complex(G2, D2) * (x - 3) * dx +
             Complex(0, 1) * (2 * x - 3) * x * dD2 / 2) /
            ((ETA - 1) * Constant::pow2(Complex(G2, D2)) * Constant::pow3(x));
        Complex dB =
            (Complex(G2, D2) *
                 (-4 * Constant::sqrt_pi *
                      (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                      Constant::pow3(x) +
                  (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x -
                  2 * (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) -
             Complex(0, 1) *
                 (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) *
                      Constant::pow2(x) +
                  (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 x * dD2) /
            (2 * (ETA - 1) * Constant::pow2(Complex(G2, D2)) *
             Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB -
                     Complex(0, 1.5 * dD2 * ETA) * A +
                     Complex(0, dD2 * ETA) * B +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdG2(double dG2) const noexcept {
    Complex dsqrty =
        dG2 / (2 * invGD * (ETA - 1) * Constant::pow2(Complex(G2, D2)));
    double ddeltax = 3 * (ETA - 1) * dG2 / 2;
    Complex dx = (-Complex(G2, D2) * ddeltax + deltax * dG2) /
                 ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB =
            (Constant::sqrt_pi * Complex(G2, D2) *
                 ((-(Constant::pow2(z1) - 1) * w1 +
                   (Constant::pow2(z2) - 1) * w2) *
                      dsqrty +
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                   (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                   2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                      sqrty) -
             (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
              Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 + 2 * sqrty) *
                 sqrty * dG2) /
            (2 * (ETA - 1) * Constant::pow2(Complex(G2, D2)) *
             Constant::pow2(sqrty));
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB =
            -invGD * (Constant::sqrt_pi * ((Constant::pow2(z1) - 1) *
                                               Complex(0, 1) * dw1 * dz1 +
                                           2 * w1 * z1 * dz1) -
                      dz1);
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 *
                     (Constant::sqrt_pi * Complex(G2, D2) *
                          (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) -
                      (Constant::sqrt_pi * w2 * z2 - 1) * dG2) /
                     ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
        Complex dB = (-2 * Complex(G2, D2) *
                          (Constant::sqrt_pi *
                               (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                               (2 * Constant::pow2(sqrty) + x - 1) +
                           Constant::sqrt_pi * w1 * dz1 +
                           Complex(0, Constant::sqrt_pi) * z1 * dw1 * dz1 +
                           (4 * sqrty * dsqrty + dx) *
                               (Constant::sqrt_pi * w2 * z2 - 1)) +
                      (2 * Constant::sqrt_pi * w1 * z1 +
                       2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                           (2 * Constant::pow2(sqrty) + x - 1) -
                       1) *
                          dG2) /
                     ((ETA - 1) * Constant::pow2(Complex(G2, D2)));
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (Complex(G2, D2) * (x - 3) * dx + (2 * x - 3) * x * dG2 / 2) /
            ((ETA - 1) * Constant::pow2(Complex(G2, D2)) * Constant::pow3(x));
        Complex dB =
            (Complex(G2, D2) *
                 (-4 * Constant::sqrt_pi *
                      (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                      Constant::pow3(x) +
                  (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x -
                  2 * (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) -
             (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) * Constant::pow2(x) +
              (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 x * dG2) /
            (2 * (ETA - 1) * Constant::pow2(Complex(G2, D2)) *
             Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB - 3 * ETA * A * dG2 / 2 +
                     ETA * B * dG2 +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdFVC(double dFVC) const noexcept {
    double ddeltax = dFVC;
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = dx / (2 * sqrtxy);

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Constant::sqrt_pi *
                     ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                      (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                      2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) /
                     (2 * sqrty * (ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB =
            -invGD * (Constant::sqrt_pi * ((Constant::pow2(z1) - 1) *
                                               Complex(0, 1) * dw1 * dz1 +
                                           2 * w1 * z1 * dz1) -
                      dz1);
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = invGD * ddeltax;
        Complex dA = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1;
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = invGD * ddeltax;
        Complex dz2 = dsqrtxy;
        Complex dA =
            Complex(0, Constant::sqrt_pi * invGD) * (dw1 * dz1 - dw2 * dz2);
        Complex dB = Complex(0, Constant::sqrt_pi * invGD) * dw1 * dz1 -
                     invGD * dz1 / (2 * Constant::pow2(z1)) +
                     9 * invGD * dz1 / (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (Constant::sqrt_pi * w2 * z2 - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx +
                      (2 * x - 3) * x * dx / 2) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA -
                     A * dFVC;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdETA(double dETA) const noexcept {
    double dmF0 = -(D0 - 3 * D2 / 2) * dETA;
    double dGD = (dmF0 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty =
        ((ETA - 1) * dinvGD + invGD * dETA) /
        (2 * Complex(G2, D2) * Constant::pow2(ETA - 1) * Constant::pow2(invGD));
    Complex ddeltax = -dETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2);
    Complex dx = (-(ETA - 1) * ddeltax + deltax * dETA) /
                 (Complex(G2, D2) * Constant::pow2(ETA - 1));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            (Constant::sqrt_pi *
                 ((-(Constant::pow2(z1) - 1) * w1 +
                   (Constant::pow2(z2) - 1) * w2) *
                      dsqrty +
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                   (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                   2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                      sqrty) *
                 (ETA - 1) -
             (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
              Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 + 2 * sqrty) *
                 sqrty * dETA) /
            (2 * Complex(G2, D2) * Constant::pow2(ETA - 1) *
             Constant::pow2(sqrty));
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA =
            2 *
            (Constant::sqrt_pi * (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                 (ETA - 1) -
             (Constant::sqrt_pi * w2 * z2 - 1) * dETA) /
            (Complex(G2, D2) * Constant::pow2(ETA - 1));
        Complex dB = (-2 * (ETA - 1) *
                          (Constant::sqrt_pi *
                               (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                               (2 * Constant::pow2(sqrty) + x - 1) +
                           Constant::sqrt_pi * w1 * dz1 +
                           Complex(0, Constant::sqrt_pi) * z1 * dw1 * dz1 +
                           (4 * sqrty * dsqrty + dx) *
                               (Constant::sqrt_pi * w2 * z2 - 1)) +
                      (2 * Constant::sqrt_pi * w1 * z1 +
                       2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                           (2 * Constant::pow2(sqrty) + x - 1) -
                       1) *
                          dETA) /
                     (Complex(G2, D2) * Constant::pow2(ETA - 1));
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            ((ETA - 1) * (x - 3) * dx + (2 * x - 3) * x * dETA / 2) /
            (Complex(G2, D2) * Constant::pow2(ETA - 1) * Constant::pow3(x));
        Complex dB =
            (-(2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) * Constant::pow2(x) +
               (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 x * dETA +
             (ETA - 1) *
                 (-4 * Constant::sqrt_pi *
                      (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                      Constant::pow3(x) +
                  (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x -
                  2 * (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx)) /
            (2 * Complex(G2, D2) * Constant::pow2(ETA - 1) * Constant::pow3(x));
        Complex dK = (-FVC + Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA) * dA +
                     Complex(G2, D2) * B * dETA + Complex(G2, D2) * ETA * dB -
                     Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * A * dETA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  Complex dFdH(double dZ) const noexcept {
    double dmF0 = dZ;
    double dGD = (dmF0 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty =
        dinvGD / (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(invGD));
    Complex ddeltax = Complex(0, dZ);
    Complex dx = -ddeltax / ((ETA - 1) * Complex(G2, D2));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            Constant::sqrt_pi *
            ((-(Constant::pow2(z1) - 1) * w1 + (Constant::pow2(z2) - 1) * w2) *
                 dsqrty +
             ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
              (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
              2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                 sqrty) /
            (2 * (ETA - 1) * Complex(G2, D2) * Constant::pow2(sqrty));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA = 2 * Constant::sqrt_pi *
                     (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dB = -(2 * Constant::sqrt_pi *
                           (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                           (2 * Constant::pow2(sqrty) + x - 1) +
                       2 * Constant::sqrt_pi * w1 * dz1 +
                       Complex(0, 2 * Constant::sqrt_pi) * z1 * dw1 * dz1 +
                       2 * (4 * sqrty * dsqrty + dx) *
                           (Constant::sqrt_pi * w2 * z2 - 1)) /
                     ((ETA - 1) * Complex(G2, D2));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA =
            (x - 3) * dx / ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dB = (-2 * Constant::sqrt_pi *
                          (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                          Constant::pow3(x) +
                      (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x / 2 -
                      (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) /
                     ((ETA - 1) * Complex(G2, D2) * Constant::pow3(x));
        Complex dK = ETA * Complex(G2, D2) * dB +
                     (ETA * Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) - FVC) * dA;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  template <typename LineData>
  Complex dFdVMR(LineData d) const noexcept {
    double dmF0 = (1 - ETA) * (d.D0 - 3 * d.D2 / 2) - (D0 - 3 * D2 / 2) * d.ETA;
    double dGD = (dmF0 / (invGD * mF0));
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty = (Complex(G2, D2) * (ETA - 1) * dinvGD +
                      Complex(G2, D2) * invGD * d.ETA +
                      Complex(d.G2, d.D2) * (ETA - 1) * invGD) /
                     (2 * Constant::pow2(Complex(G2, D2)) *
                      Constant::pow2(ETA - 1) * Constant::pow2(invGD));
    Complex ddeltax =
        -(ETA - 1) * Complex(d.G0 - 1.5 * d.G2, d.D0 - 1.5 * d.D2) -
        Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * d.ETA + d.FVC;
    Complex dx = (-Complex(G2, D2) * (ETA - 1) * ddeltax +
                  Complex(G2, D2) * deltax * d.ETA +
                  Complex(d.G2, d.D2) * (ETA - 1) * deltax) /
                 (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            (Constant::sqrt_pi * Complex(G2, D2) *
                 ((-(Constant::pow2(z1) - 1) * w1 +
                   (Constant::pow2(z2) - 1) * w2) *
                      dsqrty +
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                   (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                   2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                      sqrty) *
                 (ETA - 1) -
             Complex(G2, D2) *
                 (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
                  Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 +
                  2 * sqrty) *
                 sqrty * d.ETA -
             Complex(d.G2, d.D2) * (ETA - 1) *
                 (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
                  Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 +
                  2 * sqrty) *
                 sqrty) /
            (2 * Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1) *
             Constant::pow2(sqrty));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA =
            2 *
            (Constant::sqrt_pi * Complex(G2, D2) *
                 (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) * (ETA - 1) -
             Complex(G2, D2) * (Constant::sqrt_pi * w2 * z2 - 1) * d.ETA -
             Complex(d.G2, d.D2) * (Constant::sqrt_pi * w2 * z2 - 1) *
                 (ETA - 1)) /
            (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
        Complex dB =
            (-2 * Complex(G2, D2) * (ETA - 1) *
                 (Constant::sqrt_pi *
                      (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                      (2 * Constant::pow2(sqrty) + x - 1) +
                  Constant::sqrt_pi * w1 * dz1 +
                  Complex(0, Constant::sqrt_pi) * z1 * dw1 * dz1 +
                  (4 * sqrty * dsqrty + dx) *
                      (Constant::sqrt_pi * w2 * z2 - 1)) +
             Complex(G2, D2) *
                 (2 * Constant::sqrt_pi * w1 * z1 +
                  2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                      (2 * Constant::pow2(sqrty) + x - 1) -
                  1) *
                 d.ETA +
             Complex(d.G2, d.D2) * (ETA - 1) *
                 (2 * Constant::sqrt_pi * w1 * z1 +
                  2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                      (2 * Constant::pow2(sqrty) + x - 1) -
                  1)) /
            (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA = (2 * Complex(G2, D2) * (ETA - 1) * (x - 3) * dx +
                      Complex(G2, D2) * (2 * x - 3) * x * d.ETA +
                      Complex(d.G2, d.D2) * (ETA - 1) * (2 * x - 3) * x) /
                     (2 * Constant::pow2(Complex(G2, D2)) *
                      Constant::pow2(ETA - 1) * Constant::pow3(x));
        Complex dB =
            (-Complex(G2, D2) *
                 (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) *
                      Constant::pow2(x) +
                  (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 x * d.ETA +
             Complex(G2, D2) * (ETA - 1) *
                 (-4 * Constant::sqrt_pi *
                      (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                      Constant::pow3(x) +
                  (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x -
                  2 * (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) -
             Complex(d.G2, d.D2) *
                 (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) *
                      Constant::pow2(x) +
                  (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 (ETA - 1) * x) /
            (2 * Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1) *
             Constant::pow3(x));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  template <typename LineData>
  Complex dFdT(LineData d, double T) const noexcept {
    double dmF0 = (1 - ETA) * (d.D0 - 3 * d.D2 / 2) - (D0 - 3 * D2 / 2) * d.ETA;
    double dGD =
        (dmF0 / (invGD * mF0)) - invGD * invGD / (2 * T * Constant::sqrt_ln_2);
    double dinvGD = -dGD * Constant::pow2(invGD);
    Complex dsqrty = (Complex(G2, D2) * (ETA - 1) * dinvGD +
                      Complex(G2, D2) * invGD * d.ETA +
                      Complex(d.G2, d.D2) * (ETA - 1) * invGD) /
                     (2 * Constant::pow2(Complex(G2, D2)) *
                      Constant::pow2(ETA - 1) * Constant::pow2(invGD));
    Complex ddeltax =
        -(ETA - 1) * Complex(d.G0 - 1.5 * d.G2, d.D0 - 1.5 * d.D2) -
        Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * d.ETA + d.FVC;
    Complex dx = (-Complex(G2, D2) * (ETA - 1) * ddeltax +
                  Complex(G2, D2) * deltax * d.ETA +
                  Complex(d.G2, d.D2) * (ETA - 1) * deltax) /
                 (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
    Complex dsqrtxy = (sqrty * dsqrty + dx / 2) / sqrtxy;

    switch (calcs) {
      case CalcType::Full: {
        Complex dz1 = dsqrtxy - dsqrty;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB =
            (Constant::sqrt_pi * Complex(G2, D2) *
                 ((-(Constant::pow2(z1) - 1) * w1 +
                   (Constant::pow2(z2) - 1) * w2) *
                      dsqrty +
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 -
                   (Constant::pow2(z2) - 1) * Complex(0, 1) * dw2 * dz2 +
                   2 * w1 * z1 * dz1 - 2 * w2 * z2 * dz2) *
                      sqrty) *
                 (ETA - 1) -
             Complex(G2, D2) *
                 (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
                  Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 +
                  2 * sqrty) *
                 sqrty * d.ETA -
             Complex(d.G2, d.D2) * (ETA - 1) *
                 (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 -
                  Constant::sqrt_pi * (Constant::pow2(z2) - 1) * w2 +
                  2 * sqrty) *
                 sqrty) /
            (2 * Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1) *
             Constant::pow2(sqrty));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tLowZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB =
            -(Constant::sqrt_pi *
                  ((Constant::pow2(z1) - 1) * Complex(0, 1) * dw1 * dz1 +
                   2 * w1 * z1 * dz1) -
              dz1) *
                invGD -
            (Constant::sqrt_pi * (Constant::pow2(z1) - 1) * w1 - z1) * dinvGD;
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::Noc2tHighZ: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dA =
            Constant::sqrt_pi * (Complex(0, invGD) * dw1 * dz1 + w1 * dinvGD);
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowXandHighY: {
        Complex dz1 = deltax * dinvGD + invGD * ddeltax;
        Complex dz2 = dsqrtxy + dsqrty;
        Complex dA =
            Constant::sqrt_pi * ((w1 - w2) * dinvGD +
                                 (Complex(0, invGD) * (dw1 * dz1 - dw2 * dz2)));
        Complex dB = ((4 * Constant::sqrt_pi * w1 * Constant::pow3(z1) +
                       2 * Constant::pow2(z1) - 3) *
                          z1 * dinvGD +
                      (Complex(0, 4 * Constant::sqrt_pi) * Constant::pow4(z1) *
                           dw1 * dz1 -
                       2 * Constant::pow2(z1) * dz1 + 9 * dz1) *
                          invGD) /
                     (4 * Constant::pow4(z1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandLowX: {
        Complex dz1 = dsqrtxy;
        Complex dz2 = dx / (2 * sqrtx);
        Complex dA =
            2 *
            (Constant::sqrt_pi * Complex(G2, D2) *
                 (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) * (ETA - 1) -
             Complex(G2, D2) * (Constant::sqrt_pi * w2 * z2 - 1) * d.ETA -
             Complex(d.G2, d.D2) * (Constant::sqrt_pi * w2 * z2 - 1) *
                 (ETA - 1)) /
            (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
        Complex dB =
            (-2 * Complex(G2, D2) * (ETA - 1) *
                 (Constant::sqrt_pi *
                      (w2 * dz2 + z2 * Complex(0, 1) * dw2 * dz2) *
                      (2 * Constant::pow2(sqrty) + x - 1) +
                  Constant::sqrt_pi * w1 * dz1 +
                  Complex(0, Constant::sqrt_pi) * z1 * dw1 * dz1 +
                  (4 * sqrty * dsqrty + dx) *
                      (Constant::sqrt_pi * w2 * z2 - 1)) +
             Complex(G2, D2) *
                 (2 * Constant::sqrt_pi * w1 * z1 +
                  2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                      (2 * Constant::pow2(sqrty) + x - 1) -
                  1) *
                 d.ETA +
             Complex(d.G2, d.D2) * (ETA - 1) *
                 (2 * Constant::sqrt_pi * w1 * z1 +
                  2 * (Constant::sqrt_pi * w2 * z2 - 1) *
                      (2 * Constant::pow2(sqrty) + x - 1) -
                  1)) /
            (Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
      case CalcType::LowYandHighX: {
        Complex dz1 = dsqrtxy;
        Complex dA = (2 * Complex(G2, D2) * (ETA - 1) * (x - 3) * dx +
                      Complex(G2, D2) * (2 * x - 3) * x * d.ETA +
                      Complex(d.G2, d.D2) * (ETA - 1) * (2 * x - 3) * x) /
                     (2 * Constant::pow2(Complex(G2, D2)) *
                      Constant::pow2(ETA - 1) * Constant::pow3(x));
        Complex dB =
            (-Complex(G2, D2) *
                 (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) *
                      Constant::pow2(x) +
                  (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 x * d.ETA +
             Complex(G2, D2) * (ETA - 1) *
                 (-4 * Constant::sqrt_pi *
                      (w1 * dz1 + z1 * Complex(0, 1) * dw1 * dz1) *
                      Constant::pow3(x) +
                  (4 * sqrty * dsqrty + dx) * (2 * x - 3) * x -
                  2 * (x - 3) * (2 * Constant::pow2(sqrty) + x - 1) * dx) -
             Complex(d.G2, d.D2) *
                 (2 * (-2 * Constant::sqrt_pi * w1 * z1 + 1) *
                      Constant::pow2(x) +
                  (2 * x - 3) * (2 * Constant::pow2(sqrty) + x - 1)) *
                 (ETA - 1) * x) /
            (2 * Constant::pow2(Complex(G2, D2)) * Constant::pow2(ETA - 1) *
             Constant::pow3(x));
        Complex dK =
            Complex(G2, D2) * B * d.ETA + Complex(G2, D2) * ETA * dB +
            Complex(d.G2, d.D2) * B * ETA +
            (Complex(G0 - 1.5 * G2, D0 - 1.5 * D2) * ETA - FVC) * dA +
            (-Complex(1.5 * G2 - G0, 1.5 * D2 - D0) * d.ETA -
             Complex(1.5 * d.G2 - d.G0, 1.5 * d.D2 - d.D0) * ETA - d.FVC) *
                A;
        return Constant::inv_pi * (-A * dK + K * dA) / Constant::pow2(K);
      }
    }
    return {};
  }

  static constexpr Complex dFdDV(double) noexcept { return 0; }
  static constexpr Complex dFdY(double) noexcept { return 0; }
  static constexpr Complex dFdG(double) noexcept { return 0; }

  double F0() const noexcept { return mF0; }

  Complex &operator()(double f) noexcept {
    reinterpret_cast<double(&)[2]>(deltax)[1] = mF0 - f;
    x = deltax / ((1 - ETA) * Complex(G2, D2));
    sqrtxy = std::sqrt(x + sqrty * sqrty);
    update_calcs();
    calc();
    return F;
  }

 private:
  CalcType init(const Complex c2t) const noexcept {
    if (abs_squared(c2t) == 0)
      return CalcType::Noc2tHighZ;  // nb. Value of high/low changes elsewhere
    else if (abs_squared(x) <= 9e-16 * abs_squared(sqrty * sqrty))
      return CalcType::LowXandHighY;
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)) and
             abs_squared(std::sqrt(x)) <= 16.e6)
      return CalcType::LowYandLowX;  // Weird case, untested
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)))
      return CalcType::LowYandHighX;
    else
      return CalcType::Full;
  }

  void update_calcs() noexcept { calcs = init((1 - ETA) * Complex(G2, D2)); }

  void calc() noexcept {
    switch (calcs) {
      case CalcType::Full:
        z1 = sqrtxy - sqrty;
        z2 = sqrtxy + sqrty;
        w1 = Faddeeva::w(Complex(0, 1) * z1);
        w2 = Faddeeva::w(Complex(0, 1) * z2);
        A = Constant::sqrt_pi * invGD * (w1 - w2);
        B = (-1 +
             Constant::sqrt_pi / (2 * sqrty) * (1 - Constant::pow2(z1)) * w1 -
             Constant::sqrt_pi / (2 * sqrty) * (1 - Constant::pow2(z2)) * w2) /
            ((1 - ETA) * Complex(G2, D2));
        break;
      case CalcType::Noc2tLowZ:
      case CalcType::Noc2tHighZ:
        z1 = deltax * invGD;
        w1 = Faddeeva::w(Complex(0, 1) * z1);
        A = Constant::sqrt_pi * invGD * w1;
        if (abs_squared(z1) < 16e6) {
          calcs = CalcType::Noc2tLowZ;
          B = Constant::sqrt_pi * invGD *
              ((1 - Constant::pow2(z1)) * w1 + z1 / Constant::sqrt_pi);
        } else {
          calcs = CalcType::Noc2tHighZ;
          B = invGD * (Constant::sqrt_pi * w1 + 1 / z1 / 2 -
                       3 / Constant::pow3(z1) / 4);
        }
        break;
      case CalcType::LowXandHighY:
        z1 = deltax * invGD;
        z2 = sqrtxy + sqrty;
        w1 = Faddeeva::w(Complex(0, 1) * z1);
        w2 = Faddeeva::w(Complex(0, 1) * z2);
        A = Constant::sqrt_pi * invGD * (w1 - w2);
        B = invGD *
            (Constant::sqrt_pi * w1 + 1 / z1 / 2 - 3 / Constant::pow3(z1) / 4);
        break;
      case CalcType::LowYandLowX:
        sqrtx = std::sqrt(x);
        z1 = sqrtxy;
        z2 = sqrtx;
        w1 = Faddeeva::w(Complex(0, 1) * z1);
        w2 = Faddeeva::w(Complex(0, 1) * z2);
        A = (2 * Constant::sqrt_pi / ((1 - ETA) * Complex(G2, D2))) *
            (Constant::inv_sqrt_pi - z2 * w2);
        B = (1 / ((1 - ETA) * Complex(G2, D2))) *
            (-1 +
             2 * Constant::sqrt_pi * (1 - x - 2 * sqrty * sqrty) *
                 (1 / Constant::sqrt_pi - z2 * w2) +
             2 * Constant::sqrt_pi * z1 * w1);
        break;
      case CalcType::LowYandHighX:
        z1 = sqrtxy;
        w1 = Faddeeva::w(Complex(0, 1) * z1);
        A = (1 / ((1 - ETA) * Complex(G2, D2))) *
            (1 / x - 3 / Constant::pow2(x) / 2);
        B = (1 / ((1 - ETA) * Complex(G2, D2))) *
            (-1 +
             (1 - x - 2 * sqrty * sqrty) * (1 / x - 3 / Constant::pow2(x) / 2) +
             2 * Constant::sqrt_pi * z1 * w1);
        break;
    }

    dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - z1 * w1);
    dw2 = Complex(0, 2) * (Constant::inv_sqrt_pi - z2 * w2);
    K = 1 - (FVC - ETA * (Complex(G0, D0) - 3 * Complex(G2, D2) / 2)) * A +
        ETA * Complex(G2, D2) * B;
    F = Constant::inv_pi * A / K;
  }
};  // HartmannTran

class SpeedDependentHardCollisionVoigt {
  enum class CalcType : char {
    Voigt,
    LowXandHighY,
    LowYandLowX,
    LowYandHighX,
    Full
  };
  union Sq {
    Complex x;
    Complex xy;
    Sq() noexcept : xy(0) {}
  };

  double mF0;
  double invGD;
  double FVC;
  Complex invc2;
  Complex dx;
  Complex x;
  Complex sqrty;
  CalcType calcs;
  Sq sq;
  Complex w1;
  Complex w2;
  Complex A;
  Complex dw1;
  Complex dw2;

 public:
  Complex F;

  template <typename LineData>
  SpeedDependentHardCollisionVoigt(double F0_noshift, LineData ls,
                                   double GD_div_F0, double dZ) noexcept
      : mF0(F0_noshift + dZ + ls.D0 - 1.5 * ls.D2),
        invGD(Constant::sqrt_ln_2 / (GD_div_F0 * mF0)),
        FVC(ls.FVC),
        invc2(1.0 / Complex(ls.G2, ls.D2)),
        dx(Complex(ls.G0 - 1.5 * ls.G2 + FVC, mF0)),
        x(dx * invc2),
        sqrty(invc2 / (2 * invGD)),
        calcs(init(Complex(ls.G2, ls.D2))) {
    calc();
  }

  Complex dFdf(double) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return invGD * invc2 * (dw1 - dw2) /
               (2 * Constant::sqrt_pi * sq.xy * Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return dw1 * Constant::pow2(invGD) /
               (Constant::sqrt_pi * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return invGD * (2 * dw1 * invGD * sq.xy - dw2 * invc2) /
               (2 * Constant::sqrt_pi * sq.xy * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) * (-dw1 * sq.x + Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return Complex(0, 1) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x) * Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdF0() const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (4 * Constant::pow2(invGD) * (-w1 + w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                            2 * invGD * sq.xy + invc2) -
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return -invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0 * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return (4 * Constant::pow2(invGD) * (-w1 + w2) * sq.xy -
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) * (dw1 * sq.x - Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return Complex(0, 1) * Constant::pow2(invc2) * (3 - x) /
               (Constant::pi * Constant::pow3(x) * Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdD0(double dD0dD0) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return -dD0dD0 *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (-dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                             2 * invGD * sq.xy + invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return -dD0dD0 * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0 * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return -dD0dD0 *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return dD0dD0 * Constant::pow2(invc2) *
               (dw1 * sq.x - Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return -Complex(0, dD0dD0) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x) * Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdG0(double dG0dG0) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return Complex(0, dG0dG0) * invGD * invc2 * (dw1 - dw2) /
               (2 * Constant::sqrt_pi * sq.xy * Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return Complex(0, dG0dG0) * dw1 * Constant::pow2(invGD) /
               (Constant::sqrt_pi * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return Complex(0, dG0dG0) * invGD *
               (2 * dw1 * invGD * sq.xy - dw2 * invc2) /
               (2 * Constant::sqrt_pi * sq.xy * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return -dG0dG0 * Constant::pow2(invc2) *
               (Complex(0, 1) * dw1 * sq.x + w1) /
               (Constant::sqrt_pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return -dG0dG0 * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x) * Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdD2(double dD2dD2) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return dD2dD2 *
               (12 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (dw1 * (-Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                                (2 * dx * invc2 + 3) +
                            4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                            6 * invGD * sq.xy -
                            Complex(0, 2 * mF0) * Constant::pow2(invc2) -
                            3 * invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                                (2 * dx * invc2 + 3) +
                            4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                            6 * invGD * sq.xy +
                            Complex(0, 2 * mF0) * Constant::pow2(invc2) +
                            3 * invc2))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return 3 * dD2dD2 * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (2 * Constant::sqrt_pi * mF0 * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return dD2dD2 *
               (12 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (12 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                              (2 * dx * invc2 + 3) +
                          4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                          6 * invGD * sq.xy +
                          Complex(0, 2 * mF0) * Constant::pow2(invc2) +
                          3 * invc2))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return dD2dD2 * Constant::pow2(invc2) *
               (4 * Complex(0, 1) * sq.x * (Constant::sqrt_pi * w1 * sq.x - 1) +
                Constant::sqrt_pi * (-dw1 * sq.x + Complex(0, 1) * w1) *
                    (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return Complex(0, dD2dD2) * Constant::pow2(invc2) *
               (-x * (2 * x - 3) + (x - 3) * (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * Constant::pow3(x) *
                Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdG2(double dG2dG2) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return Complex(0, dG2dG2) * invc2 *
               (dw1 * (-Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                       2 * invGD * invc2 * sq.xy - Constant::pow2(invc2)) +
                dw2 * (Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                       2 * invGD * invc2 * sq.xy + Constant::pow2(invc2))) /
               (4 * Constant::sqrt_pi * invGD * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return -3 * Complex(0, dG2dG2) * dw1 * Constant::pow2(invGD) /
               (2 * Constant::sqrt_pi * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return Complex(0, dG2dG2) *
               (-6 * dw1 * Constant::pow3(invGD) * sq.xy +
                dw2 * invc2 *
                    (Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                     2 * invGD * invc2 * sq.xy + Constant::pow2(invc2))) /
               (4 * Constant::sqrt_pi * invGD * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return dG2dG2 * Constant::pow2(invc2) *
               (4 * sq.x * (Constant::sqrt_pi * w1 * sq.x - 1) +
                Constant::sqrt_pi * (2 * dx * invc2 + 3) *
                    (Complex(0, 1) * dw1 * sq.x + w1)) /
               (2 * Constant::pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return dG2dG2 * Constant::pow2(invc2) *
               (-x * (2 * x - 3) + (x - 3) * (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * Constant::pow3(x) *
                Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  Complex dFdFVC(double dFVCdFVC) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return dFVCdFVC *
               (2 * Constant::pow2(A) * sq.xy +
                Complex(0, Constant::sqrt_pi * invGD) * invc2 * (dw1 - dw2)) /
               (2 * Constant::pi * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::Voigt:
        return dFVCdFVC *
               (Constant::pow2(A) +
                Complex(0, Constant::sqrt_pi * Constant::pow2(invGD)) * dw1) /
               (Constant::pi *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowXandHighY:
        return dFVCdFVC *
               (2 * Constant::pow2(A) * sq.xy +
                Complex(0, Constant::sqrt_pi * invGD) *
                    (2 * dw1 * invGD * sq.xy - dw2 * invc2)) /
               (2 * Constant::pi * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowYandLowX:
        return dFVCdFVC *
               (Constant::pow2(A) * sq.x -
                Constant::sqrt_pi * Constant::pow2(invc2) *
                    (Complex(0, 1) * dw1 * sq.x + w1)) /
               (Constant::pi * sq.x *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowYandHighX:
        return dFVCdFVC *
               (Constant::pow2(A) * Constant::pow3(x) -
                Constant::pow2(invc2) * (x - 3)) /
               (Constant::pi * Constant::pow3(x) *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
    }
    return {};
  }

  Complex dFdH(double dZ) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return -dZ *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (-dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                             2 * invGD * sq.xy + invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::Voigt:
        return -dZ * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0 * Constant::pow2(A * FVC - 1));
      case CalcType::LowXandHighY:
        return -dZ *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy *
                Constant::pow2(A * FVC - 1));
      case CalcType::LowYandLowX:
        return dZ * Constant::pow2(invc2) * (dw1 * sq.x - Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x * Constant::pow2(A * FVC - 1));
      case CalcType::LowYandHighX:
        return -Complex(0, dZ) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x) * Constant::pow2(A * FVC - 1));
    }
    return {};
  }

  template <typename LineData>
  Complex dFdVMR(LineData d) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (Constant::pow2(A) * d.FVC * invGD * mF0 * sq.xy +
                Constant::sqrt_pi *
                    (-4 * Constant::pow2(invGD) * (2 * d.D0 - 3 * d.D2) *
                         (w1 - w2) * sq.xy +
                     Complex(0, 1) * invc2 *
                         (dw1 * (-2 * Constant::pow2(invGD) * mF0 *
                                     (Complex(3 * d.G2 - 2 * d.FVC - 2 * d.G0,
                                              3 * d.D2 - 2 * d.D0) +
                                      2 * dx * invc2 * Complex(d.G2, d.D2)) +
                                 4 * invGD * invc2 * mF0 * sq.xy *
                                     Complex(d.G2, d.D2) -
                                 2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                                 2 * Constant::pow2(invc2) * mF0 *
                                     Complex(d.G2, d.D2) +
                                 invc2 * (2 * d.D0 - 3 * d.D2)) -
                          dw2 * (-2 * Constant::pow2(invGD) * mF0 *
                                     (Complex(3 * d.G2 - 2 * d.FVC - 2 * d.G0,
                                              3 * d.D2 - 2 * d.D0) +
                                      2 * dx * invc2 * Complex(d.G2, d.D2)) -
                                 4 * invGD * invc2 * mF0 * sq.xy *
                                     Complex(d.G2, d.D2) +
                                 2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                                 2 * Constant::pow2(invc2) * mF0 *
                                     Complex(d.G2, d.D2) +
                                 invc2 * (2 * d.D0 - 3 * d.D2)))) /
                    8) /
               (Constant::pi * invGD * mF0 * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::Voigt:
        return (Constant::pow2(A) * d.FVC * mF0 -
                Constant::sqrt_pi * invGD *
                    (Complex(0, invGD) * dw1 *
                         (dx * (2 * d.D0 - 3 * d.D2) -
                          mF0 * Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                        2 * d.D0 - 3 * d.D2)) +
                     w1 * (2 * d.D0 - 3 * d.D2)) /
                    2) /
               (Constant::pi * mF0 *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowXandHighY:
        return (Constant::pow2(A) * d.FVC * invGD * mF0 * sq.xy -
                Constant::sqrt_pi *
                    (4 * Constant::pow2(invGD) * (2 * d.D0 - 3 * d.D2) *
                         (w1 - w2) * sq.xy +
                     Complex(0, 1) *
                         (4 * dw1 * Constant::pow3(invGD) * sq.xy *
                              (dx * (2 * d.D0 - 3 * d.D2) -
                               mF0 * Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                             2 * d.D0 - 3 * d.D2)) +
                          dw2 * invc2 *
                              (2 * Constant::pow2(invGD) * mF0 *
                                   (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                            2 * d.D0 - 3 * d.D2) -
                                    2 * dx * invc2 * Complex(d.G2, d.D2)) -
                               4 * invGD * invc2 * mF0 * sq.xy *
                                   Complex(d.G2, d.D2) +
                               2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                               2 * Constant::pow2(invc2) * mF0 *
                                   Complex(d.G2, d.D2) +
                               invc2 * (2 * d.D0 - 3 * d.D2)))) /
                    8) /
               (Constant::pi * invGD * mF0 * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowYandLowX:
        return (Constant::pow2(A) * d.FVC * sq.x +
                2 * Constant::pow2(invc2) * sq.x * Complex(d.G2, d.D2) *
                    (Constant::sqrt_pi * w1 * sq.x - 1) -
                Constant::sqrt_pi * Constant::pow2(invc2) *
                    (Complex(0, 1) * dw1 * sq.x + w1) *
                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                             2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2)) /
                    2) /
               (Constant::pi * sq.x *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowYandHighX:
        return (2 * Constant::pow2(A) * d.FVC * Constant::pow3(x) -
                Constant::pow2(invc2) * x * (2 * x - 3) * Complex(d.G2, d.D2) -
                Constant::pow2(invc2) * (x - 3) *
                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                             2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * Constant::pow3(x) *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
    }
    return {};
  }

  template <typename LineData>
  Complex dFdT(LineData d, double T) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (Constant::pow2(A) * T * d.FVC * invGD * mF0 * sq.xy *
                    Constant::pow3(Constant::sqrt_ln_2) +
                Constant::sqrt_pi *
                    (-Constant::pow2(invGD) * (w1 - w2) * sq.xy *
                         (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                          Constant::pow3(invGD) * mF0) *
                         Constant::ln_16 +
                     Complex(0, 1) * invc2 *
                         (dw1 *
                              (T * invGD * invc2 * mF0 * sq.xy *
                                   Complex(d.G2, d.D2) * Constant::ln_16 -
                               2 * invGD * sq.xy *
                                   (T * (2 * d.D0 - 3 * d.D2) *
                                        Constant::sqrt_ln_2 -
                                    Constant::pow3(invGD) * mF0) *
                                   Constant::sqrt_ln_2 -
                               (2 * T * Constant::pow2(invGD) * mF0 *
                                    (Complex(3 * d.G2 - 2 * d.FVC - 2 * d.G0,
                                             3 * d.D2 - 2 * d.D0) +
                                     2 * dx * invc2 * Complex(d.G2, d.D2)) *
                                    Constant::sqrt_ln_2 +
                                2 * T * Constant::pow2(invc2) * mF0 *
                                    Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                                invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                             Constant::sqrt_ln_2 +
                                         Constant::pow3(invGD) * mF0)) *
                                   Constant::sqrt_ln_2) +
                          dw2 *
                              (T * invGD * invc2 * mF0 * sq.xy *
                                   Complex(d.G2, d.D2) * Constant::ln_16 +
                               2 * invGD * sq.xy *
                                   (T * (-2 * d.D0 + 3 * d.D2) *
                                        Constant::sqrt_ln_2 +
                                    Constant::pow3(invGD) * mF0) *
                                   Constant::sqrt_ln_2 +
                               (-2 * T * Constant::pow2(invGD) * mF0 *
                                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                             2 * d.D0 - 3 * d.D2) -
                                     2 * dx * invc2 * Complex(d.G2, d.D2)) *
                                    Constant::sqrt_ln_2 +
                                2 * T * Constant::pow2(invc2) * mF0 *
                                    Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                                invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                             Constant::sqrt_ln_2 +
                                         Constant::pow3(invGD) * mF0)) *
                                   Constant::sqrt_ln_2)) *
                         Constant::sqrt_ln_2) /
                    8) /
               (Constant::pi * T * invGD * mF0 * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1) *
                Constant::pow3(Constant::sqrt_ln_2));
      case CalcType::Voigt:
        return (Constant::pow2(A) * T * d.FVC * mF0 * Constant::sqrt_ln_2 -
                Constant::sqrt_pi * invGD *
                    (Complex(0, invGD) * dw1 *
                         (-T * mF0 *
                              Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                      2 * d.D0 - 3 * d.D2) *
                              Constant::sqrt_ln_2 +
                          dx *
                              (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                               Constant::pow3(invGD) * mF0)) +
                     w1 * (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                           Constant::pow3(invGD) * mF0)) /
                    2) /
               (Constant::pi * T * mF0 *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1) *
                Constant::sqrt_ln_2);
      case CalcType::LowXandHighY:
        return (Constant::pow2(A) * T * d.FVC * invGD * mF0 * sq.xy *
                    Constant::pow2(Constant::ln_2) +
                Constant::sqrt_pi *
                    (-Constant::pow2(invGD) * (w1 - w2) * sq.xy *
                         (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                          Constant::pow3(invGD) * mF0) *
                         Constant::ln_16 +
                     Complex(0, 1) *
                         (dw1 * Constant::pow3(invGD) * sq.xy *
                              (T * mF0 *
                                   Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                           2 * d.D0 - 3 * d.D2) *
                                   Constant::sqrt_ln_2 -
                               dx * (T * (2 * d.D0 - 3 * d.D2) *
                                         Constant::sqrt_ln_2 -
                                     Constant::pow3(invGD) * mF0)) *
                              Constant::ln_16 +
                          dw2 * invc2 *
                              (T * invGD * invc2 * mF0 * sq.xy *
                                   Complex(d.G2, d.D2) * Constant::ln_16 +
                               2 * invGD * sq.xy *
                                   (T * (-2 * d.D0 + 3 * d.D2) *
                                        Constant::sqrt_ln_2 +
                                    Constant::pow3(invGD) * mF0) *
                                   Constant::sqrt_ln_2 +
                               (-2 * T * Constant::pow2(invGD) * mF0 *
                                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                                             2 * d.D0 - 3 * d.D2) -
                                     2 * dx * invc2 * Complex(d.G2, d.D2)) *
                                    Constant::sqrt_ln_2 +
                                2 * T * Constant::pow2(invc2) * mF0 *
                                    Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                                invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                             Constant::sqrt_ln_2 +
                                         Constant::pow3(invGD) * mF0)) *
                                   Constant::sqrt_ln_2) *
                              Constant::sqrt_ln_2)) *
                    Constant::sqrt_ln_2 / 8) /
               (Constant::pi * T * invGD * mF0 * sq.xy *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1) *
                Constant::pow2(Constant::ln_2));
      case CalcType::LowYandLowX:
        return (Constant::pow2(A) * d.FVC * sq.x +
                2 * Constant::pow2(invc2) * sq.x * Complex(d.G2, d.D2) *
                    (Constant::sqrt_pi * w1 * sq.x - 1) -
                Constant::sqrt_pi * Constant::pow2(invc2) *
                    (Complex(0, 1) * dw1 * sq.x + w1) *
                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                             2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2)) /
                    2) /
               (Constant::pi * sq.x *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
      case CalcType::LowYandHighX:
        return (2 * Constant::pow2(A) * d.FVC * Constant::pow3(x) -
                Constant::pow2(invc2) * x * (2 * x - 3) * Complex(d.G2, d.D2) -
                Constant::pow2(invc2) * (x - 3) *
                    (Complex(2 * d.FVC + 2 * d.G0 - 3 * d.G2,
                             2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * Constant::pow3(x) *
                (Constant::pow2(A) * Constant::pow2(FVC) - 2 * A * FVC + 1));
    }
    return {};
  }

  static constexpr Complex dFdETA(double) noexcept { return 0; }
  static constexpr Complex dFdDV(double) noexcept { return 0; }
  static constexpr Complex dFdY(double) noexcept { return 0; }
  static constexpr Complex dFdG(double) noexcept { return 0; }

  double F0() const noexcept { return mF0; }

  Complex &operator()(double f) noexcept {
    reinterpret_cast<double(&)[2]>(dx)[1] = mF0 - f;
    x = dx * invc2;
    update_calcs();
    calc();
    return F;
  }

 private:
  CalcType init(const Complex c2) const noexcept {
    if (abs_squared(c2) == 0)
      return CalcType::Voigt;
    else if (abs_squared(x) <= 9e-16 * abs_squared(sqrty * sqrty))
      return CalcType::LowXandHighY;
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)) and
             abs_squared(std::sqrt(x)) <= 16.e6)
      return CalcType::LowYandLowX;  // Weird case, untested
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)))
      return CalcType::LowYandHighX;
    else
      return CalcType::Full;
  }

  void update_calcs() noexcept {
    if (calcs not_eq CalcType::Voigt) calcs = init(Complex(1, 1));
  }

  void calc() noexcept {
    switch (calcs) {
      case CalcType::Full:
        sq.xy = std::sqrt(x + sqrty * sqrty);
        w1 = Faddeeva::w(Complex(0, 1) * (sq.xy - sqrty));
        w2 = Faddeeva::w(Complex(0, 1) * (sq.xy + sqrty));
        A = Constant::sqrt_pi * invGD * (w1 - w2);
        F = Constant::inv_pi * A / (1 - FVC * A);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy - sqrty) * w1);
        dw2 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy + sqrty) * w2);
        break;
      case CalcType::Voigt:
        w1 = Faddeeva::w(Complex(0, 1) * dx * invGD);
        A = Constant::sqrt_pi * invGD * w1;
        F = Constant::inv_pi * A / (1 - FVC * A);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - dx * invGD * w1);
        break;
      case CalcType::LowXandHighY:
        sq.xy = std::sqrt(x + sqrty * sqrty);
        w1 = Faddeeva::w(Complex(0, 1) * dx * invGD);
        w2 = Faddeeva::w(Complex(0, 1) * (sq.xy + sqrty));
        A = Constant::sqrt_pi * invGD * (w1 - w2);
        F = Constant::inv_pi * A / (1 - FVC * A);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - dx * invGD * w1);
        dw2 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy + sqrty) * w2);
        break;
      case CalcType::LowYandLowX:
        sq.x = std::sqrt(x);
        w1 = Faddeeva::w(Complex(0, 1) * sq.x);
        A = 2 * invc2 * (1 - Constant::sqrt_pi * sq.x * w1);
        F = Constant::inv_pi * A / (1 - FVC * A);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - sq.x * w1);
        break;
      case CalcType::LowYandHighX:
        A = invc2 * (1 / x - 1.5 / Constant::pow2(x));
        F = Constant::inv_pi * A / (1 - FVC * A);
        break;
    }
  }
};  // SpeedDependentHardCollisionVoigt

class SpeedDependentVoigt {
  enum class CalcType : char {
    Voigt,
    LowXandHighY,
    LowYandLowX,
    LowYandHighX,
    Full
  };
  union Sq {
    Complex x;
    Complex xy;
    Sq() noexcept : xy(0) {}
  };

  double mF0;
  double invGD;
  Complex invc2;
  Complex dx;
  Complex x;
  Complex sqrty;
  CalcType calcs;
  Sq sq;
  Complex w1;
  Complex w2;
  Complex dw1;
  Complex dw2;

 public:
  Complex F;

  template <typename LineData>
  SpeedDependentVoigt(double F0_noshift, LineData ls, double GD_div_F0,
                      double dZ) noexcept
      : mF0(F0_noshift + dZ + ls.D0 - 1.5 * ls.D2),
        invGD(Constant::sqrt_ln_2 / (GD_div_F0 * mF0)),
        invc2(1.0 / Complex(ls.G2, ls.D2)),
        dx(Complex(ls.G0 - 1.5 * ls.G2, mF0)),
        x(dx * invc2),
        sqrty(invc2 / (2 * invGD)),
        calcs(init(Complex(ls.G2, ls.D2))) {
    calc();
  }

  Complex dFdf(double) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return invGD * invc2 * (dw1 - dw2) / (2 * Constant::sqrt_pi * sq.xy);
      case CalcType::Voigt:
        return dw1 * Constant::pow2(invGD) * Constant::inv_sqrt_pi;
      case CalcType::LowXandHighY:
        return dw1 * Constant::pow2(invGD) * Constant::inv_sqrt_pi -
               dw2 * invGD * invc2 / (2 * Constant::sqrt_pi * sq.xy);
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) * (-dw1 * sq.x + Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x);
      case CalcType::LowYandHighX:
        return Complex(0, 1) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdF0() const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (4 * Constant::pow2(invGD) * (-w1 + w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                            2 * invGD * sq.xy + invc2) -
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::Voigt:
        return -invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0);
      case CalcType::LowXandHighY:
        return (4 * Constant::pow2(invGD) * (-w1 + w2) * sq.xy -
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) * (dw1 * sq.x - Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x);
      case CalcType::LowYandHighX:
        return Complex(0, 1) * Constant::pow2(invc2) * (3 - x) /
               (Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdD0(double dD0dD0) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return -dD0dD0 *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (-dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                             2 * invGD * sq.xy + invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::Voigt:
        return -dD0dD0 * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0);
      case CalcType::LowXandHighY:
        return -dD0dD0 *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::LowYandLowX:
        return dD0dD0 * Constant::pow2(invc2) *
               (dw1 * sq.x - Complex(0, 1) * w1) / (Constant::sqrt_pi * sq.x);
      case CalcType::LowYandHighX:
        return -Complex(0, dD0dD0) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdG0(double dG0dG0) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return Complex(0, dG0dG0) * invGD * invc2 * (dw1 - dw2) /
               (2 * Constant::sqrt_pi * sq.xy);
      case CalcType::Voigt:
        return Complex(0, dG0dG0) * dw1 * Constant::pow2(invGD) *
               Constant::inv_sqrt_pi;
      case CalcType::LowXandHighY:
        return Complex(0, dG0dG0) * invGD *
               (2 * dw1 * invGD * sq.xy - dw2 * invc2) /
               (2 * Constant::sqrt_pi * sq.xy);
      case CalcType::LowYandLowX:
        return -dG0dG0 * Constant::pow2(invc2) *
               (Complex(0, 1) * dw1 * sq.x + w1) / (Constant::sqrt_pi * sq.x);
      case CalcType::LowYandHighX:
        return -dG0dG0 * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdD2(double dD2dD2) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return dD2dD2 *
               (12 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (dw1 * (-Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                                (2 * dx * invc2 + 3) +
                            4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                            6 * invGD * sq.xy -
                            Complex(0, 2 * mF0) * Constant::pow2(invc2) -
                            3 * invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                                (2 * dx * invc2 + 3) +
                            4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                            6 * invGD * sq.xy +
                            Complex(0, 2 * mF0) * Constant::pow2(invc2) +
                            3 * invc2))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::Voigt:
        return 3 * dD2dD2 * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (2 * Constant::sqrt_pi * mF0);
      case CalcType::LowXandHighY:
        return dD2dD2 *
               (12 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (12 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) *
                              (2 * dx * invc2 + 3) +
                          4 * Complex(0, invGD) * invc2 * mF0 * sq.xy +
                          6 * invGD * sq.xy +
                          Complex(0, 2 * mF0) * Constant::pow2(invc2) +
                          3 * invc2))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::LowYandLowX:
        return dD2dD2 * Constant::pow2(invc2) *
               (4 * Complex(0, 1) * sq.x * (Constant::sqrt_pi * w1 * sq.x - 1) -
                Constant::sqrt_pi * (dw1 * sq.x - Complex(0, 1) * w1) *
                    (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * sq.x);
      case CalcType::LowYandHighX:
        return Complex(0, dD2dD2) * Constant::pow2(invc2) *
               (-x * (2 * x - 3) + (x - 3) * (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdG2(double dG2dG2) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return Complex(0, dG2dG2) * invc2 *
               (dw1 * (-Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                       2 * invGD * invc2 * sq.xy - Constant::pow2(invc2)) +
                dw2 * (Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                       2 * invGD * invc2 * sq.xy + Constant::pow2(invc2))) /
               (4 * Constant::sqrt_pi * invGD * sq.xy);
      case CalcType::Voigt:
        return -3 * Complex(0, dG2dG2) * dw1 * Constant::pow2(invGD) /
               (2 * Constant::sqrt_pi);
      case CalcType::LowXandHighY:
        return Complex(0, dG2dG2) *
               (-6 * dw1 * Constant::pow3(invGD) * sq.xy +
                dw2 * invc2 *
                    (Constant::pow2(invGD) * (2 * dx * invc2 + 3) +
                     2 * invGD * invc2 * sq.xy + Constant::pow2(invc2))) /
               (4 * Constant::sqrt_pi * invGD * sq.xy);
      case CalcType::LowYandLowX:
        return dG2dG2 * Constant::pow2(invc2) *
               (4 * sq.x * (Constant::sqrt_pi * w1 * sq.x - 1) +
                Constant::sqrt_pi * (2 * dx * invc2 + 3) *
                    (Complex(0, 1) * dw1 * sq.x + w1)) /
               (2 * Constant::pi * sq.x);
      case CalcType::LowYandHighX:
        return dG2dG2 * Constant::pow2(invc2) *
               (-x * (2 * x - 3) + (x - 3) * (2 * dx * invc2 + 3)) /
               (2 * Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  Complex dFdH(double dZ) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return -dZ *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) * invc2 *
                    (-dw1 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) -
                             2 * invGD * sq.xy + invc2) +
                     dw2 * (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                            2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::Voigt:
        return -dZ * invGD *
               (Complex(0, invGD) * dw1 * (dx - Complex(0, mF0)) + w1) /
               (Constant::sqrt_pi * mF0);
      case CalcType::LowXandHighY:
        return -dZ *
               (4 * Constant::pow2(invGD) * (w1 - w2) * sq.xy +
                Complex(0, 1) *
                    (4 * dw1 * Constant::pow3(invGD) * (dx - Complex(0, mF0)) *
                         sq.xy +
                     dw2 * invc2 *
                         (Complex(0, 2 * mF0 * Constant::pow2(invGD)) +
                          2 * invGD * sq.xy + invc2))) /
               (4 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::LowYandLowX:
        return dZ * Constant::pow2(invc2) * (dw1 * sq.x - Complex(0, 1) * w1) /
               (Constant::sqrt_pi * sq.x);
      case CalcType::LowYandHighX:
        return -Complex(0, dZ) * Constant::pow2(invc2) * (x - 3) /
               (Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  template <typename LineData>
  Complex dFdVMR(LineData d) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (-4 * Constant::pow2(invGD) * (2 * d.D0 - 3 * d.D2) * (w1 - w2) *
                    sq.xy +
                Complex(0, 1) * invc2 *
                    (dw1 * (-2 * Constant::pow2(invGD) * mF0 *
                                (Complex(3 * d.G2 - 2 * d.G0,
                                         3 * d.D2 - 2 * d.D0) +
                                 2 * dx * invc2 * Complex(d.G2, d.D2)) +
                            4 * invGD * invc2 * mF0 * sq.xy *
                                Complex(d.G2, d.D2) -
                            2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                            2 * Constant::pow2(invc2) * mF0 *
                                Complex(d.G2, d.D2) +
                            invc2 * (2 * d.D0 - 3 * d.D2)) -
                     dw2 * (-2 * Constant::pow2(invGD) * mF0 *
                                (Complex(3 * d.G2 - 2 * d.G0,
                                         3 * d.D2 - 2 * d.D0) +
                                 2 * dx * invc2 * Complex(d.G2, d.D2)) -
                            4 * invGD * invc2 * mF0 * sq.xy *
                                Complex(d.G2, d.D2) +
                            2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                            2 * Constant::pow2(invc2) * mF0 *
                                Complex(d.G2, d.D2) +
                            invc2 * (2 * d.D0 - 3 * d.D2)))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::Voigt:
        return -invGD *
               (Complex(0, invGD) * dw1 *
                    (dx * (2 * d.D0 - 3 * d.D2) -
                     mF0 * Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2)) +
                w1 * (2 * d.D0 - 3 * d.D2)) /
               (2 * Constant::sqrt_pi * mF0);
      case CalcType::LowXandHighY:
        return -(4 * Constant::pow2(invGD) * (2 * d.D0 - 3 * d.D2) * (w1 - w2) *
                     sq.xy +
                 Complex(0, 1) *
                     (4 * dw1 * Constant::pow3(invGD) * sq.xy *
                          (dx * (2 * d.D0 - 3 * d.D2) -
                           mF0 * Complex(2 * d.G0 - 3 * d.G2,
                                         2 * d.D0 - 3 * d.D2)) +
                      dw2 * invc2 *
                          (2 * Constant::pow2(invGD) * mF0 *
                               (Complex(2 * d.G0 - 3 * d.G2,
                                        2 * d.D0 - 3 * d.D2) -
                                2 * dx * invc2 * Complex(d.G2, d.D2)) -
                           4 * invGD * invc2 * mF0 * sq.xy *
                               Complex(d.G2, d.D2) +
                           2 * invGD * (2 * d.D0 - 3 * d.D2) * sq.xy -
                           2 * Constant::pow2(invc2) * mF0 *
                               Complex(d.G2, d.D2) +
                           invc2 * (2 * d.D0 - 3 * d.D2)))) /
               (8 * Constant::sqrt_pi * invGD * mF0 * sq.xy);
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) *
               (4 * sq.x * Complex(d.G2, d.D2) *
                    (Constant::sqrt_pi * w1 * sq.x - 1) -
                Constant::sqrt_pi * (Complex(0, 1) * dw1 * sq.x + w1) *
                    (Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * sq.x);
      case CalcType::LowYandHighX:
        return -Constant::pow2(invc2) *
               (x * (2 * x - 3) * Complex(d.G2, d.D2) +
                (x - 3) * (Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2) -
                           2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  template <typename LineData>
  Complex dFdT(LineData d, double T) const noexcept {
    switch (calcs) {
      case CalcType::Full:
        return (-Constant::pow2(invGD) * (w1 - w2) * sq.xy *
                    (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                     Constant::pow3(invGD) * mF0) *
                    Constant::ln_16 +
                Complex(0, 1) * invc2 *
                    (dw1 *
                         (T * invGD * invc2 * mF0 * sq.xy *
                              Complex(d.G2, d.D2) * Constant::ln_16 -
                          2 * invGD * sq.xy *
                              (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                               Constant::pow3(invGD) * mF0) *
                              Constant::sqrt_ln_2 -
                          (2 * T * Constant::pow2(invGD) * mF0 *
                               (Complex(3 * d.G2 - 2 * d.G0,
                                        3 * d.D2 - 2 * d.D0) +
                                2 * dx * invc2 * Complex(d.G2, d.D2)) *
                               Constant::sqrt_ln_2 +
                           2 * T * Constant::pow2(invc2) * mF0 *
                               Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                           invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                        Constant::sqrt_ln_2 +
                                    Constant::pow3(invGD) * mF0)) *
                              Constant::sqrt_ln_2) +
                     dw2 * (T * invGD * invc2 * mF0 * sq.xy *
                                Complex(d.G2, d.D2) * Constant::ln_16 +
                            2 * invGD * sq.xy *
                                (T * (-2 * d.D0 + 3 * d.D2) *
                                     Constant::sqrt_ln_2 +
                                 Constant::pow3(invGD) * mF0) *
                                Constant::sqrt_ln_2 +
                            (-2 * T * Constant::pow2(invGD) * mF0 *
                                 (Complex(2 * d.G0 - 3 * d.G2,
                                          2 * d.D0 - 3 * d.D2) -
                                  2 * dx * invc2 * Complex(d.G2, d.D2)) *
                                 Constant::sqrt_ln_2 +
                             2 * T * Constant::pow2(invc2) * mF0 *
                                 Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                             invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                          Constant::sqrt_ln_2 +
                                      Constant::pow3(invGD) * mF0)) *
                                Constant::sqrt_ln_2)) *
                    Constant::sqrt_ln_2) /
               (8 * Constant::sqrt_pi * T * invGD * mF0 * sq.xy *
                Constant::pow3(Constant::sqrt_ln_2));
      case CalcType::Voigt:
        return -invGD *
               (-Complex(0, invGD) * dw1 *
                    (T * mF0 *
                         Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2) *
                         Constant::sqrt_ln_2 -
                     dx * (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                           Constant::pow3(invGD) * mF0)) +
                w1 * (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                      Constant::pow3(invGD) * mF0)) /
               (2 * Constant::sqrt_pi * T * mF0 * Constant::sqrt_ln_2);
      case CalcType::LowXandHighY:
        return (-Constant::pow2(invGD) * (w1 - w2) * sq.xy *
                    (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                     Constant::pow3(invGD) * mF0) *
                    Constant::ln_16 +
                Complex(0, 1) *
                    (dw1 * Constant::pow3(invGD) * sq.xy *
                         (T * mF0 *
                              Complex(2 * d.G0 - 3 * d.G2,
                                      2 * d.D0 - 3 * d.D2) *
                              Constant::sqrt_ln_2 -
                          dx *
                              (T * (2 * d.D0 - 3 * d.D2) * Constant::sqrt_ln_2 -
                               Constant::pow3(invGD) * mF0)) *
                         Constant::ln_16 +
                     dw2 * invc2 *
                         (T * invGD * invc2 * mF0 * sq.xy *
                              Complex(d.G2, d.D2) * Constant::ln_16 +
                          2 * invGD * sq.xy *
                              (T * (-2 * d.D0 + 3 * d.D2) *
                                   Constant::sqrt_ln_2 +
                               Constant::pow3(invGD) * mF0) *
                              Constant::sqrt_ln_2 +
                          (-2 * T * Constant::pow2(invGD) * mF0 *
                               (Complex(2 * d.G0 - 3 * d.G2,
                                        2 * d.D0 - 3 * d.D2) -
                                2 * dx * invc2 * Complex(d.G2, d.D2)) *
                               Constant::sqrt_ln_2 +
                           2 * T * Constant::pow2(invc2) * mF0 *
                               Complex(d.G2, d.D2) * Constant::sqrt_ln_2 +
                           invc2 * (T * (-2 * d.D0 + 3 * d.D2) *
                                        Constant::sqrt_ln_2 +
                                    Constant::pow3(invGD) * mF0)) *
                              Constant::sqrt_ln_2) *
                         Constant::sqrt_ln_2)) /
               (8 * Constant::sqrt_pi * T * invGD * mF0 * sq.xy *
                Constant::pow3(Constant::sqrt_ln_2));
      case CalcType::LowYandLowX:
        return Constant::pow2(invc2) *
               (4 * sq.x * Complex(d.G2, d.D2) *
                    (Constant::sqrt_pi * w1 * sq.x - 1) -
                Constant::sqrt_pi * (Complex(0, 1) * dw1 * sq.x + w1) *
                    (Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2) -
                     2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * sq.x);
      case CalcType::LowYandHighX:
        return -Constant::pow2(invc2) *
               (x * (2 * x - 3) * Complex(d.G2, d.D2) +
                (x - 3) * (Complex(2 * d.G0 - 3 * d.G2, 2 * d.D0 - 3 * d.D2) -
                           2 * dx * invc2 * Complex(d.G2, d.D2))) /
               (2 * Constant::pi * Constant::pow3(x));
    }
    return {};
  }

  static constexpr Complex dFdETA(double) noexcept { return 0; }
  static constexpr Complex dFdFVC(double) noexcept { return 0; }
  static constexpr Complex dFdDV(double) noexcept { return 0; }
  static constexpr Complex dFdY(double) noexcept { return 0; }
  static constexpr Complex dFdG(double) noexcept { return 0; }

  double F0() const noexcept { return mF0; }

  Complex &operator()(double f) noexcept {
    reinterpret_cast<double(&)[2]>(dx)[1] = mF0 - f;
    x = dx * invc2;
    update_calcs();
    calc();
    return F;
  }

 private:
  CalcType init(const Complex c2) const noexcept {
    if (abs_squared(c2) == 0)
      return CalcType::Voigt;
    else if (abs_squared(x) <= 9e-16 * abs_squared(sqrty * sqrty))
      return CalcType::LowXandHighY;
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)) and
             abs_squared(std::sqrt(x)) <= 16.e6)
      return CalcType::LowYandLowX;  // Weird case, untested
    else if ((abs_squared(sqrty * sqrty) <= 1.e-30 * abs_squared(x)))
      return CalcType::LowYandHighX;
    else
      return CalcType::Full;
  }

  void update_calcs() noexcept {
    if (calcs not_eq CalcType::Voigt) calcs = init(Complex(1, 1));
  }

  void calc() noexcept {
    switch (calcs) {
      case CalcType::Full:
        sq.xy = std::sqrt(x + sqrty * sqrty);
        w1 = Faddeeva::w(Complex(0, 1) * (sq.xy - sqrty));
        w2 = Faddeeva::w(Complex(0, 1) * (sq.xy + sqrty));
        F = Constant::inv_sqrt_pi * invGD * (w1 - w2);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy - sqrty) * w1);
        dw2 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy + sqrty) * w2);
        break;
      case CalcType::Voigt:
        w1 = Faddeeva::w(Complex(0, 1) * dx * invGD);
        F = Constant::inv_sqrt_pi * invGD * w1;
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - dx * invGD * w1);
        break;
      case CalcType::LowXandHighY:
        sq.xy = std::sqrt(x + sqrty * sqrty);
        w1 = Faddeeva::w(Complex(0, 1) * dx * invGD);
        w2 = Faddeeva::w(Complex(0, 1) * (sq.xy + sqrty));
        F = Constant::inv_sqrt_pi * invGD * (w1 - w2);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - dx * invGD * w1);
        dw2 = Complex(0, 2) * (Constant::inv_sqrt_pi - (sq.xy + sqrty) * w2);
        break;
      case CalcType::LowYandLowX:
        sq.x = std::sqrt(x);
        w1 = Faddeeva::w(Complex(0, 1) * sq.x);
        F = 2 * Constant::inv_pi * invc2 * (1 - Constant::sqrt_pi * sq.x * w1);
        dw1 = Complex(0, 2) * (Constant::inv_sqrt_pi - sq.x * w1);
        break;
      case CalcType::LowYandHighX:
        F = Constant::inv_pi * invc2 * (1 / x - 1.5 / Constant::pow2(x));
        break;
    }
  }
};  // SpeedDependentVoigt

class Voigt {
  double mF0;
  double invGD;
  Complex z;

 public:
  Complex F;

 private:
  Complex dF;

 public:
  template <typename LineData>
  Voigt(double F0_noshift, LineData ls, double GD_div_F0, double dZ) noexcept
      : mF0(F0_noshift + dZ + ls.D0 + ls.DV),
        invGD(Constant::sqrt_ln_2 / (GD_div_F0 * mF0)),
        z(invGD * Complex(-mF0, ls.G0)),
        F(Constant::inv_sqrt_pi * invGD * Faddeeva::w(z)),
        dF(2 * (Complex(0, invGD * Constant::inv_pi) - z * F)) {}

  Complex dFdf(double) const noexcept { return dF; }
  Complex dFdF0() const noexcept { return -F / mF0 - dF; }
  Complex dFdDV(double d) const noexcept { return d * dFdF0(); }
  Complex dFdD0(double d) const noexcept { return d * dFdF0(); }
  Complex dFdG0(double d) const noexcept { return Complex(0, d) * dF; }
  Complex dFdH(double dZ) const noexcept { return -dZ * dF; }
  template <typename LineData>
  Complex dFdVMR(LineData d) const noexcept {
    return Complex(-d.D0 - d.DV, d.G0) * dF;
  }
  template <typename LineData>
  Complex dFdT(LineData d, double T) const noexcept {
    return dFdVMR(d) + Constant::pow2(invGD) * (z * dF + invGD * F) /
                           (2 * Constant::sqrt_ln_2 * T);
  }
  static constexpr Complex dFdETA(double) noexcept { return 0; }
  static constexpr Complex dFdFVC(double) noexcept { return 0; }
  static constexpr Complex dFdD2(double) noexcept { return 0; }
  static constexpr Complex dFdG2(double) { return 0; }
  static constexpr Complex dFdY(double) noexcept { return 0; }
  static constexpr Complex dFdG(double) noexcept { return 0; }

  double F0() const noexcept { return mF0; }

  Complex &operator()(double f) noexcept {
    reinterpret_cast<double(&)[2]>(z)[0] = invGD * (f - mF0);
    F = Constant::inv_sqrt_pi * invGD * Faddeeva::w(z);
    dF = 2 * invGD * (Complex(0, Constant::inv_pi * invGD) - z * F);
    return F;
  }
};  // Voigt

class Lorentz {
  double mF0;
  Complex z;

 public:
  Complex F;

 private:
  Complex dF;

 public:
  template <typename LineData>
  constexpr Lorentz(double F0_noshift, LineData ls, double, double dZ) noexcept
      : mF0(F0_noshift + ls.D0 + ls.DV + dZ),
        z(Constant::pi * ls.G0, Constant::pi * mF0),
        F(1.0 / z),
        dF(-Constant::pi * Constant::pow2(F)) {}

  template <typename LineData>
  constexpr Complex dFdVMR(LineData d) const noexcept {
    return Complex(d.G0, d.D0 + d.DV) * dF;
  }
  template <typename LineData>
  constexpr Complex dFdT(LineData d, double) const noexcept {
    return dFdVMR(d);
  }
  constexpr Complex dFdf(double) const noexcept { return Complex(0, -1) * dF; }
  constexpr Complex dFdF0() const noexcept { return Complex(0, 1) * dF; }
  constexpr Complex dFdDV(double d) const noexcept { return d * dFdF0(); }
  constexpr Complex dFdD0(double d) const noexcept { return d * dFdF0(); }
  constexpr Complex dFdH(double dZ) const noexcept {
    return Complex(0, dZ) * dF;
  }
  constexpr Complex dFdG0(double d) const noexcept { return d * dF; }
  static constexpr Complex dFdY(double) noexcept { return 0; }
  static constexpr Complex dFdG(double) noexcept { return 0; }
  static constexpr Complex dFdD2(double) noexcept { return 0; }
  static constexpr Complex dFdG2(double) { return 0; }
  static constexpr Complex dFdETA(double) noexcept { return 0; }
  static constexpr Complex dFdFVC(double) noexcept { return 0; }

  constexpr double F0() const noexcept { return mF0; }

  Complex &operator()(double f) noexcept {
    reinterpret_cast<double(&)[2]>(z)[1] = Constant::pi * (mF0 - f);
    F = 1.0 / z;
    dF = -Constant::pi * Constant::pow2(F);
    return F;
  }
};  // Lorentz

class Doppler {
  double mF0;
  double invGD;
  double x;

 public:
  double F;

  template <typename LineData>
  Doppler(double F0_noshift, LineData, double GD_div_F0, double dZ) noexcept
      : mF0(F0_noshift + dZ),
        invGD(Constant::sqrt_ln_2 / (GD_div_F0 * mF0)),
        x(-mF0 * invGD),
        F(invGD * Constant::inv_sqrt_pi * std::exp(-Constant::pow2(x))) {}

  template <typename LineData>
  double dFdT(LineData, double T) const noexcept {
    return -(Constant::pow2(x) - 0.5) * Constant::pow3(invGD) /
           (Constant::sqrt_ln_2 * T) * F;
  }
  double dFdf(double) const noexcept { return -2 * invGD * F * x; }
  double dFdF0() const noexcept {
    return F * Constant::pow2(x) * (2 / mF0) + F * x * 2 * (invGD - 1 / mF0);
  }
  double dFdH(double dZ) const noexcept { return dZ * dFdF0(); }
  static constexpr double dFdFVC(double) noexcept { return 0; }
  static constexpr double dFdETA(double) noexcept { return 0; }
  template <typename LineData>
  static constexpr double dFdVMR(LineData) noexcept {
    return 0;
  }
  static constexpr double dFdDV(double) noexcept { return 0; }
  static constexpr double dFdD0(double) noexcept { return 0; }
  static constexpr double dFdG0(double) noexcept { return 0; }
  static constexpr double dFdY(double) noexcept { return 0; }
  static constexpr double dFdG(double) noexcept { return 0; }
  static constexpr double dFdD2(double) noexcept { return 0; }
  static constexpr double dFdG2(double) noexcept { return 0; }

  double F0() const noexcept { return mF0; }

  double &operator()(double f) noexcept {
    x = (f - mF0) * invGD;
    F = invGD * Constant::inv_sqrt_pi * std::exp(-Constant::pow2(x));
    return F;
  }
};  // Doppler
}  // namespace Base
}  // namespace LineShape
}  // namespace Absorption

#endif
