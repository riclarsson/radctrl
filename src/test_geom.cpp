#include "file.h"
#include "geom.h"

void test001() {
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  std::cout << "WGS84: " << wgs84 << '\n';
}

void test002() {
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  std::cout<<'\n'; {
    std::cout << "pos converted from xyz\n";
    auto x = Geom::Pos<Geom::PosType::Xyz>({a+1, 0, 0});
    auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
    std::cout << "x-pos (xyz): " << x << '\n';
    std::cout << "x-pos (sph): " << xs << '\n';
    std::cout << "x-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from spherical\n";
    auto xs = Geom::Pos<Geom::PosType::Spherical>({a+1, 0, 0});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xs, wgs84);
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{xs, wgs84};
    std::cout << "x-pos (xyz): " << x << '\n';
    std::cout << "x-pos (sph): " << xs << '\n';
    std::cout << "x-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from ellipsoidal\n";
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>({1, 0, 0});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xe, wgs84);
    auto xs = Geom::Pos<Geom::PosType::Spherical>(xe, wgs84);
    std::cout << "x-pos (xyz): " << x << '\n';
    std::cout << "x-pos (sph): " << xs << '\n';
    std::cout << "x-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from xyz\n";
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, a+1, 0});
    auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
    std::cout << "y-pos (xyz): " << x << '\n';
    std::cout << "y-pos (sph): " << xs << '\n';
    std::cout << "y-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from spherical\n";
    auto xs = Geom::Pos<Geom::PosType::Spherical>({a+1, 0, 90});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xs, wgs84);
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{xs, wgs84};
    std::cout << "y-pos (xyz): " << x << '\n';
    std::cout << "y-pos (sph): " << xs << '\n';
    std::cout << "y-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from ellipsoidal\n";
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>({1, 0, 90});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xe, wgs84);
    auto xs = Geom::Pos<Geom::PosType::Spherical>(xe, wgs84);
    std::cout << "y-pos (xyz): " << x << '\n';
    std::cout << "y-pos (sph): " << xs << '\n';
    std::cout << "y-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from xyz\n";
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, 0, b+1});
    auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
    std::cout << "z-pos (xyz): " << x << '\n';
    std::cout << "z-pos (sph): " << xs << '\n';
    std::cout << "z-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from spherical\n";
    auto xs = Geom::Pos<Geom::PosType::Spherical>({b+1, 90, 0});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xs, wgs84);
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{xs, wgs84};
    std::cout << "z-pos (xyz): " << x << '\n';
    std::cout << "z-pos (sph): " << xs << '\n';
    std::cout << "z-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from ellipsoidal\n";
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>({1, 90, 0});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xe, wgs84);
    auto xs = Geom::Pos<Geom::PosType::Spherical>(xe, wgs84);
    std::cout << "z-pos (xyz): " << x << '\n';
    std::cout << "z-pos (sph): " << xs << '\n';
    std::cout << "z-pos (ell): " << xe << '\n';
  }
  std::cout<<'\n'; {
    std::cout << "pos converted from xyz\n";
    auto x = Geom::Pos<Geom::PosType::Xyz>({3*a, 2*a, 1*b+1});
    auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
    std::cout << "some-pos (xyz): " << x << '\n';
    std::cout << "some-pos (sph): " << xs << '\n';
    std::cout << "some-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from spherical\n";
    auto xs = Geom::Pos<Geom::PosType::Spherical>({2.38591e+07, 15.4519, 33.6901});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xs, wgs84);
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{xs, wgs84};
    std::cout << "some-pos (xyz): " << x << '\n';
    std::cout << "some-pos (sph): " << xs << '\n';
    std::cout << "some-pos (ell): " << xe << '\n';
  }
  
  std::cout<<'\n'; {
    std::cout << "pos converted from ellipsoidal\n";
    auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>({1.74825e+07, 15.4783, 33.6901});
    auto x = Geom::Pos<Geom::PosType::Xyz>(xe, wgs84);
    auto xs = Geom::Pos<Geom::PosType::Spherical>(xe, wgs84);
    std::cout << "z-pos (xyz): " << x << '\n';
    std::cout << "z-pos (sph): " << xs << '\n';
    std::cout << "z-pos (ell): " << xe << '\n';
  }
}

void test003() {
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  auto x = Geom::Pos<Geom::PosType::Xyz>({3*a, 2*a, 1*b+1});
  auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
  auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
  
  std::cout<<std::endl; {
    auto dx = Geom::Los<Geom::LosType::Xyz>({5,4,2});
    auto dxs_x = Geom::Los<Geom::LosType::Spherical>(dx, x, wgs84);
    auto dxs_xs = Geom::Los<Geom::LosType::Spherical>(dx, xs, wgs84);
    auto dxs_xe = Geom::Los<Geom::LosType::Spherical>(dx, xe, wgs84);
    
    std::cout << "Los (xyz-xyz): " << dx << '\n';
    std::cout << "Los (sph-xyz): " << dxs_x << '\n';
    std::cout << "Los (sph-sph): " << dxs_xs << '\n';
    std::cout << "Los (sph-ell): " << dxs_xe << '\n';
  }
  
  std::cout<<std::endl; {
    auto dxs = Geom::Los<Geom::LosType::Spherical>({5.12971, 67.6429, 6.7082});
    auto dx_x = Geom::Los<Geom::LosType::Xyz>(dxs, x, wgs84);
    auto dx_xs = Geom::Los<Geom::LosType::Xyz>(dxs, xs, wgs84);
    auto dx_xe = Geom::Los<Geom::LosType::Xyz>(dxs, xe, wgs84);
    
    std::cout << "Los (xyz-xyz): " << dxs << '\n';
    std::cout << "Los (sph-xyz): " << dx_x << '\n';
    std::cout << "Los (sph-sph): " << dx_xs << '\n';
    std::cout << "Los (sph-ell): " << dx_xe << '\n';
  }
}

