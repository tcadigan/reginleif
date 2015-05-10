#ifndef MATH_HPP_
#define MATH_HPP_

float MathAngle(float angle);
float MathAngle(float x1, float y1, float x2, float y2);
float MathAngleDifference(float a1, float a2);
float MathAverage(float n1, float n2);
float MathInterpolate(float n1, float n2, float delta);
float MathLine_distance(float x2,
                        float y1,
                        float x2, 
                        float y2, 
                        float ps,
                        float py);

float MathDistance(float x1, float y1, float x2, float y2);
float MathDistance2(float x1, float y1, float x2, float y2);
float MathSmoothStep(float val, float a, float b);
float MathScalarCurve(float val);

#endif /* MATH_HPP_ */
