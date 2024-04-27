import socket

PORT = 8080
HOST = "localhost"

try:
    _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # _socket.connect((HOST, PORT))

    for i in range (0, 1000):
        # open socket connection
        print(f"{i}: opening socket connection")
        _socket.connect((HOST, PORT))
        print(f"{i}: closing socket connection")
        _socket.close()

except Exception as e:
    print(e)
    _socket.close()