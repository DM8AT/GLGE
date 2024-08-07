/**
 * @file GLGEMath.cpp
 * @author DM8AT
 * @brief define the math functoins for GLGEMath.h
 * the private noise functions and the cosine interpolation is from https://stackoverflow.com/questions/29711668/perlin-noise-generation
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright DM8AT 2023. All rights reserved. This project is released under the MIT license. 
 */

#include "GLGEMath.h"
#include <math.h>
#include "GLGEIndependend/glgePrivDefines.hpp"

/**
 * @brief store the seed for the noise functions
 */
unsigned long glgeSeed = 0;

mat4 glgeLookAt(vec3 eye, vec3 center, vec3 up)
{
    //create the u,v,n vectors
    vec3 u,v,n;
    //create the forward vector (n)
    n = (eye - center);
    //normalize the forward vector
    n.normalize();
    //create the right vector (u)
    u = up.cross(n);
    //normalize the vector
    u.normalize();
    //create the new up vector (v)
    v = n.cross(u);

    //return the final transformation matrix
    return mat4(
        u.x, u.y, u.z, -(eye * u),
        v.x, v.y, v.z, -(eye * v),
        n.x, n.y, n.z, -(eye * n),
        0,0,0,1
    );
}

vec3 glgeRotateVector(float angle, vec3 axis, vec3 v)
{
    //create a rotation quaternion from the vector input and the angle
    Quaternion RotationQ = Quaternion(angle, axis);

    //conjugate the rotation quaternion
    Quaternion ConjugateQ = RotationQ.conjugate();

    //do some quaternion magic, I don't understand this part compleatly
    Quaternion W = (RotationQ * v) * ConjugateQ;

    //return the vector component of the quaternions
    return vec3(W.x,W.y,W.z);
}

//thanks for https://stackoverflow.com/questions/29711668/perlin-noise-generation for help with the noise

  /**
   * @brief store the numbers on X
   */
  int numX = 512,
      /**
       * @brief store the numbers on Y
       */
      numY = 512,
      /**
       * @brief store the number of octaves
       */
      numOctaves = 7;
  /**
   * @brief store the persistance for heigher-level noise
   */
  float persistence = 0.5;

  /**
   * @brief define how much prime number trippels are in the list
   */
  #define maxPrimeIndex 10
  /**
   * @brief store the current prime index
   */
  int primeIndex = 0;

  /**
   * @brief store a lot of heigh prime numbers
   */
  int primes[maxPrimeIndex][3] = {
    { 995615039, 600173719, 701464987 },
    { 831731269, 162318869, 136250887 },
    { 174329291, 946737083, 245679977 },
    { 362489573, 795918041, 350777237 },
    { 457025711, 880830799, 909678923 },
    { 787070341, 177340217, 593320781 },
    { 405493717, 291031019, 391950901 },
    { 458904767, 676625681, 424452397 },
    { 531736441, 939683957, 810651871 },
    { 997169939, 842027887, 423882827 }
  };

  /**
   * @brief calculate simplex noise
   * 
   * @param i the iteration
   * @param x the x position to sample the noise
   * @param y the y position to sample the noise
   * @return float the noise value at the specified position
   */
  float Noise(int i, int x, int y) {
    //do some magic
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int a = primes[i][0], b = primes[i][1], c = primes[i][2];
    int t = (n * (n * n * a + b) + c) & 0x7fffffff;
    //output a semi-random number
    return 1.0 - (float)(t)/1073741824.0;
  }
  /**
   * @brief calculate smoothed simplex noise
   * 
   * @param i the iteration
   * @param x the x position to sample the noise
   * @param y the y position to sample the noise
   * @return float the smoothed noise value
   */
  float SmoothedNoise(int i, int x, int y) {
    //calculate a lot of noise
    float corners = (Noise(i, x-1, y-1) + Noise(i, x+1, y-1) +
                      Noise(i, x-1, y+1) + Noise(i, x+1, y+1)) / 16,
          sides = (Noise(i, x-1, y) + Noise(i, x+1, y) + Noise(i, x, y-1) +
                    Noise(i, x, y+1)) / 8,
          center = Noise(i, x, y) / 4;
    //calculate the sum of a lot of noise
    return corners + sides + center;
  }

  /**
   * @brief interpolate between two numbers
   * 
   * @param a the first number
   * @param b the second number
   * @param x the interpolation slider
   * @return float the interpolated value
   */
  float Interpolate(float a, float b, float x) {  // cosine interpolation
    //multiply x by pi and calculate the half of 1 minus cosine of ft(x * PI)
    float ft = x * GLGE_PI,
          f = (1 - cos(ft)) * 0.5;
    //return a times 1 minus f and add b times f (alias do some magic)
    return  a*(1-f) + b*f;
  }

  /**
   * @brief even smoother noise than smooth noise
   * 
   * @param i the iteration
   * @param x the x position to sample the noise
   * @param y the y position to sample the noise
   * @return float the interpolated and smoothed noise
   */
  float InterpolatedNoise(int i, float x, float y) {
    //store the inputed x as an integer
    int integer_X = x;
    //store the part behind the dot of x
    float fractional_X = x - integer_X;
    //do the same for y
    int integer_Y = y;
    float fractional_Y = y - integer_Y;

    //calculate 4 somooth noise values and interpolate betwean them
    float v1 = SmoothedNoise(i, integer_X, integer_Y),
          v2 = SmoothedNoise(i, integer_X + 1, integer_Y),
          v3 = SmoothedNoise(i, integer_X, integer_Y + 1),
          v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1),
          i1 = Interpolate(v1, v2, fractional_X),
          i2 = Interpolate(v3, v4, fractional_X);
    //return an interpolation of the noise
    return Interpolate(i1, i2, fractional_Y);
  }

  /**
   * @brief value noise
   * 
   * @param x the x position to sample the noise
   * @param y the y position to sample the noise
   * @return float the noise at the sample position
   */
  float ValueNoise_2D(float x, float y) {
    //set the total to 0
    float total = 0,
          //set the frequency to 2 to the power of the number of octaves
          frequency = pow(2, numOctaves),
          //set the amplitude to 1
          amplitude = 1;
    //repeat num octave times
    for (int i = 0; i < numOctaves; ++i) {
      //divide the frequency by 2
      frequency /= 2;
      //multypile the amplitude by the persitance
      amplitude *= persistence;
      //add a interpolated noise to the total
      total += InterpolatedNoise((primeIndex + i) % maxPrimeIndex,
          x / frequency, y / frequency) * amplitude;
    }
    //return the total divided by the freuqency
    return total / frequency;
  }


