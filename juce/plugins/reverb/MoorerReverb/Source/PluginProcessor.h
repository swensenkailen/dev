/**
 * @file   PluginProcessor.h
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  Contains JUCE processor class and everything pertaining to it
 *
 * @note   This file contains base JUCE code. Modified 2022-06-12.
 */

#pragma once

#include <JuceHeader.h>
#include "../../MoorerReverb.h" 
#include <string>
#include <iostream>

/**
 * @brief Wrapper class for juce::AudioVisualiserComponent
 *        w/ defaulted paramteters
 * 
 */
class Visualizer : public juce::AudioVisualiserComponent
{
public:

  // constructor, instantiate to 2 channels 
  Visualizer() : AudioVisualiserComponent(2)
  {
    // messed with these til they felt good
    setBufferSize(512);
    setSamplesPerBlock(96);

    // set default colors
    juce::AudioVisualiserComponent::setColours(juce::Colours::black, juce::Colours::mediumpurple);
  }

  Visualizer(juce::Colour c1, juce::Colour c2) : AudioVisualiserComponent(2)
  {
    juce::AudioVisualiserComponent::setColours(c1, c2);
  }
};

/**
 * @brief Processor class
 * 
 */
class ReverbPlayerAudioProcessor  : public juce::AudioProcessor
{
public:

  ReverbPlayerAudioProcessor();
  ~ReverbPlayerAudioProcessor() override;

  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  #ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
  #endif
    
  juce::AudioProcessorValueTreeState& getState();

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  // getters for both visualizers
  Visualizer& getViz1()
  {
    return Viz1;
  }

  Visualizer& getViz2()
  {
    return Viz2;
  }

  // Moorer reverb object
  MoorerReverb verb;

private:

  juce::AudioProcessorValueTreeState* pState;

  Visualizer Viz1, Viz2;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbPlayerAudioProcessor)
};
