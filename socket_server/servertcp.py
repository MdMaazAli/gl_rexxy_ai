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
 
conn.setblocking(True)

buffer = ReplayBuffer(10000)

prev_state = None
prev_action = None

while True:
    try:
       
        data = conn.recv(32)  # 8 floats * 4 bytes

        if not data or len(data) < 32:
            continue

        unpack_data = struct.unpack('8f', data)

        # current state and reward variables
        currState = unpack_data[0:6]
        reward = unpack_data[6]
        done = False;

        if prev_state is not None:
            experience = (prev_state, prev_action, reward, currState, done)
            buffer.add(experience)
            # print("Buffer size:", buffer.size())
      
        action = random.randint(0, 1)

        prev_state = currState
        prev_action = action
      
        conn.sendall(struct.pack('i', action))

    except Exception as e:
       # print("Error:", e)
        break

conn.close()
server.close()