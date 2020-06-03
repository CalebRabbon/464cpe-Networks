/* Server stop and wait code - Writen: Hugh Smith */
// bunch of #includes go here
#include <sys/types.h>
#include <sys/wait.h>
#include "networks.h"
#include "srej.h"
#include "cpe464.h"
#include "buffer.h"

typedef enum State STATE;
enum State
{
   START, DONE, FILENAME, CHECK_WINDOW, SEND_DATA, WAIT_ON_ACK, CHECK_RR_SREJ, TIMEOUT_ON_ACK, WAIT_ON_EOF_ACK, TIMEOUT_ON_EOF_ACK
};
void process_server(int serverSocketNumber);
void process_client(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len, Connection *
      client);
STATE filename(Connection * client, uint8_t * buf, int32_t recv_len, int32_t * data_file,
      int32_t * win_size, int32_t * buf_size);
STATE send_data(Connection * client, uint8_t * packet, int32_t * packet_len, int32_t data_file,
      int32_t buf_size, uint32_t * seq_num, WindowElement* window, uint32_t windowSize);
STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE timeout_on_eof_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE wait_on_ack(Connection * client);
STATE wait_on_eof_ack(Connection * client);
int processArgs(int argc, char ** argv);

int main ( int argc, char *argv[])
{
   int32_t serverSocketNumber = 0;
   int portNumber = 0;
   portNumber = processArgs(argc, argv);
   sendtoErr_init(atof(argv[1]), DROP_OFF, FLIP_OFF, DEBUG_ON, RSEED_OFF);
   /* set up the main server port */
   serverSocketNumber = udpServerSetup(portNumber);
   process_server(serverSocketNumber);
   return 0;
}

void process_server(int serverSocketNumber)
{
   pid_t pid = 0;
   int status = 0;
   uint8_t buf[MAX_LEN];
   Connection client;
   uint8_t flag = 0;
   uint32_t seq_num =0;
   int32_t recv_len = 0;
   // get new client connection, fork() child, parent processes nonblocking
   // waitpid()
   while (1)
   {
      // block waiting for a new client
      if (select_call(serverSocketNumber, LONG_TIME, 0, NOT_NULL) == 1)
      {
         recv_len = recv_buf(buf, MAX_LEN, serverSocketNumber, &client, &flag, &seq_num);
         if (recv_len != CRC_ERROR)
         {
            if ((pid = fork()) < 0)
            {
               perror("fork");
               exit(-1);
            }
            if (pid == 0)
            {
               // child process - a new process for each client
               printf("Child fork() - child pid: %d\n", getpid());
               process_client(serverSocketNumber, buf, recv_len, &client);
               exit(0); }
         }
      }
      // check to see if any children quit (only get here in the parent process)
      while (waitpid(-1, &status, WNOHANG) > 0)
      {
      }
   }
}

/*
void printFileName(uint8_t * packet){
   packet += 7;
   printf("buf_size = %i\n", ntohl(((uint32_t*)packet)[0]));
}
*/

STATE check_window(WindowElement* window, uint32_t win_size)
{
   if(isWindowEmpty(window, win_size) == EMPTY){
      return SEND_DATA;
   }
   return CHECK_RR_SREJ;
}

void process_client(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len, Connection *
      client)
{
   WindowElement* window;
   STATE state = START;
   int32_t data_file = 0;
   int32_t packet_len = 0;
   uint8_t packet[MAX_LEN];
   int32_t win_size = 0;
   int32_t buf_size = 0;
   uint32_t seq_num = START_SEQ_NUM;
   while (state != DONE)
   {
      switch (state)
      {
         case START:
            state = FILENAME;
            break;
         case FILENAME:
            state = filename(client, buf, recv_len, &data_file, &win_size, &buf_size);
            window = createWindow(win_size);
            break;
         case CHECK_WINDOW:
            state = check_window(window, win_size);
            break;
         case SEND_DATA:
            state = send_data(client, packet, &packet_len, data_file, buf_size, &seq_num, window, win_size);
            break;
            /*
         case CHECK_RR_SREJ:
            state = check_rr_srej();
            break;
            */
         case WAIT_ON_ACK:
            state = wait_on_ack(client);
            break;
         case TIMEOUT_ON_ACK:
            state = timeout_on_ack(client, packet, packet_len);
            break;
         case WAIT_ON_EOF_ACK:
            state = wait_on_eof_ack(client);
            break;
         case TIMEOUT_ON_EOF_ACK:
            state = timeout_on_eof_ack(client, packet, packet_len);
            break;
         case DONE:
            break;
         default:
            printf("In default and you should not be here!!!!\n");
            state = DONE;
            break;
      }
   }
}

