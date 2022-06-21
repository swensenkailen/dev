/**
 * @file   Filters.h
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-03-17
 * @brief  Collection of filter classes, ranging from 
 *
 * @note   Modified 2022-06-12
 */

#pragma once

#include <queue>

/**
 * @brief Parent class to all filter classes
 * 
 */
class Filter
{
public:

  // virtual filter operator
  virtual float operator()(float input) = 0;
  
private:
};

/**
 * @brief Simple lowpass filter
 * 
 */
class LowPass : public Filter
{
public:

  // ctor
  LowPass() : y1(0.0), g(0.0) { }

  // set coefficient
  void setCoefficient(double g_) { g = g_; }

  double getCoefficient() { return g; }

  // all dsp work done here
  float operator()(float x) override;

private:

  // variables to keep track of past variables, and current coefficient
  double y1, g;
};

/**
 * @brief Lowpass-comb filter w/ added ratio functionality, mainly used
 *        for Moorer reverb algorithm
 * 
 */
class LowPassComb : public Filter
{
public:

  // ctor
  LowPassComb() :g(0.0), R(0.0), L(0), y1(0.0) { }

  void setCoefficients(double R_, double g_) 
  { 
    ratio = R_;
    g = g_;
    R = ratio - (ratio * g);
  }

  void setDelay(int L_)
  { 
    isDirty = true;
    // if real time, do we need this to reinitialize the queues???
    L = L_; 
    
    std::queue<double> e1, e2, e3;
    std::swap(delayX1, e1);
    std::swap(delayX2, e2);
    std::swap(delayY2, e3);

    // delayX1 = new std::queue<double>()
  
    for (int i = 0; i < L; ++i)
    {
      delayX1.push(0.0f);
      delayX2.push(0.0f);
      delayY2.push(0.0f);
    }

    // one extra delay for (x_(t-L-1)
    delayX2.push(0.0f);

    if (isDirty)
      isDirty = false;
  }

  float operator()(float x) override;

  // low pass object (public to allow access to setters)
  LowPass lp;

  // delay value (in samples)
  int L;

  // dampening value + coefficient
  double R, g, ratio;

private:

  bool isDirty;
  
  // x/y delay queues
  std::queue<double> delayX1, delayX2;
  std::queue<double> delayY2;

  double y1;
};

/**
 * @brief 
 * 
 */
class AllPass : public Filter
{
public:
  
  // ctor
  AllPass() : m(0), a(0.0) { }

  void setCoefficient(double a_) { a = a_; }

  void setDelay(int m_) 
  { 
    isDirty = true;
    m = m_; 
    
    std::queue<double> e1, e2;
    std::swap(delayX, e1);
    std::swap(delayY, e2);

    for (int i = 0; i < m; ++i)
    {
      delayX.push(0.0f);
      delayY.push(0.0f);
    }

    if (isDirty)
      isDirty = false;
  }

  float operator()(float x) override;

  bool isDirty;

private:

  // delay value (in samples)
  int m;

  // coeff
  double a;

  // x/y delay queues
  std::queue<double> delayX;
  std::queue<double> delayY;
};