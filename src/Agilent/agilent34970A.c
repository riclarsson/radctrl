/************************************************************************
 *                                                                      *
 *     (C) 2006 Max-Planck-Institute for Solar System Research          *
 *              Max-Planck-Str. 2                                       *
 *              37191 Katlenburg-Lindau                                 *
 *                                                                      *
 ************************************************************************/

/*
 * $Id: agilent34970A.c,v 1.11 2016/06/15 14:39:35 waspam Exp $
 */

/*
 * $Log: agilent34970A.c,v $
 * Revision 1.11  2016/06/15 14:39:35  waspam
 * header file <ugpib.h> replaced by <ni488.h>.
 *
 * Revision 1.10  2008/01/10 14:45:33  chris
 * Channel index in the shared memory for the hot and cold load changed.
 *
 * Revision 1.9  2008/01/09 14:14:17  chris
 * Channel configuration debugged.
 *
 * Revision 1.8  2008/01/09 12:55:47  chris
 * Channel configuration changed because Thot, Tcold and HEMT temperature are measured
 * by CY7 silicon diodes instead of Pt100 resistors.
 *
 * Revision 1.7  2007/10/24 18:18:15  chris
 * Bugfixes in the GPIB command syntax.
 *
 * Revision 1.6  2007/10/24 16:21:48  chris
 * Channel index of the cold and hot load (0, resp. 1) for use of the shm corrected.
 *
 * Revision 1.5  2007/10/24 16:18:57  chris
 * Channel configuration changed according the needs of CAWSES.
 *
 * Revision 1.4  2007/10/17 18:59:52  chris
 * Channel configuration changed.
 *
 * Revision 1.3  2007/10/16 19:33:39  chris
 * Channel configuration changed: Pt100 is measured in 4-wire technique now.
 *
 * Revision 1.2  2007/01/30 16:53:24  chris
 * Measurement configuration updated (silicon diode CY7 and Pirani TPR280).
 *
 * Revision 1.1  2006/12/11 04:30:16  chris
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <ni488.h>
#include "../lib/synchronisation.h"
#include "../lib/path.h"
#include "sensor.h"

#define VERSION "version 5.4 for CAWSES,  Jun 15 2016"

#define RECORDS     8600            /* number of data records per file, one record = 10.????s  */
#define DELAY       6.0             /* number of additional seconds between two measurements   */
#define LOCKDIR    "/var/lock"      /* directory in which the lock file is created             */
#define LOCKFILE   "Agilent-34970A" /* name of the lock file                                   */
#define RETRIES     10              /* number of retries to get the lock file before giving up */
#define PAD         9               /* Primary GPIB Address of the Agilent 34970A DMM          */
#define CONTROLLER  0               /* Use GPIB-ENET controller #0 (default name is gpib0)     */
#define EOTMODE     1               /* Enable the END message on write operations              */
#define EOSMODE     0               /* Disable any End-of-String termination mode              */
#define BUFSIZE     1024L           /* size of the read buffer                                 */
#define MAXSTR      512             /* max. length of string received from stdin               */
#define VERBOSE     0x1             /* bit set to 1 in 'options' indicating to be verbose      */


/*
 * Definition of global variables.
 */
char *progname;
char lock[MAXSTR] = "\0";
int  options = 0;
int  logFlag;                       /* used in runMeasurement(), but modified in sighandler()  */


/*
 * Configuration of the twenty scanner channels 1 to 20.
 */
int sensor[20] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF,
                  OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};



