///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 maintech GmbH, Otto-Hahn-Str. 15, 97204 Hoechberg, Germany //
// written by Christian Daniel                                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include "osmosdrthread.h"
#include "samplefifo.h"

OsmoSDRThread::OsmoSDRThread(osmosdr_dev_t* dev, SampleFifo* sampleFifo, QObject* parent) :
	QThread(parent),
	m_running(false),
	m_dev(dev),
	m_sampleFifo(sampleFifo)
{
}

OsmoSDRThread::~OsmoSDRThread()
{
	stop();
}

void OsmoSDRThread::start()
{
	m_startWaitMutex.lock();
	QThread::start();
	while(!m_running)
		m_startWaiter.wait(&m_startWaitMutex, 100);
	m_startWaitMutex.unlock();
}

void OsmoSDRThread::stop()
{
	m_running = false;
	wait();
}

void OsmoSDRThread::run()
{
	int res;

	m_running = true;
	m_startWaiter.wakeAll();

	while(m_running) {
		if((res = osmosdr_read_async(m_dev, &OsmoSDRThread::callbackHelper, this, 8, 65536)) < 0) {
			qCritical("OsmoSDRThread: async error: %s", strerror(errno));
			break;
		}
	}

	m_running = false;
}

void OsmoSDRThread::callback(quint8* buf, qint32 len)
{
	m_sampleFifo->write((qint16*)buf, len / sizeof(qint16));
	if(!m_running)
		osmosdr_cancel_async(m_dev);
}

void OsmoSDRThread::callbackHelper(unsigned char* buf, uint32_t len, void* ctx)
{
	OsmoSDRThread* thread = (OsmoSDRThread*)ctx;
	thread->callback(buf, len);
}