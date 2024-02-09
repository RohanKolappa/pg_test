/*
 * resolutionchangedlistener.h
 *
 *  Created on: Dec 3, 2010
 *      Author: rkale
 */

#ifndef RESOLUTIONCHANGELISTENER_HH_
#define RESOLUTIONCHANGELISTENER_HH_


class IResolutionChangeListener
{
public:
    IResolutionChangeListener() {};
    virtual ~IResolutionChangeListener() {};

    virtual void OnResolutionChange() = 0;
};

#endif /* RESOLUTIONCHANGEDLISTENER_HH_ */
