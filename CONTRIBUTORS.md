# Project Roles & Contributors

This document outlines the core contributions and architectural responsibilities for the `gl_rexxy_ai` environment.

## Core Team

* **Md Maaz Ali Haider** (@MdMaazAli) - **Project Founder & Lead Architect**
  * **Core Engine:** Engineered the C++ OpenGL environment, including window management, deterministic physics simulation, and state-space definitions.
  * **Brain Architecture:** Implemented the Vanilla DQN intelligence core, including the Bellman optimality logic and the "Surgical Edit" update method for the Q-matrix.
  * **Persistence & Safety:** Developed the model persistence system (`.keras`) featuring the "Backup Brain" loading logic and the auto-save safety triggers.
  * **IPC Bridge:** Implemented the Winsock-based TCP client to transmit state payloads and receive action decisions from the engine.
  * **Architecture Stabilization:** Resolved critical synchronization bugs, decoupling the 60Hz rendering loop from the network logic to resolve early-stage framerate drops.

* **Ritesh Ranjan** (@Riteshiiitbh24) - **Lead Optimizer & Performance Engineer**
  * **Performance Engineering:** Responsible for the Phase 2 optimization push, focused on reducing inference latency and bypassing Keras overhead to reach a stable 60 FPS.
  * **Python Backend:** Established the Python-side TCP server to receive, unpack, and process binary payloads from the C++ engine.
  * **Data Pipeline:** Engineered the ReplayBuffer structure to manage experience memory (state, action, reward, next_state, done) for the neural network.
  * **Optimization Infrastructure:** Managing high-speed data formatting and the transition toward raw TensorFlow execution paths for accelerated training.

## Contribution Policy
Detailed contribution metrics and specific component ownership will continue to be updated as the project moves through the active Performance Optimization phase and into Advanced Brain Architectures.
