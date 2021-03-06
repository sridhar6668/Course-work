import threading;
import sys;

n = int(sys.argv[1]);
rep = int(sys.argv[2])/n;
choosing = [0 for i in range(0,n)];
num = [0 for i in range(0,n)];

class myThread (threading.Thread):
	def __init__(self, id, name):
		threading.Thread.__init__(self);
		self.threadID = id;
		self.name =  name;
	def run(self):
		access_cs(self.name, self.threadID);
		
def find_max(num):
	max = num[0];
	for i in range(0,n):
		if(num[i] > max):
			max = num[i]
	return max;
def access_cs(name, id):
	global n;
	global choosing;
	global num;
	global rep;
	flag = 0;
	i = 0;
	while(i < rep):
		choosing[id] = 1;
		num[id] = 1 + find_max(num);
		choosing[id] = 0;
		print name  + " entering CS -> iteration: " + str(i) + "\n";
		for j in range(0,n):
			if(j != id):			
				# check whether all choosing are zero else spin	
				while(choosing[j]!=0):
					print name + " is stuck in spin lock because choosing of thread_" + str(j) + "is non-zero \n";
				# check whether all num are either 0 or greater than the num[id] else spin
				while(((num[j] == 0) or (num[j] > num[id]) or (num[j] == num[id] and j > id)) == False):
					# print msg starts
					if(flag == 0):
						flag = 1;
						print name + " is stuck in spin lock because its value is greater than thread_" + str(j) + "\n";
		print name  + " accessing CS -> iteration: " + str(i) + "\n";
		for temp in range(1000):
			temp1 = temp;
		flag = 0;
		print name  + " exiting CS -> iteration: " + str(i) + "\n";
		num[id] = 0;
		i+=1;
# Create n threads
for i in range(0,n):
	name = "Thread_" + str(i);
	thread_0 = myThread(i, name);
	thread_0.start();

