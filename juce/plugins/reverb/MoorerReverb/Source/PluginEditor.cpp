/**
 * @file   PluginEditor.cpp
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  Contains all JUCE editor function declarations
 *
 * @note   This file contains base JUCE code. Modified 2022-06-12.
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define WINDOWX 800
#define WINDOWY 600

/**
 * @brief Changes processor parameter values if value was changed by user
 * 
 * @param state reference to processor
 * @param coeff value representing which slider to change
 * @param index index of which indexed value to change (if applicable)
 * @param value value to change
 */
void ReverbPlayerAudioProcessorEditor::sliderValueChanged(ReverbPlayerAudioProcessor& state, int coeff, int index, double value)
{
  // maintaining ratio between g and R
  // R = ratio - (ratio * g);
  // -> R + (ratio*g) = ratio -> 
  // g = (ratio - R) / ratio;

  switch (coeff)
  {
    // MIX
    case 0:
      state.verb.setMix(value/100.0);
      break;

    // R values
    case 1:
      state.verb.lp_combs[index].R = value;
      g_Vals[index]->setValue((state.verb.lp_combs[index].ratio - state.verb.lp_combs[index].R) / state.verb.lp_combs[index].ratio);
      break;

    // g values
    case 2:
      state.verb.lp_combs[index].g = value;
      R_Vals[index]->setValue(state.verb.lp_combs[index].ratio - (state.verb.lp_combs[index].ratio * state.verb.lp_combs[index].g));
      break;

    // L values
    case 3:
      state.verb.lp_combs[index].setDelay((value / 1000.0) * state.verb.rate);
      break;

    // ratio (R/1-g)
    case 4:
      state.verb.lp_combs[index].ratio = value;
      R_Vals[index]->setValue(state.verb.lp_combs[index].ratio - (state.verb.lp_combs[index].ratio * state.verb.lp_combs[index].g));
      g_Vals[index]->setValue((state.verb.lp_combs[index].ratio - state.verb.lp_combs[index].R) / state.verb.lp_combs[index].ratio);
      break;
    
    // a
    case 5:
      state.verb.ap.setCoefficient(value);
      break;

    // m
    case 6:
      state.verb.ap.setDelay((value / 1000.0) * state.verb.rate);
      break;

    default:
      break;
  }
}

/**
 * @brief Constructor + initializes all plugin UI 
 * 
 */
