/* Link header for application - AppBuilder 2.03  */

#if defined(__cplusplus)
extern "C" {
#endif

extern ApContext_t AbContext;

ApWindowLink_t ThermoControl = {
	"ThermoControl.wgtw",
	&AbContext,
	AbLinks_ThermoControl, 0, 9
	};


#if defined(__cplusplus)
}
#endif

