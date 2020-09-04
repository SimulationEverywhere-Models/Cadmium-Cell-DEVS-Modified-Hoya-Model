/**
 * Copyright (c) 2020, Román Cárdenas Rodríguez
 * ARSLab - Carleton University
 * GreenLSI - Polytechnic University of Madrid
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
#define CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP

#include <cmath>
#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

#include "state.hpp"
#include "vicinity.hpp"
#include "config.hpp"
#include "lockdown.hpp"

using nlohmann::json;
using namespace cadmium::celldevs;


static std::default_random_engine rand_gen = std::default_random_engine();

template <typename T>
class hoya_cell : public grid_cell<T, sird, mc> {
public:
    using grid_cell<T, sird, mc>::cell_id;
    using grid_cell<T, sird, mc>::simulation_clock;
    using grid_cell<T, sird, mc>::state;
    using grid_cell<T, sird, mc>::map;
    using grid_cell<T, sird, mc>::neighbors;

	using config_type = config;  // IMPORTANT FOR THE JSON
	
	std::vector<float> susceptibility;
	std::vector<float> virulence;
	std::vector<float> recovery;
	std::vector<float> mortality;
	float infected_capacity;
	float over_capacity_modifier;
	std::vector<float> mask_use;
    float mask_susceptibility_reduction;
    float mask_virulence_reduction;
	float mask_adoption;
	unsigned int lockdown_type;
	Lockdown *lockdown;
	
	unsigned int rand_type;
	float rand_seed;
	mutable std::normal_distribution<float> rand_normal_dist;
	mutable std::uniform_real_distribution<float> rand_uniform_dist;
	mutable std::exponential_distribution<float> rand_exponential_dist;
	
	std::vector<float> age_ratio;
	float precision = 100;


	hoya_cell() : grid_cell<T, sird, mc>()  {}

	hoya_cell(cell_position const &cell_id, cell_unordered<mc> const &neighborhood, sird &initial_state,
              cell_map<sird, mc> const &map_in, std::string const &delay_id, config &config) :
			    grid_cell<T, sird, mc>(cell_id, neighborhood, initial_state, map_in, delay_id) {
		susceptibility = config.susceptibility;
		virulence = config.virulence;
		recovery = config.recovery;
		mortality = config.mortality;
		infected_capacity = config.infected_capacity;
		over_capacity_modifier = config.over_capacity_modifier;
		mask_use = config.mask_use;
        mask_susceptibility_reduction = config.mask_susceptibility_reduction;
        mask_virulence_reduction = config.mask_virulence_reduction;
		mask_adoption = config.mask_adoption;
		precision = config.precision;
		age_ratio = std::vector<float>();
		auto s = state.current_state;
		
		rand_type = config.rand_type;
		rand_seed = config.rand_seed;

		lockdown_type = config.lockdown_type;

		switch(lockdown_type) {
			case 1: // lockdown type: scheduled lockdown in phases
				lockdown = new ScheduledPhaseLockdown(config.lockdown_rates, config.phase_durations, config.disobedience);
				break;

			case 2: // lockdown type: continuous reaction to infected
				lockdown = new ReactionContinuousLockdown(config.lockdown_rates, config.lockdown_adoption, config.disobedience);
				break;

			case 3: // lockdown type: reaction to infected in phases
				lockdown = new ReactionPhaseLockdown(config.lockdown_rates, config.phase_thresholds, config.threshold_buffers, config.disobedience);
				break;

			default: // lockdown type: no response
				lockdown_type = 0;
				lockdown = new NoLockdown();
		}
		
		rand_gen.seed();
		switch(rand_type) {
			case 1:
				rand_normal_dist = std::normal_distribution<float>(config.rand_mean, config.rand_stddev);
				break;
			case 2:
				rand_uniform_dist = std::uniform_real_distribution<float>(config.rand_lower, config.rand_upper);
				break;
			case 3:
				rand_exponential_dist = std::exponential_distribution<float>(config.rand_avg_occurence_rate);
				break;
			default:
				rand_type = 0;
		}

		for (int i = 0; i < n_age_segments(); i++) {
			float ratio = std::round(precision * (s.susceptible[i] + s.infected[i] + s.recovered[i] + s.deceased[i])) / precision;
			age_ratio.push_back(ratio);
		}
	}
	
	[[nodiscard]] float random() const {
		switch(rand_type) {
		case 1:
			return rand_normal_dist(rand_gen);
		case 2:
			return rand_uniform_dist(rand_gen);
		case 3:
			return rand_exponential_dist(rand_gen);
		default:
			return 1.0;
		}
	}

	[[nodiscard]] unsigned int inline n_age_segments() const {
		return virulence.size();
	}

	// user must define this function. It returns the next cell state and its corresponding timeout
	[[nodiscard]] sird local_computation() const override {
		auto res = state.current_state;
		auto new_i = new_infections(res);
		auto new_r = new_recoveries(res);
		auto new_d = new_deaths(res);

		res.phase = lockdown->next_phase(simulation_clock, res);

		for (int i = 0; i < n_age_segments(); i++) {
			res.recovered[i] = std::round((res.recovered[i] + new_r[i]) * precision) / precision;
			res.deceased[i] = std::round((res.deceased[i] + new_d[i]) * precision) / precision;
			res.infected[i] = std::round((res.infected[i] + new_i[i] - (new_r[i] + new_d[i])) * precision) / precision;
			res.susceptible[i] = age_ratio[i] - (res.recovered[i] + res.infected[i] + res.deceased[i]);
		}
		return res;
	}

	// It returns the delay to communicate cell's new state.
	T output_delay(sird const &cell_state) const override { return 1; }

	[[nodiscard]] std::vector<float> new_infections(sird const &last_state) const {
		std::vector<float> new_inf = {};
		std::vector<float> virulence_factors = std::vector<float>(n_age_segments(), 0.0);
		std::vector<float> susceptibility_factors = std::vector<float>(n_age_segments(), 0.0);
		float total_virulence_factor = 0.0;

		for(auto neighbor: neighbors) {
			sird neighbor_state = state.neighbors_state.at(neighbor);
			mc neighbor_vicinity = state.neighbors_vicinity.at(neighbor);
			std::vector<float> mobility = find_mobility_factors(neighbor_vicinity);
			std::vector<float> neighbor_virulence_factors = find_virulence_factors(neighbor_state);

			for(int i = 0; i < neighbor_virulence_factors.size(); i++) {
				virulence_factors.at(i) += neighbor_virulence_factors.at(i) * mobility.at(i);
			}
		}
		// ^ find how much the neighbouring cells are contributing to infection

		std::vector<float> local_virulence_factors = find_virulence_factors(last_state);
		for(int i = 0; i < local_virulence_factors.size(); i++) {
			virulence_factors.at(i) += local_virulence_factors.at(i);
		}
		// ^ find how much this cell is contributing to its own infection

		susceptibility_factors = find_susceptibility_factors(last_state);
		// ^ find how vulnerable this cell is to infection

		for(float virulence_factor : virulence_factors) {
			total_virulence_factor += virulence_factor;
		}

		for(int i = 0; i < n_age_segments(); i++) {
			float new_infected_amount = last_state.susceptible[i] * total_virulence_factor * susceptibility_factors[i] / (float)last_state.population * random();
			new_inf.push_back(std::min(last_state.susceptible[i], new_infected_amount));
		}
		return new_inf;
	}

	[[nodiscard]] std::vector<float> new_recoveries(sird const &last_state) const {
		std::vector<float> new_r = std::vector<float>();
		for(int i = 0; i < n_age_segments(); i++) {
			float new_recovered_amount = last_state.infected[i] * recovery[i] * random();
			new_r.push_back(std::min(last_state.infected[i], new_recovered_amount));
		}
		return new_r;
	}

	[[nodiscard]] std::vector<float> new_deaths(sird const &last_state) const {
		std::vector<float> new_d = std::vector<float>();
		float total_infected = 0;

		// Apply the regular mortality rate
		for(int i = 0; i < n_age_segments(); i++) {
			total_infected += last_state.infected[i];
			new_d.push_back(last_state.infected[i] * mortality[i] * random());
		}

		// Increase mortality if too many people are infected
		if(total_infected > infected_capacity) {
			for(int i = 0; i < n_age_segments(); i++) {
				new_d[i] *= over_capacity_modifier;
			}
		}

		// Make sure there aren't more deaths than people currently infected
		for(int i = 0; i < n_age_segments(); i++) {
			if(new_d.at(i) > last_state.infected.at(i)) {
				new_d.at(i) = last_state.infected.at(i);
			}
		}
		return new_d;
	}

	[[nodiscard]] std::vector<float> find_virulence_factors(sird const &last_state) const {
		std::vector<float> virulence_factors = {};
		std::vector<float> mask_rates = find_mask_rates(last_state);
		std::vector<float> lockdown_factors = lockdown->new_lockdown_factors(last_state);

		for(int i = 0; i < n_age_segments(); i++) {
			float infected_count = last_state.infected[i] * (float)last_state.population;
			float mask_impact = (1.0 - mask_rates[i]) + (mask_rates[i] * mask_virulence_reduction);

			virulence_factors.push_back(infected_count * virulence[i]  * mask_impact * lockdown_factors[i]);
		}
		return virulence_factors;
	}

	[[nodiscard]] std::vector<float> find_susceptibility_factors(sird const &last_state) const {
		std::vector<float> susceptibility_factors = {};
		std::vector<float> mask_rates = find_mask_rates(last_state);

		for(int i = 0; i < n_age_segments(); i++) {
			float mask_impact = (1.0 - mask_rates[i]) + (mask_rates[i] * mask_susceptibility_reduction);
			float age_group_susceptibility_factor = susceptibility[i] * mask_impact;

			susceptibility_factors.push_back(age_group_susceptibility_factor);
		}
		return susceptibility_factors;
	}

	[[nodiscard]] std::vector<float> find_mobility_factors(mc const &cell_vicinity) const {
		std::vector<float> mobility_factors = {};

		for(int i = 0; i < n_age_segments(); i++) {
			mobility_factors.push_back(cell_vicinity.movement[i] * cell_vicinity.connection[i]);
		}
		return mobility_factors;
	}

	[[nodiscard]] std::vector<float> find_mask_rates(sird const &last_state) const {
			std::vector<float> mask_rates = std::vector<float>();
			float total_infected = 0;
			for(int i = 0; i < n_age_segments(); i++) {
				total_infected += last_state.infected[i];
			}
			// ^ the amount of infected people affects the likelihood of the population to wear masks

			for(int i = 0; i < n_age_segments(); i++) {
				double age_group_mask_rate = mask_use[i] * mask_adoption * total_infected;
				mask_rates.push_back(std::min(age_group_mask_rate, 1.0));
				// ^ no more than 100% of people can wear masks
			}
			return mask_rates;
	}
};

#endif //CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
