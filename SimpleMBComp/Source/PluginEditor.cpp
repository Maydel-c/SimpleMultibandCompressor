/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    auto enabled = slider.isEnabled();
    
    // creating the bg for slider
    g.setColour(enabled ? Colour(97u, 18u, 167u) : Colours::darkgrey);
    g.fillEllipse(bounds);
    
    // creating the border for slider
    g.setColour(enabled ? Colour(255u, 154u, 1u) : Colours::grey);
    g.drawEllipse(bounds, 1.f);
    
    
    if(auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        // drawing the marker inside the slider and rotating it
        auto center = bounds.getCentre();
        
        Path p; // needed to make anything move
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5); // inner rectangle marker not starting from exact center. Saving space for text
        
        p.addRoundedRectangle(r, 2.f); // change from addRectangle to addRoundedRectangle
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        // rotating the narrow rectangle acc to the above radian angle - sliderAngRad
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        g.setFont(rswl->getTextHeight()); // uses the default and gives height
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        // changing the same rectangle now to use with text
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        
        // setting the center of this rectangle to the center of bounding box
        r.setCentre(bounds.getCentre());
        
        // making the text background black
        g.setColour(enabled ? Colours::black : Colours::darkgrey);
        g.fillRect(r);
        
        // adding text making the text white
        g.setColour(enabled ? Colours::white : Colours::lightgrey);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1); // accepts Rectangle<int> as 2nd argument
        
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    
    if( auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;
        
        auto bounds = toggleButton.getLocalBounds();
        
    // see the bounding box region for bypass buttons
    //    g.setColour(Colours::red);
    //    g.drawRect(bounds);
        
        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6; // JUCE_LIVE_CONSTANT(6);
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
        
        float ang = 30.f; // JUCE_LIVE_CONSTANT(30);
        
        size -= 6; // JUCE_LIVE_CONSTANT(6);
        
        powerButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY(),
                                  size * 0.5f,
                                  size * 0.5f,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360.f - ang),
                                  true);
        
        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());
        
        PathStrokeType pst(2, PathStrokeType::JointStyle::curved);
        
        auto color = toggleButton.getToggleState() ? juce::Colours::dimgrey : juce::Colour(0u, 172u, 1u);
        g.setColour(color);
        
        g.strokePath(powerButton, pst);
        
        // draw circle around the whole thing
        g.drawEllipse(r, 2);
    }
    else if( auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = ! toggleButton.getToggleState() ? juce::Colours::dimgrey : juce::Colour(0u, 172u, 1u);
        
        g.setColour(color);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
//        auto insetRect = bounds.reduced(4); // container for random path representing a waveform icon
//
//        Path randomPath;
//
//        Random r;
//
//        randomPath.startNewSubPath(insetRect.getX(), insetRect.getY() + insetRect.getHeight() * r.nextFloat());
//
//        for ( auto x = insetRect.getX() + 1; x < insetRect.getRight(); x+=2) {
//            randomPath.lineTo(x, insetRect.getY() + insetRect.getHeight() * r.nextFloat());
//        }
        
        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);
        auto buttonIsOn = toggleButton.getToggleState();
        const int cornerSize = 4;
        
        g.setColour(buttonIsOn ? juce::Colours::white : juce::Colours::black);
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
        
        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colours::white);
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);
        
        g.drawFittedText(toggleButton.getName(), bounds, Justification::centred, 1);
        
    }
}

//==============================================================================
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

