#include "inverse.h"

#include <numeric>

#include "forward.h"

namespace RTE::Inverse {

#define ATMVARSETTEREXEC(ACCESSOPERATION)                  \
  for (size_t itid = 0; itid < atm.ntid(); itid++) {       \
    for (size_t ialt = 0; ialt < atm.nalt(); ialt++) {     \
      for (size_t ilat = 0; ilat < atm.nlat(); ilat++) {   \
        for (size_t ilon = 0; ilon < atm.nlon(); ilon++) { \
          out[i] = ACCESSOPERATION;                        \
          i++;                                             \
        }                                                  \
      }                                                    \
    }                                                      \
  }                                                        \
  break

#define SHAPEVALSETTER(VAR)                                         \
  Derivative::Line::Shape##VAR##X0 : for (auto& band : bands) {     \
    for (auto& line : band.Lines()) {                               \
      if (line.ID() == d.LineID()) {                                \
        out[i] = 0;                                                 \
        for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) {   \
          if (line.ShapeModel()[i].s == d.Species()) {              \
            out[i] = line.ShapeModel()[i].VAR.X0;                   \
          }                                                         \
        }                                                           \
        i++;                                                        \
        goto quick_exit;                                            \
      }                                                             \
    }                                                               \
  }                                                                 \
  break;                                                            \
  case Derivative::Line::Shape##VAR##X1:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          out[i] = 0;                                               \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              out[i] = line.ShapeModel()[i].VAR.X1;                 \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break;                                                          \
  case Derivative::Line::Shape##VAR##X2:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          out[i] = 0;                                               \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              out[i] = line.ShapeModel()[i].VAR.X2;                 \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break;                                                          \
  case Derivative::Line::Shape##VAR##X3:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          out[i] = 0;                                               \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              out[i] = line.ShapeModel()[i].VAR.X3;                 \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break

size_t size_of_x(const std::vector<Derivative::Target>& derivs,
                 const std::size_t natm) {
  size_t nt = 0;
  for (auto& d : derivs) nt += (d == Derivative::Type::Line ? 1 : natm);
  return nt;
}

Eigen::VectorXd map_input_to_x(const Atmosphere::Atm& atm,
                               const std::vector<Absorption::Band>& bands,
                               const std::vector<Derivative::Target>& derivs) {
  const size_t natm = atm.ntid() * atm.nalt() * atm.nlat() * atm.nlon();
  Eigen::VectorXd out(size_of_x(derivs, natm));

  size_t i = 0;
  for (auto& d : derivs) {
    if (d == Derivative::Type::Atm) {
      switch (d.AtmType()) {
        case Derivative::Atm::Temperature:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).Temp());
        case Derivative::Atm::WindU:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().u());
        case Derivative::Atm::WindV:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().v());
        case Derivative::Atm::WindW:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().w());
        case Derivative::Atm::MagneticU:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().u());
        case Derivative::Atm::MagneticV:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().v());
        case Derivative::Atm::MagneticW:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().w());
        case Derivative::Atm::VMR:
          ATMVARSETTEREXEC(
              atm(itid, ialt, ilat, ilon).VolumeMixingRatio(d.Species()));
        case Derivative::Atm::NLTE:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).NonLTERatio(d.NLTE()));
        case Derivative::Atm::FINAL: { /* Leave last */
        }
      }
    } else {
      switch (d.LineType()) {
        case Derivative::Line::Strength:
          for (auto& band : bands) {
            for (auto& line : band.Lines()) {
              if (line.ID() == d.LineID()) {
                out[i] = line.I0();
                i++;
                goto quick_exit;
              }
            }
          }
          break;
        case Derivative::Line::Center:
          for (auto& band : bands) {
            for (auto& line : band.Lines()) {
              if (line.ID() == d.LineID()) {
                out[i] = line.F0();
                i++;
                goto quick_exit;
              }
            }
          }
          break;
        case SHAPEVALSETTER(G0); case SHAPEVALSETTER(D0);
            case SHAPEVALSETTER(G2); case SHAPEVALSETTER(D2);
            case SHAPEVALSETTER(FVC); case SHAPEVALSETTER(ETA);
            case SHAPEVALSETTER(Y); case SHAPEVALSETTER(G);
            case SHAPEVALSETTER(DV);
            case Derivative::Line::FINAL: { /* Leave last */
        }
      }
    }
  quick_exit : {}
  }

  return out;
}
#undef ATMVARSETTEREXEC
#undef SHAPEVALSETTER

#define ATMVARSETTEREXEC(FULLSETTINGOP)                    \
  for (size_t itid = 0; itid < atm.ntid(); itid++) {       \
    for (size_t ialt = 0; ialt < atm.nalt(); ialt++) {     \
      for (size_t ilat = 0; ilat < atm.nlat(); ilat++) {   \
        for (size_t ilon = 0; ilon < atm.nlon(); ilon++) { \
          FULLSETTINGOP;                                   \
          i++;                                             \
        }                                                  \
      }                                                    \
    }                                                      \
  }                                                        \
  break

