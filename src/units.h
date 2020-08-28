#ifndef units_h
#define units_h

// Collection of units that are scalar in some sense
// note: Each class going in here must be able to convert
// to and from its own various types via constructors

#include "constants.h"
#include "enums.h"
#include "species.h"

#define SCALAR(Scalar)                                                         \
 private:                                                                      \
  double val;                                                                  \
                                                                               \
 public:                                                                       \
  constexpr Scalar(double v = 0) noexcept : val(v) {}                          \
  constexpr Scalar(Scalar&&) noexcept = default;                               \
  constexpr double value() const noexcept { return val; }                      \
  constexpr Scalar operator-() const noexcept { return Scalar{-val}; }         \
  constexpr Scalar operator+() const noexcept { return Scalar{val}; }          \
  template <class T>                                                           \
  constexpr T operator+(T x) const noexcept {                                  \
    return val + x;                                                            \
  }                                                                            \
  template <class T>                                                           \
  constexpr T operator-(T x) const noexcept {                                  \
    return val - x;                                                            \
  }                                                                            \
  template <class T>                                                           \
  constexpr double operator*(T x) const noexcept {                             \
    return val * x;                                                            \
  }                                                                            \
  template <class T>                                                           \
  constexpr double operator/(T x) const noexcept {                             \
    return val / x;                                                            \
  }                                                                            \
  constexpr double operator+(int x) const noexcept { return val + double(x); } \
  constexpr double operator-(int x) const noexcept { return val - double(x); } \
  constexpr double operator*(int x) const noexcept { return val * double(x); } \
  constexpr double operator/(int x) const noexcept { return val / double(x); } \
  friend constexpr Scalar operator+(int a, Scalar b) noexcept {                \
    return Scalar{double(a) + b.val};                                          \
  }                                                                            \
  friend constexpr Scalar operator-(int a, Scalar b) noexcept {                \
    return Scalar{double(a) - b.val};                                          \
  }                                                                            \
  friend constexpr double operator*(int a, Scalar b) noexcept {                \
    return Scalar{double(a) * b.val};                                          \
  }                                                                            \
  friend constexpr double operator/(int a, Scalar b) noexcept {                \
    return Scalar{double(a) / b.val};                                          \
  }                                                                            \
  friend constexpr Scalar operator+(double a, Scalar b) noexcept {             \
    return Scalar{a + b.val};                                                  \
  }                                                                            \
  friend constexpr Scalar operator-(double a, Scalar b) noexcept {             \
    return Scalar{a - b.val};                                                  \
  }                                                                            \
  friend constexpr double operator*(double a, Scalar b) noexcept {             \
    return Scalar{a * b.val};                                                  \
  }                                                                            \
  friend constexpr double operator/(double a, Scalar b) noexcept {             \
    return Scalar{a / b.val};                                                  \
  }                                                                            \
  constexpr Scalar& operator=(Scalar b) noexcept {                             \
    val = b.val;                                                               \
    return *this;                                                              \
  }                                                                            \
  constexpr Scalar& operator=(double b) noexcept {                             \
    val = b;                                                                   \
    return *this;                                                              \
  }                                                                            \
  Scalar& operator*=(double x) noexcept {                                      \
    val *= x;                                                                  \
    return *this;                                                              \
  }                                                                            \
  Scalar& operator+=(Scalar x) noexcept {                                      \
    val += x.val;                                                              \
    return *this;                                                              \
  }                                                                            \
  Scalar& operator-=(Scalar x) noexcept {                                      \
    val -= x.val;                                                              \
    return *this;                                                              \
  }                                                                            \
  double* operator&() noexcept { return &val; }                                \
  const double* operator&() const noexcept { return &val; }                    \
  friend std::ostream& operator<<(std::ostream& os, Scalar s) {                \
    return os << s.val;                                                        \
  }                                                                            \
  friend std::istream& operator>>(std::istream& is, Scalar& s) {               \
    return is >> s.val;                                                        \
  }                                                                            \
  constexpr bool operator>(Scalar b) const noexcept { return val > b.val; }    \
  constexpr bool operator>=(Scalar b) const noexcept { return val >= b.val; }  \
  constexpr bool operator<(Scalar b) const noexcept { return val < b.val; }    \
  constexpr bool operator<=(Scalar b) const noexcept { return val <= b.val; }  \
  constexpr bool operator==(Scalar b) const noexcept { return val == b.val; }  \
  constexpr bool operator!=(Scalar b) const noexcept { return val != b.val; }  \
  template <class T>                                                           \
  constexpr bool operator>(T b) const noexcept {                               \
    return *this > Scalar{b};                                                  \
  }                                                                            \
  template <class T>                                                           \
  constexpr bool operator>=(T b) const noexcept {                              \
    return *this >= Scalar{b};                                                 \
  }                                                                            \
  template <class T>                                                           \
  constexpr bool operator<(T b) const noexcept {                               \
    return *this < Scalar{b};                                                  \
  }                                                                            \
  template <class T>                                                           \
  constexpr bool operator<=(T b) const noexcept {                              \
    return *this <= Scalar{b};                                                 \
  }                                                                            \
  template <class T>                                                           \
  constexpr bool operator==(T b) const noexcept {                              \
    return *this == Scalar{b};                                                 \
  }                                                                            \
  template <class T>                                                           \
  constexpr bool operator!=(T b) const noexcept {                              \
    return *this != Scalar{b};                                                 \
  }                                                                            \
  constexpr operator double() const noexcept { return val; }                   \
                                                                               \
 private:

