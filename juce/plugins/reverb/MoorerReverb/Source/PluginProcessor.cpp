/**
 * @file   PluginProcessor.cpp
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  
 *
 * @note   This file contains base JUCE code. Modified 2022-06-12.
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

/**
 * @brief Constructor + initializes parameters
 * 
 */
ReverbPlayerAudioProcessor::ReverbPlayerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
  // initialize state
  pState = new juce::AudioProcessorValueTreeState(*this, nullptr);

  // some default initialization (values proposed by moorer)
  verb.setRate(48000);
  verb.setMix(0.2);
  verb.initializeFilters();
  
  // add all our parameters to value tree
  pState->createAndAddParameter("mix", "Mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), verb.getMix(), nullptr, nullptr);

  for (int i = 0; i < 6; ++i)
  {
    std::string s1 = std::string("g" + std::to_string(i));
    std::string s2 = std::string("G" + std::to_string(i));
    std::string s3 = std::string("r" + std::to_string(i));
    std::string s4 = std::string("R" + std::to_string(i));
    std::string s5 = std::string("l" + std::to_string(i));
    std::string s6 = std::string("L" + std::to_string(i));

    pState->createAndAddParameter(s1, s2, s2, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), verb.lp_combs[i].g, nullptr, nullptr);
    pState->createAndAddParameter(s3, s4, s4, juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), verb.lp_combs[i].R, nullptr, nullptr);
    pState->createAndAddParameter(s5, s6, s6, juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), verb.lp_combs[i].L / 48 , nullptr, nullptr);
  }

  pState->state = juce::ValueTree("mix");

  for (int i = 0; i < 6; ++i)
  {
    std::string s1 = std::string("g" + std::to_string(i));
    std::string s2 = std::string("G" + std::to_string(i));
    std::string s3 = std::string("r" + std::to_string(i));
    std::string s4 = std::string("R" + std::to_string(i));
    std::string s5 = std::string("l" + std::to_string(i));
    std::string s6 = std::string("L" + std::to_string(i));

    pState->state = juce::ValueTree(juce::Identifier(s1));
    pState->state = juce::ValueTree(juce::Identifier(s3));
    pState->state = juce::ValueTree(juce::Identifier(s5));
  }
}

/**
 * @brief Destructor
 * 
 */
ReverbPlayerAudioProcessor::~ReverbPlayerAudioProcessor()
{
  pAudioFormatReader = nullptr;
}

/**
 * @brief Gets plugin name
 * 
 */
const juce::String ReverbPlayerAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

/**
 * @brief Returns true if plugin accepts midi signals
 * 
 */
bool ReverbPlayerAudioProcessor::acceptsMidi() const
{
  #if JucePlugin_WantsMidiInput
  return true;
  #else
  return false;
  #endif
}

/**
 * @brief Returns true if plugin produces midi signals
 * 
 */
bool ReverbPlayerAudioProcessor::producesMidi() const
{
  #if JucePlugin_ProducesMidiOutput
  return true;
  #else
  return false;
  #endif
}

/**
 * @brief Returns true if plugin is midi effect
 * 
 */
bool ReverbPlayerAudioProcessor::isMidiEffect() const
{
  #if JucePlugin_IsMidiEffect
  return true;
  #else
  return false;
  #endif
}

/**
 * @brief Prepares plugin for use
 * 
 * @param sampleRate      Sampling rate
 * @param samplesPerBlock # of samples per block
 */
void ReverbPlayerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  Viz1.setNumChannels(1);
  Viz2.setNumChannels(1);
}

/**
 * @brief Releases resources where needed
 * 
 */
void ReverbPlayerAudioProcessor::releaseResources()
{
  Viz1.clear();
  Viz2.clear();
}

// #ifndef JucePlugin_PreferredChannelConfigurations

/**
 * @brief Returns true if buses layout is supported
 * 
 * @param layouts reference to layouts
 * @return true 
 * @return false 
 */
bool ReverbPlayerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  const juce::AudioChannelSet& input = layouts.getMainInputChannelSet();
  const juce::AudioChannelSet& output = layouts.getMainOutputChannelSet();

  return input.size() == 1 && output.size() == 2;
}

/**
 * @brief Gets pointer to state
 * 
 */
juce::AudioProcessorValueTreeState& ReverbPlayerAudioProcessor::getState()
{
  return *pState;
}

/**
 * @brief Processes audio input using Moorer Reverb class
 * 
 */
void ReverbPlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels  = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  
  // stereo
  buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());

  // push buffer to visualizer before affected
  Viz1.pushBuffer(buffer);

  // ********************* //
    
  float* l = buffer.getWritePointer(0);
  float* r = buffer.getWritePointer(1);
 
  // for each sample
  for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
  {
    // store clean data
    float clean = *l;
        
    /** Reverb calculations  **/
    *l = verb(*l);
    *r = *l;

    l++; r++;
  }

  
  // push buffer to visualizer after affected
  Viz2.pushBuffer(buffer);
}

/**
 * @brief Returns true if plugin has editor
 * 
 */
bool ReverbPlayerAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

/**
 * @brief Creates editor
 * 
 */
juce::AudioProcessorEditor* ReverbPlayerAudioProcessor::createEditor()
{
  return new ReverbPlayerAudioProcessorEditor (*this);
}

/**
 * @brief creates new instances of the plugin
 * 
 */
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new ReverbPlayerAudioProcessor();
}

/**
 * @brief Changes program name
 * 
 * @param index   program index
 * @param newName new name
 */
void ReverbPlayerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

/**
 * @brief Gets tail length in seconds
 * 
 * @return double 
 */
double ReverbPlayerAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

/**
 * @brief Gets number of programs
 * 
 * @return int 
 */
int ReverbPlayerAudioProcessor::getNumPrograms()
{
  return 1;  
}

/**
 * @brief Gets current program
 * 
 * @return int 
 */
int ReverbPlayerAudioProcessor::getCurrentProgram()
{
  return 0;
}

/**
 * @brief Sets current program
 * 
 * @param index index of program
 */
void ReverbPlayerAudioProcessor::setCurrentProgram (int index)
{
}

/**
 * @brief Gets program name
 * 
 * @param index index of program
 * @return const juce::String 
 */
const juce::String ReverbPlayerAudioProcessor::getProgramName (int index)
{
  return {};
}

/**
 * @brief Gets state information
 * 
 * @param destData Reference to destination data
 */
void ReverbPlayerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

/**
 * @brief Sets state information
 * 
 * @param data pointer to data
 * @param sizeInBytes size of data
 */
void ReverbPlayerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}