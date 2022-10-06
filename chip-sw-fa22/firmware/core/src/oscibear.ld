ENTRY(_start)

MEMORY {
  SRAM(rwx): ORIGIN = 0x80000000, LENGTH = 24K
}

SECTIONS {
  .text : {
    . = ALIGN(4);
    KEEP(*(.text.reset))          /* Reset routine */
    
    . = ALIGN(4);
    KEEP(*(.text.isr_routine))    /* ISR routine */
    *(.text.init)                 /* Startup code */

    *(.text .text*)               /* .text sections (code) */

    KEEP (*(.init))
    KEEP (*(.fini))
    
    _etext = .;                   /* define a global symbols at end of code */
  }> SRAM
  
  .rodata : {
    . = ALIGN(4);
    *(.rodata .rodata*)           /* .rodata sections (constants, strings, etc.) */
    . = ALIGN(4);
  }> SRAM

  /* data segment */
  .data : {
    *(.data .data*)
  }> SRAM

  .sdata : {
    __global_pointer$ = . + 0x800;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
    *(.rodata)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    . = ALIGN(4);
  }> SRAM
  
  /* bss segment */
  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  }> SRAM

  .bss (NOLOAD) : {
    *(.bss)
  }> SRAM

  /* thread-local data segment */
  .tdata : {
    _tdata_begin = .;
    *(.tdata)
    _tdata_end = .;
  }> SRAM
  
  .tbss : {
    *(.tbss)
    _tbss_end = .;
  }> SRAM

  /* End of uninitalized data segement */
  _end = .;

}
