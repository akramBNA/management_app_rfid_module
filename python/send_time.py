import serial
from datetime import datetime
import time

arduino = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
time.sleep(2)  # allow Arduino to reset

print("Python script running. Waiting for Arduino requests...")

while True:
    line = arduino.readline().decode(errors='ignore').strip()
    if line:
        print("Arduino:", line)
        if "REQUEST_TIME" in line:
            now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            arduino.write(f"{now}\n".encode())
            print("Sent time to Arduino:", now)

