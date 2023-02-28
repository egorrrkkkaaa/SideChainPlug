#include "SideChainPlug.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

const double METER_ATTACK = 0.6, METER_DECAY = 0.05;

enum EParams
{
  kGain = 0,
  kGain2,
  kThrash,
  kFade,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGain1X = 23,
  kGain1Y = 200,

  kGain2X = 123,
  kGain2Y = 200,

  kThrashX = 223,
  kThrashY = 200,

  kFadeX = 223,
  kFadeY = 70,

  kMeterL_X = 30,
  kMeterL_Y = 30,
  kMeterL_W = 20,
  kMeterL_H = 150,

  kMeterR_X = 60,
  kMeterR_Y = 30,
  kMeterR_W = 20,
  kMeterR_H = 150,

  kMeterLS_X = 130,
  kMeterLS_Y = 30,
  kMeterLS_W = 20,
  kMeterLS_H = 150,

  kMeterRS_X = 160,
  kMeterRS_Y = 30,
  kMeterRS_W = 20,
  kMeterRS_H = 150,

  kKnobFrames = 60
};

SideChainPlug::SideChainPlug(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo){
  TRACE;

  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(1.);

  GetParam(kGain2)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kGain2)->SetShape(1.);

  GetParam(kThrash)->InitDouble("Thrash", 50., 0., 100.0, 0.01, "%");
  GetParam(kThrash)->SetShape(1.);

  GetParam(kFade)->InitInt("Fade", 22050, 1, 44100, "%");
  GetParam(kFade)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN);
  IText text = IText(14);
   pGraphics->AttachControl(new IKnobRotaterControl(this, kFadeX, kFadeY, kFade, &knob));
   pGraphics->AttachControl(new IKnobRotaterControl(this, kGain1X, kGain1Y, kGain, &knob));
   pGraphics->AttachControl(new IKnobRotaterControl(this, kGain2X, kGain2Y, kGain2, &knob)); 
   pGraphics->AttachControl(new IKnobRotaterControl(this, kThrashX, kThrashY, kThrash, &knob));
   

  pGraphics->AttachBackground(BG_ID, BG_FN);
  mMeterIdx_L = pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterL)));
  mMeterIdx_R = pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterR)));
  mMeterIdx_LS = pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterLS)));
  mMeterIdx_RS = pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterRS)));

  if (GetAPI() == kAPIVST2) 
  {
    SetInputLabel(0, "main input L");
    SetInputLabel(1, "main input R");
    SetInputLabel(2, "sc input L");
    SetInputLabel(3, "sc input R");
    SetOutputLabel(0, "output L");
    SetOutputLabel(1, "output R");
  }
  else
  {
    SetInputBusLabel(0, "main input");
    SetInputBusLabel(1, "sc input");
    SetOutputBusLabel(0, "output");
  }

  //mGain(1.);
  mPrevL = 0.0;
  mPrevR = 0.0;
  mPrevLS = 0.0;
  mPrevRS = 0.0;
  //LowPassFreq = 1.0;

  changer = 0;
  FadeTime = 22050;
  Thrash = 100.0;

  scSwitch = false;

  for (int i = 0; i <= 1; i++)
  {
      buf0[i] = 0;
      buf1[i] = 0;
      buf2[i] = 0;
      buf3[i] = 0;
  }
  iter = 0;

  AttachGraphics(pGraphics);
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

SideChainPlug::~SideChainPlug() {}

