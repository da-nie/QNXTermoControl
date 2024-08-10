/* Import (extern) header for application - AppBuilder 2.03  */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern ApWindowLink_t ThermoControl;
extern ApWidget_t AbWidgets[ 8 ];


#if defined(__cplusplus)
}
#endif


#ifdef __cplusplus
int OnOpening_Window_Main( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnClosing_Window_Main( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnActivate_Redraw( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnActivate_Timer( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnActivate_Button_Save( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnActivate_Button_ClearTrend( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
