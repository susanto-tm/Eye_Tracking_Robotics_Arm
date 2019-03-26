import serial, time

ser = serial.Serial('COM3', baudrate=9600, timeout=2)
time.sleep(2)

coord = [[180, 20], [29, 30]]

xCoord = str(coord[0][0])
yCoord = str(coord[0][1])

coordinate = "<{0:d},{1:d}>".format(coord[0][0], coord[0][1])
string = "<10"
count = 0

while count < 29:
    ser.write(bytes(coordinate, 'utf-8'))
    print(ser.readline())
    count +=1