#include "xcustom.h"
#include <stdint.h>
#include <stdio.h>

#define AES_BLOCKLEN 16
#define AES_OPC 0

#define aes_extended_keysetup128(key_address) \
    ROCC_INSTRUCTION_0_R_R(AES_OPC, key_address, 0, 0)

#define aes_extended_keysetup256(key_address) \
    ROCC_INSTRUCTION_0_R_R(AES_OPC, key_address, 0, 1)

#define aes_extended_addressload(src_address, dest_address) \
    ROCC_INSTRUCTION_0_R_R(AES_OPC, src_address, dest_address, 2)

#define aes_extended_encryptblocks(num_blocks, interrupt_enable) \
    ROCC_INSTRUCTION_0_R_R(AES_OPC, num_blocks, interrupt_enable, 3)

#define aes_extended_decryptblocks(num_blocks, interrupt_enable) \
    ROCC_INSTRUCTION_0_R_R(AES_OPC, num_blocks, interrupt_enable, 4)

#define aes_extended_querystatus(status) \
    ROCC_INSTRUCTION_R_R_R(AES_OPC, status, 0, 0, 5)

#define aes_fence() \
    asm volatile("fence")

void AES_ECB_ENC_POLL(uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  aes_extended_addressload(src, dest);
  aes_extended_encryptblocks((int) (length / AES_BLOCKLEN), 0);
  int status = 1;
  while (status) {
      aes_extended_querystatus(status);
  }
}

void AES_ECB_DEC_POLL(uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  aes_extended_addressload(src, dest);
  aes_extended_decryptblocks((int) (length / AES_BLOCKLEN), 0);
  int status = 1;
  while (status) {
    aes_extended_querystatus(status);
  }
}

void AES_ECB_NO_KEY_ENC_POLL(uint8_t *src, uint8_t *dest, int length) {
  aes_extended_addressload(src, dest);
  aes_extended_encryptblocks((int) (length / AES_BLOCKLEN), 0);
  int status = 1;
  while (status) {
      aes_extended_querystatus(status);
  }
}

void AES_ECB_NO_KEY_DEC_POLL(uint8_t *src, uint8_t *dest, int length) {
  aes_extended_addressload(src, dest);
  aes_extended_decryptblocks((int) (length / AES_BLOCKLEN), 0);
  int status = 1;
  while (status) {
    aes_extended_querystatus(status);
  }
}

void AES_ECB_ENC_INT(uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  aes_extended_addressload(src, dest);
  aes_extended_encryptblocks((int) (length / AES_BLOCKLEN), 1);
}

void AES_ECB_DEC_INT(uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  aes_extended_addressload(src, dest);
  aes_extended_decryptblocks((int) (length / AES_BLOCKLEN), 1);
}

static void XOR_IV(uint8_t *src, uint8_t *dest, uint8_t *IV) {
  for (int i = 0; i < AES_BLOCKLEN; ++i) {
    dest[i] = src[i] ^ IV[i];
  }
}

void AES_CBC_ENC_POLL(uint8_t *IV, uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  for (int i = 0; i < length; i += AES_BLOCKLEN) {
    XOR_IV(src, dest, IV);
    aes_extended_addressload(dest, dest);
    aes_extended_encryptblocks(1, 0);
    int status = 1;
    while (status) {
      aes_extended_querystatus(status);
    }
    IV = dest;
    src += AES_BLOCKLEN;
    dest += AES_BLOCKLEN;
  }
}

void AES_CBC_DEC_POLL(uint8_t *IV, uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  if (keylen == 128) {
    aes_extended_keysetup128(key);
  } else {
    aes_extended_keysetup256(key);
  }
  for (int i = 0; i < length; i += AES_BLOCKLEN) {
    aes_extended_addressload(src, dest);
    aes_extended_decryptblocks(1, 0);
    int status = 1;
    while (status) {
      aes_extended_querystatus(status);
    }
    XOR_IV(dest, dest, IV);
    IV = src;
    src += AES_BLOCKLEN;
    dest += AES_BLOCKLEN;
  }
}

static void cpy(uint8_t *src, uint8_t *dest, int length) {
  for (int i = 0; i < length; ++i) {
    dest[i] = src[i];
  }
}

void AES_CTR_XCRYPT_POLL(uint8_t *IV, uint8_t *key, int keylen, uint8_t *src, uint8_t *dest, int length) {
  uint8_t buffer[AES_BLOCKLEN];
  cpy(IV, buffer, AES_BLOCKLEN);

  for (int i = 0, bi = AES_BLOCKLEN; i < length; ++i, ++bi)
  {
    if (bi == AES_BLOCKLEN)
    {
      cpy(buffer, dest + i, AES_BLOCKLEN);

      /* Increment Iv and handle overflow */
      for (bi = (AES_BLOCKLEN - 1); bi >= 0; --bi)
      {
        /* inc will overflow */
        if (buffer[bi] == 255)
        {
          buffer[bi] = 0;
          continue;
        }
        buffer[bi] += 1;
        break;
      }
      bi = 0;
    }
  }
  // note: worth checking if interweaving encryption (w/interrupts?) and xor'ing is faster
  AES_ECB_ENC_POLL(key, keylen, dest, dest, length);
  for (int i = 0; i < length; ++i)
  {
    dest[i] ^= src[i];
  } 
}

void printBytes(uint8_t* block, int bytesCount) {
  for (int i = 0; i < bytesCount; i++) {
    printf("%.2x ", (unsigned)*(unsigned char*)(block + i));
  }
  printf("\n");
}

void checkBytes(uint8_t* real, uint8_t* exp, int lenBytes) {
  int fail = 0;
  int mismatch = -1;
  for (int i = 0; i < lenBytes; i++) {
    // Checking match
    if ((unsigned)*(unsigned char*)(real + i) != (unsigned)*(unsigned char*)(exp + i)) {
      fail += 1;
      if (mismatch == -1) mismatch = i;
    }
  }
  if (fail) {
    printf("FAILED TEST. %d bytes are mismatched in output. Index of first mismatch: %d\n", fail, mismatch);
    printf("Expected output: ");
    printBytes(exp, lenBytes);
    printf("Actual output: ");
    printBytes(real, lenBytes);
  } else {
    printf("PASSED TEST. All real bytes match expected bytes. Bytes checked: %d\n", lenBytes);
  }
}
