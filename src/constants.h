#ifndef constants_h
#define constants_h

#include <cmath>

/** Namespace containing several constants, physical and mathematical **/
namespace Constant {
/** power of two */
template <class T>
[[gnu::const]] constexpr auto pow2(T x) noexcept -> decltype(x * x) {
  return x * x;
}

/** power of three */
template <class T>
[[gnu::const]] constexpr auto pow3(T x) noexcept -> decltype(x * x) {
  return pow2(x) * x;
}

/** power of four */
template <class T>
[[gnu::const]] constexpr auto pow4(T x) noexcept -> decltype(x * x) {
  return pow2(pow2(x));
}

template <class T>
[[gnu::const]] constexpr auto pow5(T x) noexcept -> decltype(x * x) {
  return pow3(x) * pow2(x);
}

/** The following mathematical constants are generated in python Decimal package
  by the code:
    *
    import decimal as d
    pi_str =
  '3.141592653589793238462643383279502884197169399375105820974944592307816406286'
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
constexpr double neutron_mass =
    electron_mass * mass_ratio_electrons_per_neutron;

/** Bohr magneton [J/T] **/
constexpr double bohr_magneton = e * h_bar / (2 * m_e);

/** Ideal gas constant [J/mol K] **/
constexpr double ideal_gas_constant = k * NA;

/** Ideal gas constant convenience name [J/mol K] **/
constexpr double R = ideal_gas_constant;

/** Doppler broadening constant squared [kg/T]^2 **/
constexpr double doppler_broadening_const_squared = 2000 * R / pow2(c);

/** Zeroes in the first_order if first kind Bessel function */
constexpr std::array<double, 128> bessel_1storder_1stkind_zeroes{
    3.831705970207512468306276787188835442066192626953125,
    7.0155866698156188476787065155804157257080078125,
    10.1734681350627216289694842998869717121124267578125,
    13.323691936314222772352877655066549777984619140625,
    16.470630050877634431572005269117653369903564453125,
    19.6158585104682430255707004107534885406494140625,
    22.7600843805927723906279425136744976043701171875,
    25.90367208761838213604278280399739742279052734375,
    29.0468285349168553466370212845504283905029296875,
    32.1896799109744051747838966548442840576171875,
    35.332307550083868363799410872161388397216796875,
    38.47476623477161439268456888385117053985595703125,
    41.6170942128144503158182487823069095611572265625,
    44.75931899765281940517525072209537029266357421875,
    47.90146088718544348239447572268545627593994140625,
    51.0435351835715067636556341312825679779052734375,
    54.185553641061318330685026012361049652099609375,
    57.32752543790100929754771641455590724945068359375,
    60.4694578453474917978383018635213375091552734375,
    63.6113566984812308646723977290093898773193359375,
    66.753226734098490169344586320221424102783203125,
    69.89507183749577734488411806523799896240234375,
    73.0368952255738435042076162062585353851318359375,
    76.178699584641464070955407805740833282470703125,
    79.320487175476301899834652431309223175048828125,
    82.462259914373561286993208341300487518310546875,
    85.604019436350228033916209824383258819580078125,
    88.745767144926304581531439907848834991455078125,
    91.887504251694991808108170516788959503173828125,
    95.02923180804469893701025284826755523681640625,
    98.1709507307907784934286610223352909088134765625,
    101.3126618230387379071544273756444454193115234375,
    104.454365791282754116764408536255359649658203125,
    107.5960632595091652774499380029737949371337890625,
    110.737754780899223305823397822678089141845703125,
    113.8794408475949921921710483729839324951171875,
    117.02112189889243154539144597947597503662109375,
    120.1627983281490088529608328826725482940673828125,
    123.3044704886357152417986071668565273284912109375,
    126.446138698516591603038250468671321868896484375,
    129.58780324510399850623798556625843048095703125,
    132.729464388509626360246329568326473236083984375,
    135.871122364788988079453702084720134735107421875,
    139.012777388659714006280410103499889373779296875,
    142.15442965585901902159093879163265228271484375,
    145.29607934519589207411627285182476043701171875,
    148.4377266203422323087579570710659027099609375,
    151.57937163140144321005209349095821380615234375,
    154.72101451628594759313273243606090545654296875,
    157.8626554019302830056403763592243194580078125,
    161.004294405362003317350172437727451324462890625,
    164.1459316346496279948041774332523345947265625,
    167.28756718974409523070789873600006103515625,
    170.429201163226622384172515012323856353759765625,
    173.570833640975934031303040683269500732421875,
    176.712464702763753621184150688350200653076171875,
    179.854094422788392648726585321128368377685546875,
    182.99572287015297433754312805831432342529296875,
    186.137350109295510947049479000270366668701171875,
    189.27897620037600745490635745227336883544921875,
    192.4206011996257075225003063678741455078125,
    195.562225159662574469621176831424236297607421875,
    198.703848129777043141075409948825836181640625,
    201.845470156190884836178156547248363494873046875,
    204.987091282292340110871009528636932373046875,
    208.12871154885004898460465483367443084716796875,
    211.27033099420776807164656929671764373779296875,
    214.411949654461977843311615288257598876953125,
    217.55356756362419901051907800137996673583984375,
    220.695184753769353847019374370574951171875,
    223.8368012551717356473091058433055877685546875,
    226.978417096429467392226797528564929962158203125,
    230.120032304579098081376287154853343963623046875,
    233.26164690520062094947206787765026092529296875,
    236.403260922514306230368674732744693756103515625,
    239.544874379469860059543862007558345794677734375,
    242.68648729782870532289962284266948699951171875,
    245.828099698239810777522507123649120330810546875,
    248.969711600309921095686149783432483673095703125,
    252.111323022668585736028035171329975128173828125,
    255.2529339830281287504476495087146759033203125,
    258.3945444982394974431372247636318206787109375,
    261.53615458434404672516393475234508514404296875,
    264.67776425662151495998841710388660430908203125,
    267.81937352963456078214221633970737457275390625,
    270.96098241727071354034706018865108489990234375,
    274.102590932780685761827044188976287841796875,
    277.24419908881458241012296639382839202880859375,
    280.3858068974556090324767865240573883056640625,
    283.52741437025139248362393118441104888916015625,
    286.6690215182434258167631924152374267578125,
    289.810628351994409968028776347637176513671875,
    292.9522348816138901383965276181697845458984375,
    296.09384111678247109011863358318805694580078125,
    299.23544706677415661033592186868190765380859375,
    302.3770527404775521063129417598247528076171875,
    305.51865814641558927178266458213329315185546875,
    308.6602632927643981020082719624042510986328125,
    311.80186818737047360627911984920501708984375,
    314.94347283776716039938037283718585968017578125,
    318.08507725119039832861744798719882965087890625,
    321.226681434592819641693495213985443115234375,
    324.3682853946578461545868776738643646240234375,
    327.50988913781247902079485356807708740234375,
    330.65149267023940637955092824995517730712890625,
    333.7930959978885994132724590599536895751953125,
    336.9346991264879989103064872324466705322265625,
    340.076302061554088140837848186492919921875,
    343.21790480840127202100120484828948974609375,
    346.359507372151028903317637741565704345703125,
    349.50110975774094868029351346194744110107421875,
    352.64271196993240664596669375896453857421875,
    355.78431401331880579164135269820690155029296875,
    358.9259158923326822332455776631832122802734375,
    362.06751761125264010843238793313503265380859375,
    365.20911917421011594342417083680629730224609375,
    368.35072058519568827250623144209384918212890625,
    371.49232184806481882333173416554927825927734375,
    374.63392296654370738906436599791049957275390625,
    377.77552394423463510975125245749950408935546875,
    380.91712478462096669318270869553089141845703125,
    384.05872549107215263575199060142040252685546875,
    387.200326066848219852545298635959625244140625,
    390.34192651510437599426950328052043914794921875,
    393.48352683889498848657240159809589385986328125,
    396.62512704117756356936297379434108734130859375,
    399.76672712481678217955050058662891387939453125,
    402.90832709258796739959507249295711517333984375};
}  // namespace Constant