ENUMCLASS(PressureType, char, Pa, Atm, Bar,
          Torr)  // PressureType

template <PressureType X>
class Pressure final {
  SCALAR(Pressure)
  void pa2self() noexcept {
    if constexpr (X == PressureType::Pa) {
    }
    if constexpr (X == PressureType::Atm) {
      val = Conversion::pa2atm(val);
    }
    if constexpr (X == PressureType::Torr) {
      val = Conversion::pa2torr(val);
    }
    if constexpr (X == PressureType::Bar) {
      val = Conversion::pa2bar(val);
    }
  }

 public:
  constexpr Pressure(const Pressure<PressureType::Pa>& p) noexcept
      : val(p.value()) {
    pa2self();
  }
  constexpr Pressure(const Pressure<PressureType::Atm>& p) noexcept
      : val(Conversion::atm2pa(p.value())) {
    pa2self();
  }
  constexpr Pressure(const Pressure<PressureType::Bar>& p) noexcept
      : val(Conversion::bar2pa(p.value())) {
    pa2self();
  }
  constexpr Pressure(const Pressure<PressureType::Torr>& p) noexcept
      : val(Conversion::torr2pa(p.value())) {
    pa2self();
  }
};  // Pressure

ENUMCLASS(TemperatureType, char, K, C, F,
          eV)  // TemperatureType

template <TemperatureType X>
class Temperature final {
  SCALAR(Temperature)
  void k2self() noexcept {
    if constexpr (X == TemperatureType::K) {
    }
    if constexpr (X == TemperatureType::C) {
      val = Conversion::k2c(val);
    }
    if constexpr (X == TemperatureType::F) {
      val = Conversion::k2f(val);
    }
    if constexpr (X == TemperatureType::eV) {
      val = Conversion::ev2k(val);
    }
  }

 public:
  constexpr Temperature(const Temperature<TemperatureType::K>& t) noexcept
      : val(t.value()) {
    k2self();
  }
  constexpr Temperature(const Temperature<TemperatureType::C>& t) noexcept
      : val(Conversion::c2k(t.value())) {
    k2self();
  }
  constexpr Temperature(const Temperature<TemperatureType::F>& t) noexcept
      : val(Conversion::f2k(t.value())) {
    k2self();
  }
  constexpr Temperature(const Temperature<TemperatureType::eV>& t) noexcept
      : val(Conversion::ev2k(t.value())) {
    k2self();
  }
};  // Temperature

ENUMCLASS(NLTEType, char, ratio)  // TemperatureType

template <NLTEType X>
class NLTE final {
  SCALAR(NLTE)
  void ratio2self() noexcept {
    if constexpr (X == NLTEType::ratio) {
    }
  }

 public:
  constexpr NLTE(const NLTE<NLTEType::ratio>& nlte) noexcept
      : val(nlte.value()) {
    ratio2self();
  }
};  // Temperature

ENUMCLASS(MagnetismType, char, T,
          G)  // MagnetismType

template <MagnetismType X>
class Magnetism {
  std::array<double, 3> M;
  void t2self() noexcept {
    if constexpr (X == MagnetismType::T) {
    }
    if constexpr (X == MagnetismType::G) {
      for (auto& x : M) x = Conversion::t2g(x);
    }
  }

 public:
  constexpr Magnetism(std::array<double, 3> m) noexcept : M(m) {}
  Magnetism& operator=(const Magnetism& m) = default;
  Magnetism(const Magnetism<MagnetismType::T>& m) noexcept : M(m.M) {
    t2self();
  }
  Magnetism(const Magnetism<MagnetismType::G>& m) noexcept
      : M({Conversion::g2t(m.M[0]), Conversion::g2t(m.M[1]),
           Conversion::g2t(m.M[2])}) {
    t2self();
  }

