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
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using nlohmann::json;
using namespace cadmium::celldevs;

/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
struct sir {
    unsigned int population;
    std::vector<float> susceptible;
    std::vector<float> infected;
    std::vector<float> recovered;
    sir() : population(0), susceptible({1}), infected({0}), recovered({0}) {}  // a default constructor is required
    sir(unsigned int pop, std::vector<float> &s, std::vector<float> &i, std::vector<float> &r) : population(pop), susceptible(s), infected(i), recovered(r) {}

    template <typename T>
    static T sum_vector(std::vector<T> const &v) {
        T res = T();
        for (auto const &i: v)
            res += i;
        return res;
    }

    float susceptible_ratio() const {
        return sum_vector<float>(susceptible);
    }

    float infected_ratio() const {
        return sum_vector<float>(infected);
    }

    float recovered_ratio() const {
        return sum_vector<float>(recovered);
    }
};
// Required for comparing states and detect any change
inline bool operator != (const sir &x, const sir &y) {
    return x.population != y.population || x.susceptible != y.susceptible || x.infected != y.infected ||
           x.recovered != y.recovered;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const sir& lhs, const sir& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const sir &x) {
	float total_susceptible = 0;
	float total_infected = 0;
	float total_recovered = 0;
	
    os << "<" << x.population;
	
	for(int i = 0; i < x.susceptible.size(); i++) {
		total_susceptible += x.susceptible[i];
		os << "," << x.susceptible[i];
	}
	for(int i = 0; i < x.infected.size(); i++) {
		total_infected += x.infected[i];
		os << "," << x.infected[i];
	}
	for(int i = 0; i < x.recovered.size(); i++) {
		total_recovered += x.recovered[i];
		os << "," << x.recovered[i];
	}
	
	os << "," << total_susceptible << "," << total_infected << "," << total_recovered << ">";
    return os;
}

// Required for creating SIR objects from JSON file
void from_json(const json& j, sir &s) {
    j.at("population").get_to(s.population);
    j.at("susceptible").get_to(s.susceptible);
    j.at("infected").get_to(s.infected);
    j.at("recovered").get_to(s.recovered);
}

/************************************/
/*****COMPLEX VICINITY STRUCTURE*****/
/************************************/
struct mc {
    std::vector<float> connection;
    std::vector<float> movement;
    mc() : connection({ 0 }), movement({ 0 }) {}  // a default constructor is required
    mc(std::vector<float> &c, std::vector<float> &m) : connection(c), movement(m) {}
};
// Required for creating movement-connection objects from JSON file
void from_json(const json& j, mc &m) {
    j.at("connection").get_to(m.connection);
    j.at("movement").get_to(m.movement);
}

/************************************/
/******COMPLEX CONFIG STRUCTURE******/
/************************************/
struct config {
    std::vector<float> virulence;
    std::vector<float> recovery;
    std::vector<float> mask_use;
    float mask_reduction;
	float mask_adoption;
    float precision;
    config(): virulence({0.6}), recovery({0.4}), mask_use({1.0}), mask_reduction(0.5), mask_adoption(0.5), precision(100) {}
    config(std::vector<float> &v, std::vector<float> &r, std::vector<float> &mu, float &mr, float &ma, float p): virulence(v), recovery(r), mask_use(mu), mask_reduction(mr), mask_adoption(ma), precision(p) {}
};
void from_json(const json& j, config &v) {
    j.at("virulence").get_to(v.virulence);
    j.at("recovery").get_to(v.recovery);
    j.at("mask_use").get_to(v.mask_use);
    j.at("mask_reduction").get_to(v.mask_reduction);
    j.at("mask_adoption").get_to(v.mask_adoption);
    j.at("precision").get_to(v.precision);
}

template <typename T>
class hoya_cell : public grid_cell<T, sir, mc> {
public:
    using grid_cell<T, sir, mc>::cell_id;
    using grid_cell<T, sir, mc>::simulation_clock;
    using grid_cell<T, sir, mc>::state;
    using grid_cell<T, sir, mc>::map;
    using grid_cell<T, sir, mc>::neighbors;