/** Namespace containing several practical unit conversions, physical and
 * mathematical **/
namespace Conversion {
using namespace Constant;

/** Conversion constant degrees to radians and back.  Use conversion formulae
 * instead of pure constant if possible. NOTE:  No constexpr cos etal in ARTS
 * yet. **/
constexpr double DEG2RAD = pi / 180;
constexpr double RAD2DEG = 1 / DEG2RAD;
/** Converts degrees to radians  */
template <class T>
[[gnu::const]] constexpr double deg2rad(T x) {
  return x * DEG2RAD;
}

/** Converts radians to degrees  */
template <class T>
[[gnu::const]] constexpr double rad2deg(T x) {
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

/** Conversion constant Kayser wavenumber to frequency and back.  Use conversion
 * formulae instead of pure constant if possible. **/
constexpr double KAYCM2FREQ = 100 * c;
constexpr double FREQ2KAYCM = 1 / KAYCM2FREQ;
template <class T>
[[gnu::const]] constexpr double kaycm2freq(T x) {
  return x * KAYCM2FREQ;
}
template <class T>
[[gnu::const]] constexpr double freq2kaycm(T x) {
  return x * FREQ2KAYCM;
}

/** Conversion constant Angular wavenumber to frequency and back.  Use
 * conversion formulae instead of pure constant if possible. **/
constexpr double ANGCM2FREQ = KAYCM2FREQ * inv_two_pi;
constexpr double FREQ2ANGCM = 1 / ANGCM2FREQ;
template <class T>
[[gnu::const]] constexpr double angcm2freq(T x) {
  return x * ANGCM2FREQ;
}
template <class T>
[[gnu::const]] constexpr double freq2angcm(T x) {
  return x * FREQ2ANGCM;
}

/** Conversion constant Angular frequency to frequency and back **/
template <class T>
[[gnu::const]] constexpr double angfreq2freq(T x) {
  return x * inv_two_pi;
}
template <class T>
[[gnu::const]] constexpr double freq2angfreq(T x) {
  return x * two_pi;
}

/** Conversion wavelength to frequency and back. **/
template <class T>
[[gnu::const]] constexpr double wavelen2freq(T x) {
  return c / x;
}
template <class T>
[[gnu::const]] constexpr double freq2wavelen(T x) {
  return c / x;
}

/** Conversion constant 1 atmosphere to 1 Pascal and back.  Use conversion
 * formulae instead of pure constant if possible. **/
constexpr double ATM2PA = 101'325;
constexpr double PA2ATM = 1 / ATM2PA;
template <class T>
[[gnu::const]] constexpr double atm2pa(T x) {
  return x * ATM2PA;
}
template <class T>
[[gnu::const]] constexpr double pa2atm(T x) {
  return x * PA2ATM;
}

/** Conversion constant 1 bar to 1 Pascal and back.  Use conversion formulae
 * instead of pure constant if possible. **/
constexpr double BAR2PA = 100'000;
constexpr double PA2BAR = 1 / BAR2PA;
template <class T>
[[gnu::const]] constexpr double bar2pa(T x) {
  return x * BAR2PA;
}
template <class T>
[[gnu::const]] constexpr double pa2bar(T x) {
  return x * PA2BAR;
}

/** Conversion constant 1 torr to 1 Pascal and back.  Use conversion formulae
 * instead of pure constant if possible. **/
constexpr double TORR2PA = ATM2PA / 760;
constexpr double PA2TORR = 1 / TORR2PA;
template <class T>
[[gnu::const]] constexpr double torr2pa(T x) {
  return x * TORR2PA;
}
template <class T>
[[gnu::const]] constexpr double pa2torr(T x) {
  return x * PA2TORR;
}

/** Conversion constant Celsius to Kelvin and back.  Use conversion formulae
 * instead of pure constant if possible. **/
constexpr double CEL2KEL = 273.15;
template <class T>
[[gnu::const]] constexpr double c2k(T x) {
  return x + CEL2KEL;
}
template <class T>
[[gnu::const]] constexpr double k2c(T x) {
  return x - CEL2KEL;
}

/** Conversion constant Farenheit to Kelvin and back.  Use conversion formulae
 * instead of pure constant if possible. **/
constexpr double FAR2KELoff = 459.67;
constexpr double FAR2KELsca = 5.0 / 9.0;
template <class T>
[[gnu::const]] constexpr double f2k(T x) {
  return (x + FAR2KELoff) * FAR2KELsca;
}
template <class T>
[[gnu::const]] constexpr double k2f(T x) {
  return x / FAR2KELsca - FAR2KELoff;
}

/** Conversion constant Electron Vold to Kelvin and back.  Use conversion
 * formulae instead of pure constant if possible. **/
template <class T>
[[gnu::const]] constexpr double ev2k(T x) {
  return x * (e / k);
}
template <class T>
constexpr double k2ev(T x) {
  return x / (e / k);
}

/** Conversion constant Tesla to Gauss and back.  Use conversion formulae
 * instead of pure constant if possible. **/
template <class T>
[[gnu::const]] constexpr double t2g(T x) {
  return x * 10e4;
}
template <class T>
[[gnu::const]] constexpr double g2t(T x) {
  return x * 10e-4;
}

/** Conversion from cm-1 per molecule per cm^2 to Hz per molecule per m^2 **/
constexpr double HITRAN2ARTS_LS = KAYCM2FREQ * 1e-4;
constexpr double ARTS2HITRAN_LS = 1 / HITRAN2ARTS_LS;
template <class T>
[[gnu::const]] constexpr T cgs2si_linestrength(T x) {
  return x * HITRAN2ARTS_LS;
}
template <class T>
[[gnu::const]] constexpr T si2cgs_linestrength(T x) {
  return x * ARTS2HITRAN_LS;
}

/** Conversion from cm-1 per atmosphere to Hz per Pascal **/
constexpr double HITRAN2ARTS_GAMMA = KAYCM2FREQ / ATM2PA;
constexpr double ARTS2HITRAN_GAMMA = 1 / HITRAN2ARTS_GAMMA;
template <class T>
[[gnu::const]] constexpr T cgs2si_broadening(T x) {
  return x * HITRAN2ARTS_GAMMA;
}
template <class T>
[[gnu::const]] constexpr T si2cgs_broadening(T x) {
  return x * ARTS2HITRAN_GAMMA;
}

/** Conversion from cm-1 to Joule **/
constexpr double HITRAN2ARTS_ENERGY = h * KAYCM2FREQ;
constexpr double ARTS2HITRAN_ENERGY = 1 / HITRAN2ARTS_ENERGY;
template <class T>
[[gnu::const]] constexpr T cgs2si_energy(T x) {
  return x * HITRAN2ARTS_ENERGY;
}
template <class T>
[[gnu::const]] constexpr T si2cgs_energy(T x) {
  return x * ARTS2HITRAN_ENERGY;
}

template <class T>
[[gnu::const]] constexpr T angstrom2meter(T x) {
  return x * 1e-10;
}
template <class T>
[[gnu::const]] constexpr T meter2angstrom(T x) {
  return x * 1e10;
}
}  // namespace Conversion

#endif  // constants_h
