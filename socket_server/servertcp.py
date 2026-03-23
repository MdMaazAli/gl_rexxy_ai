import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from replay_buffer.replay_buffer import ReplayBuffer
import socket
import time
import random
import struct

HOST = "0.0.0.0"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind((HOST, PORT))
server.listen(1)

print("Server waiting for connection...")

conn, addr = server.accept()
print("Connected from:", addr)

conn.send(b"Connection Established!")

 
conn.setblocking(True)

buffer = ReplayBuffer(10000)

prev_state = None
prev_action = None

while True:
    try:
       
        data = conn.recv(24)  # 6 floats * 4 bytes

        if not data or len(data) < 24:
            continue

        state = struct.unpack('6f', data)
        print("State:", state)

        reward = 1
        done = False

        if prev_state is not None:
            experience = (prev_state, prev_action, reward, state, done)
            buffer.add(experience)
            print("Buffer size:", buffer.size())

      
        action = random.randint(0, 1)

        prev_state = state
        prev_action = action

      
        conn.sendall(struct.pack('i', action))

    except Exception as e:
       # print("Error:", e)
        break

    time.sleep(0.1)

conn.close()
server.close()