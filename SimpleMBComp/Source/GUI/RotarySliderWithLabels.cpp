/*
  ==============================================================================

    RotarySliderWithLabels.cpp
    Created: 16 Oct 2024 2:14:57pm
    Author:  Abhinav

  ==============================================================================
*/

#include "RotarySliderWithLabels.h"
#include "Utilities.h"
#include "CustomButtons.h"


void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderbounds();
    
    auto bounds = getLocalBounds();
    
    g.setColour(Colours::blueviolet);
    g.drawFittedText(getName(),
                     bounds.removeFromTop(getTextHeight() + 2),
                     Justification::centredBottom,
                     1);
 
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
    
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;
    
    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());
    
    auto numChoices = labels.size(); // get the array of labels, iterate through them
    for( int i = 0; i < numChoices; ++i )
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);
        
        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
        
        auto  c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang); // getting the point where the text will be placed
        
        // drawing a rectangle for the text and positioning it at point c
        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());
        
        // drawing the text inside that rectangle
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderbounds() const
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(getTextHeight() * 1.5);
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    // making space for text - shrink size even further based on text size
    size -= getTextHeight() * 1.5;
    
    // creating and positioning a rectangle at top centre of component
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
//    r.setY(2); // 2 pixels from top
    r.setY(bounds.getY());
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if( auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param) )
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;
    
    if( auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param) )
    {
        float val = getValue();
//        if( val > 999.f )
//        {
//            val /= 1000.f;
//            addK = true;
//        }
        addK = truncateKiloValue(val);

        str = juce::String(val, (addK ? 2 : 0));

    } else {
        jassertfalse;
    }

    if( suffix.isNotEmpty() ) // appending the suffix after a space
    {
        str<<" ";
        if (addK) {
            str<<"k";
        }
        str<<suffix;
    }
    
    return str;
}

void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter *p)
{
    param = p;
    repaint();
}

//==============================================================================

juce::String RatioSlider::getDisplayString() const
{
    auto choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
    jassert(choiceParam != nullptr);
    
    auto currentChoice = choiceParam->getCurrentChoiceName();
    if(currentChoice.contains(".0"))
        currentChoice = currentChoice.substring(0, currentChoice.indexOf("."));
    currentChoice << ":1";
    
    return currentChoice;
}
