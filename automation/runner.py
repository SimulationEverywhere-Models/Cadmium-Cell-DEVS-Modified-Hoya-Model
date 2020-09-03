
import os
import shutil
import subprocess

import notebook

def gather_scenarios(scenario_path = './scenarios'):
    """
    scenario_path: string
        The path to the directory containing the scenario files
    
    returns: dict
        A dictionary mapping the names of the scenarios to their filepath
    """
    
    filenames = os.listdir(scenario_path)
    filenames = filter(lambda item: item[-5:]=='.json', filenames)
    scenarios = dict()
    
    for file in filenames:
        scenarios[file.split('.')[0]] = os.path.join(scenario_path, file)
    
    return scenarios

def _make_directory(directory_name):
    try:
        os.mkdir(directory_name)
    except Exception as e:
        if os.path.isdir(directory_name):
            print("Directory {dirname} already exists. ".format(dirname = directory_name))
        else:
            print("File exists at {dirname} that is not a directory. ".format(dirname = directory_name))
            raise e    

def prepare_output_directory(scenarios, output_path = './output'):
    """
    output_path:
    scenarios:
    
    raises:
    
    returns:
    """
    _make_directory(output_path)
    
    for name in scenarios:
        scenario_directory = os.path.join(output_path, name)
        
        _make_directory(scenario_directory)
        
        shutil.copy(scenarios[name], scenario_directory)
        
        _make_directory(os.path.join(scenario_directory, 'epidemic_graphs'))
        _make_directory(os.path.join(scenario_directory, 'images_and_video'))
        _make_directory(os.path.join(scenario_directory, 'simulation_logs'))
        _make_directory(os.path.join(scenario_directory, 'visualization_files'))

def _move_simulation_results(scenario_directory, results_path = '../simulation_results/pandemic_hoya_age_json'):
    """
    scenario_directory:
    results_path:
    """
    shutil.copy(os.path.join(results_path, 'hoya_age.json'), os.path.join(scenario_directory, 'simulation_logs', 'hoya_age.json'))
    shutil.copy(os.path.join(results_path, 'output_messages.txt'), os.path.join(scenario_directory, 'simulation_logs', 'output_messages.txt'))
    shutil.copy(os.path.join(results_path, 'state.txt'), os.path.join(scenario_directory, 'simulation_logs', 'state.txt'))    

def run_simulations(scenarios, executable_path = '..', output_path = './output', max_time = 1000):
    """
    scenarios:
    executable_path:
    output_path:
    max_time:
    """
    #executable_path = os.path.abspath(executable_path)
    results_path = "../simulation_results/pandemic_hoya_age_json"
    for name in scenarios:
        scenario_directory = os.path.join(output_path, name)
        
        # Run the simulation
        command = "{exe_path} {scenario_path} {max_time}".format(exe_path = os.path.join(executable_path, 'hoya.exe'), scenario_path = scenarios[name], max_time = max_time)
        #command = "{exe_path}".format(exe_path = os.path.join(executable_path, 'hoya.exe'))
        print(command)
        command_return = os.system(command)
        
        #command = []
        #command.append('"{0}"'.format(os.path.join(executable_path, 'hoya.exe')))
        #command.append(os.path.abspath(scenarios[name]))
        #command.append(str(max_time))
        #print(command)
        #command_return = subprocess.run(command, capture_output = True)
        
        print(command_return)
        
        _move_simulation_results(scenario_directory, results_path)
        
        #current_dir = os.getcwd()
        #os.chdir(os.path.join(scenario_directory, "epidemic_graphs"))
        ### BEGIN OPERATING IN OTHER DIRECTORY ##
        #notebook.main("../simulation_logs")
        ### FINISH OPERATING IN OTHER DIRECTORY ##
        #os.chdir(current_dir)

if __name__ == "__main__":
    scenarios = gather_scenarios()
    print(scenarios)
    prepare_output_directory(scenarios)
    run_simulations(scenarios)

