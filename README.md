# JSON-configurable Cell-DEVS Hoya model with age-separated population, separation between virulence and susceptibility, deaths, infected capacity,  quarantines, and masks

Virulence applies to the people spreading the disease (*virulence*).
Susceptibility applies to the people contracting the disease (*susceptibility*).
Both are multiplied together for each pair considered in the calculations.


Mortality rate affects how likely people are to die before recovering (*mortality*).
When more people are infected in a cell than the infected capacity, the mortality changes (*infected_capacity*).
 - This represents running out of space in hospitals

The over capacity modifier changes the amount that the mortality is affected after the infected cap is reached (*over_capacity_modifier*).


Each phase of the quarantine is reacted to differently by different age groups (*phase_penalties*).
Each different phase has a list of each group's reaction to that particular phase.
Each phase of the quarantine has a different duration (*phase_durations*)
Each age group has a different rate of disobedience toward the quarantine (*disobedience*).
The quarantine is applied as a "mobility correction" on infection contribution from neighboring cells.


Masks reduce the infection rate of the disease by a configurable amount (*mask_reduction*).
 - This value is the amount that the rate is reduced to
 - Eg. a value of 0.3 means that the mask blocks 70% of the spread

Different sub-populations (age groups) may be more or less likely to wear masks (*mask_use*).
The amount that the infection rate is slowed depends on the number of people currently infected.
 - The amount that the infected amount affects this is configurable (*mask_adoption*)
