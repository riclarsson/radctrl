#ifndef lbl_h
#define lbl_h

#include <array>
#include <cmath>
#include <vector>

#include "enums.h"
#include "file.h"
#include "lineshapes.h"
#include "quantum.h"
#include "species.h"
#include "units.h"
#include "zeeman.h"

namespace Absorption {
ENUMCLASS(Mirroring, unsigned char, None, Lorentz, Same)

ENUMCLASS(Normalization, unsigned char, None, VVH, VVW, RosenkranzQuadratic)

ENUMCLASS(Population, unsigned char, ByLTE, ByNLTE)

ENUMCLASS(Cutoff, unsigned char, None, ByLineOffset, ByFixedFrequency)

ENUMCLASS(Shape, unsigned char, DP, LP, VP, SDVP, SDHCVP, HTP)

class Band;

class Line {
  Frequency<FrequencyType::Freq> f0;  // Central frequency
  LineStrength<FrequencyType::Freq, AreaType::m2>
      i0;                                    // Reference line strength
  Energy<EnergyType::Joule> e0;              // Lower state energy
  Zeeman::Model zeeman;                      // Zeeman model
  double gl;                                 // Lower degeneracy
  double gu;                                 // Upper degeneracy
  Decay<DecayType::ExponentialPerSecond> a;  // Einstein coefficient
  std::pair<std::size_t, std::size_t>
      ids;  // ID, only initialized if necessary (lower, upper)
  std::vector<Quantum::Number> local_lower;  // Lower local quantum
  std::vector<Quantum::Number> local_upper;  // Upper local quantum
  LineShape::Model lineshape;                // Line shape model

 public:
  Line(Species::Isotope s) noexcept
      : f0(0),
        i0(0),
        e0(0),
        zeeman(),
        gl(0),
        gu(0),
        a(0),
        local_lower(s.localQuantumNumberCount()),
        local_upper(s.localQuantumNumberCount()) {}

  Line(Species::Isotope s, Frequency<FrequencyType::Freq> f,
       LineStrength<FrequencyType::Freq, AreaType::m2> i,
       Energy<EnergyType::Joule> e, Zeeman::Model z, double Gu, double Gl,
       double A, std::vector<Quantum::Number> ll,
       std::vector<Quantum::Number> lu, LineShape::Model m) noexcept
      : f0(f),
        i0(i),
        e0(e),
        zeeman(z),
        gl(Gl),
        gu(Gu),
        a(A),
        local_lower(ll),
        local_upper(lu),
        lineshape(m) {
    size_t size = s.localQuantumNumberCount();
    if (size not_eq ll.size() or size not_eq lu.size()) {
      std::cerr << "Bad quantum number sizes\n";
      std::terminate();
    }
  }

  Frequency<FrequencyType::Freq> F0() const noexcept { return f0; }
  LineStrength<FrequencyType::Freq, AreaType::m2> I0() const noexcept {
    return i0;
  }
  Energy<EnergyType::Joule> E0() const noexcept { return e0; }
  Zeeman::Model Ze() const noexcept { return zeeman; }
  std::pair<int, int> ZeemanRange(Polarization p, Species::Isotope s) const {
    auto l = s.get_local(local_lower, Quantum::Type::F).rat();
    auto u = s.get_local(local_upper, Quantum::Type::F).rat();
    if (l.ok() and u.ok())
      return std::pair<int, int>{Zeeman::start(u, l, p), Zeeman::end(u, l, p)};

    l = s.get_local(local_lower, Quantum::Type::J).rat();
    u = s.get_local(local_upper, Quantum::Type::J).rat();
    if (l.ok() and u.ok())
      return std::pair<int, int>{Zeeman::start(u, l, p), Zeeman::end(u, l, p)};

    return std::pair<int, int>{0, 0};  // No Zeeman effect means one line!
  }
  Frequency<FrequencyType::Freq> ZeemanSplitting(Polarization p,
                                                 Species::Isotope s,
                                                 int i) const {
    auto l = s.get_local(local_lower, Quantum::Type::F).rat();
    auto u = s.get_local(local_upper, Quantum::Type::F).rat();
    if (l.ok() and u.ok()) return zeeman.Splitting(u, l, p, i);

    l = s.get_local(local_lower, Quantum::Type::J).rat();
    u = s.get_local(local_upper, Quantum::Type::J).rat();
    if (l.ok() and u.ok()) return zeeman.Splitting(u, l, p, i);

    return 0.0;  // No Zeeman effect means no splitting!
  }
  double ZeemanStrength(Polarization p, Species::Isotope s, int i) const {
    auto l = s.get_local(local_lower, Quantum::Type::F).rat();
    auto u = s.get_local(local_upper, Quantum::Type::F).rat();
    if (l.ok() and u.ok()) return zeeman.Strength(u, l, p, i);

    l = s.get_local(local_lower, Quantum::Type::J).rat();
    u = s.get_local(local_upper, Quantum::Type::J).rat();
    if (l.ok() and u.ok()) return zeeman.Strength(u, l, p, i);

    return 1.0;  // No Zeeman effect all is in the main line
  }
  double Gu() const noexcept { return gu; }
  double Gl() const noexcept { return gl; }
  double A() const noexcept { return a; }
  Quantum::Number localUpperQuantumNumber(size_t i) const noexcept {
    return local_upper[i];
  }
  Quantum::Number localLowerQuantumNumber(size_t i) const noexcept {
    return local_lower[i];
  }
  const LineShape::Model &ShapeModel() const noexcept { return lineshape; }

