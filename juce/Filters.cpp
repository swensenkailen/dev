/**
 * @file   Filters.cpp
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  Implementation of all filter class operators
 *
 * @note   Modified 2022-06-12
 */

#include "Filters.h"

/**
 * @brief Lowpass filter operator
 * 
 *   X                 Y
 *   ---> | low pass | --->
 *
 *
 *   H(z) = 1 / (1 - gz^-1)
 *   ==>
 *   yt = x + gy_(t-1)
 * 
 * @param x input
 * 
 * @return float 
 */
float LowPass::operator()(float x)
{
  // perform lowpass operation here
  float y = (double)x + g * y1;

  // update past y variable
  y1 = y;

  // return modified signal
  return y;
}

/**
 * @brief Lowpass filter operator
 * 
 *   X                               Y
 *  ---> [SUM] ---> | z^-L | ---> | --->
 *         |                      |
 *         | *R  (dampening val)  |
 *         |                      |
 *        <----- |low pass| <-----
 *  
 *  
 *  H(z) = (z^-L) / (1 - R[1 / (1 - gz^-1)]z^-L)
 *  
 *         * (1-gz^-1) [to cancel out LP]
 *  ==>
 *  
 *    yt = x_(t-L) + R*[LP(x)]*y_(t-L) <-- w/ LP
 *  
 *    yt = x_(t-L) - gx_(t-L-1) + gy_(t-1) + Ry_(t-L) <-- w/o LP
 * 
 * @param x input
 * 
 * @return float 
 */
float LowPassComb::operator()(float x)
{
  double y = 0;

  if (!isDirty)
  {
    if (!delayX1.empty() && !delayX2.empty() && !delayY2.empty())
    {
      // filter function 
      y = delayX1.front() - (g * delayX2.front()) 
                 + (g * y1) + (R * delayY2.front());
  
      // pop off front of queues
      delayX1.pop();
      delayX2.pop();
      delayY2.pop();

      // push new x/y into back of queues
      delayX1.push((double)x);
      delayX2.push((double)x);
      delayY2.push(y);

      // set single past y variable
      y1 = y;

    }
  }

  return y;
}

/**
 * @brief All pass filter with modifiable delay value (m)
 *
 *
 *   X                 Y
 *  ---> | all pass | --->
 *
 *
 *  H(z) = (z^-m + a) / (1 + az^-m)
 *
 *  ==>
 *
 *    yt = x_(t-m) + ax - ay_(t-m)
 *    
 *    yt = a*(x - y_(t-m)) + x_(t-m) 
 *    
 * @param x 
 * 
 * @return float 
 */
float AllPass::operator()(float x)
{
  double y = 0;
  
  if (!isDirty)
  {
    if (!delayX.empty() && !delayY.empty())
    {
      y = a * ((double)x - delayY.front()) + delayX.front();

      delayX.pop();
      delayY.pop();

      delayX.push((double)x);
      delayY.push(y);
    }
  }

  return y;
}
