/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

/*
 GUI Roadmap:
 1. Global controls (x-over sliders, gain sliders) DONE
 2. Main band controls (attack, release, threshold, ratio) - below global controls - assigned to midband by default DONE
 3. Add solo/mute/bypass buttons - assigned to midband by default - right side of main band controls DONE
 4. Band select functionality (Resettable parameter attachments) - assigning same set of controls for every band DONE
 5. Band select buttons reflect the solo/mute/bypass state. Logic that prevents solo and mute to be on at same time
 6. Custom LookAndFeel for the rotary sliders and toggle buttons
 7. Spectrum analyzer overview - different from simpleEq
 8. Data structures for spectrum analyzer. Grab code from SimpleEq
 9. Fifo usage in pluginProcessor::processBlock. DSP modification
 10. Implementing the analyzer pre-computed paths - Code grab from SimpleEq
 11. Drawing crossovers on top of the analyzer plot. Here we customize spectrum analyzer
 12. Drawing gain reduction on top of analyzer. Will need to work little bit in DSP for this to work
 13. Analyzer bypass. Copy code from SimpleEq
 14. Global bypass 
*/

#include <JuceHeader.h>
#include "DSP/CompressorBand.h"
#include "DSP/Params.h"

//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();
    
    APVTS apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:
    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];
    
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    //      fc0     fc1
    Filter  LP1,    AP2,
            HP1,    LP2,
                    HP2;
    
//    Filter invAP1, invAP2;
//    juce::AudioBuffer<float> invAPBuffer; // only one buffer needed because both AP filters with different cutoffs will be in series
    
    juce::AudioParameterFloat* lowMidCrossover {nullptr};
    juce::AudioParameterFloat* midHighCrossover {nullptr};
    
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};
    
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float> (buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float> (block);
        gain.process(ctx);
    }
    
    void updateState();
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