  friend void readBand(File::File<File::Operation::Read, File::Type::Xml> &file,
                       Band &band, const std::string &key);
  friend void readBand(
      File::File<File::Operation::ReadBinary, File::Type::Xml> &file,
      Band &band, const std::string &key);
};  // Line

class Band {
  Species::Isotope spec;
  Mirroring mirroring;
  Normalization normalization;
  Population population;
  Cutoff cutoff;
  Shape shape;
  bool do_zeeman;
  Temperature<TemperatureType::K> t0;
  Frequency<FrequencyType::Freq> fcut;
  std::vector<Quantum::Number> global_lower;
  std::vector<Quantum::Number> global_upper;
  std::vector<Line> lines;

 public:
  Band() noexcept : spec(Species::Species::FINAL, 0) {}

  // Init band
  Band(Species::Isotope s, Mirroring m, Normalization n, Population p, Cutoff c,
       Shape sh, bool dz, Temperature<TemperatureType::K> T0,
       Frequency<FrequencyType::Freq> FCut,
       const std::vector<Quantum::Number> &gl,
       const std::vector<Quantum::Number> &gu, long N = 0) noexcept
      : spec(s),
        mirroring(m),
        normalization(n),
        population(p),
        cutoff(c),
        shape(sh),
        do_zeeman(dz),
        t0(T0),
        fcut(FCut),
        global_lower(gl),
        global_upper(gu),
        lines(N, Line{s}) {
    size_t size = s.globalQuantumNumberCount();
    if (size not_eq gl.size() or size not_eq gu.size()) {
      std::cerr << "Bad quantum number sizes\n";
      std::terminate();
    }
  }

  bool operator==(const Band &b) const noexcept {
    return spec == b.spec and mirroring == b.mirroring and
           normalization == b.normalization and population == b.population and
           cutoff == b.cutoff and shape == b.shape and
           do_zeeman == b.do_zeeman and t0 == b.t0 and fcut == b.fcut and
           global_lower == b.global_lower and global_upper == b.global_upper;
  }

  void appendLine(Line line) noexcept { lines.push_back(std::move(line)); }

