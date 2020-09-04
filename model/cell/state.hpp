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

#ifndef PANDEMIC_HOYA_2002_STATE_HPP
#define PANDEMIC_HOYA_2002_STATE_HPP

#include <nlohmann/json.hpp>

struct sird {
    unsigned int population;
    unsigned int phase;
    std::vector<float> susceptible;
    std::vector<float> infected;
    std::vector<float> recovered;
    std::vector<float> deceased;

    sird() : population(0), susceptible({1}), infected({0}), recovered({0}), deceased({0}), phase(0) {}
    sird(unsigned int pop, std::vector<float> &s, std::vector<float> &i, std::vector<float> &r, std::vector<float> &d) :
            population(pop), susceptible(s), infected(i), recovered(r), deceased(d), phase(0) {}

    template <typename T>
    static T sum_vector(std::vector<T> const &v) {
        T res = T();
        for (auto const &i: v)
            res += i;
        return res;
    }

    [[maybe_unused]] [[nodiscard]] float susceptible_ratio() const {
        return sum_vector<float>(susceptible);
    }

    [[maybe_unused]] [[nodiscard]] float infected_ratio() const {
        return sum_vector<float>(infected);
    }

    [[maybe_unused]] [[nodiscard]] float recovered_ratio() const {
        return sum_vector<float>(recovered);
    }
};
// Required for comparing states and detect any change
inline bool operator != (const sird &x, const sird &y) {
    return x.population != y.population || x.susceptible != y.susceptible || x.infected != y.infected ||
           x.recovered != y.recovered || x.deceased != y.deceased;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const sird& lhs, const sird& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const sird &x) {
    float total_susceptible = 0.0;
    float total_infected = 0.0;
    float total_recovered = 0.0;
    float total_deceased = 0.0;

    os << "<" << x.population << "," << x.phase;

    for(float i : x.susceptible) {
        total_susceptible += i;
        os << "," << i;
    }
    for(float i : x.infected) {
        total_infected += i;
        os << "," << i;
    }
    for(float i : x.recovered) {
        total_recovered += i;
        os << "," << i;
    }
    for(float i : x.deceased) {
        total_deceased += i;
        os << "," << i;
    }

    os << "," << total_susceptible << "," << total_infected << "," << total_recovered << "," << total_deceased << ">";
    return os;
}

// Required for creating SIR objects from JSON file
[[maybe_unused]] void from_json(const nlohmann::json& j, sird &s) {
    j.at("population").get_to(s.population);
    j.at("susceptible").get_to(s.susceptible);
    j.at("infected").get_to(s.infected);
    j.at("recovered").get_to(s.recovered);
    j.at("deceased").get_to(s.deceased);
}

#endif //PANDEMIC_HOYA_2002_STATE_HPP
