
###
"""
Steps:
1. read base scenario
2. copy into a dict
3. modify the dict for each variation of the simulation I need to run
4. write each dict into a new json file
"""

import json
import copy

def get_json(filename):
    with open(filename) as file:
        data = json.load(file)
    return data

def write_new_json(filename, data):
    with open(filename, 'w') as file:
        json.dump(data, file)

if __name__ == "__main__":
    scenarios = dict()
    base_scenario = get_json("./scenarios/base_scenario.json")
    #print("\n\n", base_scenario)
    
    for i in range(10):
        temp_scenario = copy.deepcopy(base_scenario)
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_virulence_reduction"] = i/10
        #print("\n\n", temp_scenario)
        scenarios["surgical_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(10):
        temp_scenario = copy.deepcopy(base_scenario)
        mask_use = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"]:
            mask_use.append(1-(i/10))
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"] = mask_use
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_susceptibility_reduction"] = 0
        #print("\n\n", temp_scenario)
        scenarios["ffp2_{0}".format(100-(i*10))] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    #print("\n\n", base_scenario)
    
    for scenario in scenarios:
        print(scenarios[scenario])
        write_new_json("./scenarios/" + scenario + ".json", scenarios[scenario])

