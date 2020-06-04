/* Server stop and wait code - Writen: Hugh Smith */
// bunch of #includes go here
#include <sys/types.h>
#include <sys/wait.h>
#include "networks.h"
#include "srej.h"
#include "cpe464.h"
#include "buffer.h"

/*
#define PRINT
*/
#define WAITLASTRR
#define PRINTSTATES

typedef struct wptr WPtr;
struct wptr{
   int l;
   int c;
   int u;
};

typedef enum State STATE;
enum State
{
   START, DONE, FILENAME, CHECK_WINDOW, SEND_DATA, WAIT_LAST_RR, CHECK_RR_SREJ, TIMEOUT_ON_ACK, WAIT_ON_EOF_ACK, TIMEOUT_ON_EOF_ACK
};
void process_server(int serverSocketNumber);
void process_client(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len, Connection *
      client);
STATE filename(Connection * client, uint8_t * buf, int32_t recv_len, int32_t * data_file,
      int32_t * windowSize, int32_t * buf_size);
STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE timeout_on_eof_ack(Connection * client, uint8_t * packet, int32_t packet_len);
STATE wait_on_eof_ack(Connection * client, uint32_t expSeqNum);
int processArgs(int argc, char ** argv);

int main ( int argc, char *argv[])
{
   int32_t serverSocketNumber = 0;
   int portNumber = 0;
   portNumber = processArgs(argc, argv);
   sendtoErr_init(atof(argv[1]), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
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
#ifdef PRINT
               printf("buf %s\n", &buf[8]);
               printf("**rec_len %i\n", recv_len);
#endif
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

STATE filename(Connection * client, uint8_t * buf, int32_t recv_len, int32_t * data_file,
      int32_t * windowSize, int32_t * buf_size)
{
   uint8_t response[1];
   char fname[MAX_LEN];
   STATE returnValue = DONE;

   // extract buffer sized used for sending data and also filename

   memcpy(windowSize, buf, SIZE_OF_WIN_SIZE);
   *windowSize = ntohl(*windowSize);
#ifdef PRINT
   printf("WinSize = %i\n", *windowSize);
#endif

   memcpy(buf_size, &buf[SIZE_OF_WIN_SIZE], SIZE_OF_BUF_SIZE);
   *buf_size = ntohl(*buf_size);
#ifdef PRINT
   printf("BufSize = %i\n", *buf_size);
   printf("Recieved Length = %i\n", recv_len);
#endif

   memcpy(fname, &buf[sizeof(*windowSize) + sizeof(*buf_size)], recv_len - SIZE_OF_WIN_SIZE - SIZE_OF_BUF_SIZE + 1);

   // Null Terminate the string
   fname[recv_len - SIZE_OF_WIN_SIZE - SIZE_OF_BUF_SIZE] = '\0';
#ifdef PRINT
   printf("FName = %s\n", fname);
#endif

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

void resend(Connection* client, WindowElement* window, uint32_t windowSize, uint32_t seq_num){
   uint8_t packet[MAX_LEN];

   WindowElement element;
   getElement(seq_num, &element, window, windowSize);
   if(element.flag == EMPTY){
      element.data_len = 0;
      printf("EMPTY ELEMENT SENDING ZERO DATA\n");
   }

   send_buf(element.data_buf, element.data_len, client, DATA, seq_num, packet);
}

STATE check_window(Connection* client, WindowElement* window, uint32_t windowSize, WPtr* wptr)
{
   STATE returnValue = CHECK_WINDOW;
   static int retryCount = 0;

   if(isWindowFull(window, windowSize) == EMPTY){
      return SEND_DATA;
   }

#ifdef PRINT
   printf(" WIndow is full\n");
#endif

   if ((returnValue = processSelect(client, &retryCount, CHECK_WINDOW, CHECK_RR_SREJ, DONE)) == CHECK_RR_SREJ)
   {
      return returnValue;
   }
   else if(returnValue == CHECK_WINDOW){
      resend(client, window, windowSize, wptr->l);
      return returnValue;
   }
   return returnValue;
}

void updateWPtr(WPtr* wptr, int32_t windowSize, uint32_t seq_num){
   // Update the lower bound
   wptr->l = seq_num;

   // Update the upper bound
   wptr->u = seq_num + windowSize;
}

void removeFromBuffer(WindowElement* window, uint32_t windowSize, uint32_t seq_num, WPtr* wptr){
   int l = wptr->l;

   for (l = wptr->l; l < seq_num; l++){
      deleteElement(l, window, windowSize);
   }
}

void printWPtr(WPtr* wptr){
   printf("L: %i\tC: %i\tU: %i\n", wptr->l, wptr->c, wptr->u);
}

STATE check_rr_srej(Connection *client, WindowElement* window, uint32_t windowSize, WPtr* wptr)
{
   STATE returnValue = CHECK_WINDOW;
   uint32_t crc_check = 0;
   uint8_t buf[MAX_LEN];
   int32_t len = MAX_LEN;
   uint8_t flag = 0;
   uint32_t seq_num = 0;

   //if ((returnValue = processSelect(client, &retryCount, CHECK_WINDOW, CHECK_WINDOW, DONE)) == CHECK_WINDOW)

   if (select_call(client->sk_num, 0, 0, NOT_NULL) == 1)
   {
      crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);
      // if crc error ignore packet

      if (crc_check == CRC_ERROR)
      {
         // Do nothing
         returnValue = CHECK_RR_SREJ;
      }
      else if (flag == FSREJ)
      {
         // Resend the data from SREJ buffer
         resend(client, window, windowSize, seq_num);
      }
      else if (flag == ACK)
      {
         // Remove data from Buffer
         removeFromBuffer(window, windowSize, seq_num, wptr);
         updateWPtr(wptr, windowSize, seq_num);
#ifdef PRINT
         printWPtr(wptr);
#endif
      }
      else
      {
         printf("In check_rr_srej but its not an ACK flag (this should never happen) is %d\n", flag);
         returnValue = DONE;
      }
   }
   return returnValue;
}

STATE send_data(Connection *client, uint8_t * packet, int32_t * packet_len, int32_t data_file, int buf_size, uint32_t * seq_num, WindowElement* window, uint32_t windowSize, WPtr* wptr)
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
         //(*seq_num)++;
         printf("EOF Waiting on seq num %i\n", *seq_num);
         returnValue = WAIT_LAST_RR;
         break;
      default:
         (*packet_len) = send_buf(buf, len_read, client, DATA, *seq_num, packet);
         createWindowElement(&element, buf, len_read);
         addElement(*seq_num, element, window, windowSize);
         //printWindow(window, windowSize);
         (*seq_num)++;
         wptr->c = *seq_num;
         returnValue = CHECK_RR_SREJ;
         break;
   }
   return returnValue;
}

