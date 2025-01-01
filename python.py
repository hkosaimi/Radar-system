
import serial
import time
# Serial port to communicate with Arduino
serial_port = "COM6"  # Change this to your correct COM port
baud_rate = 9600
ser = serial.Serial(serial_port, baud_rate)

# Function to save to a file
def save_to_file(data):
    with open("detection_log.txt", "a") as f:
        f.write(data + "\n")
        print("Data saved to file.")

# Function to display and log object detection status
def display_and_log_data(data):
    print(f"Received: {data}")
    save_to_file(data)

# Read from serial and process data
while True:
    if ser.in_waiting > 0:
        # Read line from serial input
        data = ser.readline().decode('utf-8').strip()
        
        # Assuming the Arduino sends "Distance: X cm" or "Total detections: Y"
        # Log and display data when object detected
        if "Distance:" in data:
            display_and_log_data(f"Object detected at {data}")
        
        # Log and display total detections count
        elif "Total detections" in data:
            display_and_log_data(data)
        
        time.sleep(0.1)  # To prevent overloading the serial buffer