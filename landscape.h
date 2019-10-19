#ifndef GUARD_landscape
#define GUARD_landscape

/* landscape.h
 * 
 * Header file for functions related to generating 
 * one dimensional energy landscapes.
 * 
 * Richard Hanes
 * 
 * 26/11/2012
 * 
 */
 
#include <vector>

/* 
 * Fill the passed container with normally distributed random numbers
 * with the special case that the first and last set of numbers are the
 * same.
 * 
 */
void generate_roughness(long, long, int, std::vector<double>&);

/*
 * Generate a spherical array to represent the volume of a sphere
 * in one dimension.
 * 
 */
double fill_sphere(float, std::vector<double>&);

/* 
 * Calculate the convolution of the sample with the kernel
 * and save it in the result.
 * 
 */
void convolution1D(const std::vector<double>& kernel, 
		   const std::vector<double>& sample, 
			 std::vector<double>& result);

/* 
 * Take the landscape and multiply by a constant.
 * 
 */
void rescale_landscape(double, std::vector<double>&);

#endif
