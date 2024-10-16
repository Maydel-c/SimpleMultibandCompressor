/*
  ==============================================================================

    Utilities.h
    Created: 16 Oct 2024 2:37:51pm
    Author:  Abhinav

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


template<
    typename Attachment,
    typename APVTS,
    typename Params,
    typename ParamName,
    typename SliderType
>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
                    APVTS& apvts,
                    const Params& params,
                    const ParamName& name,
                    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

template<
    typename APVTS,
    typename Params,
    typename Name
    >
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert( param != nullptr ); // when parameter not found
    
    return *param; // cause the calling function needs a reference, we will dereference it here
}

juce::String getValString(const juce::RangedAudioParameter& param, // getting value from parameters
                          bool getLow,
                          juce::String suffix);
                          
                          
template<typename T>
bool truncateKiloValue(T& value) // returns true if trucation performed
{
    if(value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }
    return false;
}

                          

template<
    typename Labels,
    typename ParamType,
    typename SuffixType
    >
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({0.f, getValString(param, true, suffix)}); // defined in LabelPos struct see above
    labels.add({1.f, getValString(param, false, suffix)});
}


void drawModuleBackground(juce::Graphics& g,
                          juce::Rectangle<int> bounds);