void printHelp(void)
{
  fprintf(stdout, "\n");
  fprintf(stdout, "Usage: %s [option] [command]\n", progname);
  fprintf(stdout, "\n");
  fprintf(stdout, "Valid options are:\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "  -V         - print the program version and exit\n");
  fprintf(stdout, "  -h         - print this help screen and exit\n");
  fprintf(stdout, "  -v         - be verbose\n");
  fprintf(stdout, "  -A address - specify the GPIB address to use (default is %d)\n", PAD);
  fprintf(stdout, "  -d delay   - specify the additional delay between the scans (default is %.1f seconds)\n", DELAY);
  fprintf(stdout, "\n");
  fprintf(stdout, "Valid comands are:\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "  help       - print this help screen\n");
  fprintf(stdout, "  log        - measure and log the data (stop with Ctrl-C)\n");
  fprintf(stdout, "  open,i     - open the valve with index i (i = 1..20)\n");
  fprintf(stdout, "  close,i    - close the valve with index i (i = 1..20)\n");
  fprintf(stdout, "  hmm?       - read and clear an error message\n");
  fprintf(stdout, "  <text>     - send <text> literally as GPIB command to the instrument\n");
  fprintf(stdout, "  exit       - end the program\n");
  fprintf(stdout, "  quit       - end the program\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "File name convention:\n");
  fprintf(stdout, "The file name is setup from UTC time as yyyydddhhmm.dmm where\n");
  fprintf(stdout, " yyyy - the year, in the range 1900..2099\n");
  fprintf(stdout, "  ddd - the number of days since January 1, in the range 0 to 365\n");
  fprintf(stdout, "   hh - the number of hours past midnight, in the range 0 to 23\n");
  fprintf(stdout, "   mm - the number of minutes after the hour, in the range 0 to 59\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "NOTE 1: The program expects the 20 channel multiplexer module 34901A in slot 100,\n");
  fprintf(stdout, "        and the 20 channel general purpose switch module 34903A in slot 200!\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "NOTE 2: The program automatically sends the command 'MEMORY:STATE:RECALL:AUTO OFF' to\n");
  fprintf(stdout, "        keep the valves closed (i.e. the relais contacts open) after a power failure!\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "NOTE 3: To start a regular measurement type:\n");
  fprintf(stdout, "        nohup agilent34970A log > /data/agilent34970A.out 2>&1 &\n");
  fprintf(stdout, "\n");
}




FILE *fopenNewFile(void)
/*
 * Returns a file pointer for a new file or NULL on error.
 */
{
  FILE        *fp;
  char        str[MAXSTR];
  static char fname[MAXSTR];
  time_t      clock;
  struct tm   *tm;

  clock = time(NULL);
  tm = gmtime(&clock);  /* filenames use Coordinated Universal Time (UTC). */

  /*
   * The file name is setup as yyyydddhhmm.dmm where
   *  yyyy - the year, in the range 1900..2099
   *   ddd - the number of days since January 1, in the range 0 to 365
   *    hh - the number of hours past midnight, in the range 0 to 23
   *    mm - the number of minutes after the hour, in the range 0 to 59
   */
  sprintf(fname, "%s/%4d%03d%02d%02d.dmm", DATA_DIR, tm->tm_year+1900, tm->tm_yday, tm->tm_hour, tm->tm_min);

  if (!(fp=fopen(fname, "w")))
  {
    sprintf(str, "%s: cannot open file '%s'", progname, fname);
    perror(str);
    return NULL;
  }
  return fp;
}




void rmLock(void)
{
  if (strlen(lock) > 0)  /* a lock file should exist */
  {
    if (remove(lock) < 0)
    {
      char str[MAXSTR];

      sprintf(str, "%s: rmLock(): removing lock failed", progname);
      perror(str);
    }
    else
    {
      lock[0] = '\0';  /* the lock file doesn't exist any more */
      if (options & VERBOSE)
        printf("Lock successfully removed\n");
    }
  }
}




int openLock(void)
/*
 * Returns 1 in case the lock is already in use, 0 otherwise.
 */
{
  int fd;

  if ((fd=open(lock, O_CREAT | O_EXCL | O_WRONLY, S_IREAD | S_IWRITE)) >= 0)
  {
    char pid[MAXSTR];
    int n;

    sprintf(pid, "%d\n", getpid());
    n = strlen (pid);
    if (write(fd, pid, n) != n)
    {
      char str[MAXSTR];

      sprintf(str, "%s: openLock(): writing lock failed", progname);
      perror(str);
      exit(1);      /* serious error: give up! */
    }
    close(fd);
    return 0;  /* success - got the lock */
  }
  return 1;  /* failure - lock already in use */
}




void getLock(char *dev)
{
  struct timeval timeout;
  char           *s;
  int            i;

  /* strip path from absolute device name */
  for (s=dev,i=0; dev[i]!='\0'; i++)
    if (dev[i] == '/')
      s = dev+i+1;

  sprintf(lock, "%s/LCK.%s", LOCKDIR, s);

  for (i=1; i<=RETRIES; i++)
  {
    if (openLock() == 0)  /* got the lock */
    {
      if (options & VERBOSE)
        printf("got the lock\n");
      return;
    }

    /* lock in use: wait a bit */
    if (options & VERBOSE)
      printf("lock in use! Retrying ...\n");
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000 * (200 * i);
    if (select(0, NULL, NULL, NULL, &timeout) < 0)  /* some error occurred */
    {
      fprintf(stderr, "%s: getLock(): select() failed", progname);
      exit(1);
    }
  }

  fprintf(stderr, "%s: stale lock file '%s' ?\n", progname, lock);
  exit(1);
}




void gpibErr(char *msg)  /* This is a simple error handling function. */
{
  fprintf(stderr, "%s\n", msg);

  fprintf(stderr, "ibsta = 0x%x  < ", ibsta);
  if (ibsta & ERR)  fprintf(stderr, "ERR ");
  if (ibsta & TIMO) fprintf(stderr, "TIMO ");
  if (ibsta & END)  fprintf(stderr, "END ");
  if (ibsta & SRQI) fprintf(stderr, "SRQI ");
  if (ibsta & RQS)  fprintf(stderr, "RQS ");
  if (ibsta & CMPL) fprintf(stderr, "CMPL ");
  if (ibsta & LOK)  fprintf(stderr, "LOK ");
  if (ibsta & REM)  fprintf(stderr, "REM ");
  if (ibsta & CIC)  fprintf(stderr, "CIC ");
  if (ibsta & ATN)  fprintf(stderr, "ATN ");
  if (ibsta & TACS) fprintf(stderr, "TACS ");
  if (ibsta & DTAS) fprintf(stderr, "DTAS ");
  if (ibsta & DCAS) fprintf(stderr, "DCAS ");
  fprintf(stderr, ">\n");

  fprintf(stderr, "iberr = %d", iberr);
  if (iberr == EDVR) fprintf(stderr, " EDVR <Error>\n");
  if (iberr == ECIC) fprintf(stderr, " ECIC <Not Controller-In-Charge>\n");
  if (iberr == ENOL) fprintf(stderr, " ENOL <No Listener>\n");
  if (iberr == EADR) fprintf(stderr, " EADR <Adress error>\n");
  if (iberr == EARG) fprintf(stderr, " EARG <Invalid Argument>\n");
  if (iberr == ESAC) fprintf(stderr, " ESAC <Not System Controller>\n");
  if (iberr == EABO) fprintf(stderr, " EABO <Operation aborted>\n");
  if (iberr == ENEB) fprintf(stderr, " ENEB <No GPIB board>\n");
  if (iberr == EOIP) fprintf(stderr, " EOIP <Async I/O in progress>\n");
  if (iberr == ECAP) fprintf(stderr, " ECAP <No capability>\n");
  if (iberr == EFSO) fprintf(stderr, " EFSO <File system error>\n");
  if (iberr == EBUS) fprintf(stderr, " EBUS <Command error>\n");
  if (iberr == ESTB) fprintf(stderr, " ESTB <Status byte lost>\n");
  if (iberr == ESRQ) fprintf(stderr, " ESRQ <SRQ stuck on>\n");
  if (iberr == ETAB) fprintf(stderr, " ETAB <Table Overflow>\n");

  fprintf(stderr, "ibcnt = %d\n", ibcnt);
}




int isQuestion(char *cmd)
{
  int i,n;

  for (n=strlen(cmd),i=0; i<n; i++)
    if (cmd[i] == '?')
      return 1;  /* cmd is a question */

  return 0;  /* cmd is not a question */
}




int gpibTalk(int ud, char *cmd)
{
  char s[MAXSTR], errmsg[MAXSTR];
  int  n;

  /*
   * Send the given command to the Agilent 34970A.
   */
  n = strlen(cmd);
  if (options & VERBOSE)
  {
    strcpy(s, cmd);
    if (s[n-1] == '\n')
    {
      s[n-1] = '\\';
      s[n] =    'n';
      s[n+1] = '\0';
    }
    printf("Sending '%s' to the Agilent 34970A\n", s);
  }
  ibwrt(ud, cmd, (long)n);
  if (ibsta & ERR)
  {
    strcpy(s, cmd);
    if (s[n-1] == '\n')
    {
      s[n-1] = '\\';
      s[n] =    'n';
      s[n+1] = '\0';
    }
    sprintf(errmsg, "ERROR: Sending '%s' to the Agilent 34970A failed", s);
    gpibErr(errmsg);
    return -1;  /* hardware error */
  }

  return 0;  /* success */
}






int gpibListen(int ud, char *buf, int bufsize)
{
  /*
   * Read some response from the Agilent 34970A.
   */
  if (options & VERBOSE)
    printf("Waiting for response ...\n");
  ibrd(ud, buf, bufsize-1);
  if (ibsta & ERR)
  {
    gpibErr("ERROR: Reading response from the Agilent 34970A failed");
    return -1;  /* error */
  }
  buf[ibcnt] = '\0';

  if (buf[ibcnt-1] == '\n' && buf[ibcnt-2] == '\r')
  {
    buf[ibcnt-1] = '\0';
    buf[ibcnt-2] = '\n';
  }

  if (options & VERBOSE)
    printf("Response: %s", buf);

  return 0;  /* success */
}





void setRelais(int address, char *str)
{
  int ud;
  int index = 0, action = -1;    /* action -1? fix it */
  char cmd[MAXSTR];

  /*
   * - get Lock
   * - open GPIB connection
   * - send 'cmd'
   * - close GPIB connection
   * - release Lock
   */

  if (sscanf(str, "open,%d", &index) == 1)
    action = 1;
  if (sscanf(str, "close,%d", &index) == 1)
    action = 0;

  if (index < 1 || index > 20)
  {
    fprintf(stderr, "illegal or missing relais index\n");
    return;
  }

  if (action == 1) /* CLOSE relais contact resp. OPEN valve */
    sprintf(cmd, "ROUTE:CLOSE (@%d)\n", 200+index);
  if (action == 0) /* OPEN relais contact resp. CLOSE valve */
    sprintf(cmd, "ROUTE:OPEN (@%d)\n", 200+index);

  getLock(LOCKFILE);  /* get the lock file */

 /*
  * Bring the Agilent 34970A Data Acquisition/Switch Unit online using ibdev.
  */
  if (options & VERBOSE)
    printf("Opening GPIB connection to the 34970A (GPIB address %d)\n", address);
  ud = ibdev(CONTROLLER, address, NO_SAD, T10s, EOTMODE, EOSMODE);
  if (ibsta & ERR)
  {
    gpibErr("setRelais(): ibdev() Error");
    return;
  }

  if (gpibTalk(ud, cmd) < 0)
    return;

  if (options & VERBOSE)
    printf("Closing GPIB connection to the 34970A\n");
  ibonl(ud, 0);

  rmLock();
}




void printPrompt(void)
{
  fprintf(stdout, "Agilent 34970A > ");
  fflush(stdout);
}




void sighandler(int signal)
{
  if (options & VERBOSE)
  {
    fprintf(stdout, "%s: received signal %d ", progname, signal);
    if (signal == SIGHUP) fprintf(stdout, "<SIGHUP>\n");
    if (signal == SIGINT) fprintf(stdout, "<SIGINT>\n");
    if (signal == SIGTERM) fprintf(stdout, "<SIGTERM>\n");
  }
  logFlag = 0;  /* this results in a smooth termination of runMeasurement() */
}




void sleepSomeTime(double timeout)
{
  struct timeval tv;

  if (timeout > 0.0)
  {
    tv.tv_sec  = (unsigned int)timeout;
    tv.tv_usec = (unsigned int)(1000000.0*(timeout-tv.tv_sec));
    select(0, NULL, NULL, NULL, &tv);
  }
}




int cleanup()
{
  SHM  *shm;
  char answer[MAXSTR];
  int  result=0;  /* result 0 means success, result 1 means error */

  if (options & VERBOSE)
    system("ipcs -a");

  fprintf(stderr, "\nAre you sure that the semaphore and shared memory are not in use anymore? (y/n) ");
  fgets(answer, MAXSTR, stdin);
  fprintf(stderr, "\n");
  if (answer[0] != 'y' && answer[0] != 'Y')
    return result;

  if ((shm=openShm(sizeof(SHMDATA))) == NULL)  /* get the IDs of the semaphore and shared memory segment */
    result = 1;
  else if (closeShm(shm) != 0)  /* close the shared memory to detach it from the process' memory space */
    result = 1;
  else if (rmShm(shm) != 0)     /* finally remove the semaphore and shared memory */
    result = 1;

  if (options & VERBOSE)
    system("ipcs -a");

  if (result != 0)
    fprintf(stderr, "\n ERROR: Removing the semaphore and shared memory failed!\n\n");

  return result;
}




void sendCommand(int address, char *cmd)
{
  int  ud;
  char buf[BUFSIZE];

  getLock(LOCKFILE);

  if (options & VERBOSE)
    printf("Opening GPIB connection to the Agilent 34970A (GPIB address %d)\n", address);

  ud = ibdev(CONTROLLER, address, NO_SAD, T30s, EOTMODE, EOSMODE);
  if (ibsta & ERR)
  {
    gpibErr("sendCommand(): ibdev() Error");
    return;
  }

  if (gpibTalk(ud, cmd) < 0)
    return;

  if (isQuestion(cmd))
  {
    if (gpibListen(ud, buf, BUFSIZE) < 0)
      printf("--> no response!\n");
    else
      printf("--> %s", buf);
  }

  if (options & VERBOSE)
    printf("Closing GPIB connection to the 34970A\n");
  ibonl(ud, 0);

  rmLock();
}




void getStatus(int address)
{
  int  ud, i;
  char buf[BUFSIZE];

  getLock(LOCKFILE);  /* get the lock file */

 /*
  * Bring the Agilent 34970A Data Acquisition/Switch Unit online using ibdev.
  */
  if (options & VERBOSE)
    printf("Opening GPIB connection to the Agilent 34970A (GPIB address %d)\n", address);

  ud = ibdev(CONTROLLER, address, NO_SAD, T10s, EOTMODE, EOSMODE);
  if (ibsta & ERR)
  {
    gpibErr("getStatus(): ibdev() Error");
    exit(1);  /* give up! */
  }

  /*
   * Check if the relais 201 to 220 are closed
   */
  if (gpibTalk(ud, "route:close? (@201:220)") < 0)
    return;
  if (gpibListen(ud, buf, BUFSIZE) < 0)
    printf(" no response!\n");
  else
  {
    /*
     * Replace ',' by ' '
     */
    for (i=0; i<ibcnt; i++)
      if (buf[i] == ',')
        buf[i] = ' ';

    for (i=0; i<20; i++)
    {
      if (buf[2*i] == '1')
        fprintf(stdout, " valve %02d is opened    1  (relais 2%02d is closed)\n", i+1, i+1);
      else if (buf[2*i] == '0')
        fprintf(stdout, " valve %02d is closed  0    (relais 2%02d is opened)\n", i+1, i+1);
      else
        fprintf(stdout, " relais 2%02d: illegal state '%c'\n", i+1, buf[2*i]);
    }
  }

  if (options & VERBOSE)
    printf("Closing GPIB connection to the 34970A\n");
  ibonl(ud, 0);

  rmLock();  /* remove the lock */
}




int configureInstrument(int ud, SHMDATA data)
{
  /*
   * Reset the instrument to the following default settings:
   *  - scan interval   = 0 seconds
   *  - number of scans = 1
   *  - channel delay   = AUTO (delay between closing the relay contacts and
   *    starting the measurement)
   */

  if (gpibTalk(ud, "SYSTEM:CPON 100\n") < 0) return -1;
  if (gpibTalk(ud, "ROUTE:CHAN:DELAY:AUTO ON, (@101:120)\n") < 0) return -1;
  if (gpibTalk(ud, "CALC:SCALE:STATE OFF, (@101:120)\n") < 0) return -1;

  /*
   * Setup the measurement functions and parameters for each channel;
   * see the User's Guide for the remote control commands and syntax:
   */
  sensor[ 0] = CY7;       /* Channel 101:  DC voltage measurement               T1   Cold Load Temperature      */
  sensor[ 1] = CY7;       /* Channel 102:  DC voltage measurement               T1   Hot  Load Temperature      */
  sensor[ 2] = CY7;       /* Channel 103:  DC voltage measurement               T1   HEMT Temperature           */
  sensor[ 3] = PT100;     /* Channel 104:  2W resistance measurement            T2   CTS1 Filter Temp. Branch A */
  sensor[ 4] = PT100;     /* Channel 105:  2W resistance measurement            T3   CTS2 Filter Temp. Branch A */
  sensor[ 5] = PT100;     /* Channel 106:  2W resistance measurement            T6   CTS3 Filter Temp. Branch A */
  sensor[ 6] = PT100;     /* Channel 107:  2W resistance measurement            T7   CTS4 Filter Temp. Branch A */
  sensor[ 7] = PKR251P;   /* Channel 108   DC voltage measurement               P1   Pressure 1 Dewar           */
  sensor[ 8] = PKR251P;   /* Channel 109:  DC voltage measurement               P2   Pressure 2 Pump            */
  sensor[ 9] = OFF;       /* Channel 110:  no measurement                                                       */
  sensor[10] = OFF;       /* Channel 111:  DC voltage measurement               T5   Spare Voltage 1            */
  sensor[11] = OFF;       /* Channel 112:  DC voltage measurement               T5   Spare Voltage 2            */
  sensor[12] = OFF;       /* Channel 113:  DC voltage measurement               T5   Spare Voltage 3            */
  sensor[13] = PT100;     /* Channel 114:  2W resistance measurement            T2   CTS1 Filter Temp. Branch B */
  sensor[14] = PT100;     /* Channel 115:  2W resistance measurement            T3   CTS2 Filter Temp. Branch B */
  sensor[15] = PT100;     /* Channel 116:  2W resistance measurement            T6   CTS3 Filter Temp. Branch B */
  sensor[16] = PT100;     /* Channel 117:  2W resistance measurement            T7   CTS4 Filter Temp. Branch B */
  sensor[17] = PKR251I;   /* Channel 118:  DC voltage measurement               P1   Pressure 1 Identification  */
  sensor[18] = PKR251I;   /* Channel 119:  DC voltage measurement               P2   Pressure 2 Identification  */
  sensor[19] = OFF;       /* Channel 120:  no measurement                                                       */

  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@101)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@102)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@103)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@104)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@105)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@106)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@107)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@108)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@109)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@111)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@112)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@113)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@114)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@115)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@116)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"RESISTANCE\", (@117)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@118)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:FUNCTION \"VOLTAGE:DC\", (@119)\n") < 0) return -1;

  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@101)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@102)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@103)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@104)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@105)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@106)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@107)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@108)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@109)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@111)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@112)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@113)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@114)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@115)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@116)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:RESISTANCE:NPLC 10, (@117)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@118)\n") < 0) return -1;
  if (gpibTalk(ud, "SENSE:VOLTAGE:DC:NPLC 10, (@119)\n") < 0) return -1;

  if (gpibTalk(ud, "ROUTE:SCAN (@101,102,103,104,105,106,107,108,109,114,115,116,117,118,119)\n") < 0) return -1;

  return 0;  /* success */
}




