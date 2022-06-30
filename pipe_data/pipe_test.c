int main(void)
{  
  struct sockaddr_un address;
  int socket_fd, connection_fd;
  socklen_t address_length;
  pid_t child;  

  char buffer[256];
  int n;

  socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd < 0){
      printf("socket() failed\n");
      return 1;
  } 

  unlink("/tmp/demo_socket");
  memset(&address, 0, sizeof(struct sockaddr_un));

  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, UNIX_PATH_MAX, "/tmp/demo_socket");

  if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
      printf("bind() failed\n");
      return 1;
  }
  if(listen(socket_fd, 5) != 0) {
      printf("listen() failed\n");
      return 1;
  }

  address_length = sizeof(address);  
  while((connection_fd = accept(socket_fd, 
                   (struct sockaddr *) &address,
                   &address_length)) > -1)
  {
      printf("successfully received data\n");
      bzero(buffer,256);
      n = read(connection_fd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
         printf("Here is the message: %s\n\n", buffer);
      strcpy(buffer, "Hi back from the C world"); 
      n = write(connection_fd, buffer, strlen(buffer));
      if (n < 0) 
         printf("ERROR writing to socket\n");           
      break;      
  }
  close(socket_fd);
  return(0);
}

