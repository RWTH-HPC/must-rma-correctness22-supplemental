#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

if len(sys.argv) != 2:
    print(f"Usage: ./{sys.argv[0]} lit-output-file")
    exit(1)

output = open(sys.argv[1], 'r')

classes_patterns = {'RRB-ProcessLocal' : 'ProcessLocal', 
                 'RRB-AcrossProcesses': 'AcrossProcesses',
                 'MBI-LocalConcurrency' : 'LocalConcurrency', 
                 'MBI-GlobalConcurrency' : 'GlobalConcurrency'}

results = {}
for c in classes_patterns.keys():
    results[c] = {'total': 0, 'TP': 0, 'FP': 0, 'TN': 0, 'FN': 0}


for line in output.readlines():
    if not line.startswith('PASS') and not line.startswith('FAIL') and not line.startswith('XFAIL'):
        continue

    passed = False
    if line.startswith('PASS'):
        passed = True

    for c,pattern in classes_patterns.items():
        if pattern in line:
            results[c]['total'] += 1
            if 'Safe.c' in line or '_ok.c' in line:
                if passed:
                    results[c]['TN'] += 1
                else:
                    results[c]['FP'] += 1
            elif 'Race.c' in line or '_nok.c' in line:
                if passed:
                    results[c]['TP'] += 1
                else:
                    results[c]['FN'] += 1
            else:
                print("FAIL")
                print(line)

precision = {}
recall = {}
for c in classes_patterns.keys():
    precision[c] = results[c]['TP'] / (results[c]['TP'] + results[c]['FP'])
    recall[c] = results[c]['TP'] / (results[c]['TP'] + results[c]['FN'])

print("{:<24} {:<6} {:<6} {:<6} {:<6} {:<6} {:<12} {:<12}".format('Class','Total','TP','FP','TN','FN','Precision','Recall')) 
print("===============================================================================")
for c in results.keys():
    print("{:<24} {:<6} {:<6} {:<6} {:<6} {:<6} {:<12.2f} {:<12.2f}".format(c,results[c]['total'],results[c]['TP'],results[c]['FP'],results[c]['TN'],results[c]['FN'],precision[c],recall[c]))
print("===============================================================================")
print("Total passed:", sum([results[c]['TP'] + results[c]['TN'] for c in classes_patterns.keys()]))
print("Total failed:", sum([results[c]['FP'] + results[c]['FN'] for c in classes_patterns.keys()]))
