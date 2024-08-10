/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.03  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "cmain.h"

extern CMain cMain;

int OnOpening_Window_Main( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
 //настроим системный тик
 struct _clockperiod new_clock;
 new_clock.nsec=100000;//0.1 мс
 new_clock.fract=0;
 ClockPeriod_r(CLOCK_REALTIME,&new_clock,NULL,0);
	
 cMain.OnOpening();
 return(Pt_CONTINUE);
}

