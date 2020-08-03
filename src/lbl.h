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

namespace Absorption {
ENUMCLASS(Mirroring, unsigned char,
          None,
          Lorentz,
          Same
)

ENUMCLASS(Normalization, unsigned char,
          None,
          VVH,
          VVW,
          RosenkranzQuadratic
)

ENUMCLASS(Population, unsigned char,
          ByLTE,
          ByNLTEPopulationDistribution
)

ENUMCLASS(Cutoff, unsigned char,
          None,
          LineByLineOffset,
          BandFixedFrequency
)

ENUMCLASS(Shape, unsigned char,
          DP,
          LP,
          VP,
          SDVP,
          SDHCVP,
          HTP
)

struct Zeeman {
  double gu, gl;
  constexpr Zeeman(double u=0, double l=0) noexcept : gu(u), gl(l) {}
  friend std::ostream& operator<<(std::ostream& os, Zeeman z) {
    return os << z.gu << ' ' << z.gl;
  }
  friend std::istream& operator>>(std::istream& os, Zeeman& z) {
    return os >> z.gu >> z.gl;
  }
};

class Band;

class Line {
  Frequency<FrequencyType::Freq> f0;
  LineStrength<FrequencyType::Freq, AreaType::m2> i0;
  Energy<EnergyType::Joule> e0;
  Zeeman zeeman;
  double gu;
  double gl;
  double a;
  std::vector<Quantum::Number> local_lower;
  std::vector<Quantum::Number> local_upper;
  LineShape::Model model;
public:
  Line(Species::Isotope s) noexcept : f0(0), i0(0), e0(0), zeeman(), gu(0), gl(0), a(0),
  local_lower(s.localQuantumNumberCount()), local_upper(s.localQuantumNumberCount()) {
  }
  
  Line(Species::Isotope s,
       Frequency<FrequencyType::Freq> f,
       LineStrength<FrequencyType::Freq, AreaType::m2> i,
       Energy<EnergyType::Joule> e,
       Zeeman z,
       double Gu,
       double Gl,
       double A,
       std::vector<Quantum::Number> ll,
       std::vector<Quantum::Number> lu,
       LineShape::Model m) noexcept :
  f0(f), i0(i), e0(e), zeeman(z), gu(Gu), gl(Gl), a(A),
  local_lower(ll), local_upper(lu), model(m) {
    size_t size = s.localQuantumNumberCount();
    if (size not_eq ll.size() or  size not_eq lu.size()) {
      std::cerr << "Bad quantum number sizes\n";
      std::terminate();
    }
  }
  
  Frequency<FrequencyType::Freq> F0() const noexcept {return f0;}
  LineStrength<FrequencyType::Freq, AreaType::m2> I0() const noexcept {return i0;}
  Energy<EnergyType::Joule> E0() const noexcept {return e0;}
  Zeeman Ze() const noexcept {return zeeman;}
  double Gu() const noexcept {return gu;}
  double Gl() const noexcept {return gl;}
  double A() const noexcept {return a;}
  Quantum::Number localUpperQuantumNumber(size_t i) const noexcept {return local_upper[i];}
  Quantum::Number localLowerQuantumNumber(size_t i) const noexcept {return local_lower[i];}
  const LineShape::Model& ShapeModel() const noexcept {return model;}
  
  friend void readBand(File::File<File::Operation::Read, File::Type::Xml>& file, Band& band, const std::string& key);
  friend void readBand(File::File<File::Operation::ReadBinary, File::Type::Xml>& file, Band& band, const std::string& key);
};  // Line

class Band {
  Species::Isotope spec;
  Mirroring mirroring;
  Normalization normalization;
  Population population;
  Cutoff cutoff;
  Shape shape;
  Temperature<TemperatureType::K> t0;
  Frequency<FrequencyType::Freq> fcut;
  std::vector<Quantum::Number> global_lower;
  std::vector<Quantum::Number> global_upper;
  std::vector<Line> lines;
public:
  Band() noexcept : spec(Species::Species::FINAL, 0) {}
  
