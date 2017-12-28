//Mehmet Ozan Güven - 220201036
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#define NUMBER_OF_THREADS_A 4

void checkFilesEmpty();
int fileError(char *fileName1, char *fileName2);
void destroyMutexes();

void createThreadTypeA();
void waitForAllThreads();
void *threadA_work(void *data);
void criticalSectionForThreadA(int whichTxtFile);
void cutLine(int random_number, char* fileName);
void putLineToResult(char *eachLine);


void createThreadTypeB();
void *threadB_work(void *data);
void criticalSectionForThreadB(char* fileName);

void createThreadTypeC();
void *threadC_work(void *data);
void criticalSectionForThreadC(char* fileName);

/*
  Create threads handler, initialize the mutexes
  and necessary global variables.
*/
pthread_t threadA[NUMBER_OF_THREADS_A], threadC, threadB;
int isNumbers1FileEmpty, isNumbers2FileEmpty;
pthread_mutex_t mutexForNumbers1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexForNumbers2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexForResult = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char const *argv[]) {
  //Before execute anything, check files are empty or not
  //If it is empty then, exit the program
  //Otherwise, continue
  checkFilesEmpty();
  if(isNumbers1FileEmpty == 1 && isNumbers2FileEmpty == 1)
  {
    printf("Resources are empty\n");
    exit(0);
  }
  else
  {
    createThreadTypeA();
    createThreadTypeB();
    createThreadTypeC();
    waitForAllThreads();
    destroyMutexes();
  }
  return 0;
}


/*
  After all operation destroy the mutexes
*/
void destroyMutexes()
{

  int destroy;
  destroy = pthread_mutex_destroy(&mutexForNumbers1);
  if(destroy != 0)
  {
    printf("Error occured when destroying the mutexForNumbers1\n");
  }
  destroy = pthread_mutex_destroy(&mutexForNumbers2);
  if(destroy != 0)
  {
    printf("Error occured when destroying the mutexForNumbers2\n");
  }
  destroy = pthread_mutex_destroy(&mutexForResult);
  if(destroy != 0)
  {
    printf("Error occured when destroying the mutexForResult\n");
  }

}

/*
  @param fileName1, fileName2, determines which files I check for error
  One of them is NULL, this is enough to say an error occured
  This function returns: if one of the file is null, then return 1
  otherwise 0
*/
int fileError(char *fileName1, char *fileName2)
{

  int isError = 0;
  FILE *file1, *file2;
  file1 = fopen(fileName1, "r");
  file2 = fopen(fileName2, "r");
  if(file1 == NULL || file2 == NULL)
  {
    isError = 1;
    return isError;
  }
  fclose(file1);
  fclose(file2);
  return isError;

}

/*
  if numbers1.txt is empty, then isNumbers1FileEmpty = 1
  otherwise isNumbers1FileEmpty = 0
  if numbers2.txt is empty, then isNumbers2FileEmpty = 1
  otherwise isNumbers2FileEmpty = 0;
*/
void checkFilesEmpty()
{

  FILE *numbers1, *numbers2;
  int file_Error = fileError("numbers1.txt", "numbers2.txt");
  if(file_Error == 1)
  {
    printf("Error occured when opening the file\n");
    return;
  }
  numbers1 = fopen("numbers1.txt","r");
  numbers2 = fopen("numbers2.txt","r");

  fseek(numbers1, 0, SEEK_END);
  if(ftell(numbers1) == 0)
  {
    isNumbers1FileEmpty = 1;
  }
  else
  {
    fseek(numbers1, 0, SEEK_SET);
  }
  fclose(numbers1);

  fseek(numbers2, 0, SEEK_END);
  if(ftell(numbers2) == 0)
  {
    isNumbers2FileEmpty = 1;
  }
  else
  {
    fseek(numbers2, 0, SEEK_SET);
  }
  fclose(numbers2);

}

