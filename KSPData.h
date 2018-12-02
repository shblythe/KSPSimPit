#include <KerbalSimpit.h>

class KSPData
{
  private:
    static KSPData* sm_pInstance;
    KerbalSimpit *m_pSimpit;
    float m_vvi;
    float m_apoapsis;
    float m_periapsis;
    int32_t m_tApoapsis;
    int32_t m_tPeriapsis;
    int m_msgCount;

    void callbackHandler(byte msgType, byte msg[], byte msgSize);
    
  public:
    KSPData(KerbalSimpit *pSimpit);
    static void static_callbackHandler(byte msgType, byte msg[], byte msgSize);
    void update();
    float get_vvi() { return m_vvi; }
    float get_apoapsis() { return m_apoapsis; }
    float get_periapsis() { return m_periapsis; }
    int32_t get_tApoapsis() { return m_tApoapsis; }
    int32_t get_tPeriapsis() { return m_tPeriapsis; }
    int get_msgCount() { return m_msgCount; }

};

