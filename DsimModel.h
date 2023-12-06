#pragma once
#include "StdAfx.h"
#include "sdk/vsm.hpp"
#include "m6502.h"


//#define InfoLog(__s__) sprintf_s(LogLineT, "%05d: ", LogLine++); myPopup->print(LogLineT); myPopup->print(__s__); myPopup->print("\n")
#define InfoLog(__s__) myPopup->print(__s__); myPopup->print("\n")
//#define DEBUGCALLS

#define SetHigh drivestate(time, SHI)
#define SetLow drivestate(time, SLO)
#define SetFloat drivestate(time, FLT)



class DsimModel : public IDSIMMODEL
{
public:
	INT isdigital (CHAR *pinname);
	VOID setup (IINSTANCE *inst, IDSIMCKT *dsim);
	VOID runctrl (RUNMODES mode);
	VOID actuate (REALTIME time, ACTIVESTATE newstate);
	BOOL indicate (REALTIME time, ACTIVEDATA *data);
	VOID clockstep(ABSTIME time, DSIMMODES mode);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback (ABSTIME time, EVENTID eventid);
	
private:
	VOID SetAddr(uint16_t val, ABSTIME time);
	VOID SetData(uint8_t val, ABSTIME time);
	uint8_t GetData(void);
	void HIZAddr(ABSTIME time);
	void HIZData(ABSTIME time);
	void UpdateData(ABSTIME time);
	void ReadInputControlPins();
	void SetOutputControlPins(ABSTIME time);
	void DebugLog();
	

	IINSTANCE *inst;
	IDSIMCKT *ckt;
	IDSIMPIN *pin_NMI, *pin_IRQ;
	IDSIMPIN *pin_RDY;
	IDSIMPIN *pin_RW;
	IDSIMPIN *pin_SYNC;
	IDSIMPIN *pin_RES;
	IDSIMPIN *pin_CLK;
	IDSIMPIN *pin_A[16];
	IDSIMPIN *pin_D[8];

	IDEBUGPOPUP *myPopup;

	// Global variables
	UINT8 cycle = 0;		// Current cycle of the state machine
	

	int LogLine = 1;
	char LogLineT[10];
	char LogMessage[256];
	char DebugMessage[256];

	m6502_t cpu;
	m6502_desc_t m6502_desc;
	uint64_t pins;
	


};
