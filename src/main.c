#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

typedef struct {
  char account_number[9]; // Fixed size for account number (8 chars + null)
  char name[100];
  double balance;
} Account;

typedef struct {
  char account_number[9];
  char transaction_type[8]; // Fixed size for transactions type (8 chars + null)
  double amount;
} Transcation;

Account accounts[100];
Transcation transactions[100];
int transaction_count = 0;
int accounts_count = 0;

void createUniqueId(char short_uuid[9]) {
  uuid_t uuid;
  char uuid_str[37]; // UUID is 36 characters + null terminator
  uuid_generate(uuid);
  uuid_unparse(uuid, uuid_str);
  strncpy(short_uuid, uuid_str, 8);
  short_uuid[8] = '\0';
}

void withdraw(Account *account) {
  int amount = 0;

  printf("Enter Amount To withdraw: ");
  scanf("%d", &amount);

  if (account->balance > amount) {
    account->balance -= amount;
    printf("Current Balance: %0.2lf", account->balance);
    transactions[transaction_count].amount = amount;
    strcpy(transactions[transaction_count].account_number,
           account->account_number);
    strcpy(transactions[transaction_count].transaction_type, "withdraw");
    transaction_count++;

  } else {
    printf("InSufficient funds");
  }
}

void deposit(Account *account) {
  int amount = 0;

  printf("Enter Amount To deposit: ");
  scanf("%d", &amount);

  account->balance += amount;
  transactions[transaction_count].amount = amount;
  strcpy(transactions[transaction_count].account_number,
         account->account_number);
  strcpy(transactions[transaction_count].transaction_type, "deposit");
  transaction_count++;
  printf("Current Balance: %0.2lf", account->balance);
}

void make_transaction(Account *account) {

  if (account != NULL) {
    PrintAccount(*account);

    int choice = 0;

    do {

      printf("\n1.Withdraw \n2.Deposit \n3.Exit \n");
      printf("Enter Choice: ");
      scanf("%d", &choice);

      switch (choice) {
      case 1:
        withdraw(account);
        break;
      case 2:
        deposit(account);
        break;
      }
    } while (choice != 3);
  } else {
    printf("\nError Finding Account!!!\n");
  }
}

void printScreen() {

  printf("All Account Details\n");
  printf("----------------");

  for (int i = 0; i < accounts_count; i++) {
    printf("\nAccount Number: %s Name:%s Balance:%.2lf",
           accounts[i].account_number, accounts[i].name, accounts[i].balance);
  }
}

void showTransactions() {

  printf("All Transaction Details\n");
  printf("----------------");

  for (int i = 0; i < transaction_count; i++) {
    printf("\nAccount Number: %s Method:%s Balance:%.2lf",
           transactions[i].account_number, transactions[i].transaction_type,
           transactions[i].amount);
  }
}

void PrintAccount(Account account) {
  printf("Account Details\n");
  printf("----------------");
  printf("\nAccount Number: %s\n", account.account_number);
  printf("Name: %s\n", account.name);
  printf("Balance: %.2lf\n", account.balance);
}

Account *find_account() {
  printScreen();

  char inputAccount[9];
  Account *found_account = NULL;

  printf("\n Enter Account Number: ");
  scanf("%s", inputAccount);
  inputAccount[8] = '\0';

  printf("\n searching for Account %s", inputAccount);

  for (int i = 0; i < accounts_count; i++) {
    if (strcmp(inputAccount, accounts[i].account_number) == 0) {
      found_account = &accounts[i];
      break;
    }
  }

  if (found_account != NULL) {
    return found_account;
  } else {
    return NULL;
  }
}

void create_account(Account *acc) {

  createUniqueId(acc->account_number);
  printf("Enter Your Name: ");
  scanf("%99s", acc->name);
  printf("Enter Amount: ");
  scanf("%lf", &acc->balance);
  printf("Acoount Added\n");
}

void delete_account() {

  printScreen();
  char inputAccount[9];
  printf("Enter Account want to delete: ");
  scanf("%s", inputAccount);
  for (int i = 0; i < accounts_count; i++) {
    if (strcmp(inputAccount, accounts[i].account_number) == 0) {
      for (int j = i; j < accounts_count - 1; j++) {
        accounts[j] = accounts[j + 1];
      }
      accounts_count--;
      printf("Account Deleted\n");
    }
  }
}

