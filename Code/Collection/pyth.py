from smbus import SMBus

channel = 1

address = eval("0x" + input("Enter the device address (0x??): 0x" ))

# Initialize I2C (SMBus)
bus = SMBus(channel)

def quit():
    exit(1)

def get_bin(tt):
    
    temp = bin(tt)[2:]
    while len(temp) < 8:
        temp = "0" + temp
    out = temp[:4] + " " + temp[4:]
    return out
    

def read_reg():
    r = eval("0x" + input("Which register: 0x"))
    b = bus.read_byte_data(address, r)
    
    print(f"{hex(r)} - {hex(b)} | {get_bin(b)} | {b}")
    print()

def write_reg():
    r = eval("0x" + input("Which register: 0x"))
    d = eval("0x" + input("Data: 0x"))
    b = bus.write_byte_data(address, r, d)
    print(f"{hex(r)} - {hex(d)} | {get_bin(d)} | {d}")
    print()

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
    
