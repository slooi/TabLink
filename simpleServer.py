import socket

def handle_client(client_socket, addr):
    """Handle individual client connection and receive multiple messages."""
    print(f"Accepted connection from {addr}")
    
    try:
        while True:
            # Receive data from client
            data = client_socket.recv(1024).decode()
            
            # If no data received (client disconnected), break the loop
            if not data:
                print(f"Client {addr} disconnected")
                break
                
            print(f"Received Pressure from {addr}:", data)
            
    except ConnectionResetError:
        print(f"Client {addr} forcibly closed the connection")
    except Exception as e:
        print(f"Error handling client {addr}:", e)
    finally:
        client_socket.close()

def main():
    # Create server socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Allow reuse of the address
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    # Bind to all available network interfaces
    server.bind(("0.0.0.0", 54000))
    server.listen(5)
    
    # Print server information
    host_name = socket.gethostname()
    host_ip = socket.gethostbyname(host_name)
    print(f"Server hostname: {host_name}")
    print(f"Server IP: {host_ip}")
    print("Listening on all interfaces (0.0.0.0) port 54000...")
    
    try:
        while True:
            # Accept new connections
            client, addr = server.accept()
            # Handle each client
            handle_client(client, addr)
            
    except KeyboardInterrupt:
        print("\nShutting down server...")
    finally:
        server.close()

if __name__ == "__main__":
    main()