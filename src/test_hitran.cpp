#include "file.h"
#include "hitran.h"
#include "lbl.h"

void test001(std::string file) {
  File::File<File::Operation::Read, File::Type::Raw> f(file);

  auto bands = Absorption::parse_hitran_with_qns(f, -1, 1e99);
  std::cout << "Done reading" << std::endl;
  File::File<File::Operation::Write, File::Type::Xml> wtxt(file + "txt.xml");
  saveBands(wtxt, bands);
  wtxt.close();

  //   File::File<File::Operation::WriteBinary, File::Type::Xml>
  //   wbin(file+"bin.xml"); saveBands(wbin, bands); wbin.close();
}

int main(int argc, char** argv) {
  if (argc == 2) test001(std::string{argv[1]});
}