    using config_type = config;  // IMPORTANT FOR THE JSON
    std::vector<float> virulence;
    std::vector<float> recovery;
    std::vector<float> mask_use;
    float mask_reduction;
    float mask_adoption;
    std::vector<float> age_ratio;
    float precision = 100;

    hoya_cell() : grid_cell<T, sir, mc>() {}

    hoya_cell(cell_position const &cell_id, cell_unordered<mc> const &neighborhood, sir &initial_state,
              cell_map<sir, mc> const &map_in, std::string const &delay_id, config &config) :
            grid_cell<T, sir, mc>(cell_id, neighborhood, initial_state, map_in, delay_id) {
        virulence = config.virulence;
        recovery = config.recovery;
        mask_use = config.mask_use;
        mask_reduction = config.mask_reduction;
        mask_adoption = config.mask_adoption;
        precision = config.precision;
        age_ratio = std::vector<float>();
        auto s = state.current_state;
        for (int i = 0; i < n_age_segments(); i++) {
            float ratio = std::round(precision * (s.susceptible[i] + s.infected[i] + s.recovered[i])) / precision;
            age_ratio.push_back(ratio);
        }
    }

    unsigned int inline n_age_segments() const {
        return virulence.size();
    }

    // user must define this function. It returns the next cell state and its corresponding timeout
    sir local_computation() const override {
        auto res = state.current_state;
        auto new_i = new_infections(res);
        auto new_r = new_recoveries(res);

        for (int i = 0; i < n_age_segments(); i++) {
            res.recovered[i] = std::round((res.recovered[i] + new_r[i]) * precision) / precision;
            res.infected[i] = std::round((res.infected[i] + new_i[i] - new_r[i]) * precision) / precision;
            res.susceptible[i] = age_ratio[i] - res.recovered[i] - res.infected[i];
        }
        return res;
    }
    // It returns the delay to communicate cell's new state.
    T output_delay(sir const &cell_state) const override {
        return 1;
    }

    std::vector<float> new_infections(sir const &last_state) const {
        std::vector<float> aux = std::vector<float>();
        for (int i = 0; i < n_age_segments(); i++) {
            aux.push_back(0);
        }
        for(auto neighbor: neighbors) {
            sir n = state.neighbors_state.at(neighbor);
            mc v = state.neighbors_vicinity.at(neighbor);
            float total_infected = n.infected_ratio();  // This is the sum of all the infected people in neighbor cell, regardless of age
			std::vector<float> mask_rates = new_mask_rates(last_state);
            for(int i = 0; i < n_age_segments(); i++) {
                aux[i] += total_infected * n.population * v.movement[i] * v.connection[i] * virulence[i] * (1.0 - mask_rates[i] + (mask_rates[i] * mask_reduction));
            }
        }
        std::vector<float> res = std::vector<float>();
        for (int i = 0; i < n_age_segments(); i++) {
            if (aux[i] > 0)
                aux[i] += 0;
            res.push_back(std::min(last_state.susceptible[i], last_state.susceptible[i] * aux[i] / (float)last_state.population));
        }
        return res;
    }

    std::vector<float> new_recoveries(sir const &last_state) const {
        std::vector<float> new_r = std::vector<float>();
        for(int i = 0; i < n_age_segments(); i++) {
            new_r.push_back(last_state.infected[i] * recovery[i]);
        }
        return new_r;
    }

    std::vector<float> new_mask_rates(sir const &last_state) const {
        std::vector<float> mask_rates = std::vector<float>();
		float total_infected = 0;
		
        for(int i = 0; i < n_age_segments(); i++) {
            total_infected += last_state.infected[i];
        }
		
		for(int i = 0; i < n_age_segments(); i++) {
			double age_group_mask_rate = mask_use[i] * mask_adoption * total_infected;
			mask_rates.push_back(std::min(age_group_mask_rate, 1.0));
			// ^ no more than 100% of people can wear masks
		}
		
        return mask_rates;
    }
};

#endif //CADMIUM_CELLDEVS_PANDEMIC_CELL_HPP