void runMeasurement(int address, double delay)
{
  char     buf[BUFSIZE], *s;  /* input buffer for response                   */
  time_t   clock;             /* time of the begin of the measurement        */
  double   hkp[2][20];        /* raw and calibrated housekeeping data        */
  int      rs[20];            /* general purpose relais status               */
  FILE     *fp;               /* file pointer of the data file               */
  SHM      *shm;              /* shared memory pointer, similar to FILE *fp  */
  SHMDATA  data;              /* shared memory data structure                */
  int      count = RECORDS;   /* max. number of data records per single file */
  int      i,j,n;
  int      ud;

  logFlag = 1;

  /* open new file */
  if ((fp=fopenNewFile()) == NULL)
    exit(1);

  if ((shm=openShm(sizeof(SHMDATA))) == NULL)
    exit(1);

  if (options & VERBOSE)
    system("ipcs -a");

  /*
   * Bring the Agilent 34970A Data Acquisition/Switch Unit online using ibdev.
   */
  ud = ibdev(CONTROLLER, address, NO_SAD, T10s, EOTMODE, EOSMODE);
  if (ibsta & ERR)
  {
    gpibErr("runMeasurement(): ibdev Error");
    exit(1);
  }

  /*
   * Reset the Agilent 34970A. The ibclr function sends the GPIB Selected
   * Device Clear (SDC) message to the device at the the given address.
   */
  ibclr(ud);
  if (ibsta & ERR)
  {
    gpibErr("ibclr() Error");
    ibonl(ud, 0);
    exit(1);
  }

  /*
   * Send *IDN? to make sure we are talking to the correct device.
   */
  gpibTalk(ud, "*IDN?\n");
  gpibListen(ud, buf, BUFSIZE);
  if (strstr(buf, "34970A") == NULL)
  {
    int n = strlen(buf)-1;
    if (buf[n] == '\n')
      buf[n] = '\0';
    fprintf(stderr, "\n Improper device identification! The device identified\n");
    fprintf(stderr, " itself as \"%s\",\n", buf);
    fprintf(stderr, " but expected is \"HEWLETT-PACKARD,34970A\".\n\n");
    ibonl(ud, 0);
    exit(1);       /* give up, if we are talking to the wrong instrument */
  }


  /*
   * Configure the instrument
   */
  if (configureInstrument(ud, data) != 0)
  {
    fprintf(stderr, "%s: configuration of the 34970A failed\n", progname);
    ibonl(ud, 0);
    exit(1);       /* give up, if we are not successfull at this point */
  }

  while (logFlag && count--)   /* infinite (!) measurement loop */
  {
    /*
     * Get the current time.
     */
    clock = time(NULL);

    /*
     * Reset the array with the raw and calibrated housekeeping data.
     */
    for (i=0; i<2; i++)
      for (j=0; j<20; j++)
        hkp[i][j] = 0.0;

    /*
     * Trigger the scan ...
     */
    getLock(LOCKFILE);
    gpibTalk(ud, "READ?\n");

    /*
     * .. and read the measurements.
     */
    gpibListen(ud, buf, BUFSIZE);
    rmLock();

    /*
     * Replace ',' by ' '
     */
    for (i=0; i<ibcnt; i++)
      if (buf[i] == ',')
        buf[i] = ' ';

    /*
     * Log the current time to file 'fp'.
     */
    fprintf(fp, "%10ld", (long)clock);
    if (options & VERBOSE)
      printf("%10ld", (long)clock);

    /*
     * Log the housekeeping data to file 'fp'.
     */
    for (s=buf,n=0,i=0; i<20; i++)  /* loop over channel 1 to 20 */
    {
      if (sensor[i] != OFF)
      {
        if (sscanf(s+=n, "%lf%n", &(hkp[0][i]), &n) != 1)             /* hkp[0][i] are the raw data */
        {
          fprintf(stderr, "%ld: strange response: '%s'\n", clock, buf);
          fprintf(fp, "\terror: %s\n", buf);
        }
        else
        {
          hkp[1][i] = calibrate(hkp[0][i], sensor[i]);              /* hkp[1][i] are the calibrated data */
          fprintf(fp, "\t%12.5e\t%12.5e", hkp[1][i], hkp[0][i]);
          if (options & VERBOSE)
            printf(" %10.3e", hkp[1][i]);
        }
      }
      else  /* sensor OFF */
      {
        fprintf(fp, "\t%12.5e\t%12.5e", 0.0, 0.0);
      }
    }
    fprintf(fp, "\n");
    fflush(fp);
    if (options & VERBOSE)
      fprintf(stdout, "\n");

    /*
     * Check if the relais 201 to 220 are closed
     */
    for (i=0; i<20; i++)
      rs[i] = -1;

    gpibTalk(ud, "route:close? (@201:220)");
    gpibListen(ud, buf, BUFSIZE);

    for (i=0; i<20; i++)
    {
      if (buf[2*i] == '1')
        rs[i] = 1;
      if (buf[2*i] == '0')
        rs[i] = 0;
    }

    /*
     * Log the relais status to file 'fp'.
     */
    for (i=0; i<20; i++)
      fprintf(fp, "\t%2d", rs[i]);

    fprintf(fp, "\n");
    fflush(fp);
    if (options & VERBOSE)
      fprintf(stdout, "\n");

    /*
     * Update the shared memory data
     */
    data.clock = clock;                  /* time of the begin of the measurement       */
    for (i=0; i<20; i++)
    {
      data.ch[0][i] = hkp[0][i];         /* raw data from channel 1 to 20              */
      data.ch[1][i] = hkp[1][i];         /* calibrated data from channel 1 to 20       */
      data.rs[i]    = rs[i];             /* general purpose relais status              */
    }
    data.iCold = 1;                      /* channel index of the cold load temperature */
    data.iHot  = 2;                      /* channel index of the hot load temperature  */
    writeShm(shm, &data, sizeof(data));  /* finally update the shared memory data      */

    /*
     * Check if the data file is completed; if yes, start a new file
     */
    if (count == 0)
    {
      fclose(fp);                        /* close the current data file                */
      count = RECORDS;                   /* restore record counter                     */
      if ((fp=fopenNewFile()) == NULL)   /* open a new file                            */
        exit(1);                         /* give up, if no new data file can be opened */
    }

   /*
    * wait a little bit until the next measurement needs to be done.
    */
   sleepSomeTime(delay);

  }  /* end of the infinite (!) measurement loop */

  ibonl(ud, 0);
  closeShm(shm);
  fclose(fp);
}




