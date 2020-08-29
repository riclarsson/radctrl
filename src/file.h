#ifndef file_h
#define file_h

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <pugixml.hpp>
#include <string>
#include <vector>

#include "enums.h"

namespace File {
ENUMCLASS(Operation, unsigned char, Read, Write, Append, ReadBinary,
          WriteBinary, AppendBinary)

ENUMCLASS(Type, unsigned char, Raw, Xml)

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &x) {
  if constexpr (std::is_arithmetic<T>::value) {
    for (auto &b : x) os << b << ' ';
  } else {
    for (auto &b : x) os << b << '\n';
  }
  return os;
}

template <class T>
std::istream &operator>>(std::istream &is, std::vector<T> &x) {
  for (auto &b : x) is >> b;
  return is;
}

template <Operation X, Type Y, int PREC = 15>
class File {
  std::filesystem::path path;
  std::fstream fil;
  pugi::xml_document doc;
  pugi::xml_parse_result result;
  pugi::xml_node root;
  pugi::xml_node child;
  pugi::xml_node oldchild;

 public:
  File(const std::string &p) : path(p) {
    if constexpr (Y == Type::Xml)
      if (path.extension() not_eq ".xml") path = path.replace_extension(".xml");

    if constexpr (X == Operation::Read or X == Operation::ReadBinary) {
      if (not std::filesystem::exists(path)) {
        std::ostringstream os;
        os << '"' << path << '"' << " does not exist.  Cannot read it.";
        throw std::runtime_error(os.str());
      }
    }

    // FIXME:
    static_assert(Y == Type::Raw or Y == Type::Xml);

    if constexpr (Y == Type::Raw) {
      if constexpr (X == Operation::Read) {
        fil.open(path, std::ios::in);
      } else if constexpr (X == Operation::ReadBinary) {
        fil.open(path, std::ios::in | std::ios::binary);
      } else if constexpr (X == Operation::Write) {
        fil.open(path, std::ios::out);
        fil << std::setprecision(PREC);
      } else if constexpr (X == Operation::WriteBinary) {
        fil.open(path, std::ios::out | std::ios::binary);
      } else if constexpr (X == Operation::Append) {
        fil.open(path, std::ios::out | std::ios::app);
        fil << std::setprecision(PREC);
      } else if constexpr (X == Operation::AppendBinary) {
        fil.open(path, std::ios::out | std::ios::binary | std::ios::app);
      }
    } else if constexpr (Y == Type::Xml) {
      if constexpr (X == Operation::Read) {
        result = doc.load_file(path.c_str());
        oldchild = child = root = doc.document_element();
      } else if constexpr (X == Operation::ReadBinary) {
        result = doc.load_file(path.c_str());
        oldchild = child = root = doc.document_element();
        fil.open((path.string() + std::string(".bin")).c_str(),
                 std::ios::in | std::ios::binary);
      } else if constexpr (X == Operation::Write) {
        auto declarationNode = doc.append_child(pugi::node_declaration);
        declarationNode.append_attribute("version") = "1.0";
        oldchild = child = root = doc.append_child("RADCTRL");
      } else if constexpr (X == Operation::WriteBinary) {
        auto declarationNode = doc.append_child(pugi::node_declaration);
        declarationNode.append_attribute("version") = "1.0";
        oldchild = child = root = doc.append_child("RADCTRL");
        fil.open((path.string() + std::string(".bin")).c_str(),
                 std::ios::out | std::ios::binary);
      } else if constexpr (X == Operation::Append) {
        result = doc.load_file(path.c_str());
        oldchild = child = root = doc.document_element();
      } else if constexpr (X == Operation::AppendBinary) {
        result = doc.load_file(path.c_str());
        fil.open((path.string() + std::string(".bin")).c_str(),
                 std::ios::out | std::ios::binary | std::ios::app);
        oldchild = child = root = doc.document_element();
      }
    }
  }

  std::string getline(const char limit = '\n') noexcept {
    static_assert(X == Operation::Read and Y == Type::Raw,
                  "Bad file type and operation");
    std::string line;
    std::getline(fil, line, limit);
    return line;
  }

  bool at_end() const noexcept {
    static_assert(Y == Type::Raw);
    return fil.eof();
  }

  template <typename T>
  File &operator<<(const T &x) {
    static_assert(X == Operation::Append or X == Operation::Write,
                  "Bad operation");
    if constexpr (Y == Type::Raw)
      fil << x;
    else if constexpr (Y == Type::Xml) {
      std::ostringstream os;
      os << std::setprecision(PREC);
      os << x;
      child.append_child(pugi::node_pcdata).set_value(os.str().c_str());
    }
    return *this;
  }

  template <typename T>
  File &operator>>(T &x) {
    static_assert(X == Operation::Read and (Y == Type::Raw or Y == Type::Xml),
                  "Bad file type and operation");
    if constexpr (Y == Type::Raw)
      fil >> x;
    else if constexpr (Y == Type::Xml) {
      std::istringstream is(child.text().as_string());
      is >> x;
    }
    return *this;
  }

