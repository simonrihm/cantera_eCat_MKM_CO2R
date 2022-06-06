# cantera_eCat_MKM_CO2R
Microkinetic Modelling of electrocatalytic CO2 reduction by using Cantera libraries

# Input File Manual
There are two main input files needed for executing the C++ code: a mechanism file (XML) and an option-defining file (txt).

## Mechanism File Input
The main C++ executable that runs the MKM takes a XML input file, specifying species, phases and reaction steps. This XML file is created by first generating a CTI input file that is then converted via the ctml_writer.py
This CTI file is created via Python script "mechGen/py". It takes CSV input files with reactions, phases and species.

### Mechanism Input
- “id” will become reaction name (with suffix in case of hydrogenation reaction)
- “ed1”, “ed2”, “prod1”, “prod2”, “PT”, “ET” declare chemical equation as in database
-	“reverse” indicates if reaction is bidirectional (allow products to educts)
-	“A”, “E_act”, “beta” give kinetic parameters for Arrhenius, barrier and charge transfer
-	“Tafel”, “alkaline”, “acidic” indicate different types allowed for a hydrogenation reaction. Based on this, up to 3 reactions are created per row. Their kinetics differ in:
    - Pre-exponential factor by a factor of K (“tafelK”, “acidK”, “alkaK”)
    - Activation barrier by either completely different number (“tafelE”, “acidE”, “alkaE”) or an offset/difference to the original value (“tafelDE”, “acidDE”, “alkaDE”)

### Species Input
-	“name” that has to line up with the species names given in mechanism, plus certain standard species that are implied: “e-“, “H+”, “*H”, “H2O”, “OH-“, “N2”
-	“phase” is the name of the phase this species is part of that has to match with input below
-	“mu [eV]” is the generated chemical potential for the species
-	“x0 [-]” is the initial molar fraction or coverage of the species in its phase

### Phases Input
-	“name” of the phase that has to line up with the values given for the species in this phase
-	“type” of the phase model for Cantera input file
-	“add1”, “add2”, “add3” are additional lines for the Cantera input file that have to be specified for this particular phase

## Option File Input
Options set in the option input file are grouped into different aspects. An option is generally set by a single line in the .txt file that goes “[option name]=[option value]” without any additional spaces. Lines that begin with “##” are ignored and serve readability. Only a few of the options below must be set for a simulation, most have a default value that proves sufficient in most cases.

### Input File
These options manage the additional input files with all necessary parameters for the simulation.
-	“xml” (in Code ioManager::xml)
    - *Absolute (or relative) path to XML file that defines phases, species and reactions*
    -	For VisualStudio version: path relative to project folder
    -	For released version: path relative to folder with .exe
-	“csv” (in Code ioManager::csv)
    - *Absolute (or relative) path to CSV file with which variables can be defined from an external source (for example within MoDS environment)
      - see “var” and “phi”*
    - For VisualStudio version: path relative to project folder
    - For released version: path relative to folder with .exe

### Parameter Variations
These options manage the entirety of simulations (“steps”) to be carried out via this input file.
- “var” (in Code simManager::variable)
  - *Multi-purpose variable that can be set to change across the different*
  - const(1.3): Set variable to a constant value for all steps, in this case 1.3
  - csv(t,2): Set variable to a constant value for all steps that is defined in above mentioned “csv” file, for this case in column “t” (in 0th row of CSV) and row nr. 2
    - Row number (second argument) can be set negative (e.g. “-1” for the last row)
  - lin(0,-2): Set variable to a function that changes its value from in this case 0.0 to -2.0 in a linear fashion going from simulation step 1 to step n+1  see “steps”
- “steps” (in Code simManager::i_max)
  - Nr. of simulations (minus 1) to be carried out over which “var” can change
  - Control variable i goes from 0 to i_max in “simManager”

### Single Simulation
These options manage each specific simulation (“step”) to be carried out via solver (“timesteps”).
- “phi” (in Code simManager::potential)
  - *Potential vs. RHE (in Volt) applied to the catalyst (solid phase) for this simulation*
  - const(1.3): Set potential to a constant value for all t, in this case 1.3
  - csv(phi2): Set potential to a constant value for all t that is defined in above mentioned “csv” file, for this case in column “t” (in 0th row of CSV) and row nr. 2
  - lin(0,-2): Set potential to a function that changes value from in this case 0.0 to -2.0  linearly from time t=0s to t=t_end over nr. of steps 
    - see “t_end” and “dt_max”
  - var: constant value the variable defined in input file (see above) takes for this step
  - csv(phi,t): Tabulated potential as phi=f(t) in the given .csv file
    - intermediate values are calculated via linear interpolation
- “t_end” (in Code simManager::endTime)
  - *Simulated duration of the mechanism for this step*
    - time variable goes from 0 to dt_max in “Cat_reactor”
  - A value of -1 indicates a steady state simulation that only terminates after certain steady-state criteria are met – independent of simulated duration
- “dt_max” (in Code simManager::maxTimeStep)
  - *Largest allowed time step that is passed to the solver / integrator*
    - smaller steps are taken internally if necessary
  - var: constant value the variable defined in input file (see above) takes for this step
- “dt_min” (in Code simManager::minTimeStep)
  - *Minimum allowed time step that is passed to solver / integrator*
    - If no convergence is reached for this, the simulation terminates.
- “dt_init” (in Code simManager::initTimeStep)
  - * Initial time step that is passed to solver / integrator (for t=0)*
    - If no convergence is reached for this, the simulation terminates.
- “solv_atol” (in Code Cat_reactor::absTol)
    - *Absolute tolerance passed to CVodes solver*
