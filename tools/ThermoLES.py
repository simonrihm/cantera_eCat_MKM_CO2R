# -*- coding: utf-8 -*-
"""
Created on Thu Jul 15 17:48:49 2021

@author: Simon Rihm
"""

# Imports
import numpy as np
import pandas as pd

import ElementaryReaction as rxn
import IntermediateSpecies as spec

    
def createMatrix(givenList,speciesList):
    thermoLES = np.zeros((len(givenList),len(speciesList)))
    i=0
    for item in givenList:
        if type(item) is spec.IntermediateSpecies:
            idx_temp = speciesList.index(item)
            thermoLES[i][idx_temp] += 1
        elif type(item) is rxn.ElementaryReaction:
            for prod in item.products:
                spec_temp = next((x for x in speciesList if x.name == prod), None)
                idx_temp = speciesList.index(spec_temp)
                thermoLES[i][idx_temp] += 1
            for ed in item.educts:
                spec_temp = next((x for x in speciesList if x.name == ed), None)
                idx_temp = speciesList.index(spec_temp)
                thermoLES[i][idx_temp] -= 1
        i+=1
        
    return thermoLES