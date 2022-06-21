/**
 * @file   PluginEditor.h
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  Contains JUCE editor class and everything pertaining to it
 *
 * @note   This file contains base JUCE code. Modified 2022-06-12.
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * @brief Editor class
 * 
 */
class ReverbPlayerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
  ReverbPlayerAudioProcessorEditor (ReverbPlayerAudioProcessor&);
  ~ReverbPlayerAudioProcessorEditor() override;

  void paint (juce::Graphics&) override;
  void resized() override;
    
  // ui -> parameter communication
  // coeff is param; mix, R, g, L values are 0,1,...
  void sliderValueChanged(ReverbPlayerAudioProcessor& state, int coeff, int index, double value);

private:

  // all our sliders and buttons
  juce::Slider sMix { "Mix" };

  juce::Slider a, m;

  juce::ToggleButton bVerb;

  // parameter values
  std::vector<std::unique_ptr<juce::Slider>> R_Vals;
  std::vector<std::unique_ptr<juce::Slider>> g_Vals;
  std::vector<std::unique_ptr<juce::Slider>> L_Vals;
  std::vector<std::unique_ptr<juce::Slider>> ratios;

  // reference to audio processor
  ReverbPlayerAudioProcessor& audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbPlayerAudioProcessorEditor)
};
