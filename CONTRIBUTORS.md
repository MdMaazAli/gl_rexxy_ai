# Project Roles & Contributors

This document outlines the core contributions and architectural responsibilities for the `gl_rexxy_ai` environment.

## Core Team

* **Md Maaz Ali Haider** (@MdMaazAli) - **Project Founder & Lead Architect**
  * **Core Engine:** Engineered the C++ OpenGL environment, including window management, the deterministic physics simulation, and state-space definitions.
  * **IPC Bridge:** Implemented the Winsock-based TCP client to transmit state payloads from the engine.
  * **Architecture Stabilization :** Resolved critical synchronization bugs, specifically fixing TCP blocking issues and decoupling the 60Hz rendering loop from the 30Hz network loop to eliminate framerate drops.

* **Ritesh Ranjan** (@Riteshiiitbh24) - **Lead Optimizer & AI Backend**
  * **Python Backend:** Established the Python-side TCP server to receive, unpack, and process binary payloads from the C++ engine.
  * **Data Pipeline:** Engineered the ReplayBuffer structure to manage experience memory (state, action, reward, next_state, done) for the neural network.
  * **Infrastructure:** Responsible for ongoing IPC stability, data formatting, and preparing the foundation for Deep Q-Network training.

## Contribution Policy
Detailed contribution metrics and specific component ownership will continue to be updated as the project moves fully through Core Development and into active model training.
