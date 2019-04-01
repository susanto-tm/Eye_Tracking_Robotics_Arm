import serial, time
import random

ser = serial.Serial('COM3', baudrate=9600, timeout=3)
time.sleep(2)

coord = []

try:
    while True:
        coord.insert(0, [random.randint(0, 50), random.randint(0, 10)])

        if len(coord) > 5:
            coord.pop()

        coordinate = "<{0:d},{1:d}>".format(coord[0][0], coord[0][1])

        ser.write(bytes(coordinate, 'utf-8'))
        print(ser.readline())

        time.sleep(0.1)

except KeyboardInterrupt:
    pass
