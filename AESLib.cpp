#include "AESLib.h"

uint8_t AESLib::getrnd()
{
   uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
   return really_random;
}

void AESLib::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) getrnd();
    }
}

String AESLib::encrypt(String msg, byte key[], byte my_iv[]) {
  char b64data[200];
  byte cipher[1000];
    
  aes.set_key( key , sizeof(key));
  base64_encode(b64data, (char *)my_iv, N_BLOCK);
  int b64len = base64_encode(b64data, (char *)msg.c_str(),msg.length());
  // Encrypt! With AES128, our key and IV, CBC and pkcs7 padding    
  aes.do_aes_encrypt((byte *)b64data, b64len , cipher, key, 128, my_iv);
  base64_encode(b64data, (char *)cipher, aes.get_size() );  
  return String(b64data);  
}
