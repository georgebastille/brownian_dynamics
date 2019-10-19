#include <iostream>   // basic io streams
#include <fstream>    // file stream for file io
#include <vector>     // vector data container
#include <stdexcept>  // needed for the srandard exceptions
#include <cstdlib>    // needed for string to number functions
#include <cmath>      // mathematical functions
#include <climits>	  // biggest possible numbers
#include <stdint.h>   // uint64_t

#include <sys/stat.h> // gets stats from a file (to see if it exists)

#include "rough_sim.h"
#include "split.h"
#include "randn.h"
#include "landscape.h"

using namespace std;

// the config mechanism would benefit greatly from oop
// if the configuration is a class, it can contain lots
// of state about whch options have been read and which
// are the defaults (not read) as well as methods for
// saving and restoring (and checking) the configuration
// options
bool read_config(configuration& conf, const char* file_name)
{
    
    cout << "Reading config file..." << endl;
    // check for config file
    // if exists, read it
    
    // if a line is not recognised, or a config
    // file present but not complete, write to the log
    // and use the default or quit and complain?
    
    // test for existence of config file
    ifstream cFile(file_name);
    bool file_exists;
    
    if (cFile)
    {
		cout << "Config file found..." << endl;
		file_exists = true;
		string s;
		int line_no = 0;
		// read the file a line at a time
		while (getline(cFile, s))
		{
			++line_no;
			// ignore lines starting with #
			if (s[0] != '#')
			{
				vector<string> tokens;
				// split the line and insert tokens the container tokens
				split(s, back_inserter(tokens));
				// if-else-if to set options
				if (tokens[0] == "epsilon")
				{
					conf.epsilon = atof(tokens[2].c_str());
				}
				else if (tokens[0] == "partRad")
				{
					conf.partRad = atof(tokens[2].c_str());
				}
				else if (tokens[0] == "tBSteps")
				{
					conf.tBSteps = atol(tokens[2].c_str());
				}
				else if (tokens[0] == "lsize")
				{
					conf.lsize = atol(tokens[2].c_str());
				}
				else if (tokens[0] == "rSeed")
				{
					conf.rSeed = atol(tokens[2].c_str());
				}
				else if (tokens[0] == "nParticles")
				{
					conf.nParticles = atol(tokens[2].c_str());
				}
				else
				{
					cout << line_no << " contains an unrecognised option, ignored." << endl;
				}
			}
		}
    }
    else
    {
		cout << "Config file not found" << endl;
		file_exists = false;
    }
    
    return file_exists; 
}

void write_default_config_file(const char* file_name)
{
    cout << "Writing default config file" << endl;
    ofstream cfile(file_name);
    if (cfile)
    {
		cfile << "epsilon\t\t= " << default_configuration::epsilon << endl;
		cfile << "partRad\t\t= " << default_configuration::partRad << endl;
		cfile << "tBSteps\t\t= " << default_configuration::tBSteps << endl;
		cfile << "lsize\t\t= " << default_configuration::lsize << endl;
		cfile << "rSeed\t\t= " << default_configuration::rSeed << endl;
		cfile << "nParticles\t= " << default_configuration::nParticles << endl;
	
		cfile.close();
    }
    else 
    {
		throw runtime_error("Config file could not be created");
    }
}

template <typename T>
void write_as_file(const vector<T>& vec, const char* fn){
	
	ofstream vector_file(fn);
	
    if (vector_file)
    {
		int i = 0;
		
		cout << "Writing vector file " << fn << endl;
		for (typename vector<T>::const_iterator itr = vec.begin(); itr != vec.end(); ++itr)
		{
			// note using '\n' here instead of endl, allowing the stream to buffer
			// greatly improving the write performance
			vector_file << ++i << '\t' << *itr << '\n';
		}
		
		vector_file.close();
    }
    else 
    {
		throw runtime_error("Vector output file could not be created");
    }
	
	return;
}

void print_parameters(const configuration& params, ostream& out)
{
	out << "epsilon = "    << params.epsilon << endl;
	out << "partRad = "    << params.partRad << endl;
	out << "tBSteps = "    << params.tBSteps << endl;
	out << "lsize = "      << params.lsize << endl;
	out << "rSeed = "      << params.rSeed << endl;
	out << "nParticles = " << params.nParticles << endl;

	return;
}

class exponential_sequence
{
	public:
		exponential_sequence() : current(0), power_of(1.01) {};
		
		exponential_sequence(int present_current) : power_of(1.01)
		{
			current = present_current;
		}
		
		uint64_t next()
		{
			uint64_t next_val, prev_val;
			if (current == 0)
			{
				prev_val = 0;
			}
			else
			{
				prev_val = pow(power_of, current);
			}
			
			do
			{
				next_val = pow(power_of, ++current);
			}
			while (next_val == prev_val);
		
			return next_val;
		}
		
		void set_current(int new_current)
		{
			current = new_current;
		}
		int get_current()
		{
			return current;
		}
	private:
		int current;
		double power_of;
};

