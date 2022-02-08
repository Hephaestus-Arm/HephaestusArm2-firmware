/*
 * LightServer.h
 *
 *  Created on: May 24, 2020
 *      Author: acamilo
 */

#ifndef SRC_COMMANDS_LIGHTSERVER_H_
#define SRC_COMMANDS_LIGHTSERVER_H_

#include <PacketEvent.h>

class LightServer: public PacketEventAbstract {
public:
	LightServer();
	virtual ~LightServer();

	void event(float * buffer);
};

#endif /* SRC_COMMANDS_LIGHTSERVER_H_ */
