#!/usr/bin/env python
# coding: utf-8

import os
import re
from collections import defaultdict
from copy import copy

import runner
import scenarios

all_scenarios = scenarios.scenario_list

for path in runner.run_sim_batched(all_scenarios, 1):
    with open(os.path.join(path, 'cout.log')) as cout_file, open(os.path.join(path, 'cerr.log')) as cerr_file, open(os.path.join(path, 'pandemic_state.txt')) as state_file, open(os.path.join(path, 'pandemic_outputs.txt')) as outputs_file, open(os.path.join(path, 'input.json')) as input_file:
        for state_line in state_file:
            print(state_line)
        
        log_filename = path + "/output_messages.txt"
        dim = 25,25
        patt_out_line = "\{\((?P<x>\w+),(?P<y>\w+)\).*<(?P<state>[\w,. -]+)>"
        
        COLOR_SUSCEPTIBLE = "#3498db"
        COLOR_INFECTED = "#e74c3c"
        COLOR_RECOVERED = "#2ecc71"
        COLOR_DECEASED = "#8ba2ad"
                
        def curr_states_to_df_row(sim_time, curr_states):
            sus_acc = 0
            inf_acc = 0
            rec_acc = 0
            dec_acc = 0
            for i in range(len(curr_states)):
                for j in range(len(curr_states[0])):
                    sus, inf, rec, dec = curr_states[i][j]
                    sus_acc += sus
                    inf_acc += inf
                    rec_acc += rec
                    dec_acc += dec
                    
                    assert 0.99 <= sus + inf + rec + dec < 1.01, (curr_time, i, j, sus + inf + rec + dec)
                    
            num_cells = len(curr_states) * len(curr_states[0])
            sus_acc /= num_cells
            inf_acc /= num_cells
            rec_acc /= num_cells
            dec_acc /= num_cells
            
            assert 0.999 <= sus_acc + inf_acc + rec_acc + dec_acc < 1.001, (curr_time, sus_acc + inf_acc + rec_acc + dec_acc)
            
            return [int(sim_time), sus_acc, inf_acc, rec_acc, dec_acc]
        
        curr_time = None
        curr_states = []
        
        for _ in range(dim[0]):
            curr_states.append([])
            for _ in range(dim[1]):
                curr_states[-1].append([1, 0, 0, 0])
        
                states = ["sus", "infec", "rec", "dec"]
                data = []
                curr_data = []
                
                with open(log_filename, "r") as log_file:
                    for line in log_file:
                        line = line.strip()
                        if line.isnumeric() and line != curr_time:
                            curr_time = line
                            data.append(curr_states_to_df_row(curr_time, curr_states))
                            continue
                
                        match = re.search(patt_out_line, line)
                        if not match:
                            print(line)
                            continue
                
                        x = int(match.group("x"))
                        y = int(match.group("y"))
                
                        state = list(map(float, match.group("state").split(",")[-4:]))
                        curr_states[x][y] = state
                        
                    data.append(curr_states_to_df_row(curr_time, curr_states))
        
        data[:5]
        
        
        # Visualization
        
        import pandas as pd
        import numpy as np
        import matplotlib.pyplot as plt
        import matplotlib
        
        font = {'family' : 'Times New Roman',
                'weight' : 'normal',
                'size'   : 20}
        
        matplotlib.rc('font', **font)
        
        columns = ["time", "susceptible", "infected", "recovered", "deceased"]
        df_vis = pd.DataFrame(data, columns=columns)
        df_vis = df_vis.set_index("time")
        df_vis.to_csv("states.csv")
        df_vis.head()
        
        base_name = path + "/" + os.path.splitext(os.path.basename(log_filename))[0]
        
        col_names = ["deceased", "infected", "recovered"]
        colors=[COLOR_DECEASED, COLOR_INFECTED, COLOR_RECOVERED]
        
        x = list(df_vis.index)
        y = np.vstack([df_vis[col] for col in col_names])
        
        fig, ax = plt.subplots(figsize=(15,6))
        ax.stackplot(x, y, labels=col_names, colors=colors)
        plt.legend(loc='upper right')
        plt.margins(0,0)
        plt.xlabel("Time (days)")
        plt.ylabel("Population (%)")
        plt.ylim((0,1))
        plt.savefig(base_name + "_area.png")
        
        fig, ax = plt.subplots(figsize=(15,6))
        linewidth = 2
        
        x = list(df_vis.index)
        ax.plot(x, df_vis["susceptible"], label="Susceptible", color=COLOR_SUSCEPTIBLE, linewidth=linewidth)
        ax.plot(x, df_vis["infected"], label="Infected", color=COLOR_INFECTED, linewidth=linewidth, linestyle="--")
        ax.plot(x, df_vis["recovered"], label="Recovered", color=COLOR_RECOVERED, linewidth=linewidth, linestyle="-.")
        ax.plot(x, df_vis["deceased"], label="Deceased", color=COLOR_DECEASED, linewidth=linewidth, linestyle=":")
        plt.legend(loc='upper right')
        plt.margins(0,0)
        plt.xlabel("Time (days)")
        plt.ylabel("Population (%)")
        plt.savefig(base_name + "_lines.png")
        
        fig, axs = plt.subplots(3, figsize=(15,8))
        linewidth = 2
        
        x = list(df_vis.index)
        axs[0].plot(x, df_vis["susceptible"], label="Susceptible", color=COLOR_SUSCEPTIBLE, linewidth=linewidth)
        axs[0].plot(x, df_vis["recovered"], label="Recovered", color=COLOR_RECOVERED, linewidth=linewidth, linestyle="-.")
        axs[0].set(ylabel='Population (%)')
        axs[0].legend()
        
        axs[1].plot(x, df_vis["infected"], label="Infected", color=COLOR_INFECTED, linewidth=linewidth, linestyle="--")
        axs[1].set(xlabel='Time (days)', ylabel='Population (%)')
        axs[1].legend()
        
        axs[2].plot(x, df_vis["deceased"], label="Deceased", color=COLOR_DECEASED, linewidth=linewidth, linestyle=":")
        axs[2].set(xlabel='Time (days)', ylabel='Population (%)\n')
        axs[2].legend()
        plt.savefig(base_name + "_lines_sep.png")
        
        df_vis.to_pickle(base_name + ".pickle")
        
