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
    std::vector<int> population;
    std::vector<float> susceptible;
    std::vector<float> infected;
    std::vector<float> recovered;
    std::vector<float> deceased;

    sird() : population({0}), susceptible({1}), infected({0}), recovered({0}), deceased({0}){}
    sird(std::vector<int> &pop, std::vector<float> &s, std::vector<float> &i, std::vector<float> &r, std::vector<float> &d) :
            population(pop), susceptible(s), infected(i), recovered(r), deceased(d){}

    template <typename T>
    static T sum_vector(std::vector<T> const &v) {
        T res = T();
        for (auto const &i: v)
            res += i;
        return res;
    }

    int total_population() const {
        int res = 0;
        for (auto const &p: population)
            res += p;
        return res;
    }

    float susceptible_ratio(int i) const {
        return susceptible[i] * population[i] / (float) total_population();
    }

    float infected_ratio(int i) const {
        return infected[i] * population[i] / (float) total_population();
    }

    float recovered_ratio(int i) const {
        return recovered[i] * population[i] / (float) total_population();
    }

    float deceased_ratio(int i) const {
        return deceased[i] * population[i] / (float) total_population();
    }

    float susceptible_ratio() const {
        float res;
        for (int i = 0; i < population.size(); i++) {
            res += susceptible_ratio(i);
        }
        return res;
    }

    float infected_ratio() const {
        float res;
        for (int i = 0; i < population.size(); i++) {
            res += infected_ratio(i);
        }
        return res;
    }

    float recovered_ratio() const {
        float res;
        for (int i = 0; i < population.size(); i++) {
            res += recovered_ratio(i);
        }
        return res;
    }

    float deceased_ratio() const {
        float res;
        for (int i = 0; i < population.size(); i++) {
            res += deceased_ratio(i);
        }
        return res;
    }
};

// Required for comparing states and detect any change
inline bool operator != (const sird &x, const sird &y) {
    return x.population != y.population || x.susceptible != y.susceptible || x.infected != y.infected ||
           x.recovered != y.recovered || x.deceased != y.deceased;
}

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const sird &x) {
    os << "<" << x.total_population();

    for (int i = 0; i < x.population.size(); i++) {
        os << "," << x.susceptible_ratio(i);
    }
    for (int i = 0; i < x.population.size(); i++) {
        os << "," << x.infected_ratio(i);
    }
    for (int i = 0; i < x.population.size(); i++) {
        os << "," << x.recovered_ratio(i);
    }
    for (int i = 0; i < x.population.size(); i++) {
        os << "," << x.deceased_ratio(i);
    }

    os << "," << x.susceptible_ratio() << "," << x.infected_ratio() << "," << x.recovered_ratio() << "," << x.deceased_ratio() << ">";
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
