#include "Calculator.h"

int CalculateFinalScore(int usual_score, int end_score, double weightA) {
    double result = weightA * usual_score + (1.0 - weightA) * end_score;
    return (int)(result + 0.5);
}