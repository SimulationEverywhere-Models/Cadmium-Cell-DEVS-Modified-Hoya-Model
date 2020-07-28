# JSON-configurable Cell-DEVS Hoya model with age-separated population and masks
Masks reduce the infection rate of the disease by a configurable amount.
They separately reduce the likelihood of the wearer infecting others (*mask_virulence_reduction*), and the likelihood of the wearer being infected (*mask_susceptibility_reduction*).
 - This value is the amount that the rate is reduced to
 - Eg. a value of 0.3 means that the mask blocks 70% of the spread
Different sub-populations (age groups) may be more or less likely to wear masks (*mask_use*)
The amount that the infection rate is slowed depends on the number of people currently infected
 - The amount that the infected amount affects this is configurable (*mask_adoption*)