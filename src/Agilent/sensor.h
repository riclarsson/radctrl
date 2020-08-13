/************************************************************************
 *                                                                      *
 *     (C) 2006 Max-Planck-Institute for Solar System Research          *
 *              Max-Planck-Str. 2                                       *
 *              37191 Katlenburg-Lindau                                 *
 *                                                                      *
 ************************************************************************/

/*
 * $Id: sensor.h,v 1.5 2007/10/24 16:18:27 chris Exp $
 */

/*
 * $Log: sensor.h,v $
 * Revision 1.5  2007/10/24 16:18:27  chris
 * Sensor IDs PKR251P and PKR251I added.
 *
 * Revision 1.4  2007/10/21 22:14:24  chris
 * Sensor IDs removed which are not needed for CAWSES.
 *
 * Revision 1.3  2007/10/16 18:15:27  chris
 * Sensor type VMIXER added; sensor type CURRENT1 and CURRENT2
 * replaced by IMIXER resp. ICOIL.
 *
 * Revision 1.2  2007/01/30 16:50:34  chris
 * Sensor type CY7 and TPR280 added.
 *
 * Revision 1.1  2006/12/11 04:36:49  chris
 * Initial revision
 *
 */

#define OFF        0  /* no sensor connected                                     */
#define RAW        1  /* raw data translate directly to calibrated data          */
#define PT100      2  /* Pt100 temperature sensor                                */
#define CY7        3  /* OMEGA CY7 silicon diode temperature sensor              */
#define PKR251P    4  /* Pfeiffer Compact FullRange Gauge PKR251: Pressure       */
#define PKR251I    5  /* Pfeiffer Compact FullRange Gauge PKR251: Identification */

/*
 * Definition of global variables.
 */
extern char *progname;

double calibrate(double x, int SensorID);
