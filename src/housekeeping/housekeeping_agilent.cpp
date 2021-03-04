#include "housekeeping_agilent.h"

template <size_t N>
constexpr double linear_interpolation(const std::array<double, N> &x,
                                      const std::array<double, N> &y,
                                      double t) {
  size_t i = 0;
  while (i not_eq N and t > x[i]) i++;
  if (i == 0)
    return y.front();
  else if (i == N)
    return y.back();
  else
    return y[i - 1] + (y[i] - y[i - 1]) * (t - x[i - 1]) / (x[i] - x[i - 1]);
}

constexpr double CY7(double v) {
  /*
   * calibration data for the OMEGA CY7 series temperature sensor: Standard
   * curve #10 for CY7 silicon diodes;  measurement current = 10 micro-ampere
   * +/- 0.05%.
   */

  constexpr std::array<double, 120> Temperature = {
      /* in Kelvin */
      475.0, 470.0, 460.0, 450.0, 440.0, 430.0, 420.0, 410.0, 400.0, 390.0,
      380.0, 370.0, 360.0, 350.0, 340.0, 330.0, 320.0, 310.0, 300.0, 290.0,
      280.0, 270.0, 260.0, 250.0, 240.0, 230.0, 220.0, 210.0, 200.0, 190.0,
      180.0, 170.0, 160.0, 150.0, 140.0, 130.0, 120.0, 110.0, 100.0, 95.0,
      90.0,  85.0,  80.0,  75.0,  70.0,  65.0,  60.0,  58.0,  56.0,  54.0,
      52.0,  50.0,  48.0,  46.0,  44.0,  42.0,  40.0,  38.0,  36.0,  34.0,
      32.0,  30.0,  29.0,  28.0,  27.0,  26.0,  25.0,  24.0,  23.0,  22.0,
      21.0,  20.0,  19.5,  19.0,  18.5,  18.0,  17.5,  17.0,  16.5,  16.0,
      15.5,  15.0,  14.5,  14.0,  13.5,  13.0,  12.5,  12.0,  11.5,  11.0,
      10.5,  10.0,  9.5,   9.0,   8.5,   8.0,   7.5,   7.0,   6.5,   6.0,
      5.5,   5.0,   4.8,   4.6,   4.4,   4.2,   4.0,   3.8,   3.6,   3.4,
      3.2,   3.0,   2.8,   2.6,   2.4,   2.2,   2.0,   1.8,   1.6,   1.4};

  constexpr std::array<double, 120> Voltage = {
      /* in Volt */
      0.09062, 0.10191, 0.12547, 0.14985, 0.17464, 0.19961, 0.22463, 0.24963,
      0.27456, 0.29941, 0.32416, 0.34881, 0.37337, 0.39783, 0.42221, 0.44647,
      0.47069, 0.49484, 0.51892, 0.54294, 0.56690, 0.59080, 0.61465, 0.63841,
      0.66208, 0.68564, 0.70908, 0.73238, 0.75554, 0.77855, 0.80138, 0.82404,
      0.84650, 0.86873, 0.89072, 0.91243, 0.93383, 0.95487, 0.97550, 0.98564,
      0.99565, 1.00552, 1.01525, 1.02482, 1.03425, 1.04353, 1.05267, 1.05629,
      1.05988, 1.06346, 1.06700, 1.07053, 1.07402, 1.07748, 1.08093, 1.08436,
      1.08781, 1.09131, 1.09490, 1.09864, 1.10263, 1.10702, 1.10945, 1.11212,
      1.11517, 1.11896, 1.12463, 1.13598, 1.15558, 1.17705, 1.19645, 1.21440,
      1.22314, 1.23184, 1.24053, 1.24928, 1.25810, 1.26702, 1.27607, 1.28527,
      1.29464, 1.30422, 1.31403, 1.32412, 1.33453, 1.34530, 1.35647, 1.36809,
      1.38021, 1.39287, 1.40615, 1.42013, 1.43488, 1.45048, 1.46700, 1.48443,
      1.50272, 1.52166, 1.54097, 1.56027, 1.57928, 1.59782, 1.60506, 1.61220,
      1.61920, 1.62602, 1.63263, 1.63905, 1.64529, 1.65134, 1.65721, 1.66292,
      1.66845, 1.67376, 1.67880, 1.68352, 1.68786, 1.69177, 1.69521, 1.69812};
  return linear_interpolation(Voltage, Temperature, v);
}