/*
  Create all threads type A
*/
void createThreadTypeA()
{

  for(int i = 0; i < NUMBER_OF_THREADS_A; i++)
  {
    int threadCreation = 0;
    threadCreation = pthread_create(&threadA[i], NULL, threadA_work, NULL);
    if(threadCreation)
    {
      printf("Thread A can not created\n");
    }
  }

}

/*
  When one thread's job is done, then
  it is waiting for the others.
*/
void waitForAllThreads()
{
    int join;
    for(int i = 0; i < NUMBER_OF_THREADS_A; i++)
    {
      join = pthread_join(threadA[i], NULL);
      if(join != 0)
      {
        printf("Error occured in pthread_join\n");
        return;
      }
    }

    join = pthread_join(threadB, NULL);
    if(join != 0)
    {
      printf("Error occured in pthread_join\n");
      return;
    }
    join = pthread_join(threadC, NULL);
    if(join != 0)
    {
      printf("Error occured in pthread_join\n");
      return;
    }
}

/*
  Create thread type B
*/
void createThreadTypeB()
{

  int threadCreation = 0;
  threadCreation = pthread_create(&threadB, NULL, threadB_work, NULL);
  if(threadCreation)
  {
    printf("Thread B can not created\n");
  }

}

/*
  Create thread type C
*/
void createThreadTypeC()
{

  int threadCreation = 0;
  threadCreation = pthread_create(&threadC, NULL, threadC_work, NULL);
  if(threadCreation)
  {
    printf("Thread C can not created\n");
  }

}

/*
  @param number, checks it prime or not
  this function returns 1, if number is prime
  otherwise returns 0
*/
int isPrime(int number)
{

  int isPrime = 1;
  if(number <= 0 || number == 1)
  {
      return 0;
  }

  if(number == 2)
  {
    return 1;
  }

  for(int i = 2; i < number; i++)
  {
    if(number % i == 0)
    {
      isPrime = 0;
      break;
    }
  }
  return isPrime;

}

/*
  @param eachLine, it is the line of the file
  This function does: take the line(eachLine) and
  put that line to the result.txt
*/
void putLineToResult(char *eachLine)
{

  FILE *destination_file;
  destination_file = fopen("result.txt", "a");
  if(destination_file == NULL)
  {
    printf("Error, result.txt can not open\n");
    return;
  }
  fputs(eachLine, destination_file);
  fclose(destination_file);

}

/*
  @param random_number, is a number to determine how many lines cut-paste
  @param fileName, is the file name which operates on.
  This function produces cut-paste line by line from resource to destination_file.
  temp_file is the temporary file to do cut-paste operations
  Using lineCounting, for temp_file I am skipping cut lines and for destination_file
  I am only adding cut lines.
  Then I have renamed the temp_file as resource
*/
void cutLine(int random_number, char* fileName)
{

  FILE *resource, *temp_file;
  resource = fopen(fileName, "r");
  temp_file = fopen("temporaryA.txt", "w");
  int lineCounting = 0;
  char eachLineInResource[256];
  while (fgets(eachLineInResource, sizeof(eachLineInResource), resource) != NULL)
  {//start while
    if(lineCounting < random_number)
    {
      putLineToResult(eachLineInResource);
      lineCounting++;
    }
    else
    {
      fputs(eachLineInResource, temp_file);
    }
  }//end while
  fclose(resource);
  fclose(temp_file);
  int isRenamed = rename("temporaryA.txt", fileName);
  if(isRenamed != 0)
  {
    printf("File rename operation for criticalSectionForThreadA could not work\n");
  }

}

/*
  @param whichTxtFile, determines which files I operate on. For 0, file is numbers1.txt
  for 1, file is numbers2.txt
  This function does : according to whichTxtFile,
  it goes to cutLine_paste function with corresponding file name.
*/
void criticalSectionForThreadA(int whichTxtFile)
{

  srand ( time(NULL) );
  int random_number = rand()%10 + 1;
  if(whichTxtFile == 0)
  {
    cutLine(random_number, "numbers1.txt");
  }
  else
  {
    cutLine(random_number, "numbers2.txt");
  }

}

