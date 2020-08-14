#include "housekeeping.h"

namespace Instrument {
namespace Housekeeping {

}  // Housekeeping
}  // Instrument

void testAgilent34970A() {
  auto a = Instrument::Housekeeping::Agilent34970A();
  a.startup("/dev/ttyS0", 57600);
  a.init();
  a.run();
  a.get_data();
  auto d = a.data();
  a.close();
  
  for (auto& x: d) std::cout<<x.first<<' '<<x.second<<'\n';
}

int main() {testAgilent34970A();}
