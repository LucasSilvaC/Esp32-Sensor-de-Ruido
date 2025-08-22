import serial
import csv
import time

ser = serial.Serial('COM5', 9600) 
time.sleep(2) 

with open('sensor_data.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp", "Sensor Value", "Environment"]) 

    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            print(line)

            if "Ambiente" in line:
                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
                writer.writerow([timestamp, line])  

            time.sleep(3)  

        except KeyboardInterrupt:
            print("Exiting...")
            break

# pip install pyserial