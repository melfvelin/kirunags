#include "SetConf.h"

/* SetConfTable - this is a "protected object" 
*	Takes a pointer to a conf table (uplink or downlink), the length of the table and the type of the table
*	and updates the global variable with the data stored at the address pointed to by the pointer
*/

void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType)
{
	switch(nTabType)
	{
		case 0:
		memcpy(&sDlTable, pnData, nMsglen);
		return;
		case 1:
		memcpy(&sUlTable, pnData, nMsglen);
		return;
	}
	return;
}