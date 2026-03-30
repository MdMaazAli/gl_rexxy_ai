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
    # used to watch forged in fire kinda analogus
    # scrap metal -> hammering -> melting/smelting -> cooling -> heating again -> quenching -> look for cracks if there whole seq agian
    # ----------------------------------------------------------------if quenching is successfull then test the blade its sharpness,balance and strength
    # rl model -> initilaly drunk with high epsilon -> guesses randomly -> tries to find an optimal policy -> via bellman optimality eqn (here using q-learning) -> if it found one optimal policy -> test in inference mode(epsilon = 0) -> if perofrms well then good if not then it needs some tweaking
    # (-_-) (above analogy is just a fun little thought experiment there are some descripencies involved like we don't usually reset the whole model when we get stuck as we melt a balde)
    
    def __init__(self,state_size = 6,action_size = 2,isTrainMode = 1) :
        self.state_size = 6
        self.action_size = 2
        self.batch_size = 64
        self.isTrainMode = isTrainMode

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
        state_tensor = tf.convert_to_tensor(state, dtype=tf.float32)
        state_tensor = tf.reshape(state_tensor, (1, -1))
        act_values = self.model.predict(state_tensor,verbose = 0) # predict action values
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
        # Q(sk) (current values of Q for this batch)
        q_values = self.model.predict(states,verbose = 0)

        # Q(sk+1) (next values of Q for this batch)
        next_q_values = self.model.predict(next_states,verbose = 0)

        # Bellman Loop 
        # based on classical Q learning Q new(sk,ak) = Q old(sk,ak) + alpha*(r(k) + y*(max over a)Q(sk+1,a) - Q old(sk,ak))
        # but here in DQN we just calculate the TD target estimate----------{    this fella above here     } 
        # as sort of preparing a target value for our neural netwrok to do its thing(backpropagation) as we don't have an actual value in case of rl
        # sums up the whole process mostly
        for i in range(self.batch_size) :
            target = rewards[i]

            if not dones[i] :
                target += self.gamma * np.max(next_q_values[i])
            
            q_values[i][actions[i]] = target

        # Hammer(backpropagation)
        self.model.fit(states,q_values,epochs = 1,verbose = 0)

        # Decay
        if self.epsilon > self.epsilon_min :
            self.epsilon *= self.epsilon_decay

    def save_model(self, filename="rexxy_brain.keras"):
        self.model.save(filename)
        print(f"Brain saved securely to {filename}")

    def load_model(self, filename="rexxy_brain.keras"):
        if os.path.exists(filename):
            # loading the backup model
            self.model = models.load_model(filename)

            if self.isTrainMode == 1 :
                self.epsilon = self.epsilon_min 
            else :
                self.epsilon = 0

            print(f"Loaded existing brain from {filename}")
        else:
            print("No existing brain found. Starting fresh.")

isTrainMode = int(input("train:"))

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

# Initialize the Brain and try to load past memories
agent = DQNAgent(state_size=6, action_size=2,isTrainMode=isTrainMode)
agent.load_model("rexxy_brain_run_var_speed.keras")

prev_state = None
prev_action = None

# tracking the survival time
frame_counter = 0  
episode_frames = 0 

# log file for the dashboard
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

        # frames survived
        episode_frames += 1 

        # TELEMETRY: If it dies, log the score and reset
        if done:
            with open("training_log.csv", "a") as f:
                f.write(f"{frame_counter},{episode_frames},{agent.epsilon:.4f}\n")
            episode_frames = 0 

            # training the brain at the end of each episode rather than some frame intervals
            # some optimisation stuff ( ' ^ ' )
            if isTrainMode == 1:
                # traing for like 5 times on the generated replay buffer
                # 5 here is an experimentral value
                for i in range(5):
                    agent.train(buffer)

        # Add memory to the buffer
        if prev_state is not None:
            experience = (prev_state, prev_action, reward, currState, done)
            buffer.add(experience)
      
        # Ask the AI for the next move
        state_for_net = np.array(currState).reshape(1, -1)
        action = agent.act(state_for_net)

        # Train the Brain
        # toggle it on for training
        # if isTrainMode == 1 :
        #     if frame_counter % 4 == 0: # train every 4th frame to let C++ run at 60fps (some optimisation stuff)
        #         agent.train(buffer)
        
        # AUTO-SAVE 
        frame_counter += 1
        if frame_counter % 5000 == 0 and isTrainMode == 1:
            print(f"\n[AUTO-SAVE] {frame_counter} frames reached. Backing up weights...")
            agent.save_model("rexxy_brain.keras") 

        # Shift the timeline forward
        prev_state = currState
        prev_action = action
      
        # Send to main.cpp
        conn.sendall(struct.pack('i', action))

# GRACEFUL SHUTDOWN
except KeyboardInterrupt:
    print("\n[SHUTDOWN] Manual exit triggered.")
    
    if isTrainMode == 1:
        # Only save if we actually altered the brain
        timestamp = int(time.time())
        backup_name = f"rexxy_brain_run_{timestamp}.keras"
        agent.save_model(backup_name)
        print(f"Final brain state protected as: {backup_name}")
    else:
        print("Inference Mode: No new learning to save. Exiting cleanly.")

except Exception as e:
    print("\n[CRITICAL ERROR] Python crashed:", e)
    print("Attempting emergency brain save...")
    agent.save_model("rexxy_brain_emergency.keras")

finally:
    conn.close()
    server.close()
    print("Server connection closed safely.")
    