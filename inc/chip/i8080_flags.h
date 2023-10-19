#pragma once

// Position of flag bits in PSW when pushed and popped

#define FLAGS_CY	0x01
//                      0x02: ignored during POP, write 1 during PUSH.
#define FLAGS_P		0x04
//                      0x08: ignored during POP, write 0 during PUSH.
#define FLAGS_AC	0x10
//                      0x20: ignored during POP, write 0 during PUSH.
#define FLAGS_Z		0x40
#define FLAGS_S		0x80
