// TODO: finish commenting


/**
 * @file   MoorerReverb.cpp
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  
 *
 * @note   Modified 2022-06-12
 */

#include "MoorerReverb.h"

/**
 * @brief 
 * 
 * @param x 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @return double 
 */
static double lerpBetweenPlots(double x, double x1, double y1, double x2, double y2)
{
  return y1 + ((x - x1) / (x2 - x1)) * (y2 - y1);
}

/**
 * @brief This initializes the filter to the recommended parameter values
 *        by James A. Moorer, originally noted in his publication,
 *        "About This Reverberation Business." g values are interpolated
 *        to fit values at 44.1khz sampling rate.
 * 
 */
void MoorerReverb::initializeFilters()
{
  // l values
    // suggested is 50, 56, 61, 68, 72 and 78 ms (* 0.001 to get sec)
    // * rate 
    // rounded to nearest whole number
  int lVals[6] = { std::round(0.050 * (double)rate), std::round(0.056 * (double)rate), std::round(0.061 * (double)rate),
                   std::round(0.068 * (double)rate), std::round(0.072 * (double)rate), std::round(0.078 * (double)rate) };
  
  // g values
    // { 0.24, 0.26, 0.28, 0.29, 0.30, 0.32 } <-- recommended vals for 25khz
    // { 0.46, 0.48, 0.50, 0.52, 0.53, 0.55 ) <-- 50khz
    // lerp between sample rates
  double gVals[6] = {lerpBetweenPlots((double)rate, 25000.0, 0.24, 50000.0, 0.46), lerpBetweenPlots(rate, 25000.0, 0.26, 50000.0, 0.48),
                     lerpBetweenPlots((double)rate, 25000.0, 0.28, 50000.0, 0.50), lerpBetweenPlots(rate, 25000.0, 0.29, 50000.0, 0.52),
                     lerpBetweenPlots((double)rate, 25000.0, 0.30, 50000.0, 0.53), lerpBetweenPlots(rate, 25000.0, 0.32, 50000.0, 0.55)};

  // set all comb filter coefficient and their LPs respective values
  for (int i = 0; i < numCombs; ++i)
  {
    // set all lp comb coefficients and delays
    lp_combs[i].setCoefficients(0.83, gVals[i]);
    lp_combs[i].setDelay(lVals[i]);
  }

  // set allpass coefficient/delay
  ap.setCoefficient(0.7);
  // 6ms = .006 sec
  ap.setDelay(std::round(0.006 * (double)rate));
  
  // wet to dry ratio
  setMix(wet);
}

/**
 * @brief Moorer reverb using a combination of filtered and clean signal
 * 
 *         / --> | low comb | --> \       (6 low pass comb filters)
 *   X    / --->     ....     ---> \                                  Y
 *  ---> / ----> | "      " | --> [SUM] ---> | all pass | ---> [SUM] --->
 *        |                                                       |
 *        |                    (clean signal)                     |
 *        -------------------------> *K ------------------------->| 
 * 
 * @param x input
 * 
 * @return float 
 */
float MoorerReverb::operator()(float x)
{
  // if bypass isn't set, process data
  if (isActive)
  {
    double y = 0.0f;

    for (int i = 0; i < numCombs; i++)
    {
      // sum all comb filter outputs 
      y += lp_combs[i]((double)x);
    }
  
    // apply allpass, and add the clean value
    return ((dry * x) + (wet * ap(y)));
  }
  else
     return x;
}
