#include "StdAfx.h"
#include "DsimModel.h"


#define DEBUG
//static z80_t cpu;
//static uint64_t pins;
//
const STATE pLOHI[] = { SLO,FLT,FLT,FLT,SLO,SHI };
const STATE dLOHI[] = { SLO,SHI,FLT,FLT };


void DsimModel::HIZAddr(ABSTIME time) {						// Sets the address bus to HIZ
	int i;

	for (i = 0; i < 16; i++) {
		pin_A[i]->SetFloat;
	}
}

void DsimModel::HIZData(ABSTIME time) {						// Sets the data bus to HIZ
	int i;

	for (i = 0; i < 8; i++) {
		pin_D[i]->SetFloat;
	}
}

void DsimModel::SetAddr(UINT16 val, ABSTIME time) {			// Sets an address onto the address bus
	int i, j;

	for (i = 0; i < 16; i++) {
		j = (val >> i) & 0x01;
		if (j) {
			pin_A[i]->SetHigh;
		} else {
			pin_A[i]->SetLow;
		}
	}
}

void DsimModel::SetData(UINT8 val, ABSTIME time) {			// Sets a value onto the data bus
	int i, j;

	for (i = 0; i < 8; i++) {
		j = (val >> i) & 0x01;
		if (j) {
			pin_D[i]->SetHigh;
		} else {
			pin_D[i]->SetLow;
		}
	}
}

UINT8 DsimModel::GetData(void) {							// Reads a value from the data bus
	int i;
	UINT8 val = 0;

	for (i = 0; i < 8; i++) {
		if (ishigh(pin_D[i]->istate()))
			val |= (1 << i);
	}
	return(val);
}

void DsimModel::SetCPUState(ABSTIME time) {
	const uint16_t addr = M6502_GET_ADDR(pins);
	SetAddr(addr, time);
	if (pins & M6502_RW) {
		/* memory read */
		uint8_t val = GetData();
		M6502_SET_DATA(pins, val);
	}
	else {
		/* memory write */
		uint8_t val = M6502_GET_DATA(pins);
		SetData(val, time);
		HIZData(time + 20000);
	}

}

INT DsimModel::isdigital(CHAR *pinname) {
	return TRUE;											// Indicates all the pins are digital
}

VOID DsimModel::setup(IINSTANCE *instance, IDSIMCKT *dsimckt) {

	int n;
	char s[8];

	inst = instance;
	ckt = dsimckt;

	

	CREATEPOPUPSTRUCT *cps = new CREATEPOPUPSTRUCT;
	cps->caption = "M6502 Simulator Debugger Log";			// WIN Header
	cps->flags = PWF_VISIBLE | PWF_SIZEABLE;				// Show + Size
	cps->type = PWT_DEBUG;									// WIN DEBUG
	cps->height = 500;
	cps->width = 400;
	cps->id = 123;

	myPopup = (IDEBUGPOPUP *)instance->createpopup(cps);
	
	InfoLog("Connecting control pins...");

	pin_NMI = inst->getdsimpin("$NMI$", true);				// Connects M1 cycle pin
	pin_IRQ = inst->getdsimpin("$IRQ$", true);			// Connects memory request pin
	pin_RDY = inst->getdsimpin("RDY", true);			// Connects IO request pin
	pin_RW = inst->getdsimpin("R/W", true);				// Connects memory read pin
	pin_SYNC = inst->getdsimpin("SYNC", true);				// Connects memory write pin
	pin_RES = inst->getdsimpin("$RES$", true);			// Connects reset pin
	pin_CLK = inst->getdsimpin("CLK", true);				// Connects Clock pin
	
	InfoLog("Connecting data pins...");
	for (n = 0; n < 8; n++) {								// Connects Data pins
		s[0] = 'D';
		_itoa_s(n, &s[1], 7, 10);
		pin_D[n] = inst->getdsimpin(s, true);
	}

	InfoLog("Connecting address pins...");
	for (n = 0; n < 16; n++) {								// Connects Address pins
		s[0] = 'A';
		_itoa_s(n, &s[1], 7, 10);
		pin_A[n] = inst->getdsimpin(s, true);
	}
	LogLine = 1;
	pins = m6502_init(&cpu, &m6502_desc);
	
	
	// Connects function to handle Clock steps (instead of using "simulate")
	pin_CLK->sethandler(this, (PINHANDLERFN)&DsimModel::clockstep);

	
	HIZAddr(0);
	HIZData(0);
}

VOID DsimModel::runctrl(RUNMODES mode) {
}

VOID DsimModel::actuate(REALTIME time, ACTIVESTATE newstate) {
}

BOOL DsimModel::indicate(REALTIME time, ACTIVEDATA *data) {
	return FALSE;
}

VOID DsimModel::clockstep(ABSTIME time, DSIMMODES mode) {
	if (pin_CLK->isposedge()) {
		SetCPUState(time);
		pins = m6502_tick(&cpu, pins);
		SetCPUState(time);
#ifdef DEBUG
		sprintf_s(LogMessage, "     Address: 0x%X  data  : 0x%X", M6502_GET_ADDR(pins), M6502_GET_DATA(pins));
		InfoLog(LogMessage);
#endif // DEBUG
		if (0 == (pins & M6502_SYNC)) {
			sprintf_s(LogMessage, "cyle number is: %d", cycle);
			InfoLog(LogMessage);
			cycle = 1;
		}
		else {
			cycle++;
		}
	}
}

VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode) {
}

VOID DsimModel::callback(ABSTIME time, EVENTID eventid) {
}
