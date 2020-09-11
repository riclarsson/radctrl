#include "inverse.h"

#include <numeric>

#include "forward.h"

namespace RTE::Inverse {
template <std::size_t N>
void convolve_forward_simulation(
    ForwardConvolution& res, const Forward::Results<N>& partres,
    const std::vector<Derivative::Target>& derivs,
    const std::vector<Atmosphere::InterPoints>& interp_weights,
    const double path_weight) {
  const std::size_t np = partres.x.sizes()[0];
  const std::size_t nf = partres.x.sizes()[1];
  const std::size_t nt = derivs.size();

  std::size_t NT0 = 0;
  for (std::size_t ip = 0; ip < np; ip++) {
    auto weights = interp_weights[ip].Weights();

    for (std::size_t it = 0; it < nt; it++) {
      for (std::size_t iv = 0; iv < nf; iv++) {
        // Path-weighted radiation derivative vector
        const RadVec<N> this_val = path_weight * partres.dx(it, ip, iv);
        for (std::size_t is = 0; is < N; is++) {
          for (auto& iw : weights) {
            // Add if there is weight... note that 0 weight might mean out of
            // bound index.
            if (iw.w not_eq 0) {
              res.jac(iv * N + is,
                      NT0 + res.iatmdata(iw.itid, iw.ialt, iw.ilat, iw.ilon)) +=
                  iw.w * this_val[is];
            }
          }
        }
      }

      // Move derivative pointer
      NT0 += (derivs[it] == Derivative::Type::Line) ? 1 : res.natmdata();
    }
  }

  auto ypart = partres.sensor_results();
  for (std::size_t iv = 0; iv < nf; iv++) {
    for (std::size_t is = 0; is < N; is++) {
      res.rad[iv] += path_weight * ypart[iv][is];
    }
  }
}

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

size_t size_of_x(const std::vector<Derivative::Target>& derivs, size_t natm) {
  size_t nt = 0;
  for (auto& d : derivs) nt += (d == Derivative::Type::Line ? 1 : natm);
  return nt;
}

Eigen::VectorXd map_derivs_to_x(const Atmosphere::Atm& atm,
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

ForwardConvolution compute_forward(
    const Atmosphere::Atm& atm, const Geom::Nav& pos_los,
    const std::vector<Absorption::Band>& bands,
    const std::vector<Derivative::Target>& derivs,
    const std::vector<Frequency<FrequencyType::Freq>>& sensor_f_grid,
    const std::size_t stokes_dimension, const BeamType beamtype) {
  ForwardConvolution out(sensor_f_grid.size(), stokes_dimension, atm, derivs);

  std::vector<std::pair<std::vector<Path::Point>, double>> paths;
  switch (beamtype) {
    case BeamType::PencilBeam:
      paths = {std::pair<std::vector<Path::Point>, double>{
          Path::calc_single_geometric_path(pos_los, atm, 1e3, 90e3), 1}};
      break;
    case BeamType::FINAL: { /* Leave last */
    }
  }

  // Compute and convolve all the different paths (stokes dim must be compiled
  // separately)
  for (auto& [path, path_weight] : paths) {
    if (stokes_dimension == 4) {
      const auto rad0 = RTE::source_vec_planck<4>(
          path.front().atm.Temp(),
          sensor_f_grid);  // FIXME: COMPUTE INITIAL RADIATION BETTER!
      auto forward_results =
          Forward::compute(rad0, sensor_f_grid, derivs, bands, path);
      std::vector<Atmosphere::InterPoints> interp_weights(path.size());
      std::transform(path.cbegin(), path.cend(), interp_weights.begin(),
                     [](auto& p) { return p.ip; });
      convolve_forward_simulation(out, forward_results, derivs, interp_weights,
                                  path_weight);
    } else if (stokes_dimension == 3) {
      const auto rad0 = RTE::source_vec_planck<3>(
          path.front().atm.Temp(),
          sensor_f_grid);  // FIXME: COMPUTE INITIAL RADIATION BETTER!
      auto forward_results =
          Forward::compute(rad0, sensor_f_grid, derivs, bands, path);
      std::vector<Atmosphere::InterPoints> interp_weights(path.size());
      std::transform(path.cbegin(), path.cend(), interp_weights.begin(),
                     [](auto& p) { return p.ip; });
      convolve_forward_simulation(out, forward_results, derivs, interp_weights,
                                  path_weight);
    } else if (stokes_dimension == 2) {
      const auto rad0 = RTE::source_vec_planck<2>(
          path.front().atm.Temp(),
          sensor_f_grid);  // FIXME: COMPUTE INITIAL RADIATION BETTER!
      auto forward_results =
          Forward::compute(rad0, sensor_f_grid, derivs, bands, path);
      std::vector<Atmosphere::InterPoints> interp_weights(path.size());
      std::transform(path.cbegin(), path.cend(), interp_weights.begin(),
                     [](auto& p) { return p.ip; });
      convolve_forward_simulation(out, forward_results, derivs, interp_weights,
                                  path_weight);
    } else if (stokes_dimension == 1) {
      const auto rad0 = RTE::source_vec_planck<1>(
          path.front().atm.Temp(),
          sensor_f_grid);  // FIXME: COMPUTE INITIAL RADIATION BETTER!
      auto forward_results =
          Forward::compute(rad0, sensor_f_grid, derivs, bands, path);
      std::vector<Atmosphere::InterPoints> interp_weights(path.size());
      std::transform(path.cbegin(), path.cend(), interp_weights.begin(),
                     [](auto& p) { return p.ip; });
      convolve_forward_simulation(out, forward_results, derivs, interp_weights,
                                  path_weight);
    } else {
      std::cerr << "Bad Stokes Dim\n";
      std::terminate();
    }
  }

  return out;
}
}  // namespace RTE::Inverse
