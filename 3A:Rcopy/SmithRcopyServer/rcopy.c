/* rcopy - client in stop and wait protocol Writen: Hugh Smith */
// a bunch of #includes go here!

#include "networks.h"
#include "srej.h"
#include "cpe464.h"
#include "args.h"
#include "buffer.h"

#define HEADERLEN 7
//#define PRINT

// Used to store the variables passed between STATES
typedef struct statevars StateVars;
struct statevars{
   uint8_t data_buf[MAX_LEN];
   int32_t data_len;
   int srej_fg;
   int expSeqNum;
   int32_t output_file;
};

/*
char* convertToString(char* string, uint8_t *databuf, uint32_t data_len){
   if(data_len == 0){
      return NULL;
   }
   memcpy(string, databuf, data_len);
   string[data_len] = '\0';
   return string;
}
*/

void createStateVars(StateVars* p){
   memset(p->data_buf, 0, MAX_LEN);
   p->data_len = 0;
   p->srej_fg = 0;
   p->expSeqNum = 0;
   p->output_file = 0;
}

void printStateVars(StateVars* p){
   char str[MAX_LEN];
   memset(str, 0, MAX_LEN);
   printf("Print Vars\n");
   printf("\tdata_buf: %s\n", convertToString(str, p->data_buf, p->data_len));
   printf("\tdata_len: %i\n", p->data_len);
   printf("\tsrej_fg : %i\n", p->srej_fg);
   printf("\texpSeqNum: %i\n", p->expSeqNum);
   printf("\toutput_file: %i\n", p->output_file);
}

typedef enum State STATE;
enum State
{
   DONE, FILENAME, RECV_DATA, FILE_OK, START_STATE, CHECK_BUFFER, CHECK_SREJ_FG
};
void stateMachine(Args* args);
STATE start_state(StateVars* sv, Args* args, Connection * server);
STATE filename(char * fname, Connection * server, Args* args);
STATE recv_data(StateVars* sv, Connection * server, Args* args, WindowElement* window);
STATE check_buffer(StateVars* sv, Connection * server, Args* args, WindowElement* window);
STATE file_ok(StateVars* sv, char * outputFileName);
void check_args(int argc, char ** argv);

int main ( int argc, char *argv[] )
{
   Args args;

   checkArgs(argc, argv, &args);

   sendErr_init(args.percentError, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);

   stateMachine(&args);

   return 0;
}

void printState(STATE state){
   switch(state)
   {
      case START_STATE:
         printf("State = START_STATE\n");
         break;
      case FILENAME:
         printf("State = FILENAME\n");
         break;
      case FILE_OK:
         printf("State = FILE_OK\n");
         break;
      case RECV_DATA:
         printf("State = RECV_DATA\n");
         break;
      case CHECK_BUFFER:
         printf("State = CHECK_BUFFER\n");
         break;
      case DONE:
         printf("State = DONE\n");
         break;
      default:
         printf("ERROR - in default state\n");
         break;
   }
}

void stateMachine(Args* args)
{
   // Args* args needed to get file names, server name and server port number
   Connection server;
   WindowElement* window = createWindow(args->windowSize);
   StateVars sv;
   createStateVars(&sv);
   printStateVars(&sv);

   STATE state = START_STATE;
   while (state != DONE)
   {
      switch (state)
      {
         case START_STATE:
            printState(state);
            state = start_state(&sv, args, &server);
            break;
         case FILENAME:
            printState(state);
            state = filename(args->fromFile, &server, args);
            break;
         case FILE_OK:
            printState(state);
            state = file_ok(&sv, args->toFile);
            break;
         case RECV_DATA:
            printState(state);
            //printStateVars(&sv);
            state = recv_data(&sv, &server, args, window);
            break;
         case CHECK_BUFFER:
            printState(state);
            state = check_buffer(&sv, &server, args, window);
            break;
         case DONE:
            printState(state);
            break;
         default:
            printf("ERROR - in default state\n");

            break;
      }
   }
}

void sendFlag(uint8_t * buf, uint32_t dataLen, Connection * server,
      uint8_t flag, uint32_t seq_num, uint8_t * packet){
   /* Send the setup flag */
   // put in buffer size (for sending data) and setup flag
   printIPv6Info(&server->remote);
   send_buf(buf, dataLen + HEADERLEN, server, flag, seq_num, packet);
}

