/************************************************************************
 *                                                                      *
 *     (C) 2006 Max-Planck-Institute for Solar System Research          *
 *              Max-Planck-Str. 2                                       *
 *              37191 Katlenburg-Lindau                                 *
 *                                                                      *
 ************************************************************************/

/*
 * $Id: sensor.c,v 1.6 2008/01/09 12:53:03 chris Exp $
 */

/*
 * $Log: sensor.c,v $
 * Revision 1.6  2008/01/09 12:53:03  chris
 * Now CY7 voltages are devided by 5.0 before converted to temperature.
 *
 * Revision 1.5  2007/10/24 16:17:40  chris
 * Function PKR251_Identification() added.
 *
 * Revision 1.4  2007/10/21 22:12:41  chris
 * Calibration functions removed which are not needed for CAWSES.
 *
 * Revision 1.3  2007/10/16 18:24:18  chris
 * Calibration functions updated with the correct conversion factors.
 *
 * Revision 1.2  2007/01/30 16:51:07  chris
 * Conversion functions for the temperature sensor CY7 and pressure sensor TPR280 added.
 * In addition the conversion functions for the glass-carbon temperature sensors removed,
 * because they are not needed.
 *
 * Revision 1.1  2006/12/11 04:38:57  chris
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sensor.h"



void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
  void exit();

  fprintf(stderr, "Numerical Recipes run-time error...\n");
  fprintf(stderr, "%s\n", error_text);
  fprintf(stderr, "...now exiting to system...\n");
  exit(1);
}




double *dvector(int nl, int nh) /* allocates a double vector with range [nl..nh] */
{
  double *v;

  v = (double *) calloc((unsigned) (nh-nl+1), (unsigned) sizeof(double));
  if (!v) nrerror("allocation error in dvector()\n");
  return v-nl;
}




