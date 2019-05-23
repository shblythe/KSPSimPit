#include "KSPData.h"
#include <assert.h>
#include <PayloadStructs.h>
#include "IO.h"

KSPData* KSPData::sm_pInstance=0;

KSPData::KSPData(KerbalSimpit *pSimpit):
	    m_vvi(0),m_apoapsis(0),m_periapsis(0),m_tApoapsis(0),m_tPeriapsis(0),m_actionStatus(0),m_autopilotMode(0)
{
  assert(sm_pInstance==0);
  sm_pInstance=this;  // Lazy singleton, just assumes constructor will only be called once!
  m_pSimpit=pSimpit;
  m_msgCount=0;
  m_pSimpit->inboundHandler(static_callbackHandler);
  m_pSimpit->registerChannel(VELOCITY_MESSAGE);
  m_pSimpit->registerChannel(APSIDES_MESSAGE);
  m_pSimpit->registerChannel(APSIDESTIME_MESSAGE);
  m_pSimpit->registerChannel(ACTIONSTATUS_MESSAGE);
  m_pSimpit->registerChannel(LF_MESSAGE);
  m_fuelGauge=new LedGauge(LED_FUEL_RED,LED_FUEL_YEL,LED_FUEL_GRN);
  m_pSimpit->registerChannel(MONO_MESSAGE);
  m_monoGauge=new LedGauge(LED_MONO_RED,LED_MONO_YEL,LED_MONO_GRN);
  m_pSimpit->registerChannel(ELECTRIC_MESSAGE);
  m_electricGauge=new LedGauge(LED_ELEC_RED,LED_ELEC_YEL,LED_ELEC_GRN);
  m_pSimpit->registerChannel(AB_MESSAGE);
  m_ablatorGauge=new LedGauge(LED_ABLT_RED,LED_ABLT_YEL,LED_ABLT_GRN);
  m_pSimpit->registerChannel(AUTOPILOT_MESSAGE);
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
    case ACTIONSTATUS_MESSAGE:
      if (msgSize==1)
        m_actionStatus=msg[0];
      break;
    case LF_MESSAGE:
      if (msgSize==sizeof(resourceMessage))
      {
        resourceMessage res;
        res=parseResource(msg);
        m_fuelGauge->illuminate(res.available, res.total);
      }
      break;
    case MONO_MESSAGE:
      if (msgSize==sizeof(resourceMessage))
      {
        resourceMessage res;
        res=parseResource(msg);
        m_monoGauge->illuminate(res.available, res.total);
      }
      break;
    case ELECTRIC_MESSAGE:
      if (msgSize==sizeof(resourceMessage))
      {
        resourceMessage res;
        res=parseResource(msg);
        m_electricGauge->illuminate(res.available, res.total);
      }
      break;
    case AB_MESSAGE:
      if (msgSize==sizeof(resourceMessage))
      {
        resourceMessage res;
        res=parseResource(msg);
        m_ablatorGauge->illuminate(res.available, res.total);
      }
      break;
    case AUTOPILOT_MESSAGE:
      if (msgSize==1)
      {
        digitalWrite(LED_SAS_STABILITY, (msg[0]==AP_STABILITY)  ?HIGH:LOW);
        digitalWrite(LED_SAS_PROGRADE,  (msg[0]==AP_PROGRADE)   ?HIGH:LOW);
        digitalWrite(LED_SAS_RETROGRADE,(msg[0]==AP_RETROGRADE) ?HIGH:LOW);
        digitalWrite(LED_SAS_NORM,      (msg[0]==AP_NORMAL)     ?HIGH:LOW);
        digitalWrite(LED_SAS_ANTI_NORM, (msg[0]==AP_ANTINORMAL) ?HIGH:LOW);
        digitalWrite(LED_SAS_RAD,       (msg[0]==AP_RADIALIN)   ?HIGH:LOW);
        digitalWrite(LED_SAS_ANTI_RAD,  (msg[0]==AP_RADIALOUT)  ?HIGH:LOW);
        digitalWrite(LED_SAS_TARG,      (msg[0]==AP_TARGET)     ?HIGH:LOW);
        digitalWrite(LED_SAS_ANTI_TARG, (msg[0]==AP_ANTITARGET) ?HIGH:LOW);
        digitalWrite(LED_SAS_MANOUEVRE, (msg[0]==AP_MANEUVER)   ?HIGH:LOW);
        m_autopilotMode=msg[0];
      }
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

