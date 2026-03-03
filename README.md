# GL_REXXY: 3D Reinforcement Learning Agent

**Rexxy AI** is a high-performance 3D autonomous agent mastering a custom **OpenGL** environment through **Reinforcement Learning (RL)** optimized with Prioritized Experience Replay and Polyak Averaging.

## Project Overview
**GL_REXXY** is an autonomous agent designed to navigate a custom-rendered 3D world built from the ground up. By decoupling the C++ environment from the Python-based intelligence, the system achieves high-speed "Headless" training and modular scalability.

## Technical Architecture
* **Environment**: A custom 3D platformer engine developed in **C++** using **OpenGL**, featuring granular physics and state-space extraction.
* **Intelligence Core**: A Deep Q-Network (DQN) that processes dynamic state vectors including obstacle distance, vertical height, and velocity.
* **Optimization Strategy**: Implements **Prioritized Experience Replay (PER)** using a SumTree data structure and **Polyak Averaging** for stable, smooth target network updates.
* **Communication Layer**: High-speed Inter-Process Communication (IPC) via localhost sockets, allowing for parallel execution of the game and the brain.

## Roadmap
* **Phase 0: Infrastructure**: Repository setup, licensing, and environment isolation (Completed).
* **Phase 1: Brain Development**: Implementing the Keras-based DQN architecture and PER buffer.
* **Phase 2: Handshake**: Establishing the C++ to Python socket communication protocol.
* **Phase 3: Training & Refinement**: Headless mode optimization and rewards shaping.

## License & Contributors
* **License**: This project is licensed under the **MIT License**.
* **Authors**: Developed by **Md Maaz Ali Haider** as the Lead Architect. Detailed roles are available in [CONTRIBUTORS.md](./CONTRIBUTORS.md).