STATE start_state(StateVars* sv, Args* args, Connection * server)
{
   // Returns FILENAME if no error, otherwise DONE (to many connects, cannot
   // connect to sever)
   // uint8_t packet[args->bufferSize + HEADERLEN];
   // uint8_t buf[args->bufferSize];
   uint8_t packet[MAX_LEN];
   uint8_t buf[MAX_LEN];
   int fileNameLen = strlen(args->fromFile);
   STATE returnValue = FILENAME;
   uint32_t bufferSize = 0;
   uint32_t windowSize = 0;

   // if we have connected to server before, close it before reconnect
   /* This was in Prof Smith's code but it just closes the client before sending
    * any data
   if (server->sk_num > 0)
   {
      close(server->sk_num);
   }
   */

   /* Try connecting to the server */
   if (udpClientSetup(args->remoteMachine, args->remotePort, server) < 0)
   {
      // could not connect to server= DONE;
      returnValue = DONE;
   }
   else {
      /* Send the file name, window size, and buffer size */
      windowSize = htonl(args->windowSize);
      bufferSize = htonl(args->bufferSize);

      memcpy(buf, &windowSize, SIZE_OF_WIN_SIZE);
      memcpy(&buf[SIZE_OF_WIN_SIZE], &bufferSize, SIZE_OF_BUF_SIZE);
      memcpy(&buf[SIZE_OF_WIN_SIZE + SIZE_OF_BUF_SIZE], args->fromFile, fileNameLen);

#ifdef PRINT
      printIPv6Info(&server->remote);
#endif
      send_buf(buf, fileNameLen + SIZE_OF_BUF_SIZE + SIZE_OF_WIN_SIZE, server, FNAME, sv->expSeqNum, packet);
      (sv->expSeqNum) = START_SEQ_NUM;
      returnValue = FILENAME;
   }

   return returnValue;
}

STATE filename(char * fname, Connection * server, Args* args)
{
   // Send the file name, get response
   //   return START_STATE if no reply from server, DONE if bad filename,
   // FILE_OK otherwise
   int returnValue = START_STATE;
   //uint8_t packet[args->bufferSize + HEADERLEN];
   uint8_t packet[MAX_LEN];
   uint8_t flag = 0;
   uint32_t seq_num = 0;
   int32_t recv_check = 0;
   static int retryCount = 0;
   printf("Retry count %i\n", retryCount);
   if ((returnValue = processSelect(server, &retryCount, START_STATE, FILE_OK, DONE)) == FILE_OK)
   {
      //recv_check = recv_buf(packet, args->bufferSize + HEADERLEN, server->sk_num, server, &flag, &seq_num);
      recv_check = recv_buf(packet, MAX_LEN, server->sk_num, server, &flag, &seq_num);
      /* check for bit flip */
      if (recv_check == CRC_ERROR)
      {
         returnValue = START_STATE;
      }
      else if (flag == FNAME_BAD)
      {
         printf("File %s not found\n", fname);
         returnValue = DONE;
      }
      else if (flag == DATA)
      {
         // file yes/no packet lost - instead its a data packet
         returnValue = FILE_OK;
      }
   }
   return(returnValue);
}