STATE filename(Connection * client, uint8_t * buf, int32_t recv_len, int32_t * data_file,
      int32_t * win_size, int32_t * buf_size)
{
   uint8_t response[1];
   char fname[MAX_LEN];
   STATE returnValue = DONE;

   // extract buffer sized used for sending data and also filename

   memcpy(win_size, buf, SIZE_OF_WIN_SIZE);
   *win_size = ntohl(*win_size);
   printf("WinSize = %i\n", *win_size);

   memcpy(buf_size, &buf[SIZE_OF_WIN_SIZE], SIZE_OF_BUF_SIZE);
   *buf_size = ntohl(*buf_size);
   printf("BufSize = %i\n", *buf_size);

   memcpy(fname, &buf[sizeof(*win_size) + sizeof(*buf_size)], recv_len - SIZE_OF_WIN_SIZE - SIZE_OF_BUF_SIZE);
   printf("FName = %s\n", fname);

   /* Create client socket to allow for processing this particular client */
   client->sk_num = safeGetUdpSocket();
   if (((*data_file) = open(fname, O_RDONLY)) < 0)
   {
      send_buf(response, 0, client, FNAME_BAD, 0, buf);
      returnValue = DONE;
   } else
   {
      send_buf(response, 0, client, FNAME_OK, 0, buf);
      returnValue = CHECK_WINDOW;
   }
   return returnValue;
}

STATE send_data(Connection *client, uint8_t * packet, int32_t * packet_len, int32_t data_file, int buf_size, uint32_t * seq_num, WindowElement* window, uint32_t windowSize)
{
   uint8_t buf[MAX_LEN];
   int32_t len_read = 0;
   STATE returnValue = DONE;
   WindowElement element;
   len_read = read(data_file, buf, buf_size);
   switch (len_read)
   {
      case -1:
         perror("send_data, read error");
         returnValue = DONE;
         break;
      case 0:
         (*packet_len) = send_buf(buf, 1, client, END_OF_FILE, *seq_num, packet);
         returnValue = WAIT_ON_EOF_ACK;
         break;
      default:
         (*packet_len) = send_buf(buf, len_read, client, DATA, *seq_num, packet);
         createWindowElement(&element, buf, len_read);
         addElement(*seq_num, element, window, windowSize);
         printWindow(window, windowSize);
         (*seq_num)++;
         returnValue = WAIT_ON_ACK;
         break;
   }
   return returnValue;
}

STATE wait_on_ack(Connection * client)
{
   STATE returnValue = DONE;
   uint32_t crc_check = 0;
   uint8_t buf[MAX_LEN];
   int32_t len = MAX_LEN;
   uint8_t flag = 0;
   uint32_t seq_num = 0;
   static int retryCount = 0;
   if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_ACK, SEND_DATA, DONE)) ==
         SEND_DATA)
   {
      crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);
      // if crc error ignore packet

      if (crc_check == CRC_ERROR)
      {
         returnValue = WAIT_ON_ACK;
      }
      else if (flag != ACK)
      {
         printf("In wait_on_ack but its not an ACK flag (this should never happen) is %d\n", flag);
         returnValue = DONE;
      }
   }
   return returnValue;
}
STATE wait_on_eof_ack(Connection * client)
{
   STATE returnValue = DONE;
   uint32_t crc_check = 0;
   uint8_t buf[MAX_LEN];
   int32_t len = MAX_LEN;
   uint8_t flag = 0;
   uint32_t seq_num = 0;
   static int retryCount = 0;
   if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_EOF_ACK, DONE, DONE)) ==
         DONE)
   {
      crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);
      // if crc error ignore packet

      if (crc_check == CRC_ERROR)
      {
         returnValue = WAIT_ON_EOF_ACK;
      }
      else if (flag != EOF_ACK)
      {
         printf("In wait_on_eof_ack but its not an EOF_ACK flag (this should never happen) is: %d\n", flag);
         returnValue = DONE;
      }
      else
      {
         printf("File transfer completed successfully.\n");
         returnValue = DONE;
      }
   }
   return returnValue;
}
STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len)
{
   safeSendto(packet, packet_len, client);
   return WAIT_ON_ACK;
}
STATE timeout_on_eof_ack(Connection * client, uint8_t * packet, int32_t packet_len)
{
   safeSendto(packet, packet_len, client);
   return WAIT_ON_EOF_ACK;
}

int processArgs(int argc, char ** argv)
{
   int portNumber = 0;
   if (argc < 2 || argc > 3)
   {
      printf("Usage %s error_rate [port number]\n", argv[0]);
      exit(-1);
   }
   if (argc == 3)
   {
      portNumber = atoi(argv[2]);
   }
   else
   {
      portNumber = 0;
   }
   return portNumber;
}
