/* Event header for application - AppBuilder 2.03  */

#if defined(__cplusplus)
extern "C" {
#endif

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &ThermoControl, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_ThermoControl[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 18023, OnOpening_Window_Main, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 18024, OnClosing_Window_Main, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 2009, OnActivate_Redraw, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 10003, OnActivate_Redraw, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 2010, OnActivate_Redraw, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ThermoControl", 18021, OnActivate_Redraw, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtTimer_Main", 41002, OnActivate_Timer, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButton_Save", 2009, OnActivate_Button_Save, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "PtButton_ClearTrend", 2009, OnActivate_Button_ClearTrend, 0, 0, 0, 0, },
	{ 0 }
	};

const char ApOptions[] = AB_OPTIONS;

#if defined(__cplusplus)
}
#endif