  template <typename T>
  size_t write(const T &x, size_t n = sizeof(T)) {
    static_assert(
        (X == Operation::WriteBinary or X == Operation::AppendBinary) and
            (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    fil.write(reinterpret_cast<const char *>(&x), n);
    return n;
  }

  template <typename T>
  size_t write(T *x, size_t n) {
    static_assert(
        (X == Operation::WriteBinary or X == Operation::AppendBinary) and
            (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    fil.write(reinterpret_cast<const char *>(x), n);
    return n;
  }

  template <typename T>
  size_t write(const std::vector<T> &x) {
    static_assert(
        (X == Operation::WriteBinary or X == Operation::AppendBinary) and
            (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    size_t n = 0;
    for (auto &v : x) n += write(v);
    return n;
  }

  template <typename T>
  size_t read(T &x, size_t n = sizeof(T)) {
    static_assert(
        X == Operation::ReadBinary and (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    fil.read(reinterpret_cast<char *>(&x), n);
    return n;
  }

  template <typename T>
  size_t read(T *x, size_t n) {
    static_assert(
        X == Operation::ReadBinary and (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    fil.read(reinterpret_cast<char *>(x), n);
    return n;
  }

  template <typename T>
  size_t read(std::vector<T> &x) {
    static_assert(
        X == Operation::ReadBinary and (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    size_t n = 0;
    for (auto &v : x) n += read(v);
    return n;
  }

  template <bool relative = true>
  void seek(long n) {
    static_assert(
        X == Operation::ReadBinary and (Y == Type::Raw or Y == Type::Xml),
        "Bad file type and operation");
    if constexpr (relative)
      fil.seekg(n, std::ios_base::cur);
    else
      fil.seekg(n);
  }

  void close() {
    static_assert(Y == Type::Raw or Y == Type::Xml);
    if constexpr (Y == Type::Raw)
      fil.close();
    else if constexpr (Y == Type::Xml) {
      if constexpr (X == Operation::Write or X == Operation::WriteBinary or
                    X == Operation::Append or X == Operation::AppendBinary) {
        if (not doc.save_file(path.c_str(), "")) {
          std::ostringstream os;
          os << "Cannot save file to:" << '\n'
             << '\t' << path << '\n'
             << "Exiting";
          throw std::runtime_error(os.str());
        }
        if constexpr (X == Operation::AppendBinary or
                      X == Operation::WriteBinary)
          fil.close();
      }
    }
  }

  void new_child(const std::string &name) {
    static_assert(Y == Type::Xml and
                  (X == Operation::Write or X == Operation::WriteBinary or
                   X == Operation::Append or X == Operation::AppendBinary));
    oldchild = child;
    child = child.append_child(name.c_str());
  }

  pugi::xml_node get_child(const std::string &name) {
    static_assert(Y == Type::Xml);
    oldchild = child;
    child = oldchild.child(name.c_str());
    if (not child) {
      child = oldchild;
      std::ostringstream os;
      os << "Problem getting child of XML document: " << path << '\n';
      os << "Child does not exist: " << name << '\n';
      throw std::runtime_error(os.str());
    }
    return child;
  }

  void leave_child() {
    static_assert(Y == Type::Xml);
    child = oldchild;
  }

  template <class T>
  void add_attribute(const std::string &name, const T &value) {
    static_assert(Y == Type::Xml and
                  (X == Operation::Write or X == Operation::WriteBinary or
                   X == Operation::Append or X == Operation::AppendBinary));
    std::ostringstream os;
    os << std::boolalpha << std::setprecision(PREC) << value;
    child.append_attribute(name.c_str()) = os.str().c_str();
  }

  template <class T>
  void add_attribute(const std::string &name, const std::vector<T> &values) {
    static_assert(Y == Type::Xml and
                  (X == Operation::Write or X == Operation::WriteBinary or
                   X == Operation::Append or X == Operation::AppendBinary));
    std::ostringstream os;
    os << std::boolalpha << std::setprecision(PREC);
    for (size_t i = 0; i < values.size(); i++) {
      os << values[i];
      if (i < values.size() - 1) os << ',';
    }
    child.append_attribute(name.c_str()) = os.str().c_str();
  }

  template <class T, size_t N>
  void add_attribute(const std::string &name, const std::array<T, N> &values) {
    static_assert(Y == Type::Xml and
                  (X == Operation::Write or X == Operation::WriteBinary or
                   X == Operation::Append or X == Operation::AppendBinary));
    std::ostringstream os;
    os << std::boolalpha << std::setprecision(PREC);
    for (size_t i = 0; i < N; i++) {
      os << values[i];
      if (i < values.size() - 1) os << ',';
    }
    child.append_attribute(name.c_str()) = os.str().c_str();
  }

  pugi::xml_attribute get_attribute(const std::string &name) {
    static_assert(Y == Type::Xml);
    return child.attribute(name.c_str());
  }

  template <typename T>
  std::vector<T> get_vector_attribute(const std::string &name) {
    std::vector<T> out;
    std::stringstream x(get_attribute(name).as_string());
    while (not x.eof()) {
      std::stringstream ss;
      std::string y;
      T z;  // requires basic initialization

      std::getline(x, y, ',');
      ss << y;
      ss >> z;
      out.push_back(z);
    }
    return out;
  }

  template <typename T, size_t N>
  std::array<T, N> get_array_attribute(const std::string &name) {
    std::array<T, N> out;
    std::stringstream x(get_attribute(name).as_string());
    for (size_t i = 0; i < N; i++) {
      std::stringstream ss;
      std::string y;

      std::getline(x, y, ',');
      ss << y;
      ss >> out[i];
    }
    return out;
  }

  void inc_size(const int n = 1) {
    static_assert(Y == Type::Xml);
    child.attribute("size") = child.attribute("size").as_int() + n;
  }

  int size() {
    static_assert(Y == Type::Xml);
    int x = child.attribute("size").as_int();
    return x ? x : 1;
  }
};  // File

std::vector<std::string> Devices(std::vector<std::string> contains,
                                 size_t maxN = 9);
}  // namespace File

#endif  // file_h
