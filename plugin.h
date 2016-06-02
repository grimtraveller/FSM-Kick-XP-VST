#ifndef __VST_PLUGIN_H__
#define __VST_PLUGIN_H__

#include <vector>
#include <public.sdk/source/vst2.x/audioeffectx.h>

#ifdef DEBUG
#include <io.h>
#include <windows.h>
#include <conio.h>
#include <fcntl.h>
#define DEBUG_CONSOLE
#define dprintf(...) printf (__VA_ARGS__)
#define DISPATCHER_DEBUG_TRACE
#else
#define dprintf(...) {}
#endif


#define PLUGIN_EFFECT_NAME "None"
#define PLUGIN_VENDOR_NAME "FSM"
#define PLUGIN_PRODUCT_NAME "FSM Kick XP"
#define LOG_SCALE_GAIN (0.3)
#define SCALE_GAIN_OVERHEAD (1.3)

#define NOTE_OFF				255

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

class FSM_VST_Plugin;

enum
{
	// Global
	kNumPrograms = 100,
	kNumOutputs = 2,
};

enum
{
	kVolume,
	kStartFrq,
	kEndFrq,
	kBuzzAmt,

	kClickAmt,
	kPunchAmt,
	kToneDecay,
	kToneShape,
	kBDecay,

	kCDecay,
	kDecSlope,
	kDecTime,
	kRelSlope,
	kNumParams
};

class SynthParameter
{
public:
	char const *Name;		// Short name: "Cutoff"
	char const *Description;// Longer description: "Cutoff Frequency (0-7f)"
	int MinValue;			// 0
	int MaxValue;			// 127
	int DefValue;			// default value for params that have MPF_STATE flag set
};


class ProgramParameters
{
public:
	float bStartFrq;
	float bEndFrq;
	float bBuzzAmt;

	float bClickAmt;
	float bPunchAmt;
	float bToneDecay;
	float bToneShape;
	float bBDecay;

	float bCDecay;
	float bDecSlope;
	float bDecTime;
	float bRelSlope;
};


class VoiceParameters
{
public:
	float PitchLimit;
	float ThisPitchLimit;
	float StartFrq;
	float ThisStartFrq;
	float EndFrq;
	float ThisEndFrq;
	float TDecay;
	float ThisTDecay;
	float TShape;
	float ThisTShape;
	float DSlope;
	float ThisDSlope;
	float DTime;
	float ThisDTime;
	float RSlope;
	float ThisRSlope;
	float BDecay;
	float ThisBDecay;
	float CDecay;
	float ThisCDecay;
	float CurVolume;
	float ThisCurVolume;
	float LastValue;
	float AntiClick;
	float ClickAmt;
	float PunchAmt;
	float BuzzAmt;
	float Amp;
	float DecAmp;
	float BAmp;
	float MulBAmp;
	float CAmp;
	float MulCAmp;
	float Frequency;
	int SamplesToGo;
	int Retrig;
	int RetrigCount;

	double xSin, xCos, dxSin, dxCos;

	int EnvPhase;
	int LeftOver;
	int Age;
	double OscPhase;
};

class FSM_Voice : public VoiceParameters
{
public:
	VstInt32 currentNote;
	VstInt32 currentVelocity;
	VstInt32 currentDelta;
	bool released;
	FSM_Voice(VstInt32 note, VstInt32 velocity, VstInt32 delta);
	void release();
	void setParameters(ProgramParameters* vals, float sr);
	void trigger();
	double velocity()const
	{
		return currentVelocity / 127.0;
	}

};


//------------------------------------------------------------------------------------------
// FSM_VST_Program
//------------------------------------------------------------------------------------------

class FSM_VST_Program : public ProgramParameters
{
	friend class FSM_VST_Plugin;
public:
	FSM_VST_Program ();
	~FSM_VST_Program() {}
	void set(char* _name, int start, int end,
		int buzz, int click, int punch, int tDecR, int tDecS,
		int bDecR, int CPDecR, int ADecS, int ADecT, int ARelS);

private:
	char name[kVstMaxProgNameLen+1];
};


//------------------------------------------------------------------------------------------
// FSM_VST_Plugin
//------------------------------------------------------------------------------------------

class FSM_VST_Plugin : public AudioEffectX {

public:
	FSM_VST_Plugin (audioMasterCallback audioMaster);
	~FSM_VST_Plugin ();

	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* events);

	virtual void setProgram(VstInt32 program);
	virtual void setProgramName(char* name);
	virtual void getProgramName(char* name);
	virtual bool beginSetProgram() { this->issetprogram = true; return false; }	///< Called before a program is loaded
	virtual bool endSetProgram() { this->issetprogram = false; return false; }		///< Called after a program was loaded
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual void setSampleRate (float sampleRate);
	virtual void setBlockSize (VstInt32 blockSize);

	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	virtual VstInt32 canDo (char* text);

	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();

	FSM_VST_Program* current() {
		return &(curProgram >= 0 && curProgram < kNumPrograms ? programs[curProgram] : programs[0]);
	}

#ifdef DISPATCHER_DEBUG_TRACE
	VstIntPtr dispatcher(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
#endif // DEBUG

private:

#ifdef DEBUG_CONSOLE
	void startConsoleWin(int width, int height, char* fname);
	HANDLE hout;
#endif // DEBUG_CONSOLE

private:
	void noteOn(VstInt32 note, VstInt32 velocity, VstInt32 delta);
	void noteOff(VstInt32 note);
	void allNotesOff(bool decay);
	void initProcess();
	bool processVoice(FSM_Voice *trk, float *pout, int c, float gain);

	FSM_VST_Program* programs;
	std::vector<FSM_Voice*> voices;
	float fVolume;
	bool issetprogram;
	float thumpdata1[1024];

};

#endif