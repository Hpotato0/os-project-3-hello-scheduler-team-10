import plotly.figure_factory as ff
import random as rd
f = open("test.txt", 'r')
temp = []

array = []
cpu_task = [[] for i in range(4)]
df = []
factorize_time = {}
# for i in range(10000):
while True:
      line = f.readline()
      # print(line)
      if not line: break
      array = line.split()
      # print(array)
      time  = float(array[1][:-1])
      cmd = array[2]
      cpu = int(array[3])
      pid = array[4]
      name = array[5]

      if cmd == "context_switch":
            if cpu_task[cpu] and len(cpu_task[cpu][-1]) == 3:
                  cpu_task[cpu][-1].append(time)
                  # print(cpu_task[cpu][-1])
            cpu_task[cpu].append([name, pid, time])
            # print(cpu_task[cpu][-1])
      elif cmd == "deactivate_task":
            if cpu_task[cpu]:
                  cpu_task[cpu][-1].append(time)

f.close()

# for i in range(4):
#       for temp_task in cpu_task[i]:
#             print(temp_task)


for i in range(4):
      if cpu_task[i] and len(cpu_task[i][-1]) == 3 :
            cpu_task[i][-1].append(cpu_task[i][-1][2])

for i in range(4):
      for temp_task in cpu_task[i]:
            # print("temp_task:", temp_task)
            if temp_task[0] == "factorizeLoop":
                  df.append(dict(Task=str(i), Start = temp_task[2], Finish = temp_task[3], Resource = temp_task[1]))
                  # total time tracking
                  if temp_task[1] not in factorize_time:
                        factorize_time[temp_task[1]] = temp_task[3] - temp_task[2]
                  else:
                        factorize_time[temp_task[1]] += temp_task[3] - temp_task[2]
                  
r = lambda: rd.randint(0,255)
colors = ['#%02X%02X%02X' % (r(),r(),r())]
for i in range(1, 100):                                   
      colors.append('#%02X%02X%02X' % (r(),r(),r()))

fig = ff.create_gantt(df, group_tasks= True, index_col='Resource', colors=colors, show_colorbar=True)
fig.update_layout(xaxis_type='linear')
fig.show()

sorted_time = sorted(factorize_time.items())
print(sorted_time)
for key in sorted_time:
      print(key)  