import serial
ser=serial.Serial()
ser.baudrate=9600
ser.port='COM12'
ser.open()
with open("dati_seriale.txt","w") as file:
    r=ser.readline()
    while(r!=b'\r#   \n'):
        file.write(r.decode('utf8'))
        print(r.decode('utf8'))
        r=ser.readline()

with open("dati_seriale.txt","r") as file:
    line=file.readline()
    cnt=1
    vrefint_cal=0
    vrefint_data=0
    vec=[]
    m=0
    i=0
    while line:
        if line!='\n':
            a=int(line)
            b=3.3*a/(4095)
            vec.append(b)
            """m+=b
            if(i==1):
                vec.append(m/2.0)
                m=0
                i=0
            else:
                i+=1
                """
            cnt+=1
        line=file.readline()

with open("dati_seriale_convertiti.txt","w") as file:
    for i in range(3,len(vec)):
        file.write("{}, {}\n".format(i,vec[i]))

import matplotlib.pyplot as plt
import csv
x=[]
y=[]
line=[]
with open("dati_seriale_convertiti.txt","r") as csvfile:
     plots = csv.reader(csvfile,delimiter=',')
     for row in plots:
             x.append(float(row[0]))
             y.append(float(row[1]))
plt.plot(x,y)
threshold=1000*3.3/4095
for _ in x:
    line.append(threshold)

plt.plot(x,line)
plt.show()