int main(int argc, char *argv[])
{
  int    address=PAD;
  int    cmdFlag=0, runFlag=1;
  char   cmd[MAXSTR];
  double delay = DELAY;

  progname = argv[0];

  /*
   * Install a signal handler to smoothly exit the program  with <Ctrl><C> or kill.
   */
  signal(SIGHUP,  sighandler);
  signal(SIGINT,  sighandler);
  signal(SIGTERM, sighandler);

  while (--argc > 0)
  {
    int c, skip=0;

    if ((*++argv)[0] == '-')  /* argument is an option */
    {
      while ((c = *++argv[0]))
        switch (c)
        {
          case 'V': fprintf(stderr, "%s: %s\n", progname, VERSION);
                    exit(0);
          case 'h': printHelp();
                    exit(0);
          case 'A': skip++;
                    if (argc-skip > 0)
                    {
                      sscanf(*(argv+skip), "%d", &address);
                    }
                    else
                    {
                      fprintf(stderr, "%s: missing GPIB address\n", progname);
                      exit(1);
                    }
                    break;
          case 'd': skip++;
                    if (argc-skip > 0)
                    {
                      if (sscanf(*(argv+skip), "%lf", &delay) != 1)
                      {
                        fprintf(stderr, "%s: option -d: illegal delay value '%s'\n", progname, *(argv+skip));
                        exit(1);
                      }
                      if (delay < 0.0)
                      {
                        fprintf(stderr, "%s: option -d: illegal delay value '%s'\n", progname, *(argv+skip));
                        exit(1);
                      }
                      if (delay > 300.0)
                      {
                        fprintf(stderr, "%s: delay too long (max. 300 seconds)\n", progname);
                        exit(1);
                      }
                    }
                    else
                    {
                      fprintf(stderr, "%s: option -d: missing delay value\n", progname);
                      exit(1);
                    }
                    break;
          case 'v': options |= VERBOSE;
                    break;
           default: fprintf(stderr, "%s: illegal option: %c\n", progname, c);
                    exit(1);
        }
    }
    else  /* argument is not an option, so it is used as a command */
    {
      if (!cmdFlag)
      {
        strcpy(cmd, *argv);  /* this should be made safe for buffer overflow */
        strcat(cmd, "\n");
        cmdFlag = 1;
      }
      else
      {
        fprintf(stderr, "%s: too many commands\n", progname);
        exit(1);
      }

    }
    argc -= skip;
    argv += skip;
  }

  if (cmdFlag)  /* process the given command and exit */
  {
    if (strstr(cmd, "help"))
    {
      printHelp();
    }
    else if (strncmp(cmd, "open", 4) == 0 || strncmp(cmd, "close", 5) == 0)
    {
      setRelais(address, cmd);
    }
    else if (strstr(cmd, "status"))
    {
      getStatus(address);
    }
    else if (strstr(cmd, "cleanup"))
    {
      cleanup();
    }
    else if (strstr(cmd, "hmm?"))
    {
      sendCommand(address, "system:err?\n");
    }
    else if (strstr(cmd, "log"))
    {
      runMeasurement(address, delay);
    }
    else
    {
      sendCommand(address, cmd);
    }
  }
  else
  {
    do  /* run interactively */
    {
      printPrompt();

      if (fgets(cmd, MAXSTR, stdin) == NULL)
      {
        printf("exit\n");
        runFlag = 0;  /* leave loop on <CTRL><D> */
      }
      else if (strcmp(cmd, "\n") == 0)
      {
        /* ignore a single newline character */
      }
      else if (strstr(cmd, "exit") || strstr(cmd, "quit"))
      {
        runFlag = 0;  /* leave loop on 'exit' or 'quit' */
      }
      else if (strstr(cmd, "help"))
      {
        printHelp();
      }
      else if (strncmp(cmd, "open", 4) == 0 || strncmp(cmd, "close", 5) == 0)
      {
        setRelais(address, cmd);
      }
      else if (strstr(cmd, "status"))
      {
        getStatus(address);
      }
      else if (strstr(cmd, "cleanup"))
      {
        cleanup();
      }
      else if (strstr(cmd, "hmm?"))
      {
        sendCommand(address, "system:err?\n");
      }
      else if (strstr(cmd, "log"))
      {
        /*
         * Run the measurement
         */
        runMeasurement(address, delay);
      }
      else
      {
        /*
         * Talk interactively to the instrument
         */
        sendCommand(address, cmd);
      }
    } while (runFlag);
  }

  return 0;
}

