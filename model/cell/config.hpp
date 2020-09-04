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

#ifndef PANDEMIC_HOYA_2002_CONFIG_HPP
#define PANDEMIC_HOYA_2002_CONFIG_HPP

#include <nlohmann/json.hpp>

struct config {
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
    float precision;

    unsigned int lockdown_type;
    std::vector<std::vector<float>> lockdown_rates;
    std::vector<int> phase_durations;
    float lockdown_adoption;
    std::vector<float> phase_thresholds;
    std::vector<float> threshold_buffers;
    std::vector<float> disobedience;
	
	unsigned int rand_type;
	float rand_mean;
	float rand_stddev;
	float rand_upper;
	float rand_lower;
	float rand_avg_occurence_rate;
	float rand_seed;

    config(): susceptibility({1.0}), virulence({0.6}), recovery({0.4}), mortality({0.03}), infected_capacity(0.1),
              over_capacity_modifier(1.5), mask_use({1.0}), mask_susceptibility_reduction(0.5),
              mask_virulence_reduction(0.5), mask_adoption(0.5), lockdown_type(0), lockdown_rates({{0.0}}),
              disobedience({0.0}), phase_durations({1}), lockdown_adoption(0.0), phase_thresholds({0.0}),
              threshold_buffers({0.0}), rand_type(0), rand_seed(0.0), rand_mean(1.0), rand_stddev(0.5), rand_upper(1.5),
			  rand_lower(0.5), rand_avg_occurence_rate(5.0), precision(100) {}


    [[maybe_unused]] config(std::vector<float> &s, std::vector<float> &v, std::vector<float> &r, std::vector<float> &m,
                            float &c, float &oc, std::vector<float> &mu, float &msr, float &mvr, float &ma,
                            unsigned int &lt, std::vector<std::vector<float>> &lr, std::vector<float> &d,
                            std::vector<int> &pd, float &la, std::vector<float> &pt, std::vector<float> &tb,
							unsigned int &rt, float &rs, float &rm, float &rsd, float &ru, float &rl, float &rao, float p):
            susceptibility(s), virulence(v), recovery(r), mortality(m), infected_capacity(c), over_capacity_modifier(oc),
            mask_use(mu), mask_susceptibility_reduction(msr), mask_virulence_reduction(mvr), mask_adoption(ma),
            lockdown_type(lt), lockdown_rates(lr), disobedience(d), phase_durations(pd), lockdown_adoption(la),
            phase_thresholds(pt), threshold_buffers(tb), rand_type(rt), rand_seed(rs), rand_mean(rm), rand_stddev(rsd), rand_upper(ru),
			rand_lower(rl), rand_avg_occurence_rate(rao), precision(p) {}
};

[[maybe_unused]] void from_json(const nlohmann::json& j, config &v) {
    j.at("susceptibility").get_to(v.susceptibility);
    j.at("virulence").get_to(v.virulence);
    j.at("recovery").get_to(v.recovery);
    j.at("mortality").get_to(v.mortality);
    j.at("infected_capacity").get_to(v.infected_capacity);
    j.at("over_capacity_modifier").get_to(v.over_capacity_modifier);
    j.at("mask_use").get_to(v.mask_use);
    j.at("mask_susceptibility_reduction").get_to(v.mask_susceptibility_reduction);
    j.at("mask_virulence_reduction").get_to(v.mask_virulence_reduction);
    j.at("mask_adoption").get_to(v.mask_adoption);
    j.at("precision").get_to(v.precision);

    j.at("lockdown_type").get_to(v.lockdown_type);
    switch(v.lockdown_type) {
        case 1: // lockdown type: scheduled lockdown in phases
            j.at("lockdown_rates").get_to(v.lockdown_rates);
            j.at("phase_durations").get_to(v.phase_durations);
            j.at("disobedience").get_to(v.disobedience);
            break;
        case 2: // lockdown type: continuous reaction to infected
            j.at("lockdown_rates").get_to(v.lockdown_rates);
            j.at("lockdown_adoption").get_to(v.lockdown_adoption);
            j.at("disobedience").get_to(v.disobedience);
            break;
        case 3: // lockdown type: reaction to infected in phases
            j.at("lockdown_rates").get_to(v.lockdown_rates);
            j.at("phase_thresholds").get_to(v.phase_thresholds);
            j.at("threshold_buffers").get_to(v.threshold_buffers);
            j.at("disobedience").get_to(v.disobedience);
            break;
        default: // lockdown type: no response
            v.lockdown_type = 0;
    }
	
    j.at("rand_type").get_to(v.rand_type);
    switch(v.rand_type) {
		case 1: // normal distribution
            j.at("rand_seed").get_to(v.rand_seed);
            j.at("rand_mean").get_to(v.rand_mean); // consider removing since the value *should* always be 1
            j.at("rand_stddev").get_to(v.rand_stddev);
			break;
		case 2: // uniform distribution
            j.at("rand_seed").get_to(v.rand_seed);
            j.at("rand_upper").get_to(v.rand_upper);
            j.at("rand_lower").get_to(v.rand_lower);
			break;
		case 3: // exponential distribution
            j.at("rand_seed").get_to(v.rand_seed);
            j.at("rand_avg_occurence_rate").get_to(v.rand_avg_occurence_rate);
			break;
		default: // static (not random)
			v.rand_type = 0;
	}
	
}

#endif //PANDEMIC_HOYA_2002_CONFIG_HPP
