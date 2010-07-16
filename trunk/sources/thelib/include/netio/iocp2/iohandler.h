/* 
*  Copyright (c) 2010,
*  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
*  
*  This file is part of crtmpserver.
*  crtmpserver is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  crtmpserver is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef NET_IOCP2
#ifndef _IOHANDLER_H
#define	_IOHANDLER_H

#include "utils/core.h"
#include "netio/iocp2/iohandlermanagertoken.h"

typedef enum _IOHandlerType {
    IOHT_UNKNOWN,
    IOHT_ACCEPTOR,
    IOHT_TCP_CONNECTOR,
    IOHT_TCP_CARRIER,
	IOHT_UDP_CARRIER,
    IOHT_TIMER,
	IOHT_STDIO
} IOHandlerType;

typedef struct _GenericOV : public OVERLAPPED {
    IOHandlerType type;
    IOHandlerManagerToken *pToken;
    uint32_t ownerId;
    uint32_t operationsCount;
    uint32_t timerPeriod;
} GenericOV;

#define INCREMENT_GENERICOV_USAGE(x) \
	/*ERR("Incrementing usage on GOV %p: %d -> %d",&(x),(x).operationsCount,(x).operationsCount+1);*/  \
	assert((x).operationsCount<100); \
	(x).operationsCount++;

#define DECREMENT_GENERICOV_USAGE(x) \
	/*ERR("Decrementing usage on GOV %p: %d -> %d",&(x),(x).operationsCount,(x).operationsCount-1);*/ \
	assert(((x).operationsCount<100) && ((x).operationsCount>0)); \
	(x).operationsCount--; \
	assert((x).operationsCount<100);

#define FreeGenericOV(pGenericOV) \
{ \
	if(pGenericOV!=NULL) { \
		if(pGenericOV->operationsCount==0) { \
			/*ERR("Is time to delete GOV %p",pGenericOV);*/ \
			delete pGenericOV; \
		} \
		/*else { \
			ERR("Can't delete GOV %p yet. Usage: %d",pGenericOV,pGenericOV->operationsCount); \
		}  */ \
	} \
}

class DLLEXP IOHandler {
protected:
    static uint32_t _idGenerator;
    uint32_t _id;
    int32_t _inboundFd;
	int32_t _outboundFd;
private:
    IOHandlerType _type;
    IOHandlerManagerToken *_pToken;
public:
    IOHandler(int32_t inboundFd, int32_t outboundFd, IOHandlerType type);
    virtual ~IOHandler();
    void SetIOHandlerManagerToken(IOHandlerManagerToken *pToken);
    IOHandlerManagerToken * GetIOHandlerManagerToken();
    uint32_t GetId();
    int32_t GetInboundFd();
	int32_t GetOutboundFd();
    IOHandlerType GetType();
	virtual bool SignalOutputData()=0;
    virtual bool OnEvent(GenericOV *pGenericOV) = 0;
    static string IOHTToString(IOHandlerType type);
    virtual operator string() = 0;
};


#endif	/* _IOHANDLER_H */
#endif /* NET_IOCP2 */

