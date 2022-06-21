/**
 * @file   MoorerReverb.h
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  
 *
 * @note   Modified 2022-06-12
 */

#pragma once

#include "Filters.h"
#include <vector>
#include <cmath>

/**
 * @brief Moorer reverb class
 * 
 */
class MoorerReverb : public Filter
{
public:

  MoorerReverb() = default;
  MoorerReverb(int samplingRate, double mix_) : rate(samplingRate), isActive(true), wet(mix_) { initializeFilters(); }

  void initializeFilters();
  
  void setRate(int sr) { rate = sr; }
  void setMix(double wet_) { wet = wet_; dry = 1.0 - wet_; }
  double getMix() { return wet; }

  void setBypass() 
  {
    isActive = isActive ? false : true;
  }

  float operator()(float x) override;

  // filter objects (public to allow access to setters)
  LowPassComb lp_combs[6];
  AllPass ap;
  
  // sampling rate
  int rate;

private:

  // bool to control bypass of reverb effect
  bool isActive;
  
  // if parameters have recently been updated
  //bool isDirty;

  // number of comb filters needed
  const int numCombs = 6;

  // our wet/dry values
  double wet, dry;
}; 

