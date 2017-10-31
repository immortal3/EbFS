unsigned char *encrypt(unsigned char *plain_text, unsigned char *key, int size);
unsigned char *decrypt(unsigned char *cipher_text, unsigned char *key, int size);

void print_block(unsigned char block[], int size);

void shift_rows(unsigned char block[], int size);
void inv_shift_rows(unsigned char block[], int size);

void mix_columns(unsigned char block[], int size);
void inv_mix_columns(unsigned char block[], int size);
unsigned char mul_in_gf(unsigned char a, unsigned char b);

void sub_bytes(unsigned char block[], int size);
void inv_sub_bytes(unsigned char block[], int size);

void add_round_key(unsigned char block[], int size, unsigned char key_schedule[], int round);
void key_schedule(unsigned char *keys, int size);
void key_schedule_core(unsigned char word[], int i);
