//------------------------------------------------------------------------
// Copyright(c) 2022 drmcfish.
//------------------------------------------------------------------------

#include "mypluginprocessor.h"
#include "myplugincids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include <math.h>

using namespace Steinberg;

namespace MyCompanyName {

const float ln512 = 6.2383246250395077847550890931236;
//------------------------------------------------------------------------
// TestPlugProcessor
//------------------------------------------------------------------------
TestPlugProcessor::TestPlugProcessor()
{
	//--- set the wanted controller for our processor
	setControllerClass(kTestPlugControllerUID);
}

//------------------------------------------------------------------------
TestPlugProcessor::~TestPlugProcessor()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::initialize(FUnknown* context)
{
	// Here the Plug-in will be instanciated

	//---always initialize the parent-------
	tresult result = AudioEffect::initialize(context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput(STR16("Event In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::terminate()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::setActive(TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::process(Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	//*
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
			if (paramQueue)
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId ())
				{
					case GainParams::kParamGainId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mGain = value;
						break;
				}
			}
		}
	}

	// Flush case
	if (data.numInputs == 0 || data.numSamples == 0)
		return kResultOk;

	//--- Here you have to implement your processing
	int32 numChannels = data.inputs[0].numChannels;

	// get audio buffers
	uint32 sampleFramesSize = Vst::getSampleFramesSizeInBytes(processSetup, data.numSamples);
	void** in = Vst::getChannelBuffersPointer(processSetup, data.inputs[0]);
	void** out = Vst::getChannelBuffersPointer(processSetup, data.outputs[0]);
	
	// Here could check the silent flags
	//---check if silence---------------
	// normally we have to check each channel (simplification)
	if (data.inputs[0].silenceFlags != 0)
	{
		// mark output silence too
		data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

		// the Plug-in has to be sure that if it sets the flags silence that the output buffer are clear
		for (int32 i = 0; i < numChannels; i++)
		{
			// do not need to be cleared if the buffers are the same (in this case input buffer are
				// already cleared by the host)
			if (in[i] != out[i])
			{
				memset(out[i], 0, sampleFramesSize);
			}
		}
		// nothing to do at this point
		return kResultOk;
	}

	// now we will produce the output

	float gain = (float)exp(ln512 * (mGain));
	for (int32 i = 0; i < numChannels; i++) //channels
	{
		int32 samples = data.numSamples;
		Vst::Sample32* ptrIn = (Vst::Sample32*)in[i];
		Vst::Sample32* ptrOut = (Vst::Sample32*)out[i];
		Vst::Sample32 tmp;
		while (--samples >= 0)
		{
			//apply gain
			tmp = atan((*ptrIn++) * gain);
			//tmp = (gain == 0.f) ? 0.f : tmp / atan(gain);
			(*ptrOut++) = tmp;
		}
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	float savedParam1 = 0.f;
	if(!streamer.readFloat(savedParam1))
		return kResultFalse;

	mGain = savedParam1;
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API TestPlugProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat((float)mGain);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