float glgeLerp(float x, float y, float t)
{
    //return the interpolated input
    return x + t * (y - x);
}

vec2 glgeLerp(vec2 x, vec2 y, float t)
{
    //it is like for floats
    return x*t + y*(1-t);
}

vec3 glgeLerp(vec3 x, vec3 y, float t)
{
    //it is like for floats
    return x*t + y*(1-t);
}

float glgeInterpolate(float x, float y, float t)
{
    //run the interpolation function (this function is private)
    return Interpolate(x, y, t);
}

float glgeNoise2D(vec2 pos)
{
    //return the noise of the position with the stored seed
    return Noise(glgeSeed, pos.x, pos.y);
}

float glgeInterpolatedNoise2D(vec2 pos)
{
    //return the interpolated noise value of the inputed position and the stored seed
    return InterpolatedNoise(glgeSeed, pos.x, pos.y);
}

float glgeValueNoise2D(vec2 pos)
{
    //return the value noise value of the inputed position
    return ValueNoise_2D(pos.x, pos.y);
}

float glgeSmoothedNoise2D(vec2 pos)
{
    //return the smoothed noise value of the seed and the inputed position
    return SmoothedNoise(glgeSeed, pos.x, pos.y);
}

void glgeSetSeed(unsigned long seed)
{
    //store the inputed seed
    glgeSeed = seed;
}

unsigned long glgeGetSeed()
{
    //return the stored seed
    return glgeSeed;
}

void glgeSetNumOctaves(unsigned int n)
{
    //store the inputed number of octaves
    numOctaves = n;
}

int glgeGetNumOctaves()
{
    //get the stored number of octaves
    return numOctaves;
}

void glgeSetPersistance(float p)
{
    //store the inputed persistance
    persistence = p;
}

float glgeGetPersistance()
{
    //return the stored bersistance
    return persistence;
}

unsigned int glgeClamp(unsigned int v, unsigned int min, unsigned int max)
{
    //check if the value is smaler than the minimal value
    if (v < min)
    {
        //if it is, then the value is the minimum
        v = min;
    }
    //else, if the value is greater than the maximum
    else if (v > max)
    {
        //then the value is the maximum
        v = max;
    }
    //return the value
    return v;
}

int glgeClamp(int v, int min, int max)
{
    //check if the value is smaler than the minimal value
    if (v < min)
    {
        //if it is, then the value is the minimum
        v = min;
    }
    //else, if the value is greater than the maximum
    else if (v > max)
    {
        //then the value is the maximum
        v = max;
    }
    //return the value
    return v;
}

