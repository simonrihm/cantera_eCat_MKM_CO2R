# -*- coding: utf-8 -*-
"""
Created on Wed Apr 21 20:34:19 2021

@author: Simon Rihm
"""

surfBond = '*'
import pandas as pd

C = 0
H = 1
O = 2
Cu = 3
charge = 4
N = 5


class IntermediateSpecies:
    
    
    def __init__(self,name,x0=0.0,mu=0.0,specIdx=0):
        self.name =  name
        self.size = name.count(surfBond)  
        self.x0 = x0
        self.mu =  mu
        self.idx = specIdx
        
    def getAtoms(self):
        atoms=[0,0,0,0,0,0] #C H O (Cu)/* charge N
        atoms_temp=[0,0,0,0,0,0]
        bracketFlag = False
        for letter in self.name:
            if letter == 'C':
                if bracketFlag:
                    atoms_temp[C] += 1
                atoms[C] +=1
                lastAtom = C
            elif letter == 'H':
                if bracketFlag:
                    atoms_temp[H] += 1
                atoms[H] +=1
                lastAtom = H
            elif letter == 'O':
                if bracketFlag:
                    atoms_temp[O] += 1
                atoms[O] +=1
                lastAtom = O
            elif letter == '*':
                if bracketFlag:
                    atoms_temp[Cu] += 1
                atoms[Cu] +=1
                lastAtom = Cu
            elif letter == 'N':
                if bracketFlag:
                    atoms_temp[N] += 1
                atoms[N] +=1
                lastAtom = N
            elif letter == '(':
                bracketFlag = True
            elif letter == ')':
                bracketFlag = False
                lastAtom = -1
            elif letter == '+':
                atoms[charge] +=1
            elif letter == '-':
                atoms[charge] -=1
            elif letter == 'e':
                pass
            else:
                number = int(letter)
                if lastAtom == -1:
                    atoms[C] += atoms_temp[C] * (number-1)
                    atoms[H] += atoms_temp[H] * (number-1)
                    atoms[O] += atoms_temp[O] * (number-1)
                    atoms[N] += atoms_temp[N] * (number-1)
                    atoms_temp=[0,0,0,0,0,0]
                else:
                    if bracketFlag:
                        atoms_temp[lastAtom] +=  number - 1
                    else:
                        atoms[lastAtom] += number -1
        return atoms
        
    def writeSpecies(self,thermoFlag=False,sizeFlag=False):
        lineStrings =[]
        lineStrings.append("species(\n")
        lineStrings.append("\t name='" + self.name + "',\n")
        atoms = self.getAtoms()
        lineStrings.append("\t atoms='H:" + str(atoms[H]) + " C:" + str(atoms[C])
                           + " O:" + str(atoms[O]) + " E:" + str(-atoms[charge]) + " N:" + str(atoms[N]) + "',\n")
        if sizeFlag:
            lineStrings.append("\t size=" + str(self.size) + ",\n")
        if thermoFlag:
            lineStrings.append("\t thermo=const_cp(\n")
            lineStrings.append("\t \t t0=t_0,\n")
            lineStrings.append("\t \t cp0=(0,'J/mol/K'),\n")
            lineStrings.append("\t \t s0=(0,'J/mol/K'),\n")
            lineStrings.append("\t \t h0=(" + str(self.mu) + ",'eV'),\n")
            lineStrings.append("\t )\n")
        lineStrings.append(")\n")
        lineStrings.append("\n")
        return lineStrings
              
        
class SpeciesPhases:
    def __init__(self,name,phaseType,add1="",add2="",add3=""):
        self.name = name
        self.phaseType = phaseType
        self.add1 = add1
        self.add2 = add2
        self.add3 = add3  
        self.speciesList = []
        
    def writePhase(self):
        if self.phaseType == "ideal_interface":
            x0_string = "coverages"
            surfFlag = True
            metalFlag = False
        else:
            if self.phaseType == "metal":
                metalFlag = True
            else:
                metalFlag = False
            x0_string = "mole_fractions"
            surfFlag = False
            
        lineStrings =[]
        lineStrings.append("#------------------------------------------------------------------ \n")
        lineStrings.append("#\n")
        lineStrings.append("#    " + self.name + " phase\n")
        lineStrings.append("#\n")
        lineStrings.append("#------------------------------------------------------------------ \n")
        lineStrings.append("\n")
        lineStrings.append(self.phaseType+"(\n")
        lineStrings.append("\t name='" + self.name + "',\n")
        lineStrings.append("\t elements='H C O E N',\n")
        lineStrings.append("\t species=(")
        for species in self.speciesList:
            lineStrings.append("'"+species.name+"'")
            lineStrings.append(",")
        del lineStrings[-1]
        lineStrings.append("),\n")
        lineStrings.append("\t  initial_state=state(\n")
        lineStrings.append("\t \t " + x0_string + "='")
        for species in self.speciesList:
            lineStrings.append(species.name+":"+str(species.x0))
            lineStrings.append(",")
        del lineStrings[-1]
        lineStrings.append("',\n")
        lineStrings.append("\t \t temperature=t_0,\n")
        lineStrings.append("\t \t pressure=p_0\n")
        lineStrings.append("\t ),\n")
        lineStrings.append("\t transport='None',\n")
        if not pd.isna(self.add1):
            lineStrings.append("\t " + self.add1 + ",\n")
        if not pd.isna(self.add2):
            lineStrings.append("\t " + self.add2 + ",\n")
        if not pd.isna(self.add3):
            lineStrings.append("\t " + self.add3 + ",\n")
        lineStrings.append(")\n")
        lineStrings.append("\n")
        for species in self.speciesList:
            lineStrings.extend(species.writeSpecies(sizeFlag=surfFlag,thermoFlag=(not metalFlag)))
            
        return lineStrings