#ifndef GUARD_rough_sim
#define GUARD_rough_sim

#include <vector>
#include <iostream>
#include <stdint.h>

// Default values  for configuration options, in general should not be modified.
// To change the simulation params, modify ./config_rough_sim which will
// override any parameters defined here.
// If ./config_rough_sim does not exist, run the simulation once and it
// will be created automatically.
struct default_configuration {
    const static double epsilon  = 1.5;	   // strength of the roughness
    const static double partRad  = 32.0;   // particle radius in steps
    const static uint64_t tBSteps    = 1e3;	   // final time in Brownian times
    const static uint32_t lsize      = 524288; // linear size of the landscape (steps)
    const static long rSeed      = 0;      // random seed 
    const static unsigned int nParticles = 128;	   // the number of independent particles
};

struct configuration {
    // default constructor initialises default configuration
    configuration() 
    {
	epsilon    = default_configuration::epsilon;
	partRad    = default_configuration::partRad;
	tBSteps    = default_configuration::tBSteps;
	lsize      = default_configuration::lsize;
	rSeed      = default_configuration::rSeed;
	nParticles = default_configuration::nParticles;
    };
    
    double epsilon;	
    double partRad;	
    uint64_t tBSteps;	
    uint32_t lsize;		
    long rSeed;		
    unsigned int nParticles;
};

// One struct to rule them all
struct system_state
{
	// static parameters
	configuration 		params;
	std::vector<double> landscape;
	std::vector<int32_t> 	zeroState;
	int32_t left_from, left_to, right_from, right_to;
	
	// dynamic parameters
	std::vector<int32_t> 	particles;
	std::vector<int32_t> 	particles_abs;
	int current;
	uint64_t t;
};

bool read_config(configuration& conf, const char* file_name = "./config_rough_sim");

void write_default_config_file(const char* file_name = "./config_rough_sim");

// useful for debugging, write a vector to a file
// although this is a template fuction, the source does not need to be #inculded here (why?)
template <typename T>
void write_as_file(const std::vector<T>&, const char*);

void print_parameters(const configuration&, std::ostream&);

unsigned long next_dt();

template <typename T>
int write_binary(const std::vector<T>&, const char*);

template <typename T>
int read_binary(std::vector<T>&, const char*);

int save_static_state(const system_state&);
int save_dynamic_state(const system_state&);

int read_state(system_state&);

bool fileExists(const std::string& filename);
#endif