  Species::Isotope Isotopologue() const noexcept { return spec; }
  Species::Species Species() const noexcept { return spec.Spec(); }
  std::vector<Species::ChargedAtom> Atoms() const noexcept {
    return spec.atoms();
  }
  Mirroring MirrorType() const noexcept { return mirroring; }
  Normalization NormType() const noexcept { return normalization; }
  Population PopType() const noexcept { return population; }
  Cutoff CutType() const noexcept { return cutoff; }
  Shape ShapeType() const noexcept { return shape; }
  Temperature<TemperatureType::K> T0() const noexcept { return t0; }
  Frequency<FrequencyType::Freq> FCut() const noexcept { return fcut; }
  size_t n_global() const noexcept { return spec.globalQuantumNumberCount(); }
  size_t n_local() const noexcept { return spec.localQuantumNumberCount(); }
  size_t n_lines() const noexcept { return lines.size(); }
  size_t n_broadspec() const noexcept {
    if (n_lines())
      return lines[0].ShapeModel().n_spec();
    else
      return 0;
  }
  std::string broadspec() const noexcept {
    if (n_lines())
      return lines[0].ShapeModel().description();
    else
      return "0";
  }
  std::vector<Quantum::Type> globalQuantumNumbers() const noexcept {
    return spec.get_globalquantumnumbers();
  }
  Quantum::Number globalUpperQuantumNumber(size_t i) const noexcept {
    return global_upper[i];
  }
  Quantum::Number globalLowerQuantumNumber(size_t i) const noexcept {
    return global_lower[i];
  }
  std::vector<Quantum::Type> localQuantumNumbers() const noexcept {
    return spec.get_localquantumnumbers();
  }
  const std::vector<Line> &Lines() const noexcept { return lines; }
  std::vector<Line> &Lines() noexcept { return lines; }
  double GD_giv_F0(Temperature<TemperatureType::K> T) const {
    return std::sqrt(Constant::doppler_broadening_const_squared * T /
                     spec.mass());
  }
  Frequency<FrequencyType::Freq> CutoffUpper(size_t iline) const noexcept {
    switch (cutoff) {
      case Cutoff::None:
        return std::numeric_limits<double>::max();
      case Cutoff::ByFixedFrequency:
        return fcut;
      case Cutoff::ByLineOffset:
        return lines[iline].F0() + fcut;
      case Cutoff::FINAL: { /*leave last*/
      }
    }
    return {};
  }
  Frequency<FrequencyType::Freq> MeanFreq() const noexcept {
    const double val = std::inner_product(
        lines.cbegin(), lines.cend(), lines.cbegin(), 0.0, std::plus<double>(),
        [](const auto &a, const auto &b) { return a.F0() * b.I0(); });
    const double div = std::accumulate(
        lines.cbegin(), lines.cend(), 0.0,
        [](const auto &a, const auto &b) { return a + b.I0(); });
    return val / div;
  }
  Frequency<FrequencyType::Freq> CutoffLower(size_t iline) const noexcept {
    switch (cutoff) {
      case Cutoff::None:
        return -std::numeric_limits<double>::max();
      case Cutoff::ByFixedFrequency:
        return fcut - 2 * MeanFreq();
      case Cutoff::ByLineOffset:
        return lines[iline].F0() - fcut;
      case Cutoff::FINAL: { /*leave last*/
      }
    }
    return {};
  }
  double QT0() const noexcept { return spec.QT(t0); }
  double QT(Temperature<TemperatureType::K> T) const noexcept {
    return spec.QT(T);
  }
  bool doZeeman() const noexcept { return do_zeeman; }

  friend void readBand(File::File<File::Operation::Read, File::Type::Xml> &file,
                       Band &band, const std::string &key);
  friend void readBand(
      File::File<File::Operation::ReadBinary, File::Type::Xml> &file,
      Band &band, const std::string &key);
};  // Band

std::vector<Band> parse_hitran_with_qns(
    File::File<File::Operation::Read, File::Type::Raw> &hitranfile,
    Frequency<FrequencyType::Freq> flow,
    Frequency<FrequencyType::Freq> fupp) noexcept;

void saveBand(File::File<File::Operation::Write, File::Type::Xml> &file,
              const Band &band, const std::string &key = "Band");
void saveBand(File::File<File::Operation::WriteBinary, File::Type::Xml> &file,
              const Band &band, const std::string &key = "Band");
void readBand(File::File<File::Operation::Read, File::Type::Xml> &file,
              Band &band, const std::string &key = "Band");
void readBand(File::File<File::Operation::ReadBinary, File::Type::Xml> &file,
              Band &band, const std::string &key = "Band");

template <File::Operation X>
void saveBands(File::File<X, File::Type::Xml> &file,
               const std::vector<Band> &bands) {
  file.add_attribute("size", bands.size());
  for (size_t i = 0; i < bands.size(); i++)
    saveBand(file, bands[i], std::string{"Band"} + std::to_string(i + 1));
}
}  // namespace Absorption

#endif  // lbl_h
