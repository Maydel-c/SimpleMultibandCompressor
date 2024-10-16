/*
  ==============================================================================

    Utilities.cpp
    Created: 16 Oct 2024 2:37:51pm
    Author:  Abhinav

  ==============================================================================
*/

#include "Utilities.h"

juce::String getValString(const juce::RangedAudioParameter& param,
                          bool getLow,
                          juce::String suffix)
{
    juce::String str;
    
    // getting parameter min and max value from parameter range
    
    auto val = getLow ? param.getNormalisableRange().start : param.getNormalisableRange().end;
    
    bool useK = truncateKiloValue(val);
    str << val;
    
    if(useK)
        str << "k";

    str << suffix;
    
    return str;
}

void drawModuleBackground(juce::Graphics& g,
                          juce::Rectangle<int> bounds)
{
    using namespace juce;
//    auto bounds = getLocalBounds();
    g.setColour(Colours::blueviolet);
    g.fillAll();
    
    auto localBounds = bounds;
    
    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
}
