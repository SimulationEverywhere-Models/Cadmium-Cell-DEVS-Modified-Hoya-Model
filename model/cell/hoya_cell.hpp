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
#include "hoya_config.hpp"

using nlohmann::json;
using namespace cadmium::celldevs;


template <typename T>
class hoya_cell : public grid_cell<T, sird, mc> {
public:
    using grid_cell<T, sird, mc>::cell_id;
    using grid_cell<T, sird, mc>::simulation_clock;
    using grid_cell<T, sird, mc>::state;
    using grid_cell<T, sird, mc>::map;
    using grid_cell<T, sird, mc>::neighbors;

	std::vector<float> susceptibility;
	std::vector<float> virulence;
	std::vector<float> recovery;
	std::vector<float> mortality;

	float precision = 100;

	hoya_cell() : grid_cell<T, sird, mc>()  {}

	hoya_cell(cell_position const &cell_id, cell_unordered<mc> const &neighborhood, sird &initial_state,
              cell_map<sird, mc> const &map_in, std::string const &delay_id, hoya_config &config) :
			    grid_cell<T, sird, mc>(cell_id, neighborhood, initial_state, map_in, delay_id) {
		susceptibility = config.susceptibility;
		virulence = config.virulence;
		recovery = config.recovery;
		mortality = config.mortality;
		precision = config.precision;
		auto s = state.current_state;
	}

	unsigned int inline n_age_segments() const {
		return virulence.size();
	}

	// user must define this function. It returns the next cell state and its corresponding timeout
	[[nodiscard]] sird local_computation() const override {
		auto res = state.current_state;
		auto new_i = new_infections(res);
		auto new_r = new_recoveries(res);
		auto new_d = new_deaths(res);

		for (int i = 0; i < n_age_segments(); i++) {
			res.recovered[i] = std::round((res.recovered[i] + new_r[i]) * precision) / precision;
			res.deceased[i] = std::round((res.deceased[i] + new_d[i]) * precision) / precision;
			res.infected[i] = std::round((res.infected[i] + new_i[i] - (new_r[i] + new_d[i])) * precision) / precision;
			res.susceptible[i] = 1 - (res.recovered[i] + res.infected[i] + res.deceased[i]);
		}
		return res;
	}

	// It returns the delay to communicate cell's new state.
	T output_delay(sird const &cell_state) const override { return 1; }

	std::vector<float> new_infections(sird const &last_state) const {
        float n_effect = 0;
        for (auto neighbor: neighbors) {
            sird n_state = state.neighbors_state.at(neighbor);
            mc n_vicinity = state.neighbors_vicinity.at(neighbor);
            for (int k = 0; k < n_age_segments(); k++) {
                n_effect += n_state.population[k] * n_state.infected[k] * n_vicinity.connection
                            * n_vicinity.movement[k] * virulence[k];
            }
        }

        if (n_effect > 0) {
            int a = 1;
        }

        auto new_inf = std::vector<float>();
        for (int n = 0; n < n_age_segments(); n++) {
            float ratio = n_effect / (float) last_state.population[n];
            new_inf.push_back(last_state.susceptible[n] * susceptibility[n] * ratio);
        }
        return new_inf;
	}

	std::vector<float> new_recoveries(sird const &last_state) const {
		std::vector<float> new_r = std::vector<float>();
		for(int i = 0; i < n_age_segments(); i++) {
			new_r.push_back(last_state.infected[i] * recovery[i]);
		}
		return new_r;
	}

	std::vector<float> new_deaths(sird const &last_state) const {
		std::vector<float> new_d = std::vector<float>();
		for(int i = 0; i < n_age_segments(); i++) {
			new_d.push_back(last_state.infected[i] * mortality[i]);
		}
		return new_d;
	}
};

#endif //CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