STATE wait_last_rr(Connection * client, uint32_t expSeqNum, WindowElement* window, uint32_t windowSize, WPtr* wptr)
{
   STATE returnValue = DONE;
   uint32_t crc_check = 0;
   uint8_t buf[MAX_LEN];
   uint8_t packet[MAX_LEN];
   int32_t len = MAX_LEN;
   uint8_t flag = 0;
   uint32_t recSeqNum = 0;
   uint32_t expRR = expSeqNum;
   static int retryCount = 0;

#ifdef PRINT
   printf("retryCount = %i\n", retryCount);
#endif
   if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_ACK, WAIT_LAST_RR, DONE)) == WAIT_LAST_RR)
   {
      crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &recSeqNum);

      // if crc error ignore packet
      if (crc_check == CRC_ERROR)
      {
         // Do nothing
         returnValue = WAIT_LAST_RR;
      }
      else if (flag == FSREJ)
      {
         // Resend the data from SREJ buffer
         resend(client, window, windowSize, recSeqNum);
      }
      else if (flag == ACK)
      {
#ifdef WAITLASTRR
         printf("in wait last rr = %i\n", expRR);
#endif
         // Received RR
         if(recSeqNum == expRR){
            // Send EOF
            send_buf(buf, 1, client, END_OF_FILE, expRR, packet);

            returnValue = WAIT_ON_EOF_ACK;
         }
         else{
            // Remove RR from Buffer
#ifdef PRINT
            printf("Removing from buffer seqNum %i\n", recSeqNum);
#endif
            removeFromBuffer(window, windowSize, recSeqNum, wptr);

            returnValue = WAIT_LAST_RR;
         }
      }
      else
      {
#ifdef PRINT
         printf("In wait_last_rr but its not an ACK flag (this should never happen) is %d\n", flag);
#endif
         returnValue = DONE;
      }
   }
   else if (returnValue == TIMEOUT_ON_ACK){
      // Resend the lowest RR
      resend(client, window, windowSize, wptr->l);
      returnValue = WAIT_LAST_RR;
   }
   return returnValue;
}

