# -*- coding: utf-8 -*-
"""
Created on Fri Mar 19 12:07:26 2021

@author: SRIH01
"""

import numpy as np
import pandas as pd

surfBond = '*'

class ElementaryReaction:
    
    def __init__(self,name=""):
        self.name = name
        self.educts =[]
        self.products = []
        self.E_act = 0.0
        self.beta = -1
        self.A = 0.0
        self.alt = []
        self.rev = True
        
    def readData(self,df,idx=1):
        self.name = df['id'].loc[idx]
        
        if not pd.isna(df['ed1'].loc[idx]):
            self.educts.append(df['ed1'].loc[idx])
        if not pd.isna(df['ed2'].loc[idx]):
            self.educts.append(df['ed2'].loc[idx])
        for i in range(int(df['PT'].loc[idx])):
            self.educts.append('p+')
        for i in range(int(df['ET'].loc[idx])):
            self.educts.append('e-')
        
        if not pd.isna(df['prod1'].loc[idx]):
            self.products.append(df['prod1'].loc[idx])
        if not pd.isna(df['prod2'].loc[idx]):
            self.products.append(df['prod2'].loc[idx])
        self.balanceEq()
        self.E_act = df['E_act'].loc[idx]
        self.A = df['A'].loc[idx]
        self.rev = df['reverse'].loc[idx]
            
        if df['ET'].loc[idx] > 0 or df['PT'].loc[idx] > 0:
            self.beta = df['beta'].loc[idx]
            
        if df['PT'].loc[idx] > 0:
            if 'Tafel' in df and df['Tafel'].loc[idx] and df['ET'].loc[idx] > 0:
                E=df['tafelE'].loc[idx]
                dE=df['tafelDE'].loc[idx]
                K=df['tafelK'].loc[idx]
                PEF=df['tafelA'].loc[idx]
                self.alt.append(self.getTafel(K,PEF=PEF,E=E,dE=dE))
            if 'acidic' in df and df['acidic'].loc[idx]:
                E=df['acidE'].loc[idx]
                B=df['acidB'].loc[idx]
                dE=df['acidDE'].loc[idx]
                dB=df['acidDB'].loc[idx]
                K=df['acidK'].loc[idx]
                PEF=df['acidA'].loc[idx]
                self.alt.append(self.getAcidic(K,PEF=PEF,E=E,B=B,dE=dE,dB=dB))
            if 'alkaline' in df and df['alkaline'].loc[idx]:
                E=df['alkaE'].loc[idx]
                B=df['alkaB'].loc[idx]
                dE=df['alkaDE'].loc[idx]
                dB=df['alkaDB'].loc[idx]
                K=df['alkaK'].loc[idx]
                PEF=df['alkaA'].loc[idx]
                self.alt.append(self.getAlkaline(K,PEF=PEF,E=E,B=B,dE=dE,dB=dB))
        
        return df['active'].loc[idx]
                
            
    def getTafel(self,K=1,PEF=np.nan,E=np.nan,dE=0.0):
        tafel = ElementaryReaction(name=self.name + "tafel")
        tafel.educts = list(self.educts)
        tafel.products = list(self.products)
        tafel.educts.remove('p+')
        tafel.educts.remove('e-')
        tafel.educts.append('*H')
        tafel.products.append(surfBond)
        tafel.rev = self.rev
        if not pd.isna(PEF):
            tafel.A = PEF
        else:
            tafel.A = self.A * K
        if not pd.isna(E):
            tafel.E_act = E
        else:
            tafel.E_act = self.E_act + dE
        return tafel
    
    def getAlkaline(self,K=1,PEF=np.nan,E=np.nan,B=np.nan,dE=0.0,dB=0.0):
        alkaline = ElementaryReaction(name=self.name + "water")
        alkaline.educts = list(self.educts)
        alkaline.products = list(self.products)
        alkaline.educts.remove('p+')
        if 'H2O' in alkaline.products:
            alkaline.products.remove('H2O')
        else:
            alkaline.educts.append('H2O')
        alkaline.products.append('OH-')
        alkaline.rev = self.rev
        if not pd.isna(PEF):
             alkaline.A = PEF
        else:
            alkaline.A = self.A * K
        if not pd.isna(E):
            alkaline.E_act = E
        else:
            alkaline.E_act = self.E_act + dE
        if not pd.isna(B):
            alkaline.beta = B
        else:
            alkaline.beta = self.beta + dB
        return alkaline
    
    def getAcidic(self,K=1,PEF=np.nan,E=np.nan,B=np.nan,dE=0.0,dB=0.0):
        acidic = ElementaryReaction(name=self.name + "acid")
        acidic.educts = list(self.educts)
        acidic.products = list(self.products)
        acidic.educts.remove('p+')
        acidic.educts.append('H+')
        acidic.rev = self.rev
        if not pd.isna(PEF):
            acidic.A = PEF
        else:
            acidic.A = self.A * K
        if not pd.isna(E):
            acidic.E_act = E
        else:
            acidic.E_act = self.E_act + dE
        if not pd.isna(B):
            acidic.beta = B
        else:
            acidic.beta = self.beta + dB
        return acidic
    
    def getEquation(self):
        equation = ""
        self.educts.reverse()
        for ed in self.educts:
            equation += ed + " + "
        equation = equation[0:len(equation)-3]
        if self.rev:
            equation += " <=> "
        else:
            equation += " => "
        for prod in self.products:
            equation += prod + " + "
        equation = equation[0:len(equation)-3]
        return equation
    
    def writeRxn(self):
        lineStrings =[]
        lineStrings.append("surface_reaction(\n")
        lineStrings.append("\t id='" + self.name + "',\n")
        eq = self.getEquation()
        lineStrings.append("\t equation='" + eq + "',\n")
        lineStrings.append("\t kf=(" + str(self.A) + ",0," + str(self.E_act) + "),\n")
        if self.beta >= 0:
            lineStrings.append("\t beta=" +  str(self.beta) + "\n")
        lineStrings.append(")\n")
        return lineStrings
    
    def listSpec(self):
        species = []
        for ed in self.educts:
            species.append(ed)
        for prod in self.products:
            species.append(prod)
        return species
    
    def balanceEq(self):
        counter = 0
        for ed in self.educts:
            counter += ed.count(surfBond)
        for prod in self.products:
            counter -= prod.count(surfBond)
            
        while counter > 0:
            self.products.append(surfBond)
            counter -= 1
        
        while counter < 0:
            self.educts.append(surfBond)
            counter += 1
            
    def calcGibbs(self,speciesList):
        dG = 0
        for prod in self.products:
            dG = dG + speciesList['mu [eV]'].loc[prod]
        for ed in self.educts:
            dG = dG - speciesList['mu [eV]'].loc[ed]
            
        return dG