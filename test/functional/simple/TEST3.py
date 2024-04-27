import socket

PORT = 8080
HOST = "localhost"

LOGON_MSG = f"8=FIX.4.2{chr(1)}9=71{chr(1)}35=A{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=1{chr(1)}52=20240124-13:30:02.705{chr(1)}108=30{chr(1)}98=0{chr(1)}10=105{chr(1)}"
NEW_ORDER_SINGLE_MSG = f"8=FIX.4.2{chr(1)}9=124{chr(1)}35=D{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=2{chr(1)}52=20240124-13:30:02.706{chr(1)}11=1{chr(1)}21=3{chr(1)}38=100{chr(1)}40=2{chr(1)}44=1200{chr(1)}54=4{chr(1)}55=GOLD{chr(1)}60=20210301-10:00:000{chr(1)}10=104{chr(1)}"
NEW_ORDER_SINGLE_MSG2 = f"8=FIX.4.2{chr(1)}9=124{chr(1)}35=D{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=3{chr(1)}52=20240124-13:30:02.706{chr(1)}11=2{chr(1)}21=3{chr(1)}38=250{chr(1)}40=2{chr(1)}44=1400{chr(1)}54=4{chr(1)}55=GOLD{chr(1)}60=20210301-10:00:000{chr(1)}10=114{chr(1)}"
ORDER_CANCEL_REPLACE_REQUEST_MSG = f"8=FIX.4.2{chr(1)}9=128{chr(1)}35=G{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=4{chr(1)}52=20240124-13:30:02.706{chr(1)}11=3{chr(1)}41=1{chr(1)}21=3{chr(1)}38=50{chr(1)}40=2{chr(1)}44=1225{chr(1)}54=4{chr(1)}55=GOLD{chr(1)}60=20210301-10:00:100{chr(1)}10=035{chr(1)}"
ORDER_CANCEL_REQUEST_MSG = f"8=FIX.4.2{chr(1)}9=104{chr(1)}35=F{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=5{chr(1)}52=20240124-13:30:02.706{chr(1)}11=4{chr(1)}41=2{chr(1)}54=4{chr(1)}55=GOLD{chr(1)}60=20210301-10:00:100{chr(1)}10=249{chr(1)}"
LOGOUT_MSG = f"8=FIX.4.2{chr(1)}9=59{chr(1)}35=5{chr(1)}49=CLIENT_1{chr(1)}56=MyMarket{chr(1)}34=6{chr(1)}52=20240124-13:38:39.297{chr(1)}10=103{chr(1)}"

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

    print("sending message: logon\n")
    _socket.sendall(LOGON_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "A", f"Logon message failed (Didn't receive a Logon). Expected MsgType(35) = A. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logon message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"Logon message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "1", f"Logon message failed. Expected MsgSeqNum(34) = 1. Instead received: {response['34']}"
    assert response["108"] == "30", f"Logon message failed. Expected HeartBtInt = 30. Instead received: {response['108']}"
    print("Logon message passed.", "\n\n")

    print("sending message: new_order_single\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "8", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 8. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"New Order Single message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "2", f"New Order Single message failed. Expected MsgSeqNum(34) = 2. Instead received: {response['34']}"

    # assert response["6"] == "0", "New Order Single message failed. Expected AvgPx = 0."
    assert response["14"] == "0", f"New Order Single message failed. Expected CumQty(14) = 0. Instead received: {response['14']}"
    assert response["17"] == "1", f"New Order Single message failed. Expected ExecID(17) = 1. Instead received: {response['17']}"
    assert response["20"] == "0", f"New Order Single message failed. Expected ExecTransType(20) = 0. Instead received: {response['20']}"
    assert response["37"] == "1", f"New Order Single message failed. Expected OrderID(37) = 1. Instead received: {response['37']}"
    assert response["38"] == "100", f"New Order Single message failed. Expected OrderQty(38) = 100. Instead received: {response['38']}"
    assert response["39"] == "0", f"New Order Single message failed. Expected OrdStatus(39) = 0. Instead received: {response['39']}"
    assert response["40"] == "2", f"New Order Single message failed. Expected OrdType(40) = 2. Instead received: {response['40']}"
    assert response["44"] == "1200.000000", f"New Order Single message failed. Expected Price(44) = 1200.000000. Instead received: {response['44']}"
    assert response["54"] == "4", f"New Order Single message failed. Expected Side(54) = 4. Instead received: {response['54']}"
    assert response["55"] == "GOLD", f"New Order Single message failed. Expected Symbol(55) = GOLD. Instead received: {response['55']}"
    assert response["150"] == "0", f"New Order Single message failed. Expected ExecType(150) = 0. Instead received: {response['150']}"
    assert response["151"] == "100", f"New Order Single message failed. Expected LeavesQty(151) = 100. Instead received: {response['151']}"

    print("New Order Single message passed.", "\n\n")

    print("sending message: new_order_single 2\n")
    _socket.sendall(NEW_ORDER_SINGLE_MSG2.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "8", f"New Order Single message failed (Didn't receive an Execution Report). Expected MsgType(35) = 8. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"New Order Single message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"New Order Single message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "3", f"New Order Single message failed. Expected MsgSeqNum(34) = 3. Instead received: {response['34']}"
    # assert response["6"] == "0", "New Order Single message failed. Expected AvgPx = 0."
    assert response["14"] == "0", f"New Order Single message failed. Expected CumQty(14) = 0. Instead received: {response['14']}"
    assert response["17"] == "2", f"New Order Single message failed. Expected ExecID(17) = 3. Instead received: {response['17']}"
    assert response["20"] == "0", f"New Order Single message failed. Expected ExecTransType(20) = 0. Instead received: {response['20']}"
    assert response["37"] == "2", f"New Order Single message failed. Expected OrderID(37) = 2. Instead received: {response['37']}"
    assert response["38"] == "250", f"New Order Single message failed. Expected OrderQty(38) = 250. Instead received: {response['38']}"
    assert response["39"] == "0", f"New Order Single message failed. Expected OrdStatus(39) = 0. Instead received: {response['39']}"
    assert response["40"] == "2", f"New Order Single message failed. Expected OrdType(40) = 2. Instead received: {response['40']}"
    assert response["44"] == "1400.000000", f"New Order Single message failed. Expected Price(44) = 1400.000000. Instead received: {response['44']}"
    assert response["54"] == "4", f"New Order Single message failed. Expected Side(54) = 4. Instead received: {response['54']}"
    assert response["55"] == "GOLD", f"New Order Single message failed. Expected Symbol(55) = GOLD. Instead received: {response['55']}"
    assert response["150"] == "0", f"New Order Single message failed. Expected ExecType(150) = 0. Instead received: {response['150']}"
    assert response["151"] == "250", f"New Order Single message failed. Expected LeavesQty(151) = 250. Instead received: {response['151']}"

    print("New Order Single message passed.", "\n\n")

    print("sending message: order_cancel_replace_request\n")
    _socket.sendall(ORDER_CANCEL_REPLACE_REQUEST_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "8", f"Order Cancel Replace Request message failed (Didn't receive an Execution Report). Expected MsgType(35) = 8. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Order Cancel Replace Request message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"Order Cancel Replace Request message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "4", f"Order Cancel Replace Request message failed. Expected MsgSeqNum(34) = 4. Instead received: {response['34']}"
    # assert response["6"] == "0", "Order Cancel Replace Request message failed. Expected AvgPx = 0."
    assert response["14"] == "0", f"Order Cancel Replace Request message failed. Expected CumQty(14) = 0. Instead received: {response['14']}"
    assert response["17"] == "3", f"Order Cancel Replace Request message failed. Expected ExecID(17) = 4. Instead received: {response['17']}"
    assert response["20"] == "0", f"Order Cancel Replace Request message failed. Expected ExecTransType(20) = 5. Instead received: {response['20']}"
    # assert response["37"] == "1", f"Order Cancel Replace Request message failed. Expected OrderID(37) = 1. Instead received: {response['37']}"
    assert response["38"] == "50", f"Order Cancel Replace Request message failed. Expected OrderQty(38) = 50. Instead received: {response['38']}"
    assert response["39"] == "5", f"Order Cancel Replace Request message failed. Expected OrdStatus(39) = 5. Instead received: {response['39']}"
    assert response["40"] == "2", f"Order Cancel Replace Request message failed. Expected OrdType(40) = 2. Instead received: {response['40']}"
    # assert response["41"] == "1", f"Order Cancel Replace Request message failed. Expected OrigClOrdID(41) = 1. Instead received: {response['41']}"
    assert response["44"] == "1225.000000", f"Order Cancel Replace Request message failed. Expected Price(44) = 1225.000000. Instead received: {response['44']}"
    assert response["54"] == "4", f"Order Cancel Replace Request message failed. Expected Side(54) = 4. Instead received: {response['54']}"
    assert response["55"] == "GOLD", f"Order Cancel Replace Request message failed. Expected Symbol(55) = GOLD. Instead received: {response['55']}"
    assert response["150"] == "5", f"Order Cancel Replace Request message failed. Expected ExecType(150) = 5. Instead received: {response['150']}"
    assert response["151"] == "50", f"Order Cancel Replace Request message failed. Expected LeavesQty(151) = 50. Instead received: {response['151']}"

    print("Order Cancel Replace Request message passed.", "\n\n")

    print("sending message: order_cancel_request\n")
    _socket.sendall(ORDER_CANCEL_REQUEST_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "8", f"Order Cancel Request message failed (Didn't receive an Execution Report). Expected MsgType(35) = 8. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Order Cancel Request message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"Order Cancel Request message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "5", f"Order Cancel Request message failed. Expected MsgSeqNum(34) = 5. Instead received: {response['34']}"
    # assert response["6"] == "0", "Order Cancel Request message failed. Expected AvgPx = 0."
    assert response["14"] == "0", f"Order Cancel Request message failed. Expected CumQty(14) = 0. Instead received: {response['14']}"
    assert response["17"] == "4", f"Order Cancel Request message failed. Expected ExecID(17) = 5. Instead received: {response['17']}"
    assert response["20"] == "0", f"Order Cancel Request message failed. Expected ExecTransType(20) = 5. Instead received: {response['20']}"
    assert response["37"] == "2", f"Order Cancel Request message failed. Expected OrderID(37) = 2. Instead received: {response['37']}"
    assert response["39"] == "4", f"Order Cancel Request message failed. Expected OrdStatus(39) = 5. Instead received: {response['39']}"
    assert response["40"] == "2", f"Order Cancel Request message failed. Expected OrdType(40) = 2. Instead received: {response['40']}"
    # assert response["41"] == "2", f"Order Cancel Request message failed. Expected OrigClOrdID(41) = 2. Instead received: {response['41']}"
    assert response["54"] == "4", f"Order Cancel Request message failed. Expected Side(54) = 4. Instead received: {response['54']}"
    assert response["55"] == "GOLD", f"Order Cancel Request message failed. Expected Symbol(55) = GOLD. Instead received: {response['55']}"
    assert response["150"] == "4", f"Order Cancel Request message failed. Expected ExecType(150) = 5. Instead received: {response['150']}"
    # assert response["151"] == "0", f"Order Cancel Request message failed. Expected LeavesQty(151) = 0. Instead received: {response['151']}"

    print("Order Cancel Request message passed.", "\n\n")

    print("sending message: logout\n")
    _socket.sendall(LOGOUT_MSG.encode())
    data = _socket.recv(1024).decode()
    print(f"Received message from server: '{data}'", "\n\n")
    response = decode_message(data)

    assert response["35"] == "5", f"Logout message failed (Didn't receive a Logout). Expected MsgType(35) = 5. Instead received: {response['35']}"
    assert response["49"] == "MyMarket", f"Logout message failed. Expected senderCompID(49) = MyMarket. Instead received: {response['49']}"
    assert response["56"] == "CLIENT_1", f"Logout message failed. Expected targetCompID(56) = CLIENT_1. Instead received: {response['56']}"
    assert response["34"] == "6", f"Logout message failed. Expected MsgSeqNum(34) = 6. Instead received: {response['34']}"
    print("Logout message passed.", "\n\n")

    _socket.close()

except Exception as e:
    print(f"e.args[0]: {e.args[0]}\n")
    print(e)
    if len(e.args[0]) == 1 or e.args[0].isnumeric():
        print(f"Error: tag '{e.args[0]}' is missing.")
    else:
        print(f"Test failed. Error: {e}")
    print(f"\nResponse: \n")
    for key, value in response.items():
        print(f"{key}: {value}")
    _socket.close()