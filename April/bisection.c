/*
    NUMERICAL ANALYSIS
     
    Bisection method - Spring 2025
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// function f(x) = x^2 - 2 
double f(double x)
{
    return x * x - 2;  
}

double bisection(double a, double b, double epsilon)
{
    int iterations = 0;  // variable to count the number of iterations
    double fa = f(a); // evaluate f(a)
    double fb = f(b); // evaluate f(b)
    double mid; // variable to store the midpoint
    double fmid; // variable to store the value of f(mid)

    // check if the initial interval [a, b] is valid: the signs of f(a) and f(b) must be opposite
    if ((fa * fb) >= 0) 
    {
        fprintf(stderr, "Bad input\n");  // if the signs are not opposite, it's not a valid input
        exit(1);
    }

    while (fabs(b - a) > epsilon)  // continue until the difference between a and b is smaller than epsilon (precision)
    {
        mid = a + (b - a) / 2.0;  // calculate the midpoint of the interval [a, b]
        fmid = f(mid);  // evaluate the function at the midpoint

        // if the root lies in the left half of the interval [a, mid], update the interval to [a, mid]
        if ((fa * fmid) < 0)
        {
            b = mid;  // move the right endpoint to mid
            fb = fmid;  // update f(b) to be f(mid)
        }
        // if the root lies in the right half of the interval [mid, b], update the interval to [mid, b]
        else
        {
            a = mid;  // move the left endpoint to mid
            fa = fmid;  // update f(a) to be f(mid)
        }

        iterations++;  // Increment the iteration count
    }

    // final calculation of the midpoint after the loop ends
    mid = a + (b - a) / 2.0;
    printf("Iterations: %d\n", iterations);  // print the number of iterations
    return mid;  // return the midpoint, which is the root of the function
}

int main(int argc, char **argv)
{
    double x1 = 1.0;
    double x2 = 2.0;
    double epsilon = 1e-10;  
    printf("Root: %.10lf\n", bisection(x1, x2, epsilon));  

    // For comparison, print the value of sqrt(2) to show how close the bisection method is
    printf("Sqrt(2): %.10lf\n", sqrt(2));  // Print the actual value of sqrt(2)
    return 0;  // Return 0 to indicate successful completion of the program
}
