import struct
import socket
import signal

udp_package_format = 'I Q B H Q'

UDP_FLAG_SET_BID = lambda flag: flag | 0b01
UDP_FLAG_SET_ASK = lambda flag: flag & ~0b01
UDP_FLAG_GET_BOOK = lambda flag: (flag & 0b01) != 0

UDP_FLAG_SET_STATUS = lambda flag, s: (flag & ~0b01110) | ((s & 0b0111) << 1)
UDP_FLAG_GET_STATUS = lambda flag: (flag >> 1) & 0b0111

UDP_FLAG_SET_SOLD = lambda flag, s: (flag & ~0b010000) | ((s & 0b01) << 4)
UDP_FLAG_GET_IS_SOLD = lambda flag: (flag >> 4) & 0b01


# Create a UDP socket
print('here')
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print('create')
udp_socket.bind(('127.0.0.1', 8081))
print('bind')

def signal_handler(signal, frame):
    udp_socket.close()
    print("Shutting down server...")
    exit(0)

signal.signal(signal.SIGINT, signal_handler)


while True:
    print('recv')
    data, addr = udp_socket.recvfrom(struct.calcsize(udp_package_format))
    print(data, addr)

    unpacked_data = struct.unpack(udp_package_format, data)
    print(unpacked_data)

    time, id, flag, quantity, price = unpacked_data

    is_bid = UDP_FLAG_GET_BOOK(flag)
    status = UDP_FLAG_GET_STATUS(flag)
    is_sold = UDP_FLAG_GET_IS_SOLD(flag)

    print(f'Time: {time}, ID: {id}, Flag: {flag}, Quantity: {quantity}, Price: {price}')
    print(f'Is Bid: {is_bid}, Status: {status}, Is Sold: {is_sold}')
    print('-------------------------------------------------')
