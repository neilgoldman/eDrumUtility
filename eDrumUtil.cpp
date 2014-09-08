#include "eDrumUtil.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"
#include <vector>

//TODO: Be able to lock hi hat pedal
//TODO: Learning pedal and hi-hat midi values
//TODO: Also display mNoteMinSampleLength in ms
//TODO: Interface that doesn't suck

const int kNumPrograms = 1;


void eDrumUtil::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	while (!mMidiQueue.Empty())
	{
		IMidiMsg* pMsg = mMidiQueue.Peek();
		if (pMsg->mOffset > nFrames)
			break;

		if (pMsg->StatusMsg() == IMidiMsg::kNoteOn)
		{
			int note = pMsg->mData1;
			if (mKeyStatus[note]) {
				auto offMsg = new IMidiMsg();
				offMsg->MakeNoteOffMsg(note, pMsg->mOffset, pMsg->Channel());
				SendMidiMsg(offMsg);
			}
		}

		SendMidiMsg(pMsg);
		mMidiQueue.Remove();
	}

	mMidiQueue.Flush(nFrames);
}


void eDrumUtil::ProcessMidiMsg(IMidiMsg* pMsg)
{
	int status = pMsg->StatusMsg();
	int note = pMsg->mData1;		// Note value or CC msg number
	int velocity = pMsg->mData2;	// Note velocity or CC msg value

	if (status == IMidiMsg::kControlChange && note == mIncomingPedalCC)
	{
		mCorrectHiHatNote = mHiHatTable[velocity];
		mMidiQueue.Add(pMsg);
		return;
	}

	//We can handle our own note off, thankyouverymuch
	if (status == IMidiMsg::kNoteOff && mNoteMinSampleLength > 1)
		return;

	if (status == IMidiMsg::kNoteOn)
	{
		if (note == mIncomingHiHatNote)
		{
			pMsg->mData1 = mCorrectHiHatNote;
		}
		mMidiQueue.Add(pMsg);

		if (mNoteMinSampleLength > 1)
		{
			auto offMsg = new IMidiMsg();
			offMsg->MakeNoteOffMsg(pMsg->mData1, pMsg->mOffset + mNoteMinSampleLength, pMsg->Channel());	
			mMidiQueue.Add(offMsg);
		}
		return;
	}	
	
	mMidiQueue.Add(pMsg);
}


void eDrumUtil::SetUpHiHatTable()
{
	if (mStyles.size() == 0)
		return;
	// Fill the lookup table with all potential pedal CC values.
	// We assign each index to our current HitHatStyle's note value
	// until our index is higher than the HiHatStyle range.
	// If it is, we move on to the next style with a non-negative note,
	// camping at the final one for all remaining indexes
	// I'm explaining this now, because double-dereferencing iterators to pointers is confusing.

	auto currStyle = mStyles.begin();

	for (int i = 0; i < NUM_MIDI_NOTES; i++)
	{
		while (((*currStyle)->rangeEnd < i || (*currStyle)->note == -1) && currStyle != mStyles.end()-1)
		{
			currStyle++;
		}
		if (mInvertPedalCC)
			mHiHatTable[NUM_MIDI_NOTES-i-1] = (*currStyle)->note;
		else
			mHiHatTable[i] = (*currStyle)->note;
	}
}


/****************************************
	Boring setup stuff goes here
****************************************/

enum EParams
{
	kIsLearningPedal = 0,
	kIsLearningHiHat,
	kIncomingPedalCC,
	kIncomingHiHatNote,

	kHatTightClosedNote,
	kHatFullClosedNote,
	kHatQuarterOpenNote,
	kHatHalfOpenNote,
	kHatThreeQuarterOpenNote,
	kHatLooseOpenNote,
	kHatFullOpenNote,

	kHatTightClosedValue,
	kHatFullClosedValue,
	kHatQuarterOpenValue,
	kHatHalfOpenValue,
	kHatThreeQuarterOpenValue,
	kHatLooseOpenValue,
	kHatFullOpenValue,

