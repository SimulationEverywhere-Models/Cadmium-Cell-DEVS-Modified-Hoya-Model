
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
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"]:
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 1
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Lockdown_SP_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"]:
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 2
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Lockdown_RC_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"]:
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 3
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Lockdown_RP_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        mask_use = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"]:
            mask_use.append(i/10)
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 0
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"] = mask_use
        #print("\n\n", temp_scenario)
        scenarios["Masks_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        mask_use = []
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"]:
            mask_use.append(i/10)
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 1
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"] = mask_use
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Masks_Lockdown_SP_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        mask_use = []
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"]:
            mask_use.append(i/10)
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 2
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"] = mask_use
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Masks_Lockdown_RC_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    for i in range(11):
        temp_scenario = copy.deepcopy(base_scenario)
        mask_use = []
        disobedience = []
        for age_group in temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"]:
            mask_use.append(i/10)
            disobedience.insert(0, (10-i)/10)
        disobedience[2:4] = [0.0, 0.0]
        temp_scenario["scenario"]["default_config"]["hoya_age"]["lockdown_type"] = 3
        temp_scenario["scenario"]["default_config"]["hoya_age"]["mask_use"] = mask_use
        temp_scenario["scenario"]["default_config"]["hoya_age"]["disobedience"] = disobedience
        #print("\n\n", temp_scenario)
        scenarios["Masks_Lockdown_RP_{0}".format(i*10)] = copy.deepcopy(temp_scenario)
        temp_scenario.clear()
    
    #print("\n\n", base_scenario)
    
    for scenario in scenarios:
        print(scenarios[scenario])
        write_new_json("./scenarios/" + scenario + ".json", scenarios[scenario])