void free_dvector(double *v, int nl, int nh) /* frees a double vector allocated by dvector */
{
  free((char*) (v+nl));
}



    
void NumRecSpline(double x[], double y[], int n, double yp1, double ypn, double y2[])
/*
 * given arrays x[1..n] and y[1..n] containing a tabulated function, i.e., y[i]=f(x[i]),
 * with x1<x2<...<xn, and given values yp1 and ypn for the first derivative of the
 * interpolating function at points 1 and n, respectively, this routine returns an array
 * y2[1..n] that contains the second derivatives of the interpolating function at the
 * tabulated points x[i]. If yp1 and/or ypn are equal to 1e+30 or larger, the routine is
 * signaled to set the corresponding boundary condition for a natural spline, with zero
 * second derivative on that boundary.
 */
{
  double p, qn, sig, un, *u;
  int    i,k;

  u = dvector(1, n-1);
  if (yp1 > 0.99e+30)
    y2[1] = u[1] = 0.0;
  else
  {
    y2[1] = -0.5;
    u[1] = (3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
  }

  for (i=2; i<=n-1; i++)
  {
    sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p = sig*y2[i-1]+2.0;
    y2[i] = (sig-1.0)/p;
    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }

  if (ypn > 0.99e+30)
    qn = un = 0.0;
  else
  {
    qn = 0.5;
    un = (3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
  }

  y2[n] = (un-qn*u[n-1])/(qn*y2[n-1]+1.0);
  for (k=n-1; k>=1; k--)
    y2[k] = y2[k]*y2[k+1]+u[k];

  free_dvector(u, 1, n-1);
}




void NumRecSplint(double xa[], double ya[], double y2a[], int n, double x, double *y)
/*
 * given the arrays xa[1..n] and ya[1..n], which tabulate a function (with the xa[i] in
 * order) and given the array y2a[1..n], which is the output from spline() above, and
 * given a value of x, this routine returns a cubic spline interpolated value y(x).
 */
{
  double h, b, a;
  int    low, high, mid;

  low = 1;
  high = n;
  while (high-low > 1)
  {
    mid = (high+low) >> 1;
    if (xa[mid] > x) high = mid;
    else low = mid;
  }

  h = xa[high]-xa[low];
  if (h == 0.0)
    /*  the xa[1..n] have to be distinct  */  
    nrerror("bad xa input to routine SPLINT");
  a = (xa[high]-x)/h;
  b = (x-xa[low])/h;
  *y = a*ya[low]+b*ya[high]+((a*a*a-a)*y2a[low]+(b*b*b-b)*y2a[high])*(h*h)/6.0;
}




double CY7_Temperature(double voltage)
{
/*
 * calibration data for the OMEGA CY7 series temperature sensor: Standard curve #10
 * for CY7 silicon diodes;  measurement current = 10 micro-ampere +/- 0.05%.
 */

  double Temperature[120] = {  /* in Kelvin */
    475.0,   470.0,   460.0,   450.0,   440.0,   430.0,   420.0,   410.0,   400.0,   390.0,
    380.0,   370.0,   360.0,   350.0,   340.0,   330.0,   320.0,   310.0,   300.0,   290.0,
    280.0,   270.0,   260.0,   250.0,   240.0,   230.0,   220.0,   210.0,   200.0,   190.0,
    180.0,   170.0,   160.0,   150.0,   140.0,   130.0,   120.0,   110.0,   100.0,    95.0,
     90.0,    85.0,    80.0,    75.0,    70.0,    65.0,    60.0,    58.0,    56.0,    54.0,
     52.0,    50.0,    48.0,    46.0,    44.0,    42.0,    40.0,    38.0,    36.0,    34.0,
     32.0,    30.0,    29.0,    28.0,    27.0,    26.0,    25.0,    24.0,    23.0,    22.0,
     21.0,    20.0,    19.5,    19.0,    18.5,    18.0,    17.5,    17.0,    16.5,    16.0,
     15.5,    15.0,    14.5,    14.0,    13.5,    13.0,    12.5,    12.0,    11.5,    11.0,
     10.5,    10.0,     9.5,     9.0,     8.5,     8.0,     7.5,     7.0,     6.5,     6.0,
      5.5,     5.0,     4.8,     4.6,     4.4,     4.2,     4.0,     3.8,     3.6,     3.4,
      3.2,     3.0,     2.8,     2.6,     2.4,     2.2,     2.0,     1.8,     1.6,     1.4};

  double Voltage[120] = {  /* in Volt */
  0.09062, 0.10191, 0.12547, 0.14985, 0.17464, 0.19961, 0.22463, 0.24963, 0.27456, 0.29941,
  0.32416, 0.34881, 0.37337, 0.39783, 0.42221, 0.44647, 0.47069, 0.49484, 0.51892, 0.54294,
  0.56690, 0.59080, 0.61465, 0.63841, 0.66208, 0.68564, 0.70908, 0.73238, 0.75554, 0.77855,
  0.80138, 0.82404, 0.84650, 0.86873, 0.89072, 0.91243, 0.93383, 0.95487, 0.97550, 0.98564,
  0.99565, 1.00552, 1.01525, 1.02482, 1.03425, 1.04353, 1.05267, 1.05629, 1.05988, 1.06346,
  1.06700, 1.07053, 1.07402, 1.07748, 1.08093, 1.08436, 1.08781, 1.09131, 1.09490, 1.09864,
  1.10263, 1.10702, 1.10945, 1.11212, 1.11517, 1.11896, 1.12463, 1.13598, 1.15558, 1.17705,
  1.19645, 1.21440, 1.22314, 1.23184, 1.24053, 1.24928, 1.25810, 1.26702, 1.27607, 1.28527,
  1.29464, 1.30422, 1.31403, 1.32412, 1.33453, 1.34530, 1.35647, 1.36809, 1.38021, 1.39287,
  1.40615, 1.42013, 1.43488, 1.45048, 1.46700, 1.48443, 1.50272, 1.52166, 1.54097, 1.56027,
  1.57928, 1.59782, 1.60506, 1.61220, 1.61920, 1.62602, 1.63263, 1.63905, 1.64529, 1.65134,
  1.65721, 1.66292, 1.66845, 1.67376, 1.67880, 1.68352, 1.68786, 1.69177, 1.69521, 1.69812};

  static double *y2;
  static int    initFlag;
  double        result;

  if (!initFlag)
  {
    y2 = dvector(1,120);

    /* do a cubic spline interpolation of Temperature vs. Voltage */
    NumRecSpline(Voltage-1, Temperature-1, 120, 1.0e+30, 1.0e+30, y2);
    initFlag = 1;
  }

  /*
   * Note: the monitored voltage (raw data) is five times amplified with
   * respect to the real voltage provided by the OMEGA CY7 diode, This
   * is the reason why the voltage is devided by five here. 
   */
  NumRecSplint(Voltage-1, Temperature-1, y2, 120, voltage/5.0, &result);

  return result;
}




double PT100_Temperature(double resistance)
/*
 * converts ohm to Kelvin for a DIN IRC 751 Pt100 temperature sensor.
 * Note: The temperature is exact to +/- 0.01 deg. Celsius in the
 * range -200 to +130 deg. Celsius! The whole DIN IRC 751 table has
 * been used for the calculation of the five coefficients.
 */
{
  double temperature;
  double c[5];
  int i;

  if (resistance < 18.5)
    return -999.999;  /* underrange */

  if (resistance > 150.0)
    return 999.999;   /* overrange */

  c[0] =  1.811049e-03;
  c[1] =  2.559214e+00;
  c[2] =  9.788229e-04;
  c[3] = -1.693120e-06;
  c[4] =  2.682225e-08;

  resistance -= 100.0;
  temperature = c[i=4];
  while (i>0)
    temperature = temperature*resistance+c[--i];
  temperature += 273.15;

  return temperature;
}




double PKR251_Pressure(double voltage)
/*
 *  --------------------------------------------------
 * |   Voltage U   |            Pressure p            |
 * |     [V]       | [mbar]      [Torr]      [Pa]     |
 * | --------------|----------------------------------|
 * |     < 0.5     |           sensor error           |
 * | 0.5 ...  1.82 |            underrange            |
 * |     1.82      | 5.0e-09     3.8e-09    5.0e-07   |
 * |     2.00      | 1.0e-08     7.5e-09    1.0e-06   |
 * |     2.60      | 1.0e-07     7.5e-08    1.0e-05   |
 * |     3.20      | 1.0e-06     7.5e-07    1.0e-04   |
 * |     3.80      | 1.0e-05     7.5e-06    1.0e-03   |
 * |     4.40      | 1.0e-04     7.5e-05    1.0e-02   |
 * |     5.00      | 1.0e-03     7.5e-04    1.0e-01   |
 * |     5.60      | 1.0e-02     7.5e-03    1.0e-00   |
 * |     6.20      | 1.0e-01     7.5e-02    1.0e+01   |
 * |     6.80      | 1.0e+00     7.5e-01    1.0e+02   |
 * |     7.40      | 1.0e+01     7.5e+00    1.0e+03   |
 * |     8.00      | 1.0e+02     7.5e+01    1.0e+04   |
 * |     8.60      | 1.0e+03     7.5e+02    1.0e+05   |
 * | 8.6 ...  9.5  |            overrange             |
 * | 9.5 ... 10.5  |  sensor error (Pirani defect)    |
 *  --------------------------------------------------
 */
{
  double pressure = 0.0;

  if (voltage < 0.50)
    pressure = -3.0;                                     /* sensor error */
  else if (voltage >= 0.50 && voltage <= 1.82)
    pressure = -1.0;                                     /* underrange */
  else if (voltage > 1.82 && voltage < 8.60)
    pressure = 1.0e-08*pow(10.0, (voltage-2.0)/0.6);     /* measurement ok */
  else if (voltage >= 8.60 && voltage <= 9.50)
    pressure = -2.0;                                     /* overrange */
  else if (voltage > 9.50)
    pressure = -4.0;                                     /* Pirani sensor defect */

  return pressure;
}




double PKR251_Identification(double voltage)
{

 /*
  *  ---------------------------------------------------
  * |   Voltage U   |            Identification         |
  * |     [V]       |                                   |
  * | --------------|-----------------------------------|
  * |     < 2.4     | Combined Pirani/Cold Cathode Mode |
  * |     > 2.4     | Pure Pirani Operation Mode        |
  * |     > 14.0    | No Pressure Sensor Connected      |
  *  ---------------------------------------------------
  */

  double mode = -1.0;  /* no sensor connected */

  if (voltage <= 0.1)
    mode = -2.0;                                     /* short circuit in sensor */
  else if (voltage > 0.1 && voltage <= 2.4)
    mode =  1.0;                                     /* combined Pirani/Cold Cathode mode */
  else if (voltage > 2.4 && voltage <= 14.0)
    mode =  0.0;                                     /* pure Pirani mode */
  else if (voltage > 14.0)
    mode = -1.0;                                     /* no pressore sensor connected */

  return mode;
}




double calibrate(double x, int SensorID)
{
  double y;

  switch (SensorID)
  {
    case RAW:     y = x;  /* raw and calibrated data are identical */
                  break;
    case PT100:   y = PT100_Temperature(x);
                  break;
    case CY7:     y = CY7_Temperature(x);
                  break;
    case PKR251P: y = PKR251_Pressure(x);
                  break;
    case PKR251I: y = PKR251_Identification(x);
                  break;
    default:      y = -999.999;
                  break;
  }

  return y;
}