template <typename T>
int write_binary(const vector<T>& v, const char* fn)
{
	typename vector<T>::size_type n_ele = v.size();
	int ele_size = sizeof(T);

	ofstream writer(fn, ios::out | ios::binary | ios::trunc);
	// First write the number of elements
	writer.write((char*)(&n_ele), sizeof(n_ele));
	writer.write((char*)(&v[0]), ele_size * n_ele);
	writer.close();

	return 0;

}

template <typename T>
int read_binary(vector<T>& v, const char* fn)
{
	typename vector<T>::size_type n_ele;
	int ele_size = sizeof(T);

	ifstream reader(fn, ios::in | ios::binary);
	reader.read(reinterpret_cast<char*>(&n_ele), sizeof(n_ele));
	v.clear();
	v.resize(n_ele);
	reader.read(reinterpret_cast<char*>(&v[0]), n_ele * ele_size);
	reader.close();
	return 0;
}

int save_static_state(const system_state& s)
{
	// landscape
	write_binary(s.landscape, "./landscape.bin");
	write_binary(s.zeroState, "./zeroState.bin");
	ofstream writer("./pbc.bin", ios::out | ios::binary | ios::trunc);

	writer.write((char*)(&s.left_from), sizeof(s.left_from));
	writer.write((char*)(&s.left_to), sizeof(s.left_to));
	writer.write((char*)(&s.right_from), sizeof(s.right_from));
	writer.write((char*)(&s.right_to), sizeof(s.right_to));

	writer.close();
	
	return 0;
}

int save_dynamic_state(const system_state& s)
{
	write_binary(s.particles, "./particles.bin");
	write_binary(s.particles_abs, "./particles_abs.bin");
	
	ofstream writer("./temporal.bin", ios::out | ios::binary | ios::trunc);

	writer.write((char*)(&s.t), sizeof(s.t));
	writer.write((char*)(&s.current), sizeof(s.current));

	writer.close();
	
	return 0;
	
}

int read_state(system_state& s)
{
	// Config file
	read_config(s.params);
	
	// Statics
	read_binary(s.landscape, "./landscape.bin");
	read_binary(s.zeroState, "./zeroState.bin");
	ifstream sread("./pbc.bin", ios::in | ios::binary);
	sread.read((char*)(&s.left_from), sizeof(s.left_from));
	sread.read((char*)(&s.left_to), sizeof(s.left_to));
	sread.read((char*)(&s.right_from), sizeof(s.right_from));
	sread.read((char*)(&s.right_to), sizeof(s.right_to));
	sread.close();
	
	// Dynamics
	read_binary(s.particles, "./particles.bin");
	read_binary(s.particles_abs, "./particles_abs.bin");
	ifstream dread("./temporal.bin", ios::in | ios::binary);
	dread.read((char*)(&s.t), sizeof(s.t));
	dread.read((char*)(&s.current), sizeof(s.current));
	dread.close();
	
	return 0;
	
}

// Function: fileExists
/**
    Check if a file exists
@param[in] filename - the name of the file to check

@return    true if the file exists, else false

*/
bool fileExists(const string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}