//==============================================================================

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}
//==============================================================================

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
apvts(apv),
attackSlider(nullptr, "ms", "ATTACK"),
releaseSlider(nullptr, "ms", "RELEASE"),
thresholdSlider(nullptr, "dB", "THRESH"),
ratioSlider(nullptr, ""/*, "RATIO"*/) // we are getting the 3rd parameter in this from RatioSlider class
{
    
//    using namespace Params;
//    const auto& params = GetParams();
//    BELOW CODE NOW RESIDES IN updateAttachments()
//    auto getParamHelper = [&params, &apvts = this->apvts](const auto& name) -> auto&
//    {
//        return getParam(apvts, params, name);
//    };
//
//    // unlike GC, here we need reference because the function returns a reference. so '&' is added
//    attackSlider.changeParam(&getParamHelper(Names::Attack_Mid_Band));
//    releaseSlider.changeParam(&getParamHelper(Names::Release_Mid_Band));
//    thresholdSlider.changeParam(&getParamHelper(Names::Threshold_Mid_Band));
//    ratioSlider.changeParam(&getParamHelper(Names::Ratio_Mid_Band));
//
//    addLabelPairs(attackSlider.labels, getParamHelper(Names::Attack_Mid_Band), "ms");
//    addLabelPairs(releaseSlider.labels, getParamHelper(Names::Release_Mid_Band), "ms");
//    addLabelPairs(thresholdSlider.labels, getParamHelper(Names::Threshold_Mid_Band), "dB");
//
//    ratioSlider.labels.clear();
//    ratioSlider.labels.add({0.f, "1:1"});
//    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&getParamHelper(Names::Ratio_Mid_Band));
////    ratioSlider.labels.add({1.f, ratioParam->choices.getReference(ratioParam->choices.size() - 1)}); // getting the last element
//
//    // getting the last element and displaying it in required format "x:1"
//    ratioSlider.labels.add({1.f,
//        juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1" });
//
//
//    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment,
//                                                  const auto& name,
//                                                  auto& slider)
//    {
//        makeAttachment(attachment, apvts, params, name, slider);
//    };
//
//    makeAttachmentHelper(attackSliderAttachment, Names::Attack_Mid_Band, attackSlider);
//    makeAttachmentHelper(releaseSliderAttachment, Names::Release_Mid_Band, releaseSlider);
//    makeAttachmentHelper(thresholdSliderAttachment, Names::Threshold_Mid_Band, thresholdSlider);
//    makeAttachmentHelper(ratioSliderAttachment, Names::Ratio_Mid_Band, ratioSlider);
    
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    
    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");
    
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);
    
//    makeAttachmentHelper(bypassButtonAttachment, Names::Bypass_Mid_Band, bypassButton);
//    makeAttachmentHelper(soloButtonAttachment, Names::Solo_Mid_Band, soloButton);
//    makeAttachmentHelper(muteButtonAttachment, Names::Mute_Mid_Band, muteButton);
    
    lowBand.setName("Low");
    midBand.setName("Mid");
    highBand.setName("High");
    
    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);
    
    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
        if( auto* c = safePtr.getComponent())
        {
            c->updateAttachments();
        }
    };
    
    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;
    
    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    updateAttachments();
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
}

void CompressorBandControls::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(5);
    
    auto createBandButtonControlBox = [](std::vector<Component*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;
        
        auto spacer = FlexItem().withHeight(2);
        
        for (auto* comp : comps) {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.f));
        }
        flexBox.items.add(spacer);
        
        return flexBox;
    };
    
    auto bandButtonControlBox = createBandButtonControlBox({&bypassButton, &soloButton, &muteButton});
    auto bandSelectControlBox = createBandButtonControlBox({&lowBand, &midBand, &highBand});
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4); // between each item
//    auto endCap = FlexItem().withWidth(6); // space at ends of array
//    flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(50));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
//    flexBox.items.add(endCap);
    flexBox.items.add(spacer);

    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));
    
    flexBox.performLayout(bounds);
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

void CompressorBandControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void CompressorBandControls::updateAttachments()
{
    enum BandType
    {
        Low,
        Mid,
        High
    };
    
    BandType bandType = [this]()
    {
        if(lowBand.getToggleState())
            return BandType::Low;
        if (midBand.getToggleState())
            return BandType::Mid;
        else
            return BandType::High;
    }();
 
    using namespace Params;
    std::vector<Names> names;
    
    switch (bandType) {
        
        case Low:
        {
            names = std::vector<Names>
            {
                Names::Attack_Low_Band,
                Names::Release_Low_Band,
                Names::Threshold_Low_Band,
                Names::Ratio_Low_Band,
                Names::Solo_Low_Band,
                Names::Mute_Low_Band,
                Names::Bypass_Low_Band
            };
            break;
        }
        case Mid:
        {
            names = std::vector<Names>
            {
                Names::Attack_Mid_Band,
                Names::Release_Mid_Band,
                Names::Threshold_Mid_Band,
                Names::Ratio_Mid_Band,
                Names::Solo_Mid_Band,
                Names::Mute_Mid_Band,
                Names::Bypass_Mid_Band
            };
            break;
        }
        case High:
        {
            names = std::vector<Names>
            {
                Names::Attack_High_Band,
                Names::Release_High_Band,
                Names::Threshold_High_Band,
                Names::Ratio_High_Band,
                Names::Solo_High_Band,
                Names::Mute_High_Band,
                Names::Bypass_High_Band
            };
            break;
        }
    }
    
    enum Pos
    {
        Attack,
        Release,
        Threshold,
        Ratio,
        Mute,
        Solo,
        Bypass
    };
    
    const auto& params = GetParams();
    
    auto getParamHelper = [&params, &apvts = this->apvts, &names](const auto& pos) -> auto&
    {
        return getParam(apvts, params, names.at(pos));
    };
    
    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    bypassButtonAttachment.reset();
    soloButtonAttachment.reset();
    muteButtonAttachment.reset();
    
    auto& attackParam = getParamHelper(Pos::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);
    
    auto& releaseParam = getParamHelper(Pos::Release);
    addLabelPairs(releaseSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);
    
    auto& thresholdParam = getParamHelper(Pos::Threshold);
    addLabelPairs(thresholdSlider.labels, thresholdParam, "dB");
    thresholdSlider.changeParam(&thresholdParam);
    
    auto& ratioParamRap = getParamHelper(Pos::Ratio);
    ratioSlider.labels.clear();
    ratioSlider.labels.add({0.f, "1:1"});
    auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&ratioParamRap);
    ratioSlider.labels.add({1.f,
        juce::String(ratioParam->choices.getReference(ratioParam->choices.size() - 1).getIntValue()) + ":1" });
    ratioSlider.changeParam(ratioParam);
    
    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
    makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
    makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
    makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);
    
}

//==============================================================================
GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };
    
    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& lowMidParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);
    
    inGainSlider = std::make_unique<RSWL>(&gainInParam,
                                          "dB",
                                          "INPUT TRIM");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam,
                                               "Hz",
                                               "LOW-MID X-OVER");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam,
                                                "Hz",
                                                "MID-HI X-OVER");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam,
                                           "dB",
                                           "OUTPUT TRIM");
    
    auto makeAttachmentHelper = [&params, &apvts](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    makeAttachmentHelper(inGainSliderAttachment, Names::Gain_In, *inGainSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment, Names::Low_Mid_Crossover_Freq, *lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment, Names::Mid_High_Crossover_Freq, *midHighXoverSlider);
    makeAttachmentHelper(outGainSliderAttachment, Names::Gain_Out, *outGainSlider);
    
    addLabelPairs(inGainSlider->labels,
                  gainInParam,
                  "dB");
    addLabelPairs(lowMidXoverSlider->labels,
                  lowMidParam,
                  "Hz");
    addLabelPairs(midHighXoverSlider->labels,
                  midHighParam,
                  "Hz");
    addLabelPairs(outGainSlider->labels,
                  gainOutParam,
                  "dB");
    
    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
}

void GlobalControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(5);
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4); // between each item
    auto endCap = FlexItem().withWidth(6); // space at ends of array
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);
    
    flexBox.performLayout(bounds);
}

//==============================================================================
SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    setLookAndFeel(&lnf);
//    addAndMakeVisible(controlBar);
//    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);
    
    setSize (650, 540);
}

SimpleMBCompAudioProcessorEditor::~SimpleMBCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleMBCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void SimpleMBCompAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    controlBar.setBounds(bounds.removeFromTop(32));
    bandControls.setBounds(bounds.removeFromBottom(135));
    analyzer.setBounds(bounds.removeFromTop(255));
    globalControls.setBounds(bounds);
}

