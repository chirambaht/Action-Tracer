from smbus import SMBus

channel = 1

address = eval(input("Enter the device address (0x??): " ))

# Initialize I2C (SMBus)
bus = SMBus(channel)

def quit():
    exit(1)

def read_reg():
    r = eval(input("Which register: "))
    b = bus.read_byte_data(address, r)
    print(f"{hex(r)} - {b}")

def write_reg():
    r = eval(input("Which register: "))
    d = eval(input("Data: "))
    b = bus.write_byte_data(address, r, d)
    print(f"{hex(r)} - {hex(d)}")

while True:
    print("Select an operation:")
    print("1. Read register")
    print("2. Write register")
    print("3. Exit")
    c = eval(input("Your choice: "))

    if c == 1:
        read_reg()
    elif c == 2:
        write_reg()
    elif c == 3:
        quit()
    
