import re
import sys
import string
import math

def data(filename):
	try:
		# filename = raw_input("Enter filename \n")
		file_read = open(filename, 'r')
		# file_write = open(filename,'w')
		s = file_read.readlines()

		l = []
		i = 0
		for line in s:
			l.append(line.split(" "))

		l_enqueue = []
		l_dequeue = []
		l_received = [] 
		l_dropped = [] 

		for i in range(len(l)):
			if(l[i][0] == '+' and "ns3::UdpHeader" in l[i]):
				l_enqueue.append(l[i])
				
			elif(l[i][0] == '-' and "ns3::UdpHeader" in l[i]):
				l_dequeue.append(l[i])

			elif(l[i][0] == 'r' and "ns3::UdpHeader" in l[i]):
				l_received.append(l[i])
				
			elif(l[i][0] == 'd' and "ns3::UdpHeader" in l[i]):
				l_dropped.append(l[i])
		del l
		a = 0
		for i in range(len(l_received)):
			b = l_received[i].index("Payload") + 1
			result = re.findall(r'[0-9]+', l_received[i][b])
			a = a + int(result[0])

		received = a
		total_time = float(l_received[-1][1])
		total_time = total_time - 2

		average_throughput = received/total_time
		count = 0
		packet_count = 0
		delay_list = []
		for i in range(len(l_enqueue)):
			a = l_enqueue[i].index("id")
			b = l_enqueue[i][a+1]
			c = l_enqueue[i].index("ns3::UdpHeader")
			d = l_enqueue[i][c-3] + l_enqueue[i][c-2] + l_enqueue[i][c-1]
			t1 = l_enqueue[i][1]
			delay = 0
			for j in range(len(l_received)):
				e = l_received[j].index("id")
				f = l_received[j][e+1]
				g = l_received[j].index("ns3::UdpHeader")
				h = l_received[j][g-3] + l_received[j][g-2] + l_received[j][g-1]
				t2 = l_received[j][1]
				if( b == f and d == h ):
					delay = float(t2) - float(t1)
					packet_count += 1
			for k in range(len(l_dequeue)):
				w = l_dequeue[k].index("id")
				x = l_dequeue[k][w+1]
				y = l_dequeue[k].index("ns3::UdpHeader")
				z = l_dequeue[k][y-3] + l_dequeue[k][y-2] + l_dequeue[k][y-1]
				t3 = l_dequeue[k][1]
				if( b == x and d == z ):
					delay += float(t3) - float(t1)

			count = count + delay
			delay_list.append(delay)
		forwarding_delay = count/packet_count
		sum = 0
		for delay in delay_list:
			diff = delay - forwarding_delay
			sum = sum + diff * diff
		sum = sum / len(delay_list)
		jitter = math.sqrt(sum)
		ans = {}
		ans["average throughput"] = average_throughput
		ans["forwarding delay"] = forwarding_delay
		ans["jitter"] = jitter
		del l_enqueue
		del l_dequeue
		del l_received
		del l_dropped
		file_read.close()
		return ans

	except:
		print "error file not found"

def more_data(ch):
	avg_ans = {}
	at = 0
	fd = 0
	j = 0
	l1 = []
	l2 = []
	l3 = []
	for i in range(10):
		s = "ans"+ch+"_"+str(i+1)+".tr"
		dataset = data(s)
		print s
		l1.append(dataset["average throughput"])
		l2.append(dataset["forwarding delay"])
		l3.append(dataset["jitter"])
		at += dataset["average throughput"]
		fd += dataset["forwarding delay"]
		j += dataset["jitter"]
	at = at/10
	fd = fd/10
	j = j/10
	avg_ans["average throughput"] = at
	avg_ans["forwarding delay"] = fd
	avg_ans["jitter"] = j
	sum1 = 0
	sum2 = 0
	sum3 = 0
	for i in range(10):
		sum1 += (l1[i]-at)*(l1[i]-at)
		sum2 += (l2[i]-fd)*(l2[i]-fd)
		sum3 += (l3[i]-j)*(l3[i]-j)
	# sum1 = sum1/10
	# sum2 = sum2/10
	# sum3 = sum3/10
	avg_ans["average throughput sd"] = math.sqrt(sum1)
	avg_ans["forwarding delay sd"] = math.sqrt(sum2)
	avg_ans["jitter sd"] = math.sqrt(sum3)
	del dataset
	del at
	del fd
	return avg_ans

if __name__ == "__main__":

	file_write1 = open("average throughput.txt",'w')
	file_write2 = open("forwarding delay.txt",'w')
	file_write3 = open("jitter.txt",'w')
	file_list = []
	file_list.append(["16","1"])
	file_list.append(["32","2"])
	file_list.append(["64","3"])
	file_list.append(["128","4"])
	file_list.append(["256","5"])
	file_list.append(["512","6"])
	file_list.append(["1024","7"])
	for li in file_list:
		ans = more_data(li[1])
		file_write1.write(li[0] + "\t\t" + str(ans["average throughput"]) + "\t" + str(ans["average throughput sd"]) + "\n")
		file_write2.write(li[0] + "\t\t" + str(ans["forwarding delay"]) + "\t" + str(ans["forwarding delay sd"]) + "\n")
		file_write3.write(li[0] + "\t\t" + str(ans["jitter"]) + "\t" + str(ans["jitter sd"]) + "\n")
	del file_list
	file_write1.close()
	file_write1.close()
	file_write1.close()
	# print data("ans7_10.tr")



