#ifndef __AES_H__
#define __AES_H__

#include "AES_config.h"

class AES
{
 public:

  AES();
    
  byte set_key (byte key[], int keylen) ;
  
  void clean () ;  // delete key schedule after use
  
  void copy_n_bytes (byte * AESt, byte * src, byte n) ;

  byte encrypt (byte plain [N_BLOCK], byte cipher [N_BLOCK]) ;
  
  byte cbc_encrypt (byte * plain, byte * cipher, int n_block, byte iv [N_BLOCK]) ;
  
  byte cbc_encrypt (byte * plain, byte * cipher, int n_block) ;

  byte decrypt (byte cipher [N_BLOCK], byte plain [N_BLOCK]) ;
  
  byte cbc_decrypt (byte * cipher, byte * plain, int n_block, byte iv [N_BLOCK]) ;
  
  byte cbc_decrypt (byte * cipher, byte * plain, int n_block) ;
    
  void set_IV(unsigned long long int IVCl);
    
  void iv_inc();
    
  int get_size();
  
  void set_size(int sizel);
  
  void get_IV(byte *out);
    
  void calc_size_n_pad(int p_size);
  
  void padPlaintext(void* in,byte* out);
    
  bool CheckPad(byte* in,int size);

  void printArray(byte output[],bool p_pad = true);
  
  void printArray(byte output[],int sizel);
  
  void do_aes_encrypt(byte *plain,int size_p,byte *cipher,byte *key, int bits, byte ivl [N_BLOCK]);
  
  void do_aes_encrypt(byte *plain,int size_p,byte *cipher,byte *key, int bits);
  
  void do_aes_decrypt(byte *cipher,int size_c,byte *plain,byte *key, int bits, byte ivl [N_BLOCK]);
  
  void do_aes_decrypt(byte *cipher,int size_c,byte *plain,byte *key, int bits);

  #if defined(AES_LINUX)

    double millis();
  #endif
 private:
  int round ;
  byte key_sched [KEY_SCHEDULE_BYTES] ;
  unsigned long long int IVC;
  byte iv[16];/**< holds the initialization vector that will be used in the cipher. */
  int pad;
  int size;/**< hold the size of the plaintext to be ciphered */
  #if defined(AES_LINUX)
  timeval tv;
  byte arr_pad[15];
  #else
  byte arr_pad[15] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };/**< holds the hexadecimal padding values */
  #endif
} ;


#endif