void writeFile() {
  FILE *file = fopen("index.json", "w");
  if (file == NULL) {
    printf("Error: Could not open file for writing.\n");
    return;
  }
  cJSON *json = cJSON_CreateObject();
  cJSON *accountsArray = cJSON_CreateArray();
  cJSON *transaction_countArray = cJSON_CreateArray();
  for (int i = 0; i < accounts_count; i++) {
    cJSON *accountSingle = cJSON_CreateObject();
    cJSON_AddStringToObject(accountSingle, "account_number",
                            accounts[i].account_number);
    cJSON_AddStringToObject(accountSingle, "name", accounts[i].name);
    cJSON_AddNumberToObject(accountSingle, "balance", accounts[i].balance);
    cJSON_AddItemToArray(accountsArray, accountSingle);
  }
  for (int j = 0; j < transaction_count; j++) {
    cJSON *transactionHolder = cJSON_CreateObject();
    cJSON_AddStringToObject(transactionHolder, "account_number",
                            transactions[j].account_number);
    cJSON_AddStringToObject(transactionHolder, "transaction_type",
                            transactions[j].transaction_type);
    cJSON_AddNumberToObject(transactionHolder, "amount",
                            transactions[j].amount);
    cJSON_AddItemToArray(transaction_countArray, transactionHolder);
  }
  cJSON_AddNumberToObject(json, "accounts_count", accounts_count);
  cJSON_AddItemToObject(json, "accounts", accountsArray);
  cJSON_AddItemToObject(json, "transaction", transaction_countArray);
  cJSON_AddNumberToObject(json, "transaction_count", transaction_count);

  char *json_str = cJSON_Print(json);
  fprintf(file, "%s", json_str);
  printf("%s\n", json_str);
}

char *readFile() {
  FILE *file = fopen("index.json", "r");
  if (file == NULL) {
    printf("Error: Could not open");

    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = (char *)malloc(length + 1);
  if (content == NULL) {
    printf("Memory allocation failed.\n");
    fclose(file);
    free(content);
    return NULL;
  }

  fread(content, 1, length, file);
  content[length] = '\0';
  fclose(file);
  return content;
}

void init() {

  char *content = readFile();

  cJSON *jsonData = cJSON_Parse(content);
  cJSON *accounts_data = cJSON_GetObjectItem(jsonData, "accounts");
  cJSON *transactions_data = cJSON_GetObjectItem(jsonData, "transaction");
  cJSON *accounts_count_data = cJSON_GetObjectItem(jsonData, "accounts_count");
  cJSON *transaction_count_data =
      cJSON_GetObjectItem(jsonData, "transaction_count");

  if (!cJSON_IsArray(accounts_data) || !cJSON_IsArray(transactions_data)) {
    printf("Something went wrong");
    return;
  }

  transaction_count = transaction_count_data->valueint;
  printf("Transaction count %d", transaction_count);
  accounts_count = accounts_count_data->valueint;

  for (int i = 0; i < accounts_count; i++) {
    cJSON *account = cJSON_GetArrayItem(accounts_data, i);
    printf("%s", cJSON_Print(account));
    const char *account_number =
        cJSON_GetObjectItem(account, "account_number")->valuestring;
    const char *name = cJSON_GetObjectItem(account, "name")->valuestring;
    double balance = cJSON_GetObjectItem(account, "balance")->valuedouble;
    strcpy(accounts[i].account_number, account_number);
    strcpy(accounts[i].name, name);
    accounts[i].balance = balance;
  }

  for (int i = 0; i < transaction_count; i++) {
    cJSON *transaction_holder = cJSON_GetArrayItem(transactions_data, i);
    printf("%s", cJSON_Print(transaction_holder));
    const char *account_number =
        cJSON_GetObjectItem(transaction_holder, "account_number")->valuestring;
    const char *transaction_type =
        cJSON_GetObjectItem(transaction_holder, "transaction_type")
            ->valuestring;
    double amount =
        cJSON_GetObjectItem(transaction_holder, "amount")->valuedouble;
    strcpy(transactions[i].account_number, account_number);
    strcpy(transactions[i].transaction_type, transaction_type);
    transactions[i].amount = amount;
  }
  free(content);
}

int main() {

  int choice;
  Account *current_account;
  init();

  do {

    printf("\n1. Create New Account \n2. Show Accounts \n3. Make Transcation "
           "\n4. Show Transactions \n5. Exit\n");
    printf("Enter Choice: ");

    scanf("%d", &choice);

    switch (choice) {
    case 1:
      create_account(&accounts[accounts_count]);

      PrintAccount(accounts[accounts_count]);
      accounts_count++;
      break;
    case 2:
      printScreen();
      break;
    case 3:
      current_account = find_account();
      make_transaction(current_account);
      break;
    case 4:
      // delete_account();
      showTransactions();
    }
  } while (choice != 5);
  writeFile();
  printf("\nExiting\n");
  return 0;
}