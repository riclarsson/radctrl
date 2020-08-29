#ifndef derivatives_h
#define derivatives_h

#include "enums.h"

namespace Derivative {
ENUMCLASS(Type, unsigned char, Atm, Line, Sensor)

ENUMCLASS(Atm, unsigned char, Temperature, WindMagnitude, WindU, WindV, WindW,
          MagneticMagnitude, MagneticU, MagneticV, MagneticW, Electrons,
          Particulates)

ENUMCLASS(Line, unsigned char, VMR, Strength, Center, ShapeG0X0, ShapeG0X1,
          ShapeG0X2, ShapeG0X3, ShapeD0X0, ShapeD0X1, ShapeD0X2, ShapeD0X3,
          ShapeG2X0, ShapeG2X1, ShapeG2X2, ShapeG2X3, ShapeD2X0, ShapeD2X1,
          ShapeD2X2, ShapeD2X3, ShapeFVCX0, ShapeFVCX1, ShapeFVCX2, ShapeFVCX3,
          ShapeETAX0, ShapeETAX1, ShapeETAX2, ShapeETAX3, ShapeYX0, ShapeYX1,
          ShapeYX2, ShapeYX3, ShapeGX0, ShapeGX1, ShapeGX2, ShapeGX3, ShapeDVX0,
          ShapeDVX1, ShapeDVX2, ShapeDVX3, NLTE, SpecialParameter1)

ENUMCLASS(Sensor, unsigned char, FrequencyShift, FrequencyStretch, Polyfit,
          Sinefit)

/** Union of quantities */
union TypeOfTarget {
  Atm atm;
  Line line;
  Sensor sensor;
  constexpr TypeOfTarget() noexcept : atm(Atm(-1)) {}
  constexpr TypeOfTarget(Atm a) noexcept : atm(a) {}
  constexpr TypeOfTarget(Line l) noexcept : line(l){};
  constexpr TypeOfTarget(Sensor s) noexcept : sensor(s){};
};

inline TypeOfTarget toTypeOfTarget(const std::string &s, Type x) noexcept {
  switch (x) {
    case Type::Sensor:
      return TypeOfTarget(toSensor(s));
    case Type::Line:
      return TypeOfTarget(toLine(s));
    case Type::Atm:
      return TypeOfTarget(toAtm(s));
    case Type::FINAL: /* leave last, don't use default */;
  }
  return TypeOfTarget();
}

inline std::string toString(TypeOfTarget y, Type x) noexcept {
  switch (x) {
    case Type::Sensor:
      return toString(y.sensor);
    case Type::Line:
      return toString(y.line);
    case Type::Atm:
      return toString(y.atm);
    case Type::FINAL: /* leave last, don't use default */;
  }
  return "Unrecognizable Target";
}

class Target {
  Type mtype;

  /** Type of quantity, set manually */
  TypeOfTarget msubtype;

  /** Perturbations for methods where theoretical computations are impossible or
   * plain slow */
  double mperturbation;

  /** ID of calculations that requires an ID to work */
  std::size_t mid;

 public:
  /** Atmospheric type */
  constexpr explicit Target(Atm type) noexcept
      : mtype(Type::Atm),
        msubtype(type),
        mperturbation(std::numeric_limits<double>::quiet_NaN()),
        mid(-1) {}

  /** Line type */
  constexpr explicit Target(Line type, std::size_t id) noexcept
      : mtype(Type::Line),
        msubtype(type),
        mperturbation(std::numeric_limits<double>::quiet_NaN()),
        mid(id) {}

  /** Sensor type */
  constexpr explicit Target(Sensor type) noexcept
      : mtype(Type::Sensor),
        msubtype(type),
        mperturbation(std::numeric_limits<double>::quiet_NaN()),
        mid(-1) {}

  /** A default none-type */
  constexpr Target() noexcept
      : mtype(Type::FINAL),
        msubtype(),
        mperturbation(std::numeric_limits<double>::quiet_NaN()),
        mid(-1) {}

  /** Perturbation set */
  void Perturbation(double x) noexcept { mperturbation = x; }

  /** Perturbation get */
  constexpr double Perturbation() const noexcept { return mperturbation; }

  /** Identity set */
  void Identity(std::size_t x) noexcept { mid = x; }

  /** Identity get */
  constexpr std::size_t Identity() const noexcept { return mid; }

  /** Equality */
  constexpr bool operator==(Target other) const noexcept {
    if (mtype == other.mtype) {
      switch (mtype) {
        case Type::Atm:
          return other.msubtype.atm == msubtype.atm and mid == other.mid and
                 mperturbation == other.mperturbation;
        case Type::Sensor:
          return other.msubtype.sensor == msubtype.sensor and
                 mid == other.mid and mperturbation == other.mperturbation;
        case Type::Line:
          return other.msubtype.line == msubtype.line and mid == other.mid and
                 mperturbation == other.mperturbation;
        case Type::FINAL: {
        }
      }
    }
    return false;
  }

  /** Return the line type */
  constexpr Line LineType() const {
    if (mtype == Type::Line)
      return msubtype.line;
    else
      return Line::FINAL;
  }

  /** Return the line type */
  constexpr Atm AtmType() const {
    if (mtype == Type::Atm)
      return msubtype.atm;
    else
      return Atm::FINAL;
  }

  /** Return the line type */
  constexpr Sensor SensorType() const {
    if (mtype == Type::Sensor)
      return msubtype.sensor;
    else
      return Sensor::FINAL;
  }

  /** Checks if the type of Jacobian is the input atmospheric parameter */
  constexpr bool operator==(Atm other) const noexcept {
    return Type::Atm == mtype and other == msubtype.atm;
  }

  /** Checks if the type of Jacobian is the input line parameter */
  constexpr bool operator==(Line other) const noexcept {
    return Type::Line == mtype and other == msubtype.line;
  }

  /** Checks if the type of Jacobian is the input sensor parameter */
  constexpr bool operator==(Sensor other) const noexcept {
    return Type::Sensor == mtype and other == msubtype.sensor;
  }

  /** Checks if the type is correct */
  constexpr bool operator==(Type other) const noexcept {
    return other == mtype;
  }

  /** Special wind case */
  constexpr bool isWind() const noexcept {
    return mtype == Type::Atm and
           (msubtype.atm == Atm::WindMagnitude or msubtype.atm == Atm::WindU or
            msubtype.atm == Atm::WindV or msubtype.atm == Atm::WindW);
  }

  /** Special magnetic field case */
  constexpr bool isMagnetic() const noexcept {
    return mtype == Type::Atm and
           (msubtype.atm == Atm::MagneticMagnitude or
            msubtype.atm == Atm::MagneticU or msubtype.atm == Atm::MagneticV or
            msubtype.atm == Atm::MagneticW);
  }

  /** Special frequency case */
  constexpr bool isFrequency() const noexcept {
    return mtype == Type::Sensor and
           (msubtype.sensor == Sensor::FrequencyStretch or
            msubtype.sensor == Sensor::FrequencyShift);
  }

  /** Output operator */
  friend std::ostream &operator<<(std::ostream &os, const Target &x) {
    return os << x.mtype << " " << toString(x.msubtype, x.mtype) << " "
              << x.Perturbation() << " " << x.Identity();
  }

  /** Input operator */
  friend std::istream &operator>>(std::istream &is, Target x) {
    std::string typeoftarget;
    is >> x.mtype >> typeoftarget >> x.mperturbation >> x.mid;
    x.msubtype = toTypeOfTarget(typeoftarget, x.mtype);
    return is;
  }
};
}  // namespace Derivative

#endif  // derivatives_h
