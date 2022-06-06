# -*- coding: utf-8 -*-
"""
Created on Thu Mar 18 15:16:23 2021

@author: Simon Rihm
"""

import pandas as pd
import ElementaryReaction as rxn
import sys
import IntermediateSpecies as spec
import matlabFunctions as mlf

pathString = r'D:\Dropbox (Cambridge CARES)\SRIHM WorkSpace\25_Modelling\CO2RR_MKM\18_AutoThermo\FullSets\A5-O1b'
#pathString = 'E:/Simon/Documents/Dropbox (Cambridge CARES)/SRIHM WorkSpace/25_Modelling/Reaction mechanism/Mechanism Generation/2021-04-20_new/'
rxnFile = 'mechanism.csv'
specFile = 'species.csv'
phaseFile = 'phases.csv'
templateFile = 'template.txt'
mechanismFile = 'mechanism.cti'
writeFlag = True
thermoFlag = False
phaseNames=['gas','liquid','solid','surface']

## arguments for calling this function via bash
argList = sys.argv.copy()
if len(argList) > 1:
    pathString = ""
    rxnFile = argList[1]
    if len(argList) > 2:
        specFile = argList[2]
    if len(argList) > 3:
        phaseFile = argList[3]
    if len(argList) > 4:
        mechanismFile = argList[4]
    if len(argList) > 5:
        templateFile = argList[5]

## importing all phases
ourPhases = pd.read_csv(mlf.fullfile([pathString,phaseFile]),index_col=0)
phaseList = []
for i in ourPhases.index:
    phaseObj = spec.SpeciesPhases(name=ourPhases['name'].loc[i],phaseType=ourPhases['type'].loc[i],
                                  add1=ourPhases['add1'].loc[i],add2=ourPhases['add2'].loc[i],add3=ourPhases['add3'].loc[i])
    phaseList.append(phaseObj)

## importing all species
ourSpecies = pd.read_csv(mlf.fullfile([pathString,specFile]),index_col=0)
speciesList = []
    
for i in ourSpecies.index:
    speciesObj = spec.IntermediateSpecies(ourSpecies['name'].loc[i], x0=ourSpecies['x0'].loc[i], mu=ourSpecies['mu'].loc[i])
    speciesList.append(speciesObj)
    for phase in phaseList:
        if phase.name==ourSpecies['phase'].loc[i]:
            phase.speciesList.append(speciesObj)
            
## importing all reactions
ourReactions = pd.read_csv(mlf.fullfile([pathString,rxnFile]),index_col=0)

rxnList = []
completeRxns = []
for i in ourReactions.index:
    rxnObj = rxn.ElementaryReaction()
    check = rxnObj.readData(ourReactions,i)
    if check:
        rxnList.append(rxnObj)
        if len(rxnObj.alt) > 0:
            completeRxns.extend(rxnObj.alt)
        else:
            completeRxns.append(rxnObj)




if thermoFlag:
    
    gibbsEnergies = pd.DataFrame(columns=('id','dG','dG_tafel','dG_alka','dG_acid'))
    for rxn in rxnList:
        rxn_id = rxn.name
        dG_alt=[float("NaN"),float("NaN"),float("NaN")]
        dG=float("NaN")
        if len(rxn.alt) > 0:
            for rxn_alt in rxn.alt:
                if '*H' in rxn_alt.educts:
                    i=0
                if 'H2O' in rxn_alt.educts:
                    i=1
                if 'H+' in rxn_alt.educts:
                    i=2
                dG_alt[i]=rxn_alt.calcGibbs(ourSpecies)
        else:
            dG=rxn.calcGibbs(ourSpecies)
        gibbsEnergy = pd.DataFrame([[rxn_id,dG,dG_alt[0],dG_alt[1],dG_alt[2]]],columns=('id','dG','dG_tafel','dG_alka','dG_acid'))
        gibbsEnergies=gibbsEnergies.append(gibbsEnergy,ignore_index=True)
    
    
if writeFlag:
    f1 = open(mlf.fullfile([pathString,templateFile]), "r")
    templateString = f1.read()
    f1.close()
    
    f2 = open(mlf.fullfile([pathString,mechanismFile]), "w")
    f2.write(templateString)
    
    for phase in phaseList:
        f2.writelines(phase.writePhase())
    
    f2.writelines([" \n", "####################### REACTIONS ####################################" ," \n"])
    for rxn in completeRxns:
        f2.writelines([" \n"," \n"])
        f2.writelines(rxn.writeRxn())
    
    f2.close()