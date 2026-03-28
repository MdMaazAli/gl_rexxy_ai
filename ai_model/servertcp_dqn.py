import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, models, optimizers
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from replay_buffer.replay_buffer import ReplayBuffer
import socket
import time
import random
import struct

class DQNAgent :
    # training a rl model is kinda like forging a blade

    def __init__(self,state_size = 6,action_size = 2) :
        self.state_size = 6
        self.action_size = 2
        self.batch_size = 64

        # Hyperparameters
        self.gamma = 0.95
        self.epsilon = 1.0
        self.epsilon_min = 0.01
        self.epsilon_decay = 0.995
        self.learning_rate = 0.001

        self.model = self._build_model()

    def _build_model(self) :
        # model
        model = models.Sequential([
            layers.Input(shape=(self.state_size,)),
            layers.Dense(24,activation='relu'),
            layers.Dense(24,activation='relu'),
            layers.Dense(self.action_size,activation='linear')
        ])
        model.compile(loss = 'mse',optimizer = optimizers.Adam(learning_rate = self.learning_rate))
        return model
    
    # action decider
    def act(self,state) :
        # exploration
        if np.random.rand() <= self.epsilon :
            return random.randrange(self.action_size)
        
        # exploitation 
        state = np.array(state).reshape(1,-1) # transforms it into a batch as tensorflow is designed to handle batches rather than points
        act_values = self.model.predict(state,verbose = 0) # predict action values
        return np.argmax(act_values[0]) # max score between run vs jump
    
    def train(self,replay_buffer) :
        if replay_buffer.size() < self.batch_size :
            return
        
        batch = replay_buffer.sample(self.batch_size)

        states,actions,rewards,next_states,dones = zip(*batch)

        states = np.array(states)
        next_states = np.array(next_states)
        rewards = np.array(rewards)
        actions = np.array(actions)
        dones = np.array(dones)

        # quality
        # Q(sk) (current value of Q)
        q_values = self.model.predict(states,verbose = 0)

        # Q(sk+1) (next value of Q)
        next_q_values = self.model.predict(next_states,verbose = 0)

        # Bellman Loop 
        # Q new(sk,ak) = Q old(sk,ak) + alpha*(r(k) + y*(max over a)Q(sk+1,a) - Q old(sk,ak))
        for i in range(self.batch_size) :
            target = rewards[i]

            if not dones[i] :
                target += self.gamma * np.max(next_q_values[i])
            
            q_values[i][actions[i]] = target

        # Hammer
        self.model.fit(states,q_values,epochs = 1,verbose = 0)

        # Decay
        if self.epsilon > self.epsilon_min :
            self.epsilon *= self.epsilon_decay

    def save_model(self, filename="rexxy_brain.keras"):
        self.model.save(filename)
        print(f"Brain saved securely to {filename}")

    def load_model(self, filename="rexxy_brain.keras"):
        if os.path.exists(filename):
            self.model = models.load_model(filename)
            # If we load a smart brain, we don't want it to act completely drunk again
            self.epsilon = self.epsilon_min 
            print(f"Loaded existing brain from {filename}")
        else:
            print("No existing brain found. Starting fresh.")

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

# 1. Initialize the Brain and try to load past memories
agent = DQNAgent(state_size=6, action_size=2)
agent.load_model("rexxy_brain.keras")

prev_state = None
prev_action = None
frame_counter = 0  
episode_frames = 0 # <--- TRACKS SURVIVAL TIME

# <--- Create the log file for the dashboard
with open("training_log.csv", "w") as f:
    f.write("TotalFrames,SurvivalScore,Epsilon\n")

try:
    while True:
        data = conn.recv(32)  

        if not data or len(data) < 32:
            continue

        unpack_data = struct.unpack('8f', data)

        currState = unpack_data[0:6]
        reward = unpack_data[6]
        done = unpack_data[7]

        # The AI survived another frame!
        episode_frames += 1 

        # <--- TELEMETRY: If it dies, log the score and reset
        if done:
            with open("training_log.csv", "a") as f:
                f.write(f"{frame_counter},{episode_frames},{agent.epsilon:.4f}\n")
            episode_frames = 0 

        # 2. Add memory to the buffer
        if prev_state is not None:
            experience = (prev_state, prev_action, reward, currState, done)
            buffer.add(experience)
      
        # 3. Ask the AI for the next move (Lightning Fast)
        state_for_net = np.array(currState).reshape(1, -1)
        action = agent.act(state_for_net)

        # 4. Train the Brain (Heavy Math)
        # <--- THE LAG FIX: Only train every 4th frame to let C++ run at 60fps
        if frame_counter % 4 == 0:
            agent.train(buffer)
        
        # 5. THE AUTO-SAVE PROTOCOL
        frame_counter += 1
        if frame_counter % 5000 == 0:
            print(f"\n[AUTO-SAVE] {frame_counter} frames reached. Backing up weights...")
            agent.save_model("rexxy_brain.keras") 

        # 6. Shift the timeline forward
        prev_state = currState
        prev_action = action
      
        # 7. Send to C++
        conn.sendall(struct.pack('i', action))

# 8. GRACEFUL SHUTDOWN
except KeyboardInterrupt:
    print("\n[SHUTDOWN] Manual exit triggered. Saving final brain state...")
    agent.save_model("rexxy_brain.keras")

except Exception as e:
    print("\n[CRITICAL ERROR] Python crashed:", e)
    print("Attempting emergency brain save...")
    agent.save_model("rexxy_brain_emergency.keras")

finally:
    conn.close()
    server.close()
    print("Server connection closed safely.")