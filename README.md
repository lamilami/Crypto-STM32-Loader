# Crypto-STM32-Loader
This is part of a project for encrypted bootloader of STM32 based platforms
A: decryption module is loaded in the RAM area of the ARM processor.
B: (Manufacturing - encrypted firmware) is decrypted in target MCU RAM, this makes it extremely difficult breaking the RSA keys.
