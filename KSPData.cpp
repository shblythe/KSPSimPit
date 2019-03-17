#include "KSPData.h"
#include <assert.h>
#include <PayloadStructs.h>

KSPData* KSPData::sm_pInstance=0;

KSPData::KSPData(KerbalSimpit *pSimpit):m_vvi(0),m_apoapsis(0),m_periapsis(0),m_tApoapsis(0),m_tPeriapsis(0)
{
  assert(sm_pInstance==0);
  sm_pInstance=this;  // Lazy singleton, just assumes constructor will only be called once!
  m_pSimpit=pSimpit;
  m_msgCount=0;
  m_pSimpit->inboundHandler(static_callbackHandler);
  m_pSimpit->registerChannel(VELOCITY_MESSAGE);
  m_pSimpit->registerChannel(APSIDES_MESSAGE);
  m_pSimpit->registerChannel(APSIDESTIME_MESSAGE);
}

void KSPData::callbackHandler(byte msgType, byte msg[], byte msgSize)
{
  m_msgCount++;
  switch (msgType) {
    case VELOCITY_MESSAGE:
      if (msgSize==sizeof(velocityMessage))
      {
        velocityMessage vel;
        vel=parseVelocity(msg);
        m_vvi=vel.vertical;        
      }
      break;
    case APSIDES_MESSAGE:
      if (msgSize==sizeof(apsidesMessage))
      {
        apsidesMessage aps;
        aps=parseApsides(msg);
        m_periapsis=aps.periapsis;
        m_apoapsis=aps.apoapsis;
      }
      break;
    case APSIDESTIME_MESSAGE:
      if (msgSize==sizeof(apsidesTimeMessage))
      {
        apsidesTimeMessage apt;
        apt=parseApsidesTime(msg);
        m_tPeriapsis=apt.periapsis;
        m_tApoapsis=apt.apoapsis;
      }
      break;      
  }
}

void KSPData::static_callbackHandler(byte msgType, byte msg[], byte msgSize)
{
  sm_pInstance->callbackHandler(msgType,msg,msgSize);
}

void KSPData::update()
{
  m_pSimpit->update(); // Check for new serial messages
}

