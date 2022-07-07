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
    
def read_batch():
    while (True):
        c = input("Which register: 0x")
        if c == "q":
            break
        r =  eval("0x" +c)
        b = bus.read_byte_data(address, r)
        
        print()
        print(f"{hex(r)} - {hex(b)} | {get_bin(b)} | {b}")
        print()


def write_batch():
    while True:
        c = input("Which register: 0x")
        if c == "q":
            break
        r =  eval("0x" +c)
        d = eval("0x" + input("Data: 0x"))
        b = bus.write_byte_data(address, r, d)
        print()
        print(f"{hex(r)} - {hex(d)} | {get_bin(d)} | {d}")
        print()


def read_reg():
    r = eval("0x" + input("Which register: 0x"))
    b = bus.read_byte_data(address, r)
    
    print()
    print(f"{hex(r)} - {hex(b)} | {get_bin(b)} | {b}")
    print()

def write_reg():
    r = eval("0x" + input("Which register: 0x"))
    d = eval("0x" + input("Data: 0x"))
    b = bus.write_byte_data(address, r, d)
    print()
    print(f"{hex(r)} - {hex(d)} | {get_bin(d)} | {d}")
    print()

while True:
    print("Select an operation:")
    print("1. Read register")
    print("2. Read batch register")
    print("3. Write register")
    print("4. Write batch register")
    print("5. Exit")
    c = eval(input("Your choice: "))

    if c == 1:
        read_reg()
    elif c == 3:
        write_reg()
    elif c == 5:
        quit()
    elif c == 2:
        read_batch()
    elif c == 4:
        write_batch()
    else:
        continue    