  constexpr std::array<double, 3> value() const noexcept { return M; }
  double Strength() const noexcept { return std::hypot(M[0], M[1], M[2]); }
  Magnetism& operator*=(double x) noexcept {
    for (auto& m : M) m *= x;
    return *this;
  }
  Magnetism& operator+=(Magnetism x) noexcept {
    M[0] += x.M[0];
    M[1] += x.M[1];
    M[2] += x.M[2];
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os, Magnetism m) {
    return os << m.M[0] << ' ' << m.M[1] << ' ' << m.M[2];
  }
  friend std::istream& operator>>(std::istream& is, Magnetism& m) {
    return is >> m.M[0] >> m.M[1] >> m.M[2];
  }

  constexpr double u() const { return M[0]; }
  constexpr double v() const { return M[1]; }
  constexpr double w() const { return M[2]; }
};  // Magnetism

ENUMCLASS(WindType, char,
          meters_per_second)  // WindType

template <WindType X>
class Wind {
  std::array<double, 3> W;
  void ms2self() noexcept {
    if constexpr (X == WindType::meters_per_second) {
    }
  }

 public:
  constexpr Wind(std::array<double, 3> w) noexcept : W(w) {}
  Wind& operator=(const Wind& w) = default;
  Wind(const Wind<WindType::meters_per_second>& w) noexcept : W(w.W) {
    ms2self();
  }

  constexpr std::array<double, 3> value() const noexcept { return W; }
  double Strength() const noexcept { return std::hypot(W[0], W[1], W[2]); }
  Wind& operator*=(double x) noexcept {
    for (auto& w : W) w *= x;
    return *this;
  }
  Wind& operator+=(Wind x) noexcept {
    W[0] += x.W[0];
    W[1] += x.W[1];
    W[2] += x.W[2];
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os, Wind w) {
    return os << w.W[0] << ' ' << w.W[1] << ' ' << w.W[2];
  }
  friend std::istream& operator>>(std::istream& is, Wind& w) {
    return is >> w.W[0] >> w.W[1] >> w.W[2];
  }
};  // Wind

ENUMCLASS(VMRType, char,
          ratio)  // VMRType

template <VMRType X>
class VMR {
  Species::Isotope I;
  double R;
  void r2self() noexcept {
    if constexpr (X == VMRType::ratio) {
    }
  }

 public:
  constexpr VMR() noexcept
      : I(Species::Isotope(Species::Species::FINAL, -1)), R(0) {}
  explicit constexpr VMR(Species::Isotope s, double r) noexcept : I(s), R(r) {}
  constexpr VMR(const VMR<VMRType::ratio>& r) noexcept : I(r.I), R(r.R) {
    r2self();
  }

  constexpr Species::Species Species() const noexcept { return I.Spec(); }
  constexpr double value() const noexcept { return R; }
  double& value() noexcept { return R; }
  constexpr Species::Isotope isot() const noexcept { return I; }
  void isot(Species::Isotope i) noexcept { I = i; }
  VMR& operator*=(double x) noexcept {
    R *= x;
    return *this;
  }
  VMR& operator+=(VMR x) noexcept {
    if (x.I == I) {
      R += x.R;
    }
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os, VMR r) {
    return os << r.I << ' ' << r.R;
  }
  friend std::istream& operator>>(std::istream& is, VMR& r) {
    return is >> r.I >> r.R;
  }
};  // VMR

ENUMCLASS(DistanceType, char, meter)

template <DistanceType X>
class Distance final {
  SCALAR(Distance)
  void d2self() noexcept {
    if constexpr (X == DistanceType::meter) {
    }
  }

 public:
  constexpr Distance(const Distance<DistanceType::meter>& d) noexcept
      : val(d.value()) {
    d2self();
  }
};  // Distance

ENUMCLASS(AltitudeType, char,
          meter)  // AltType

template <AltitudeType X>
class Altitude final {
  SCALAR(Altitude)
  void m2self() noexcept {
    if constexpr (X == AltitudeType::meter) {
    }
  }

 public:
  constexpr Altitude(const Altitude<AltitudeType::meter>& z) noexcept
      : val(z.value()) {
    m2self();
  }
};  // Altitude

ENUMCLASS(LengthType, char,
          meter)  // AltType

template <LengthType X>
class Length final {
  SCALAR(Length)
  void m2self() noexcept {
    if constexpr (X == LengthType::meter) {
    }
  }

 public:
  constexpr Length(const Length<LengthType::meter>& z) noexcept
      : val(z.value()) {
    m2self();
  }
};  // Length

ENUMCLASS(CoordinateType, char, deg, lon, lat,
          rad)  // AltType

