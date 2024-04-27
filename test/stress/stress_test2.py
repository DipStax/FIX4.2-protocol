import socket
import threading

def connect_to_server(server_address):
    """Attempts to connect to the specified server address and prints the result."""
    try:
        print(f"Trying to connect to {server_address}")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect(server_address)
            print(f"Connected successfully to {server_address}")
    except Exception as e:
        print(f"Failed to connect to {server_address}: {e}")

# Replace 'server_address' with the actual server address and port
server_address = ("127.0.0.1", 80)

# Create and start multiple threads to connect to the server
num_threads = 100  # Adjust the number of threads as needed
for _ in range(num_threads):
    threading.Thread(target=connect_to_server, args=(server_address,)).start()