void test004()
{
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  auto x = Geom::Pos<Geom::PosType::Xyz>({3*a, 2*a, 1*b+1});
  auto xs = Geom::Pos<Geom::PosType::Spherical>{x, wgs84};
  auto xe = Geom::Pos<Geom::PosType::Ellipsoidal>{x, wgs84};
  auto dx = Geom::Los<Geom::LosType::Xyz>({5,4,2});
  auto dxs = Geom::Los<Geom::LosType::Spherical>(dx, x, wgs84);
  
  std::cout << "Nav (xyz-xyz): " << Geom::Nav(x, dx, wgs84) << '\n';
  std::cout << "Nav (xyz-sph): " << Geom::Nav(x, dxs, wgs84) << '\n';
  std::cout << "Nav (sph-xyz): " << Geom::Nav(xs, dx, wgs84) << '\n';
  std::cout << "Nav (sph-sph): " << Geom::Nav(xs, dxs, wgs84) << '\n';
  std::cout << "Nav (ell-xyz): " << Geom::Nav(xe, dx, wgs84) << '\n';
  std::cout << "Nav (ell-sph): " << Geom::Nav(xe, dxs, wgs84) << '\n';
}

void test005()
{
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({a+1, 0, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({1,0,0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, a+1, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({1,0,0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, 0, b+1});
    auto dx = Geom::Los<Geom::LosType::Xyz>({1,0,0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({a+1, 0, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 1, 0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, a+1, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 1, 0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, 0, b+1});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 1, 0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({a+1, 0, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 0, 1});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, a+1, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 0, 1});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({0, 0, b+1});
    auto dx = Geom::Los<Geom::LosType::Xyz>({0, 0, 1});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
  std::cout<<std::endl; {
    auto x = Geom::Pos<Geom::PosType::Xyz>({a+1, 0, 0});
    auto dx = Geom::Los<Geom::LosType::Xyz>({2,0,0});
    auto n = Geom::Nav(x, dx, wgs84);
    std::cout << n << '\n' << '\t' << Geom::Nav(n, - Distance<DistanceType::meter>{1e7})<< '\n' << '\t' << Geom::Nav(n, Distance<DistanceType::meter>{1e7}) << '\n';
  }
}

void test006()
{
  constexpr auto a = Length<LengthType::meter>{6'378'137.0};
  constexpr auto b = Length<LengthType::meter>{6'356'752.314245};
  auto wgs84 = Geom::Ellipsoid(a, std::sqrt((a*a - b*b)/(a*a)));
  
  auto x = Geom::Pos<Geom::PosType::Xyz>({3*a, 2*a, 1*b+1});
  auto dx = Geom::Los<Geom::LosType::Xyz>({1,1,1});
  auto n = Geom::Nav(x, dx, wgs84);
  
  File::File<File::Operation::Write, File::Type::Xml> fraw("test_geom_test001.xml");
  Geom::saveNav(fraw, n);
  fraw.close();
  
  File::File<File::Operation::WriteBinary, File::Type::Xml> fbin("test_geom_test002.xml");
  Geom::saveNav(fbin, n);
  fbin.close();
  
  Geom::Nav nraw;
  File::File<File::Operation::Read, File::Type::Xml> frawread("test_geom_test001.xml");
  std::cout<< "Print1: " << n << '\n';
  Geom::readNav(frawread, nraw);
  std::cout<< "Read1:  " << nraw << '\n';
  
  Geom::Nav nraw2;
  File::File<File::Operation::ReadBinary, File::Type::Xml> fbinread("test_geom_test002.xml");
  std::cout<< "Print1: " << n << '\n';
  std::cout<< "Print2: " << Geom::Nav(n, Distance<DistanceType::meter>{3e3}) << '\n';
  Geom::readNav(fbinread, nraw2);
  std::cout << "Read1:  " << nraw2 << '\n';
}

int main() {
  std::cout<<std::setprecision(20);
  test001();  // Ellipsoid basic
  std::cout<<std::endl;
  test002();  // Pos basic conversion
  std::cout<<std::endl;
  test003();  // Los basic conversion
  std::cout<<std::endl;
  test004();  // Nav basic conversion
  std::cout<<std::endl;
  test005();  // Nav basic propagation
  std::cout<<std::endl;
  test006();  // Nav IO
}
