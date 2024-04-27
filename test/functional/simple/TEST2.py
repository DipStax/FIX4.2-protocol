import socket
import time

HOST = "localhost"
PORT = 8080

LOGON_MSG = f"8=FIX.4.2{chr(1)}9=70{chr(1)}35=A{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=1{chr(1)}52=20240126-00:26:37.450{chr(1)}108=30{chr(1)}98=0{chr(1)}10=177{chr(1)}"
NEW_ORDER_SINGLE_MSG = f"8=FIX.4.2{chr(1)}9=116{chr(1)}35=D{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240122-13:58:48.962{chr(1)}11=1{chr(1)}21=3{chr(1)}38=100{chr(1)}40=2{chr(1)}44=1000{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:10.000{chr(1)}10=075{chr(1)}"
NEW_ORDER_SINGLE_MSG2 = f"8=FIX.4.2{chr(1)}9=126{chr(1)}35=D{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240122-13:58:48.962{chr(1)}11=1{chr(1)}21=3{chr(1)}38=100{chr(1)}40=2{chr(1)}44=1000{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:10.000{chr(1)}10=175{chr(1)}"
NEW_ORDER_SINGLE_MSG_3 = f"8=FIX.4.2{chr(1)}9=115{chr(1)}35=D{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240126-00:37:05.226{chr(1)}11=1{chr(1)}21=3{chr(1)}40=2{chr(1)}44=100{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:100{chr(1)}10=064{chr(1)}"
NEW_ORDER_SINGLE_MSG_4 = f"8=FIX.4.2{chr(1)}9=119{chr(1)}35=D{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240126-00:43:32.391{chr(1)}11=1{chr(1)}21=3{chr(1)}38={chr(1)}40=2{chr(1)}44=100{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:100{chr(1)}10=237{chr(1)}"
NEW_ORDER_SINGLE_MSG_5 = f"8=FIX.4.2{chr(1)}9=120{chr(1)}35=D{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240126-00:47:11.370{chr(1)}11=1{chr(1)}21=3{chr(1)}38= {chr(1)}40=2{chr(1)}44=100{chr(1)}54=3{chr(1)}55=GOLD{chr(1)}60=20201010-10:10:100{chr(1)}10=003{chr(1)}"
LOGOUT_MSG = f"8=FIX.4.2{chr(1)}9=58{chr(1)}35=5{chr(1)}49=Client1{chr(1)}56=MyMarket{chr(1)}34=3{chr(1)}52=20240126-00:26:37.450{chr(1)}10=148{chr(1)}"

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
    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((HOST, PORT))

    print("[TEST_1] sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_1] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon)."
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket."
    assert response["56"] == "Client1", f"Logon message failed. Expected targetCompID(56) = Client1."
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1."
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30."
    print("[TEST_1] Logon message passed.", f"\n\n")

    print("[TEST_1] sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_1] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "3", f"New Order Single message failed (Didn't receive an Execution Report)."
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket."
    assert response["56"] == "Client1", f"New Order Single message failed. Expected targetCompID(56) = Client1."
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2."
    # assert response["45"] == "2", f"New Order Single message failed. Expected RefSeqNum(45) = 2."
    assert response["371"] == "9", f"New Order Single message failed. Expected RefTagID(371) = 9."
    # assert response["372"] == "D", f"New Order Single message failed. Expected RefMsgType(372) = D."
    # assert response["35"] == "8", f"New Order Single message failed (Didn't receive an Execution Report)."

    print("[TEST_1] New Order Single message passed.", "\n\n")

    print("[TEST_1] sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_1] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logout message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "3", f"Logout message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"

    print("[TEST_1] Logout message passed.", "\n\n")
    _socket.close()
    # sleep(1)
    time.sleep(1)

    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((HOST, PORT))

    print("[TEST_2] sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_2] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon). Expected MsgType(35) = A. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logon message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1. Instead received: {response['34']}"
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30. Instead received: {response['108']}"
    print("[TEST_2] Logon message passed.", "\n\n")

    print("[TEST_2] sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG2.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_2] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "3", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 3. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"New Order Single message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2. Instead received: {response['34']}"
    # assert response["45"] == "2", f"New Order Single message failed. Expected RefSeqNum(45) = 2. Instead received: {response['45']}"
    assert response["371"] == "10", f"New Order Single message failed. Expected RefTagID(371) = 10. Instead received: {response['371']}"
    # assert response["372"] == "D", f"New Order Single message failed. Expected RefMsgType(372) = D. Instead received: {response['372']}"

    print("[TEST_2] New Order Single message passed.", "\n\n")

    print("[TEST_2] sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_2] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). Expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logout message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "3", f"Logout message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"

    print("[TEST_2] Logout message passed.", "\n\n")
    _socket.close()

    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((HOST, PORT))

    print("[TEST_3] sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_3] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon). Expected MsgType(35) = A. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logon message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1. Instead received: {response['34']}"
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30. Instead received: {response['108']}"
    print("[TEST_3] Logon message passed.", "\n\n")

    print("[TEST_3] sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG_3.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_3] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "3", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 3. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"New Order Single message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2. Instead received: {response['34']}"
    assert response["371"] == "38", f"New Order Single message failed. Expected RefTagID(371) = 38. Instead received: {response['371']}"

    print("[TEST_3] New Order Single message passed.", "\n\n")

    print("[TEST_3] sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_3] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). Expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logout message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "3", f"Logout message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"

    print("[TEST_3] Logout message passed.", "\n\n")
    _socket.close()

    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((HOST, PORT))

    print("[TEST_4] sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_4] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon). Expected MsgType(35) = A. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logon message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1. Instead received: {response['34']}"
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30. Instead received: {response['108']}"
    print("[TEST_4] Logon message passed.", "\n\n")

    print("[TEST_4] sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG_4.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_4] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "3", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 3. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"New Order Single message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2. Instead received: {response['34']}"
    assert response["371"] == "38", f"New Order Single message failed. Expected RefTagID(371) = 38. Instead received: {response['371']}"

    print("[TEST_4] New Order Single message passed.", "\n\n")

    print("[TEST_4] sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_4] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). Expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logout message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "3", f"Logout message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"

    print("[TEST_4] Logout message passed.", "\n\n")
    _socket.close()

    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _socket.connect((HOST, PORT))

    print("[TEST_5] sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_5] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon). Expected MsgType(35) = A. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logon message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1. Instead received: {response['34']}"
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30. Instead received: {response['108']}"
    print("[TEST_5] Logon message passed.", f"\n\n")

    print("[TEST_5] sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG_5.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_5] Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "3", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 3. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"New Order Single message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2. Instead received: {response['34']}"
    assert response["371"] == "38", f"New Order Single message failed. Expected RefTagID(371) = 38. Instead received: {response['371']}"

    print("[TEST_5] New Order Single message passed.", f"\n\n")

    print("[TEST_5] sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"[TEST_5] Received message from server: '{data}'", f"\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). Expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "Client1", f"Logout message failed. Expected targetCompID(56) = Client1. Instead received: {response['56']}"
    assert response["34"] == "3", f"Logout message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"

    print("[TEST_5] Logout message passed.", f"\n\n")
    _socket.close()


except Exception as e:
    print(e)
    _socket.close()