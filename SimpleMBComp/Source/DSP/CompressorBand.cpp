/*
  ==============================================================================

    CompressorBand.cpp
    Created: 16 Oct 2024 3:14:25pm
    Author:  Abhinav

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }
    
    void CompressorBand::updateCompressorSettings()
    {
        compressor.setAttack(attack ->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }
    
    void CompressorBand::process(juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypass->get();

        compressor.process(context);
    }