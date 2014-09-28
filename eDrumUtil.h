#ifndef __EDRUMUTIL__
#define __EDRUMUTIL__

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"
#include <vector>


#define NUM_MIDI_NOTES 128
#define MIDI_MIN_VALUE 0
#define MIDI_MAX_VALUE 127


struct HiHatStyle
{
	int note = -1;
	int rangeEnd = -1;

	HiHatStyle::HiHatStyle() {};
	HiHatStyle::HiHatStyle(int _note, int _rangeEnd)
	{
		note = _note;
		rangeEnd = _rangeEnd;
	};
	HiHatStyle::~HiHatStyle() {};
};

class eDrumUtil : public IPlug
{
public:

	eDrumUtil(IPlugInstanceInfo instanceInfo);
	~eDrumUtil();

	void Reset();
	void OnParamChange(int paramIdx);

	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
	void ProcessMidiMsg(IMidiMsg* pMsg);

private:

	bool mIsLearningPedal;
	bool mIsLearningHiHat;
	int mIncomingPedalCC;
	int mIncomingHiHatNote;

	bool mIsPedalLocked;
	int mNoteMinSampleLength;
	bool mInvertPedalCC;

	// Now, it would be smart to just use a vector of notes and
	// CC values, so we can have as many or as few as we want.
	// But that makes the GUI very hard to set up, so....
	// Also I'm in a hurry to get this done before we begin recording
	// And I'm really only planning on supporting Studio Drummer
	// (at least for v1.0)
	HiHatStyle mHatTightClosed;
	HiHatStyle mHatFullClosed;
	HiHatStyle mHatQuarterOpen;
	HiHatStyle mHatHalfOpen;
	HiHatStyle mHatThreeQuarterOpen;
	HiHatStyle mHatLooseOpen;
	HiHatStyle mHatFullOpen;

	std::vector<HiHatStyle*> mStyles; //This helps to make SetUpHiHatTable() pretty

	bool mKeyStatus[NUM_MIDI_NOTES]; // array of note-on lifespan for each key
	
	IMidiQueue mMidiQueue;
	IMidiQueue mMidiNoteOffQueue;
	
	BYTE* mHiHatTable;
	BYTE mCorrectHiHatNote;

	double mSampleRate;

	void SetUpHiHatTable();

};


//enum ELayout
//{
//	kWidth = GUI_WIDTH,  // width of plugin window
//	kHeight = GUI_HEIGHT, // height of plugin window
//};

#endif //__EDRUMUTIL__