float glgeClamp(float v, float min, float max)
{
    //check if the value is smaler than the minimal value
    if (v < min)
    {
        //if it is, then the value is the minimum
        v = min;
    }
    //else, if the value is greater than the maximum
    else if (v > max)
    {
        //then the value is the maximum
        v = max;
    }
    //return the value
    return v;
}

vec2 glgeClamp(vec2 v, float min, float max)
{
    //simply clamp the x and y value independandly
    return vec2(glgeClamp(v.x, min, max), glgeClamp(v.y, min, max));
}

vec3 glgeClamp(vec3 v, float min, float max)
{
    //clamp the x,y and z axes independandly
    return vec3(glgeClamp(v.x, min, max), glgeClamp(v.y, min, max), glgeClamp(v.z, min, max));
}

vec4 glgeClamp(vec4 v, float min, float max)
{
    //clamp the x,y,z and w axes independandly
    return vec4(glgeClamp(v.x, min, max), glgeClamp(v.y, min, max), glgeClamp(v.z, min, max), glgeClamp(v.w, min, max));
}

void glgeClamp(float* v, float min, float max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

void glgeClamp(vec2* v, float min, float max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

void glgeClamp(vec3* v, float min, float max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

void glgeClamp(vec4* v, float min, float max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

void glgeClamp(int* v, int min, int max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

void glgeClamp(unsigned int* v, unsigned int min, unsigned int max)
{
    //cast to the clamp function
    *v = glgeClamp(*v, min, max);
}

//convert the angle a to radians
float glgeToRadians(float a)
{
    //return the angle in radians
    return a * GLGE_TO_RADIANS;
}

//convert the angle a to degrees
float glgeToDegrees(float a)
{
    //return the angle in degrees
    return a * GLGE_TO_DEGREES;
}

float glgeMax(float value, float min)
{
    //if the value is greater min, return the value, else min
    return (value >= min) ? value : min;
}

float glgeMin(float value, float max)
{
    //if the value is less than max, return the value, else max
    return (max >= value) ? max : value;
}

Quaternion glgeCreateRotationQuaternion(float angle, vec3 axis)
{
    //articel to check the math: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    //create the quaternion to return
    Quaternion q;
    //pre-calculate the sin of the half angle, so it is only calculate once
    float sinh = std::sin(angle/2.f);
    //set the angle
    q.w = std::cos(angle / 2.f);
    //calcualte the axis part of the quaternion
    q.x = sinh * axis.x;
    q.y = sinh * axis.y;
    q.z = sinh * axis.z;
    //return the finished quaternion
    return q;
}

Quaternion glgeEulerToQuaternion(vec3 rot)
{
    //math: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    //pre-calculate all posibilitys for sin and cosin
    float cr = std::cos(rot.x * 0.5);
    float sr = std::sin(rot.x * 0.5);
    float cp = std::cos(rot.y * 0.5);
    float sp = std::sin(rot.y * 0.5);
    float cy = std::cos(rot.z * 0.5);
    float sy = std::sin(rot.z * 0.5);
    //create the quaternion to return
    Quaternion q;
    //calculate the quaternion from the sin and cos precalculations
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    //return the quaternion
    return q;
}

Quaternion glgeEulerToQuaternion(float x, float y, float z)
{
    //cast to another function
    return glgeEulerToQuaternion(vec3(x,y,z));
}

vec3 glgeQuaternionToEuler(Quaternion q)
{
    //math: https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
    //pre-compute this, it gets used twice
    float tmp = q.w*q.w - q.x*q.x - q.y*q.y - q.z*q.z;
    //calculate the rotation around the x axis
    float x = std::atan2(2*(q.w*q.x - q.y*q.z),tmp);
    //calculate the rotation around the y axis
    float y = std::asin(2*(q.w*q.y - q.x*q.z));
    //calculate the rotation around the z axis
    float z = std::atan2(2*(q.w*q.z - q.x*q.y),tmp);
    //return the finished rotations as vector
    return vec3(x,y,z);
}

vec3 glgeAngleToDir(vec3 rot)
{
    //convert the inputed rotation to a quaternion
    Quaternion euler = glgeEulerToQuaternion(rot);
    //rotate the z-axis as quaternion around the point
    Quaternion q = euler * Quaternion(0,0,0,1) * euler.invert();
    //extract the direction vector
    vec3 dir = vec3(q.x,q.y,q.z);
    //return the extracted direction
    return dir;
}

vec3 glgeAngleToDir(float x, float y, float z)
{
    //cast to another function
    return glgeAngleToDir(vec3(x,y,z));
}