double PT100(double resistance) {
  /*
   * converts ohm to Kelvin for a DIN IRC 751 Pt100 temperature sensor.
   * Note: The temperature is exact to +/- 0.01 deg. Celsius in the
   * range -200 to +130 deg. Celsius! The whole DIN IRC 751 table has
   * been used for the calculation of the five coefficients.
   */

  double temperature;
  double c[5];
  int i;

  if (resistance < 18.5) return -999.999; /* underrange */

  if (resistance > 150.0) return 999.999; /* overrange */

  c[0] = 1.811049e-03;
  c[1] = 2.559214e+00;
  c[2] = 9.788229e-04;
  c[3] = -1.693120e-06;
  c[4] = 2.682225e-08;

  resistance -= 100.0;
  temperature = c[i = 4];
  while (i > 0) temperature = temperature * resistance + c[--i];
  temperature += 273.15;

  return temperature;
}

namespace Instrument::Housekeeping {
  AgilentPython::AgilentPython(const std::filesystem::path &path)
  : manual(false), error_found(false), new_data(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Agilent python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"Agilent34970A"};
  };
  
  void AgilentPython::startup(const std::string &dev, int baud) {
    PyInst = Python::ClassInstance{PyClass(dev, baud)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    runfun = Python::Function{PyInst("run")};
    download = Python::Function{PyInst("get_status_as_dict")};
  }
  
  void AgilentPython::get_data() {
    status = download();
    auto keys = status.keysDict();
    for (auto &key : keys)
      database[key] = status.fromDict<Python::Type::Double>(key).toDouble();
  }
  
  void Agilent34970A::startup(const std::string &dev, unsigned int baud) {
    try {
      port.open(dev);
      port.set_baudrate(baud);
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  void Agilent34970A::init(bool manual_press) {
    manual = manual_press;
    port.write("\x03\n");
    port.write("*IDN?\n");
    
    //     std::string name = port.readline();  // This is weird!
    
    port.write("SYSTEM:CPON 100\n");
    port.write("ROUTE:CHAN:DELAY:AUTO ON, (@101:120)\n");
    port.write("CALC:SCALE:STATE OFF, (@101:120)\n");
    
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@101)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@102)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@103)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@104)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@105)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@106)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@107)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@108)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@109)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@111)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@112)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@113)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@114)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@115)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@116)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@117)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@118)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@119)\n");
    
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@101)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@102)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@103)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@104)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@105)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@106)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@107)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@108)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@109)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@111)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@112)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@113)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@114)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@115)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@116)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@117)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@118)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@119)\n");
    
    port.write(
      "ROUTE:SCAN "
      "(@101,102,103,104,105,106,107,108,109,114,115,116,117,118,119)\n");
  }
  
  void Agilent34970A::get_data() {
    if (has_new_data) {
      std::vector<double> vals;
      std::istringstream is(results);
      std::string str;
      while (std::getline(is, str, ',')) {
        for (char c : {char(19), char(17)}) {  // Remove command charachters
          str.erase(std::remove(str.begin(), str.end(), c), str.end());
        }
        
        vals.push_back(std::stod(str));
      }
      database = {{"Cold Load Temperature", CY7(vals[1] / 5.0)},
      {"Hot Load Temperature", CY7(vals[2] / 5.0)},
      {"HEMT Temperature", CY7(vals[0] / 5.0)},
      {"Room Temperature 1", PT100(vals[6])},
      {"Room Temperature 2", PT100(vals[12])},
      {"CTS 1 Temperature 1", PT100(vals[3])},
      {"CTS 1 Temperature 2", PT100(vals[4])},
      {"CTS 2 Temperature 1", PT100(vals[9])},
      {"CTS 2 Temperature 2", PT100(vals[10] / 10.0)}};
      
      has_new_data = false;
      must_read = true;
    }
  }
  
  void Agilent34970A::run() {
    if (not has_first) {
      port.write("READ?\n");
      results = port.readline();
      T0 = Time();
      has_first = true;
      has_new_data = true;
      must_read = false;
    } else {
      if (must_read) {
        port.write("READ?\n");
        must_read = false;
        T0 = Time();  // Time it read
      }
      
      else if ((Time() - T0) >
        TimeStep(10)) {  // It has been more than 10 seconds
          results = port.readline();
          has_new_data = true;
        }
    }
  }
}
