# -*- coding: utf-8 -*-
"""
Created on Tue May 11 17:20:15 2021

@author: Simon Rihm
"""

# Imports
import numpy as np
import pandas as pd

import ElementaryReaction as rxn
import IntermediateSpecies as spec
import ThermoLES as tls

import sys
#sys.path.append('../python helperFunctions')

import matlabFunctions as mlf

# Input file definition
pathString = r'E:\Simon\Documents\Dropbox (Cambridge CARES)\SRIHM WorkSpace\25_Modelling\CO2RR_MKM\18_AutoThermo\thermo_pre'
rxnFile = 'mechanism.csv'
specFile = 'species.csv'
statusFile = ''
rxnOutFile = 'mechanism2.csv'
specOutFile = 'species2.csv'

## arguments for calling this function via bash
argList = sys.argv.copy()
if len(argList) > 1:
    pathString = ""
    rxnFile = argList[1]
    if len(argList) > 2:
        specFile = argList[2]
    if len(argList) > 3:
        statusFile = argList[3]
    if len(argList) > 4:
        rxnOutFile = argList[4]
    if len(argList) > 5:
        specOutFile = argList[5]

givenList = []
solutionList = []

## importing all species
ourSpecies = pd.read_csv(mlf.fullfile([pathString,specFile]),index_col=0)
speciesList = []
    
for i in ourSpecies.index:
    speciesObj = spec.IntermediateSpecies(ourSpecies['name'].loc[i], x0=ourSpecies['x0'].loc[i], mu=ourSpecies['mu'].loc[i])
    speciesList.append(speciesObj)
    if ourSpecies['mu_given'].loc[i]:
        givenList.append(speciesObj)
        solutionList.append(ourSpecies['mu'].loc[i])


## importing all reactions
ourReactions = pd.read_csv(mlf.fullfile([pathString,rxnFile]),index_col=0)

rxnList = []
for i in ourReactions.index:
    rxnObj = rxn.ElementaryReaction()
    check = rxnObj.readData(ourReactions,i)
    if len(rxnObj.alt) > 0:
        rxnObj = next((x for x in rxnObj.alt if 'acid' in x.name), None)
    rxnList.append(rxnObj)        
    if ourReactions['dG_given'].loc[i]:
        givenList.append(rxnObj)
        solutionList.append(ourReactions['dG'].loc[i])

# SOLUTION for species
thermoLES = tls.createMatrix(givenList,speciesList)
   
thermoSol = np.array(solutionList)
thermoVec = np.linalg.solve(thermoLES,thermoSol)
thermoVec=thermoVec.round(3)

# SOLUTION for reactions
rxnLES = tls.createMatrix(rxnList,speciesList)
rxnSol = rxnLES.dot(thermoVec)

# Check reaction constraints
constraintsCheck = True
constraintsDiff = 0.0
targetDiff = 0.0
targetSquareDiff = 0.0
targetCounter = 0
constraintsCounter = 0

brokenRxnConstraints = []
ourReactions['dG']=rxnSol
ourReactions.to_csv(mlf.fullfile([pathString,rxnOutFile]))
for i in ourReactions.index:
    if  ourReactions['dG_lit'].loc[i] ==  ourReactions['dG_lit'].loc[i]:
        targetDiff += abs(ourReactions['dG'].loc[i] - ourReactions['dG_lit'].loc[i])
        targetSquareDiff += (abs(ourReactions['dG'].loc[i] - ourReactions['dG_lit'].loc[i]))**2
        targetCounter += 1
    if ourReactions['dG_min'].loc[i] < ourReactions['dG_max'].loc[i]:
        constraintsCounter  += 1
    if ourReactions['dG'].loc[i] < (ourReactions['dG_min'].loc[i] - 1e-3*abs(ourReactions['dG_min'].loc[i])):
        constraintsDiff += abs(ourReactions['dG'].loc[i]-ourReactions['dG_min'].loc[i])
        brokenRxnConstraints.append(i)
    elif ourReactions['dG'].loc[i] > (ourReactions['dG_max'].loc[i] + 1e-3*abs(ourReactions['dG_max'].loc[i])):
        constraintsDiff += abs(ourReactions['dG'].loc[i]-ourReactions['dG_max'].loc[i])
        brokenRxnConstraints.append(i)
            
brokenSpecConstraints = []
ourSpecies['mu'] = thermoVec
ourSpecies.to_csv(mlf.fullfile([pathString,specOutFile]))
for i in ourSpecies.index:
    if ourSpecies['mu_lit'].loc[i] == ourSpecies['mu_lit'].loc[i]:
        targetDiff += abs(ourSpecies['mu'].loc[i] - ourSpecies['mu_lit'].loc[i])
        targetSquareDiff += (abs(ourSpecies['mu'].loc[i] - ourSpecies['mu_lit'].loc[i]))**2
        targetCounter += 1
    if ourSpecies['min_mu'].loc[i] < ourSpecies['max_mu'].loc[i]:
        constraintsCounter  += 1
    if ourSpecies['mu'].loc[i] < (ourSpecies['min_mu'].loc[i] - 1e-3*abs(ourSpecies['min_mu'].loc[i])):
        constraintsDiff += abs(ourSpecies['mu'].loc[i]-ourSpecies['min_mu'].loc[i])
        brokenSpecConstraints.append(i)
    elif ourSpecies['mu'].loc[i] > (ourSpecies['max_mu'].loc[i] + 1e-3*abs(ourSpecies['max_mu'].loc[i])):
        constraintsDiff += abs(ourSpecies['mu'].loc[i]-ourSpecies['max_mu'].loc[i])
        brokenSpecConstraints.append(i)

avgTargetDiff = targetDiff / targetCounter
avgConstrDiff = constraintsDiff / constraintsCounter

if len(brokenRxnConstraints) + len(brokenSpecConstraints) > 0:
    constraintsCheck = False
    
if statusFile != '' and statusFile != 'exitValue':
    statusDf = pd.DataFrame(columns=('obj_fun','target_dist','target_dist_avg','constraint_check','constr_dist','constr_dist_avg','failed_reactions','failed_species','reactions_detail','species_detail'),\
                            data=([[targetSquareDiff,targetDiff,avgTargetDiff,int(constraintsCheck),constraintsDiff,avgConstrDiff,len(brokenRxnConstraints),len(brokenSpecConstraints),brokenRxnConstraints,brokenSpecConstraints]]))
    statusDf.to_csv(mlf.fullfile([pathString,statusFile]))

if statusFile == 'exitValue':
    sys.exit(len(brokenRxnConstraints) + len(brokenSpecConstraints))