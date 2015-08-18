import threading;
import sys;

n = int(sys.argv[1]);
rep = int(sys.argv[2])/n;
#print "n: " + str(n);
#print "rep: " + str(rep);

b = [0 for i in range(0,n)];
y = 0;
x = 0;
class myThread (threading.Thread):
	def __init__(self, id, name):
		threading.Thread.__init__(self);
		self.threadID = id;
		self.name =  name;
	def run(self):
		#print "Starting " + self.name + " id: " + str(self.threadID);
		access_cs(self.name, self.threadID);
		

def access_cs(name, i):
	global n;
	global b;
	global y;
	global x;
	global rep;
	flag = 0;
	iteration = 0;
	while(iteration < rep):
		flag = 0;
		while(flag == 0):
			flag = 1;
			b[i] = 1;
			x = i;
	        if (y!=0): 
				b[i] = 0 
	       		while ((y == 0) == False):
					print "kicked out in section 2 - " + name  + " -> iteration: " + str(iteration) + "\n";
					flag = 0;

			if(flag != 0):
			        y = i;
				print name  + " entering CS -> iteration: " + str(iteration) + "\n";
			        if (x!=i): 
					b[i] = 0;
					print "kicked out in section 3 - " + name + "\n";
			        	for j in range(0, n): 
						while((b[j]==0) == False):
							flag = 0; 
					        if(y!=i): 
							while((y == 0) == False):
								flag = 0;	
          
       	print name  + " accessing CS -> iteration: " + str(iteration) + "\n";
		for ii in range(1000):
			ii = ii; 
        	print name  + " exiting CS -> iteration: " + str(iteration) + "\n";
	        y = 0; 
	        b[i] = 0
		iteration += 1;
		
for i in range(0,n):
	name = "Thread_" + str(i);
	thread_0 = myThread(i, name);
	thread_0.start();