STATE file_ok(StateVars* sv, char * outputFileName)
{
   STATE returnValue = DONE;
   if ((sv->output_file = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0)
   {
      perror("File open error: ");
      returnValue = DONE;
   } else
   {
      returnValue = RECV_DATA;
   }
   return returnValue;
}

STATE recv_data(StateVars* sv, Connection * server, Args* args, WindowElement* window)
{
   uint32_t recSeqNum = 0;
   uint32_t ackSeqNum = 0;
   uint8_t flag = 0;
   //int32_t data_len = 0;
   //uint8_t data_buf[args->bufferSize]; // Contains just the data
   //
   char dataString[MAX_LEN];   // Null terminating string of the data
   uint8_t packet[args->bufferSize + HEADERLEN];
   WindowElement element;
   //static int32_t expected_seq_num = START_SEQ_NUM;

   if (select_call(server->sk_num, LONG_TIME, 0, NOT_NULL) == 0)
   {
      printf("Timeout after 10 seconds, server must be gone.\n");
      return DONE;
   }

   // data_len = length of just the data portion
   // Sets the value of the flag
   // Sets the value of the recSeqNum
   sv->data_len = recv_buf(sv->data_buf, args->bufferSize + HEADERLEN, server->sk_num, server, &flag, &recSeqNum);
   /* do state RECV_DATA again if there is a crc error (don't send ack, don't
    * write data) */
   if (sv->data_len == CRC_ERROR)
   {
      return RECV_DATA;
   }
   if (flag == END_OF_FILE)
   {
      /* send ACK */
      (sv->expSeqNum)++;

      send_buf(packet, 1, server, EOF_ACK, sv->expSeqNum, packet);
      printf("File done\n");
      return DONE;
   }
   printf("receieved sequence number: %i\n", recSeqNum);

   if (recSeqNum < sv->expSeqNum){
      /* send sv->expSeqNum ACK but don't increase the sequence number as this is an old
       * packet */
      ackSeqNum = htonl(sv->expSeqNum);
      send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, ACK, sv->expSeqNum, packet);

      return RECV_DATA;
   }
   else if (recSeqNum == sv->expSeqNum)
   {
      /* Increase sequence number and set srej flag */
      (sv->expSeqNum)++;
      sv->srej_fg = 0;

      /* write to file */
      printf("Outputing to file %s\n", convertToString(dataString, sv->data_buf, sv->data_len));
      write(sv->output_file, &(sv->data_buf), sv->data_len);

      return CHECK_BUFFER;
   }
   else if (recSeqNum > sv->expSeqNum){
      /* Lost a packet so add the received packet to the buffer */
      createWindowElement(&element, sv->data_buf, sv->data_len);
      addElement(recSeqNum, element, window, args->windowSize);

      return RECV_DATA;
   }
   return RECV_DATA;
}

STATE check_buffer(StateVars* sv, Connection * server, Args* args, WindowElement* window){
   uint32_t ackSeqNum = 0;
   uint8_t packet[args->bufferSize + HEADERLEN];
   char dataString[MAX_LEN];
   WindowElement newElement;

   // Check if the whole window is empty. If it is return with an RR of the
   // current sequence number
   if(isWindowEmpty(window, args->windowSize) == EMPTY){
      /* send ACK */
      ackSeqNum = htonl(sv->expSeqNum);
      send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, ACK, sv->expSeqNum, packet);

      return RECV_DATA;
   }

   // If window is not empty check the current spot in the buffer if this is empty
   // send SREJ of the current sequence number since there is other data missing
   if(isEmptySpot(sv->expSeqNum, window, args->windowSize) == EMPTY){
      /* send SREJ */
      ackSeqNum = htonl(sv->expSeqNum);
      send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, FSREJ, sv->expSeqNum, packet);

      return RECV_DATA;
   }
   // Not empty
   else if(isEmptySpot(sv->expSeqNum, window, args->windowSize) == FULL){
      /* write to file */
      getElement(sv->expSeqNum, &newElement, window, args->windowSize);
      printf("Outputing to file %s\n", convertToString(dataString, newElement.data_buf, newElement.data_len));
      write(sv->output_file, &(newElement.data_buf), newElement.data_len);

      deleteElement(sv->expSeqNum, window, args->windowSize);

      /* Increase sequence number */
      (sv->expSeqNum)++;

      return CHECK_BUFFER;
   }
   else{
      printf("YOU SHOULD NOT BE HERE: ERROR WITH isEmptySpot Function\n");
   }
   return RECV_DATA;
}

/*
void check_args(int argc, char ** argv)
{
   if (argc != 7)
   {
      printf("Usage %s fromFile toFile buffer_size error_rate hostname port\n", argv[0]);
      exit(-1);
   }
   if (strlen(argv[1]) > 1000)
   {
      printf("FROM filename to long needs to be less than 1000 and is: %d\n", (int)strlen(argv[1]));
      exit(-1);
   }
   if (strlen(argv[2]) > 1000)
   {
      printf("TO filename to long needs to be less than 1000 and is: %d\n", (int)strlen(argv[1]));
      exit(-1);
   }
   if (atoi(argv[3]) < 400 || atoi(argv[3]) > 1400)
   {
      printf("Buffer size needs to be between 400 and 1400 and is: %d\n", atoi(argv[3]));
      exit(-1);
   }
   if (atoi(argv[4]) < 0 || atoi(argv[4]) >= 1 )
   {
      printf("Error rate needs to be between 0 and less then 1 and is: %d\n", atoi(argv[4]));
      exit(-1);
   }
}
*/
