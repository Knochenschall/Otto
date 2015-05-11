#include "Otto.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "IBitmapMonoText.h"

const int kNumPrograms = 1;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

enum EParams
{
	kButton1 = 0,
	kButton2,
	kButton3,
	kButton4,
	kButton5,
	kButton6,
	kButton7,
	kButton8,

	kDownsample,

	kDry,
	kWet,
	kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kButtonsContainerX = 50,
	kButtonsContainerY = 90,
	kButtonWidth = 42,

	kDownsampleX = 315,
	kDownsampleY = 200,

	kDryX = 53,
	kDryY = kDownsampleY,

	kWetX = 110,
	kWetY = kDownsampleY,

	kKnobFrames = 64
};

enum eBitState
{
	kBitOn = 0,
	kBitInv = 1,
	kBitOff = 2
	
};
Otto::Otto(IPlugInstanceInfo instanceInfo)
	: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDownsample(1), mCurrentSample(0), mDry(1.0), mWet(0.0)
{
	TRACE;

	// -------- INIT PARAMS ----------
	for (int i = 0; i < kNumBits; i++){ // ranges from kButton1 to kButton8

		/* init the off and invert bit to no effect */
		mOnOffBits[i] = 1;
		mInvertBits[i] = 0;

		/* init the IParam for the buttons */
		char ButtonLabel[9];
		sprintf(ButtonLabel, "Button %d", i+1);
		GetParam(i)->InitEnum(ButtonLabel, 0, 3);
		GetParam(i)->SetDisplayText(kBitOn, "On");
		GetParam(i)->SetDisplayText(kBitInv, "Inv");
		GetParam(i)->SetDisplayText(kBitOff, "Off"); 
		
	}

	GetParam(kDownsample)->InitInt("Downsample", 1, 1, 64);

	GetParam(kDry)->InitDouble("Dry", -61.0, -61.0, 0., 0.2, "dB","",0.2);
	GetParam(kWet)->InitDouble("Wet", 0.0, -61.0, 0., 0.2, "dB", "", 0.2);
	
	// --------- BUILD GUI -----------
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);
	
	IText text(12, &COLOR_WHITE, "Courier", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
	

	IBitmap bitmap = pGraphics->LoadIBitmap(BUTTON_ID, BUTTON_FN, 3);

	for (int i = 0; i < kNumBits; i++){  // ranges from kButton1 to kButton8
		pGraphics->AttachControl(new ISwitchControl(this, kButtonsContainerX + (i * kButtonWidth), kButtonsContainerY, i, &bitmap));
	}
	
	IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
	
	// IRECT constructor args are:  left, top, right, bottom
	pGraphics->AttachControl(new IKnobMultiControl(this, kDownsampleX, kDownsampleY, kDownsample, &knob));
	IRECT degradeLabelRect(kDownsampleX, kDownsampleY + 40, kDownsampleX + 45, kDownsampleY + 40 + 20); //  
	pGraphics->AttachControl(new ITextControl(this, degradeLabelRect, &text, "Downsample"));
	
	pGraphics->AttachControl(new IKnobMultiControl(this, kDryX, kDryY, kDry, &knob));
	IRECT dryLabelRect(kDryX, kDryY + 40, kDryX + 45, kDryY + 40 + 20); //  // left, top, right, bottom
	pGraphics->AttachControl(new ITextControl(this, dryLabelRect, &text, "Dry"));

	pGraphics->AttachControl(new IKnobMultiControl(this, kWetX, kWetY, kWet, &knob));
	IRECT wetLabelRect(kWetX, kWetY + 40, kWetX + 45, kWetY + 40 + 20); //  // left, top, right, bottom
	pGraphics->AttachControl(new ITextControl(this, wetLabelRect, &text, "Wet"));


	AttachGraphics(pGraphics);
	//MakePreset("preset 1", ... );
	MakeDefaultPreset((char *) "-", kNumPrograms);
}

Otto::~Otto() {}

void Otto::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	int nChannels = (IsInChannelConnected(1) && IsOutChannelConnected(1) ? 2 : 1);

	for (int channel = 0; channel < nChannels; channel++){
		for (int frame = 0; frame < nFrames; frame++)
		{
			double in = inputs[channel][frame];
			double out = inputs[channel][frame];

			/* downsampling with sample and hold */
			if (frame % mDownsample == 0){
				mCurrentSample = in;
			}

			out = mCurrentSample;

			/* bit crusher */
			/* I took the algorithm from this brilliant video
			   https://www.youtube.com/watch?v=ohYQe5kzZg8 and ported it into C++ */
			if (!mOnOffBits.all() || mInvertBits.any()){
				/* enters the if only if at least one button is either red or grey */

				int signIn = sgn(out);

				out = std::abs(out);
				out *= 255.0;

				unsigned int roundedIn = int(round(out));
				roundedIn &= mOnOff;
				roundedIn ^= mInvert;

				out = (double(roundedIn) / 255.0) * signIn;
			}

			outputs[channel][frame] = mix(in, out); // in is the wet now 
		}
	}
}

void Otto::Reset()
{
	TRACE;
	IMutexLock lock(this);

	//double sr = GetSampleRate();
}

void Otto::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);

	switch (paramIdx) {
	case kDownsample:{
		mDownsample = GetParam(paramIdx)->Int();
	}; break;

	case kDry: {
		if (GetParam(kDry)->Value() < -60.){ // when it goes below 60db just turn it off
			mDry = 0.0;
		}
		else {
			mDry = ::DBToAmp(GetParam(kDry)->Value());
		}

	}; break;

	case kWet: {
		if (GetParam(kWet)->Value() < -60.0){
			mWet = 0.0;
		}
		else{
			mWet = ::DBToAmp(GetParam(kWet)->Value());
		}
	}; break;

	default: { // executed for buttons, paramIdx = 0 - 7

		int bit = GetParam(paramIdx)->Int();

		switch (bit) {
		case kBitOn:
			mOnOffBits.set(paramIdx, 1);
			mOnOff = mOnOffBits.to_ulong();

			mInvertBits.set(paramIdx, 0);
			mInvert = mInvertBits.to_ulong();
			break;
		case kBitOff:
			mOnOffBits.set(paramIdx, 0);
			mOnOff = mOnOffBits.to_ulong();

			mInvertBits.set(paramIdx, 0);
			mInvert = mInvertBits.to_ulong();
			break;
		case kBitInv:
			mOnOffBits.set(paramIdx, 1);
			mOnOff = mOnOffBits.to_ulong();

			mInvertBits.set(paramIdx, 1);
			mInvert = mInvertBits.to_ulong();
			break;
		}
	};  break; // outer switch default:
	}
}

double Otto::mix(double dry, double wet) const {
		return dry * mDry + wet * mWet;
}