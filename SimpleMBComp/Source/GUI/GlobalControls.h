/*
  ==============================================================================

    GlobalControls.h
    Created: 16 Oct 2024 3:08:31pm
    Author:  Abhinav

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;
    
    // preparing allocating these sliders on the heap. Now declaring a unique_ptr to the attachment and create one attachment per slider
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> inGainSliderAttachment,
                                lowMidXoverSliderAttachment,
                                midHighXoverSliderAttachment,
                                outGainSliderAttachment;
    
};
