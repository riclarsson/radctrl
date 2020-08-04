#ifndef python_interface_h
#define python_interface_h

#include <filesystem>
#include <iostream>
#include <iomanip>

#include <pybind11/embed.h>

#include "enums.h"
namespace py = pybind11;

namespace Python {
inline py::scoped_interpreter createPython() noexcept {return {};}

inline py::object getScope() noexcept {return py::module::import("__main__").attr("__dict__");}

ENUMCLASS(Type, unsigned char,
          None,
          Bool,
          Int,
          Double,
          String)

template <Type X=Type::None>
class Object {
public:
  py::object Obj;
  Object() noexcept {}
  Object(py::object && x) noexcept : Obj(std::move(x)) {}
  Object& operator=(py::object && x) noexcept {Obj(std::move(x)); return *this;}
  Object(Object<Type::None> && x) noexcept : Obj(std::move(x.Obj)) {}
  
  std::string toString() const {static_assert(X == Type::String); return Obj.cast<std::string>();}
  double toDouble() const {static_assert(X == Type::Double); return Obj.cast<double>();}
  int toInt() const {static_assert(X == Type::Int); return Obj.cast<int>();}
  int toBool() const {static_assert(X == Type::Bool); return Obj.cast<bool>();}
};
  
class Function {
  py::object Fun;
public:
  Function() {}
  Function(py::object && X) noexcept : Fun(std::move(X)) {}
  
  // template <const char * KEY>
  Function(const char * const KEY) try : Fun(getScope()[KEY])
  {} catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot generate function with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
  
  template <typename ... A>
  Object<> operator()(A ... a) const try {
    return Object<>(Fun(a ...));
  } catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot call function with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
};

class ClassInstance {
  py::object Inst;
public:
  ClassInstance() noexcept {}
  ClassInstance(py::object && X) try : Inst(std::move(X)) {}
  catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot generate instance with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
  
  // template <const char * KEY>
  Function operator()(const char * const KEY) const try {
    return Function(Inst.attr(KEY));
  } catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot generate function with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
};

// template <const char * KEY>
class ClassInterface {
  py::object Obj;
public:
  ClassInterface() {}
  ClassInterface(const char * const KEY) try : Obj(getScope()[KEY]) {}
  catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot generate class with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
  
  template <typename ... A>
  ClassInstance operator()(A ... a) const try {
    return ClassInstance(Obj(a ...));
  } catch (const std::exception & c) {
    std::ostringstream os;
    os << "Cannot generate instance with error:\n" << c.what();
    throw std::runtime_error(os.str());
  }
};
}

#endif  // python_interface_h
