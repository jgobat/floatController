#include <i2c_t3.h>
#include <infinityPV_INA233.h>
#include <printf.h>

INA233 ina233(0x40);

elapsedMillis ina233_since;

void
initINA233(void)
{
  uint16_t CAL;
  int16_t m_c = 0, m_p = 0;
  int8_t R_c = 0, R_p =  0;
  uint8_t Set_ERROR = 0;
  float Power_LSB = 0, Current_LSB = 0;
  uint16_t Read_CAL = 0;

  ina233.begin();
  ina233.setCalibration(0.025, 3.0, &Current_LSB,&Power_LSB,&m_c,&R_c,&m_p,&R_p, &Set_ERROR);  
  ina233_since = 0;
}


int
debugINA233(int argc, char **argv)
{
  printf("Raw Bus V:   0x%x\n", ina233.getBusVoltage_raw());
  printf("Raw shunt V: 0x%x\n", ina233.getShuntVoltage_raw());
  printf("Raw current: 0x%x\n", ina233.getCurrent_raw());
  printf("Raw power:   0x%x\n", ina233.getPower_raw());
  printf("Bus V:   %.2f V\n", ina233.getBusVoltage_V());
  printf("Shunt V: %.2f V\n", ina233.getShuntVoltage_mV());
  printf("Current: %.2f mA\n", ina233.getCurrent_mA());
  printf("Power:   %.2f mW\n", ina233.getPower_mW());

  return 0;
}


long
readINA233(float *V, float *mA, float *mW, float *av_mW)
{
    long latch;

    if (V)
        *V = ina233.getBusVoltage_V();
    if (mA)
        *mA = ina233.getCurrent_mA();
    if (mW)
        *mW = ina233.getPower_mW();
    if (av_mW)
        *av_mW = ina233.getAv_Power_mW();
    
    latch = ina233_since;
    ina233_since = 0;
    
    return latch;
}
