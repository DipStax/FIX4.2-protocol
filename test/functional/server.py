import socket
import threading
import signal

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

port = 8080
server_socket.bind(("localhost", port))
server_socket.listen(5)

print(f"Listening on port {port}")

clients = []
threads = []

def handle_client(client_socket):
    try:
        while True:
            data = client_socket.recv(1024).decode()
            if not data:
                break

            print(f"Received message from client: {data}")
            client_socket.sendall("{data}".encode())

    finally:
        client_socket.close()
        for client in clients:
            if client != client_socket:
                client.sendall("Client disconnected".encode())

        clients.remove(client_socket)

def signal_handler(signal, frame):
    # Gracefully close all connections and threads
    for client in clients:
        client.close()

    for thread in threads:
        thread.join()

    server_socket.close()
    print("Shutting down server...")
    exit(0)

signal.signal(signal.SIGINT, signal_handler)

while True:
    client_socket, client_address = server_socket.accept()
    clients.append(client_socket)

    client_thread = threading.Thread(target=handle_client, args=(client_socket,))
    threads.append(client_thread)
    client_thread.start()

    # Wait for all threads to finish before closing the server socket
    for thread in threads:
        thread.join()

    server_socket.close()
