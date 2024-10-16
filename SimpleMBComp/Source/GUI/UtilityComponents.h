/*
  ==============================================================================

    UtilityComponents.h
    Created: 16 Oct 2024 2:31:35pm
    Author:  Abhinav

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider();
};