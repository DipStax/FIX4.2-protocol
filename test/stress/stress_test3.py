import socket
import threading

LOGON_MSG = f"8=FIX.4.2{chr(1)}9=70{chr(1)}35=A{chr(1)}49=CLIENT1{chr(1)}56=MyMarket{chr(1)}34=1{chr(1)}52=20240121-23:15:30.218{chr(1)}98=0{chr(1)}108=30{chr(1)}10=010{chr(1)}"


def connect_to_server(server_address):
    """Attempts to connect to the specified server address and prints the result."""
    try:
        print(f"Trying to connect to {server_address}")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect(server_address)
            print(f"Connected successfully to {server_address}")
            sock.sendall(LOGON_MSG.encode())
            print(f"Sent logon message to {server_address}")
            sock.close()
    except Exception as e:
        print(f"Failed to connect to {server_address}: {e}")

# Replace 'server_address' with the actual server address and port
server_address = ("127.0.0.1", 80)

# Create and start multiple threads to connect to the server
num_threads = 1  # Adjust the number of threads as needed
for _ in range(num_threads):
    threading.Thread(target=connect_to_server, args=(server_address,)).start()
