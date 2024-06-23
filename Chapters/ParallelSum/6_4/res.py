import sys

f = open("res.txt", "r")

line = f.readline()
str_num = line[14::]
seq_time = float(str_num)

line = f.readline()
str_num = line[14::]
par_time = float(str_num)

th_num = int(sys.argv[1])

speedup = seq_time / par_time
efficiency = (seq_time / th_num) / par_time

print(f"Speedup is: {speedup}")
print(f"Efficiency is: {efficiency}")

f.close()