int main()
{
	// struct holding the system variables
	system_state sys;
	
	// Object to determine the next dt to save,
	// state can be read and restored
	exponential_sequence dt;
	
	ofstream moments;
		    	
	// check for existence of saved state
	// if so, load it
	if (fileExists("./landscape.bin"))
	{
		cout << "Previous state vector detected" << endl;
		read_state(sys);
		// seed RNG
		seed_me(0);
		dt.set_current(sys.current);
		moments.open("./moments", ios::app);

	}
	else // if not, initialise from new
	{
		cout << "No previous state vector detected" << endl;
    	// read config file
    	if (!read_config(sys.params))
    	{
			write_default_config_file();
			// this could lead to in infinite loop if the file 
			// cannot be written for some reason
			read_config(sys.params);
    	}
        
    	if (sys.params.lsize > sys.landscape.max_size())
    	{
			throw runtime_error("Landscape size exceeds maximum capacity");
		}
	
    	print_parameters(sys.params, cout);
    
    	/* initialize random seed: */
	
		unsigned long used_seed = seed_me(sys.params.rSeed);
    	cout << "Random Seed = "  << used_seed << endl;
	
		vector<double> roughness;
		int periodic_overlap = 4;
    	cout << "Generating roughness" << endl;
		generate_roughness(sys.params.lsize, sys.params.partRad, periodic_overlap, roughness);

    	vector<double> sphere;
    	cout << "Generating spherical array" << endl;
    	double rescaled = fill_sphere(sys.params.partRad, sphere);
	
		cout << "Convoluting sphere with roughness" << endl;
		convolution1D(sphere, roughness, sys.landscape);
	
		cout << "Rescaling landscape to desired epsilon" << endl;
		rescale_landscape((sys.params.epsilon / rescaled), sys.landscape);

		// delete roughness and sphere to save some memory
		roughness.resize(0);
		sphere.resize(0);
	
		// these indices are for the periodic boundary conditions, if a particle
		// finds itself at *_from (an edge), it gets moved to *_to (not an edge)
		// The landscape has been designed so that these elements are locally 
		// equivalent (i.e. the left and right neighbours look the same at each
		// point), but the exist on opposite ends of the landscape.
		sys.left_from = 0;
		sys.left_to   = sys.params.lsize;
		sys.right_from = sys.landscape.size()-1;
		sys.right_to = ((periodic_overlap-2)*sys.params.partRad)-1;

		// landscape is complete
	
		const char* fn = "./landscape";
	
		write_as_file(sys.landscape, fn);
	
		cout << "Initialising particles" << endl;
		// initialise particles with a space between each,
		// and a space at the start and the end
		uint32_t space = sys.params.lsize / ( sys.params.nParticles + 1 );
	
		for (uint32_t i = 0; i < sys.params.nParticles; ++i)
		{
			sys.particles.push_back(space * (1 + i));
		}
		
		// save the initial positions
		cout << "Saving initial system state vector" << endl;
		sys.zeroState = sys.particles;
		sys.particles_abs = sys.particles;

		save_static_state(sys);
		
		sys.t = 1;
		sys.current = 0;
		save_dynamic_state(sys);
		
		moments.open("./moments", ios::trunc);

	
	}
	// the time is defined in Brownian times, one Brownian time is
	// partRad^2 steps
	uint32_t brownian_time = pow(sys.params.partRad, 2);
	
	uint64_t end_t = sys.params.tBSteps * brownian_time;
	
	uint64_t next_dt = dt.next();
	
	cout << "Starting simulation\nt=" << flush;
	
	// loop over time
	while (sys.t < end_t)
	{
		
		if (sys.t % 1024 == 0)
		{
			cout << sys.t << " - " << flush;
		}
		
		for (vector<int32_t>::size_type pp = 0; pp != sys.particles.size(); ++pp)
		{
			// pick a direction (-1 or +1)
			int dir = ((rand() % 2) * 2) - 1;
			
			int32_t particle_pos = sys.particles[pp];
			
			// trying to find the seg-fault
			if (particle_pos < 0 || particle_pos >= sys.landscape.size())
			{
				cout << "Uh oh, the first if fired. particle_pos = " << particle_pos << endl;
				throw new runtime_error("Uh oh");
			}
			
			// trying to find the seg-fault
			if ((particle_pos + dir) < 0 || (particle_pos + dir) >= sys.landscape.size())
			{
				cout << "Uh oh, the second if fired. particle_pos + dir = " << (particle_pos + dir) << endl;
				cout << "Landscape size = " << sys.landscape.size() << endl;
				cout << "Left_from = " << sys.left_from << endl;
				cout << "Left_to = " << sys.left_to << endl;
				cout << "Right_from = " << sys.right_from << endl;
				cout << "Right_to = " << sys.right_to << endl;
				
				throw new runtime_error("Uh oh");
			}
			
			// calculate dE
			double dE = sys.landscape[sys.particles[pp] + dir] - sys.landscape[sys.particles[pp]];
			double random_val = rand_u();
			
			// compare rand to exp(-dE) and move
			if (dE <= 0.0 || (random_val < exp(-dE)))
			{
				sys.particles[pp] += dir;
				sys.particles_abs[pp] += dir;
			}
			
			// Implementation of the periodic boundary
			if (sys.particles[pp] == sys.left_from)
			{
				sys.particles[pp] = sys.left_to;
			} 
			
			if (sys.particles[pp] == sys.right_from)
			{
				sys.particles[pp] = sys.right_to;
			}
		}
	
		// code for analysing particle trajectories
		
		if (sys.t == next_dt)
		{
			int j = 0;
			double m1 = 0.0;
			double m2 = 0.0;
			double m4 = 0.0;
		
			// calculate moments
			for (vector<int32_t>::const_iterator pp = sys.particles_abs.begin(); pp != sys.particles_abs.end(); ++pp)
			{
				int64_t diff = *pp - sys.zeroState[j++];
				double fdiff = (double) diff;
				m1 += diff;
				m2 += fdiff * fdiff;
				m4 += fdiff * fdiff * fdiff * fdiff;
			}
				
			m1 /= (sys.params.nParticles * sys.params.partRad);
			m2 /= (sys.params.nParticles * pow(sys.params.partRad, 2));
			m4 /= (sys.params.nParticles * pow(sys.params.partRad, 4));
			
			// Calculate and save the next time step where the moments will be saved
			sys.current = dt.get_current();
			next_dt = dt.next();

			moments << (double) sys.t / brownian_time << '\t' << m1 << '\t' << m2 << '\t' << m4 << endl;
			
			save_dynamic_state(sys);

		}
		
		
		if (sys.t % 10000 == 0)
		{
			save_dynamic_state(sys);
		}
		
		++sys.t;
	}
	
	cout << "Simulation finished" << endl;
	
	moments.close();
		
	return 0;
    
} 
