import plotly.figure_factory as ff
import random as rd
f = open("log.txt", 'r')
temp = []

array = []
cpu_task = [[] for i in range(4)]
df = []

# for i in range(10000):
while True:
      line = f.readline()
      # print(line)
      if not line: break
      array = line.split(' ')
      print(array)
      cmd = array[3]
      time  = float(array[2][:-1])
      cpu = int(array[4])
      pid = array[5][:-1]
      # if cmd == "task_fork_wrr":
      #       if cpu_task[cpu] and len(cpu_task[cpu][-1]) == 2:
      #             cpu_task[cpu][-1].append(time)
      #       cpu_task[cpu].append([time, pid])
      if cmd == "context_switch":
            if cpu_task[cpu] and len(cpu_task[cpu][-1]) == 2:
                  cpu_task[cpu][-1].append(time)
            cpu_task[cpu].append([time, pid])
      elif cmd == "deactivate_task":
            if cpu_task[cpu]:
                  cpu_task[cpu][-1].append(time)

f.close()

for i in range(4):
      if cpu_task[i] and len(cpu_task[i][-1]) == 2 :
            cpu_task[i][-1].append(cpu_task[i][-1][0])

for i in range(4):
      for temp_task in cpu_task[i]:
            # print("temp_task:", temp_task)
            df.append(dict(Task=str(i), Start = temp_task[0], Finish = temp_task[2], Resource = temp_task[1]))

r = lambda: rd.randint(0,255)
colors = ['#%02X%02X%02X' % (r(),r(),r())]
for i in range(1, 1000):                                   
      colors.append('#%02X%02X%02X' % (r(),r(),r()))

fig = ff.create_gantt(df, group_tasks= True, index_col='Resource', colors=colors, show_colorbar=True)
fig.update_layout(xaxis_type='linear')
fig.show()
