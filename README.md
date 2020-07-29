# JSON-configurable Cell-DEVS Hoya model with age-separated population, separation between virulence and susceptibility, deaths, infected capacity,  quarantines, and masks

## Populations
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

## "Transition Factors"
	- *susceptibility* (array of decimals)
		- How easily a susceptible person in each age group can be infected by the disease.
	- *virulence* (array of decimals)
		- How effective an infected person in each age group is at spreading the disease.
	- *recovery* (array of decimals)
		- How quickly an infected person in each age group recovers from the disease.
	- *mortality* (array of decimals)
		- How likely an infected person in each age group is to die from the disease.

## Hospital Capacity
	- *infected_capacity* (decimal)
		- The portion of the population can be handled as infected before the hospitals run out of space.
	- *over_capacity_modifier* (decimal)
		- The mortality increase that is applied when the hospitals don't have enough space to handle the infected population.

## Quarantines
	- *phase_penalties* (array of arrays of decimals)
		- Each array represents a phase, and the decimal values within those arrays represent the impact of that particular phase of the quarantine on the corresponding age group.
	- *phase_durations* (array of integers)
		- How long each corresponding phase lasts.
	- *disobedience* (array of decimals)
		- The amount of people in each age group that disobey the quarantine rules.

## Masks
	- *mask_use* (array of decimals)
		- The amount of people in each age group that use masks.
	- *mask_reduction* (decimal)
		- The amount that the infection rate is reduced to (Eg. a value of 0.3 means that the mask blocks 70% of the spread).
	- *mask_adoption* (decimal)
		- How much the infected population impacts the number of people who wear masks.

