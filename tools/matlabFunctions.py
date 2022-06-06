# -*- coding: utf-8 -*-
"""
Created on Tue May 11 17:39:03 2021

@author: SRIH01
"""

    
def fullfile(arr):
    pathString = ""
    for x in arr:
        if x != '':
            temp = x.replace('\\', '/')
            pathString = pathString + temp + '/'
    return pathString[0:-1]