#ifndef GUARD_randn_h
#define GUARD_randn_h


#include <cmath>

#define PI 3.14159265358979323846

/******************************************************************************/
/* randn()
 * 
 * Normally (Gaussian) distributed random numbers, using the Box-Muller 
 * transformation.  This transformation takes two uniformly distributed deviates
 * within the unit circle, and transforms them into two independently 
 * distributed normal deviates.  Utilizes the internal rand() function; this can
 * easily be changed to use a better and faster RNG.
 * 
 * The parameters passed to the function are the mean and standard deviation of 
 * the desired distribution.  The default values used, when no arguments are 
 * passed, are 0 and 1 - the standard normal distribution.
 * 
 * 
 * Two functions are provided:
 * 
 * The first uses the so-called polar version of the B-M transformation, using
 * multiple calls to a uniform RNG to ensure the initial deviates are within the
 * unit circle.  This avoids making any costly trigonometric function calls.
 * 
 * The second makes only a single set of calls to the RNG, and calculates a 
 * position within the unit circle with two trigonometric function calls.
 * 
 * The polar version is generally superior in terms of speed; however, on some
 * systems, the optimization of the math libraries may result in better 
 * performance of the second.  Try it out on the target system to see which
 * works best for you.  On my test machine (Athlon 3800+), the non-trig version
 * runs at about 3x10^6 calls/s; while the trig version runs at about
 * 1.8x10^6 calls/s (-O2 optimization).
 * 
 * 
 * Example calls:
 * randn_notrig();	//returns normal deviate with mean=0.0, std. deviation=1.0
 * randn_notrig(5.2,3.0);	//returns deviate with mean=5.2, std. deviation=3.0
 * 
 * 
 * Dependencies - requires <cmath> for the sqrt(), sin(), and cos() calls, and a
 * #defined value for PI.
 */

// Seed the RNG
unsigned long seed_me(long seed);

/******************************************************************************/
//	"Polar" version without trigonometric calls
double randn_notrig(double mu=0.0, double sigma=1.0);
/******************************************************************************/

//	Standard version with trigonometric calls
double randn_trig(double mu=0.0, double sigma=1.0);

// Uniform random on [0.0,1.0)
double rand_u();


#endif
