/*
  ==============================================================================

    CustomButtons.h
    Created: 16 Oct 2024 2:26:30pm
    Author:  Abhinav

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton {
    void resized() override;    
    juce::Path randomPath;
};
