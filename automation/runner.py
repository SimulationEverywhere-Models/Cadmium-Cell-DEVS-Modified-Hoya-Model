
import os
import shutil
import subprocess

import notebook

def gather_scenarios(scenario_path = './scenarios'):
    """
    Looks at the scenario files and generates a dict that maps the names of the
    scenarios to the path of the corresponding scenario file
    
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
    Creates the necessary folders for each scenario's output
    
    output_path: string
        The path to the directory that the simulation results should be stored
        in
    scenarios: dict
        A dictionary mapping the names of scenarios to their filepaths
    
    raises: Exception
        If, when trying to prepare the output directory, a non-directory file
        exists with the name of a directory that needs to be created or accessed
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
    shutil.copy(os.path.join(results_path, 'hoya_age.json'), os.path.join(scenario_directory, 'simulation_logs', 'hoya_age.json'))
    shutil.copy(os.path.join(results_path, 'output_messages.txt'), os.path.join(scenario_directory, 'simulation_logs', 'output_messages.txt'))
    shutil.copy(os.path.join(results_path, 'state.txt'), os.path.join(scenario_directory, 'simulation_logs', 'state.txt'))    

def run_simulations(scenarios, executable_path = '..', output_path = './output', max_time = 1000):
    """
    Runs all of the simulations sequentially and stores the generated files
    in folders for each scenario
    
    scenarios: dict
        A dictionary mapping the names of scenarios to their filepaths
    executable_path: string
        The path to the executable that runs the simulation
    output_path:
        The path to the directory that the simulation results should be stored
        in 
    max_time:
        The maximum number of time steps the simulator will run before stopping
        early
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

