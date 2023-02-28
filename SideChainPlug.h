#ifndef __SideChainPlug__
#define __SideChainPlug__

#include "IPlug_include_in_plug_hdr.h"
#include "IPlugSideChain_Controls.h"

class SideChainPlug : public IPlug
{
public:

  SideChainPlug(IPlugInstanceInfo instanceInfo);
  ~SideChainPlug();

  void Reset();

  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double LowPass(double* in, int i, double freq);

private:

  double mGain;
  double mGain2;
  double mPrevL, mPrevR, mPrevLS, mPrevRS;
  int mMeterIdx_L, mMeterIdx_R, mMeterIdx_LS, mMeterIdx_RS;

  int FadeTime;
  int changer;

  //Filter
  double Thrash;
  double LowPassFreq[44101];
  double buf0[2];
  double buf1[2];
  double buf2[2];
  double buf3[2];
  int iter;
  bool scSwitch;

};

#endif