  // Init band
  Band (Species::Isotope s,
        Mirroring m,
        Normalization n,
        Population p,
        Cutoff c,
        Shape sh,
        Temperature<TemperatureType::K> T0,
        Frequency<FrequencyType::Freq> FCut, 
        const std::vector<Quantum::Number>& gl,
        const std::vector<Quantum::Number>& gu,
        long N=0) noexcept :
  spec(s), mirroring(m), normalization(n), population(p), cutoff(c),
  shape(sh), t0(T0), fcut(FCut), global_lower(gl), global_upper(gu),
  lines(N, Line{s}) {
    size_t size = s.globalQuantumNumberCount();
    if (size not_eq gl.size() or  size not_eq gu.size()) {
      std::cerr << "Bad quantum number sizes\n";
      std::terminate();
    }
  }
  
  bool operator==(const Band& b) const noexcept {
    return spec == b.spec and
           mirroring == b.mirroring and
           normalization == b.normalization and
           population == b.population and
           cutoff == b.cutoff and
           shape == b.shape and
           t0 == b.t0 and
           fcut == b.fcut and
           global_lower == b.global_lower and
           global_upper == b.global_upper;
  }
  
  void appendLine(Line line) noexcept {lines.push_back(std::move(line));}
  
  Species::Species Species() const noexcept {return spec.Spec();}
  std::vector<Species::ChargedAtom> Atoms() const noexcept {return spec.atoms();}
  Mirroring MirrorType() const noexcept {return mirroring;}
  Normalization NormType() const noexcept {return normalization;}
  Population PopType() const noexcept {return population;}
  Cutoff CutType() const noexcept {return cutoff;}
  Shape ShapeType() const noexcept {return shape;}
  Temperature<TemperatureType::K> T0() const noexcept {return t0;}
  Frequency<FrequencyType::Freq> FCut() const noexcept {return fcut;}
  size_t n_global() const noexcept {return spec.globalQuantumNumberCount();}
  size_t n_local() const noexcept {return spec.localQuantumNumberCount();}
  size_t n_lines() const noexcept {return lines.size();}
  size_t n_broadspec() const noexcept {if (n_lines()) return lines[0].ShapeModel().n_spec(); else return 0;}
  std::string broadspec() const noexcept {if (n_lines()) return lines[0].ShapeModel().description(); else return "0";}
  std::vector<Quantum::Type> globalQuantumNumbers() const noexcept {return spec.get_globalquantumnumbers();}
  Quantum::Number globalUpperQuantumNumber(size_t i) const noexcept {return global_upper[i];}
  Quantum::Number globalLowerQuantumNumber(size_t i) const noexcept {return global_lower[i];}
  std::vector<Quantum::Type> localQuantumNumbers() const noexcept {return spec.get_localquantumnumbers();}
  const std::vector<Line>& Lines() const noexcept {return lines;}
  
  friend void readBand(File::File<File::Operation::Read, File::Type::Xml>& file, Band& band, const std::string& key);
  friend void readBand(File::File<File::Operation::ReadBinary, File::Type::Xml>& file, Band& band, const std::string& key);
};  // Band

std::vector<Band> parse_hitran_with_qns(File::File<File::Operation::Read, File::Type::Raw>& hitranfile,
                                        Frequency<FrequencyType::Freq> flow,
                                        Frequency<FrequencyType::Freq> fupp) noexcept;

void saveBand(File::File<File::Operation::Write, File::Type::Xml>& file, const Band& band, const std::string& key="Band");
void saveBand(File::File<File::Operation::WriteBinary, File::Type::Xml>& file, const Band& band, const std::string& key="Band");
void readBand(File::File<File::Operation::Read, File::Type::Xml>& file, Band& band, const std::string& key="Band");
void readBand(File::File<File::Operation::ReadBinary, File::Type::Xml>& file, Band& band, const std::string& key="Band");

template <File::Operation X>
void saveBands(File::File<X, File::Type::Xml>& file, const std::vector<Band>& bands) {
  file.add_attribute("size", bands.size());
  for (size_t i=0; i<bands.size(); i++)
    saveBand(file, bands[i], std::string{"Band"}+std::to_string(i+1));
}
}  // Absorption

#endif  // lbl_h
