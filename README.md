# Multiplayer Third-Person Shooter (Unreal Engine 5)

A third-person multiplayer shooter developed in Unreal Engine 5 with a strong focus on network architecture and latency compensation techniques. The project is implemented primarily in C++ to ensure performance, control, and a deeper understanding of Unreal’s networking framework.

The main goal of this project was to experiment with and implement advanced multiplayer systems that maintain smooth and responsive gameplay even under high-latency conditions.

---

# Features

- Third-person multiplayer shooter gameplay
- Authoritative server architecture
- Server-Side Rewind for accurate hit validation
- Client-Side Prediction for responsive movement
- Lag compensation techniques
- Steamworks API integration (no manual port forwarding required)
- Session-based multiplayer support

---

# Networking Architecture

The project focuses heavily on multiplayer best practices:

### Server Authority
All critical gameplay logic (damage, hit detection, movement validation) is handled server-side to prevent cheating and maintain fairness.

### Client-Side Prediction
Movement is predicted locally on the client to eliminate perceived input delay, then reconciled with the server state.

### Server-Side Rewind
The server stores historical player positions and rewinds them during hit validation to compensate for latency, ensuring fair hit registration even with high ping.

---

# Tech Stack

- Unreal Engine 5  
- C++  
- Unreal Networking Framework  
- Steamworks API  

---

# Core Systems

- Character movement replication
- Weapon and projectile systems
- Hit detection and validation
- Latency compensation
- Multiplayer session management
- Steam-based matchmaking and networking

---

# Build & Run

1. Clone the repository  
2. Open the `.uproject` file with Unreal Engine 5  
3. Build the project (Visual Studio / Rider recommended)  
4. Launch through the Unreal Editor or packaged build  

Steam integration requires valid Steam App ID configuration.

---

# What This Project Demonstrates

- Advanced multiplayer networking concepts  
- Low-level gameplay programming in C++  
- Latency compensation techniques  
- Understanding of client-server architecture  
- Integration of third-party APIs (Steamworks)  
- Multiplayer debugging and testing  

---

# Future Improvements

- Dedicated server deployment  
- Ranked matchmaking system  
- Advanced anti-cheat validation  
- Replay system  
- Improved animation blending and polish  
