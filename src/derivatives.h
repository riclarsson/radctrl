#ifndef derivatives_h
#define derivatives_h

#include "enums.h"
#include "species.h"

namespace Derivative {
ENUMCLASS(Type, unsigned char, Atm, Line)

ENUMCLASS(Atm, unsigned char, Temperature, WindU, WindV, WindW, MagneticU,
          MagneticV, MagneticW)

ENUMCLASS(Line, unsigned char, VMR, Strength, Center, ShapeG0X0, ShapeG0X1,
          ShapeG0X2, ShapeG0X3, ShapeD0X0, ShapeD0X1, ShapeD0X2, ShapeD0X3,
          ShapeG2X0, ShapeG2X1, ShapeG2X2, ShapeG2X3, ShapeD2X0, ShapeD2X1,
          ShapeD2X2, ShapeD2X3, ShapeFVCX0, ShapeFVCX1, ShapeFVCX2, ShapeFVCX3,
          ShapeETAX0, ShapeETAX1, ShapeETAX2, ShapeETAX3, ShapeYX0, ShapeYX1,
          ShapeYX2, ShapeYX3, ShapeGX0, ShapeGX1, ShapeGX2, ShapeGX3, ShapeDVX0,
          ShapeDVX1, ShapeDVX2, ShapeDVX3, NLTE, SpecialParameter1)

/** Union of quantities */
union TypeOfTarget {
  Atm atm;
  Line line;
  constexpr TypeOfTarget() noexcept : atm(Atm(-1)) {}
  constexpr TypeOfTarget(Atm a) noexcept : atm(a) {}
  constexpr TypeOfTarget(Line l) noexcept : line(l){};
  constexpr bool is(Type x, TypeOfTarget y) const {
    switch (x) {
      case Type::Atm:
        return atm == y.atm;
      case Type::Line:
        return line == y.line;
      case Type::FINAL: { /*leave last*/
      }
    }
    return false;
  }
};

inline TypeOfTarget toTypeOfTarget(const std::string& s, Type x) noexcept {
  switch (x) {
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

  /** Species ID for when it is required (lines require only speciesthis) */
  Species::Isotope species_id;

  /** ID of calculations that requires an ID to work */
  std::size_t line_id;

  /** Perturbations for methods where theoretical computations are difficult */
  double mperturbation;

 public:
  /** Atmospheric type */
  constexpr Target(
      Atm type, double pert = std::numeric_limits<double>::quiet_NaN()) noexcept
      : mtype(Type::Atm),
        msubtype(type),
        species_id(Species::Species::FINAL, -1),
        line_id(-1),
        mperturbation(pert) {}

  /** Line type */
  constexpr Target(
      Line type, std::size_t id, Species::Species spec,
      double pert = std::numeric_limits<double>::quiet_NaN()) noexcept
      : mtype(Type::Line),
        msubtype(type),
        species_id(spec, -1),
        line_id(id),
        mperturbation(pert) {}

  /** VMR type */
  constexpr Target(
      Species::Isotope id,
      double pert = std::numeric_limits<double>::quiet_NaN()) noexcept
      : mtype(Type::Line),
        msubtype(Line::VMR),
        species_id(id),
        line_id(-1),
        mperturbation(pert) {}

  /** A default none-type */
  constexpr Target() noexcept
      : mtype(Type::FINAL),
        msubtype(),
        species_id(Species::Species::FINAL, -1),
        line_id(-1),
        mperturbation(std::numeric_limits<double>::quiet_NaN()) {}

  constexpr bool operator==(Target x) const noexcept {
    if (mtype not_eq x.mtype or not msubtype.is(mtype, x.msubtype) or
        species_id not_eq x.species_id or line_id not_eq x.line_id)
      return false;
    else
      return true;
  }

  constexpr bool operator==(Atm x) const noexcept {
    if (mtype not_eq Type::Atm) {
      return false;
    } else if (msubtype.atm == x) {
      return true;
    } else {
      return false;
    }
  }

  constexpr bool operator==(Line x) const noexcept {
    if (mtype not_eq Type::Line) {
      return false;
    } else if (msubtype.line == x) {
      return true;
    } else {
      return false;
    }
  }

  constexpr Species::Species Species() const noexcept {
    return species_id.Spec();
  }

  constexpr bool isWind() const noexcept {
    if (mtype not_eq Type::Atm)
      return false;
    else if (msubtype.atm == Atm::WindU or msubtype.atm == Atm::WindV or
             msubtype.atm == Atm::WindW)
      return true;
    else
      return false;
  }

  constexpr bool isMagnetism() const noexcept {
    if (mtype not_eq Type::Atm)
      return false;
    else if (msubtype.atm == Atm::MagneticU or msubtype.atm == Atm::MagneticV or
             msubtype.atm == Atm::MagneticW)
      return true;
    else
      return false;
  }

  constexpr bool isLineCenter(std::size_t id) const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::Center and line_id == id)
      return true;
    else
      return false;
  }

  constexpr bool isLineStrength(std::size_t id) const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::Strength and line_id == id)
      return true;
    else
      return false;
  }

  constexpr bool isVMR(Species::Isotope id) const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::VMR and species_id == id)
      return true;
    else
      return false;
  }

  constexpr bool isshapeG0() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeG0X0 or
             msubtype.line == Line::ShapeG0X1 or
             msubtype.line == Line::ShapeG0X2 or
             msubtype.line == Line::ShapeG0X3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeD0() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeD0X0 or
             msubtype.line == Line::ShapeD0X1 or
             msubtype.line == Line::ShapeD0X2 or
             msubtype.line == Line::ShapeD0X3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeG2() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeG2X0 or
             msubtype.line == Line::ShapeG2X1 or
             msubtype.line == Line::ShapeG2X2 or
             msubtype.line == Line::ShapeG2X3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeD2() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeD2X0 or
             msubtype.line == Line::ShapeD2X1 or
             msubtype.line == Line::ShapeD2X2 or
             msubtype.line == Line::ShapeD2X3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeFVC() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeFVCX0 or
             msubtype.line == Line::ShapeFVCX1 or
             msubtype.line == Line::ShapeFVCX2 or
             msubtype.line == Line::ShapeFVCX3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeETA() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeETAX0 or
             msubtype.line == Line::ShapeETAX1 or
             msubtype.line == Line::ShapeETAX2 or
             msubtype.line == Line::ShapeETAX3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeY() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeYX0 or
             msubtype.line == Line::ShapeYX1 or
             msubtype.line == Line::ShapeYX2 or msubtype.line == Line::ShapeYX3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeG() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeGX0 or
             msubtype.line == Line::ShapeGX1 or
             msubtype.line == Line::ShapeGX2 or msubtype.line == Line::ShapeGX3)
      return true;
    else
      return false;
  }

  constexpr bool isshapeDV() const noexcept {
    if (mtype not_eq Type::Line)
      return false;
    else if (msubtype.line == Line::ShapeDVX0 or
             msubtype.line == Line::ShapeDVX1 or
             msubtype.line == Line::ShapeDVX2 or
             msubtype.line == Line::ShapeDVX3)
      return true;
    else
      return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const Target& t) {
    os << t.mtype << ' ';
    return os << toString(t.msubtype, t.mtype);
  }
};

}  // namespace Derivative

#endif  // derivatives_h
