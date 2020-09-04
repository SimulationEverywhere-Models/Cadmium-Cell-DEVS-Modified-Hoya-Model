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

#ifndef PANDEMIC_HOYA_2002_LOCKDOWN_HPP
#define PANDEMIC_HOYA_2002_LOCKDOWN_HPP

class Lockdown {
public:
    virtual ~Lockdown() = default;
    [[nodiscard]] virtual std::vector<float> new_lockdown_factors(sird const &last_state) const { return {}; };
    [[nodiscard]] virtual unsigned int next_phase(int simulation_clock, sird const &last_state) const { return 0; };
};

class NoLockdown: public Lockdown {
public:
    NoLockdown() = default;

    [[nodiscard]] std::vector<float> new_lockdown_factors(sird const &last_state) const override {
        return std::vector<float>(last_state.susceptible.size(), 1); // Movement is not limited (1x normal)
    }

    [[nodiscard]] unsigned int next_phase(int simulation_clock, sird const &last_state) const override {
        return 0;
    }
};

class ScheduledPhaseLockdown: public Lockdown {
    const std::vector<std::vector<float>> lockdown_rates;
    const std::vector<int> phase_durations;
    const std::vector<float> disobedience;
    int days_sum;

public:
    ScheduledPhaseLockdown(std::vector<std::vector<float>> &lr, std::vector<int> &pd, std::vector<float> &d):
        lockdown_rates(lr), phase_durations(pd), disobedience(d) {
        days_sum = 0;
        for(int phase_duration: phase_durations) {
            days_sum += phase_duration;
        }
    }

    [[nodiscard]] std::vector<float> new_lockdown_factors(sird const &last_state) const override {
        std::vector<float> lockdown_factors = {};
        for(int i = 0; i < last_state.infected.size(); i++) {
            double age_group_lockdown_factor = disobedience.at(i)
                    + (1.0 - disobedience.at(i)) * lockdown_rates.at(last_state.phase).at(i);
            lockdown_factors.push_back(age_group_lockdown_factor);
        }
        return lockdown_factors;
    }

    [[nodiscard]] unsigned int next_phase(int simulation_clock, sird const &last_state) const override {
        int aux = simulation_clock % days_sum;
        int i = 0;
        while (aux >= phase_durations.at(i)) {
            aux -= phase_durations.at(i++);
        }
        return i;
    }
};

class ReactionContinuousLockdown: public Lockdown {
    const std::vector<std::vector<float>> lockdown_rates;
    const float lockdown_adoption;
    const std::vector<float> disobedience;

public:
    ReactionContinuousLockdown(std::vector<std::vector<float>> &lr, float &la, std::vector<float> &d):
        lockdown_rates(lr), lockdown_adoption(la), disobedience(d) {}

    [[nodiscard]] std::vector<float> new_lockdown_factors(sird const &last_state) const override {
        std::vector<float> lockdown_factors = {};
        float total_infected = last_state.infected_ratio();
        double lockdown_strength;
        double age_group_lockdown_factor;

        for(int i = 0; i < last_state.infected.size(); i++) {
            lockdown_strength = 1.0 - (lockdown_adoption * total_infected);
            age_group_lockdown_factor = disobedience.at(i)
                    + (1.0 - disobedience.at(i)) * lockdown_strength * lockdown_rates.at(last_state.phase).at(i);
            lockdown_factors.push_back(std::max(age_group_lockdown_factor, 0.0));
        }
        return lockdown_factors;
    }

    [[nodiscard]] unsigned int next_phase(int simulation_clock, sird const &last_state) const override { return 0; }
};


class ReactionPhaseLockdown: public Lockdown {
    const std::vector<std::vector<float>> lockdown_rates;
    const std::vector<float> phase_thresholds;
    const std::vector<float> threshold_buffers;
    const std::vector<float> disobedience;

    [[nodiscard]] bool shouldGoToNextPhase(sird const &last_state) const {
        return (last_state.phase + 1 < phase_thresholds.size()
            && last_state.infected_ratio() >= phase_thresholds[last_state.phase + 1]);
    }

    [[nodiscard]] bool shouldGoToPreviousPhase(sird const &last_state) const {
        return (last_state.phase > 0
            && (last_state.infected_ratio() + threshold_buffers[last_state.phase]) < phase_thresholds[last_state.phase]);
    }

public:
    ReactionPhaseLockdown(std::vector<std::vector<float>> &lr, std::vector<float> &pt, std::vector<float> &tb,
                          std::vector<float> &d): lockdown_rates(lr), phase_thresholds(pt), threshold_buffers(tb), disobedience(d) {}

    [[nodiscard]] std::vector<float> new_lockdown_factors(sird const &last_state) const override {
        std::vector<float> lockdown_factors = {};
        for(int i = 0; i < last_state.infected.size(); i++) {
            double age_group_lockdown_factor = disobedience.at(i)
                    + (1 - disobedience.at(i)) * lockdown_rates.at(last_state.phase).at(i);
            lockdown_factors.push_back(age_group_lockdown_factor);
        }
        return lockdown_factors;
    }

    [[nodiscard]] unsigned int next_phase(int simulation_clock, sird const &last_state) const override {
        unsigned int temp_phase = last_state.phase;
        if(shouldGoToNextPhase(last_state)) {
            temp_phase++;
        } else if (shouldGoToPreviousPhase(last_state)) {
            temp_phase--;
        }
        return temp_phase;
    }
};

#endif //PANDEMIC_HOYA_2002_LOCKDOWN_HPP
