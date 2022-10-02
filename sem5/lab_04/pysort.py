#!/bin/python3

print("pysort input file: ", end="")
array = map(int, open(input().strip(), "r").readline().split())
array = sorted(array)
print(*array)
