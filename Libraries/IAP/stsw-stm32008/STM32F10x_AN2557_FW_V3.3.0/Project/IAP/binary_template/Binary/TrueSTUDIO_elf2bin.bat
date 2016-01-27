@echo off

if exist .\..\TrueSTUDIO\STM32100E-EVAL\Debug\STM32100E-EVAL.elf ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM32100E-EVAL\Debug\STM32100E-EVAL.elf" ".\..\TrueSTUDIO\STM32100E-EVAL\Debug\STM32100E-EVAL_SysTick.bin")
if exist .\..\TrueSTUDIO\STM32100B-EVAL\Debug\STM32100B-EVAL.elf ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM32100B-EVAL\Debug\STM32100B-EVAL.elf" ".\..\TrueSTUDIO\STM32100B-EVAL\Debug\STM32100B-EVAL_SysTick.bin")
if exist .\..\TrueSTUDIO\STM3210C-EVAL\Debug\STM3210C-EVAL.elf  ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM3210C-EVAL\Debug\STM3210C-EVAL.elf" ".\..\TrueSTUDIO\STM3210C-EVAL\Debug\STM3210C-EVAL_SysTick.bin")
if exist .\..\TrueSTUDIO\STM3210B-EVAL\Debug\STM3210B-EVAL.elf  ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM3210B-EVAL\Debug\STM3210B-EVAL.elf" ".\..\TrueSTUDIO\STM3210B-EVAL\Debug\STM3210B-EVAL_SysTick.bin")
if exist .\..\TrueSTUDIO\STM3210E-EVAL\Debug\STM3210E-EVAL.elf  ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM3210E-EVAL\Debug\STM3210E-EVAL.elf" ".\..\TrueSTUDIO\STM3210E-EVAL\Debug\STM3210E-EVAL_SysTick.bin")
if exist .\..\TrueSTUDIO\STM3210E-EVAL_XL\Debug\STM3210E-EVAL_XL.elf    ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM3210E-EVAL_XL\Debug\STM3210E-EVAL_XL.elf" ".\..\TrueSTUDIO\STM3210E-EVAL_XL\Debug\STM3210E-EVAL_XL_SysTick.bin")

pause