	kIsPedalLocked,
	kNoteMinSampleLength,
	kInvertPedalCC,
	kNumParams
};

eDrumUtil::eDrumUtil(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
mSampleRate(44100.)
{
	TRACE;

	//==
	GetParam(kHatTightClosedNote)->InitInt("HatTightClosedNote", 22, -1, 127, "HiHat Tight Closed Note");
	GetParam(kHatFullClosedNote)->InitInt("HatFullClosedNote", 23, -1, 127, "HiHat Full Closed Note");
	GetParam(kHatQuarterOpenNote)->InitInt("HatQuarterOpenNote", 24, -1, 127, "HiHat Quarter Open Note");
	GetParam(kHatHalfOpenNote)->InitInt("HatHalfOpenNote", 25, -1, 127, "HiHat Half Open Note");
	GetParam(kHatThreeQuarterOpenNote)->InitInt("HatThreeQuarterOpenNote", 26, -1, 127, "HiHat Three Quarter Open Note");
	GetParam(kHatLooseOpenNote)->InitInt("HatLooseOpenNote", 27, -1, 127, "HiHat Loose Open Note");
	GetParam(kHatFullOpenNote)->InitInt("HatFullOpenNote", 28, -1, 127, "HiHat Full Open Note");

	GetParam(kHatTightClosedValue)->InitInt("HatTightClosedUpperValue", 4, 0, 127, "Hat Tight Closed Upper Value");
	GetParam(kHatFullClosedValue)->InitInt("HatFullClosedUpperValue", 16, 0, 127, "Hat Full Closed Upper Value");
	GetParam(kHatQuarterOpenValue)->InitInt("HatQuarterOpenUpperValue", 40, 0, 127, "Hat Quarter Open Upper Value");
	GetParam(kHatHalfOpenValue)->InitInt("HatHalfOpenUpperValue", 72, 0, 127, "Hat Half Open Upper Value");
	GetParam(kHatThreeQuarterOpenValue)->InitInt("HatThreeQuarterOpenUpperValue", 96, 0, 127, "Hat ThreeQuarterOpen Upper Value");
	GetParam(kHatLooseOpenValue)->InitInt("HatLooseOpenUpperValue", 120, 0, 127, "Hat Loose Open Upper Value");
	GetParam(kHatFullOpenValue)->InitInt("HatFullOpenUpperValue", 127, 0, 127, "Hat Full Open Upper Value");

	mHatTightClosed = HiHatStyle(22, 4);
	mHatFullClosed = HiHatStyle(23, 16);
	mHatQuarterOpen = HiHatStyle(24, 40);
	mHatHalfOpen = HiHatStyle(25, 72);
	mHatThreeQuarterOpen = HiHatStyle(26, 96);
	mHatLooseOpen = HiHatStyle(27, 120);
	mHatFullOpen = HiHatStyle(28, 127);

	GetParam(kIsLearningPedal)->InitBool("IsLearningPedal", false, "Learn e-drum HH Pedal?");
	GetParam(kIsLearningHiHat)->InitBool("IsLearningHiHat", false, "Learn e-drum HH Cymbal");
	GetParam(kIncomingPedalCC)->InitInt("PedalIncomingCC", 4, -1, 127, "Pedal Incoming CC");
	GetParam(kIncomingHiHatNote)->InitInt("HiHatIncomingNote", 46, -1, 127, "HiHat Incoming Note");
	GetParam(kIsPedalLocked)->InitBool("IsPedalLocked", false, "Lock Hi Hat Pedal");
	GetParam(kNoteMinSampleLength)->InitInt("MinHitSampleLength", 50, 0, 1000, "Duration of hits in samples");
	GetParam(kInvertPedalCC)->InitBool("InvertPedalCC", false, "Invert Pedal CC");

	mIsLearningPedal = false;
	mIsLearningHiHat = false;
	mIncomingPedalCC = 4;
	mIncomingHiHatNote = 46;
	mIsPedalLocked = false;
	mNoteMinSampleLength = 50;
	//==

	mStyles = {
		&mHatTightClosed,
		&mHatFullClosed,
		&mHatQuarterOpen,
		&mHatHalfOpen,
		&mHatThreeQuarterOpen,
		&mHatLooseOpen,
		&mHatFullOpen
	};

	memset(mKeyStatus, 0, 128 * sizeof(bool));

	mHiHatTable = new BYTE[NUM_MIDI_NOTES];
	SetUpHiHatTable();
	mCorrectHiHatNote = mHiHatTable[MIDI_MAX_VALUE];

	MakeDefaultPreset((char *) "-", kNumPrograms);
}

eDrumUtil::~eDrumUtil()
{
	delete mHiHatTable;
}

void eDrumUtil::Reset()
{
	TRACE;
	IMutexLock lock(this);

	mSampleRate = GetSampleRate();
	mMidiQueue.Resize(GetBlockSize());
	mMidiNoteOffQueue.Resize(GetBlockSize());
}

void eDrumUtil::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);

	switch (paramIdx)
	{
		case kIsLearningPedal:
				mIsLearningPedal = GetParam(kIsLearningPedal)->Bool();
				return;
		case kIsLearningHiHat:
				mIsLearningHiHat = GetParam(kIsLearningHiHat)->Bool();
				return;
		case kIncomingPedalCC:
				mIncomingPedalCC = GetParam(kIncomingPedalCC)->Int();
				return;
		case kIncomingHiHatNote:
				mIncomingHiHatNote = GetParam(kIncomingHiHatNote)->Int();
				return;
		case kIsPedalLocked:
				mIsPedalLocked = GetParam(kIsPedalLocked)->Bool();
				return;
		case kNoteMinSampleLength:
				mNoteMinSampleLength = GetParam(kNoteMinSampleLength)->Int();
				return;
		case kInvertPedalCC:
				mIncomingPedalCC = GetParam(kIncomingPedalCC)->Int();
				return;
		//==
		//Below here triggers SetUpHiHatTable
		case kHatTightClosedNote:
				mHatTightClosed.note = GetParam(kHatTightClosedNote)->Int();
				break;
		case kHatFullClosedNote:
				mHatFullClosed.note = GetParam(kHatFullClosedNote)->Int();
				break;
		case kHatQuarterOpenNote:
				mHatQuarterOpen.note = GetParam(kHatQuarterOpenNote)->Int();
				break;
		case kHatHalfOpenNote:
				mHatHalfOpen.note = GetParam(kHatHalfOpenNote)->Int();
				break;
		case kHatThreeQuarterOpenNote:
				mHatThreeQuarterOpen.note = GetParam(kHatThreeQuarterOpenNote)->Int();
				break;
		case kHatLooseOpenNote:
				mHatLooseOpen.note = GetParam(kHatLooseOpenNote)->Int();
				break;
		case kHatFullOpenNote:
				mHatFullOpen.note = GetParam(kHatFullOpenNote)->Int();
				break;
		case kHatTightClosedValue:
				mHatTightClosed.rangeEnd = GetParam(kHatTightClosedValue)->Int();
				break;
		case kHatFullClosedValue:
				mHatFullClosed.rangeEnd = GetParam(kHatFullClosedValue)->Int();
				break;
		case kHatQuarterOpenValue:
				mHatQuarterOpen.rangeEnd = GetParam(kHatQuarterOpenValue)->Int();
				break;
		case kHatHalfOpenValue:
				mHatHalfOpen.rangeEnd = GetParam(kHatHalfOpenValue)->Int();
				break;
		case kHatThreeQuarterOpenValue:
				mHatThreeQuarterOpen.rangeEnd = GetParam(kHatThreeQuarterOpenValue)->Int();
				break;
		case kHatLooseOpenValue:
				mHatLooseOpen.rangeEnd = GetParam(kHatLooseOpenValue)->Int();
				break;
		case kHatFullOpenValue:
				mHatFullOpen.rangeEnd = GetParam(kHatFullOpenValue)->Int();
				break;
		default:
			break;
	}

	SetUpHiHatTable();
	mCorrectHiHatNote = mHiHatTable[MIDI_MAX_VALUE];
}
