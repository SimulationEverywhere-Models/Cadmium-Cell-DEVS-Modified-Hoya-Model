# JSON-configurable Cell-DEVS Hoya model with age-separated population, separation between virulence and susceptibility, deaths, infected capacity,  quarantines, masks, and lockdowns

## Info

This is a modified version of the Hoya model. The original model that this implementation is based off of can be found at:

https://www.sciencedirect.com/science/article/pii/S0096300306009295

This implementation adds several features which allow more real-world factors to be accounted for. See the Parameters section for more information.


## Compilation

Note: A C++17 compliant compiler is required, along with the Cadmium library (available at: https://github.com/SimulationEverywhere/cadmium/tree/celldevs-json)

Using the Terminal

1. Clone this repository and go to its main directory

2. Run this command on the command line:

```bash
g++ -g -I<path to cadmium>/cadmium/include -I<path to cadmium>/include -I<path to cadmium>/json/single_include -std=c++17 -o hoya ./model/main.cpp ./model/hoya_cell.hpp
```


## Usage

To run a simulation with this model:

1. Compile the executable.

2. Ensure a "../simulation_results/pandemic_hoya_age_json/" directory exists relative to the executable.

3. Create a scenario. An example is shown in the repository's "./config/" folder as "scenario.json"

4. Run the executable from the command line, passing in the filepath to the scenario file. Optionally, you can also pass the maximum number of time steps before the simulation stops (default is 500).


## Parameters

### Populations
- *population* (integer)
	- The total number of people in the cell.
- *susceptible* (array of decimals)
	- The portion of the population which can catch the disease. Each number in the array corresponds to a different age group.
- *infected* (array of decimals)
	- The portion of the population which currently have the disease. Each number in the array corresponds to a different age group.
- *recovered* (array of decimals)
	- The portion of the population which no longer have the disease. Each number in the array corresponds to a different age group.
- *deceased* (array of decimals)
	- The portion of the population which have died. Each number in the array corresponds to a different age group.

### "Transition Factors"
- *susceptibility* (array of decimals)
	- How easily a susceptible person in each age group can be infected by the disease.
- *virulence* (array of decimals)
	- How effective an infected person in each age group is at spreading the disease.
- *recovery* (array of decimals)
	- How quickly an infected person in each age group recovers from the disease.
- *mortality* (array of decimals)
	- How likely an infected person in each age group is to die from the disease.

### Hospital Capacity
- *infected_capacity* (decimal)
	- The portion of the population can be handled as infected before the hospitals run out of space.
- *over_capacity_modifier* (decimal)
	- The mortality increase that is applied when the hospitals don't have enough space to handle the infected population.

### Quarantines
- *phase_penalties* (array of arrays of decimals)
	- Each array represents a phase, and the decimal values within those arrays represent the impact of that particular phase of the quarantine on the corresponding age group.
- *phase_durations* (array of integers)
	- How long each corresponding phase lasts.
- *disobedience* (array of decimals)
	- The amount of people in each age group that disobey the quarantine rules.

### Masks
- *mask_use* (array of decimals)
	- The amount of people in each age group that use masks.
- *mask_virulence_reduction* (decimal)
	- The amount that the virulence rate is reduced to (Eg. a value of 0.3 means that the mask blocks 70% of the outgoing spread).
- *mask_susceptibility_reduction* (decimal)
	- The amount that the susceptibility rate is reduced to (Eg. a value of 0.3 means that the mask blocks 70% of the incoming spread).
- *mask_adoption* (decimal)
	- How much the infected population impacts the number of people who wear masks.

### Lockdowns
- *lockdown_rates* (array of decimals)
	- How much each age group is restricted from travel by the lockdown (1.0 is unaffected, 0.0 is most affected).
- *lockdown_adoption* (decimal)
	- How much of an impact the current infected population has on the strength of the lockdown.
- (Note: Lockdowns are completely separate from Quarantines)