ReverbPlayerAudioProcessorEditor::ReverbPlayerAudioProcessorEditor (ReverbPlayerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
  // make all our sliders for mix, bypass and g, R, L vals + a and m for allpass
  addAndMakeVisible(sMix);
  sMix.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  sMix.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, WINDOWX / 20, WINDOWY / 30);
  sMix.setNormalisableRange(juce::NormalisableRange<double>(0, 100, 1));
  sMix.setValue(20);
  sMix.onValueChange = [this] { sliderValueChanged(audioProcessor, 0, 0, sMix.getValue()); };
  
  addAndMakeVisible(bVerb);
  bVerb.setButtonText("Bypass");
  bVerb.onClick = [this] { audioProcessor.verb.setBypass(); };

  addAndMakeVisible(a);
  addAndMakeVisible(m);
  a.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
  m.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
  a.setNormalisableRange(juce::NormalisableRange<double>(0.0, 1.0, 0.01));
  m.setNormalisableRange(juce::NormalisableRange<double>(0.0, 100.0, 1.0));
  a.setValue(0.7);
  m.setValue(6);
  a.onValueChange = [this] { sliderValueChanged(audioProcessor, 5, 0, a.getValue()); };
  m.onValueChange = [this] { sliderValueChanged(audioProcessor, 6, 0, m.getValue()); };

  for (int i = 0; i < 6; ++i)
  {
    g_Vals.push_back(std::make_unique<juce::Slider>());
    R_Vals.push_back(std::make_unique<juce::Slider>());
    L_Vals.push_back(std::make_unique<juce::Slider>());
    ratios.push_back(std::make_unique<juce::Slider>());

    addAndMakeVisible((*g_Vals[i]));
    addAndMakeVisible((*R_Vals[i]));
    addAndMakeVisible((*L_Vals[i]));
    addAndMakeVisible((*ratios[i]));

    g_Vals[i]->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    R_Vals[i]->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    L_Vals[i]->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    ratios[i]->setSliderStyle(juce::Slider::SliderStyle::Rotary);

    g_Vals[i]->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, WINDOWX / 24, WINDOWY / 32);
    R_Vals[i]->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, WINDOWX / 24, WINDOWY / 32);
    L_Vals[i]->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, WINDOWX / 24, WINDOWY / 32);
    ratios[i]->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, WINDOWX / 24, WINDOWY / 32);

    g_Vals[i]->setNormalisableRange(juce::NormalisableRange<double>(0.0, 1.0, 0.01));
    R_Vals[i]->setNormalisableRange(juce::NormalisableRange<double>(0.0, 1.0, 0.01));
    L_Vals[i]->setNormalisableRange(juce::NormalisableRange<double>(0, 100, 1));
    ratios[i]->setNormalisableRange(juce::NormalisableRange<double>(0.01, 1.0, 0.01));

    g_Vals[i]->setValue(audioProcessor.verb.lp_combs[i].g);
    R_Vals[i]->setValue(audioProcessor.verb.lp_combs[i].R);
    L_Vals[i]->setValue(audioProcessor.verb.lp_combs[i].L / 48);
    ratios[i]->setValue(0.83);

    R_Vals[i]->onValueChange = [this, i] { sliderValueChanged(audioProcessor, 1, i, R_Vals[i]->getValue()); };
    g_Vals[i]->onValueChange = [this, i] { sliderValueChanged(audioProcessor, 2, i, g_Vals[i]->getValue()); };
    L_Vals[i]->onValueChange = [this, i] { sliderValueChanged(audioProcessor, 3, i, L_Vals[i]->getValue()); };
    ratios[i]->onValueChange = [this, i] { sliderValueChanged(audioProcessor, 4, i, ratios[i]->getValue()); };
  }

  addAndMakeVisible(audioProcessor.getViz1());
  addAndMakeVisible(audioProcessor.getViz2());

  setSize(WINDOWX, WINDOWY);
}

/**
 * @brief Destructor
 * 
 */
ReverbPlayerAudioProcessorEditor::~ReverbPlayerAudioProcessorEditor()
{
}

/**
 * @brief Paints the scene each frame
 * 
 * @param g reference to graphics
 */
void ReverbPlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
  // background
  g.fillAll (juce::Colours::darkslateblue);

  // text
  g.setColour (juce::Colours::white);
  g.setFont (17.0f);

  g.drawText("Moorer Reverb", getWidth() / 2 - getWidth() / 16, getHeight() / 5 - getHeight() / 30, getWidth() / 25 * 4, getHeight() / 13, juce::Justification::centred, false);
  g.drawText("Input", getWidth() / 4 - getWidth() / 32, getHeight() / 20, getWidth() / 3, getHeight() / 4, juce::Justification::topLeft, false);
  g.drawText("Output", getWidth() / 2, getHeight() / 20, getWidth() / 3, getHeight() / 4, juce::Justification::topRight, false);

  g.drawText("R", getWidth() / 16, getHeight() / 8 * 3 - getHeight() / 32, getWidth() / 6, getHeight() / 6, juce::Justification::left, false);
  g.drawText("g", getWidth() / 16, getHeight() / 8 * 4 - getHeight() / 32, getWidth() / 6, getHeight() / 6, juce::Justification::left, false);
  g.drawText("L", getWidth() / 16, getHeight() / 8 * 5 - getHeight() / 32, getWidth() / 6, getHeight() / 6, juce::Justification::left, false);
  g.drawText("R / 1 - g", getWidth() / 16 - getWidth() / 32, getHeight() / 8 * 6 - getHeight() / 32, getWidth() / 6, getHeight() / 6, juce::Justification::left, false);

  g.drawText("Allpass coefficients a/m:", getWidth() / 10, getHeight() - getHeight() / 16, getWidth() / 3, getWidth() / 32, juce::Justification::left, false);

  g.drawText("M", getWidth() / 16, getHeight() / 17 * 8 + getHeight() / 50, getWidth() - getWidth() / 10 , getHeight() / 6, juce::Justification::right, false);
  g.drawText("I", getWidth() / 16, getHeight() / 17 * 9 + getHeight() / 50, getWidth() - getWidth() / 10, getHeight() / 6, juce::Justification::right, false);
  g.drawText("X", getWidth() / 16, getHeight() / 17 * 10 + getHeight() / 50, getWidth() - getWidth() / 10, getHeight() / 6, juce::Justification::right, false);

  // Shapes
  g.setColour(juce::Colours::mediumpurple);

  g.drawArrow(juce::Line<float>(juce::Point<float>(getWidth() / 32 * 5, getHeight() / 5), juce::Point<float>(getWidth() / 16 * 7, getHeight() / 5)), 3.0, 10.0, 10.0);
  g.drawArrow(juce::Line<float>(juce::Point<float>(getWidth() / 32 * 19, getHeight() / 5), juce::Point<float>(getWidth() / 18 * 12, getHeight() / 5)), 3.0, 10.0, 10.0);

  g.drawRect(getWidth()/2 - getWidth()/16, getHeight()/5 - getHeight() / 30, getWidth()/25 * 4, getHeight()/13, getWidth()/200);
  g.drawRect(getWidth() / 8 - 4, getHeight() / 10 - 4, getWidth() / 4 + 8, getHeight() / 5 + 8, 4.0);
  g.drawRect(getWidth() / 9 * 6 - 4, getHeight() / 10 - 4, getWidth() / 4 + 8, getHeight() / 5 + 8, 4.0);

  for (int i = 0; i < 6; ++i)
  {
    g.setColour(juce::Colours::mediumpurple);

    g.drawEllipse(2.1 * getWidth() / 16 + (i * getWidth() / 8), getHeight() / 32 * 14, getWidth() / 32, getHeight() / 32, 18.0);
    g.drawEllipse(2.1 * getWidth() / 16 + (i * getWidth() / 8), getHeight() / 32 * 18, getWidth() / 32, getHeight() / 32, 18.0);
    g.drawEllipse(2.1 * getWidth() / 16 + (i * getWidth() / 8), getHeight() / 36 * 25, getWidth() / 32, getHeight() / 32, 18.0);
    g.drawEllipse(2.1 * getWidth() / 16 + (i * getWidth() / 8), getHeight() / 42 * 34, getWidth() / 32, getHeight() / 32, 18.0);
  }
}

/**
 * @brief Sets bounds for all scene objects
 * 
 */
void ReverbPlayerAudioProcessorEditor::resized()
{
  for (int i = 0; i < 6; ++i)
  {
    R_Vals[i]->setBounds(getWidth() / 16 + (i * getWidth() / 8), getHeight() / 8 * 3, getWidth() / 6, getHeight() / 7);
    g_Vals[i]->setBounds(getWidth() / 16 + (i * getWidth() / 8), getHeight() / 8 * 4, getWidth() / 6, getHeight() / 7);
    L_Vals[i]->setBounds(getWidth() / 16 + (i * getWidth() / 8), getHeight() / 8 * 5, getWidth() / 6, getHeight() / 7);
    ratios[i]->setBounds(getWidth() / 16 + (i * getWidth() / 8), getHeight() / 8 * 6, getWidth() / 6, getHeight() / 7);
  }

  bVerb.setBounds(getWidth() - getWidth() / 8, getHeight() - getHeight() / 8, 100, 30);
  sMix.setBounds(getWidth() - getWidth() / 8, getHeight()/2 - getHeight() / 10, 50, getHeight() / 2 - getHeight() / 20);

  a.setBounds(getWidth() / 8 * 3, getHeight() - getHeight() / 16, getWidth() / 6, getWidth() / 32);
  m.setBounds(getWidth() / 8 * 5, getHeight() - getHeight() / 16, getWidth() / 6, getWidth() / 32);

  audioProcessor.getViz1().setBounds(getWidth() / 8, getHeight() / 10, getWidth() / 4, getHeight() / 5);
  audioProcessor.getViz2().setBounds(getWidth() / 9 * 6, getHeight() / 10, getWidth() / 4, getHeight() / 5);
  audioProcessor.getViz1().setAlwaysOnTop(true);
  audioProcessor.getViz2().setAlwaysOnTop(true);
}