void SideChainPlug::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{

  bool in1ic = IsInChannelConnected(0);
  bool in2ic = IsInChannelConnected(1);
  bool in3ic = IsInChannelConnected(2);
  bool in4ic = IsInChannelConnected(3);

  printf("%i %i %i %i, ------------------------- \n", in1ic, in2ic, in3ic, in4ic);
    

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* scin1 = inputs[2];
  double* scin2 = inputs[3];

  double* out1 = outputs[0];
  double* out2 = outputs[1];

  double peakL = 0.0, peakR = 0.0, peakLS = 0.0, peakRS = 0.0;
   
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++scin1, ++scin2, ++out1, ++out2)
  {
      *in1 = *in1 * mGain;
      *in2 = *in2 * mGain;

      *out1 = *in1;
      *out2 = *in2;

      if (changer != FadeTime)
      {
          for (int i = 0; i <= FadeTime; i++)
          {
              LowPassFreq[i] = (0.0 + i) * 0.8 / FadeTime;
          }
          changer = FadeTime;
      }

      
     

    *scin1 = *scin1 * mGain2;
    *scin2 = *scin2 * mGain2;

    
    if (mPrevRS > Thrash || mPrevLS > Thrash)
    {
                
        *out1 = LowPass(out1, 0, LowPassFreq[iter]);
        *out2 = LowPass(out2, 0, LowPassFreq[iter]);
        iter = iter + 1;
        if (iter > FadeTime)
        {
            iter = FadeTime;
        }
        //* out1 = *in1 * 0.5;
        //*out2 = *in2 * 0.5;

        /**out1 = LowPass(out1, 0);
        *out2 = LowPass(out2, 1);*/

    }
    else
    {

        /**out1 = *out1;
        *out2 = *out2;*/


        *out1 = LowPass(out1, 0, LowPassFreq[iter]);
        *out2 = LowPass(out2, 0, LowPassFreq[iter]);
        iter--;
        if (iter < 0)
        {
            iter = 0;
        }
    }
	   	  
    peakL = IPMAX(peakL, fabs(*in1));
    peakR = IPMAX(peakR, fabs(*in2));
    peakLS = IPMAX(peakLS, fabs(*scin1));
    peakRS = IPMAX(peakRS, fabs(*scin2));
  }

  double xL = (peakL < mPrevL ? METER_DECAY : METER_ATTACK);
  double xR = (peakR < mPrevR ? METER_DECAY : METER_ATTACK);
  double xLS = (peakLS < mPrevLS ? METER_DECAY : METER_ATTACK);
  double xRS = (peakRS < mPrevRS ? METER_DECAY : METER_ATTACK);

  peakL = peakL * xL + mPrevL * (1.0 - xL);
  peakR = peakR * xR + mPrevR * (1.0 - xR);
  peakLS = peakLS * xLS + mPrevLS * (1.0 - xLS);
  peakRS = peakRS * xRS + mPrevRS * (1.0 - xRS);

  mPrevL = peakL;
  mPrevR = peakR;
  mPrevLS = peakLS;
  mPrevRS = peakRS;
   

  if (GetGUI())
  {
    GetGUI()->SetControlFromPlug(mMeterIdx_L, peakL);
    GetGUI()->SetControlFromPlug(mMeterIdx_R, peakR);
    GetGUI()->SetControlFromPlug(mMeterIdx_LS, peakLS);
    GetGUI()->SetControlFromPlug(mMeterIdx_RS, peakRS);
  }

}

void SideChainPlug::Reset()
{
  TRACE;
  IMutexLock lock(this);

  //double sr = GetSampleRate();
}

double SideChainPlug::LowPass(double* in, int i, double freq)
{
    if (*in == 0.0 || freq == 0) return *in;

    buf0[i] += (1 - freq) * (*in - buf0[i]);
    buf1[i] += (1 - freq) * (buf0[i] - buf1[i]);
    buf2[i] += (1 - freq) * (buf1[i] - buf2[i]);
    buf3[i] += (1 - freq) * (buf2[i] - buf3[i]);

    return *in = buf3[i];
}

void SideChainPlug::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
        mGain = 2 * GetParam(kGain)->Value()/100;;
      break;
    case kGain2:
        mGain2 = 2 * GetParam(kGain2)->Value() / 100;;
        break;
    case kThrash:
        Thrash = GetParam(kThrash)->Value() / 100;;
        break;
    case kFade:
        FadeTime = GetParam(kFade)->Value();
        break;
    default:
      break;
  }
}