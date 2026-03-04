import socket
import time

HOST = "0.0.0.0"
PORT = 5000

try:
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
except socket.error as err:
    print("Socket creation failed:", err)
    exit()

try:
    server.bind((HOST, PORT))
except socket.error as err:
    print("Bind failed:", err)
    exit()

server.listen(1)
print("Server waiting for connection...")

try:
    conn, addr = server.accept()
    print("Connected from:", addr)

    
    conn.send(b"Connection Established! AI takes over now.")

   
    conn.setblocking(False)

    while True:

       
        try:
            data = conn.recv(1024)

            if data:
                print("Rexxy State:", data.decode())

        except BlockingIOError:
           
            pass

        except Exception as e:
            print("Client disconnected:", e)
            break


       
        ai_action = "MOVE_LEFT"  

        try:
            conn.send(ai_action.encode())

        except BlockingIOError:
            pass

        except socket.error:
            print("Client disconnected while sending")
            break


       
        time.sleep(0.1)


except socket.error as err:
    print("Connection error:", err)

except KeyboardInterrupt:
    print("\nServer stopped")

finally:
    try:
        conn.close()
    except:
        pass

    server.close()