- “solv_rtol” (in Code Cat_reactor::relTol)
    - *Relative tolerance passed to CVodes solver*

### Final Criteria
These options manage criteria that can influence the final values of a transient simulation.
-	“ss_crit” (in Code Cat_reactor::ssCrit)
    - *Criterion for assessing if steady state is reached at given time of simulation*
    - *Threshold for di/dt AND 1-sum(FE) to fall below in order to trigger steady state*
    - influences simulation time for SS (see t_end) and SS output (see changerate) 
-	“t_avg” (in Code Cat_reactor::t_avg)
    - *Duration before the end of the simulation over which to average results*
    - *Cumulated datapoints are now calculated from multiple transient datapoints*
      - applicable for transient sim. where cumulated output is generated (see “mode”)
- “n_avg” (in Code Cat_reactor::n_avg)
    - *Number of data points at the end of the simulation over which to average results*
    - *Cumulated datapoints are now calculated from multiple transient datapoints*
      - *applicable for transient sim. where cumulated output is generated (see “mode”)*

### General Options
These options manage how, when, and where simulation results are calculated and written.
-	“output” (in Code ioManager::outputPath)
    - *Path (string) to which resulting CSV files are saved to*
    - “_” if output is to be saved in the origin folder
- “interaction” (in Code ioManager::requireInteraction)
    - *This setting only applies to the Windows version (Linux default is NO)*
    - YES (true): user has to press button in order to end simulations
    - NO (false): no user input required
- “products” (in Code ioManager::productNames)
    - *Comma-separated list of species (vector) that are to be considered as products*
    - *If options are selected, current densities and FE of these are calculated*
    - *First species in the list is always the reference for FE, i.e. “e-“*

### Output Options
These Options manage how the simulation results that are written to the output files: File name, frequency and calculated values of output. 
-	“mode” (in Code ioManager::fileOutputOpts)
    -	ALL (OUTPUT_ALL): generate files for every simulation and cumulated
    -	CUMULATED (OUTPUT_CUMULATED): only generate single output file that cumulates the last timestep of every single simulation done in this run
    -	TRANSIENT (OUTPUT_TRANSIENT): generate output file for every single simulation with each time step written, but no cumulated results
    -	NONE (OUTPUT_NONE): generate no output files
-	“filename” (in Code ioManager::nameOutputOpts)
    -	TIMESTAMP (FILENAME_TIMESTAMP): Generate output filename w/ timestamp
    -	OUTPUT (FILENAME_OUTPUT): Default output filename, so it gets replaced!
-	“species” (in Code ioManager::specOutputOpts)
    -	ALL (SPECIES_ALL): output net production rates, concentrations and activities for each species
    -	RATES (SPECIES_RATES): output only net production rates for each species
    -	NONE (SPECIES_NONE): no output for each species
-	“reactions” (in Code ioManager::rxnOutputOpts)
    -	ALL (REACTION_ALL): output forward/reverse reaction rate constants, forward/reverse reaction rates and net reaction rates for each reaction
    -	RATES (REACTION_RATES): output forward, reverse and net reaction rates
    -	NET (REACTION_NET): Only output net reaction rates for each reaction
    -	NONE (REACTION_NONE): No output for each reaction
-	“coverage” (in Code ioManager::covOutputOpts)
    - YES (COVERAGE_ON): output coverage of each surface species
    -	NO (COVERAGE_OFF): no special output for each surface species
-	“current” (in Code ioManager::currOutputOpts)
    -	ALL (CURRENT_ALL): output current densities and Faradaic Efficiencies for products
    -	FE (CURRENT_FE): output only current density for reference and FE for the rest
    -	SPEC (CURRENT_SPEC): output only specific current densities for all products
    -	NONE (CURRENT_NONE): no special output for each product
-	“sampling” (in Code ioManager::sampleOutputOpts)
    -	T10 (SAMPLING_T10): only print output results of every 10 seconds (roughly)
      -	this only works for transient output with set t_end (no steady-state)
    -	NONE (SAMPLING_NONE): no sampling, outputs are printed for all steps
- “changerate” (in Code ioManager::ddtOutputOpts)
    - ALL (DDT_ALL): output relative rate of change for total current density, sum of Faradaic Efficiencies for all products, rates of change for all product FE, Faradaic Efficiencies for all intermediates and time when steady state was reached (-1 if not)
    -	PROD (DDT_PROD): output relative rate of change for total current density, sum of Faradaic Efficiencies for all products, rates of change for all product FE, ss time
    -	SS (DDT_SS): output relative di/dt, sum(FE) and steady state time
    -	NONE (DDT_NONE): no outputs regarding rates of change or steady state
-	“logging” (in Code ioManager::logOutputOpts)
    -	ALL (LOG_ALL): Copy of mechanism.xml as well as simulation log and input.txt are saved to output folder with a timestamp in their name for later reference
    -	INPUT (LOG_INPUT): Mechanism.xml is copied to output folder with timestamp
    -	TXT (LOG_TXT): Input.txt and simulation log are saved to file with timestamp
    -	NONE (LOG_NONE): Log only displayed in consol during runtime, no input file copies
    -	SILENT (LOG_SILENT): No log files are created, no log is displayed

# Software usage manual

## Simulation Software (C++)
Cantera_Catalyst.exe can be started via command line. It takes two different arguments:
- defined option input file path/name (default: “input.txt”)
- executable batch script, for example to create CTI/XML files from CSV via pre-processing scripts (default: “runBefore.bat”)


## Pre-processing Software (Python)



## Post-processing software (Matlab)


