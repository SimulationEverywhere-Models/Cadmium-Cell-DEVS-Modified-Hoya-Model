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

#ifndef CADMIUM_CELLDEVS_HOYA_COUPLED_HPP
#define CADMIUM_CELLDEVS_HOYA_COUPLED_HPP

#include <nlohmann/json.hpp>
#include <cadmium/celldevs/coupled/grid_coupled.hpp>
#include "cell/hoya_cell.hpp"

template <typename T>
class hoya_coupled : public cadmium::celldevs::grid_coupled<T, sird, mc> {
public:

    explicit hoya_coupled(std::string const &id) : grid_coupled<T, sird, mc>(id){}

    template <typename X>
    using cell_unordered = std::unordered_map<std::string,X>;

    void add_grid_cell_json(std::string const &cell_type, cell_map<sird, mc> &map, std::string const &delay_id,
                            nlohmann::json const &config) override {
        if (cell_type == "hoya_age") {
            auto conf = config.get<typename hoya_cell<T>::config_type>();
            this->template add_cell<hoya_cell>(map, delay_id, conf);
        } else throw std::bad_typeid();
    }
};

#endif //CADMIUM_CELLDEVS_HOYA_COUPLED_HPP
