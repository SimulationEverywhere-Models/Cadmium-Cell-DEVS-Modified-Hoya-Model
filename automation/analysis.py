
import os
import pandas

def main(output_directory):
    observations = {
        "infection_peak": None,
        "final_dead": None,
        "final_uninfected": None
    }
    
    df_vis = pandas.read_pickle(os.path.join(output_directory, "output_messages.pickle"))
    observations["infection_peak"] = df_vis["infected"].max()
    observations["final_dead"] = df_vis["deceased"].iloc[-1]
    observations["final_uninfected"] = df_vis["susceptible"].iloc[-1]
    
    return observations

if __name__ == "__main__":
    scenario_stats = dict()
    sim_log_path = "output"
    scenario_names = [f for f in os.listdir(sim_log_path) if not os.path.isfile(os.path.join(sim_log_path, f))]
    analysis = pandas.DataFrame([], columns = ["scenario", "infection_peak", "final_dead", "final_uninfected"])
    
    for scenario in scenario_names:
        scenario_observations = main(os.path.join(sim_log_path, scenario, "epidemic_graphs"))
        scenario_observations["scenario"] = scenario
        scenario_stats[scenario] = scenario_observations
        print(scenario_observations)
        analysis = analysis.append(scenario_observations, ignore_index=True)
    
    print(analysis)
    analysis.to_csv("simulation_set_analysis.csv", index = True)
    
