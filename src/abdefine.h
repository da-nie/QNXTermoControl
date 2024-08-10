/* Define header for application - AppBuilder 2.03  */

#if defined(__cplusplus)
extern "C" {
#endif

/* 'ThermoControl' Window link */
extern const int ABN_ThermoControl;
#define ABW_ThermoControl                    AbGetABW( ABN_ThermoControl )
extern const int ABN_PtTimer_Main;
#define ABW_PtTimer_Main                     AbGetABW( ABN_PtTimer_Main )
extern const int ABN_PtLabel_TValue;
#define ABW_PtLabel_TValue                   AbGetABW( ABN_PtLabel_TValue )
extern const int ABN_PtButton_Save;
#define ABW_PtButton_Save                    AbGetABW( ABN_PtButton_Save )
extern const int ABN_PtTrend_Temp;
#define ABW_PtTrend_Temp                     AbGetABW( ABN_PtTrend_Temp )
extern const int ABN_PtLabel_TMax;
#define ABW_PtLabel_TMax                     AbGetABW( ABN_PtLabel_TMax )
extern const int ABN_PtLabel_TMin;
#define ABW_PtLabel_TMin                     AbGetABW( ABN_PtLabel_TMin )
extern const int ABN_PtButton_ClearTrend;
#define ABW_PtButton_ClearTrend              AbGetABW( ABN_PtButton_ClearTrend )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

#define AB_OPTIONS "s:x:y:h:w:S:"

#if defined(__cplusplus)
}
#endif

