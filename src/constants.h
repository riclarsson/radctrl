#ifndef constants_h
#define constants_h

#include <cmath>

/** Namespace containing several constants, physical and mathematical **/
namespace Constant {
/** power of two */
template <class T> [[gnu::const]]
constexpr auto pow2(T x) noexcept -> decltype(x * x) {
  return x * x;
}

/** power of three */
template <class T> [[gnu::const]]
constexpr auto pow3(T x) noexcept -> decltype(x * x) {
  return pow2(x) * x;
}

/** power of four */
template <class T> [[gnu::const]]
constexpr auto pow4(T x) noexcept -> decltype(x * x) {
  return pow2(pow2(x));
}

template<class T> [[gnu::const]]
constexpr auto pow5(T x) noexcept -> decltype(x * x) {
  return pow3(x)*pow2(x);
}

/** The following mathematical constants are generated in python Decimal package by the code:
    * 
    import decimal as d
    pi_str = '3.141592653589793238462643383279502884197169399375105820974944592307816406286'
    d.getcontext().prec = len(pi_str) - 1
    pi = d.Decimal(pi_str)
    one = d.Decimal('1')
    two = d.Decimal('2')
    ten = d.Decimal('10')
    print('pi =', pi)
    print('inv_pi =', 1/pi)
    print('two_pi =', two*pi)
    print('inv_two_pi =', 1/(two*pi))
    print('sqrt_pi =', pi.sqrt())
    print('inv_sqrt_pi =', 1/pi.sqrt())
    print('euler =', one.exp())
    print('inv_euler =', 1/one.exp())
    print('log10_euler =', one.exp().log10())
    print('ln_10 =', ten.ln())
    print('sqrt_2 =', two.sqrt())
    print('inv_sqrt_2 =', 1/two.sqrt())
    print('ln_2 = ', two.ln())
    print('inv_ln_2 =', 1/two.ln())
    print('sqrt_ln_2 =', two.ln().sqrt())
    print('inv_sqrt_ln_2 =', 1/two.ln().sqrt())
    print('ln_16 = ', (two**4).ln())
    *
  To improve the numerical accuracy further, insert larger pi string */

/** Pi, related to circles
   From: //www.geom.uiuc.edu/~huberty/math5337/groupe/digits.html 2019-04-01
   **/
constexpr double pi =
    3.141592653589793238462643383279502884197169399375105820974944592307816406286;

/** Inverse of pi */
constexpr double inv_pi =
    0.3183098861837906715377675267450287240689192914809128974953346881177935952685;

/** Two times pi **/
constexpr double two_pi =
    6.283185307179586476925286766559005768394338798750211641949889184615632812572;

/** Inverse of two pi **/
constexpr double inv_two_pi =
    0.1591549430918953357688837633725143620344596457404564487476673440588967976342;

/** Square root of pi */
constexpr double sqrt_pi =
    1.772453850905516027298167483341145182797549456122387128213807789852911284591;

/** Inverse of the square root of pi */
constexpr double inv_sqrt_pi =
    0.5641895835477562869480794515607725858440506293289988568440857217106424684415;

/** Euler's number */
constexpr double euler =
    2.718281828459045235360287471352662497757247093699959574966967627724076630354;

/** Inverse of Euler's number */
constexpr double inv_euler =
    0.3678794411714423215955237701614608674458111310317678345078368016974614957448;

/** Ten's logarithm of Euler's number */
constexpr double log10_euler =
    0.4342944819032518276511289189166050822943970058036665661144537831658646492089;

/** Natural logarithm of 10 */
constexpr double ln_10 =
    2.302585092994045684017991454684364207601101488628772976033327900967572609677;

/** Square root of 2 */
constexpr double sqrt_2 =
    1.414213562373095048801688724209698078569671875376948073176679737990732478462;

/** Inverse of the square root of 2 */
constexpr double inv_sqrt_2 =
    0.7071067811865475244008443621048490392848359376884740365883398689953662392311;

/** Natural logarithm of 2 */
constexpr double ln_2 =
    0.6931471805599453094172321214581765680755001343602552541206800094933936219697;

/** Inverse of the natural logarithm of 2 */
constexpr double inv_ln_2 =
    1.442695040888963407359924681001892137426645954152985934135449406931109219181;

/** Square root of natural logarithm of 2 */
constexpr double sqrt_ln_2 =
    0.8325546111576977563531646448952010476305888522644407291668291172340794351973;

/** Inverse of the square root of the natural logarithm of 2 */
constexpr double inv_sqrt_ln_2 =
    1.201122408786449794857803286095221722566764028068699423868879896733837175546;

/** Natural logarithm of 16 */
constexpr double ln_16 =
    2.772588722239781237668928485832706272302000537441021016482720037973574487879;

/** Cesium-133 Unperturbed ground-state hyperfine transition frequency [Hz]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
   **/
constexpr double Delta_nu_Cs = 9192631770;

/** Speed of light [m/s]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double speed_of_light = 299792458;

/** Speed of light convenience name [m/s] **/
constexpr double c = speed_of_light;

/** Planck constant [J s]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double planck_constant = 6.62607015e-34;

/** Planck constant convenience name [J s] **/
constexpr double h = planck_constant;

/** Reduced planck constant [J s] **/
constexpr double reduced_planck_constant = h * inv_two_pi;

/** Reduced planck constant convenience name [J s] **/
constexpr double h_bar = reduced_planck_constant;

/** Elementary charge [C]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double elementary_charge = 1.602176634e-19;

/** Elementary charge convenience name [C] **/
constexpr double e = elementary_charge;

/** Boltzmann constant [J/K]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double boltzmann_constant = 1.380649e-23;

/** Boltzmann constant convenience name [J/K] **/
constexpr double k = boltzmann_constant;

/** Inverse of Boltzmann constant convenience name [K/J] **/
constexpr double inv_k = 1 / boltzmann_constant;

/** Avogadro constant [1/mol]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double avogadro_constant = 6.02214076e23;

/** Avogadro constant convenience name [1/mol] **/
constexpr double NA = avogadro_constant;

/** Luminous efficacy of monochromatic 540 THz radiation [lm / W]
   From: https://en.wikipedia.org/wiki/2019_redefinition_of_SI_base_units
   Date: 2019-04-01
    **/
constexpr double K_cd = 683;

/** Fine structure constant [-]
   From: https://physics.nist.gov/cgi-bin/cuu/Value?alph
   Date: 2019-06-18
   Reported error: (11)
    **/
constexpr double fine_structure_constant = 7.2973525693e-3;

/** Fine structure constant convenience name [-] **/
constexpr double alpha = fine_structure_constant;

/** Rydberg constant [1/m]
    From: https://physics.nist.gov/cgi-bin/cuu/Value?ryd
    Date: 2016-06-18
    Reported error: (21)
    **/
constexpr double rydberg_constant = 10973731.568160;

/** Rydberg constant convenience name [1/m] **/
constexpr double R_inf = rydberg_constant;

/** Magnetic constant [H/m] **/
constexpr double magnetic_constant = 2 * h * alpha / (c * pow2(e));

/** Magnetic constant convenience name [H/m] **/
constexpr double mu_0 = magnetic_constant;

/** Vacuum permittivity [F/m] **/
constexpr double vacuum_permittivity = pow2(e) / (2 * h * c * alpha);

/** Vacuum permittivity convenience name [F/m] **/
constexpr double epsilon_0 = vacuum_permittivity;

/** Mass of resting electron [kg] **/
constexpr double electron_mass = 2 * h * R_inf / (c * pow2(alpha));

/** Mass of resting electron convenience name [kg] **/
constexpr double m_e = electron_mass;

/** Unified atomic mass unit [kg] 
    From: https://physics.nist.gov/cgi-bin/cuu/Value?ukg
    Date: 2020-02-18
    Reported error: (50)
    **/
constexpr double unified_atomic_mass_unit = 1.66053906660e-27;

/** Unified atomic mass unit convenience name [kg] **/
constexpr double m_u = unified_atomic_mass_unit;

/** Mass ratio of electrons to protons [-]
    From: https://physics.nist.gov/cgi-bin/cuu/Value?mpsme
    Date: 2020-01-08
    Reported error: (11)
    **/
constexpr double mass_ratio_electrons_per_proton = 1'836.152'673'43;

/** Mass of a proton [kg] */
constexpr double proton_mass = electron_mass * mass_ratio_electrons_per_proton;

/** Mass ratio of electrons to protons [-]
    From: https://physics.nist.gov/cgi-bin/cuu/Value?mnsme
    Date: 2020-01-08
    Reported error: (89)
    **/
constexpr double mass_ratio_electrons_per_neutron = 1'838.683'661'73;

/** Mass of a neutron [kg] */
constexpr double neutron_mass = electron_mass * mass_ratio_electrons_per_neutron;

/** Bohr magneton [J/T] **/
constexpr double bohr_magneton = e * h_bar / (2 * m_e);

/** Ideal gas constant [J/mol K] **/
constexpr double ideal_gas_constant = k * NA;

/** Ideal gas constant convenience name [J/mol K] **/
constexpr double R = ideal_gas_constant;

/** Doppler broadening constant squared [kg/T]^2 **/
constexpr double doppler_broadening_const_squared = 2000 * R / pow2(c);
}  // Constant

