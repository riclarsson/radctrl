#include "lbl.h"

#include "hitran.h"

namespace Absorption {
std::vector<Band> parse_hitran_with_qns(
    File::File<File::Operation::Read, File::Type::Raw>& hitranfile,
    Frequency<FrequencyType::Freq> flow,
    Frequency<FrequencyType::Freq> fupp) noexcept {
  std::vector<Band> database;

  // Get the HITRAN data
  std::vector<std::vector<HITRAN::IsotopologueData>> hitrandata{
      int(Species::Species::FINAL)};
  for (long i = 0; i < long(Species::Species::FINAL); i++) {
    hitrandata[i] = HITRAN::getIsotopologueData(i);
  }

  while (not hitranfile.at_end()) {
    std::string line = hitranfile.getline();
    auto hitpar = HITRAN::parse_parform(line, 0);
    if (hitpar.v < flow) continue;
    if (hitpar.v > fupp) break;

    auto quantums = HITRAN::parse_quantum(line, 161);
    auto s = Species::Isotope{
        Species::Isotope(hitrandata[hitpar.M][hitpar.I - 1].spec,
                         hitrandata[hitpar.M][hitpar.I - 1].isotpos)};
    std::vector<Quantum::Number> gu{s.globalQuantumNumberCount()};
    std::vector<Quantum::Number> gl{s.globalQuantumNumberCount()};
    std::vector<Quantum::Number> lu{s.localQuantumNumberCount()};
    std::vector<Quantum::Number> ll{s.localQuantumNumberCount()};

    for (size_t i = 0; i < quantums[0].size(); i++) {
      s.set_global(gu, quantums[0][i],
                   HITRAN::toQuantumType(HITRAN::QuantumTypes(i)));
      s.set_global(gl, quantums[1][i],
                   HITRAN::toQuantumType(HITRAN::QuantumTypes(i)));
      s.set_local(lu, quantums[0][i],
                  HITRAN::toQuantumType(HITRAN::QuantumTypes(i)));
      s.set_local(ll, quantums[1][i],
                  HITRAN::toQuantumType(HITRAN::QuantumTypes(i)));
    }

    Band b{s,
           Mirroring::None,
           Normalization::None,
           Population::ByLTE,
           Cutoff::None,
           Shape::VP,
           false,
           296,
           -1,
           gu,
           gl};
    auto band = std::find(database.begin(), database.end(), b);
    if (band == database.end()) {
      database.push_back(b);
      band = database.end() - 1;
    }

    Line l{s,
           hitpar.v,
           hitpar.S,
           hitpar.Elow,
           Zeeman::Model{},
           hitpar.gupp,
           hitpar.glow,
           hitpar.A,
           ll,
           lu,
           LineShape::Model{s.Spec(), hitpar.gamma_air, hitpar.gamma_self,
                            hitpar.delta_air, hitpar.n_air}};

    band->appendLine(l);
  }
  return database;
}

void saveBand(File::File<File::Operation::Write, File::Type::Xml>& file,
              const Band& band, const std::string& key) {
  file.new_child(key.c_str());

  file.add_attribute("Species", band.Species());

  std::ostringstream atoms;
  for (size_t i = 0; i < band.Atoms().size(); i++) {
    if (i) atoms << ' ';
    atoms << band.Atoms()[i];
  }
  file.add_attribute("Atoms", atoms.str());

  file.add_attribute("Mirroring", band.MirrorType());
  file.add_attribute("Normalization", band.NormType());
  file.add_attribute("Population", band.PopType());
  file.add_attribute("CutoffType", band.CutType());
  file.add_attribute("Shape", band.ShapeType());
  file.add_attribute("Zeeman", band.doZeeman());
  file.add_attribute("T0", band.T0());
  file.add_attribute("CutoffFrequency", band.FCut());
  file.add_attribute("BroadeningSpecies", band.broadspec());

  std::ostringstream globalqn;
  auto gqn = band.globalQuantumNumbers();
  globalqn << gqn.size();
  for (size_t i = 0; i < gqn.size(); i++) {
    globalqn << ' ' << gqn[i] << ' ' << band.globalUpperQuantumNumber(i) << ' '
             << band.globalLowerQuantumNumber(i);
  }
  file.add_attribute("GlobalQuantumNumbers", globalqn.str());

  std::ostringstream localqn;
  auto lqn = band.localQuantumNumbers();
  localqn << lqn.size();
  for (size_t i = 0; i < lqn.size(); i++) {
    localqn << ' ' << lqn[i];
  }
  file.add_attribute("LocalQuantumNumbers", localqn.str());

  file.add_attribute("N", band.n_lines());

  file << '\n';
  for (auto& x : band.Lines()) {
    file << x.F0() << ' ' << x.I0() << ' ' << x.E0() << ' ' << x.Ze() << ' '
         << x.Gu() << ' ' << x.Gl() << ' ' << x.A();
    file << ' ' << x.ShapeModel();
    for (size_t i = 0; i < lqn.size(); i++) {
      if (i) file << ' ';
      file << x.localUpperQuantumNumber(i) << ' '
           << x.localLowerQuantumNumber(i);
    }
    file << '\n';
  }

  file.leave_child();
}

void readBand(File::File<File::Operation::Read, File::Type::Xml>& file,
              Band& band, const std::string& key) {
  auto c = file.get_child(key.c_str());

  Species::Species spec =
      Species::toSpecies(file.get_attribute("Species").as_string());

  std::stringstream val(file.get_attribute("Atoms").as_string());
  std::vector<Species::ChargedAtom> atoms(Species::getAtomCount(spec));
  for (size_t i = 0; i < atoms.size(); i++) val >> atoms[i];
  band.spec = Species::Isotope(spec, atoms);

  band.mirroring = toMirroring(file.get_attribute("Mirroring").as_string());
  band.normalization =
      toNormalization(file.get_attribute("Normalization").as_string());
  band.population = toPopulation(file.get_attribute("Population").as_string());
  band.cutoff = toCutoff(file.get_attribute("CutoffType").as_string());
  band.shape = toShape(file.get_attribute("Shape").as_string());
  band.do_zeeman = file.get_attribute("Zeeman").as_bool();
  band.t0 = file.get_attribute("T0").as_double();
  band.fcut = file.get_attribute("CutoffFrequency").as_double();

  std::stringstream bs(file.get_attribute("BroadeningSpecies").as_string());
  int nbs;
  bs >> nbs;
  std::vector<LineShape::ModelHelper> bsdata(nbs);
  for (auto& a : bsdata) bs >> a.s;
  for (auto& a : bsdata) bs >> a.p >> a.tm >> a.pres;
  LineShape::Model model(bsdata);

  std::stringstream globalqn(
      file.get_attribute("GlobalQuantumNumbers").as_string());
  int ngqn;
  globalqn >> ngqn;
  band.global_upper.resize(Species::getGlobalQuantumNumberCount(spec));
  band.global_lower.resize(Species::getGlobalQuantumNumberCount(spec));
  for (int i = 0; i < ngqn; i++) {
    std::string a, b, c;
    globalqn >> a >> b >> c;
    auto qt = Quantum::toType(a);
    band.spec.set_global(band.global_upper, b, qt);
    band.spec.set_global(band.global_lower, c, qt);
  }

  std::stringstream localqn(
      file.get_attribute("LocalQuantumNumbers").as_string());
  int nlqn;
  localqn >> nlqn;
  std::vector<Quantum::Type> lqn(nlqn);
  for (int i = 0; i < nlqn; i++) {
    std::string a;
    localqn >> a;
    lqn[i] = Quantum::toType(a);
  }

  std::stringstream is{c.text().as_string()};
  band.lines =
      std::vector<Line>(file.get_attribute("N").as_ullong(), {band.spec});
  for (auto& line : band.lines) {
    line.model = model;
    is >> line.f0 >> line.i0 >> line.e0 >> line.zeeman >> line.gu >> line.gl >>
        line.a >> line.model;
    for (int i = 0; i < nlqn; i++) {
      std::string b, c;
      is >> b >> c;
      band.spec.set_local(line.local_upper, b, lqn[i]);
      band.spec.set_local(line.local_lower, c, lqn[i]);
    }
  }
  file.leave_child();
}

void saveBand(File::File<File::Operation::WriteBinary, File::Type::Xml>& file,
              const Band& band, const std::string& key) {
  file.new_child(key.c_str());

  file.add_attribute("Species", band.Species());

  std::ostringstream atoms;
  for (size_t i = 0; i < band.Atoms().size(); i++) {
    if (i) atoms << ' ';
    atoms << band.Atoms()[i];
  }
  file.add_attribute("Atoms", atoms.str());

  file.add_attribute("Mirroring", band.MirrorType());
  file.add_attribute("Normalization", band.NormType());
  file.add_attribute("Population", band.PopType());
  file.add_attribute("CutoffType", band.CutType());
  file.add_attribute("Shape", band.ShapeType());
  file.add_attribute("Zeeman", band.doZeeman());
  file.add_attribute("T0", band.T0());
  file.add_attribute("CutoffFrequency", band.FCut());
  file.add_attribute("BroadeningSpecies", band.broadspec());

  std::ostringstream globalqn;
  auto gqn = band.globalQuantumNumbers();
  globalqn << gqn.size();
  for (size_t i = 0; i < gqn.size(); i++) {
    globalqn << ' ' << gqn[i] << ' ' << band.globalUpperQuantumNumber(i) << ' '
             << band.globalLowerQuantumNumber(i);
  }
  file.add_attribute("GlobalQuantumNumbers", globalqn.str());

  std::ostringstream localqn;
  auto lqn = band.localQuantumNumbers();
  localqn << lqn.size();
  for (size_t i = 0; i < lqn.size(); i++) {
    localqn << ' ' << lqn[i];
  }
  file.add_attribute("LocalQuantumNumbers", localqn.str());

  file.add_attribute("N", band.n_lines());

  for (auto& x : band.Lines()) {
    file.write(x.F0());
    file.write(x.I0());
    file.write(x.E0());
    file.write(x.Ze());
    file.write(x.Gu());
    file.write(x.Gl());
    file.write(x.A());

    const auto& model = x.ShapeModel();
    for (size_t i = 0; i < model.n_spec(); i++) file.write(model[i]);

    for (size_t i = 0; i < lqn.size(); i++) {
      file.write(x.localUpperQuantumNumber(i));
      file.write(x.localLowerQuantumNumber(i));
    }
  }
  file.leave_child();
}

void readBand(File::File<File::Operation::ReadBinary, File::Type::Xml>& file,
              Band& band, const std::string& key) {
  file.get_child(key.c_str());

  Species::Species spec =
      Species::toSpecies(file.get_attribute("Species").as_string());

  std::stringstream val(file.get_attribute("Atoms").as_string());
  std::vector<Species::ChargedAtom> atoms(Species::getAtomCount(spec));
  for (size_t i = 0; i < atoms.size(); i++) val >> atoms[i];
  band.spec = Species::Isotope(spec, atoms);

  band.mirroring = toMirroring(file.get_attribute("Mirroring").as_string());
  band.normalization =
      toNormalization(file.get_attribute("Normalization").as_string());
  band.population = toPopulation(file.get_attribute("Population").as_string());
  band.cutoff = toCutoff(file.get_attribute("CutoffType").as_string());
  band.shape = toShape(file.get_attribute("Shape").as_string());
  band.do_zeeman = file.get_attribute("Zeeman").as_bool();
  band.t0 = file.get_attribute("T0").as_double();
  band.fcut = file.get_attribute("CutoffFrequency").as_double();

  std::stringstream bs(file.get_attribute("BroadeningSpecies").as_string());
  int nbs;
  bs >> nbs;

  std::stringstream globalqn(
      file.get_attribute("GlobalQuantumNumbers").as_string());
  int ngqn;
  globalqn >> ngqn;
  band.global_upper.resize(Species::getGlobalQuantumNumberCount(spec));
  band.global_lower.resize(Species::getGlobalQuantumNumberCount(spec));
  for (int i = 0; i < ngqn; i++) {
    std::string a, b, c;
    globalqn >> a >> b >> c;
    auto qt = Quantum::toType(a);
    band.spec.set_global(band.global_upper, b, qt);
    band.spec.set_global(band.global_lower, c, qt);
  }

  band.lines =
      std::vector<Line>(file.get_attribute("N").as_ullong(), {band.spec});
  for (Line& line : band.lines) {
    file.read(line.f0);
    file.read(line.i0);
    file.read(line.e0);
    file.read(line.zeeman);
    file.read(line.gu);
    file.read(line.gl);
    file.read(line.a);

    line.model.resize(nbs);
    for (size_t i = 0; i < line.model.n_spec(); i++) file.read(line.model[i]);

    for (size_t i = 0; i < line.local_lower.size(); i++) {
      file.read(line.local_upper[i]);
      file.read(line.local_lower[i]);
    }
  }
  file.leave_child();
}
}  // namespace Absorption
