import serial
import time

# Specify the serial port and baud rate to match your Arduino
arduino_port = 'COM7'  # Replace 'COM3' with your actual port, e.g., '/dev/ttyUSB0' for Linux
baud_rate = 9600



# Initialize the serial connection
ser = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(2)  # Allow time for the connection to initialize

# Function to send data to Arduino
def send_data_to_arduino(data):
    # Convert the data to bytes and send
    ser.write(data.encode())

# Send some test data

def send_coord(x,y):
    coord = f"<{x},{y}>"
    ser.write(coord.encode())
    print(f"Sent coordinates: {coord.strip()}")

try:
    while True:  
        dataPacket = ser.readline()
        print(dataPacket)
        user_input = input("Enter a character to send to Arduino (or type 'exit' to quit): ")
        # dataPacket = ser.readline()
        # print(dataPacket)
        if user_input == 'exit':
            break
        elif user_input == 'c':
            send_data_to_arduino('c')
            user_inputX = input("Enter X coordinate: ")
            user_inputY = input("Enter Y coordinate: ")
            send_coord(user_inputX,user_inputY)
        elif len(user_input) == 1:
            send_data_to_arduino(user_input)
        else:
            print("Please enter valid option.")
        if ser.in_waiting > 0:
            response = ser.readline().decode().strip()
            print(f"Arduino response: {response}")
finally:
    ser.close()  # Close the serial connection when done