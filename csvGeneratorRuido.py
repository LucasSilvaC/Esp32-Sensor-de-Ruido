import serial
import csv
import time

ser = serial.Serial('COM5', 115200)  
time.sleep(2) 

with open('sensor_data.csv', mode='w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp", "Media", "Classificacao"]) 

    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            print(line)

            if "Média em 10s:" in line:
                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

                try:
                    partes = line.split("->")
                    media_str = partes[0].split(":")[1].split("/")[0].strip()
                    classificacao = partes[1].strip()

                    media = int(media_str)

                    writer.writerow([timestamp, media, classificacao])
                    file.flush() 
                except Exception as e:
                    print("Erro ao processar linha:", e, line)

        except KeyboardInterrupt:
            print("Encerrando...")
            break
