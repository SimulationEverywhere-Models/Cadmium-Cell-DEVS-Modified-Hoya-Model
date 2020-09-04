
# Automation tools for running simulation sets

## Preparing the scenarios
In order to run a set of simulations, you first need to set up the scenarios that the simulations will be configured to use.

You can set up your scenario files manually, but if you're planning to run a large number of simulations, it would probably be easier to write a script to set them up.

The `scenarios.py` script is what I modify every time I need to set up a new simulation set. It doesn't allow you to define your own simulations when you run it; you need to actually edit the source code in order to change the scenario files that it creates.

## Running the simulations
To actually run the simulations, you're going to have to run the `runner.py` script.

This script makes use of the compiled executable, so make sure that the executable is compiled and located at `../hoya.exe` relative to the runner script. If you have been working on other branches with adjustments to the source code, you should recompile to ensure that your executable is running the code for the current branch.

Please note that you may encounter issues when attempting to run the script if you compiled the executable on Windows with cygwin. If you get an error mentioning certain cygwin DLLs not being found, a solution I came up with is to copy the DLLs from cygwin's bin folder into the same directory as the executable.

After running this script, all of the outputs from the simulations should be in `./output/<scenario name>/simulation_logs` relative to the script.

## Generating the graphs
Generating graphs for each of the scenarios is done with the `notebook.py` script. The graphs and pickle file should appear in the `./output/<scenario name>/epidemic_graphs` directory relative to the script.

## Analyzing the data
The `analysis.py` script uses the pickle files generated by the `notebook.py` script. The analysis takes all of the pickle files and generates a CSV file where each scenario is listed alongside its simulation's infection peak, death count, and final uninfected amount.

As the nature of the analysis often depends on the specific study that the user wishes to conduct, the rest of the analysis is left up to the user. The results are left in a format that is easy to plot onto a graph on their own, and the analysis can show how each of the scenarios compares to the others.
