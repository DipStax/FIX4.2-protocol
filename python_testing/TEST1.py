import socket

HOST = "localhost"
PORT = 8080

LOGON_MSG = f"8=FIX.4.2{chr(1)}9=70{chr(1)}35=A{chr(1)}49=CLIENT1{chr(1)}56=MyMarket{chr(1)}34=1{chr(1)}52=20240121-23:15:30.218{chr(1)}98=0{chr(1)}108=30{chr(1)}10=010{chr(1)}"
NEW_ORDER_SINGLE_MSG = f"8=FIX.4.2{chr(1)}9=126{chr(1)}35=D{chr(1)}49=CLIENT1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240122-14:11:23.995{chr(1)}11=1{chr(1)}21=3{chr(1)}38=100{chr(1)}40=2{chr(1)}44=1000{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:10.000{chr(1)}10=160{chr(1)}"
LOGOUT_MSG = f"8=FIX.4.2{chr(1)}9=58{chr(1)}35=5{chr(1)}49=CLIENT1{chr(1)}56=MyMarket{chr(1)}34=3{chr(1)}52=20240121-23:15:30.218{chr(1)}10=237{chr(1)}"

def decode_message(msg: str) -> dict:
    """Decode a FIX message into a dictionary."""
    msg_dict = {}
    msg_list = msg.split(chr(1))
    for item in msg_list:
        if "=" in item:
            key, value = item.split("=")
            msg_dict[key] = value
    return msg_dict

try:
    socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    socket.connect((HOST, PORT))

    print("sending message: logon\n")
    socket.sendall(LOGON_MSG.encode())
    data = socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", "Logon message failed (Didn't receive a Logon)."
    assert response["49"] == "MyMarket", "Logon message failed. Expected senderCompID(49) = MyMarket."
    assert response["56"] == "CLIENT1", "Logon message failed. Expected targetCompID(56) = CLIENT1."
    assert response["34"] == "1", "Logon message failed. Expected MsgSeqNum(34) = 1."
    assert response["108"] == "30", "Logon message failed. Expected HeartBtInt = 30."
    print("Logon message passed.", "\n\n")

    print("sending message: new_order_single\n")
    socket.sendall(NEW_ORDER_SINGLE_MSG.encode())
    data = socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "8", "New Order Single message failed (Didn't receive an Execution Report)."
    assert response["49"] == "MyMarket", "New Order Single message failed. Expected senderCompID(49) = MyMarket."
    assert response["56"] == "CLIENT1", "New Order Single message failed. Expected targetCompID(56) = CLIENT1."
    assert response["34"] == "2", "New Order Single message failed. Expected MsgSeqNum(34) = 2."

    assert response["6"] == "0", "New Order Single message failed. Expected AvgPx = 0."
    assert response["14"] == "0", "New Order Single message failed. Expected CumQty = 0."
    assert response["17"] == "1", "New Order Single message failed. Expected ExecID = 1."
    assert response["20"] == "0", "New Order Single message failed. Expected ExecTransType = 0."
    assert response["37"] == "1", "New Order Single message failed. Expected OrderID = 1."
    assert response["38"] == "100", "New Order Single message failed. Expected OrderQty = 100."
    assert response["39"] == "0", "New Order Single message failed. Expected OrdStatus = 0."
    assert response["40"] == "2", "New Order Single message failed. Expected OrdType = 2."
    assert response["44"] == "1000", "New Order Single message failed. Expected Price = 1000."
    assert response["54"] == "3", "New Order Single message failed. Expected Side = 3."
    assert response["55"] == "GOLD", "New Order Single message failed. Expected Symbol = GOLD."
    assert response["150"] == "0", "New Order Single message failed. Expected ExecType = 0."
    assert response["151"] == "100", "New Order Single message failed. Expected LeavesQty = 100."

    print("New Order Single message passed.", "\n\n")

    print("sending message: logout\n")
    socket.sendall(LOGOUT_MSG.encode())
    data = socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", "Logout message failed (Didn't receive a Logout)."
    assert response["49"] == "MyMarket", "Logout message failed. Expected senderCompID(49) = MyMarket."
    assert response["56"] == "CLIENT1", "Logout message failed. Expected targetCompID(56) = CLIENT1."
    assert response["34"] == "3", "Logout message failed. Expected MsgSeqNum(34) = 3."

    print("Logout message passed.", "\n\n")
    socket.close()

except Exception as e:
    print(e)
    socket.close()