template <CoordinateType X>
class Coordinate final {
  SCALAR(Coordinate)
  void deg2self() noexcept {
    if constexpr (X == CoordinateType::deg) {
    }
    if constexpr (X == CoordinateType::lat) {
    }
    if constexpr (X == CoordinateType::lon) {
    }
    if constexpr (X == CoordinateType::rad) {
      val = Conversion::deg2rad(val);
    }
  }

 public:
  constexpr Coordinate(const Coordinate<CoordinateType::deg>& d) noexcept
      : val(d.value()) {
    deg2self();
  }
  constexpr Coordinate(const Coordinate<CoordinateType::lon>& d) noexcept
      : val(d.value()) {
    deg2self();
    while (val >= 360) val -= 360;
    while (val < 0) val += 360;
  }
  constexpr Coordinate(const Coordinate<CoordinateType::lat>& d) noexcept
      : val(d.value()) {
    deg2self();
    if (val > 90)
      val = 90;
    else if (val < -90)
      val = -90;
  }
  constexpr Coordinate(const Coordinate<CoordinateType::rad>& d) noexcept
      : val(Conversion::rad2deg(d.value())) {
    deg2self();
  }
};  // Coordinate

ENUMCLASS(FrequencyType, char, Freq, Kayser,
          Wavelength)  // FrequencyType

template <FrequencyType X>
class Frequency final {
  SCALAR(Frequency)
  void f2self() noexcept {
    if constexpr (X == FrequencyType::Freq) {
    }
    if constexpr (X == FrequencyType::Kayser) {
      val = Conversion::freq2kaycm(val);
    }
    if constexpr (X == FrequencyType::Wavelength) {
      val = Conversion::freq2wavelen(val);
    }
  }

 public:
  constexpr Frequency(const Frequency<FrequencyType::Freq>& f) noexcept
      : val(f.value()) {
    f2self();
  }
  constexpr Frequency(const Frequency<FrequencyType::Kayser>& f) noexcept
      : val(Conversion::kaycm2freq(f.value())) {
    f2self();
  }
  constexpr Frequency(const Frequency<FrequencyType::Wavelength>& f) noexcept
      : val(Conversion::wavelen2freq(f.value())) {
    f2self();
  }
};  // Frequency

ENUMCLASS(AreaType, char, m2,
          cm2)  // AreaType

template <FrequencyType X, AreaType Y>
class LineStrength {
  SCALAR(LineStrength)
  void hzm22self() noexcept {
    if constexpr (X == FrequencyType::Freq and Y == AreaType::m2) {
    }
    if constexpr (X == FrequencyType::Kayser and Y == AreaType::m2) {
      val = Conversion::arts2hitran_linestrength(val);
    }
  }

 public:
  constexpr LineStrength(
      const LineStrength<FrequencyType::Freq, AreaType::m2>& ls) noexcept
      : val(ls.value()) {
    hzm22self();
  }
  constexpr LineStrength(
      const LineStrength<FrequencyType::Kayser, AreaType::cm2>& ls) noexcept
      : val(Conversion::hitran2arts_linestrength(ls.value())) {
    hzm22self();
  }
};  // LineStrength

ENUMCLASS(EnergyType, char, Joule,
          invcm)  // EnergyType

template <EnergyType X>
class Energy {
  SCALAR(Energy)
  void hzm22self() noexcept {
    if constexpr (X == EnergyType::Joule) {
    }
    if constexpr (X == EnergyType::invcm) {
      val = Conversion::arts2hitran_energy(val);
    }
  }

 public:
  constexpr Energy(const Energy<EnergyType::Joule>& e) noexcept
      : val(e.value()) {
    hzm22self();
  }
  constexpr Energy(const Energy<EnergyType::invcm>& e) noexcept
      : val(Conversion::hitran2arts_energy(e.value())) {
    hzm22self();
  }
};  // Energy

template <FrequencyType X, PressureType Y>
class PressureBroadening {
  SCALAR(PressureBroadening)
  void hzpa2self() noexcept {
    if constexpr (X == FrequencyType::Freq and Y == PressureType::Pa) {
    }
    if constexpr (X == FrequencyType::Kayser and Y == PressureType::Atm) {
      val = Conversion::arts2hitran_broadening(val);
    }
  }

 public:
  constexpr PressureBroadening(
      const PressureBroadening<FrequencyType::Freq, PressureType::Pa>&
          pb) noexcept
      : val(pb.value()) {
    hzpa2self();
  }
  constexpr PressureBroadening(
      const PressureBroadening<FrequencyType::Kayser, PressureType::Atm>&
          pb) noexcept
      : val(Conversion::hitran2arts_broadening(pb.value())) {
    hzpa2self();
  }
};  // PressureBroadening

#undef SCALAR

#endif  // units_h
