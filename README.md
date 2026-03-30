# GL_REXXY: 3D Reinforcement Learning Agent

**Rexxy AI** is an autonomous 3D agent mastering a custom-rendered environment through **Reinforcement Learning**. Built with a decoupled architecture, the project bridges high-performance C++ physics with Python-based neural intelligence.

---

## Technical Architecture

* **Environment**: A custom 3D platformer engine developed in **C++** using **OpenGL 3.3**, featuring a decoupled physics loop and real-time state-space extraction.
* **Intelligence Core**: A **Vanilla Deep Q-Network (DQN)** utilizing a 3-layer Dense architecture ($6 \to 24 \to 24 \to 2$). The model maps 6 dynamic state variables (distance, speed, height, etc.) to discrete action Q-values.
* **Memory & Learning**: 
    * **Uniform Experience Replay**: Utilizes a cyclic buffer to store and sample past transitions, breaking temporal correlation during training.
    * **Bellman Optimization**: Employs the standard Bellman equation with a $0.95$ discount factor ($\gamma$) to calculate temporal difference targets.
* **Communication Layer**: High-speed Inter-Process Communication (IPC) via localhost sockets, enabling real-time inference and asynchronous training batches.

---

## Current Status & Dev Log

> **Note to Collaborators**: This section is for raw updates on how Rexxy is doing.

Currently rexxy is stable for varibale velocities.

**The FPS Hurdle**: Performance has improved from **10-12 FPS** to **20-22 FPS** through initial code cleanup. However, achieving a stable 60 FPS remains the priority. The bottleneck is the Keras `.predict()` overhead; the next major push is bypassing high-level API calls for raw TensorFlow execution.

---

## Roadmap

### **Phase 1: Foundation**
* OpenGL 3D rendering pipeline and custom shader implementation.
* C++ Physics engine with collision detection and state-data broadcasting.
* Socket-based handshake protocol between C++ (Client) and Python (Server).

### **Phase 2: Core Intelligence (Active)**
* Implementation of the Vanilla DQN Agent using Keras/TensorFlow.
* Uniform Experience Replay buffer integration.
* Persistence system for saving/loading model weights (`.keras`) with Auto-Save protection.
* **Optimization**: Reducing inference latency to achieve a stable 60 FPS performance.

### **Phase 3: Advanced Optimization (Planned)**
* **Stability**: Transition to a **Double DQN (DDQN)** architecture to reduce Q-value overestimation.
* **Efficiency**: Implementation of **Prioritized Experience Replay (PER)** using a SumTree.
* **Smoothing**: Integration of **Polyak Averaging** (Soft Updates) for target network synchronization.

### **Phase 4: Scaling & Generalization**
* Multi-obstacle patterns and procedural terrain generation.
* "Headless" training mode for accelerated GPU-bound learning.

---

## Tech Stack
* **Language**: C++ (Engine), Python (AI)
* **Graphics**: OpenGL, GLAD, GLFW, GLM
* **AI/ML**: TensorFlow, Keras, NumPy
* **Sockets**: WinSock32
* **GUI**: Dear ImGui (Matrix Telemetry Dashboard)

---

## License & Contributors
* **License**: MIT License
* **Authors**: Developed by **Md Maaz Ali Haider** (Lead Architect). 
* **Contributors**: Detailed roles and contributions are available in [CONTRIBUTORS.md](./CONTRIBUTORS.md).
