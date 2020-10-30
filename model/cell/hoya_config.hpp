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

#ifndef PANDEMIC_HOYA_2002_HOYA_CONFIG_HPP
#define PANDEMIC_HOYA_2002_HOYA_CONFIG_HPP

#include <nlohmann/json.hpp>

struct hoya_config {
    std::vector<float> susceptibility;
    std::vector<float> virulence;
    std::vector<float> recovery;
    std::vector<float> mortality;
    float precision;

    hoya_config(): susceptibility({1.0}), virulence({0.6}), recovery({0.4}), mortality({0.03}), precision(100) {}

    hoya_config(std::vector<float> &s, std::vector<float> &v,
                std::vector<float> &r, std::vector<float> &m, float &rao, float p):
            susceptibility(s), virulence(v), recovery(r), mortality(m), precision(p) {}
};

void from_json(const nlohmann::json& j, hoya_config &v) {
    j.at("susceptibility").get_to(v.susceptibility);
    j.at("virulence").get_to(v.virulence);
    j.at("recovery").get_to(v.recovery);
    j.at("mortality").get_to(v.mortality);
    j.at("precision").get_to(v.precision);
}

#endif //PANDEMIC_HOYA_2002_HOYA_CONFIG_HPP