/*
  @param data, not using
  This is threads A function.
  My strategy is :
    - First try to lock result.txt
    -     If it is locked, then check files are empty or not
    -       If files are empty, that means threads A works is over, unlock the result.txt and terminate thread.
    -
    -       If not, try to lock either numbers1.txt or numbers2.txt
    -         According to which file is lock, go to critical section (criticalSectionForThreadA).
    -         When exiting, unlock file and result.txt respectively.
    -       If neither numbers1.txt nor numbers2.txt can lock, then unlock the result.txt.
    -       Then, go over the steps again until files are empty
*/
void *threadA_work(void *data)
{

  while (1)
  {//start while
    if (pthread_mutex_trylock(&mutexForResult) == 0)
    {//if-1
      checkFilesEmpty();
      if(isNumbers1FileEmpty == 1 && isNumbers2FileEmpty == 1)
      {
        printf("Thread A works is DONE\n");
        pthread_mutex_unlock(&mutexForResult);
        pthread_exit(NULL);
      }
      else
      {
        if (pthread_mutex_trylock(&mutexForNumbers1) == 0)
        {
          //Critical section
          criticalSectionForThreadA(0);
          //end critical section
          pthread_mutex_unlock(&mutexForNumbers1);
          pthread_mutex_unlock(&mutexForResult);
        }
        else if(pthread_mutex_trylock(&mutexForNumbers2) == 0)
        {
          //Critical section
          criticalSectionForThreadA(1);
          //end critical section
          pthread_mutex_unlock(&mutexForNumbers2);
          pthread_mutex_unlock(&mutexForResult);
        }
        else
        {
          pthread_mutex_unlock(&mutexForResult);
        }
      }
    }//if-1
    checkFilesEmpty();
  }//end while

}

/*
  @param fileName, determines which file I operate on.
  This function does : Remove prime numbers in the file(resource), using isPrime function
  Then updates the file.
*/
void criticalSectionForThreadB(char* fileName)
{

    FILE *resource, *update_file;
    resource = fopen(fileName, "r");
    update_file = fopen("temporaryB.txt", "w");
    char eachLineInResource[256];
    int number, is_prime;
    while (fgets(eachLineInResource, sizeof(eachLineInResource), resource) != NULL)
    {//start while
      number = atoi(eachLineInResource);
      is_prime = isPrime(number);
      if(is_prime == 1)
      {
        continue;
      }
      else
      {
        fputs(eachLineInResource, update_file);
      }
    }//end while
    fclose(resource);
    fclose(update_file);
    int isRenamed = rename("temporaryB.txt", fileName);
    if(isRenamed != 0)
    {
      printf("File rename operation in criticalSectionForThreadB could not work\n");
    }

}

