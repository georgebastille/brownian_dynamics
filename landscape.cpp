/*
 * landscape.cpp
 * 
 * Functions related to generating one dimensional
 * energy landscapes.
 * 
 * Richard Hanes
 * 26/11/2012
 * 
 */

#include <vector>
#include <stdexcept>

#include <iostream>
#include <cmath>

#include "randn.h"



using std::vector;
using std::domain_error;

using std::cout;
using std::endl;

using std::sqrt;

void generate_roughness(long lsize, long partRad, int overlap, vector<double>& roughness)
{
	
	roughness.clear();
	
	for (int i = 0; i < lsize; ++i)
    {
		roughness.push_back(randn_notrig());
    }
	
	// add code here to copy the first part of the roughness to the end
	// so that the first and last sections look the same
			
	for (int k = 0; k < (overlap * partRad); ++k)
	{
		roughness.push_back(roughness[k]);
	}
	
	return;
}

/* Fills the sphere with the projection of a spherical volume
 * of the given radius.
 * 
 * Returns the second moment of the sphere for rescaling
 */
double fill_sphere(float radius, vector<double>& sphere)
{
	int n = 2.0 * radius;
	
	// error checking code
	if (n < 1)
	{
		throw domain_error("Spherical array too small");
	}
	
	// delete anything in the output vector
	sphere.clear();
	
	// this geometrical equation calculates the volume of a sphere sliced
	// through at a height i from the bottom, i.e. the cumulative form
	// of what we need
	for (int i = 1; i <= n; ++i)
	{
		sphere.push_back(1./3. * 3.14159265359 * i * i * (3 * radius - i));
	}
	
	double normalisation = sphere[sphere.size() - 1];
	
	// The convolution reduces the width of the noise from 1.0 to 
	// SQRT(<sphere[j]^2>)/<sphere[j]>
	// Since we normalise, <sphere[j]> = 1.0
	// therefore the reduction in the width of the distribution is
	// given by SQRT(<sphere[j]^2>), which this function returns
	
	double m2 = 0;
	
	// now go through backwards to extract the individual volume slices 
	// from the cumulative volume
	for (int j = sphere.size() - 1; j >= 0; --j)
	{
		sphere[j] -= sphere[j-1];
		sphere[j] /= normalisation;
		m2 += sphere[j] * sphere[j];
	}
	
	return sqrt(m2);
}

/*
 * convolution1D takes a one dimensional kernel vector
 * and a one dimensional sample vector and calculates
 * the convolution of the two.
 * The output convolution will be smaller than the
 * sample as the edges are not calculated.
 * 
 */
void convolution1D(const vector<double>& kernel, 
		   const vector<double>& sample, 
			 vector<double>& result)
{
    typedef vector<double>::size_type vec_sz;
        
    vec_sz kSize = kernel.size();
    vec_sz sSize = sample.size();
    
    if (kSize >= sSize)
    	throw domain_error("Sample needs to be bigger than kernel.");
    
    // I hope there was noting you wanted to keep in the result vector?
    result.clear();
    
    // reserve space in the result as we know how big it should be
    result.reserve(sSize - kSize);
    
    for (vec_sz i = 0; i < (sSize - kSize); ++i)
    {
		// set to zero before sum
		// and using push_back increases the size
		result.push_back(0);
		for (vec_sz j = 0; j < kSize; ++j)
		{
	   	 	// convolve: multiply and accumulate
	    	// the offest (kSize - 1) ensures we do
	    	// not cross over into negative space
	    	result.at(i) += sample[i - j + kSize - 1] * kernel[j];    
		}
    }
    
    return;
} 


void rescale_landscape(double rescale, vector<double>& landscape)
{
	
	// could almost certainly use std::transform here, question is, is it faster?
	for (vector<double>::iterator itr = landscape.begin(); itr != landscape.end(); ++itr)
	{
		*itr *= rescale;
	}
	
	return;
}