#define SHAPEVALSETTER(VAR)                                         \
  Derivative::Line::Shape##VAR##X0 : for (auto& band : bands) {     \
    for (auto& line : band.Lines()) {                               \
      if (line.ID() == d.LineID()) {                                \
        for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) {   \
          if (line.ShapeModel()[i].s == d.Species()) {              \
            line.ShapeModel()[i].VAR.X0 = x[i];                     \
          }                                                         \
        }                                                           \
        i++;                                                        \
        goto quick_exit;                                            \
      }                                                             \
    }                                                               \
  }                                                                 \
  break;                                                            \
  case Derivative::Line::Shape##VAR##X1:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              line.ShapeModel()[i].VAR.X1 = x[i];                   \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break;                                                          \
  case Derivative::Line::Shape##VAR##X2:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              line.ShapeModel()[i].VAR.X2 = x[i];                   \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break;                                                          \
  case Derivative::Line::Shape##VAR##X3:                            \
    for (auto& band : bands) {                                      \
      for (auto& line : band.Lines()) {                             \
        if (line.ID() == d.LineID()) {                              \
          for (size_t i = 0; i < line.ShapeModel().n_spec(); i++) { \
            if (line.ShapeModel()[i].s == d.Species()) {            \
              line.ShapeModel()[i].VAR.X3 = x[i];                   \
            }                                                       \
          }                                                         \
          i++;                                                      \
          goto quick_exit;                                          \
        }                                                           \
      }                                                             \
    }                                                               \
    break

void set_input_from_x(Atmosphere::Atm& atm,
                      std::vector<Absorption::Band>& bands,
                      const std::vector<Derivative::Target>& derivs,
                      const Eigen::VectorXd& x) {
  const size_t natm = atm.ntid() * atm.nalt() * atm.nlat() * atm.nlon();
  if (size_of_x(derivs, natm) not_eq size_t(x.size())) {
    std::cerr << "Bad x input\n";
    std::terminate();
  }

  size_t i = 0;
  for (auto& d : derivs) {
    if (d == Derivative::Type::Atm) {
      switch (d.AtmType()) {
        case Derivative::Atm::Temperature:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).Temp(x[i]));
        case Derivative::Atm::WindU:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().u() = x[i]);
        case Derivative::Atm::WindV:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().v() = x[i]);
        case Derivative::Atm::WindW:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).WindField().w() = x[i]);
        case Derivative::Atm::MagneticU:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().u() = x[i]);
        case Derivative::Atm::MagneticV:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().v() = x[i]);
        case Derivative::Atm::MagneticW:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).MagField().w() = x[i]);
        case Derivative::Atm::VMR:
          ATMVARSETTEREXEC(
              atm(itid, ialt, ilat, ilon).VolumeMixingRatio(d.Species(), x[i]));
        case Derivative::Atm::NLTE:
          ATMVARSETTEREXEC(atm(itid, ialt, ilat, ilon).NonLTERatio(d.NLTE()) =
                               x[i]);
        case Derivative::Atm::FINAL: { /* Leave last */
        }
      }
    } else {
      switch (d.LineType()) {
        case Derivative::Line::Strength:
          for (auto& band : bands) {
            for (auto& line : band.Lines()) {
              if (line.ID() == d.LineID()) {
                line.I0(x[i]);
                i++;
                goto quick_exit;
              }
            }
          }
          break;
        case Derivative::Line::Center:
          for (auto& band : bands) {
            for (auto& line : band.Lines()) {
              if (line.ID() == d.LineID()) {
                line.F0(x[i]);
                i++;
                goto quick_exit;
              }
            }
          }
          break;
        case SHAPEVALSETTER(G0); case SHAPEVALSETTER(D0);
            case SHAPEVALSETTER(G2); case SHAPEVALSETTER(D2);
            case SHAPEVALSETTER(FVC); case SHAPEVALSETTER(ETA);
            case SHAPEVALSETTER(Y); case SHAPEVALSETTER(G);
            case SHAPEVALSETTER(DV);
            case Derivative::Line::FINAL: { /* Leave last */
        }
      }
    quick_exit : {}
    }
  }
}
#undef ATMVARSETTEREXEC
#undef SHAPEVALSETTER

Results compute(Atmosphere::Atm& atm, Background::Background& background, std::vector<Absorption::Band>& bands,
                const Geom::Nav& pos_los,
                const std::vector<Derivative::Target>& derivs,
                const Sensor::Properties& sensor_prop,
                const Distance<DistanceType::meter> layer_thickness,
                const Eigen::MatrixXd& sy, const Eigen::MatrixXd& sx,
                const Eigen::VectorXd& y, const Eigen::VectorXd& x0) {
  Computations comp(ComputationsType::Linear, sy, sx, x0, y);

  do {
    const RTE::Forward::Convolution forward = RTE::Forward::compute_convolution(
        atm, background, pos_los, bands, derivs, sensor_prop, layer_thickness);
    comp.fx.noalias() = forward.rad;
    comp.J.noalias() = forward.jac;
    set_input_from_x(atm, bands, derivs, comp.update_x());
  } while (not comp.is_done());

  // FIXME: Add Sensor Errors
  return Results{y,        map_input_to_x(atm, bands, derivs),
                 comp.fx,  comp.J,
                 comp.S(), comp.D(),
                 comp.A()};
}
}  // namespace RTE::Inverse