/*
  @param data, not using
  This is threads B function
  My strategy is :
    - First check files are empty or not.
    -   If two files are empty, then just works on result.txt, then terminate thread B
    -   If not, try to lock one of the files
    -   When one file is locked, then check this file is empty or not
    -     If file is empty, then unlock the mutex for that file
    -     If not, go to critical section remove prime numbers from that file (criticalSectionForThreadB)
    -       After critical section, unlock the corresponding mutex
    -   check file is empty or not then go over steps again until files are empty
*/
void *threadB_work(void *data)
{
  while (1)
  {//start while
    checkFilesEmpty();

    if( (isNumbers1FileEmpty == 1 && isNumbers2FileEmpty == 1) )
    {//if-4
      if(pthread_mutex_trylock(&mutexForResult) == 0)
      {
        //critical section
        criticalSectionForThreadB("result.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForResult);
        printf("Thread B works is DONE\n");
        pthread_exit(NULL);
      }
    }//if-4

    else if(pthread_mutex_trylock(&mutexForResult) == 0)
    {//if-3
      //critical section
      criticalSectionForThreadB("result.txt");
      //after critical section
      pthread_mutex_unlock(&mutexForResult);
    }//if-3

    else if(pthread_mutex_trylock(&mutexForNumbers1) == 0)
    {//if-1
      if(isNumbers1FileEmpty == 0)
      {
        //critical section
        criticalSectionForThreadB("numbers1.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForNumbers1);
      }
      else
      {
        pthread_mutex_unlock(&mutexForNumbers1);
      }
    }//if-1
    else if(pthread_mutex_trylock(&mutexForNumbers2) == 0)
    {//if-2
      if(isNumbers2FileEmpty == 0)
      {
        //critical section
        criticalSectionForThreadB("numbers2.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForNumbers2);
      }
      else
      {
        pthread_mutex_unlock(&mutexForNumbers2);
      }
    }//if-2

    checkFilesEmpty();
  }//end while
}

/*
  @param fileName, determines which file I operate on.
  This function does : given file name, it removes negative numbers
  from that file.
  Then updates the file
*/
void criticalSectionForThreadC(char* fileName)
{

  FILE *resource, *update_file;
  resource = fopen(fileName, "r");
  update_file = fopen("temporaryC.txt", "w");
  char eachLineInResource[256];
  int number;
  while (fgets(eachLineInResource, sizeof(eachLineInResource), resource) != NULL)
  {//start while
    number = atoi(eachLineInResource);
    if(number < 0)
    {
      continue;
    }
    else
    {
      fputs(eachLineInResource, update_file);
    }
  }//end while
  fclose(resource);
  fclose(update_file);
  int isRenamed = rename("temporaryC.txt", fileName);
  if(isRenamed != 0)
  {
    printf("File rename operation for criticalSectionForThreadC could not work\n");
  }

}

/*
  @param data, not using
  This is threads C function
  My strategy is :
    - First check files are empty or not.
    -   If two files are empty, then just works on result.txt, then terminate thread C
    -   If not, try to lock one of the files
    -   When one file is locked, then check this file is empty or not
    -     If file is empty, then unlock the mutex for that file
    -     If not, go to critical section and remove negative numbers (criticalSectionForThreadC)
    -       After critical section, unlock the corresponding mutex
    -   check file is empty or not then go over steps again until files(resources) are empty
*/
void *threadC_work(void *data)
{

  while(1)
  {//start while
    checkFilesEmpty();
    if( (isNumbers1FileEmpty == 1 && isNumbers2FileEmpty == 1) )
    {//if-4

      if(pthread_mutex_trylock(&mutexForResult) == 0)
      {
        //critical section
        criticalSectionForThreadC("result.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForResult);
        printf("Thread C works is DONE\n");
        pthread_exit(NULL);
      }

    }//if-4

    else if(pthread_mutex_trylock(&mutexForResult) == 0)
    {//if-3
      //critical section
      criticalSectionForThreadC("result.txt");
      //after critical section
      pthread_mutex_unlock(&mutexForResult);
    }//if-3

    else if(pthread_mutex_trylock(&mutexForNumbers1) == 0)
    {//if-1
      if(isNumbers1FileEmpty == 0)
      {
        //critical section
        criticalSectionForThreadC("numbers1.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForNumbers1);
      }
      else
      {
        pthread_mutex_unlock(&mutexForNumbers1);
      }
    }//if-1

    else if(pthread_mutex_trylock(&mutexForNumbers2) == 0)
    {//if-2
      if(isNumbers2FileEmpty == 0)
      {
        //critical section
        criticalSectionForThreadC("numbers2.txt");
        //after critical section
        pthread_mutex_unlock(&mutexForNumbers2);
      }
      else
      {
        pthread_mutex_unlock(&mutexForNumbers2);
      }
    }//if-2

    checkFilesEmpty();
  }//end while

}

