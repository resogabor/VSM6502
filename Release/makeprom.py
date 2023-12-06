
# Please see this video for details:
# https://www.youtube.com/watch?v=yl8vPW5hydQ
#
#  code = bytearray([
#  0xa9, 0xff,         # lda #$ff
#  0x8d, 0x02, 0x60,   # sta $6002
 
#  0xa9, 0x55,         # lda #$55
#  0x8d, 0x00, 0x60,   # sta $6000
 
#  0xa9, 0xaa,         # lda #$aa
#  0x8d, 0x00, 0x60,   # sta $6000
  
#  0x4c, 0x05, 0x80,   # jmp $8005
#  ])
 
rom = bytearray([0xea] * 32768)
rom[0] = 0xa9
rom[1] = 0x42

rom[2] = 0x8d
rom[3] = 0x00
rom[4] = 0x60
 
rom[0x7ffc] = 0x00
rom[0x7ffd] = 0x80
 
with open("test.bin", "wb") as out_file:
  out_file.write(rom)