//------------------------------------------------------------------------
// Copyright(c) 2022 drmcfish.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

enum GainParams : Steinberg::Vst::ParamID
{
	kParamGainId = 102,
};

namespace MyCompanyName {
//------------------------------------------------------------------------
static const Steinberg::FUID kTestPlugProcessorUID (0xDA3105C8, 0xD5F25EA3, 0x8711BE5A, 0x58518E29);
static const Steinberg::FUID kTestPlugControllerUID (0x0C5BF768, 0x06B154FE, 0xA360402A, 0x0042496A);

#define TestPlugVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace MyCompanyName
