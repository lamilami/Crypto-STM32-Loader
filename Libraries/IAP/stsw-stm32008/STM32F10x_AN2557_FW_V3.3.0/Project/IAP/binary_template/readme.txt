/**
  @page binary_template AN2557 Binary Template Readme file
  
  @verbatim
  ******************** (C) COPYRIGHT 2010 STMicroelectronics *******************
  * @file    IAP/binary_template/readme.txt 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   Description of the binary_template directory.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Description

This directory contains a set of sources files that build the application to be
loaded into Flash memory using In-Application Programming (IAP, through USART).

To build such application, some special configuration has to be peformed:
1. Set the program load address at 0x08003000, using your toolchain linker file.
2. Relocate the vector table at address 0x08003000, using 
   the "NVIC_SetVectorTable" function.
 
The SysTick example provided within the STM32F10x Standard Peripherals library 
examples is used as illustration.
This example configures the SysTick to generate a time base equal to 1 ms.
The system clock is set to the maximum system frequency, the SysTick is clocked by 
the AHB clock (HCLK). A "Delay" function is implemented based on the SysTick 
end-of-count event.
Four LEDs are toggled with a timing defined by the Delay function.


@par Directory contents 

 - "IAP\binary_template\EWARM": This folder contains a pre-configured project file 
                              that produces a binary image of SysTick example to 
                              be loaded with IAP.

 - "IAP\binary_template\RIDE": This folder contains a pre-configured project file 
                              that produces a binary image of SysTick example to be 
                              loaded with IAP.

 - "IAP\binary_template\MDK-ARM": This folder contains a pre-configured project file 
                              that produces a binary image of SysTick example to be 
                              loaded with IAP.

- "IAP\binary_template\HiTOP": This folder contains a pre-configured project file 
                              that produces a binary image of SysTick example to be 
                              loaded with IAP.

 - "IAP\binary_template\TrueSTUDIO": This folder contains a pre-configured project file 
                              that produces a binary image of SysTick example to be 
                              loaded with IAP.
                           
 - "IAP\binary_template\inc": contains the binary_template firmware header files 
     - IAP/binary_template/inc/stm32f10x_conf.h    Library Configuration file
     - IAP/binary_template/inc/stm32f10x_it.h      Header for stm32f10x_it.c
     - IAP/binary_template/inc/main.h              Header for main.c
     
 - "IAP\binary_template\src": contains the binary_template firmware source files 
     - IAP/binary_template/src/main.c              Main program
     - IAP/binary_template/src/stm32f10x_it.c      Interrupt handlers
     - IAP/binary_template/src/system_stm32f10x.c  STM32F10x system source file

@par Hardware and Software environment 

  - This example runs on STM32F10x Connectivity line, High-Density, High-Density
    Value line, Medium-Density, XL-Density, Medium-Density Value line, Low-Density 
    and Low-Density Value line Devices.
  
  - This example has been tested with STMicroelectronics STM32100E-EVAL (High-Density
    Value line),  STM32100B-EVAL (Medium-Density Value line), STM3210C-EVAL (Connectivity line), 
    STM3210E-EVAL (High-Density and XL-Density), STM3210B-EVAL (Medium-Density)
    and STM3210E-EVAL (High-Density Value line) evaluation boards and can be easily 
    tailored to any other supported device and development board.    
    To select the STMicroelectronics evaluation board used to run the example, 
    uncomment the corresponding line in stm32_eval.h file (under Utilities\STM32_EVAL)
    
  - STM32100E-EVAL Set-up 
    - Use LED1, LED2, LED3 and LED4 connected respectively to PF.06, PF0.7, PF.08
      and PF.09 pins
      
  - STM32100B-EVAL Set-up  
    - Use LED1, LED2, LED3 and LED4 connected respectively to PC.06, PC.07, PC.08
      and PC.09 pins
     
  - STM3210C-EVAL Set-up 
    - Use LED1, LED2, LED3 and LED4 connected respectively to PD.07, PD.13, PF.03
      and PD.04 pins
    
  - STM3210E-EVAL Set-up 
    - Use LED1, LED2, LED3 and LED4 connected respectively to PF.06, PF0.7, PF.08
      and PF.09 pins

  - STM3210B-EVAL Set-up  
    - Use LED1, LED2, LED3 and LED4 connected respectively to PC.06, PC.07, PC.08
      and PC.09 pins
      
@par How to use it ?  

In order to load the SysTick example with the IAP, you must do the following:

 - EWARM (v5.50 and later):
    - Open the SysTick.eww workspace
    - In the workspace toolbar select the project config:
        - STM32100E-EVAL: to configure the project for STM32 High-density Value line devices
        - STM3210E-EVAL_XL: to configure the project for STM32 XL-density devices
        - STM32100B-EVAL: to configure the project for STM32 Medium-Density Value line devices
        - STM3210C-EVAL: to configure the project for STM32 Connectivity line devices
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all
    - A binary file "SysTick.bin" will be generated under "STM3210E-EVAL_XL\Exe",
      "STM32100B-EVAL\Exe", "STM3210C-EVAL\Exe", "STM3210E-EVAL\Exe", "STM3210B-EVAL\Exe"  
      or "STM32100E-EVAL\Exe" folder depending on the selected configuration.  
    - Finally load this image with IAP application

 - RIDE:
    - Open the SysTick.rprj project
    - In the configuration toolbar(Project->properties) select the project config:
        - STM32100E-EVAL: to configure the project for STM32 High-density Value line devices
        - STM3210E-EVAL_XL: to configure the project for STM32 XL-density devices
        - STM32100B-EVAL: to configure the project for STM32 Medium-Density Value line devices
        - STM3210C-EVAL: to configure the project for STM32 Connectivity line devices
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->build project
    - Go to "Utilities\Binary" directory and run "hextobin.bat"
    - A binary file "SysTick.bin" will be generated under "\STM3210E-EVAL_XL", "\STM32100B-EVAL", 
      "\STM32100B-EVAL", "\STM3210C-EVAL", \STM3210E-EVAL", "\STM3210B-EVAL" or "\STM32100E-EVAL" 
      folder depending on the selected configuration. 
    - Finally load this image with IAP application

 - MDK-ARM:
    - Open the SysTick.uvproj project
    - In the build toolbar select the project config:
        - STM32100E-EVAL: to configure the project for STM32 High-density Value line devices
        - STM3210E-EVAL_XL: to configure the project for STM32 XL-density devices
        - STM32100B-EVAL: to configure the project for STM32 Medium-Density Value line devices
        - STM3210C-EVAL: to configure the project for STM32 Connectivity line devices
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all target files
    - Go to "Utilities\Binary" directory and run "axftobin.bat"
    - A binary file "STM3210E-EVAL_XL_SysTick.bin", "STM32100B-EVAL_SysTick.bin", 
      "STM3210C-EVAL_SysTick.bin", "STM3210B-EVAL_SysTick.bin", "STM3210E-EVAL_SysTick.bin" 
      or "STM32100E-EVAL_SysTick.bin" will be generated under "\STM3210E-EVAL_XL", 
      "\STM32100B-EVAL", "\STM32100B-EVAL", "\STM3210C-EVAL", \STM3210E-EVAL", 
      "\STM3210B-EVAL" or "\STM32100E-EVAL" folder depending on the selected configuration. 
    - Finally load this image with IAP application

- HiTOP
    - Open the HiTOP toolchain.
    - Browse to open the Project.htp
        - STM32100E-EVAL: to configure the project for STM32 High-density Value line devices
        - STM3210E-EVAL_XL: to configure the project for STM32 XL-density devices
        - STM32100B-EVAL: to configure the project for STM32 Medium-Density Value line devices    
        - STM3210C-EVAL: to configure the project for STM32 Connectivity line devices
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - A "Download application" window is displayed, click "cancel".
    - Rebuild all files: Project->Rebuild all
    - Go to "Utilities\Binary" directory and run "hextobin.bat".
    - A binary file "STM3210E-EVAL_XL_SysTick.bin", "STM32100B-EVAL_SysTick.bin", 
      "Project.bin" will be generated under "\." folder
    - Finally load this image with IAP application
    
 - TrueSTUDIO:
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory, select the project:
        - STM32100E-EVAL: to configure the project for STM32 High-density Value line devices
        - STM3210E-EVAL_XL: to load the project for STM32 XL-density devices
        - STM32100B-EVAL: to load the project for STM32 Medium-Density Value line devices    
        - STM3210C-EVAL: to load the project for STM32 Connectivity line devices
        - STM3210B-EVAL: to load the project for STM32 Medium-density devices
        - STM3210E-EVAL: to load the project for STM32 High-density devices
    - Under Windows->Preferences->General->Workspace->Linked Resources, add 
      a variable path named "CurPath" which points to the folder containing
      "Libraries", "Project" and "Utilities" folders.
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Go to "Utilities\Binary" directory and run "TrueSTUDIO_elf2bin.bat"
    - A binary file "STM3210E-EVAL_XL_SysTick.bin", "STM32100B-EVAL_SysTick.bin",
      "STM3210C-EVAL_SysTick.bin", "STM3210B-EVAL_SysTick.bin", "STM3210E-EVAL_SysTick.bin" 
      or "STM32100E-EVAL_SysTick.bin" will be generated under "\Debug" folder
    - Finally load this image with IAP application

@note
 - Low-density Value line devices are STM32F100xx microcontrollers where the 
   Flash memory density ranges between 16 and 32 Kbytes.
 - Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 16 and 32 Kbytes.
 - Medium-density Value line devices are STM32F100xx microcontrollers where
   the Flash memory density ranges between 64 and 128 Kbytes.  
 - Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
 - High-density Value line devices are STM32F101xx and STM32F100xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.
 - XL-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 512 and 1024 Kbytes.
 - Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers.

 * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
 */
