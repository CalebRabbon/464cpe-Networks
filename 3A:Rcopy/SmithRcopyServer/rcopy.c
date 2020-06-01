/* rcopy - client in stop and wait protocol Writen: Hugh Smith */
// a bunch of #includes go here!

#include "networks.h"
#include "srej.h"
#include "cpe464.h"
#include "args.h"

#define HEADERLEN 7

typedef enum State STATE;
enum State
{
   DONE, FILENAME, RECV_DATA, FILE_OK, START_STATE
};
void stateMachine(Args* args);
STATE start_state(Args* args, Connection * server, uint32_t * clientSeqNum);
STATE filename(char * fname, int32_t buf_size, Connection * server);
STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum);
STATE file_ok(int * outputFileFd, char * outputFileName);
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
   uint32_t clientSeqNum = 0;
   int32_t output_file_fd = 0;

   STATE state = START_STATE;
   while (state != DONE)
   {
      switch (state)
      {
         case START_STATE:
            printState(state);
            state = start_state(args, &server, &clientSeqNum);
            break;
         case FILENAME:
            printState(state);
            state = filename(args->fromFile, args->bufferSize, &server);
            break;
         case FILE_OK:
            printState(state);
            state = file_ok(&output_file_fd, args->toFile);
            break;
         case RECV_DATA:
            printState(state);
            state = recv_data(output_file_fd, &server, &clientSeqNum);
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


STATE start_state(Args* args, Connection * server, uint32_t * clientSeqNum)
{
   // Returns FILENAME if no error, otherwise DONE (to many connects, cannot
   // connect to sever)
   uint8_t packet[MAX_LEN];
   uint8_t buf[MAX_LEN];
   int fileNameLen = strlen(args->fromFile);
   STATE returnValue = FILENAME;
   uint32_t bufferSize = 0;
   /*
   static int retryCount = 0;
   uint32_t seq_num = 0;
   uint32_t dataLen = 0;
   int32_t recv_check = 0;
   uint8_t flag = 0;
   */

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
      /* Send the file name */
      // put in buffer size (for sending data) and filename
      bufferSize = htonl(args->bufferSize);
      memcpy(buf, &bufferSize, SIZE_OF_BUF_SIZE);
      memcpy(&buf[SIZE_OF_BUF_SIZE], args->fromFile, fileNameLen);
      printIPv6Info(&server->remote);
      send_buf(buf, fileNameLen + SIZE_OF_BUF_SIZE, server, FNAME, *clientSeqNum, packet);
      (*clientSeqNum)++;
      returnValue = FILENAME;
   }

   return returnValue;

   /* Send the setup flag */
   // put in buffer size (for sending data) and setup flag
   //dataLen = 0;
   //flag = FSETUP;
   //sendFlag(buf, dataLen, server, flag, seq_num, packet);

   ///* Process the setup flag */
   //if ((returnValue = processSelect(server, &retryCount, START_STATE, FILENAME, DONE)) ==
   //      FILENAME)
   //{
   //   recv_check = recv_buf(packet, MAX_LEN, server->sk_num, server, &flag, &seq_num);
   //   /* check for bit flip */
   //   if (recv_check == CRC_ERROR)
   //   {
   //      returnValue = START_STATE;
   //   }
   //   else if (flag == FNAME_BAD)
   //   {
   //      printf("File %s not found\n", args->fromFile);
   //      returnValue = DONE;
   //   }
   //   else if (flag == DATA)
   //   {
   //      // file yes/no packet lost - instead its a data packet
   //      returnValue = FILE_OK;
   //   }
   //}
}

STATE filename(char * fname, int32_t buf_size, Connection * server)
{
   // Send the file name, get response
   //   return START_STATE if no reply from server, DONE if bad filename,
   // FILE_OK otherwise
   int returnValue = START_STATE;
   uint8_t packet[MAX_LEN];
   uint8_t flag = 0;
   uint32_t seq_num = 0;
   int32_t recv_check = 0;
   static int retryCount = 0;
   if ((returnValue = processSelect(server, &retryCount, FILENAME, FILE_OK, DONE)) == FILE_OK)
   {
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

STATE file_ok(int * outputFileFd, char * outputFileName)
{
   STATE returnValue = DONE;
   if ((*outputFileFd = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0)
   {
      perror("File open error: ");
      returnValue = DONE;
   } else
   {
      returnValue = RECV_DATA;
   }
   return returnValue;
}

char* convertToString(uint8_t *data_buf, uint32_t data_len){
   char* string = NULL;
   memcpy(string, data_buf, data_len);
   string[data_len] = '\0';
   return string;
}

STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum)
{
   uint32_t seq_num = 0;
   uint32_t ackSeqNum = 0;
   uint8_t flag = 0;
   int32_t data_len = 0;
   uint8_t data_buf[MAX_LEN];
   uint8_t packet[MAX_LEN];
   static int32_t expected_seq_num = START_SEQ_NUM;
   if (select_call(server->sk_num, LONG_TIME, 0, NOT_NULL) == 0)
   {
      printf("Timeout after 10 seconds, server must be gone.\n");
      return DONE;
   }
   data_len = recv_buf(data_buf, MAX_LEN, server->sk_num, server, &flag, &seq_num);
   /* do state RECV_DATA again if there is a crc error (don't send ack, don't
    * write data) */
   if (data_len == CRC_ERROR)
   {
      return RECV_DATA;
   }
   if (flag == END_OF_FILE)
   {
      /* send ACK */
      send_buf(packet, 1, server, EOF_ACK, *clientSeqNum, packet);
      (*clientSeqNum)++;
      printf("File done\n");
      return DONE;
   }
   else
   {
      /* send ACK */
      ackSeqNum = htonl(seq_num);
      send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, ACK, *clientSeqNum, packet);
      (*clientSeqNum)++;
   }
   if (seq_num == expected_seq_num)
   {
      //printf("Outputing to file %s\n", convertToString(data_buf, data_len));
      expected_seq_num++;
      write(output_file, &data_buf, data_len);
   }
   return RECV_DATA;
}
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