/** Namespace containing several practical unit conversions, physical and mathematical **/
namespace Conversion {
using namespace Constant;

/** Conversion constant degrees to radians and back.  Use conversion formulae instead of pure constant if possible. NOTE:  No constexpr cos etal in ARTS yet. **/
constexpr double DEG2RAD = pi / 180;
constexpr double RAD2DEG = 1 / DEG2RAD;
/** Converts degrees to radians  */
template <class T>
constexpr double deg2rad(T x) {
  return x * DEG2RAD;
}

/** Converts radians to degrees  */
template <class T>
constexpr double rad2deg(T x) {
  return x * RAD2DEG;
}

/** Returns the cosine of the deg2rad of the input  */
template <class T>
constexpr double cosd(T x) {
  return std::cos(deg2rad(x));
}

/** Returns the sine of the deg2rad of the input  */
template <class T>
constexpr double sind(T x) {
  return std::sin(deg2rad(x));
}

/** Returns the tangent of the deg2rad of the input  */
template <class T>
constexpr double tand(T x) {
  return std::tan(deg2rad(x));
}

/** Returns rad2deg of the arc-cosine of the input  */
template <class T>
constexpr double acosd(T x) {
  return rad2deg(std::acos(x));
}

/** Returns rad2deg of the arc-sine of the input  */
template <class T>
constexpr double asind(T x) {
  return rad2deg(std::asin(x));
}

/** Returns rad2deg of the arc-tangent of the input  */
template <class T>
constexpr double atand(T x) {
  return rad2deg(std::atan(x));
}

/** Returns rad2deg of the arc-tangent of inputs #T1/#T2  */
template <class T1, class T2>
constexpr double atan2d(T1 y, T2 x) {
  return rad2deg(std::atan2(y, x));
}

/** Conversion constant Kayser wavenumber to frequency and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double KAYCM2FREQ = 100 * c;
constexpr double FREQ2KAYCM = 1 / KAYCM2FREQ;
template <class T>
constexpr double kaycm2freq(T x) {
  return x * KAYCM2FREQ;
}
template <class T>
constexpr double freq2kaycm(T x) {
  return x * FREQ2KAYCM;
}

/** Conversion constant Angular wavenumber to frequency and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double ANGCM2FREQ = KAYCM2FREQ * inv_two_pi;
constexpr double FREQ2ANGCM = 1 / ANGCM2FREQ;
template <class T>
constexpr double angcm2freq(T x) {
  return x * ANGCM2FREQ;
}
template <class T>
constexpr double freq2angcm(T x) {
  return x * FREQ2ANGCM;
}

/** Conversion constant Angular frequency to frequency and back **/
template <class T>
constexpr double angfreq2freq(T x) {
  return x * inv_two_pi;
}
template <class T>
constexpr double freq2angfreq(T x) {
  return x * two_pi;
}

/** Conversion wavelength to frequency and back. **/
template <class T>
constexpr double wavelen2freq(T x) {
  return c / x;
}
template <class T>
constexpr double freq2wavelen(T x) {
  return c / x;
}

/** Conversion constant 1 atmosphere to 1 Pascal and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double ATM2PA = 101'325;
constexpr double PA2ATM = 1 / ATM2PA;
template <class T>
constexpr double atm2pa(T x) {
  return x * ATM2PA;
}
template <class T>
constexpr double pa2atm(T x) {
  return x * PA2ATM;
}

/** Conversion constant 1 bar to 1 Pascal and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double BAR2PA = 100'000;
constexpr double PA2BAR = 1 / BAR2PA;
template <class T>
constexpr double bar2pa(T x) {
  return x * BAR2PA;
}
template <class T>
constexpr double pa2bar(T x) {
  return x * PA2BAR;
}

/** Conversion constant 1 torr to 1 Pascal and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double TORR2PA = ATM2PA / 760;
constexpr double PA2TORR = 1 / TORR2PA;
template <class T>
constexpr double torr2pa(T x) {
  return x * TORR2PA;
}
template <class T>
constexpr double pa2torr(T x) {
  return x * PA2TORR;
}

/** Conversion constant Celsius to Kelvin and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double CEL2KEL = 273.15;
template <class T>
constexpr double c2k(T x) {
  return x + CEL2KEL;
}
template <class T>
constexpr double k2c(T x) {
  return x - CEL2KEL;
}

/** Conversion constant Farenheit to Kelvin and back.  Use conversion formulae instead of pure constant if possible. **/
constexpr double FAR2KELoff = 459.67;
constexpr double FAR2KELsca = 5.0/9.0;
template <class T>
constexpr double f2k(T x) {
  return (x + FAR2KELoff) * FAR2KELsca;
}
template <class T>
constexpr double k2f(T x) {
  return x / FAR2KELsca - FAR2KELoff;
}

/** Conversion constant Electron Vold to Kelvin and back.  Use conversion formulae instead of pure constant if possible. **/
template <class T>
constexpr double ev2k(T x) {
  return x * (e / k);
}
template <class T>
constexpr double k2ev(T x) {
  return x / (e / k);
}

/** Conversion constant Tesla to Gauss and back.  Use conversion formulae instead of pure constant if possible. **/
template <class T>
constexpr double t2g(T x) {
  return x * 10e4;
}
template <class T>
constexpr double g2t(T x) {
  return x * 10e-4;
}

/** Conversion from cm-1 per molecule per cm^2 to Hz per molecule per m^2 **/
constexpr double HITRAN2ARTS_LS = KAYCM2FREQ * 1e-4;
constexpr double ARTS2HITRAN_LS = 1 / HITRAN2ARTS_LS;
template <class T>
constexpr T hitran2arts_linestrength(T x) {
  return x * HITRAN2ARTS_LS;
}
template <class T>
constexpr T arts2hitran_linestrength(T x) {
  return x * ARTS2HITRAN_LS;
}

/** Conversion from cm-1 per atmosphere to Hz per Pascal **/
constexpr double HITRAN2ARTS_GAMMA = KAYCM2FREQ / ATM2PA;
constexpr double ARTS2HITRAN_GAMMA = 1 / HITRAN2ARTS_GAMMA;
template <class T>
constexpr T hitran2arts_broadening(T x) {
  return x * HITRAN2ARTS_GAMMA;
}
template <class T>
constexpr T arts2hitran_broadening(T x) {
  return x * ARTS2HITRAN_GAMMA;
}

/** Conversion from cm-1 to Joule **/
constexpr double HITRAN2ARTS_ENERGY = h * KAYCM2FREQ;
constexpr double ARTS2HITRAN_ENERGY = 1 / HITRAN2ARTS_ENERGY;
template <class T>
constexpr T hitran2arts_energy(T x) {
  return x * HITRAN2ARTS_ENERGY;
}
template <class T>
constexpr T arts2hitran_energy(T x) {
  return x * ARTS2HITRAN_ENERGY;
}

template <class T>
constexpr T angstrom2meter(T x) {
  return x * 1e-10;
}
template <class T>
constexpr T meter2angstrom(T x) {
  return x * 1e10;
}
}  // Conversion

#endif  // constants_h
