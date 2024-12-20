/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 16 Oct 2024 2:31:35pm
    Author:  Abhinav

  ==============================================================================
*/

#include "UtilityComponents.h"

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

    void Placeholder::paint(juce::Graphics& g)
    {
        g.fillAll(customColor);
    }
    
//==============================================================================
    
    
     RotarySlider::RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                  juce::Slider::TextEntryBoxPosition::NoTextBox)
    { }