import socket
import signal

PORT = 8080
HOST = "localhost"

NEW_ORDER_CHECKSUM = f"10=004{chr(1)}"

LOGON_MSG = f"8=FIX.4.2{chr(1)}9=67{chr(1)}35=A{chr(1)}49=11111{chr(1)}56=SERVER{chr(1)}34=1{chr(1)}52=20240112-01:07:56.740{chr(1)}98=0{chr(1)}108=30{chr(1)}10=145{chr(1)}"

NEW_ORDER_SINGLE_1 = f"8=FIX.4.2{chr(1)}9=121{chr(1)}35=D{chr(1)}49=11111{chr(1)}56=SERVER{chr(1)}34=2{chr(1)}52=20240112-01:10:28.888{chr(1)}11=1{chr(1)}21=3{chr(1)}38=10{chr(1)}40=2{chr(1)}44=100{chr(1)}54=3{chr(1)}55=Market1{chr(1)}60=20210201-10:00:000{chr(1)}10=010{chr(1)}"
NEW_ORDER_SINGLE_2 = f"8=FIX.4.2{chr(1)}9=120{chr(1)}35=D{chr(1)}49=11111{chr(1)}56=SERVER{chr(1)}34=3{chr(1)}52=20240112-01:10:28.888{chr(1)}11=2{chr(1)}21=3{chr(1)}38=5{chr(1)}40=2{chr(1)}44=100{chr(1)}54=4{chr(1)}55=Market2{chr(1)}60=20210201-10:00:000{chr(1)}10=224{chr(1)}"
NEW_ORDER_SINGLE_3 = f"8=FIX.4.2{chr(1)}9=121{chr(1)}35=D{chr(1)}49=11111{chr(1)}56=SERVER{chr(1)}34=4{chr(1)}52=20240112-01:10:28.888{chr(1)}11=3{chr(1)}21=3{chr(1)}38=50{chr(1)}40=2{chr(1)}44=100{chr(1)}54=3{chr(1)}55=Market3{chr(1)}60=20210201-10:00:000{chr(1)}10=018{chr(1)}"
NEW_ORDER_SINGLE_4 = f"8=FIX.4.2{chr(1)}9=120{chr(1)}35=D{chr(1)}49=11111{chr(1)}56=SERVER{chr(1)}34=5{chr(1)}52=20240112-01:10:28.888{chr(1)}11=4{chr(1)}21=3{chr(1)}38=5{chr(1)}40=2{chr(1)}44=200{chr(1)}54=4{chr(1)}55=EUR/USD{chr(1)}60=20210201-10:00:000{chr(1)}10=229{chr(1)}"

LOGOUT_MSG = f"8=FIX.4.2{chr(1)}9=55{chr(1)}35=5{chr(1)}49=CLIENT{chr(1)}56=SERVER{chr(1)}34=6{chr(1)}52=20240112-01:17:29.460{chr(1)}10=110{chr(1)}"

def signal_handler(signal, frame):
    socket.close()
    exit(0)

signal.signal(signal.SIGINT, signal_handler)
socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((HOST, PORT))
print(f"Connected to server at {HOST}:{PORT}")

while True:
    message = input("Enter message to send: ")
    if (message == "logon"):
        print("sending logon message: ", LOGON_MSG, "\n")
        socket.sendall(LOGON_MSG.encode())
        print("sent logon message")
    elif (message == "new_order1"):
        print("sending new order (Buy[Bid]) message: ", NEW_ORDER_SINGLE_1, "\n")
        socket.sendall(NEW_ORDER_SINGLE_1.encode())
    elif (message == "new_order2"):
        print("sending new order (Sell[Ask]) message: ", NEW_ORDER_SINGLE_2, "\n")
        socket.sendall(NEW_ORDER_SINGLE_2.encode())
    elif (message == "new_order3"):
        print("sending new order (Buy[Bid]) message: ", NEW_ORDER_SINGLE_3, "\n")
        socket.sendall(NEW_ORDER_SINGLE_3.encode())
    elif (message == "new_order4"):
        print("sending new order (Sell[ask]) message: ", NEW_ORDER_SINGLE_4, "\n")
        socket.sendall(NEW_ORDER_SINGLE_4.encode())
    elif (message == "logout"):
        print("sending logout message: ", LOGOUT_MSG, "\n")
        socket.sendall(LOGOUT_MSG.encode())
    else:
        continue
    data = socket.recv(1024).decode()
    print(f"Received message from server: {data}")