STATE wait_on_eof_ack(Connection * client, uint32_t expSeqNum)
{
   STATE returnValue = DONE;
   uint32_t crc_check = 0;
   uint8_t buf[MAX_LEN];
   uint8_t packet[MAX_LEN];
   int32_t len = MAX_LEN;
   uint8_t flag = 0;
   uint32_t seq_num = 0;
   static int retryCount = 0;
   if ((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_EOF_ACK, WAIT_ON_EOF_ACK, DONE)) ==
         WAIT_ON_EOF_ACK)
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
         returnValue = WAIT_ON_EOF_ACK;
      }
      else
      {
         printf("File transfer completed successfully.\n");
         returnValue = DONE;
      }
   }
   else if (returnValue == TIMEOUT_ON_EOF_ACK){
      // Send EOF
      send_buf(buf, 1, client, END_OF_FILE, expSeqNum, packet);

      returnValue = WAIT_ON_EOF_ACK;
   }
   return returnValue;
}
STATE timeout_on_ack(Connection * client, uint8_t * packet, int32_t packet_len)
{
   safeSendto(packet, packet_len, client);
   return WAIT_LAST_RR;
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

void printState(STATE state){
   switch(state)
   {
      case START:
#ifdef PRINTSTATES
         printf("State = START\n");
#endif
         break;
      case FILENAME:
#ifdef PRINTSTATES
         printf("State = FILENAME\n");
#endif
         break;
      case CHECK_WINDOW:
#ifdef PRINTSTATES
         printf("State = CHECK_WINDOW\n");
#endif
         break;
      case SEND_DATA:
#ifdef PRINTSTATES
         printf("State = SEND_DATA\n");
#endif
         break;
      case CHECK_RR_SREJ:
#ifdef PRINTSTATES
         printf("State = CHECK_RR_SREJ\n");
#endif
         break;
      case WAIT_LAST_RR:
#ifdef PRINTSTATES
         printf("State = WAIT_LAST_RR\n");
#endif
         break;
      case TIMEOUT_ON_ACK:
#ifdef PRINTSTATES
         printf("State = TIMEOUT_ON_ACK\n");
#endif
         break;
      case WAIT_ON_EOF_ACK:
#ifdef PRINTSTATES
         printf("State = WAIT_ON_EOF_ACK\n");
#endif
         break;
      case TIMEOUT_ON_EOF_ACK:
#ifdef PRINTSTATES
         printf("State = TIMEOUT_ON_EOF_ACK\n");
#endif
         break;
      case DONE:
#ifdef PRINTSTATES
         printf("State = DONE\n");
#endif
         break;
      default:
         printf("In default and you should not be here!!!!\n");
         state = DONE;
         break;
   }
}

void process_client(int32_t serverSocketNumber, uint8_t * buf, int32_t recv_len, Connection *
      client)
{
   WindowElement* window;
   WPtr wptr;
   wptr.c = 1;
   wptr.l = 1;
   wptr.u = 1;
   STATE state = START;
   int32_t data_file = 0;
   int32_t packet_len = 0;
   uint8_t packet[MAX_LEN];
   int32_t windowSize = 0;
   int32_t buf_size = 0;
   uint32_t seq_num = START_SEQ_NUM; // START_SEQ_NUM = 1
   while (state != DONE)
   {
      switch (state)
      {
         case START:
            printState(state);
            state = FILENAME;
            break;
         case FILENAME:
            printState(state);
            state = filename(client, buf, recv_len, &data_file, &windowSize, &buf_size);
            window = createWindow(windowSize);
            wptr.u = wptr.l + windowSize;
            break;
         case CHECK_WINDOW:
            printState(state);
            state = check_window(client, window, windowSize, &wptr);
            break;
         case SEND_DATA:
            printState(state);
            state = send_data(client, packet, &packet_len, data_file, buf_size, &seq_num, window, windowSize, &wptr);
            break;
         case CHECK_RR_SREJ:
            printState(state);
            state = check_rr_srej(client, window, windowSize, &wptr);
            break;
         case WAIT_LAST_RR:
            printState(state);
            state = wait_last_rr(client, seq_num, window, windowSize, &wptr);
            break;
         case TIMEOUT_ON_ACK:
            printState(state);
            state = timeout_on_ack(client, packet, packet_len);
            break;
         case WAIT_ON_EOF_ACK:
            printState(state);
            state = wait_on_eof_ack(client, seq_num);
            break;
         case TIMEOUT_ON_EOF_ACK:
            printState(state);
            state = timeout_on_eof_ack(client, packet, packet_len);
            break;
         case DONE:
            printState(state);
            break;
         default:
            printf("In default and you should not be here!!!!\n");
            state = DONE;
            break;
      }
   }
}
